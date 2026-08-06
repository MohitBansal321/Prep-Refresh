# Interfaces and Abstract Classes — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | TypeScript OOP fundamental (language feature, not a design pattern). |
| **Intent** | Describe contracts with the right tool: `interface` for pure shape (no code, no runtime footprint), `abstract class` for a family that also shares real, working code. |
| **Problem** | Some requirements are pure shape that many unrelated classes might satisfy; others need shared, working code across a genuine family of types; a single type may need several unrelated capabilities at once, which single-inheritance classes cannot express alone. |
| **Solution** | Model pure capabilities as small `interface`s (optionally `extends`-ing each other); model a shared family as one `abstract class` with concrete methods plus `abstract` methods for the parts that must differ; let a concrete class `extends` one abstract class and `implements` as many interfaces as it needs. |
| **How it works** | Interfaces are fully erased at compile time — no runtime trace at all. Abstract classes compile to real JS classes; only the "cannot instantiate directly" rule and the `abstract` keyword are compile-time-only, enforced by the type checker, not by the emitted JavaScript. `implements` never generates code — it is a compile-time shape check on the class declaration. |
| **Pros** | Zero-cost pure contracts · one authoritative home for shared code · unlimited interface stacking via `implements` · compiler-enforced completeness (missing abstract methods = compile error) · structural typing available when you don't want a hard name dependency · interfaces compose via `extends`. |
| **Cons / Gotchas** | Interfaces have no runtime representation (`instanceof` cannot check them) · `abstract`'s instantiation guard is compile-time only, not present in emitted JS · `implements` adds zero behavior, easy to mistake for `extends` · silent structural drift if you never declare `implements` · single inheritance still caps you at one abstract/base class. |
| **Use When** | A capability might apply to many unrelated classes (interface) · a genuine family of types should share real code while forcing one differing piece per member (abstract class) · a class needs several independent capabilities plus one lineage at once. |
| **Avoid When / Common Mistakes** | Writing an "abstract class" with no shared implementation at all (should be an interface) · trying to `extends` two abstract classes (not supported — use composition) · putting a method body inside an interface (not allowed — that need is what abstract classes are for) · assuming `implements` changes runtime behavior · assuming `abstract` protects you at runtime. |
| **Related Topics** | Classes & Inheritance (`extends`, `super`, overriding) · Composition over Inheritance · Generics (`Repository<T>`) · Structural Typing / Duck Typing · Access Modifiers · Polymorphism. |

### Interface vs Abstract Class vs Plain Class — Quick Decision

| Question | Answer points to |
|---|---|
| Does it need to share real, working code across a family? | **Abstract class** |
| Is it pure shape that unrelated classes might satisfy? | **Interface** |
| Does a class need many of them at once? | **Interface** (many via `implements`) — not abstract class (only one via `extends`) |
| Everything already has a sensible default, nothing must be forced per-subclass? | **Plain class** |

### What Gets Erased at Compile Time

- **`interface` declarations:** fully erased — zero trace in emitted JavaScript.
- **`implements X` on a class:** erased — it was only ever a compile-time shape check, never runtime code.
- **`abstract class` / `abstract` methods:** the class itself remains a real JS class; only the `abstract` keyword and the "no direct instantiation" rule are compile-time-only.

### Skeleton

```ts
interface Loggable {
  log(message: string): void;
}

interface Auditable extends Loggable {              // interface extending interface
  auditTrailId: string;
  recordAudit(action: string): void;
}

abstract class NotificationChannel implements Loggable {
  protected sentCount = 0;

  constructor(protected readonly channelName: string) {}

  abstract deliver(recipient: string, message: string): Promise<boolean>; // no body — forced

  async send(recipient: string, message: string): Promise<boolean> {     // shared, concrete
    this.log(`Sending via ${this.channelName} to ${recipient}`);
    const ok = await this.deliver(recipient, message);
    if (ok) this.sentCount++;
    return ok;
  }

  log(message: string): void {
    console.log(`[${this.channelName}] ${message}`);
  }
}

class EmailNotificationChannel extends NotificationChannel implements Auditable {
  auditTrailId: string;

  constructor(auditTrailId: string) {
    super("Email");                                   // must run before using `this`
    this.auditTrailId = auditTrailId;
  }

  async deliver(recipient: string, message: string): Promise<boolean> {
    console.log(`Emailing ${recipient}: "${message}"`);
    return true;
  }

  recordAudit(action: string): void {
    console.log(`[Audit ${this.auditTrailId}] ${action}`);
  }
}
```

### Remember In One Sentence
> **An `interface` is a job posting — a list of duties with no office behind it; an `abstract class` is a half-built office with shared desks already wired up, missing only the one seat every occupant must personally fill.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. What happens to an `interface` when TypeScript compiles it to JavaScript? What about `implements X`?
2. Why can a class `implements` many interfaces but `extends` only one class?
3. Can a class satisfy an interface's shape without ever writing `implements`? Name the mechanism that makes this possible.
4. What is the difference between what an `interface` can contain and what an `abstract class` can contain?
5. If you try `new NotificationChannel("X")` directly on an abstract class, what happens — and is that protection still there in the compiled JavaScript?
6. Can one interface `extends` another? Give the example from this file and explain what it forces on implementers.
7. In `EmailNotificationChannel`, why doesn't the class need to write its own `log()` method even though `Auditable` requires one (via `Loggable`)?
8. Give one situation where you should reach for an `interface`, one where you should reach for an `abstract class`, and one where a plain class is enough.
9. What goes wrong if you try to make one class `extends` two different abstract classes at once? What's the usual fix?
10. Why can't you check `value instanceof SomeInterface`, and what would you use instead if you truly needed a runtime check?
