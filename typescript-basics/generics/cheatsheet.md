# Generics — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Core TypeScript type-system feature. |
| **Intent** | Write one function/interface/class that works correctly across many types, using a type-parameter placeholder filled in at each call site or instantiation. |
| **Problem** | Logic must be reused across unrelated types, but `any` discards type safety and one class/function per type duplicates behaviour forever. |
| **Solution** | Introduce a type parameter (`<T>`) wherever a concrete type would go; the compiler substitutes the real type per call site and checks each instantiation independently. |
| **Participants** | **Type Parameter** (`T`, the placeholder) · **Constraint** (`extends`, the upper bound) · **Concrete Type** (`User`, `Product`, supplied at use) · **Generic Definition** (function/interface/class, written once). |
| **Pros** | Reuse without losing type safety · preserves the relationship between input/output types · constraints grant minimal needed permissions · defaults cut boilerplate · zero runtime cost (fully erased). |
| **Cons / Gotchas** | More upfront syntax to read · inference sometimes picks the wrong type · easy to over-generalize a single-use type · `T` does not exist at runtime (`new T()` is not valid) · dense syntax with many params/constraints/defaults. |
| **Use When** | Building a repository/data-access layer, container types (`Stack<T>`, `Cache<K,V>`), utility functions (`firstOrDefault`), or any helper that needs one specific member of an otherwise-arbitrary type. |
| **Avoid When / Common Mistakes** | Only one concrete type will ever be used (speculative generality) · reaching for `any` instead of `<T>` · forgetting a needed constraint and casting inside the body · trying to check `T` at runtime · a small closed set of alternatives (use a union instead). |
| **Related Topics** | `any` (opts out of checking) · `unknown` (safe but needs narrowing) · union types (closed alternatives) · function overloads (distinct fixed signatures) · abstract classes/interfaces (shared shape, pair with `<T>` to keep one concrete type per subclass). |

### Generic Function vs Constrained Generic vs Generic Class
- **Plain generic function** — `identity<T>(x: T): T`. No constraint; works for literally any type.
- **Constrained generic function** — `getIds<T extends HasId>(items: T[]): string[]`. Requires `T` to have at least an `id`.
- **Generic class** — `class InMemoryRepository<T extends HasId, TCreate = Partial<T>>`. Carries the type parameter through every member; instantiated per entity (`InMemoryRepository<User>`, `InMemoryRepository<Product>`).

### Skeleton
```ts
// Generic function — placeholder type, no constraint
function identity<T>(x: T): T {
  return x;
}

// Generic constraint — T must have at least an `id`
interface HasId { id: string; }
function getIds<T extends HasId>(items: T[]): string[] {
  return items.map((item) => item.id);
}

// Generic interface with a default type parameter
interface Repository<T extends HasId, TCreate = Partial<T>> {
  findById(id: string): T | undefined;
  getAll(): T[];
  save(data: TCreate): T;
}

// Generic class implementing the generic interface
class InMemoryRepository<T extends HasId, TCreate = Partial<T>>
  implements Repository<T, TCreate>
{
  private items = new Map<string, T>();
  findById(id: string): T | undefined { return this.items.get(id); }
  getAll(): T[] { return [...this.items.values()]; }
  save(data: TCreate): T {
    const entity = { id: crypto.randomUUID(), ...data } as T;
    this.items.set(entity.id, entity);
    return entity;
  }
}

// Two concrete, unrelated instantiations — zero duplicated logic
const users = new InMemoryRepository<User>();
const products = new InMemoryRepository<Product>();
```

### Remember In One Sentence
> **A generic is a fill-in-the-blank type: you write the logic once with `<T>` standing in for "the real type," and the compiler checks a fresh, fully-typed version of it every time you plug in a concrete type.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Why is `any` not an acceptable substitute for a generic type parameter, even though both "compile for every type"?
2. What does `<T extends HasId>` mean, and why would you add a constraint like this instead of leaving `T` unconstrained?
3. What is a default type parameter, and what problem does `<T, TCreate = Partial<T>>` solve for callers?
4. Do type parameters exist at runtime? What is the practical consequence of your answer for something like `new T()`?
5. Generics vs union types — when is a union the better choice, and when do generics win?
6. Generics vs function overloads — what is the one-line difference in what each is meant to express?
7. Give an example of a generic function that needs no constraint at all, and one that does. Why does the second one need it?
8. If `InMemoryRepository<User>` and `InMemoryRepository<Product>` are two different classes at compile time, why is there only one implementation to maintain?
9. What is "speculative generality" in the context of generics, and how do you recognize you have written it?
10. Name two standard-library or framework generics you use regularly (e.g. from arrays, promises, or a framework) without having written the generic yourself.
