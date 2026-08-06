/**
 * OBSERVER PATTERN — Production-style TypeScript example
 * ------------------------------------------------------
 * Scenario: When a customer places an order, several INDEPENDENT things must
 * happen: send a confirmation email, decrement inventory, record an analytics
 * event, and write an audit-log entry. Tomorrow marketing wants a loyalty-points
 * update too. The OrderService must NOT know or care who reacts to an order.
 *
 * We solve this with the Observer Pattern:
 *   - OrderObserver          -> Observer interface (the reaction contract)
 *   - OrderSubject           -> Subject interface (subscribe/unsubscribe/notify)
 *   - OrderService           -> ConcreteSubject (owns state, notifies on change)
 *   - EmailObserver, InventoryObserver, AnalyticsObserver, AuditLogObserver
 *                            -> ConcreteObservers (each reacts independently)
 *
 * Key production concerns demonstrated here (not a toy):
 *   - Error ISOLATION: one throwing observer must not break the others.
 *   - ASYNC notification: observers do I/O (email, DB) — we await them safely.
 *   - Unsubscribe support to avoid the "lapsed listener" memory leak.
 *   - PUSH model: the event payload is handed to every observer.
 *   - Dependency Injection: observers are injected, never constructed inside
 *     the subject, so everything is unit-testable with fakes.
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. DOMAIN TYPES & EVENT PAYLOAD
//    The event is an immutable snapshot of "what happened". Observers receive
//    this (the PUSH model) and never reach back into the subject's internals.
// =============================================================================

export interface OrderLine {
  sku: string;
  quantity: number;
  unitPriceInCents: number;
}

export interface Order {
  id: string;
  customerEmail: string;
  lines: OrderLine[];
  totalInCents: number;
  currency: string;
}

/**
 * The notification payload. It is `readonly` because an observer must never be
 * able to mutate the event and thereby affect other observers.
 */
export interface OrderPlacedEvent {
  readonly type: "order.placed";
  readonly order: Order;
  readonly occurredAt: Date;
}

// =============================================================================
// 2. OBSERVER INTERFACE — the reaction contract.
//    The Subject knows observers ONLY through this interface. `update` is async
//    because real reactions (send email, write to Postgres) are I/O-bound.
// =============================================================================

export interface OrderObserver {
  /** A stable name, used only for logging/diagnostics (which observer failed). */
  readonly name: string;

  /** Called by the subject whenever an order-placed event occurs. */
  update(event: OrderPlacedEvent): Promise<void>;
}

// =============================================================================
// 3. SUBJECT INTERFACE — the subscription contract.
//    Anything that can be observed exposes these three operations.
// =============================================================================

export interface OrderSubject {
  subscribe(observer: OrderObserver): void;
  unsubscribe(observer: OrderObserver): void;
  /** Notify every subscribed observer. Returns after all have settled. */
  notify(event: OrderPlacedEvent): Promise<void>;
}

// =============================================================================
// 4. A REUSABLE, PRODUCTION-GRADE SUBJECT BASE
//    Encapsulates the mechanics that EVERY subject needs: a duplicate-free
//    registry, safe iteration over a copy, error isolation, and async fan-out.
//    Concrete subjects inherit this and add their own state + trigger methods.
// =============================================================================

/** Optional hook so callers can decide what happens when an observer throws. */
export type ObserverErrorHandler = (
  error: unknown,
  observer: OrderObserver,
  event: OrderPlacedEvent,
) => void;

export abstract class AbstractOrderSubject implements OrderSubject {
  // A Set gives O(1) add/remove and prevents accidental double-subscription
  // (a classic bug that makes an observer fire twice per event).
  private readonly observers = new Set<OrderObserver>();

  constructor(
    // Default handler just logs; production code might push to Sentry, etc.
    private readonly onObserverError: ObserverErrorHandler = (err, obs) =>
      console.error(`[Subject] Observer "${obs.name}" failed:`, err),
  ) {}

  subscribe(observer: OrderObserver): void {
    this.observers.add(observer);
  }

  unsubscribe(observer: OrderObserver): void {
    // Removing on unsubscribe is what prevents the "lapsed listener" memory
    // leak: a subject holds strong references, so a forgotten observer (and
    // everything it retains) can never be garbage-collected.
    this.observers.delete(observer);
  }

  /**
   * Notify all observers. Design decisions worth understanding:
   *
   *  1. We iterate a SNAPSHOT ([...observers]). If an observer unsubscribes
   *     (or a new one subscribes) during notification, we do not corrupt the
   *     iteration or throw "Set changed during iteration".
   *
   *  2. We use `Promise.allSettled`, NOT `Promise.all`. With `all`, the first
   *     rejection would abandon the others' results and surface as one error.
   *     `allSettled` runs every observer to completion and lets us report each
   *     failure independently — one broken observer never blocks the rest.
   *
   *  3. Observers run CONCURRENTLY here. If you need strict ordering or
   *     back-pressure, replace the map with a sequential `for..of await` loop.
   */
  async notify(event: OrderPlacedEvent): Promise<void> {
    const snapshot = [...this.observers];

    const results = await Promise.allSettled(
      snapshot.map((observer) => observer.update(event)),
    );

    results.forEach((result, i) => {
      if (result.status === "rejected") {
        this.onObserverError(result.reason, snapshot[i], event);
      }
    });
  }

  /** Exposed for diagnostics/tests only. */
  protected get observerCount(): number {
    return this.observers.size;
  }
}

// =============================================================================
// 5. CONCRETE SUBJECT — OrderService.
//    Holds the real business state and TRIGGERS notification when state changes.
//    Note what it does NOT do: it never mentions Email, Inventory, Analytics,
//    or Audit. It only knows "some observers exist". That is the decoupling.
// =============================================================================

export class OrderService extends AbstractOrderSubject {
  /**
   * The business operation. Persisting the order is the subject's own job;
   * everything that must happen AS A CONSEQUENCE is delegated to observers.
   */
  async placeOrder(order: Order): Promise<void> {
    // --- core business logic / persistence would go here ---
    console.log(`[OrderService] Order ${order.id} persisted.`);

    // Build an immutable event snapshot and broadcast it.
    const event: OrderPlacedEvent = {
      type: "order.placed",
      order,
      occurredAt: new Date(),
    };

    await this.notify(event);
    console.log(`[OrderService] All observers notified for ${order.id}.`);
  }
}

// =============================================================================
// 6. COLLABORATORS the observers depend on (injected — SOLID / testable).
//    In real code these are your NestJS providers: a mailer, a repository, etc.
// =============================================================================

export interface Mailer {
  send(to: string, subject: string, body: string): Promise<void>;
}

export interface InventoryRepository {
  decrement(sku: string, quantity: number): Promise<void>;
}

export interface AnalyticsClient {
  track(eventName: string, props: Record<string, unknown>): Promise<void>;
}

export interface AuditLogRepository {
  append(entry: Record<string, unknown>): Promise<void>;
}

// =============================================================================
// 7. CONCRETE OBSERVERS — each reacts to the SAME event, independently.
//    None of them knows about the others. Adding/removing one changes nothing
//    elsewhere (Open/Closed Principle).
// =============================================================================

export class EmailObserver implements OrderObserver {
  readonly name = "EmailObserver";
  constructor(private readonly mailer: Mailer) {}

  async update(event: OrderPlacedEvent): Promise<void> {
    const { order } = event;
    await this.mailer.send(
      order.customerEmail,
      `Order ${order.id} confirmed`,
      `Thanks! We received your order of ${order.lines.length} item(s).`,
    );
    console.log(`  [Email] Confirmation sent to ${order.customerEmail}`);
  }
}

export class InventoryObserver implements OrderObserver {
  readonly name = "InventoryObserver";
  constructor(private readonly inventory: InventoryRepository) {}

  async update(event: OrderPlacedEvent): Promise<void> {
    for (const line of event.order.lines) {
      await this.inventory.decrement(line.sku, line.quantity);
    }
    console.log(`  [Inventory] Stock decremented for order ${event.order.id}`);
  }
}

export class AnalyticsObserver implements OrderObserver {
  readonly name = "AnalyticsObserver";
  constructor(private readonly analytics: AnalyticsClient) {}

  async update(event: OrderPlacedEvent): Promise<void> {
    await this.analytics.track("OrderPlaced", {
      orderId: event.order.id,
      value: event.order.totalInCents,
      currency: event.order.currency,
    });
    console.log(`  [Analytics] Tracked OrderPlaced for ${event.order.id}`);
  }
}

export class AuditLogObserver implements OrderObserver {
  readonly name = "AuditLogObserver";
  constructor(private readonly audit: AuditLogRepository) {}

  async update(event: OrderPlacedEvent): Promise<void> {
    await this.audit.append({
      action: "order.placed",
      orderId: event.order.id,
      at: event.occurredAt.toISOString(),
    });
    console.log(`  [Audit] Logged order.placed for ${event.order.id}`);
  }
}

// =============================================================================
// 8. COMPOSITION ROOT — wire subject and observers together ONCE at startup.
//    Adding a new reaction later = write one observer + one subscribe() line.
//    The OrderService source is never touched.
// =============================================================================

// --- Fake collaborators so the file runs standalone (stand-ins for real I/O) ---
const fakeMailer: Mailer = {
  async send() {
    /* pretend SMTP call */
  },
};
const fakeInventory: InventoryRepository = {
  async decrement() {
    /* pretend UPDATE ... SET stock = stock - $1 */
  },
};
const fakeAnalytics: AnalyticsClient = {
  async track() {
    /* pretend Segment/Amplitude call */
  },
};
const fakeAudit: AuditLogRepository = {
  async append() {
    /* pretend INSERT INTO audit_log ... */
  },
};

function buildOrderService(): OrderService {
  const service = new OrderService();

  service.subscribe(new EmailObserver(fakeMailer));
  service.subscribe(new InventoryObserver(fakeInventory));
  service.subscribe(new AnalyticsObserver(fakeAnalytics));
  service.subscribe(new AuditLogObserver(fakeAudit));

  return service;
}

// =============================================================================
// 9. DEMO — placing an order fans out to every observer, with error isolation.
// =============================================================================

async function main(): Promise<void> {
  const service = buildOrderService();

  const order: Order = {
    id: "ORD-2001",
    customerEmail: "buyer@example.com",
    currency: "USD",
    totalInCents: 4599,
    lines: [
      { sku: "BOOK-TS", quantity: 1, unitPriceInCents: 2999 },
      { sku: "MUG-JS", quantity: 2, unitPriceInCents: 800 },
    ],
  };

  console.log("=== Placing order with all observers healthy ===");
  await service.placeOrder(order);

  // Demonstrate error isolation: a flaky observer that always throws.
  console.log("\n=== Adding a flaky observer that throws ===");
  const flaky: OrderObserver = {
    name: "FlakyLoyaltyObserver",
    async update() {
      throw new Error("loyalty service unavailable");
    },
  };
  service.subscribe(flaky);

  await service.placeOrder({ ...order, id: "ORD-2002" });
  console.log(
    "Notice: the flaky observer failed, but Email/Inventory/Analytics/Audit still ran.",
  );

  // Demonstrate unsubscribe (prevents the lapsed-listener leak).
  console.log("\n=== Unsubscribing the flaky observer ===");
  service.unsubscribe(flaky);
  await service.placeOrder({ ...order, id: "ORD-2003" });
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
