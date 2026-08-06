/**
 * ADAPTER PATTERN — Production-style TypeScript example
 * ----------------------------------------------------
 * Scenario: Our application must accept payments through multiple providers
 * (Stripe, Razorpay). Each provider ships an SDK with a completely different
 * interface and data shape. We do NOT want our business logic to know which
 * provider is in use.
 *
 * We solve this with the Adapter Pattern:
 *   - PaymentGateway  -> Target interface (OUR contract)
 *   - PaymentService  -> Client (business logic, depends only on the Target)
 *   - StripeSDK / RazorpaySDK -> Adaptees (incompatible third-party code)
 *   - StripeAdapter / RazorpayAdapter -> Adapters (translate Target <-> Adaptee)
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. TARGET INTERFACE — the contract our application designed and controls.
//    Notice: vendor-neutral names, our own domain types, amounts in cents.
// =============================================================================

/** Money is always represented internally in the smallest currency unit (cents/paise). */
export interface PaymentResult {
  transactionId: string;
  success: boolean;
  provider: string;
  amountInCents: number;
  currency: string;
}

export interface RefundResult {
  refundId: string;
  success: boolean;
}

/** Our own domain error. Vendor-specific errors are translated into this. */
export class PaymentError extends Error {
  constructor(
    message: string,
    public readonly provider: string,
    public readonly cause?: unknown,
  ) {
    super(message);
    this.name = "PaymentError";
  }
}

/**
 * The Target. Every payment provider, no matter how different, is exposed to
 * the rest of the app through THIS interface.
 */
export interface PaymentGateway {
  pay(amountInCents: number, currency: string, cardToken: string): Promise<PaymentResult>;
  refund(transactionId: string): Promise<RefundResult>;
}

// =============================================================================
// 2. CLIENT — business logic. Depends ONLY on PaymentGateway (the Target).
//    It has no idea Stripe or Razorpay exist.
// =============================================================================

export interface Order {
  id: string;
  totalInCents: number;
  currency: string;
  cardToken: string;
}

export class PaymentService {
  // Injected as the Target interface — never a concrete adapter type.
  constructor(private readonly gateway: PaymentGateway) {}

  async checkout(order: Order): Promise<void> {
    if (order.totalInCents <= 0) {
      throw new Error("Order total must be positive");
    }

    // Pure business logic. No vendor details anywhere.
    const result = await this.gateway.pay(
      order.totalInCents,
      order.currency,
      order.cardToken,
    );

    if (!result.success) {
      throw new Error(`Payment failed for order ${order.id}`);
    }

    console.log(
      `[PaymentService] Order ${order.id} paid via ${result.provider}. ` +
        `Txn=${result.transactionId}`,
    );
  }
}

// =============================================================================
// 3. ADAPTEES — simulated third-party SDKs. We cannot change these.
//    Observe how different their interfaces and return shapes are.
// =============================================================================

/** Fake Stripe SDK: object-based API, amount in cents, returns { id, status }. */
export class StripeSDK {
  charges = {
    create: async (params: {
      amount: number;
      currency: string; // lowercase ISO, e.g. "usd"
      source: string;
    }): Promise<{ id: string; status: "succeeded" | "failed" }> => {
      // Pretend network call to Stripe.
      return { id: `ch_${params.source.slice(-4)}`, status: "succeeded" };
    },
  };

  refunds = {
    create: async (params: {
      charge: string;
    }): Promise<{ id: string; status: "succeeded" | "failed" }> => {
      return { id: `re_${params.charge.slice(-4)}`, status: "succeeded" };
    },
  };
}

/** Fake Razorpay SDK: method-based API, amount in paise, returns { payment_id, captured }. */
export class RazorpaySDK {
  async createPayment(
    paise: number,
    currency: string, // uppercase, e.g. "INR"
    token: string,
  ): Promise<{ payment_id: string; captured: boolean }> {
    return { payment_id: `pay_${token.slice(-4)}`, captured: true };
  }

  async issueRefund(paymentId: string): Promise<{ refund_id: string; processed: boolean }> {
    return { refund_id: `rfnd_${paymentId.slice(-4)}`, processed: true };
  }
}

// =============================================================================
// 4. ADAPTERS — implement the Target, wrap an Adaptee, translate both ways.
//    All the "how this vendor actually works" knowledge is confined HERE.
// =============================================================================

export class StripeAdapter implements PaymentGateway {
  private static readonly PROVIDER = "stripe";

  // Adaptee injected via constructor => testable, configurable.
  constructor(private readonly stripe: StripeSDK) {}

  async pay(
    amountInCents: number,
    currency: string,
    cardToken: string,
  ): Promise<PaymentResult> {
    try {
      // Translate OUR args into Stripe's expected shape.
      const charge = await this.stripe.charges.create({
        amount: amountInCents, // Stripe also uses cents — no conversion needed
        currency: currency.toLowerCase(), // Stripe wants lowercase
        source: cardToken,
      });

      // Translate Stripe's result back into OUR PaymentResult.
      return {
        transactionId: charge.id,
        success: charge.status === "succeeded",
        provider: StripeAdapter.PROVIDER,
        amountInCents,
        currency,
      };
    } catch (err) {
      // Translate vendor error into our domain error.
      throw new PaymentError("Stripe charge failed", StripeAdapter.PROVIDER, err);
    }
  }

  async refund(transactionId: string): Promise<RefundResult> {
    const refund = await this.stripe.refunds.create({ charge: transactionId });
    return { refundId: refund.id, success: refund.status === "succeeded" };
  }
}

export class RazorpayAdapter implements PaymentGateway {
  private static readonly PROVIDER = "razorpay";

  constructor(private readonly razorpay: RazorpaySDK) {}

  async pay(
    amountInCents: number,
    currency: string,
    cardToken: string,
  ): Promise<PaymentResult> {
    try {
      // Razorpay uses paise for INR — but cents and paise are both 1/100,
      // so the numeric value matches. We keep the conversion explicit to show
      // where unit-translation belongs: inside the adapter.
      const paise = amountInCents;

      const payment = await this.razorpay.createPayment(
        paise,
        currency.toUpperCase(), // Razorpay wants uppercase
        cardToken,
      );

      return {
        transactionId: payment.payment_id,
        success: payment.captured,
        provider: RazorpayAdapter.PROVIDER,
        amountInCents,
        currency,
      };
    } catch (err) {
      throw new PaymentError("Razorpay payment failed", RazorpayAdapter.PROVIDER, err);
    }
  }

  async refund(transactionId: string): Promise<RefundResult> {
    const refund = await this.razorpay.issueRefund(transactionId);
    return { refundId: refund.refund_id, success: refund.processed };
  }
}

// =============================================================================
// 5. COMPOSITION ROOT — wire it up. Switching vendors is a ONE-LINE change.
// =============================================================================

type ProviderName = "stripe" | "razorpay";

function buildGateway(provider: ProviderName): PaymentGateway {
  switch (provider) {
    case "stripe":
      return new StripeAdapter(new StripeSDK());
    case "razorpay":
      return new RazorpayAdapter(new RazorpaySDK());
    default:
      // Exhaustiveness check.
      const _never: never = provider;
      throw new Error(`Unknown provider: ${_never}`);
  }
}

// =============================================================================
// 6. DEMO — the client runs identically regardless of the injected adapter.
// =============================================================================

async function main(): Promise<void> {
  const order: Order = {
    id: "ORD-1001",
    totalInCents: 5000,
    currency: "USD",
    cardToken: "tok_visa_4242",
  };

  for (const provider of ["stripe", "razorpay"] as ProviderName[]) {
    const gateway = buildGateway(provider); // <- the only place provider is chosen
    const service = new PaymentService(gateway); // <- client never changes
    await service.checkout({ ...order, currency: provider === "razorpay" ? "INR" : "USD" });
  }
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
