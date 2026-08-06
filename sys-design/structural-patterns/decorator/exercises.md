# Decorator Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting a cross-cutting concern and isolating it into a wrapper that keeps the *same interface* as the thing it wraps.

> Rules of thumb for every exercise: (1) every decorator must implement the **same Component interface** as the object it wraps — never change the interface. (2) A decorator adds **one** responsibility. (3) Inject collaborators (loggers, clocks, sleep, stores) through the constructor so each decorator is testable in isolation. (4) Observing decorators must be transparent to control flow — observe, then return/re-throw unchanged.

---

## Easy — Text Notification Decorators

You have a `Notifier` interface your app expects:

```ts
interface Notifier {
  send(message: string): void;
}
```

And a base implementation:

```ts
class ConsoleNotifier implements Notifier {
  send(message: string): void { console.log(`NOTIFY: ${message}`); }
}
```

**Task:** Write two decorators, `TimestampDecorator` and `UppercaseDecorator`, both implementing `Notifier` and wrapping a `Notifier`. `TimestampDecorator` prefixes the message with an ISO timestamp; `UppercaseDecorator` upper-cases the message. Then compose them two different ways and observe the different output.

**Acceptance:** `new UppercaseDecorator(new TimestampDecorator(base))` and `new TimestampDecorator(new UppercaseDecorator(base))` produce visibly different strings. Explain in a comment *why* they differ.

---

## Medium — Data Stream Pipeline (Compression + Encryption)

Model a data source as:

```ts
interface DataSource {
  write(data: string): string;   // returns the transformed payload
  read(data: string): string;    // reverses the transform
}
```

Base implementation stores/returns data unchanged (`FileDataSource`).

**Task:** Write `CompressionDecorator` and `EncryptionDecorator` (you may *simulate* compression/encryption with simple reversible transforms, e.g. run-length or base64 and a Caesar shift). Each must implement `DataSource` and wrap a `DataSource`. `write()` applies its transform then delegates inward; `read()` delegates inward then reverses its transform.

**Bonus constraint:** Prove that `read()` must reverse the transforms in the *opposite* order to `write()`. Build `Encryption(Compression(File))` and show that writing then reading round-trips correctly — and that mismatched ordering corrupts the data.

---

## Hard — Decorated Repository with Read-Through Cache

Your app expects a repository:

```ts
interface ProductRepository {
  findById(id: string): Promise<Product | null>;
  save(product: Product): Promise<void>;
}
```

You have a `SqlProductRepository` (simulate the DB with a `Map` and a query counter).

**Task:**
- Write a `LoggingProductRepository` decorator that logs every call and its latency.
- Write a `CachingProductRepository` decorator that read-through caches `findById` results and **invalidates** the cache entry on `save`.
- Inject a `CacheStore` interface so tests can use a `Map` and prod could use Redis.
- Prove, via the DB query counter, that two consecutive `findById` calls for the same id hit the database only once, and that a `save` for that id causes the next `findById` to hit the DB again.

**Think about:** Where does invalidation belong? What happens to correctness if you put `Caching` *outside* `Logging` vs *inside*? Which order lets you observe cache hits in the logs?

---

## Real-World Challenge — Resilient LLM Client

Build an `LLMClient` port used by a service:

```ts
interface LLMClient {
  complete(prompt: string): Promise<{ text: string; tokensUsed: number }>;
}
```

Provide a base `HttpLLMClient` (simulate the network; make it fail transiently sometimes and report token usage).

Write these decorators, each keeping the `LLMClient` interface:
1. `RetryLLMClient` — retries on transient failure with exponential backoff (inject a sleep seam). Do **not** retry on a non-transient error (e.g. a 400 invalid-prompt).
2. `CachingLLMClient` — caches responses for identical prompts (identical string ⇒ cached response, zero tokens).
3. `TokenBudgetLLMClient` — tracks cumulative `tokensUsed`; throws `BudgetExceededError` once a configured budget is crossed.
4. `LoggingLLMClient` — logs prompt hash, latency, tokens, and cache hit/miss.

**Requirements:**
- The service depends only on `LLMClient` and cannot tell how many layers exist.
- Assemble the stack at a single composition root, driven by config.
- Decide and **document** the correct order. Specifically: should the token budget count cache hits? Should retries happen inside or outside the budget check? Justify each choice in comments.
- Write at least one unit test per decorator, wrapping a fake `LLMClient`, with injected seams — no real network, no real waiting.

**Stretch:** Add a `RateLimitLLMClient` (token bucket). Position it so cache hits do not consume rate-limit tokens, and prove it with a counter.

---

## Bonus Challenge — Framework-Grade Composition & the `@decorator` Distinction

1. **Generic decorator factory.** Write a higher-order function `withRetry(inner, opts)`, `withLogging(inner, logger)`, etc., that return decorated instances, plus a `compose(base, ...wrappers)` helper so you can write `compose(base, withRateLimit(...), withRetry(...), withLogging(...))`. Discuss in a comment how `compose` argument order maps to wrapping order.

2. **Runtime-configurable stack.** Given a config array like `["logging", "cache", "retry"]`, build the correct decorator stack dynamically at startup. Handle an unknown decorator name safely.

3. **Prove the language-feature distinction.** Implement the *same* "log method calls" behavior two ways: (a) as a GoF object Decorator wrapping an instance, and (b) as a TypeScript **method decorator** (`@LogCalls`) applied to a class method. In a written comment, explain precisely how they differ — when each runs, what each wraps, and why (a) can be composed/stacked at runtime while (b) is fixed at definition time.

4. **Break it on purpose.** Take your resilient client from the Real-World Challenge and deliberately mis-order the stack so that (i) cache hits are never logged, and (ii) the rate limiter wastes tokens on cache hits. Write down the exact symptom each bug would cause in production and how you'd detect it.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
