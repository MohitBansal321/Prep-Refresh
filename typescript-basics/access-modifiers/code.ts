/**
 * ACCESS MODIFIERS — Production-style TypeScript example
 * -------------------------------------------------------
 * Scenario: Model a bank account that must guard its balance and PIN from
 * careless or malicious tampering, while still letting subclasses (like a
 * savings account) read certain internal details a stranger should never see.
 *
 * We use TypeScript's access modifiers to say exactly who may touch what:
 *   - accountId   -> public, readonly    (everyone can read it, nobody can change it)
 *   - accountType -> protected           (this class + subclasses only)
 *   - balance     -> private             (this class only — but ERASED at compile time!)
 *   - #pin        -> real JS private field (this class only — ENFORCED at runtime)
 *   - formattedBalance / ownerName -> getter/setter (controlled, validated access)
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. THE BASE CLASS — demonstrates public, protected, private, readonly and
//    the constructor "parameter properties" shorthand, all in one place.
// =============================================================================

export class BankAccount {
  /**
   * A REAL JS private field (note the `#`, not the `private` keyword). This is
   * NOT a TypeScript feature — it is native JavaScript syntax, enforced by the
   * JS engine itself, at runtime, even after compilation to plain .js.
   * NOTE: TypeScript's "parameter properties" shorthand does NOT support `#`
   * fields, so `#pin` must be declared here and assigned by hand in the body.
   */
  #pin: string;

  // Backing field for the ownerName getter/setter pair (see section 3 below).
  // A getter/setter pair cannot share its own name with a plain field, so we
  // give the real storage a different name and expose `ownerName` through
  // the accessor pair instead.
  private _ownerName: string;

  constructor(
    // public + readonly parameter property: TypeScript auto-creates
    // `this.accountId = accountId` for us AND makes it assignable only here,
    // in the constructor. After this line runs, accountId can never change.
    public readonly accountId: string,

    // protected parameter property: visible to BankAccount AND any subclass
    // (e.g. SavingsAccount below), but invisible to outside code.
    protected accountType: "checking" | "savings",

    // private parameter property: visible ONLY inside BankAccount itself.
    // Subclasses cannot touch this directly — they must go through a method.
    private balance: number,

    // Plain parameters (no modifier keyword) do NOT become fields
    // automatically — that's exactly why `#pin` and `_ownerName` had to be
    // declared above and are assigned by hand in the constructor body.
    pin: string,
    ownerName: string,
  ) {
    this.#pin = pin;
    this._ownerName = ownerName;
  }

  // ===========================================================================
  // 2. GETTERS — expose a computed / restricted view instead of a raw field.
  // ===========================================================================

  /**
   * Public getter. Callers never see the raw `balance` number through this
   * API — only a formatted string. The internal representation (number,
   * Decimal, BigInt cents...) can change later without breaking any caller.
   */
  get formattedBalance(): string {
    return `₹${this.balance.toFixed(2)}`;
  }

  /**
   * A PROTECTED getter. Only BankAccount and its subclasses may read the raw
   * numeric balance for calculations (e.g. interest). Outside code still only
   * ever sees `formattedBalance`.
   */
  protected get currentBalance(): number {
    return this.balance;
  }

  // ===========================================================================
  // 3. GETTER + SETTER PAIR — validated, controlled access to a field.
  //    A setter is where you put validation a plain public field could never
  //    enforce: nothing stops `account.ownerName = ""` on a bare public
  //    field, but a setter can reject it before it ever reaches storage.
  // ===========================================================================

  get ownerName(): string {
    return this._ownerName;
  }

  set ownerName(newName: string) {
    const trimmed = newName.trim();
    if (trimmed.length === 0) {
      throw new Error("Owner name cannot be empty");
    }
    this._ownerName = trimmed;
  }

  // ===========================================================================
  // 4. PUBLIC METHODS — the only sanctioned way for outside code to move the
  //    private `balance`. This is the encapsulation payoff: balance can only
  //    ever change through these two validated paths.
  // ===========================================================================

  deposit(amount: number): void {
    if (amount <= 0) throw new Error("Deposit amount must be positive");
    this.balance += amount;
  }

  withdraw(amount: number): void {
    if (amount <= 0) throw new Error("Withdrawal amount must be positive");
    if (amount > this.balance) throw new Error("Insufficient funds");
    this.balance -= amount;
  }

  /** Verifies a PIN without ever exposing the PIN itself to the caller. */
  verifyPin(candidate: string): boolean {
    return candidate === this.#pin;
  }
}

// =============================================================================
// 5. SUBCLASS — shows exactly what `protected` buys you: SavingsAccount can
//    read/use `accountType` and the protected `currentBalance` getter, but it
//    still cannot touch the private `balance` field directly — only `deposit`
//    and `withdraw` (inherited public methods) may move it.
// =============================================================================

export class SavingsAccount extends BankAccount {
  constructor(
    accountId: string,
    balance: number,
    pin: string,
    ownerName: string,
    protected interestRate: number, // protected parameter property, own to the subclass
  ) {
    super(accountId, "savings", balance, pin, ownerName);
  }

  applyMonthlyInterest(): void {
    // OK: `accountType` is `protected` on the parent, so subclasses may read it.
    // OK: `currentBalance` is a `protected` getter, so subclasses may read it too.
    // NOT OK (would NOT compile): `this.balance` directly — balance is
    // `private` to BankAccount, and `private` does NOT extend to subclasses
    // (only `protected` does). Uncomment the next line to see the error:
    // const raw = this.balance;
    //             ^ Error: Property 'balance' is private and only accessible
    //               within class 'BankAccount'.
    const interest = this.currentBalance * this.interestRate;

    console.log(
      `[SavingsAccount] Applying ${(this.interestRate * 100).toFixed(1)}% ` +
        `interest to ${this.accountType} account ${this.accountId}: +₹${interest.toFixed(2)}`,
    );

    // Must go through the inherited public method — even a subclass cannot
    // mutate `balance` directly, because `private` means "this exact class only."
    this.deposit(interest);
  }
}

// =============================================================================
// 6. DEMO — what compiles fine vs. what TypeScript rejects at compile time,
//    and how that differs from the ONE thing that is truly enforced at runtime.
// =============================================================================

function main(): void {
  const account = new BankAccount("ACC-1001", "checking", 5000, "4321", "Asha Rao");

  // --- Things that compile and run fine -------------------------------------
  account.deposit(1500);
  account.withdraw(200);
  console.log("Balance:", account.formattedBalance); // uses the getter
  console.log("Account ID:", account.accountId); // public + readonly — reading is always fine
  console.log("PIN check:", account.verifyPin("4321")); // true, only via the public method

  account.ownerName = "  Asha K. Rao  "; // uses the setter (trims + validates)
  console.log("Owner:", account.ownerName); // uses the getter

  const savings = new SavingsAccount("ACC-2002", 10000, "7788", "Vikram Shah", 0.04);
  savings.applyMonthlyInterest(); // subclass legally uses the protected members
  console.log("Savings balance:", savings.formattedBalance);

  // --- Things TypeScript REJECTS at compile time (uncomment to see for yourself) ---
  // account.balance = 999999;
  //   ^ Error TS2341: Property 'balance' is private and only accessible
  //     within class 'BankAccount'.
  // console.log(account.accountType);
  //   ^ Error TS2445: Property 'accountType' is protected and only accessible
  //     within class 'BankAccount' and its subclasses.
  // account.accountId = "ACC-9999";
  //   ^ Error TS2540: Cannot assign to 'accountId' because it is a read-only property.
  // console.log(account.#pin);
  //   ^ Error: Property '#pin' is not accessible outside class 'BankAccount'
  //     because it has a private identifier. (Enforced by JS syntax itself,
  //     not just the type-checker — this line would fail even in plain JS.)

  // --- THE GOTCHA: TS `private` / `protected` / `readonly` are COMPILE-TIME
  //     ONLY. They are erased when compiled to plain JavaScript. -------------

  // 1) `private` can be bypassed with bracket notation, and TypeScript itself
  //    happily compiles it — dot-notation access is checked, index-notation
  //    is not:
  console.log("Bypassed private via bracket notation:", account["balance"]);
  // At runtime, in the compiled .js file, `balance` is nothing more than a
  // normal property sitting on the object (`this.balance = 5000`). Open the
  // compiled output in devtools and you can read or overwrite it freely.
  // There is no real security here — only a helpful compile-time reminder
  // for other TypeScript developers who go through the type-checker.

  // 2) `readonly` blocks direct dot/bracket assignment at compile time, but
  //    it is just as erased at runtime — anything that mutates the object
  //    WITHOUT going through a checked assignment (e.g. Object.assign, a
  //    spread + rebuild, or a plain .js caller that never ran through tsc)
  //    changes it without complaint:
  Object.assign(account, { accountId: "ACC-HACKED" });
  console.log("readonly bypassed via Object.assign:", account.accountId);

  // 3) Contrast that with the REAL `#pin` private field. There is NO
  //    bracket-notation or Object.assign workaround for it — accessing
  //    `#pin` from outside the class is not even valid JavaScript syntax,
  //    so this simply cannot compile, let alone run:
  // console.log(account["#pin"]);
  //   ^ still fails: '#pin' can only be referenced from within the class
  //     that declares it. This is enforced by the JS engine, not tsc.

  console.log(
    "\nTakeaway: `private`/`protected`/`readonly` are TypeScript courtesy " +
      "checks for other TypeScript code — real encapsulation in JavaScript " +
      "comes only from `#privateFields` (or closures).",
  );
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main();
}
