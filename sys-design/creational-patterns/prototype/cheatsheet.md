# Prototype Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Creational design pattern (GoF). |
| **Intent** | Create new objects by *cloning* an existing, fully-configured instance (the prototype) instead of constructing them from scratch — without depending on their concrete class. |
| **Problem** | You need many objects that are almost the same as a known-good template, but building each from scratch is expensive (DB/network/assembly) and/or you only hold an interface reference and cannot name the concrete class. |
| **Solution** | Give objects a `clone()` method that returns a **deep, independent, class-preserving** copy of themselves. Build the master once; create the rest by cloning. Optionally store named masters in a registry that always hands out clones. |
| **Participants** | **Prototype** (interface declaring `clone()`) · **ConcretePrototype** (`EmailCampaignTemplate` — implements `clone()`) · **Client** (`CampaignService` — creates by cloning) · **Registry** (`PrototypeRegistry` — stores masters, returns clones). |
| **Flow** | Build master once → register under key → Client calls `registry.create(key)` → registry calls `prototype.clone()` → `clone()` deep-copies nested state + rebuilds the class → returns fresh independent instance → Client mutates it safely. |
| **Pros** | Cheap creation of expensive objects · avoids a parallel factory/subclass hierarchy · class owns its copy logic (private fields, encapsulation) · decouples creation from concrete class · register/replace prototypes at runtime · great for baseline+diff (templates, fixtures). |
| **Cons** | Deep copy is hard to get right (nested/Map/Set/Date/cycles) · clone cost is not zero (CPU/memory for big graphs) · every field must be maintained in `clone()` · class-identity trap (built-in tools return plain objects) · can bypass constructor invariants. |
| **Use When** | Construction is expensive and you need many near-identical objects · you must copy without knowing the concrete class · you want to avoid a factory/subclass explosion · runtime-configurable creation · a known-good baseline to copy and tweak. |
| **Avoid When** | Construction is cheap and the class is known (just `new`) · objects are immutable/stateless (share one) · object graph is huge and cloned at high volume · you need step-by-step assembly (Builder) · a family of related objects (Abstract Factory) · snapshot-to-restore-same-object (Memento). |
| **Real Examples** | Node.js `structuredClone()` · Spring `prototype` bean scope · Java `Cloneable`/`Object.clone()` · .NET `ICloneable`/`MemberwiseClone()` · NestJS testing modules cloning a base config · React immutable updates (`{...state, x}`) · seed/fixture rows · "Duplicate this record" features. |
| **Related Topics** | Factory Method / Abstract Factory (construct fresh) · Builder (step-by-step assembly) · Memento (snapshot & restore same object) · Singleton (opposite: exactly one) · shallow vs deep copy · JS prototype-based inheritance (different concept, same word). |

### Shallow vs Deep — the #1 bug source
- **Shallow copy** (`Object.assign(new X(), src)`, `{...src}`) — copies top-level fields but **shares every nested reference**. Mutating the clone's `tags`/`metadata` mutates the original's too. This is the classic aliasing bug.
- **Deep copy** — recursively copies nested objects/arrays/`Map`/`Set`/`Date` so the clone shares **nothing**. What a correct `clone()` must produce.

### `structuredClone()` vs `JSON.parse(JSON.stringify())`
- **`JSON.parse(JSON.stringify(obj))`** — lossy and unsafe: drops functions and `undefined`, turns `Date` into a string, turns `Map`/`Set` into `{}`, **throws** on circular references, and returns a plain object (`instanceof` fails, methods gone). Fine only for pure-JSON snapshots.
- **`structuredClone(obj)`** — native (Node 17+), correctly handles `Date`/`Map`/`Set`/typed arrays and **circular references**. Its one limit: it returns a **plain object**, not your class — so use it for the *data*, then rebuild the class in `clone()` to restore identity.

### Preserving class identity & circular refs
- A clone must stay an instance of its class: return `new EmailCampaignTemplate(...)`, feeding deep-copied data through the constructor — never return the raw `structuredClone`/JSON result, or `instanceof` breaks and methods (including `clone()`) vanish.
- Circular references crash naive recursive copies and `JSON`; `structuredClone` handles them.

### GoF Prototype ≠ JavaScript prototypes
- **JS prototype-based inheritance** = the *language* mechanism: `[[Prototype]]`, `Object.create`, the prototype chain — how objects inherit properties.
- **GoF Prototype pattern** = a design pattern: a `clone()` method that copies a configured *instance* to make a new one. Same word, different concept — a favourite interview trap.

### Skeleton
```ts
interface Prototype<T> { clone(): T; }                    // the whole contract

class CampaignTemplate implements Prototype<CampaignTemplate> {
  constructor(
    public name: string,
    public tags: string[],
    public metadata: Map<string, string>,                 // Map + Date + arrays are
    public updatedAt: Date,                                // what naive copies break
  ) {}

  clone(): CampaignTemplate {
    // deep-copy the DATA with structuredClone, then rebuild the CLASS to keep identity
    return new CampaignTemplate(
      this.name,                                           // primitive: by value
      structuredClone(this.tags),                          // new array
      structuredClone(this.metadata),                      // new Map (JSON would empty it)
      structuredClone(this.updatedAt),                     // new Date (JSON would stringify it)
    );
  }
}

class PrototypeRegistry<T extends Prototype<T>> {
  private prototypes = new Map<string, T>();
  register(key: string, p: T) { this.prototypes.set(key, p); }
  create(key: string): T {                                 // ALWAYS returns a clone,
    const p = this.prototypes.get(key);                    // never the stored master
    if (!p) throw new Error(`No prototype "${key}"`);
    return p.clone();
  }
}
```

### Remember In One Sentence
> **Prototype is a cookie cutter: build one master object correctly and expensively once, then stamp out cheap, deep, independent copies of it — each a real instance of the same class, sharing nothing with the original.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the four participants and give each a one-line responsibility.
2. What two things must a *correct* `clone()` always produce? (Hint: one is about references, one is about type.)
3. Shallow vs deep copy — what exactly breaks with a shallow copy, and why is it the #1 Prototype bug?
4. Why is `JSON.parse(JSON.stringify(obj))` the wrong tool for cloning a real domain object? List at least three specific things it loses or breaks.
5. What does `structuredClone()` get right that JSON does not — and what is its one limitation you must work around?
6. How do you preserve class identity in `clone()`, and what breaks if you forget to?
7. How does the GoF Prototype pattern differ from JavaScript's own prototype-based inheritance?
8. What is the job of a `PrototypeRegistry`, and why must `create(key)` return a clone rather than the stored master?
9. Where should `clone()` live and why — could the client just copy the fields itself?
10. When is Prototype the *wrong* choice? Give at least two situations, and name which other pattern fits each instead.
