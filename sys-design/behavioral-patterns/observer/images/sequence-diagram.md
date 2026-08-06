# Observer Pattern — Sequence Diagram

Shows the runtime message exchange for one `placeOrder` call, including the
composition-root wiring and the error-isolation path when one observer throws.

```mermaid
sequenceDiagram
    autonumber
    participant Root as Composition Root
    participant S as OrderService (Subject)
    participant E as EmailObserver
    participant I as InventoryObserver
    participant A as AnalyticsObserver
    participant D as AuditLogObserver

    Note over Root: Startup wiring (once)
    Root->>S: new OrderService()
    Root->>S: subscribe(EmailObserver)
    Root->>S: subscribe(InventoryObserver)
    Root->>S: subscribe(AnalyticsObserver)
    Root->>S: subscribe(AuditLogObserver)

    Note over S,D: Runtime — one order placed
    S->>S: placeOrder(order) persists order, builds OrderPlacedEvent
    activate S
    Note over S: notify(event) fans out (Promise.allSettled)
    S->>E: update(event)
    S->>I: update(event)
    S->>A: update(event)
    S->>D: update(event)
    E-->>S: resolved (email sent)
    I-->>S: resolved (stock decremented)
    A-->>S: resolved (event tracked)
    D-->>S: resolved (audit written)
    deactivate S

    alt One observer fails
        S->>E: update(event)
        E-->>S: rejected (loyalty service down)
        Note over S: allSettled captures the rejection,<br/>calls onObserverError, others unaffected
        S->>I: update(event)
        I-->>S: resolved (still runs)
    end
```

**How to read it**
- The **Composition Root** creates the subject and calls `subscribe(...)` once at
  startup — the subject never constructs its own observers.
- At runtime `placeOrder` builds an immutable event and calls `notify`, which
  fans the event out to every observer (the PUSH model — data is handed to them).
- Because the example uses `Promise.allSettled`, all observers are invoked and each
  outcome is recorded independently.
- The `alt` block shows **error isolation**: a rejected observer is caught and
  reported via `onObserverError`; the remaining observers still run. One broken
  listener never breaks the broadcast.
