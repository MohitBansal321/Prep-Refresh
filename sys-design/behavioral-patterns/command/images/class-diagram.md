# Command Pattern — Class Diagram

Shows the five participants and their relationships. Every ConcreteCommand *implements*
the `Command` interface and *holds a reference to* a Receiver. The Invoker
(`TransactionManager`) depends only on the `Command` interface — never on a concrete
command or on the `Account` receiver. `MacroCommand` is itself a `Command` that
*composes* other commands (Command + Composite).

```mermaid
classDiagram
    class Command {
        <<interface>>
        +name: string
        +id: string
        +execute() Promise~void~
        +undo() Promise~void~
        +describe() string
    }

    class DepositCommand {
        -account: Account
        -amountInCents: number
        +execute() Promise~void~
        +undo() Promise~void~
    }

    class WithdrawCommand {
        -account: Account
        -amountInCents: number
        +execute() Promise~void~
        +undo() Promise~void~
    }

    class TransferCommand {
        -from: Account
        -to: Account
        -amountInCents: number
        +execute() Promise~void~
        +undo() Promise~void~
    }

    class MacroCommand {
        -commands: Command[]
        -executedCount: number
        +execute() Promise~void~
        +undo() Promise~void~
    }

    class Account {
        -balanceInCents: number
        +deposit(amount) void
        +withdraw(amount) void
    }

    class TransactionManager {
        -history: Command[]
        -redoStack: Command[]
        -audit: AuditLog
        +run(command) Promise~void~
        +undo() Promise~boolean~
        +redo() Promise~boolean~
    }

    class CommandQueue {
        -pending: Command[]
        -processedIds: Set~string~
        +enqueue(command) void
        +processAll() Promise~void~
    }

    DepositCommand ..|> Command : implements
    WithdrawCommand ..|> Command : implements
    TransferCommand ..|> Command : implements
    MacroCommand ..|> Command : implements
    MacroCommand o-- Command : composes many
    DepositCommand --> Account : Receiver
    WithdrawCommand --> Account : Receiver
    TransferCommand --> Account : two Receivers
    TransactionManager --> Command : triggers (Invoker)
    CommandQueue --> Command : queues (Invoker)
```

**How to read it**
- `..|>` (dashed, hollow triangle) = *implements interface*. Every concrete command implements `Command`.
- `-->` = *association / holds a reference*. Each command holds its `Account` receiver; the invokers hold `Command`s.
- `o--` = *aggregation / composition of many*. A `MacroCommand` is built from other `Command`s — and because it *is* a `Command`, an invoker treats "run payroll" exactly like a single deposit.
- The two invokers (`TransactionManager`, `CommandQueue`) have **no arrow to `Account`** — that is the whole point: they are decoupled from the receiver and know only the `Command` interface.
