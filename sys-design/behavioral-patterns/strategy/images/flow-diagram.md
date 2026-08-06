# Strategy Pattern — Flow Diagram

Step-by-step control flow of a single shipping-quote request: resolve the strategy,
inject it into the Context, then delegate. Note the unknown-method error branch.

```mermaid
flowchart TD
    Start([Need a shipping cost]) --> Input["Client has: chosen method + shipment"]
    Input --> Resolve{{"Registry.resolve(method)"}}
    Resolve -- known --> Got["Concrete Strategy instance"]
    Resolve -- unknown --> Err["Throw UnknownShippingMethodError"]
    Got --> Set["Context.setStrategy(strategy)"]
    Set --> Call["Context.getQuote(shipment)"]
    Call --> Delegate["Context delegates: strategy.quote(shipment)"]
    Delegate --> Run["Concrete Strategy runs ITS algorithm only"]
    Run --> Result["Return ShippingQuote"]
    Result --> End([Client uses the quote])
    Err --> End
```

**Key idea:** the only place a decision is made is `Registry.resolve(method)` — the
selection mechanism. Once a strategy is chosen, the Context does **no branching**: it
just delegates. Adding a new shipping method means adding one Concrete Strategy and
one registry entry; every box in this flow stays exactly as it is (Open/Closed).
