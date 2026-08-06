# Singleton Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build two reflexes: (1) implementing the single-instance guarantee correctly, and (2) recognizing when a *DI-managed* singleton is the better choice than a classic `getInstance()`.

> Rule of thumb for every exercise: if the object holds **mutable state** or you will need to **fake it in a test**, prefer dependency injection over a global `getInstance()`. Reserve classic Singletons for genuinely single, ideally immutable, per-process resources — and always give resource-holders an explicit shutdown.

---

## Easy — Classic Logger Singleton

Implement a `Logger` class as a classic lazy Singleton.

**Requirements:**
- `private constructor()`, a `private static instance`, and a public `static getInstance()`.
- A `log(message: string)` method that pushes the message into an internal `history: string[]` and prints it.
- A `getHistory(): readonly string[]` accessor.

**Acceptance:**
- `Logger.getInstance() === Logger.getInstance()` is `true`.
- After logging from two different call sites, `getHistory()` contains **both** messages (proving shared state).

**Then answer in a comment:** why is `getHistory()` returning `readonly string[]` (not `string[]`) a small but real protection for a Singleton's shared state?

---

## Medium — Eager vs Lazy, and a Config Singleton

Build a `ConfigManager` singleton that loads values from `process.env` once.

**Task:**
1. Implement it **lazily** (create on first `getInstance()`), validating that a required key `PORT` is a positive integer — throw at construction if not.
2. Now implement a second version **eagerly** (`private static instance = new ConfigManager()`).
3. Write a short note comparing them: when does the eager version's error surface? When does the lazy one's? Which do you prefer for a config object that is *always* needed at startup, and why?

**Bonus constraint:** make the config **immutable** after construction (no public setters; expose only typed getters `getString`, `getNumber`, `getBoolean`). Explain why immutability sidesteps most of the "global mutable state" criticism of Singletons.

---

## Hard — Race-Safe Async Connection Pool

Implement a `ConnectionPool` singleton whose initialization is **asynchronous** (simulate opening N connections with `await`).

**Requirements:**
- `static getInstance(): Promise<ConnectionPool>`.
- If two callers invoke `getInstance()` **concurrently before initialization finishes**, they must receive the **same** instance and initialization must run **exactly once** (do not open two pools).
- Add `acquire()` / `release()` and a `close()` that drains the pool and resets the singleton state.

**Prove it:** call `Promise.all([getInstance(), getInstance(), getInstance()])` and assert all three results are identical *and* that your "open connection" side effect ran only once (e.g. increment a counter during init and assert it equals 1).

**Think about:** why is caching the initialization **promise** (not just the resolved instance) the fix here? What would break if you only cached the instance?

---

## Real-World Challenge — Migrate a Classic Singleton to a NestJS DI Singleton

You inherit this legacy code used across 20 files:

```ts
class RateLimiter {
  private static instance: RateLimiter;
  private counts = new Map<string, number>();
  private constructor() {}
  static getInstance() {
    if (!RateLimiter.instance) RateLimiter.instance = new RateLimiter();
    return RateLimiter.instance;
  }
  hit(userId: string): number { /* increments and returns count */ }
}
```

Business code everywhere calls `RateLimiter.getInstance().hit(userId)`.

**Task:**
1. Refactor `RateLimiter` into a NestJS-style `@Injectable()` provider (default singleton scope) with a **normal constructor** — no static field, no `getInstance()`.
2. Update a sample consumer (`ApiController`) to receive it via **constructor injection**.
3. Write a unit test for `ApiController` that injects a **fake** `RateLimiter` and asserts behavior — with **no shared state** between test cases. Demonstrate that this was effectively impossible with the classic version.
4. **Distributed reality check:** your service runs as **3 Kubernetes pods**. Explain in writing why the in-memory `counts` map makes the rate limit *wrong* (a user gets 3× the intended limit). Then redesign `hit()` to use **Redis** (`INCR` + `EXPIRE`) so the limit is correct across all pods. Note which parts stay a per-process singleton (the Redis *client*) and which state must move to Redis (the *counters*).

---

## Bonus Challenge — Three Singletons, One Comparison

Implement the *same* small `FeatureFlags` service three different ways and compare them:

1. **Classic** `getInstance()`.
2. **Node module-cache** singleton (`export const flags = new FeatureFlags()` / `export default`).
3. **DI-managed** (`@Injectable()`, injected into a consumer).

For each, answer:
- How is the single instance guaranteed?
- How would you replace it with a fake in a test? Rank the three by testability.
- Is the instance shared across cluster workers? (Same answer for all three — say why.)

**Then, the Monostate twist:** implement a fourth version as **Monostate (Borg)** — a class you can freely `new`, but where all instances share the same state via static fields. Show that `new FeatureFlags()` twice still reflects one shared state. Discuss one advantage (callers use normal `new`, no special access) and one danger (surprising shared state behind an innocent-looking constructor) versus classic Singleton.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
