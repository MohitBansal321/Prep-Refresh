# State Pattern — Sequence Diagram

Shows the runtime message exchange for one action. The Context delegates to the
current state, the state performs side effects and triggers the transition, and the
Context swaps its state field and fires the new state's entry action. The second
block shows an illegal action being rejected.

```mermaid
sequenceDiagram
    autonumber
    participant Cl as Client (Controller/Service)
    participant O as Order (Context)
    participant P as PendingState
    participant Pd as PaidState

    Note over O: order.state = PendingState
    Cl->>O: pay(5000)
    O->>P: pay(order, 5000)
    activate P
    Note over P: record payment,<br/>guard: isFullyPaid()?
    P->>O: transitionTo(new PaidState())
    activate O
    Note over O: state = PaidState
    O->>Pd: onEnter(order)
    Pd-->>O: notify("PAYMENT_CONFIRMED")
    deactivate O
    deactivate P
    O-->>Cl: (state is now PAID)

    Note over Cl,O: Illegal action is rejected
    Cl->>O: ship("TRACK") %% but order is PENDING
    O->>P: ship(order, "TRACK")
    activate P
    Note over P: PendingState has no legal ship()
    P-->>Cl: throw IllegalTransitionError
    deactivate P
```

**How to read it**
- The Client only ever talks to the **Context** (`Order`); it never references a state object directly. It calls intent methods like `pay(5000)`.
- The Context does **not** inspect a status flag — it blindly delegates: `this.state.pay(this, 5000)`. The runtime type of `state` decides the behavior (polymorphism replacing conditionals).
- The **state** owns the transition: `PendingState` runs the guard (fully paid?) and, if satisfied, calls `order.transitionTo(new PaidState())`.
- `transitionTo` is the single place the state field is swapped, and it *guarantees* the new state's `onEnter` runs — which is where entry-action side effects (notifications) live.
- The second block shows the safety guarantee: `ship()` on a `PENDING` order falls through to the base state's default and throws `IllegalTransitionError` instead of silently corrupting the order.
