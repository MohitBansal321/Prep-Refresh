# Command Pattern — Sequence Diagram

Shows the runtime message exchange for one banking request: the Client builds a
command, the Invoker executes it, and later reverses it via undo.

```mermaid
sequenceDiagram
    autonumber
    participant Cl as Client
    participant Cmd as WithdrawCommand
    participant Inv as TransactionManager (Invoker)
    participant Rec as Account (Receiver)

    Note over Cl,Rec: Build + execute
    Cl->>Cmd: new WithdrawCommand(account, 2000)
    Cl->>Inv: run(command)
    activate Inv
    Inv->>Cmd: execute()
    activate Cmd
    Cmd->>Rec: withdraw(2000)
    Rec-->>Cmd: ok (balance updated)
    deactivate Cmd
    Note over Inv: push to history, clear redo, log
    deactivate Inv

    Note over Cl,Rec: Later — undo
    Cl->>Inv: undo()
    activate Inv
    Inv->>Cmd: undo()
    activate Cmd
    Cmd->>Rec: deposit(2000)
    Rec-->>Cmd: ok (reversed)
    deactivate Cmd
    Note over Inv: move command to redo stack, log
    deactivate Inv
```

**How to read it**
- The **Client** builds a `WithdrawCommand` (binding the receiver + params) and hands it to the Invoker — it never calls the account directly.
- The **Invoker** (`TransactionManager`) only speaks the `Command` interface: it calls `execute()` without knowing what the command does, then records it in history for undo.
- The **Command** delegates the real work to the **Receiver** (`Account`) and captures whatever it needs to reverse the effect later.
- The second block shows **undo**: the Invoker pops the command from history and calls `undo()`, which applies the inverse (`deposit`) on the receiver and moves the command to the redo stack.
