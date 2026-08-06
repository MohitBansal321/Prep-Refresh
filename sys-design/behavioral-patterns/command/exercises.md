# Command Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of turning a *request* into an object and separating the thing that triggers work from the thing that performs it.

> Rule of thumb for every exercise: **business logic lives in the Receiver, not the Command.** The Command only remembers "which receiver, which action, which params" and how to `undo()`. The Invoker speaks only to the `Command` interface — it must never know a concrete command type.

---

## Easy — Light Switch with Undo

You have a `Light` receiver you may extend but whose core behavior is fixed:

```ts
class Light {
  private on = false;
  turnOn(): void { this.on = true; }
  turnOff(): void { this.on = false; }
  get isOn(): boolean { return this.on; }
}
```

**Task:** Define a `Command` interface with `execute()` and `undo()`. Write `TurnOnCommand` and `TurnOffCommand`, each wrapping a `Light`. Then write a small `RemoteControl` invoker with `press(command)` and `pressUndo()` that keeps a one-item history.

**Acceptance:** Pressing on then undo leaves the light off; pressing off then undo leaves it on.

---

## Medium — Text Buffer with Redo

You are given a text buffer receiver:

```ts
class TextBuffer {
  private text = "";
  append(s: string): void { this.text += s; }
  deleteLast(n: number): void { this.text = this.text.slice(0, -n); }
  get value(): string { return this.text; }
}
```

**Task:**
- Write `AppendCommand` (execute appends, undo deletes the same length).
- Write `ReplaceCommand(buffer, newText)` — an operation that clears the buffer and sets new text. Its `undo()` must restore the *previous* text, so you must **capture the old value during `execute()`**.
- Write an `Editor` invoker with `run()`, `undo()`, and `redo()` using a history stack and a redo stack. A fresh `run()` must clear the redo stack.

**Think about:** Why can't `ReplaceCommand.undo()` just read the current text at undo time? What breaks if the buffer changed in between?

---

## Hard — Idempotent Command Queue with Retries

Model a background-job queue that runs commands later, the way BullMQ/SQS would.

**Requirements:**
- A `Command` interface with `execute(): Promise<void>`, a stable `id: string`, and `maxAttempts: number`.
- A `JobQueue` invoker with `enqueue(command)` and `async processAll()`.
- **Idempotency:** if a command with an already-processed `id` is delivered again, skip it (at-least-once delivery means duplicates happen).
- **Retries:** if `execute()` throws, retry up to `maxAttempts`; if it still fails, move the command to a `deadLetter` list instead of crashing the worker.
- Write a `FlakySendEmailCommand` whose `execute()` fails the first attempt and succeeds on the second, to prove retries work.

**Think about:** Where does the idempotency check belong — the command or the queue? What state do you need so a retried command is not counted as a duplicate of itself?

---

## Real-World Challenge — Undoable Ledger Service (NestJS-style)

Build a small ledger service used by an admin panel where operators can reverse mistakes.

```ts
interface Command {
  readonly id: string;
  readonly name: string;
  execute(): Promise<void>;
  undo(): Promise<void>;
  describe(): string;
}
```

**Requirements:**
- A `LedgerAccount` receiver (integer cents, no overdraft, throws on invalid ops).
- Concrete commands: `DepositCommand`, `WithdrawCommand`, and `TransferCommand` (two accounts, all-or-nothing with a compensating action if the second leg fails).
- A `MacroCommand` ("apply monthly fees to all accounts") that executes children in order and, on a mid-batch failure, undoes only the children that ran.
- A `TransactionManager` invoker with `run/undo/redo`, an injected `AuditLog` (Dependency Injection), and a **capped history** (keep only the last 50 commands — dropping the oldest must not break undo of the remaining ones).
- Every executed/undone command must produce an audit entry via the injected log.

**Then prove the decoupling:** write a unit test for `TransactionManager` using a fake `Command` (records whether `execute`/`undo` were called) — with **no `LedgerAccount` involved**. This shows the invoker depends only on the interface.

**Stretch:** Add persistence — serialize each executed command to an append-only in-memory event array (`{id, name, params}`), then write a `replay()` that rebuilds account balances from an empty state by re-executing the log. Note that you serialize *data*, not the live receiver reference.

---

## Bonus Challenge — CommandBus (Command + Mediator) and a Saga

1. **CommandBus.** Build a `CommandBus` that maps a command *type* to a single *handler* and dispatches commands to their handler (the pattern behind `@nestjs/cqrs` and MediatR). Register handlers for `CreateOrderCommand` and `CancelOrderCommand`. The dispatcher must not contain any `switch`/`instanceof` — use a registry (Map from command name to handler).

2. **Saga / compensation.** Implement a `PlaceOrderSaga` as a macro of commands that touch *different services*: `ReserveInventoryCommand`, `ChargePaymentCommand`, `SendConfirmationEmailCommand`. If `ChargePaymentCommand` fails, run the compensating action for `ReserveInventoryCommand` (release the stock). Explain in comments why `SendConfirmationEmailCommand` has **no true undo** and how you handle that (e.g. send a cancellation email as compensation, or mark it non-reversible).

3. Add a brand-new command (`RefundCommand`) and confirm you modified **only** a new file plus one registry line — no existing command, handler, or the bus changed. Write down which SOLID principles this demonstrates.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
