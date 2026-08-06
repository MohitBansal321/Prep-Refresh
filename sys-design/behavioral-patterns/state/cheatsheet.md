# State Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Behavioral design pattern (GoF). |
| **Intent** | Allow an object to alter its behavior when its internal state changes, so it appears to change its class. |
| **Problem** | An object behaves differently depending on a lifecycle status, and that status is branched on (`if`/`switch`) inside *every* method — logic is duplicated, transition rules are implicit, and illegal transitions slip through. |
| **Solution** | Represent each state as its own class implementing a common State interface. The Context holds the current state object and *delegates* every action to it. Each ConcreteState implements only its legal actions and triggers its own transitions. |
| **Participants** | **Context** (holds current state, delegates, owns shared data, does the swap) · **State** (interface of all actions) · **ConcreteState** (behavior + outgoing transitions for one state). |
| **Flow** | Client → calls action on Context → Context delegates to current State → State runs side effects + guard → State calls `context.transitionTo(next)` → Context swaps state field → fires `next.onEnter()` → behavior now changed. |
| **Pros** | Removes sprawling conditionals · SRP per state · OCP (new state = new class) · illegal transitions rejected by default · transitions explicit/discoverable · highly testable per state · entry/exit actions have a home. |
| **Cons** | More classes/files · indirection (jump between classes to trace) · state explosion in complex machines · shared data lives in Context · transition logic distributed · persistence adds real complexity. |
| **Use When** | Behavior varies by a **finite, well-defined lifecycle** status · large/duplicated conditionals on a status field · transition integrity matters (money, approvals, protocols) · you must model/document/test the lifecycle explicitly. |
| **Avoid When** | Few states + few actions (a boolean/`switch` is simpler) · status is a label with no behavioral difference · transitions carry **no** behavior (use a pure table) · machine is huge/hierarchical (use XState/Spring Statemachine) · states are open-ended at runtime. |
| **Real Examples** | Node streams/sockets · Express `req`/`res` (headers-sent guard) · Spring Statemachine · .NET Stateless · AWS Step Functions · Azure Durable Functions · XState (frontend UI states) · order/payment `status` columns · agent loops (plan→tool→observe). |
| **Related Topics** | Strategy (same structure, different intent) · Command (encapsulate an action) · Observer (entry actions notify) · Memento (snapshot for undo) · enum+switch (the baseline it replaces) · Finite State Machines / statecharts. |

### State vs Strategy (the #1 follow-up)
- **State** = *lifecycle*. The object **cycles through** states over time; each state **knows about and triggers** the next (`context.transitionTo(...)`). The states select the behavior.
- **Strategy** = *choosing an algorithm*. The **client** picks one interchangeable, independent algorithm; strategies never swap themselves out and don't know about each other.
- Same shape (delegate to a swappable inner object), opposite intent. Ask: *does the object move itself between these over its life (State), or does someone pick one once (Strategy)?*

### Skeleton
```ts
interface State {                                  // every action a client can attempt
  handle(ctx: Context): void;
}
class Context {
  private state: State = new StateA();
  action(): void { this.state.handle(this); }      // delegate — NO switch on status
  transitionTo(next: State): void {                // the ONLY place state is swapped
    this.state = next;
    next.onEnter?.(this);                           // guaranteed entry action
  }
}
abstract class BaseState implements State {         // reject-by-default trick
  handle(ctx: Context): void { throw new IllegalTransitionError(); }
}
class StateA extends BaseState {                    // override ONLY legal actions
  handle(ctx: Context): void { ctx.transitionTo(new StateB()); } // state owns the edge
}
```

### Remember In One Sentence
> **State is a lifecycle made of objects: the Context delegates to whichever state it holds, and each state owns both what it can do and where it goes next — so changing the state field changes the behavior.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the three participants and give each a one-line responsibility.
2. State vs Strategy — give the crisp intent difference: who selects the behavior, and do the parts trigger transitions in each other?
3. Where do transitions live in the classic (GoF) form — the Context or the states? Name the alternative and when you'd use it.
4. What is the "reject-by-default" base state, and what safety guarantee does it give you?
5. How do you enforce that an illegal transition (e.g. shipping an unpaid order) fails — what happens in code, and what is *not* drawn on the state diagram?
6. How do you persist a running state machine, and how do you rehydrate it? What must you be careful *not* to do on load?
7. Where do entry actions go, where do exit actions go, and why must side effects there be idempotent?
8. Why should the Context have no `if (status === ...)` in it, and where does per-state data (like a retry counter) live?
9. In a real backend, why are in-memory transition guards not enough, and what protects concurrent transitions on the same row?
10. When would you hand-roll State vs reach for a dedicated FSM/statechart tool (XState, Spring Statemachine, Step Functions)? Name two triggers.
