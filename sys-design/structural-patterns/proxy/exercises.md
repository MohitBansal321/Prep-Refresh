# Proxy Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting when a resource is *expensive, sensitive, or remote* and isolating the access-control policy into a same-interface stand-in.

> Rule of thumb for every exercise: the **Proxy must implement the exact same interface as the RealSubject**, must **delegate the real work** (never do business logic itself), and must receive the next Subject (or a factory for it) via **constructor injection** — never `new` the real object inside.

---

## Easy — Logging Proxy

You have a Subject interface your app depends on:

```ts
interface Calculator {
  add(a: number, b: number): number;
  divide(a: number, b: number): number;
}
```

And a RealSubject you must not modify:

```ts
class BasicCalculator implements Calculator {
  add(a: number, b: number): number { return a + b; }
  divide(a: number, b: number): number { return a / b; }
}
```

**Task:** Write a `LoggingCalculatorProxy` that implements `Calculator`, wraps a `Calculator` (injected via the constructor), logs `"calling add(2, 3)"` before each call and `"add returned 5"` after, then delegates. Prove that a `Calculator`-typed variable can hold either the real object or the proxy interchangeably.

**Acceptance:** Output shows the log lines around each call, and results are unchanged.

---

## Medium — Virtual (Lazy) Proxy

You have an expensive object:

```ts
interface ImageRenderer {
  render(): string; // returns the rendered image data
}

class HighResRenderer implements ImageRenderer {
  constructor(private path: string) {
    // Pretend this constructor loads a 200 MB file — it is EXPENSIVE.
    console.log(`Loading ${path} into memory...`);
  }
  render(): string { return `<pixels of ${this.path}>`; }
}
```

**Task:** Write a `LazyImageProxy` that implements `ImageRenderer`. It must accept a **factory** `() => HighResRenderer`, NOT an instance. The `HighResRenderer` must be created only on the **first** call to `render()`, and reused on subsequent calls (never re-created).

**Prove it:** Construct the proxy for three images but only call `render()` on one — show that only that one prints `"Loading ... into memory"`.

**Think about:** Why inject a factory instead of the instance? What would break if you injected the instance?

---

## Hard — Caching Proxy with TTL and Invalidation

Your app expects:

```ts
interface ProductService {
  getProduct(id: string): Promise<{ id: string; name: string; priceInCents: number }>;
}
```

The RealSubject hits PostgreSQL and is slow. You are given a Redis-like port:

```ts
interface Cache {
  get(key: string): Promise<string | null>;
  set(key: string, value: string, ttlSeconds: number): Promise<void>;
  del(key: string): Promise<void>;
}
```

**Task:**
- Write a `CachingProductProxy` implementing `ProductService`, wrapping a `ProductService` and a `Cache` (both injected).
- On a hit, return the cached product and do **not** call the real service.
- On a miss, delegate, store the JSON with a TTL, and return it.
- Add an `invalidate(id)` method (owned by the proxy, not the client) that deletes the key — call it whenever a product would be updated.

**Then handle cache stampede:** simulate 10 concurrent `getProduct("p1")` calls on a cold cache and prove the real service is called only **once**, not ten times. (Hint: cache the in-flight *promise*, not just the resolved value — "single-flight".)

**Think about:** Where does serialization belong? Where does invalidation belong? Why must the client stay typed as `ProductService` and not `CachingProductProxy`?

---

## Real-World Challenge — Stacked Protection + Caching + Metrics

Build a small NestJS-style analytics feature.

```ts
interface ReportService {
  generateReport(reportId: string, requester: { userId: string; roles: string[] }): Promise<Report>;
}
```

Provide, all implementing `ReportService`:
1. `DatabaseReportService` — the RealSubject (simulate a 500 ms aggregation).
2. `ProtectionReportProxy` — throws `AccessDeniedError` unless the requester has the `analyst` role.
3. `CachingReportProxy` — TTL cache with single-flight (from the Hard exercise).
4. `MetricsReportProxy` — records call count and total duration, exposing `getStats()`.

**Requirements:**
- Wire them at a composition root as `Protection → Metrics → Caching → Real` and inject the outermost into a `ReportController` typed as `ReportService`.
- Prove that reordering the stack to `Protection → Caching → Metrics → Real` changes *what the metrics measure* (misses only vs every allowed call) — write down the difference and which order you'd choose and why.
- Write a unit test for `ReportController` using a fake `ReportService` — **no proxies involved** — to prove the client is decoupled.
- Write a unit test for `ProtectionReportProxy` alone using a fake `next` Subject — prove it denies/allows correctly without any real database.

**Stretch:** Add event-driven invalidation — when a `report.data.changed` event fires, the caching proxy `del`s the affected key. Document the tradeoff vs pure TTL.

---

## Bonus Challenge — Native JS `Proxy` and a Remote Proxy

1. Using the built-in **`Proxy`** object, write a generic `timed<T>(target: T, onMeasure): T` that wraps any object so that **every method call** is timed and reported via `onMeasure(methodName, ms)` — without subclassing the target. Use the `get` trap and `Reflect`. Apply it to a repository and confirm reads and writes are both measured.

2. Build a **remote proxy**. Given a server-side object:

   ```ts
   interface UserApi {
     getUser(id: string): Promise<{ id: string; name: string }>;
   }
   ```

   Write a `RemoteUserApiProxy implements UserApi` that, instead of doing the work locally, serializes the call to an HTTP request (mock `fetch`), sends it to a fake "server", and deserializes the response — so the client uses it exactly as if it were local. Add a 3-attempt retry with exponential backoff *inside the proxy*, transparently.

3. Explain in a comment: how is your `RemoteUserApiProxy` conceptually identical to a gRPC-generated client stub? What does the proxy hide from the client?

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
