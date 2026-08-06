# Strategy Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Behavioral design pattern (GoF). |
| **Intent** | Define a family of algorithms, encapsulate each one, and make them interchangeable so the algorithm can vary independently from the code that uses it. |
| **Problem** | The same task has many variants of *how* it is done, chosen at runtime, and the set keeps growing — leading to an ever-expanding `if/else`/`switch` that selects behavior. |
| **Solution** | Pull each algorithm into its own object/function behind a shared interface; a Context *holds one* and *delegates* to it; a registry/factory *selects* which one. |
| **Participants** | **Strategy** (interface) · **Concrete Strategy** (each algorithm) · **Context** (holds a strategy, delegates) · **Registry/Factory** (selects the strategy) · **Client** (wires & triggers). |
| **Flow** | Client → registry.resolve(key) → Concrete Strategy → inject into Context → Context.doWork() → Context delegates `strategy.run(input)` → strategy runs its algorithm → returns result. |
| **Pros** | Kills behavior-selecting conditionals · OCP (new algorithm = new class/fn) · runtime swappability · SRP · each variant tested in isolation · reuse · composable. |
| **Cons** | More classes/objects · indirection (Context → strategy) · selection logic doesn't vanish, it moves to the registry · designing one interface that fits all algorithms can be hard. |
| **Use When** | Several variants of one task, likely to grow (shipping, discounts, tax, compression, ranking, hashing) · algorithm chosen at runtime (user input, config, feature flag, tenant) · A/B testing algorithms · you fear a big behavior `switch`. |
| **Avoid When** | Only one algorithm and no realistic second (YAGNI) · two/three frozen cases (plain `if` is clearer) · the variation is pure *data*, not behavior (config map of values) · it's really lifecycle transitions (State) · fixed skeleton with a couple of varying steps and compile-time choice is fine (Template Method). |
| **Real Examples** | `Array.prototype.sort(comparator)` · Node `zlib` codecs · Passport.js / NestJS `passport-*` strategies · Spring `Comparator`/`AuthenticationProvider` · DB join strategies (hash/merge/nested-loop) · password hashing (`bcrypt`/`argon2`) · RAG chunking & retrieval strategies. |
| **Related Topics** | State (same structure, different intent — lifecycle transitions) · Template Method (inheritance, fixed skeleton) · Command (encapsulate an action) · Factory/Registry (creates & selects the Strategy) · Adapter (interface compatibility, not choosing algorithms) · Open/Closed Principle · composition over inheritance. |

### Strategy vs State vs Template Method
- **Strategy vs State** — *structurally identical* (a context delegating to an interface), *intent differs*: Strategy holds interchangeable algorithms the **client** picks, unaware of each other, usually set once; State models a **lifecycle** where the object transitions between states that know about and trigger one another.
- **Strategy vs Template Method** — Strategy varies the **whole algorithm** via **composition** at **runtime** (swap a field); Template Method varies **some steps** via **inheritance** at **compile time** (pick a subclass, fixed skeleton).

### Strategies don't have to be classes
- A strategy can be a **plain function**; a `Record<Key, Fn>` map is the idiomatic TS realization.
- Reach for **classes** when you need injected configuration, multiple related methods, or DI.
- Selection goes in a **registry/factory/map** so the growing option set never becomes a `switch` back inside the Context (that would just move the problem back where it started).

### Skeleton
```ts
interface Strategy { run(input: In): Out; }              // the family's contract

class AlgoA implements Strategy { run(i: In): Out { /* one algorithm */ } }
class AlgoB implements Strategy { run(i: In): Out { /* another algorithm */ } }

class Context {                                          // holds & delegates ONLY
  constructor(private strategy: Strategy) {}
  setStrategy(s: Strategy): void { this.strategy = s; }  // runtime swap
  doWork(i: In): Out { return this.strategy.run(i); }    // no if/switch here
}

// Selection lives OUTSIDE the Context — this is where the "switch" is contained.
const registry: Record<string, Strategy> = { a: new AlgoA(), b: new AlgoB() };
const strategy = registry[key] ?? throwUnknown(key);
new Context(strategy).doWork(input);

// Function form (idiomatic when stateless, no DI):
type StrategyFn = (input: In) => Out;
const fns: Record<string, StrategyFn> = { a: (i) => /*...*/, b: (i) => /*...*/ };
```

### Remember In One Sentence
> **A Strategy is a swappable algorithm behind a shared interface: the Context holds one and delegates, and a registry picks which one — so you add behavior by adding a class, never by editing a `switch`.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the participants (Strategy, Concrete Strategy, Context, Registry/Factory, Client) and give each a one-line responsibility.
2. What is the single rule the Context must obey? (Hint: what must it *never* contain?)
3. **Strategy vs State** — same structure, so what is the actual difference in *intent*? Who chooses the object in each, and do the objects know about each other?
4. **Strategy vs Template Method** — which uses composition and which uses inheritance? Which selects at runtime vs compile time?
5. Do strategies have to be classes? When is a plain function (or a map of functions) the more idiomatic TS choice, and when do you still reach for a class?
6. Where does the selection logic live, and what goes wrong if you put it inside the Context?
7. Why should strategies be stateless, and what concrete bug appears in a Node server if a shared strategy stores per-request data in a field?
8. How do you add a brand-new algorithm, and which existing code changes? Which principle does that demonstrate?
9. What is the risk if you design the Strategy interface after looking at only one algorithm? How do you avoid a "leaky" interface?
10. When is Strategy the *wrong* choice? (Give at least two situations — e.g. pure-data variation, two frozen cases, lifecycle transitions.)
