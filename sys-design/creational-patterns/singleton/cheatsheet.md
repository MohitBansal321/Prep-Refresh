# Singleton Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Creational design pattern (GoF). |
| **Intent** | Ensure a class has exactly **one instance** per process and provide a single global point of access to it. |
| **Problem** | Some resources must be shared and single (connection pool, config, logger). Nothing stops `new` from creating duplicates, wasting resources or corrupting shared state. |
| **Solution** | **Private constructor** + **private static instance field** + **public static `getInstance()`** (lazy or eager). In Node, a module-cached `export default new X()` does the same. In production, prefer a **DI-managed singleton**. |
| **Participants** | **Singleton class** (owns the private ctor, static field, and accessor) · **Client(s)** (call `getInstance()` / receive it injected). DI variant adds **Provider class** + **DI container** + **consumers**. |
| **Flow** | First `getInstance()` → field is empty → run private ctor once (expensive setup) → store in static field → return it. Every later call → field is set → return the same reference. |
| **Pros** | Guaranteed single instance · lazy one-time init · single source of truth for shared state · saves resources (one pool/cache/logger) · DI variant adds testability. |
| **Cons** | Global mutable state · hidden dependencies · poor testability (state leaks between tests) · tight coupling to a concrete class · awkward lifecycle · per-process only. |
| **Use When** | DB connection pools/clients · app config loaded once · loggers · in-process caches/metrics registries · expensive-to-build shared clients — ideally **injected** via a DI container. |
| **Avoid When** | Per-request/per-user state · state that must be shared across pods (use Redis/DB) · just to get "global access" · stateless helpers · whenever DI gives you the single instance more testably. |
| **Real Examples** | NestJS providers (singleton by default) · Spring beans (singleton default scope) · .NET `AddSingleton<T>` · `pg.Pool` / `ioredis` client · one AWS/Azure SDK client per app · Node module caching. |
| **Related Topics** | Factory (create many) · Monostate/Borg (many instances, shared state) · Multiton (one per key) · Object Pool (fixed reusable set) · Dependency Injection · Injection scopes (singleton/request/transient). |

### Three Ways It Appears in Node/TS
- **Classic `getInstance()`** — private ctor + static field. Learn it; treat as fallback.
- **Module cache** — `export default new X()` is a de-facto singleton (Node caches modules by path). Fine for stateless/config; hard to mock.
- **DI-managed (recommended)** — `@Injectable()`, container makes one, injects everywhere. Single instance **and** testable.

### Lazy vs Eager
- **Lazy:** create on first `getInstance()`. Defers cost; needs a first-call guard; async init can race — cache the *promise*.
- **Eager:** `static instance = new X()` at load. Simpler, no branch; pays cost + surfaces errors at import time.

### Skeleton
```ts
class Singleton {
  private static instance: Singleton;          // one slot for the class
  private constructor() { /* one-time setup */ } // no external `new`
  static getInstance(): Singleton {
    if (!Singleton.instance) Singleton.instance = new Singleton(); // lazy
    return Singleton.instance;
  }
}
// Preferred in production: @Injectable() + constructor injection (DI singleton).
```

### Remember In One Sentence
> **A Singleton guarantees one instance for the whole process — but "one instance" is the goal, and in modern backends you achieve it by letting a DI container own and inject it, not by a global `getInstance()`.**

### Two Facts People Get Wrong
- It is **thread-safe? No** — you must make it so where threads exist (Node's single thread hides sync races, not async ones).
- It is shared across **pods/workers? No** — it is per-process; use **Redis/DB** for cross-process shared state.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. State the intent in one sentence. What two guarantees does Singleton provide?
2. Name the three ingredients of the classic mechanism and what each one prevents or enables.
3. Lazy vs eager initialization — give one advantage and one drawback of each.
4. Why is the classic Singleton often called an anti-pattern? Name the three specific problems.
5. How does a DI-managed singleton keep the benefit while fixing those problems?
6. In Node, why is `export default new X()` already a singleton? What is the mechanism?
7. Is a Singleton shared across your Kubernetes pods or cluster workers? What do you use for truly shared cross-process state?
8. Describe the async-init race and the exact fix. (What do you cache — the instance or the promise?)
9. Why does a resource-holding Singleton (a pool) need a `close()`, and when do you call it?
10. If you had to add a `reset()` method purely to make your tests pass, what is that telling you to do instead?
