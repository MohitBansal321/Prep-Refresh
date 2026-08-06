/**
 * FACADE PATTERN — Production-style TypeScript example
 * ----------------------------------------------------
 * Scenario: An e-commerce backend must "place an order". Placing an order is NOT
 * one operation — it is an orchestration of FOUR independent subsystems:
 *
 *   1. Inventory   -> reserve stock so two customers can't buy the last unit.
 *   2. Payment     -> charge the customer's card.
 *   3. Shipping    -> create a shipment / fetch a tracking number.
 *   4. Notification-> email/SMS the customer a confirmation.
 *
 * Each subsystem is a real collaborator with its own API, its own error modes,
 * and its own ordering rules (you must reserve stock BEFORE you charge, and you
 * must release the reservation if the charge fails). If every controller that
 * needs to place an order had to know all of this, the knowledge would be
 * copy-pasted across the codebase and drift out of sync.
 *
 * We solve this with the Facade Pattern:
 *   - OrderCheckoutFacade -> Facade (the single high-level entry point)
 *   - InventoryService / PaymentService / ShippingService / NotificationService
 *       -> Subsystem classes (the complex collaborators the facade coordinates)
 *   - OrderController      -> Client (calls ONE method, knows nothing about the dance)
 *
 * KEY POINT: The facade adds NO new business capability. Every line of real work
 * already lives in a subsystem. The facade only sequences those calls, handles the
 * cross-subsystem ordering/rollback, and exposes one simple method. Advanced code
 * is still free to call the subsystems directly (the facade does not seal them off).
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 0. SHARED DOMAIN TYPES
// =============================================================================

export interface OrderLine {
  sku: string; // stock-keeping unit, e.g. "TSHIRT-BLK-M"
  quantity: number;
}

export interface PlaceOrderRequest {
  orderId: string;
  customerId: string;
  email: string;
  lines: OrderLine[];
  amountInCents: number;
  currency: string;
  cardToken: string;
  shippingAddress: string;
}

export interface PlaceOrderResult {
  orderId: string;
  status: "CONFIRMED";
  transactionId: string;
  trackingNumber: string;
}

/**
 * A single domain-level error the client can reason about. The facade translates
 * each subsystem's low-level failure into this so the client never has to catch
 * `InsufficientStockError`, `CardDeclinedError`, `CarrierUnavailableError`, etc.
 */
export class CheckoutError extends Error {
  constructor(
    message: string,
    public readonly stage: "INVENTORY" | "PAYMENT" | "SHIPPING" | "NOTIFICATION",
    public readonly cause?: unknown,
  ) {
    super(message);
    this.name = "CheckoutError";
  }
}

// =============================================================================
// 1. SUBSYSTEM CLASSES — the complex collaborators.
//    Each does REAL work and has its own quirky, low-level API. In production
//    these would talk to Postgres, Redis, Stripe, a carrier API, an email
//    provider, etc. They are independent and know NOTHING about each other.
// =============================================================================

/** Reserves and releases stock. Backed by Postgres + Redis in real life. */
export class InventoryService {
  async reserve(lines: OrderLine[]): Promise<{ reservationId: string }> {
    for (const line of lines) {
      if (line.quantity <= 0) {
        throw new Error(`Invalid quantity for ${line.sku}`);
      }
    }
    // Pretend: SELECT ... FOR UPDATE, decrement available, write a reservation row.
    console.log(`[Inventory] Reserved ${lines.length} line(s).`);
    return { reservationId: `rsv_${Math.random().toString(36).slice(2, 8)}` };
  }

  async release(reservationId: string): Promise<void> {
    // Pretend: mark reservation row as released, increment available stock.
    console.log(`[Inventory] Released reservation ${reservationId}.`);
  }
}

/** Charges and refunds cards. Wraps a payment provider SDK in real life. */
export class PaymentService {
  async charge(
    amountInCents: number,
    currency: string,
    cardToken: string,
  ): Promise<{ transactionId: string }> {
    if (amountInCents <= 0) {
      throw new Error("Amount must be positive");
    }
    // Pretend: call Stripe/Razorpay, await webhook, etc.
    console.log(`[Payment] Charged ${amountInCents} ${currency}.`);
    return { transactionId: `txn_${cardToken.slice(-4)}` };
  }

  async refund(transactionId: string): Promise<void> {
    console.log(`[Payment] Refunded ${transactionId}.`);
  }
}

/** Creates shipments and returns tracking numbers. Wraps a carrier API. */
export class ShippingService {
  async createShipment(
    orderId: string,
    address: string,
  ): Promise<{ trackingNumber: string }> {
    if (!address || address.trim().length === 0) {
      throw new Error("Shipping address is required");
    }
    // Pretend: POST to FedEx/DHL, get a label + tracking id.
    console.log(`[Shipping] Shipment created for ${orderId} to "${address}".`);
    return { trackingNumber: `trk_${orderId}` };
  }
}

/** Sends transactional notifications. Wraps SES/Twilio/FCM. */
export class NotificationService {
  async sendOrderConfirmation(
    email: string,
    orderId: string,
    trackingNumber: string,
  ): Promise<void> {
    // Pretend: enqueue an email job. Failing here must NOT fail the order.
    console.log(
      `[Notification] Sent confirmation to ${email} ` +
        `(order ${orderId}, tracking ${trackingNumber}).`,
    );
  }
}

// =============================================================================
// 2. FACADE — the single, simplified, high-level entry point.
//    It OWNS the cross-subsystem workflow knowledge: the correct order of calls,
//    the compensating rollback when a later step fails, and which failures are
//    fatal vs. best-effort. It adds no new business rule of its own.
//
//    Subsystems are INJECTED (constructor injection) — the facade neither creates
//    nor configures them. This keeps it testable (pass fakes) and honours the
//    Dependency Inversion Principle.
// =============================================================================

export class OrderCheckoutFacade {
  constructor(
    private readonly inventory: InventoryService,
    private readonly payment: PaymentService,
    private readonly shipping: ShippingService,
    private readonly notification: NotificationService,
  ) {}

  /**
   * The ONE method the client calls. Behind it: reserve -> charge -> ship ->
   * notify, with correct ordering and rollback (release stock / refund) if a
   * later step fails. All of this complexity is hidden here, not at every caller.
   */
  async placeOrder(req: PlaceOrderRequest): Promise<PlaceOrderResult> {
    // --- Step 1: reserve stock FIRST (never charge for stock you don't have) ---
    let reservationId: string;
    try {
      ({ reservationId } = await this.inventory.reserve(req.lines));
    } catch (err) {
      throw new CheckoutError("Could not reserve stock", "INVENTORY", err);
    }

    // --- Step 2: charge the card. On failure, release the reservation. ---
    let transactionId: string;
    try {
      ({ transactionId } = await this.payment.charge(
        req.amountInCents,
        req.currency,
        req.cardToken,
      ));
    } catch (err) {
      await this.safeRelease(reservationId);
      throw new CheckoutError("Payment failed", "PAYMENT", err);
    }

    // --- Step 3: create the shipment. On failure, refund AND release. ---
    let trackingNumber: string;
    try {
      ({ trackingNumber } = await this.shipping.createShipment(
        req.orderId,
        req.shippingAddress,
      ));
    } catch (err) {
      await this.safeRefund(transactionId);
      await this.safeRelease(reservationId);
      throw new CheckoutError("Shipment creation failed", "SHIPPING", err);
    }

    // --- Step 4: notify the customer. BEST-EFFORT: never fail an order because
    // the confirmation email bounced. The order is already paid and shipped. ---
    try {
      await this.notification.sendOrderConfirmation(
        req.email,
        req.orderId,
        trackingNumber,
      );
    } catch (err) {
      // Log and move on — do NOT roll back a completed order over an email.
      console.warn(
        `[Checkout] Confirmation failed for ${req.orderId}; order still CONFIRMED.`,
        err,
      );
    }

    return {
      orderId: req.orderId,
      status: "CONFIRMED",
      transactionId,
      trackingNumber,
    };
  }

  // ---- Private compensating actions. The facade owns rollback knowledge. ----

  private async safeRelease(reservationId: string): Promise<void> {
    try {
      await this.inventory.release(reservationId);
    } catch (err) {
      // A failed rollback is an alert-worthy event but must not mask the original error.
      console.error(`[Checkout] Failed to release ${reservationId}`, err);
    }
  }

  private async safeRefund(transactionId: string): Promise<void> {
    try {
      await this.payment.refund(transactionId);
    } catch (err) {
      console.error(`[Checkout] Failed to refund ${transactionId}`, err);
    }
  }
}

// =============================================================================
// 3. CLIENT — knows only the facade. One call, one result, one error type.
//    Notice how thin and readable this is: no ordering, no rollback, no vendor.
// =============================================================================

export class OrderController {
  constructor(private readonly checkout: OrderCheckoutFacade) {}

  async handlePlaceOrder(req: PlaceOrderRequest): Promise<PlaceOrderResult> {
    // The entire subsystem dance is a single line to the client.
    const result = await this.checkout.placeOrder(req);
    console.log(
      `[Controller] Order ${result.orderId} ${result.status} ` +
        `(txn=${result.transactionId}, tracking=${result.trackingNumber}).`,
    );
    return result;
  }
}

// =============================================================================
// 4. COMPOSITION ROOT — wire the subsystems into the facade, facade into client.
//    In NestJS this is exactly what the DI container does for you via providers.
// =============================================================================

export function buildOrderController(): OrderController {
  const facade = new OrderCheckoutFacade(
    new InventoryService(),
    new PaymentService(),
    new ShippingService(),
    new NotificationService(),
  );
  return new OrderController(facade);
}

// =============================================================================
// 5. DEMO — happy path plus a failure that triggers compensating rollback.
// =============================================================================

async function main(): Promise<void> {
  const controller = buildOrderController();

  console.log("=== SCENARIO 1: successful checkout ===");
  await controller.handlePlaceOrder({
    orderId: "ORD-1001",
    customerId: "CUST-7",
    email: "buyer@example.com",
    lines: [{ sku: "TSHIRT-BLK-M", quantity: 2 }],
    amountInCents: 4000,
    currency: "USD",
    cardToken: "tok_visa_4242",
    shippingAddress: "221B Baker Street, London",
  });

  console.log("\n=== SCENARIO 2: shipping fails -> refund + release stock ===");
  try {
    await controller.handlePlaceOrder({
      orderId: "ORD-1002",
      customerId: "CUST-9",
      email: "buyer2@example.com",
      lines: [{ sku: "MUG-WHT", quantity: 1 }],
      amountInCents: 1500,
      currency: "USD",
      cardToken: "tok_visa_1111",
      shippingAddress: "", // invalid -> ShippingService throws
    });
  } catch (err) {
    if (err instanceof CheckoutError) {
      console.log(`[Controller] Checkout failed at ${err.stage}: ${err.message}`);
    } else {
      throw err;
    }
  }
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
