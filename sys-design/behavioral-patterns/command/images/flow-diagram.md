# Command Pattern — Flow Diagram

Step-by-step control flow of a request, covering both the immediate (undo/redo) path
and the deferred (queue + idempotency) path.

```mermaid
flowchart TD
    Start([Client wants an operation done]) --> Build["Client builds a ConcreteCommand<br/>(receiver + action + params)"]
    Build --> Give["Hand command to an Invoker"]
    Give --> Decide{Run now or later?}

    Decide -- Now --> Exec["Invoker calls command.execute()"]
    Decide -- Later --> Queue["Invoker enqueues command"]
    Queue --> Worker["Worker drains queue (one at a time)"]
    Worker --> Dup{Already processed?<br/>(idempotency key = command.id)}
    Dup -- Yes --> Skip["Skip duplicate"]
    Dup -- No --> Exec

    Exec --> Ok{execute() succeeded?}
    Ok -- No --> Fail["Do NOT record in history<br/>(queue: retry or dead-letter)"]
    Ok -- Yes --> Work["Command delegates to Receiver<br/>(real work + state change)"]
    Work --> Record["Invoker records in history + writes audit log<br/>(clears redo branch)"]

    Record --> UndoQ{Undo requested?}
    UndoQ -- No --> End([Done])
    UndoQ -- Yes --> Undo["Pop history → command.undo()<br/>→ Receiver reverses (uses captured 'before' state)<br/>→ push onto redo stack"]
    Undo --> RedoQ{Redo requested?}
    RedoQ -- Yes --> Redo["Pop redo stack → command.execute() again<br/>→ push back to history"]
    RedoQ -- No --> End
    Redo --> End

    Skip --> End
    Fail --> End
```

**Key idea:** the Invoker never inspects *what* the command does — it only calls
`execute()`/`undo()` and manages cross-cutting concerns (history, redo, logging,
queuing, idempotency). Swapping in a new operation means adding a new command class;
none of the boxes in this diagram change. Note the two safety rules baked in here:
**record only after a successful `execute()`**, and **dedupe by command id** on the
deferred path so at-least-once delivery cannot double-apply a request.
