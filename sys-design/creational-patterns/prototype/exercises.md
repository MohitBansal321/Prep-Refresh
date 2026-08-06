# Prototype Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build two reflexes: (1) reaching for `clone()` when construction is expensive or you need many near-copies, and (2) instinctively getting **deep vs shallow copy** right.

> Rule of thumb for every exercise: a correct `clone()` must be **deep** (the copy shares NO nested references with the original) and **class-preserving** (`clone() instanceof YourClass` is `true` and its methods still work). Prefer `structuredClone` for data; avoid `JSON.parse(JSON.stringify())` for real domain objects.

---

## Easy — Cloneable Point (and prove independence)

You have a simple class:

```ts
class Point {
  constructor(public x: number, public y: number) {}
}
```

**Task:** Add a `Prototype` interface with `clone(): Point`, make `Point` implement it, and write `clone()` so it returns a new `Point` with the same coordinates.

**Acceptance:**
- `const b = a.clone();` produces a new object.
- `b instanceof Point` is `true`.
- Mutating `b.x` does **not** change `a.x`.

**Think about:** why do primitives (`number`) make this the "easy" case? What would change if `Point` held an array of coordinates instead?

---

## Medium — Deep vs Shallow: Break It, Then Fix It

You have a template with nested state:

```ts
class UserProfile {
  constructor(
    public name: string,
    public roles: string[],
    public settings: { theme: string; notifications: boolean },
  ) {}
}
```

**Task:**
1. First write a **deliberately wrong** shallow `clone()` using `Object.assign` / spread. Demonstrate the bug: clone the profile, `push` to the clone's `roles`, and show the original's `roles` changed too.
2. Now write a **correct** deep `clone()` so that mutating the clone's `roles` or `settings` leaves the original untouched.
3. Prove both behaviours with `console.log` assertions.

**Bonus constraint:** implement the correct version **twice** — once with manual deep copy (`[...roles]`, `{...settings}`) and once with `structuredClone` — and note when the manual approach stops scaling.

---

## Hard — Prototype Registry with Correct Data Types

Build a `PrototypeRegistry<T>` and a `Document` prototype that contains data types the naive `JSON` copy destroys:

```ts
interface Prototype<T> { clone(): T; }

class Document implements Prototype<Document> {
  // must include ALL of these:
  title: string;
  createdAt: Date;                 // JSON would stringify this
  labels: Set<string>;             // JSON would turn this into {}
  properties: Map<string, unknown>;// JSON would turn this into {}
  sections: { heading: string; body: string }[];
  // ...constructor + clone()
}
```

**Task:**
- Implement `clone()` so the copy is deep AND preserves `Date`, `Set`, `Map`, and class identity.
- Implement `PrototypeRegistry<T extends Prototype<T>>` with `register(key, proto)` and `create(key)` that returns a **clone**, never the stored master.
- Register a `"default-report"` document, create two clones, mutate each differently, and prove the stored master and the two clones are all independent.

**Think about:** Where should the deep copy of a `Date`/`Map`/`Set` happen — and why does `JSON.parse(JSON.stringify())` fail every one of them? What is the ONE thing `structuredClone` cannot do for you, and how do you compensate?

---

## Real-World Challenge — "Duplicate This" for a Multi-Tenant SaaS

You are building a "Duplicate" button for saved reports in a NestJS service. A `Report` is expensive to build from scratch (it aggregates default widgets, a data-source config loaded from Postgres, and feature flags from Redis).

```ts
interface Report {
  id: string;
  ownerId: string;
  createdAt: Date;
  widgets: Widget[];           // array of nested objects
  dataSource: DataSourceConfig;// nested object
  featureFlags: Map<string, boolean>;
}
```

**Requirements:**
- Build the tenant's base report **once** (simulate the DB/Redis assembly) and register it in a `PrototypeRegistry`.
- Implement `ReportService.duplicate(sourceReportId, newOwnerId)` that **clones** the report, then:
  - assigns a **new `id`** (a clone must not reuse the source's identity),
  - sets `ownerId = newOwnerId` and `createdAt = new Date()`,
  - leaves the source report and every other clone completely unaffected.
- Selection/creation of prototypes happens only at a composition root.
- Write at least one unit test proving that duplicating a report and mutating the copy's `widgets` array does **not** mutate the source's `widgets`.

**Stretch:** Add a `deepEquals(a, b)` test helper and assert that immediately after `clone()` the copy is *value-equal* to the source but *reference-distinct* on every nested field (`a.widgets !== b.widgets`, `a.dataSource !== b.dataSource`, etc.).

---

## Bonus Challenge — Cycles, Performance, and Copy-on-Write

1. **Circular references.** Create an object graph with a cycle (`a.partner = b; b.partner = a;`). Show that a hand-rolled recursive deep copy (or `JSON.parse(JSON.stringify())`) fails, then show `structuredClone` handles it. Explain *why* in a comment.

2. **Preserve class identity across a graph.** `structuredClone` returns plain objects. Design a `clone()` for a class whose nested field is *also* a class instance (e.g. `Invoice` holds a `Customer`), such that after cloning, BOTH `clone instanceof Invoice` and `clone.customer instanceof Customer` are true. (Hint: delegate — the nested class implements `Prototype` too.)

3. **Measure the cost.** Build a large prototype (e.g. 10,000 nested items). Benchmark `clone()` vs rebuilding from scratch. Then implement a **copy-on-write** variant that shares immutable parts and only copies on mutation. Write down when deep cloning is the wrong choice and copy-on-write (or just sharing an immutable object) wins.

4. **Language vs pattern.** In a short comment, explain the difference between JavaScript's built-in prototype mechanism (`Object.create`, `[[Prototype]]`, the prototype chain) and the GoF Prototype *pattern* (`clone()`). Give one sentence on why sharing the name causes confusion.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
