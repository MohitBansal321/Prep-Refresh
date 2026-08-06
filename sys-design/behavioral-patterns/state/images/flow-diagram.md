# State Pattern — Flow Diagram

The natural way to view a State machine is as a `stateDiagram-v2`: nodes are states,
arrows are the legal transitions (labelled with the triggering action). A second
`flowchart` below shows the control flow of a single delegated action.

```mermaid
stateDiagram-v2
    [*] --> DRAFT
    DRAFT --> PENDING : submit()
    DRAFT --> CANCELLED : cancel()
    PENDING --> PENDING : pay() partial
    PENDING --> PAID : pay() full
    PENDING --> CANCELLED : cancel()
    PAID --> SHIPPED : ship()
    PAID --> CANCELLED : cancel() + refund
    SHIPPED --> DELIVERED : deliver()
    DELIVERED --> [*]
    CANCELLED --> [*]

    note right of SHIPPED
        No cancel() edge here:
        a shipped order cannot
        be cancelled.
    end note
```

And the control flow of a single delegated action:

```mermaid
flowchart TD
    Start([Client calls order.action]) --> Delegate["Order delegates: this.state.action(this)"]
    Delegate --> Legal{"Does current state<br/>implement this action?"}
    Legal -- No --> Reject["BaseOrderState.reject()<br/>throw IllegalTransitionError"]
    Legal -- Yes --> Effects["Run side effects<br/>(record payment, set tracking...)"]
    Effects --> Guard{"Guard passed?<br/>(e.g. fully paid?)"}
    Guard -- No --> Stay["Stay in same state<br/>(no transition)"]
    Guard -- Yes --> Trans["order.transitionTo(nextState)"]
    Trans --> Swap["Context swaps state field"]
    Swap --> Enter["next.onEnter(order)<br/>(entry action / notification)"]
    Enter --> End([Return to client; behavior now changed])
    Stay --> End
    Reject --> End
```

**How to read it**
- **Top diagram (the state machine):** each node is a state and each labelled arrow is a *legal* transition triggered by an action. This is the single source of truth for the lifecycle — it maps one-to-one to the ConcreteState classes in `code.ts`.
- The self-loop `PENDING --> PENDING : pay() partial` encodes the "partial payment stays PENDING" rule; only a *full* payment crosses to `PAID`.
- The absence of a `cancel()` edge out of `SHIPPED` is deliberate and load-bearing: `ShippedState` simply does not implement `cancel`, so the base class rejects it. **Illegal transitions are the edges that are not drawn.**
- `[*]` marks the start (into `DRAFT`) and the terminal states (`DELIVERED`, `CANCELLED`), which accept no further actions.
- **Bottom diagram (one action's control flow):** every action goes delegate → is-it-legal? → side effects → guard → transition (swap + `onEnter`). The "No" branch on the guard is why an action can succeed without changing state (partial payment).
