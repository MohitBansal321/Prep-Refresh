/**
 * COMMAND PATTERN — Production-style TypeScript example
 * -----------------------------------------------------
 * Scenario: A banking / ledger service must apply operations to accounts
 * (deposit, withdraw, transfer). The business requires:
 *   - UNDO / REDO of the last operations (an operator made a mistake).
 *   - QUEUING operations to be executed later / asynchronously (like a job queue).
 *   - GROUPING several operations into one atomic macro (e.g. payroll run).
 *   - An audit LOG of every request, so we can replay or investigate.
 *
 * A plain `account.deposit(100)` method call cannot be undone, queued, logged,
 * or grouped — because the *request itself* is not a first-class thing you can
 * hold, store, or reverse. The Command Pattern turns each request into an OBJECT.
 *
 * Roles in this file:
 *   - Command            -> the interface (execute + undo + metadata)
 *   - Account            -> the Receiver (knows HOW to do the real work)
 *   - DepositCommand ... -> ConcreteCommands (bind Receiver + action + params)
 *   - TransactionManager -> the Invoker (triggers commands, keeps history)
 *   - MacroCommand       -> a Composite command (a command made of commands)
 *   - CommandQueue       -> deferred/async execution with idempotency
 *   - buildDemo()/main() -> the Client + composition root
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. RECEIVER — the object that actually knows how to perform the work.
//    The Command does NOT contain business logic; it delegates to the Receiver.
// =============================================================================

/** A domain error so callers never see raw string throws. */
export class BankingError extends Error {
  constructor(message: string) {
    super(message);
    this.name = "BankingError";
  }
}

/**
 * Receiver. All real mutations live here. Amounts are integer cents to avoid
 * floating-point money bugs. This class has no idea the Command Pattern exists.
 */
export class Account {
  private balanceInCents: number;

  constructor(
    public readonly id: string,
    openingBalanceInCents = 0,
  ) {
    this.balanceInCents = openingBalanceInCents;
  }

  get balance(): number {
    return this.balanceInCents;
  }

  deposit(amountInCents: number): void {
    if (amountInCents <= 0) throw new BankingError("Deposit must be positive");
    this.balanceInCents += amountInCents;
  }

  /** Returns nothing; throws if funds are insufficient (keeps invariants). */
  withdraw(amountInCents: number): void {
    if (amountInCents <= 0) throw new BankingError("Withdrawal must be positive");
    if (amountInCents > this.balanceInCents) {
      throw new BankingError(
        `Insufficient funds in ${this.id}: have ${this.balanceInCents}, need ${amountInCents}`,
      );
    }
    this.balanceInCents -= amountInCents;
  }
}

// =============================================================================
// 2. COMMAND — the interface. Encapsulates a request as an object.
//    execute() performs it; undo() reverses it; metadata makes it loggable.
// =============================================================================

export interface Command {
  /** Human/audit-friendly name, e.g. "Deposit". */
  readonly name: string;

  /** Stable identity — used for idempotency in the queue and for audit logs. */
  readonly id: string;

  /** Perform the request. */
  execute(): Promise<void>;

  /** Reverse the effects of execute(). Must be safe to call only after execute(). */
  undo(): Promise<void>;

  /** A serializable snapshot for logging / persistence / replay. */
  describe(): string;
}

/** Tiny helper so every command gets a unique id without extra dependencies. */
let sequence = 0;
function nextId(prefix: string): string {
  sequence += 1;
  return `${prefix}-${sequence}`;
}

// =============================================================================
// 3. CONCRETE COMMANDS — each binds a Receiver, an action, and parameters.
//    Notice how undo() captures whatever state it needs to reverse cleanly.
// =============================================================================

export class DepositCommand implements Command {
  readonly name = "Deposit";
  readonly id = nextId("dep");

  constructor(
    private readonly account: Account,
    private readonly amountInCents: number,
  ) {}

  async execute(): Promise<void> {
    this.account.deposit(this.amountInCents);
  }

  async undo(): Promise<void> {
    // The inverse of a deposit is a withdrawal of the same amount.
    this.account.withdraw(this.amountInCents);
  }

  describe(): string {
    return `${this.name}(account=${this.account.id}, amount=${this.amountInCents})`;
  }
}

export class WithdrawCommand implements Command {
  readonly name = "Withdraw";
  readonly id = nextId("wdr");

  constructor(
    private readonly account: Account,
    private readonly amountInCents: number,
  ) {}

  async execute(): Promise<void> {
    this.account.withdraw(this.amountInCents);
  }

  async undo(): Promise<void> {
    // Reversing a withdrawal means depositing the money back.
    this.account.deposit(this.amountInCents);
  }

  describe(): string {
    return `${this.name}(account=${this.account.id}, amount=${this.amountInCents})`;
  }
}

/**
 * A command that spans two receivers. Undo must reverse BOTH sides and, if the
 * second leg fails, roll the first leg back so we never leave money in limbo.
 */
export class TransferCommand implements Command {
  readonly name = "Transfer";
  readonly id = nextId("trf");

  constructor(
    private readonly from: Account,
    private readonly to: Account,
    private readonly amountInCents: number,
  ) {}

  async execute(): Promise<void> {
    this.from.withdraw(this.amountInCents);
    try {
      this.to.deposit(this.amountInCents);
    } catch (err) {
      // Compensating action: put the money back so execute() is all-or-nothing.
      this.from.deposit(this.amountInCents);
      throw err;
    }
  }

  async undo(): Promise<void> {
    this.to.withdraw(this.amountInCents);
    this.from.deposit(this.amountInCents);
  }

  describe(): string {
    return `${this.name}(from=${this.from.id}, to=${this.to.id}, amount=${this.amountInCents})`;
  }
}

// =============================================================================
// 4. MACRO / COMPOSITE COMMAND — a command built out of other commands.
//    Executes them in order; undo() reverses them in REVERSE order.
// =============================================================================

export class MacroCommand implements Command {
  readonly name = "Macro";
  readonly id = nextId("mac");

  /** Tracks how many children ran, so a partial failure undoes only those. */
  private executedCount = 0;

  constructor(
    private readonly label: string,
    private readonly commands: Command[],
  ) {}

  async execute(): Promise<void> {
    this.executedCount = 0;
    for (const command of this.commands) {
      await command.execute();
      this.executedCount += 1;
    }
  }

  async undo(): Promise<void> {
    // Undo only what actually executed, and do it in reverse order.
    for (let i = this.executedCount - 1; i >= 0; i -= 1) {
      await this.commands[i].undo();
    }
  }

  describe(): string {
    return `${this.name}[${this.label}]{${this.commands.map((c) => c.describe()).join("; ")}}`;
  }
}

// =============================================================================
// 5. INVOKER — triggers commands and owns the undo/redo history.
//    The Invoker knows NOTHING about accounts, deposits, or banking. It only
//    knows the Command interface. That decoupling is the whole point.
// =============================================================================

export interface AuditLog {
  record(entry: string): void;
}

/** A trivial console-backed audit log (inject a DB-backed one in production). */
export class ConsoleAuditLog implements AuditLog {
  record(entry: string): void {
    console.log(`[audit] ${entry}`);
  }
}

export class TransactionManager {
  private readonly history: Command[] = [];
  private readonly redoStack: Command[] = [];

  // Dependency Injection: the log is a collaborator, not a hard-coded global.
  constructor(private readonly audit: AuditLog) {}

  async run(command: Command): Promise<void> {
    await command.execute(); // may throw — if it does, nothing is recorded
    this.history.push(command);
    this.redoStack.length = 0; // a fresh action invalidates the redo branch
    this.audit.record(`EXEC  ${command.describe()}`);
  }

  async undo(): Promise<boolean> {
    const command = this.history.pop();
    if (!command) {
      this.audit.record("UNDO  (nothing to undo)");
      return false;
    }
    await command.undo();
    this.redoStack.push(command);
    this.audit.record(`UNDO  ${command.describe()}`);
    return true;
  }

  async redo(): Promise<boolean> {
    const command = this.redoStack.pop();
    if (!command) {
      this.audit.record("REDO  (nothing to redo)");
      return false;
    }
    await command.execute();
    this.history.push(command);
    this.audit.record(`REDO  ${command.describe()}`);
    return true;
  }

  /** Read-only view of what has happened, e.g. for a UI history panel. */
  get log(): readonly string[] {
    return this.history.map((c) => c.describe());
  }
}

// =============================================================================
// 6. COMMAND QUEUE — deferred / asynchronous execution (a mini job queue).
//    This is the same idea BullMQ/SQS give you: enqueue a request now, run it
//    later, possibly on another worker. Because a Command is a self-contained
//    object, it is trivial to store and dispatch. We add IDEMPOTENCY so a
//    command delivered twice (queues love to do that) only runs once.
// =============================================================================

export class CommandQueue {
  private readonly pending: Command[] = [];
  private readonly processedIds = new Set<string>();

  constructor(private readonly audit: AuditLog) {}

  enqueue(command: Command): void {
    this.pending.push(command);
    this.audit.record(`QUEUE ${command.describe()}`);
  }

  /** A worker draining the queue — one command at a time, in order. */
  async processAll(): Promise<void> {
    while (this.pending.length > 0) {
      const command = this.pending.shift() as Command;

      // Idempotency guard: at-least-once delivery means we may see a job twice.
      if (this.processedIds.has(command.id)) {
        this.audit.record(`SKIP  duplicate ${command.id}`);
        continue;
      }

      try {
        await command.execute();
        this.processedIds.add(command.id);
        this.audit.record(`DONE  ${command.describe()}`);
      } catch (err) {
        // In a real queue this would go to a dead-letter queue / retry policy.
        this.audit.record(
          `FAIL  ${command.describe()} :: ${(err as Error).message}`,
        );
      }
    }
  }
}

// =============================================================================
// 7. CLIENT / COMPOSITION ROOT — creates receivers, configures commands, and
//    hands them to invokers. The client wires objects together; it does not
//    contain the business rules (those live in the receiver).
// =============================================================================

async function main(): Promise<void> {
  const audit = new ConsoleAuditLog();
  const alice = new Account("ACC-ALICE", 10_000); // $100.00
  const bob = new Account("ACC-BOB", 0);

  // ---- Undo / Redo via the Invoker's history --------------------------------
  console.log("=== Undo / Redo demo ===");
  const manager = new TransactionManager(audit);

  await manager.run(new DepositCommand(alice, 5_000)); // 10000 -> 15000
  await manager.run(new WithdrawCommand(alice, 2_000)); // 15000 -> 13000
  console.log(`Alice balance: ${alice.balance} (expect 13000)`);

  await manager.undo(); // reverse the withdrawal -> 15000
  console.log(`After undo:    ${alice.balance} (expect 15000)`);

  await manager.redo(); // re-apply the withdrawal -> 13000
  console.log(`After redo:    ${alice.balance} (expect 13000)`);

  // ---- Macro command: a "payroll run" as one undoable unit ------------------
  console.log("\n=== Macro command demo ===");
  const payroll = new MacroCommand("payroll", [
    new TransferCommand(alice, bob, 3_000),
    new DepositCommand(bob, 1_000), // a bonus
  ]);
  await manager.run(payroll);
  console.log(`Alice: ${alice.balance} (expect 10000), Bob: ${bob.balance} (expect 4000)`);

  await manager.undo(); // reverses the whole macro in reverse order
  console.log(`After undo -> Alice: ${alice.balance} (expect 13000), Bob: ${bob.balance} (expect 0)`);

  // ---- Command queue: enqueue now, execute later, idempotently --------------
  console.log("\n=== Command queue demo ===");
  const queue = new CommandQueue(audit);
  const deposit = new DepositCommand(bob, 500);
  queue.enqueue(deposit);
  queue.enqueue(deposit); // same object => same id => processed once
  queue.enqueue(new WithdrawCommand(bob, 9_999_999)); // will FAIL, not crash queue
  await queue.processAll();
  console.log(`Bob after queue: ${bob.balance} (expect 500)`);
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
