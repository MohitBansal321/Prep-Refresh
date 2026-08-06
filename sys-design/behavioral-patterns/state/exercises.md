# State Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting a lifecycle/status field with scattered conditionals and refactoring it into one class per state, where each state owns its legal actions *and* its outgoing transitions.

> Rule of thumb for every exercise: the **Context** holds the current state and delegates — it must contain **no** `if (status === ...)` / `switch` on status. Each **ConcreteState** implements only the actions it permits and triggers its own transitions via `context.transitionTo(...)`. Everything illegal should reject **by default** (via an abstract base state), not by hand-written checks.

---

## Easy — Turnstile

Model a subway turnstile with two states, `LOCKED` and `UNLOCKED`, and two actions:

```ts
interface TurnstileState {
  insertCoin(t: Turnstile): void;
  push(t: Turnstile): void;
}
```

- `LOCKED`: `insertCoin` → transition to `UNLOCKED`; `push` → stays `LOCKED` (and logs "access denied").
- `UNLOCKED`: `push` → transition to `LOCKED`; `insertCoin` → stays `UNLOCKED` (and logs "coin returned").

**Task:** Write `Turnstile` (the Context) that delegates `insertCoin()` and `push()` to its current state. Write `LockedState` and `UnlockedState`. The Context must hold no conditional on the current state name.

**Acceptance:** Starting `LOCKED`, trace the sequence `push → insertCoin → push → push`: `push` (denied, stays LOCKED) → `insertCoin` (unlock) → `push` (lock) → `push` (denied, stays LOCKED). Final state is `LOCKED`, with exactly two "access denied" logs.

---

## Medium — Document Approval Workflow

A document moves through: `DRAFT → IN_REVIEW → APPROVED / REJECTED`, with a `CHANGES_REQUESTED` state that sends it back to the author.

Actions: `submit()`, `approve()`, `reject(reason)`, `requestChanges(notes)`, `edit()`.

Rules:
- `DRAFT`: `submit` → `IN_REVIEW`; `edit` allowed (stays `DRAFT`).
- `IN_REVIEW`: `approve` → `APPROVED`; `reject` → `REJECTED`; `requestChanges` → `CHANGES_REQUESTED`. `edit` is illegal here.
- `CHANGES_REQUESTED`: `edit` allowed (stays); `submit` → back to `IN_REVIEW`.
- `APPROVED` and `REJECTED`: terminal — every action rejects.

**Task:** Build it with an abstract `BaseDocState` that rejects every action by default; each concrete state overrides only what it allows. Throw a typed `IllegalTransitionError(from, action)` on rejection.

**Bonus constraint:** Only the states `IN_REVIEW` may act on reviewer actions; assert that calling `approve()` on a `DRAFT` throws, not silently no-ops.

---

## Hard — Subscription Billing with Guards and Entry Actions

Model a SaaS subscription: `TRIALING → ACTIVE → PAST_DUE → CANCELLED / EXPIRED`.

Actions: `activate()`, `charge(success: boolean)`, `retry()`, `cancel()`.

Rules:
- `TRIALING`: `activate` → `ACTIVE`. On entering `TRIALING`, send a "trial started" notification (entry action).
- `ACTIVE`: `charge(true)` → stays `ACTIVE`; `charge(false)` → `PAST_DUE`; `cancel` → `CANCELLED`.
- `PAST_DUE`: `retry()` runs a payment; on success → `ACTIVE`, on the **third** failed retry → `EXPIRED`. On entering `PAST_DUE`, fire a dunning email. `cancel` → `CANCELLED`.
- `CANCELLED` / `EXPIRED`: terminal.

**Task:**
- Track the retry count as **Context data** (not in the state object), and let `PastDueState` read/increment it via a narrow mutator — states are stateless and shareable.
- Inject a `Notifier` and `Clock` into the Context; states reach them through the Context. No `new Notifier()` inside a state.
- Entry actions must fire on transition but **must not** fire on rehydration (see below).

**Think about:** Where does the "third failed retry" guard live — the state or the Context? Where does the retry *counter* live? Why must those be different places?

---

## Real-World Challenge — Persisted, Concurrency-Safe Order Machine

Extend the `Order` machine from [code.ts](code.ts) into something that survives a real backend.

**Requirements:**
- **Persistence round-trip:** implement `toRow()` / `fromRow(row)` so an order can be saved to a (simulated) `orders` table as a `status` string and rebuilt into the correct state object later. Rehydration must **not** re-fire entry actions (loading a `PAID` order must not re-send "payment confirmed").
- **Optimistic locking:** add a `version` column. `save(row, expectedVersion)` must reject (throw a `ConcurrencyError`) if the stored version no longer matches — simulate two requests both loading a `PAID` order and both calling `ship(...)`; exactly one must win.
- **Illegal-transition audit:** every rejected action must be logged with `{ orderId, fromState, action }` via the injected logger.
- **New state without touching old ones:** add a `RETURNED` state reachable only from `DELIVERED`. Confirm you added one class + one factory branch and modified **zero** existing state classes.

**Stretch:** Add an exit action. When leaving `PAID` via `cancel`, issue a refund; when leaving via `ship`, do nothing. Decide whether "exit action" belongs in the *leaving* state's method or in `transitionTo`, and justify it.

---

## Bonus Challenge — Two Schools of Transitions, and XState

1. **States-own-transitions vs Context-owns-a-table.** You built the machine with states calling `context.transitionTo(...)` (GoF default). Now re-implement the *same* order lifecycle as a central transition **table**: a `Map<[state, action], nextState>` in the Context, with a separate map of side-effect handlers. Write down, in comments, which approach you would choose for (a) a 3-state behavior-heavy machine and (b) a 20-state behavior-free routing graph, and why.

2. **State vs Strategy, proven in code.** Take a `PaymentStrategy` (client picks `CreditCard` vs `PayPal` once, algorithms never swap themselves) and place it beside the `OrderState` machine. In a comment, state the single intent difference: who selects the behavior, and whether the parts trigger transitions in each other.

3. **Model it in XState.** Express the order lifecycle as an XState (or pseudo-XState) machine definition — states, events, guards, and entry `actions`. Note which parts of your hand-rolled version map to XState's `guards`, `actions`, and `context`, and argue when you would reach for XState instead of hand-rolling (hierarchy, visualization, history states, parallel regions).

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
