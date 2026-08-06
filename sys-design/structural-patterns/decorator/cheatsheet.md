# Decorator Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Structural design pattern (GoF). |
| **Intent** | Attach additional responsibilities to an object *dynamically* by wrapping it in another object that shares the **same interface** — a flexible alternative to subclassing. |
| **Problem** | A core operation needs varying combinations of cross-cutting concerns (logging, retry, caching, rate limiting). Baking them into one class breaks SRP; subclassing every combination explodes to 2^N classes; and the combination is often only known at runtime. |
| **Solution** | Define one Component interface. Write concrete decorators that implement that same interface, hold an inner Component, do a little work before/after, and delegate the rest inward. Stack them at runtime. |
| **Participants** | **Component** (shared interface) · **Concrete Component** (real object) · **Decorator** (abstract base: holds inner + delegates) · **Concrete Decorators** (add one responsibility each). |
| **Flow** | Client → outermost decorator's method → each layer acts, then calls `inner.method()` → reaches Concrete Component → result unwinds back out through each layer. A short-circuiting layer (cache) can skip everything below it. |
| **Pros** | SRP per class · runtime composition · Open/Closed (new concern = new decorator) · avoids subclass explosion · transparent to clients · each layer independently testable · reusable across any Component impl. |
| **Cons** | Many small objects · order-dependence is easy to get wrong · deep chains are hard to debug · `instanceof`/concrete type is lost after wrapping · interface must stay uniform · wiring/config complexity. |
| **Use When** | Wrapping I/O clients/repositories with logging/retry/cache/metrics · independent features whose combinations vary · stream pipelines (compress→encrypt→buffer) · you'd otherwise face a subclass explosion · behavior you may want to remove later. |
| **Avoid When** | Exactly one fixed behavior (just write it) · you need to *change* the interface (Adapter) · single fixed access-control wrapper (Proxy) · concerns are tangled/not independent · ultra-hot inner loops. |
| **Real Examples** | Node `zlib`/`crypto` Transform streams · NestJS interceptors (`CacheInterceptor`) · Express/Koa middleware · Java `BufferedInputStream`, Spring `@Cacheable` AOP proxies · .NET `GZipStream`/`CryptoStream`, Polly handlers · AWS SDK v3 middleware stack · React HOCs · caching repository decorators · resilient LLM client wrappers. |
| **Related Topics** | Adapter (changes interface) · Proxy (same interface, controls access) · Composite (tree of many) · Chain of Responsibility (route until one handles) · Strategy (swap algorithm) · Middleware / interceptors · SOLID (SRP, OCP). |

### GoF Decorator vs TypeScript `@decorator`
- **GoF Decorator (this pattern)** — wrap an *object instance* at *runtime*; composable and stackable.
- **TypeScript `@decorator`** — a *language feature* that annotates/modifies a *declaration* (class/method/property) at *definition time*. Related in spirit, different mechanism. Do not conflate them.

### Skeleton
```ts
interface Component { operation(x: string): string; }          // shared interface

class ConcreteComponent implements Component {                 // the real object
  operation(x: string): string { return `core(${x})`; }
}

abstract class Decorator implements Component {                // abstract base
  constructor(protected readonly inner: Component) {}          // holds a Component
  operation(x: string): string { return this.inner.operation(x); } // delegate
}

class LoggingDecorator extends Decorator {                     // one responsibility
  operation(x: string): string {
    console.log("before");                                     // act before
    const result = this.inner.operation(x);                    // delegate inward
    console.log("after");                                      // act after
    return result;
  }
}

// Compose at runtime — order is behavior:
const c: Component = new LoggingDecorator(new ConcreteComponent());
```

### Remember In One Sentence
> **A Decorator is a layer of clothing: it wraps an object in the same shape, adds one capability, and delegates the rest — and stacking the layers in a different order changes the outcome.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the four participants and give each a one-line responsibility.
2. What single property must every layer preserve, and why is that what enables stacking?
3. Decorator vs Adapter vs Proxy — one-line difference each. Which one *changes* the interface? Which two *keep* it?
4. Why does subclassing fail here? What is the term for the class count blowing up?
5. Why does the *order* of decorators matter? Give a concrete example where a short-circuiting layer changes behavior.
6. How is the GoF Decorator pattern different from TypeScript's `@decorator` syntax?
7. Where should a safety rule like "only retry idempotent methods" live, and why there?
8. How do you unit-test a single decorator in isolation? What do you inject?
9. Name three production systems that are decorator/middleware chains under the hood.
10. When is Decorator the *wrong* choice? (Give at least two situations.)
