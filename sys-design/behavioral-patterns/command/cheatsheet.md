# Command Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Behavioral design pattern (GoF). |
| **Intent** | Encapsulate a request as an object so you can parameterize clients with different requests, queue or log them, and support undoable operations. |
| **Problem** | A plain method call is transient — it cannot be stored, queued, logged, grouped, or reversed. You need requests to be first-class objects. |
| **Solution** | Wrap each request in a Command object (`execute()`, usually `undo()`) that binds a Receiver + action + params; hand commands to an Invoker that triggers them and keeps a history/queue. |
| **Participants** | **Command** (interface) · **ConcreteCommand** (binds receiver+action+params) · **Receiver** (does the real work) · **Invoker** (triggers, holds history/queue) · **Client** (builds & wires commands). |
| **Flow** | Client builds a ConcreteCommand → hands it to the Invoker → Invoker calls `execute()` → command delegates to the Receiver → Invoker records it in history/log → later `undo()` pops history and reverses via the Receiver. |
| **Pros** | Decouples invoker from receiver · undo/redo · queuing & deferred/async execution · logging/replay (event sourcing) · macros (composite) · OCP (new op = new class) · SRP. |
| **Cons** | Class per operation (class explosion) · indirection · undo is hard for side effects · must capture "before" state for lossy undo · history uses memory · serialization/idempotency burden if persisted/queued. |
| **Use When** | You need undo/redo · queue/schedule/defer work · audit log or replay · parameterize something with an operation to run later · group operations into one reversible unit. |
| **Avoid When** | Operation is one-shot, immediate, and never reversed/queued/logged (use a direct call) · lightweight deferral only (a closure is enough) · tiny stable op set (YAGNI) · ultra-hot loops. |
| **Real Examples** | BullMQ/SQS jobs · `@nestjs/cqrs` CommandBus · MediatR (.NET) · DB migrations `up()`/`down()` · Redux actions + redux-undo · WPF `ICommand` · WAL/redo-undo logs · LLM tool calls · Saga/Step Functions compensation. |
| **Related Topics** | Strategy (swap algorithm, no undo) · Memento (capture state for undo) · Chain of Responsibility (route request) · Mediator (CommandBus) · Observer (broadcast events) · CQRS · Event Sourcing · Saga. |

### execute() vs undo()
- **`execute()`** performs the request by delegating to the Receiver. Run it *before* recording in history, so a failure leaves no bogus entry.
- **`undo()`** reverses it. For symmetric ops the inverse is obvious (deposit ↔ withdraw). For **lossy** ops (set/replace/rename) you must snapshot the previous value *during* `execute()` — this is where Command borrows **Memento**.
- Real-world side effects (emails, card charges) are not reversible by a simple inverse → use **compensating transactions** (Saga).

### Skeleton
```ts
interface Command { execute(): void; undo(): void; }          // the request contract

class Account { deposit(n: number) {/*...*/} withdraw(n: number) {/*...*/} } // Receiver

class WithdrawCommand implements Command {                     // ConcreteCommand
  constructor(private acc: Account, private amount: number) {} // binds receiver + params
  execute() { this.acc.withdraw(this.amount); }
  undo()    { this.acc.deposit(this.amount); }                 // inverse action
}

class Invoker {                                                // triggers + history
  private history: Command[] = [];
  run(c: Command) { c.execute(); this.history.push(c); }       // execute THEN record
  undo() { this.history.pop()?.undo(); }
}
```

### Remember In One Sentence
> **A Command is an order slip: it turns "do this thing" into an object you can queue, log, group, and tear up (undo) — separating who asks for the work from who does it.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the five participants and give each a one-line responsibility.
2. What does "encapsulate a request as an object" actually mean, and what does that unlock that a plain method call cannot?
3. Where does business logic live — Command, Receiver, or Invoker? What does the Command hold instead?
4. Why must you `execute()` a command *before* pushing it to the history stack?
5. For a lossy operation like "set brightness to 20", why can't `undo()` just read the current value? What must the command do, and which pattern is that?
6. How do you undo something with a real-world side effect (e.g. an email or a card charge)? Name the pattern.
7. Why do queued commands need an idempotency key, and where does the dedup check belong?
8. Command vs Strategy — give the one-line difference. Which one is typically undoable and stores a receiver?
9. How is Command the foundation of a job queue like BullMQ and of CQRS?
10. Give two situations where Command is the *wrong* choice.
