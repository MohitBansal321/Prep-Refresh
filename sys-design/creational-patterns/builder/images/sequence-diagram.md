# Builder Pattern — Sequence Diagram

Shows the runtime message exchange for constructing one request: the optional Director
recipe, extra caller-specific steps, and both the valid and invalid paths through `build()`.

```mermaid
sequenceDiagram
    autonumber
    participant Caller
    participant D as ApiRequestDirector (Director)
    participant B as FluentHttpRequestBuilder (ConcreteBuilder)
    participant P as HttpRequest (Product)

    Note over Caller,B: Optional: use a Director recipe
    Caller->>D: jsonPost(base, path, token, body)
    activate D
    D->>B: reset()
    D->>B: setUrl(base+path)
    D->>B: setMethod("POST")
    D->>B: addHeader("Authorization", ...)
    D->>B: setBody(body)
    D-->>Caller: returns builder
    deactivate D

    Note over Caller,B: Caller can add extra steps
    Caller->>B: addHeader("Idempotency-Key", ...)

    Caller->>B: build()
    activate B
    Note over B: validate required fields<br/>+ cross-field invariants
    alt all invariants pass
        B->>P: new HttpRequest(accumulated state)
        P-->>B: frozen product
        B->>B: reset()
        B-->>Caller: HttpRequest (immutable)
    else invariant violated
        B-->>Caller: throws RequestBuildError
    end
    deactivate B
```

**How to read it**
- The **Director** is optional: it `reset()`s the builder and runs a fixed sequence of steps, then hands the builder back — it deliberately does **not** call `build()`.
- After the recipe, the **Caller** can still add situation-specific steps (here an `Idempotency-Key` header) before finishing.
- `build()` is the single gate: it validates all required fields and cross-field invariants **before** any product exists.
- The `alt` block shows the two outcomes: on success the builder constructs a **frozen, immutable** `HttpRequest` and auto-`reset()`s itself; on any violation it throws `RequestBuildError` and no product is ever created.
