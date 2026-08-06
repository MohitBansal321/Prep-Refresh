# Classes and Inheritance — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | TypeScript OOP fundamental (language feature, not a design pattern). |
| **Intent** | Reuse shared state/behavior via a base class and let subclasses (`extends`) add or specialize only what genuinely differs. |
| **Problem** | Multiple related types need the same fields/methods but a few behaviors (like area calculation) differ per type — duplicating shared logic or branching on a `kind` field both cause maintenance pain. |
| **Solution** | Put shared fields/methods in a base class; each subclass `extends` it, calls `super(...)` in its constructor to initialize the inherited part, and overrides only the methods that truly differ. |
| **How it works** | `super(...)` must run first in a subclass constructor (before `this` is used) → inherited fields get set → subclass sets its own fields → overridden methods use dynamic dispatch, so `this.method()` calls resolve to the actual runtime subclass's version. |
| **Pros** | One authoritative copy of shared logic · polymorphism (base-typed code works for every subclass) · compiler-enforced init order · clear extension point via `super.method()` · natural home for class-wide data (`static`) and locked-down fields (`readonly`). |
| **Cons / Gotchas** | Deep hierarchies get hard to follow · fragile base class problem (base changes ripple to subclasses) · forgetting `super.method()` silently drops parent behavior · single inheritance only (one `extends` per class) · `this` cannot be touched before `super()` runs. |
| **Use When** | Several types share both state and behavior with only a focused part varying · you want the compiler to guarantee a constructor/method shape · you want polymorphism as new variants are added · you have genuinely class-wide data (`static`) or fixed per-instance identity (`readonly`). |
| **Avoid When / Common Mistakes** | "Shared behavior" is really just shared data (use a plain type/interface instead) · need to mix behavior from multiple unrelated sources (use composition/interfaces) · hierarchy would be deep and speculative · assuming every subclass must override every method (it doesn't) · reaching for a `static` member via an instance instead of the class name. |
| **Related Topics** | Interfaces & Abstract Classes (no-implementation vs. forced-implementation contracts) · Composition over Inheritance · Access Modifiers (`public`/`protected`/`private` in depth) · Polymorphism · Static members. |

### `super()` Call Order (the rule that trips up beginners)

- If a subclass declares its own constructor **and** the base class has a constructor, `super(...)` must be the **first statement**, before any use of `this`.
- Forgetting it → TypeScript error: *"'super' must be called before accessing 'this' in the constructor of a derived class."*
- If a subclass declares **no constructor at all**, TypeScript auto-generates one that just calls `super(...args)` with the same arguments — nothing to remember in that case.

### Overriding vs. Overloading

- **Overriding:** a subclass redefines a method with the same name/compatible signature; the version that runs is chosen at runtime by the actual object's class (dynamic dispatch). Use `super.method()` inside an override to run the parent's version too.
- **Overloading:** multiple call signatures declared for the *same* method name in *one* class; TypeScript picks the matching signature at compile time based on argument shapes — nothing to do with subclasses at all.

### Skeleton

```ts
class Shape {
  private static instanceCount = 0;           // static: one copy, on the class
  readonly id: number;                          // readonly: set once, never reassigned

  constructor(protected name: string) {         // protected: visible to subclasses
    this.id = ++Shape.instanceCount;
  }

  area(): number {
    return 0;                                   // meant to be overridden
  }

  describe(): string {
    return `${this.name} (#${this.id}) area=${this.area().toFixed(2)}`;
  }

  static getInstanceCount(): number {
    return Shape.instanceCount;                 // accessed as Shape.getInstanceCount()
  }
}

class Circle extends Shape {
  readonly radius: number;

  constructor(radius: number) {
    super("Circle");                            // must run before using `this`
    this.radius = radius;
  }

  override area(): number {
    return Math.PI * this.radius ** 2;          // full replace
  }

  override describe(): string {
    return `${super.describe()} [radius=${this.radius}]`; // extend, not replace
  }
}
```

### Remember In One Sentence
> **`extends` lets a subclass borrow a base class's fields and methods for free, `super(...)` is how it politely finishes the base class's setup before adding its own, and overriding is how it swaps in different behavior only where it actually needs to.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Why must `super(...)` be called before you use `this` in a subclass constructor? What error does TypeScript give if you forget?
2. Does a subclass have to override every method it inherits? Give an example from the `Shape` hierarchy that proves your answer.
3. What is the difference between overriding a method and overloading a method?
4. When should an override call `super.methodName(...)`, and when should it not?
5. How do you access a `static` member — through the class or through an instance? What happens if you get it backwards?
6. What does `readonly` guarantee, and where is a `readonly` field allowed to be assigned?
7. In one sentence, what does `protected` mean here, and how is it different from `private`?
8. If `Rectangle` never overrides `describe()`, whose `area()` runs when `Shape.describe()` calls `this.area()` on a `Rectangle` instance? Why?
9. Name two situations where you should prefer composition over inheritance instead of reaching for `extends`.
10. What is the "fragile base class" problem, and why does it make deep hierarchies riskier?
