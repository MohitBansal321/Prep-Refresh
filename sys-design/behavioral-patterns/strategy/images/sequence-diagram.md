# Strategy Pattern — Sequence Diagram

Shows the runtime message exchange for one shipping quote, including the startup
wiring of the registry and a runtime strategy swap when the buyer upgrades.

```mermaid
sequenceDiagram
    autonumber
    participant Root as Composition Root
    participant Cl as Client (checkout)
    participant Reg as ShippingStrategyRegistry
    participant Ctx as ShippingCostService (Context)
    participant St as ExpressShipping (Concrete Strategy)

    Note over Root: Startup wiring (once)
    Root->>Reg: register(standard, express, overnight, free)

    Note over Cl,St: Runtime — buyer chose "express"
    Cl->>Reg: resolve("express")
    Reg-->>Cl: ExpressShipping instance
    Cl->>Ctx: new ShippingCostService(strategy)
    Cl->>Ctx: getQuote(shipment)
    activate Ctx
    Ctx->>St: quote(shipment)
    activate St
    St-->>Ctx: { method:"express", costInCents:2150, ... }
    deactivate St
    Ctx-->>Cl: ShippingQuote
    deactivate Ctx

    Note over Cl,St: Buyer upgrades to overnight — runtime swap
    Cl->>Reg: resolve("overnight")
    Reg-->>Cl: OvernightShipping instance
    Cl->>Ctx: setStrategy(strategy)
    Cl->>Ctx: getQuote(shipment)
    Ctx-->>Cl: ShippingQuote (overnight)
```

**How to read it**
- The **Composition Root** registers every Concrete Strategy in the registry once at startup — the analog of a NestJS provider/module.
- At runtime the Client asks the **Registry** to `resolve` a strategy by key, then injects it into the **Context** (via constructor or `setStrategy`).
- When the Client calls `getQuote`, the Context simply **delegates** (`this.strategy.quote(shipment)`) — it never inspects the method and never branches.
- The second block shows **runtime swappability**: `setStrategy` replaces the algorithm on a live Context, so behavior changes with no new conditionals and no redeploy.
- Only the Concrete Strategy knows its own pricing rules; the Context knows only the `ShippingStrategy` interface.
