/**
 * STRATEGY PATTERN — Production-style TypeScript example
 * ------------------------------------------------------
 * Scenario: An e-commerce checkout must compute shipping cost. The rules differ
 * per shipping method (Standard, Express, Overnight, Free-over-threshold) and the
 * business keeps ADDING new methods (e.g. "Same-Day", "Locker Pickup"). We must
 * be able to pick the algorithm at runtime (the buyer chooses at checkout) and add
 * new algorithms WITHOUT editing the code that runs them.
 *
 * We solve this with the Strategy Pattern:
 *   - ShippingStrategy        -> Strategy (interface for the family of algorithms)
 *   - StandardShipping, ...   -> ConcreteStrategy (each a self-contained algorithm)
 *   - ShippingCostService     -> Context (holds a Strategy, delegates the work)
 *   - ShippingStrategyRegistry-> the SELECTION mechanism (map keyed by method)
 *
 * The whole point: `ShippingCostService` contains ZERO `if (method === ...)`
 * branches. Behavior is chosen by composition, not by conditionals.
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 0. DOMAIN TYPES — shared, vendor/algorithm-neutral vocabulary.
//    Money is ALWAYS in the smallest currency unit (cents) to avoid float bugs.
// =============================================================================

/** All monetary amounts are integer cents. Never use floats for money. */
export type Cents = number;

export type ShippingMethod =
  | "standard"
  | "express"
  | "overnight"
  | "free"; // "free over threshold"

/** The input every shipping algorithm receives. Kept small and explicit. */
export interface Shipment {
  /** Total physical weight of the parcel, in grams. */
  weightGrams: number;
  /** Order subtotal (goods only, before shipping), in cents. */
  orderSubtotal: Cents;
  /** Destination — some algorithms surcharge remote zones. */
  destinationZone: "domestic" | "regional" | "remote";
}

/** The output every shipping algorithm produces. */
export interface ShippingQuote {
  method: ShippingMethod;
  costInCents: Cents;
  /** Human-facing delivery estimate, e.g. "5-7 business days". */
  estimatedDelivery: string;
}

// =============================================================================
// 1. STRATEGY — the interface for the whole FAMILY of algorithms.
//    The Context depends ONLY on this. Every algorithm honors this one contract,
//    which is what makes them interchangeable (Liskov Substitution Principle).
// =============================================================================

export interface ShippingStrategy {
  /** Stable identifier used for selection/logging. */
  readonly method: ShippingMethod;
  /** Compute the cost for one shipment. Pure: no side effects, no shared state. */
  quote(shipment: Shipment): ShippingQuote;
}

// =============================================================================
// 2. CONCRETE STRATEGIES — each encapsulates ONE pricing algorithm.
//    They are STATELESS: no per-request fields, so a single instance is safely
//    shared (singleton) across all concurrent requests. Configuration (rates,
//    thresholds) is injected via the constructor, not hard-coded — Open/Closed.
// =============================================================================

/** Zone surcharge is a cross-cutting rule reused by several strategies. */
function zoneSurcharge(zone: Shipment["destinationZone"]): Cents {
  switch (zone) {
    case "domestic":
      return 0;
    case "regional":
      return 150; // +$1.50
    case "remote":
      return 500; // +$5.00
  }
}

export class StandardShipping implements ShippingStrategy {
  public readonly method = "standard" as const;

  // Rates injected => same class, different configuration per market/tenant.
  constructor(
    private readonly baseCents: Cents = 500, // $5.00 flat base
    private readonly perKgCents: Cents = 50, // $0.50 per kg
  ) {}

  quote(shipment: Shipment): ShippingQuote {
    const weightKg = shipment.weightGrams / 1000;
    const cost =
      this.baseCents +
      Math.ceil(weightKg) * this.perKgCents +
      zoneSurcharge(shipment.destinationZone);

    return {
      method: this.method,
      costInCents: cost,
      estimatedDelivery: "5-7 business days",
    };
  }
}

export class ExpressShipping implements ShippingStrategy {
  public readonly method = "express" as const;

  constructor(
    private readonly baseCents: Cents = 1500, // $15.00 base
    private readonly perKgCents: Cents = 100, // $1.00 per kg
  ) {}

  quote(shipment: Shipment): ShippingQuote {
    const weightKg = shipment.weightGrams / 1000;
    const cost =
      this.baseCents +
      Math.ceil(weightKg) * this.perKgCents +
      zoneSurcharge(shipment.destinationZone);

    return {
      method: this.method,
      costInCents: cost,
      estimatedDelivery: "2-3 business days",
    };
  }
}

export class OvernightShipping implements ShippingStrategy {
  public readonly method = "overnight" as const;

  // Flat premium rate; weight does not matter for this product.
  constructor(private readonly flatCents: Cents = 3000) {} // $30.00

  quote(shipment: Shipment): ShippingQuote {
    return {
      method: this.method,
      // Remote zones still surcharge even on flat rate.
      costInCents: this.flatCents + zoneSurcharge(shipment.destinationZone),
      estimatedDelivery: "Next business day",
    };
  }
}

/**
 * "Free shipping over a threshold" — a good example of an algorithm that
 * DECIDES its own result and even DELEGATES to another strategy as a fallback.
 * Below the threshold it charges standard rates; at/above it, shipping is free.
 */
export class FreeOverThresholdShipping implements ShippingStrategy {
  public readonly method = "free" as const;

  constructor(
    private readonly thresholdCents: Cents = 5000, // free at $50+
    // Composition: reuse an existing strategy for the "below threshold" case.
    private readonly fallback: ShippingStrategy = new StandardShipping(),
  ) {}

  quote(shipment: Shipment): ShippingQuote {
    if (shipment.orderSubtotal >= this.thresholdCents) {
      return {
        method: this.method,
        costInCents: 0,
        estimatedDelivery: "7-10 business days",
      };
    }
    // Not eligible yet — fall back, but keep the reported method honest.
    const fallbackQuote = this.fallback.quote(shipment);
    return { ...fallbackQuote, method: this.method };
  }
}

// =============================================================================
// 3. SELECTION — a registry (map) that resolves a method key to a strategy.
//    This REPLACES a growing `switch (method)` at the call site. Adding a new
//    method means registering one entry, not editing the Context or the service.
// =============================================================================

export class UnknownShippingMethodError extends Error {
  constructor(method: string) {
    super(`No shipping strategy registered for method "${method}"`);
    this.name = "UnknownShippingMethodError";
  }
}

export class ShippingStrategyRegistry {
  private readonly strategies = new Map<ShippingMethod, ShippingStrategy>();

  register(strategy: ShippingStrategy): this {
    this.strategies.set(strategy.method, strategy);
    return this;
  }

  resolve(method: ShippingMethod): ShippingStrategy {
    const strategy = this.strategies.get(method);
    if (!strategy) {
      throw new UnknownShippingMethodError(method);
    }
    return strategy;
  }

  /** Handy for a checkout UI that must list every available option. */
  quoteAll(shipment: Shipment): ShippingQuote[] {
    return [...this.strategies.values()].map((s) => s.quote(shipment));
  }
}

// =============================================================================
// 4. CONTEXT — the class the rest of the app talks to. It HOLDS a strategy and
//    DELEGATES to it. It contains no pricing rules and no conditionals selecting
//    behavior. Strategy can be set via constructor or swapped at runtime.
// =============================================================================

export class ShippingCostService {
  constructor(private strategy: ShippingStrategy) {}

  /** Swap the algorithm at runtime (e.g. the buyer changed their choice). */
  setStrategy(strategy: ShippingStrategy): void {
    this.strategy = strategy;
  }

  /** The Context delegates — it never knows WHICH algorithm it is running. */
  getQuote(shipment: Shipment): ShippingQuote {
    return this.strategy.quote(shipment);
  }
}

// =============================================================================
// 5. IDIOMATIC TS ALTERNATIVE — strategies as plain FUNCTIONS in a map.
//    When an algorithm is stateless and needs no configuration, a class is
//    ceremony. A function is a perfectly valid Strategy. Same pattern, less code.
// =============================================================================

export type ShippingFn = (shipment: Shipment) => ShippingQuote;

export const shippingFunctions: Record<ShippingMethod, ShippingFn> = {
  standard: (s) => new StandardShipping().quote(s),
  express: (s) => new ExpressShipping().quote(s),
  overnight: (s) => new OvernightShipping().quote(s),
  free: (s) => new FreeOverThresholdShipping().quote(s),
};

// =============================================================================
// 6. COMPOSITION ROOT — build the registry once at startup. In NestJS this is a
//    provider/module; here it is a function. Selecting a method is a map lookup.
// =============================================================================

function buildRegistry(): ShippingStrategyRegistry {
  return new ShippingStrategyRegistry()
    .register(new StandardShipping())
    .register(new ExpressShipping())
    .register(new OvernightShipping())
    .register(new FreeOverThresholdShipping(5000, new StandardShipping()));
}

// =============================================================================
// 7. DEMO — the buyer picks a method; the service runs identically for any of
//    them. Switching the algorithm is a one-line `setStrategy` or a map lookup.
// =============================================================================

function fmt(quote: ShippingQuote): string {
  return `${quote.method.padEnd(9)} $${(quote.costInCents / 100).toFixed(2).padStart(6)}  (${quote.estimatedDelivery})`;
}

function main(): void {
  const registry = buildRegistry();

  const shipment: Shipment = {
    weightGrams: 4200, // 4.2 kg -> rounds up to 5 kg
    orderSubtotal: 4000, // $40.00 -> below the $50 free threshold
    destinationZone: "regional",
  };

  console.log("=== All options for this shipment (registry.quoteAll) ===");
  for (const quote of registry.quoteAll(shipment)) {
    console.log("  " + fmt(quote));
  }

  console.log("\n=== Context delegating to a chosen strategy ===");
  const service = new ShippingCostService(registry.resolve("standard"));
  console.log("  Buyer picks standard: " + fmt(service.getQuote(shipment)));

  // Runtime swap — the buyer upgrades to express at checkout.
  service.setStrategy(registry.resolve("express"));
  console.log("  Buyer upgrades:       " + fmt(service.getQuote(shipment)));

  console.log("\n=== Free-over-threshold behavior ===");
  const eligible: Shipment = { ...shipment, orderSubtotal: 6000 }; // $60 -> free
  console.log("  $40 order: " + fmt(registry.resolve("free").quote(shipment)));
  console.log("  $60 order: " + fmt(registry.resolve("free").quote(eligible)));

  console.log("\n=== Functions-as-strategies (idiomatic TS) ===");
  console.log("  " + fmt(shippingFunctions.overnight(shipment)));
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main();
}
