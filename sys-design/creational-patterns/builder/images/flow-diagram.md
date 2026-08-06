# Builder Pattern — Flow Diagram

Step-by-step control flow of constructing one `HttpRequest`, from creating the builder
through the `build()` validation gate to returning an immutable product (or throwing).

```mermaid
flowchart TD
    Start([Need a complex HttpRequest]) --> New["Create ConcreteBuilder<br/>(defaults applied)"]
    New --> Recipe{"Use a Director recipe?"}
    Recipe -- Yes --> RunRecipe["Director runs a named<br/>sequence of steps"]
    Recipe -- No --> Manual["Call steps directly"]
    RunRecipe --> Extra["Optionally add<br/>call-site-specific steps"]
    Manual --> Extra
    Extra --> Build["Call build()"]
    Build --> Valid{"Required fields set?<br/>Invariants hold?"}
    Valid -- No --> Throw["throw RequestBuildError<br/>(no product created)"]
    Valid -- Yes --> Create["Construct + freeze Product"]
    Create --> Reset["reset() builder state"]
    Reset --> Return["Return immutable HttpRequest"]
    Throw --> End([Done])
    Return --> End
```

**Key idea:** every path funnels through `build()`, the one validation gate. Required
fields and cross-field invariants are checked *together* there — the only point where
the full picture of the object exists. If any check fails, no product is created; if all
pass, the product is frozen and returned, and the builder resets so it can be reused safely.
