# Access Modifiers — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | TypeScript class feature — member visibility control. |
| **Intent** | Control who can read/write a class member (field, method, accessor) so internal state stays valid and subclassing stays safe. |
| **Problem** | Plain JS object properties are always fully public and mutable — nothing stops any caller from corrupting internal state, sharing too much with subclasses, or reassigning values meant to be fixed after creation. |
| **Solution** | Annotate members with `public` (default) / `protected` / `private` / `readonly`, use constructor **parameter properties** to shorten declarations, and use **get/set** for validated or computed access. Reach for a real `#privateField` when protection must survive at runtime. |
| **Pros** | Compiler-enforced contracts · self-documenting visibility · safe subclassing via `protected` · prevents accidental reassignment via `readonly` · getters/setters add validation/computation without changing call syntax · parameter properties cut boilerplate. |
| **Cons / Gotchas** | `public`/`protected`/`private`/`readonly` are **erased at compile time** — zero runtime enforcement. `private` can be read via bracket notation (`obj["field"]`); `readonly` can be overwritten via `Object.assign(obj, {...})`. Only real `#privateField` is enforced by the JS engine at runtime. |
| **Use When** | Modeling invariants (balances, counters) · sharing internal state with subclasses only · marking IDs/timestamps as fixed-after-creation · exposing computed/validated access via accessors · protecting genuine secrets (use `#field`, not `private`). |
| **Avoid When / Common Mistakes** | Believing `private` is real security (it isn't — use `#field`) · confusing `private` with `protected` when a subclass needs the value · putting `readonly` on state that legitimately changes · hiding required validation inside a parameter-property shorthand · assuming `readonly` == `Object.freeze()` (different mechanisms, different guarantees). |
| **Related Topics** | Encapsulation (OOP) · Getters/Setters (accessor properties) · Immutability (`readonly`, `Object.freeze`, `const`) · Real private class fields (`#field`) · Dependency Injection (parameter properties in NestJS/Angular). |

### TS `private`/`protected`/`readonly` vs Real `#field`

| | TS `private` / `protected` / `readonly` | Real `#privateField` |
|---|---|---|
| Enforced by | TypeScript compiler only | JavaScript engine, at runtime |
| Survives compilation to `.js`? | No — fully erased | Yes — genuinely private in the emitted code |
| Bypassable via bracket notation? | Yes (`obj["field"]` compiles and works) | No — no bracket-notation route exists |
| Bypassable via `Object.assign`/plain JS caller? | Yes (`readonly` in particular) | No |
| Good for | Team discipline, self-documenting APIs, catching accidental misuse | Genuine secrets, invariants that must never be reachable from outside |

### Skeleton
```ts
class Example {
  #secret: string;                 // real JS private field — enforced at runtime

  constructor(
    public readonly id: string,    // public + readonly parameter property
    protected kind: string,        // protected parameter property (subclasses only)
    private balance: number,       // private parameter property (this class only)
    secret: string,                // plain param — no auto field, assign by hand
  ) {
    this.#secret = secret;
  }

  get formatted(): string {         // getter — computed, read-only view
    return `$${this.balance.toFixed(2)}`;
  }

  set label(value: string) {        // setter — validated write
    if (!value.trim()) throw new Error("label required");
    this.balance = this.balance;    // (validation logic goes here)
  }

  deposit(amount: number): void {   // public method — the only sanctioned mutator
    this.balance += amount;
  }
}

class Sub extends Example {
  show() {
    console.log(this.kind);         // OK: protected — visible to subclasses
    // console.log(this.balance);   // Error: private — parent class only
  }
}
```

### Remember In One Sentence
> **`public`/`protected`/`private`/`readonly` are compile-time courtesy checks erased by the time your code runs — only a real `#privateField` is actually locked at runtime.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. List the four TypeScript access modifiers and give a one-line rule for who can access each.
2. What is the constructor "parameter properties" shorthand, and what two things does it do for you in one line?
3. Why does `account.balance = 5` fail to compile when `balance` is `private`, but `account["balance"] = 5`... does it actually fail too, or not? What about just *reading* `account["balance"]`?
4. What actually happens to `private`/`protected`/`readonly` keywords when TypeScript compiles to JavaScript?
5. Name one concrete way to bypass a `readonly` field's protection at runtime, despite `tsc` refusing to compile a direct reassignment.
6. How is a real `#privateField` different from a `private` keyword field, in terms of what enforces it and when?
7. Can a subclass access a `private` member of its parent class? Can it access a `protected` one? Why the difference?
8. What is a getter/setter pair used for that a plain public field cannot provide?
9. Why can't you use the parameter-properties shorthand for a `#privateField`?
10. If you needed to protect a PIN or an API token so that it truly cannot be read from outside the class — even by code that never went through the TypeScript compiler — which mechanism would you use, and why not the alternative?
