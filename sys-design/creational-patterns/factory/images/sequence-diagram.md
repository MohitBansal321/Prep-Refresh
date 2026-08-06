# Factory Method Pattern — Sequence Diagram

Shows the runtime message exchange for one dispatch using the Factory Method
(Creator/subclass) form, including the composition-root wiring.

```mermaid
sequenceDiagram
    autonumber
    participant Root as Composition Root
    participant C as UserAlertService (Client)
    participant D as EmailDispatcher (ConcreteCreator)
    participant P as EmailSender (ConcreteProduct)

    Note over Root: Startup wiring (once)
    Root->>D: new EmailDispatcher(config)
    Root->>C: hand dispatcher (typed as NotificationDispatcher)

    Note over C,P: Runtime — one dispatch
    C->>D: dispatch(request)
    activate D
    Note over D: validate request body (shared workflow)
    D->>D: createSender()  // factory method, overridden
    D->>P: new EmailSender(config)
    activate P
    P-->>D: sender (typed as NotificationSender)
    deactivate P
    D->>P: send(to, subject, body)
    activate P
    P-->>D: SendResult
    deactivate P
    Note over D: shared audit log / metrics
    D-->>C: SendResult
    deactivate D
```

**How to read it**
- The **Composition Root** picks and constructs a specific ConcreteCreator (`EmailDispatcher`), then hands it to the Client *typed as the base* `NotificationDispatcher` — done once at startup.
- At runtime the Client only calls the Creator's workflow method `dispatch()`; it never chooses or builds a concrete product.
- Inside `dispatch()`, the Creator calls **its own factory method** `createSender()` — never `new` directly. Polymorphism resolves it to the overriding subclass, which returns the product typed as the `NotificationSender` interface.
- Shared workflow (validate, audit log, metrics) lives in the Creator and runs identically for every channel; only the created product differs.
