# Facade Pattern — Flow Diagram

Step-by-step control flow of one `placeOrder()` call through the facade, including
the compensating-rollback branches (release stock / refund) on partial failure and
the best-effort notification step.

```mermaid
flowchart TD
    Start([Client calls placeOrder]) --> Reserve["Facade: inventory.reserve(lines)"]
    Reserve --> ROk{Reserved?}
    ROk -- No --> Err1["throw CheckoutError(INVENTORY)"]
    ROk -- Yes --> Charge["Facade: payment.charge(...)"]
    Charge --> POk{Charged?}
    POk -- No --> Rel1["release(reservation)"] --> Err2["throw CheckoutError(PAYMENT)"]
    POk -- Yes --> Ship["Facade: shipping.createShipment(...)"]
    Ship --> SOk{Shipment created?}
    SOk -- No --> Comp["refund(txn) + release(reservation)"] --> Err3["throw CheckoutError(SHIPPING)"]
    SOk -- Yes --> Notify["Facade: notification.send(...) — best-effort"]
    Notify --> NOk{Sent?}
    NOk -- No --> Warn["log warning, continue"]
    NOk -- Yes --> Done
    Warn --> Done["Return PlaceOrderResult CONFIRMED"]
    Done --> End([Client gets ONE result])
```

**Key idea:** every green-path box is a single *delegation* to a subsystem — the
facade does no real work itself. The value the facade adds is the **ordering** (reserve
before charge, charge before ship) and the **compensating rollback** on failure. Note
the asymmetry: reserve/charge/ship are *fatal* (roll back and throw), while notify is
*best-effort* (log and continue) — you never undo a paid, shipped order because an
email bounced. That fatal-vs-best-effort decision is exactly the orchestration
knowledge the facade centralizes so it cannot be duplicated or forgotten by callers.
