# Builder Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Creational design pattern (GoF). |
| **Intent** | Separate the construction of a complex object from its representation, so the same step-by-step process can produce different, fully-validated objects. |
| **Problem** | Creating an object with many optional/interdependent fields via a constructor gives a **telescoping constructor** — `new X(u, "GET", null, null, 30000, 3, undefined)` — unreadable, order-sensitive, and with no place to validate cross-field rules. |
| **Solution** | Move construction into a separate Builder that accumulates fields one step at a time (fluent `return this`), validates every invariant in a single `build()` gate, and returns a finished, immutable Product. |
| **Participants** | **Product** (complex, immutable result) · **Builder** (interface: the step vocabulary) · **ConcreteBuilder** (holds state + defaults, validates in `build()`) · **Director** (OPTIONAL — a reusable recipe of steps). |
| **Flow** | Create builder → (optional) Director runs a named recipe → add extra steps → `build()` validates required fields + cross-field invariants → constructs & freezes Product → `reset()`s builder → returns immutable Product (or throws). |
| **Pros** | Readable, self-documenting call sites · handles many optional fields with defaults · single validation gate = no invalid product can exist · immutable products · reusable recipes (Director) · same steps → different representations · new fields don't break call sites. |
| **Cons** | More code (extra class[es]) · overkill for simple objects · verbose call sites · state-leak risk if `reset()` forgotten · mutable builder is not concurrency-safe · product and builder must be kept in sync. |
| **Use When** | Many optional/interdependent parameters (HTTP requests, SQL queries, config) · cross-field invariants must be validated together · same process must yield different representations · a construction recipe repeats · you want guaranteed-valid, immutable products. |
| **Avoid When** | Simple objects with few, mostly-required fields · a plain options object already gives readability + defaults + one-shot construction · tiny immutable value objects (`Money`) · you'd add a Director with only one caller (YAGNI). |
| **Real Examples** | Knex / TypeORM / Prisma query builders · NestJS Swagger `DocumentBuilder` · .NET `WebApplicationBuilder` / `StringBuilder` · Java `StringBuilder`, Lombok `@Builder`, Spring `UriComponentsBuilder` · Zod's chained schema (`z.string().min(1).email()`) · Node `URLSearchParams`. |
| **Related Topics** | Factory Method / Abstract Factory (single-call creation of a chosen type) · Prototype (clone a template) · Fluent Interface (the `return this` technique Builders use) · options-object constructor (the simpler alternative). |

### Builder vs Options Object (the key comparison)
A plain `new X({ url, method, ... })` already fixes readability and order-sensitivity — often it is *good enough*. A Builder adds three things it cannot: **incremental/conditional construction** (add a header inside a loop, apply a recipe then tweak), a **single validation gate** that makes an invalid product structurally impossible to obtain, and **reusable recipes** via a Director. If you need none of those, ship the options object.

### Skeleton
```ts
class Product {                              // immutable result
  readonly a: string; readonly b: number;
  constructor(p: { a: string; b: number }) { // builder-only, by convention
    this.a = p.a; this.b = p.b;
    Object.freeze(this);
  }
}

interface Builder {                          // the step vocabulary
  setA(a: string): this;                     // each step returns `this` (fluent)
  setB(b: number): this;
  build(): Product;
  reset(): this;
}

class ConcreteBuilder implements Builder {
  private a?: string;
  private b = 0;                             // default lives in the builder
  setA(a: string) { this.a = a; return this; }
  setB(b: number) { this.b = b; return this; }
  build(): Product {
    if (!this.a) throw new BuildError("a is required");   // ALL validation here
    if (this.b < 0) throw new BuildError("b must be >= 0"); // cross-field too
    const product = new Product({ a: this.a, b: this.b });
    this.reset();                            // reuse-safe
    return product;
  }
  reset() { this.a = undefined; this.b = 0; return this; }
}

class Director {                             // OPTIONAL — a reusable recipe
  constructor(private b: Builder) {}
  standard(a: string) { return this.b.reset().setA(a).setB(10); } // no build()!
}
```

### Remember In One Sentence
> **A Builder is a sandwich counter: you assemble a complex object one clear step at a time, optionally follow a named recipe (Director), and only get the finished, validated, immutable product once `build()` says every rule holds.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the four participants and give each a one-line responsibility. Which one is optional?
2. What exactly is the **telescoping constructor** problem, and how does the Builder solve it?
3. What is a **fluent interface**, and what one line of code makes a step chainable?
4. Where does **all** validation belong — in the setters or in `build()`? Why can cross-field invariants only be checked in `build()`?
5. A colleague says "just pass an options object instead." Name the three things a Builder gives you that an options object does not.
6. Why must the **product be immutable**, and how do you achieve that in TypeScript?
7. What is the **Director's** job, when should you add one, and why does it *not* call `build()`?
8. Why does the ConcreteBuilder `reset()` (often automatically inside `build()`), and what bug appears if you forget?
9. Builder vs Factory — give the one-line difference. (Hint: step-by-step assembly vs single-call creation of a chosen type.)
10. Name two real production query builders that are the Builder pattern applied to SQL, and say where their "build/execute" boundary is.
