# Observer Pattern — Flow Diagram

Step-by-step control flow of a single `placeOrder` → `notify` fan-out, including
the per-observer success/failure branch.

```mermaid
flowchart TD
    Start([Customer places an order]) --> Persist["OrderService.placeOrder(): persist order"]
    Persist --> Build["Build immutable OrderPlacedEvent snapshot"]
    Build --> Snap["notify(): take snapshot of observer set"]
    Snap --> Fan{{"Fan out event to every observer<br/>(Promise.allSettled)"}}

    Fan --> O1["EmailObserver.update(event)"]
    Fan --> O2["InventoryObserver.update(event)"]
    Fan --> O3["AnalyticsObserver.update(event)"]
    Fan --> O4["AuditLogObserver.update(event)"]

    O1 --> R{"Each observer<br/>settled?"}
    O2 --> R
    O3 --> R
    O4 --> R

    R -- fulfilled --> Ok["Reaction complete"]
    R -- rejected --> Err["onObserverError(reason, observer, event)<br/>log / report — do NOT rethrow"]

    Ok --> Done([All observers settled; placeOrder returns])
    Err --> Done
```

**Key idea:** the subject broadcasts the same event to every observer and waits
for all of them to *settle*, never *fail-fast*. A rejected observer is routed to
`onObserverError` and does not abort the others. Adding a new reaction means
adding a new branch here (a new observer) — the `placeOrder` logic above the
fan-out never changes.
