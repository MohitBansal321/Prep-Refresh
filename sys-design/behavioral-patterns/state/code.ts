/**
 * STATE PATTERN — Production-style TypeScript example
 * ---------------------------------------------------
 * Scenario: An e-commerce Order moves through a lifecycle:
 *
 *   DRAFT → PENDING → PAID → SHIPPED → DELIVERED
 *              │        │
 *              └────────┴──► CANCELLED
 *
 * Each stage allows a DIFFERENT set of actions:
 *   - You can only PAY an order that is PENDING.
 *   - You can only SHIP an order that is PAID.
 *   - You can only DELIVER an order that is SHIPPED.
 *   - You can CANCEL a DRAFT/PENDING/PAID order (a PAID cancel triggers a refund),
 *     but you CANNOT cancel one that has already SHIPPED or DELIVERED.
 *
 * The naive approach is a pile of `if (status === ...)` / `switch` blocks copied
 * into every method (pay/ship/deliver/cancel). That grows into an unmaintainable
 * mess. The State Pattern replaces those conditionals with ONE class per state.
 * Each state class encapsulates BOTH the behavior for that state AND which
 * transitions are legal from it.
 *
 * Participants:
 *   - OrderState        -> State interface (declares the state-specific actions)
 *   - Order             -> Context (holds current state, delegates every action to it)
 *   - Draft/Pending/... -> ConcreteState classes (behavior + allowed transitions)
 *
 * Extras that make this production-grade (not a toy):
 *   - Guard errors: illegal actions throw a typed `IllegalTransitionError`.
 *   - Entry actions: each state runs side effects on entry (via injected ports).
 *   - Dependency Injection: notifier + clock are injected, so states stay testable.
 *   - Persistence: `Order` serializes to a plain row and rehydrates from the DB,
 *     because in a real backend the object does NOT stay in memory between requests.
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 0. INFRASTRUCTURE PORTS — injected dependencies (kept as interfaces so states
//    never hard-code a concrete implementation). This is the "D" in SOLID.
// =============================================================================

/** Emits domain notifications (email/SMS/webhook). States call this on entry. */
export interface Notifier {
  notify(orderId: string, event: string): void;
}

/** Abstracts "now" so tests are deterministic and time is not read from Date.now() directly. */
export interface Clock {
  now(): Date;
}

/** A simple structured logger port. */
export interface Logger {
  info(message: string): void;
  warn(message: string): void;
}

/** Everything a state might need to do its work, bundled and injected once. */
export interface OrderDependencies {
  notifier: Notifier;
  clock: Clock;
  logger: Logger;
}

// =============================================================================
// 1. DOMAIN ERRORS — illegal transitions are rejected LOUDLY, not silently.
// =============================================================================

/** Thrown when an action is attempted that the current state does not allow. */
export class IllegalTransitionError extends Error {
  constructor(
    public readonly from: OrderStatus,
    public readonly action: string,
  ) {
    super(`Cannot "${action}" while order is in state "${from}"`);
    this.name = "IllegalTransitionError";
  }
}

// =============================================================================
// 2. STATE IDENTIFIERS — a closed union used for persistence and diagnostics.
//    The string value is what we store in the DB column `orders.status`.
// =============================================================================

export type OrderStatus =
  | "DRAFT"
  | "PENDING"
  | "PAID"
  | "SHIPPED"
  | "DELIVERED"
  | "CANCELLED";

// =============================================================================
// 3. STATE INTERFACE — declares every action a client can attempt on an order.
//    Every concrete state MUST implement all of them; states it does not allow
//    throw IllegalTransitionError. This is what replaces the scattered switches.
// =============================================================================

export interface OrderState {
  /** Machine-readable identity of this state (also the DB value). */
  readonly status: OrderStatus;

  /** Runs once, right after this state becomes the order's current state. */
  onEnter(order: Order): void;

  submit(order: Order): void; // DRAFT -> PENDING
  pay(order: Order, amountInCents: number): void; // PENDING -> PAID
  ship(order: Order, trackingId: string): void; // PAID -> SHIPPED
  deliver(order: Order): void; // SHIPPED -> DELIVERED
  cancel(order: Order, reason: string): void; // -> CANCELLED (where allowed)
}

// =============================================================================
// 4. CONTEXT — the Order. Holds the current State object and delegates EVERY
//    action to it. Notice there is not a single `if (status === ...)` here.
//    The Order also owns the data the states operate on (totals, tracking, etc.)
//    and exposes narrow mutators the states use to perform transitions.
// =============================================================================

/** Plain, serializable shape stored in PostgreSQL (one row per order). */
export interface OrderRow {
  id: string;
  status: OrderStatus;
  totalInCents: number;
  amountPaidInCents: number;
  trackingId: string | null;
  cancelReason: string | null;
  updatedAt: string; // ISO timestamp
}

export class Order {
  private state: OrderState;

  private amountPaidInCents = 0;
  private trackingId: string | null = null;
  private cancelReason: string | null = null;
  private updatedAt: Date;

  constructor(
    public readonly id: string,
    public readonly totalInCents: number,
    private readonly deps: OrderDependencies,
    initialState?: OrderState,
  ) {
    this.updatedAt = deps.clock.now();
    // A brand-new order starts as DRAFT unless we are rehydrating a known state.
    this.state = initialState ?? new DraftState();
  }

  // --- Public API the client calls. Each just delegates to the current state. ---
  // The Order never decides WHAT is legal — the state does.

  submit(): void {
    this.state.submit(this);
  }
  pay(amountInCents: number): void {
    this.state.pay(this, amountInCents);
  }
  ship(trackingId: string): void {
    this.state.ship(this, trackingId);
  }
  deliver(): void {
    this.state.deliver(this);
  }
  cancel(reason: string): void {
    this.state.cancel(this, reason);
  }

  getStatus(): OrderStatus {
    return this.state.status;
  }

  // --- Transition primitive used ONLY by state classes. ---
  // States call this to move the order forward; the Order fires the new state's
  // entry action. Centralizing the swap here means entry actions can never be
  // forgotten by an individual state.

  transitionTo(next: OrderState): void {
    this.deps.logger.info(
      `Order ${this.id}: ${this.state.status} -> ${next.status}`,
    );
    this.state = next;
    this.touch();
    next.onEnter(this); // guaranteed entry action
  }

  // --- Narrow mutators the states use. They express intent, not raw setters. ---

  recordPayment(amountInCents: number): void {
    this.amountPaidInCents += amountInCents;
    this.touch();
  }
  setTracking(trackingId: string): void {
    this.trackingId = trackingId;
    this.touch();
  }
  setCancelReason(reason: string): void {
    this.cancelReason = reason;
    this.touch();
  }

  // --- Read accessors states use to make decisions (e.g. is it fully paid?). ---

  getAmountPaid(): number {
    return this.amountPaidInCents;
  }
  isFullyPaid(): boolean {
    return this.amountPaidInCents >= this.totalInCents;
  }
  deps_(): OrderDependencies {
    return this.deps;
  }

  private touch(): void {
    this.updatedAt = this.deps.clock.now();
  }

  // ---------------------------------------------------------------------------
  // PERSISTENCE — the crux of using State in a real backend.
  // The Order object does NOT live in memory between HTTP requests. We save its
  // CURRENT STATE as a string in the DB, and rebuild the correct state OBJECT
  // when we load the row again. State-as-object in RAM, state-as-string on disk.
  // ---------------------------------------------------------------------------

  toRow(): OrderRow {
    return {
      id: this.id,
      status: this.state.status,
      totalInCents: this.totalInCents,
      amountPaidInCents: this.amountPaidInCents,
      trackingId: this.trackingId,
      cancelReason: this.cancelReason,
      updatedAt: this.updatedAt.toISOString(),
    };
  }

  /** Rebuild an Order (with the right state object) from a persisted DB row. */
  static fromRow(row: OrderRow, deps: OrderDependencies): Order {
    const state = OrderStateFactory.forStatus(row.status);
    const order = new Order(row.id, row.totalInCents, deps, state);
    // Restore mutable fields WITHOUT re-running entry actions (we are loading,
    // not transitioning — firing onEnter here would re-send emails, etc.).
    order.amountPaidInCents = row.amountPaidInCents;
    order.trackingId = row.trackingId;
    order.cancelReason = row.cancelReason;
    order.updatedAt = new Date(row.updatedAt);
    return order;
  }
}

// =============================================================================
// 5. STATE FACTORY — maps a persisted status string back to a fresh state object.
//    Keeping this in one place means "known states" are defined once (OCP-friendly:
//    add a state here + a class, and nothing else changes).
// =============================================================================

export class OrderStateFactory {
  static forStatus(status: OrderStatus): OrderState {
    switch (status) {
      case "DRAFT":
        return new DraftState();
      case "PENDING":
        return new PendingState();
      case "PAID":
        return new PaidState();
      case "SHIPPED":
        return new ShippedState();
      case "DELIVERED":
        return new DeliveredState();
      case "CANCELLED":
        return new CancelledState();
      default: {
        const _never: never = status; // exhaustiveness check
        throw new Error(`Unknown order status: ${_never}`);
      }
    }
  }
}

// =============================================================================
// 6. BASE STATE — provides the "this action is not allowed here" default for
//    every action. Concrete states OVERRIDE only the actions they permit.
//    This is the trick that keeps each concrete state small: it lists ONLY the
//    legal transitions; everything else automatically rejects.
// =============================================================================

abstract class BaseOrderState implements OrderState {
  abstract readonly status: OrderStatus;

  // Default: no side effects on entry. States with entry actions override this.
  onEnter(_order: Order): void {
    /* no-op by default */
  }

  // Default for EVERY action: reject as an illegal transition.
  submit(order: Order): void {
    this.reject(order, "submit");
  }
  pay(order: Order, _amountInCents: number): void {
    this.reject(order, "pay");
  }
  ship(order: Order, _trackingId: string): void {
    this.reject(order, "ship");
  }
  deliver(order: Order): void {
    this.reject(order, "deliver");
  }
  cancel(order: Order, _reason: string): void {
    this.reject(order, "cancel");
  }

  protected reject(order: Order, action: string): never {
    order.deps_().logger.warn(
      `Rejected "${action}" on order ${order.id} (state ${this.status})`,
    );
    throw new IllegalTransitionError(this.status, action);
  }
}

// =============================================================================
// 7. CONCRETE STATES — each overrides ONLY its legal actions and decides the
//    next state. Any action not overridden falls through to BaseOrderState and
//    throws IllegalTransitionError. That is the whole safety guarantee.
// =============================================================================

/** DRAFT: order is being assembled. Only allows submit() or cancel(). */
export class DraftState extends BaseOrderState {
  readonly status = "DRAFT" as const;

  submit(order: Order): void {
    order.transitionTo(new PendingState());
  }

  cancel(order: Order, reason: string): void {
    order.setCancelReason(reason);
    order.transitionTo(new CancelledState());
  }
}

/** PENDING: awaiting payment. Allows pay() (must be full) or cancel(). */
export class PendingState extends BaseOrderState {
  readonly status = "PENDING" as const;

  onEnter(order: Order): void {
    order.deps_().notifier.notify(order.id, "AWAITING_PAYMENT");
  }

  pay(order: Order, amountInCents: number): void {
    order.recordPayment(amountInCents);
    if (!order.isFullyPaid()) {
      // Partial payment: STAY in PENDING. No transition. The state itself
      // encodes this rule; the client does not need to know it.
      order.deps_().logger.info(
        `Order ${order.id}: partial payment, still PENDING ` +
          `(${order.getAmountPaid()}/${order.totalInCents})`,
      );
      return;
    }
    order.transitionTo(new PaidState());
  }

  cancel(order: Order, reason: string): void {
    order.setCancelReason(reason);
    order.transitionTo(new CancelledState());
  }
}

/** PAID: money captured, awaiting fulfilment. Allows ship() or cancel()+refund. */
export class PaidState extends BaseOrderState {
  readonly status = "PAID" as const;

  onEnter(order: Order): void {
    order.deps_().notifier.notify(order.id, "PAYMENT_CONFIRMED");
  }

  ship(order: Order, trackingId: string): void {
    order.setTracking(trackingId);
    order.transitionTo(new ShippedState());
  }

  cancel(order: Order, reason: string): void {
    // Cancelling a PAID order must refund. The refund side effect belongs to
    // the transition OUT of PAID, so it lives here.
    order.setCancelReason(reason);
    order
      .deps_()
      .notifier.notify(order.id, `REFUND_ISSUED:${order.getAmountPaid()}`);
    order.transitionTo(new CancelledState());
  }
}

/** SHIPPED: in transit. Only allows deliver(). Cannot be cancelled anymore. */
export class ShippedState extends BaseOrderState {
  readonly status = "SHIPPED" as const;

  onEnter(order: Order): void {
    order.deps_().notifier.notify(order.id, "OUT_FOR_DELIVERY");
  }

  deliver(order: Order): void {
    order.transitionTo(new DeliveredState());
  }
}

/** DELIVERED: terminal success state. No further actions allowed. */
export class DeliveredState extends BaseOrderState {
  readonly status = "DELIVERED" as const;

  onEnter(order: Order): void {
    order.deps_().notifier.notify(order.id, "DELIVERED");
  }
}

/** CANCELLED: terminal failure state. No further actions allowed. */
export class CancelledState extends BaseOrderState {
  readonly status = "CANCELLED" as const;

  onEnter(order: Order): void {
    order.deps_().notifier.notify(order.id, "CANCELLED");
  }
}

// =============================================================================
// 8. DEMO — including a persist/rehydrate round-trip to prove the DB story.
// =============================================================================

function makeDeps(): OrderDependencies {
  return {
    clock: { now: () => new Date("2026-07-14T10:00:00Z") },
    logger: {
      info: (m) => console.log(`  [info] ${m}`),
      warn: (m) => console.log(`  [warn] ${m}`),
    },
    notifier: {
      notify: (orderId, event) =>
        console.log(`  [notify] order=${orderId} event=${event}`),
    },
  };
}

function main(): void {
  const deps = makeDeps();

  console.log("=== Happy path: DRAFT -> PENDING -> PAID -> SHIPPED -> DELIVERED ===");
  const order = new Order("ORD-1001", 5000, deps);
  console.log(`  status: ${order.getStatus()}`);

  order.submit();
  console.log(`  status: ${order.getStatus()}`);

  console.log("\n-- partial payment of 2000 (should stay PENDING) --");
  order.pay(2000);
  console.log(`  status: ${order.getStatus()}`);

  console.log("\n-- remaining payment of 3000 (should become PAID) --");
  order.pay(3000);
  console.log(`  status: ${order.getStatus()}`);

  order.ship("TRACK-XYZ-999");
  console.log(`  status: ${order.getStatus()}`);

  order.deliver();
  console.log(`  status: ${order.getStatus()}`);

  console.log("\n=== Illegal transition is rejected ===");
  const bad = new Order("ORD-1002", 1000, deps);
  bad.submit();
  try {
    bad.ship("TRACK-NOPE"); // cannot ship a PENDING (unpaid) order
  } catch (e) {
    if (e instanceof IllegalTransitionError) {
      console.log(`  caught: ${e.message}`);
    } else {
      throw e;
    }
  }

  console.log("\n=== Cancel a PAID order triggers a refund ===");
  const refundable = new Order("ORD-1003", 1500, deps);
  refundable.submit();
  refundable.pay(1500);
  refundable.cancel("customer changed mind");
  console.log(`  status: ${refundable.getStatus()}`);

  console.log("\n=== Persist to DB and rehydrate (simulated) ===");
  const inFlight = new Order("ORD-1004", 800, deps);
  inFlight.submit();
  inFlight.pay(800); // now PAID
  const row = inFlight.toRow(); // <- what we store in PostgreSQL
  console.log(`  saved row: status=${row.status} paid=${row.amountPaidInCents}`);

  // ...later request, fresh process, object rebuilt from the row...
  const reloaded = Order.fromRow(row, deps);
  console.log(`  reloaded status: ${reloaded.getStatus()}`);
  reloaded.ship("TRACK-REHYDRATED"); // continues correctly from PAID
  console.log(`  status after ship: ${reloaded.getStatus()}`);
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main();
}
