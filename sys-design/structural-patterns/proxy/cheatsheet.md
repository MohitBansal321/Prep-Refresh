# Proxy Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Structural design pattern (GoF). |
| **Intent** | Provide a same-interface surrogate/placeholder for another object to **control access to it** (lazy creation, caching, authorization, logging, remote access). |
| **Problem** | You must add cross-cutting control (cache, auth, lazy init, network) around an object that is expensive, sensitive, or remote — without modifying the object or its callers. |
| **Solution** | Add a **Proxy** class that implements the **same interface** as the real object, holds a reference to (or a factory for) it, and on each call decides whether/how to delegate. |
| **Participants** | **Subject** (shared interface) · **RealSubject** (does the real work) · **Proxy** (same interface, controls access, delegates) · **Client** (uses Subject, unaware). |
| **Flow** | Client → calls Subject method on Proxy → Proxy applies its policy (authorize / cache lookup / lazy-create / log) → maybe delegates to RealSubject → result bubbles back → Client can't tell. |
| **Pros** | Separates access-control from real work (SRP) · transparent & substitutable (LSP) · stackable proxies · lazy cost via virtual proxy · big perf wins via caching · location transparency via remote proxy. |
| **Cons** | Extra indirection · miss path is slower than a direct call · cache invalidation is hard (stale data) · cache stampede risk · can hide latency/failures. |
| **Use When** | Caching an expensive op · access control on a sensitive resource · lazy init of heavy objects · remote/RPC stand-in · transparent logging/metrics/rate-limit/retry. |
| **Avoid When** | Object is cheap/local/unguarded (YAGNI) · you need to *add behavior* (Decorator) · *change* the interface (Adapter) · *simplify a subsystem* (Facade) · framework interceptors already cover it. |
| **Real Examples** | NestJS interceptors & `CacheInterceptor` · Spring `@Transactional`/`@Cacheable`/`@PreAuthorize` (dynamic proxies) · Hibernate/EF lazy loading · gRPC client stubs · native JS `Proxy` (Vue/MobX/Valtio) · PgBouncer/RDS Proxy/CloudFront · LiteLLM gateway. |
| **Related Topics** | Decorator (adds behavior, same interface) · Adapter (changes interface) · Facade (simplifies subsystem) · Bridge (decouple abstraction/impl) · Strategy (swap algorithms) · AOP / interceptors · Lazy Load. |

### The Four Kinds of Proxy
- **Virtual** — lazy-create an expensive object on first use (ORM lazy loading, heavy renderers/models).
- **Protection** — authorize before delegating (role/tenant checks in front of a resource).
- **Remote** — local stand-in for an object in another process/machine (gRPC/RPC/HTTP stub).
- **Smart / Caching** — cache results, reference-count, log, rate-limit around access.

### Proxy vs Decorator (the classic trap)
- **Same shape** (both share the interface and wrap an object).
- **Different intent:** Proxy **controls access & manages the real object's lifecycle** (may not create it, may deny the call). Decorator **adds behavior** to an object it is *handed*.

### Skeleton
```ts
interface Subject { request(id: string): Promise<Result>; }        // shared interface
class RealSubject implements Subject {                             // does real work
  async request(id: string) { /* expensive */ return result; }
}
class Proxy implements Subject {
  constructor(private next: Subject, private cache: Cache) {}       // inject next + deps
  async request(id: string) {
    const hit = await this.cache.get(id);
    if (hit) return hit;                                            // control access
    const r = await this.next.request(id);                         // delegate real work
    await this.cache.set(id, r, 60);
    return r;
  }
}
```

### Remember In One Sentence
> **A Proxy is a stand-in with the same face as the real object — it decides how and when you actually reach it (cache it, guard it, lazy-build it, or call it over the wire).**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the four participants and give each a one-line responsibility.
2. What single property makes a proxy *transparent* to the client, and what does that property enable (stacking, substitution)?
3. List the four classic kinds of proxy and give a backend example of each.
4. Proxy vs Decorator: same shape — what is the difference in *intent*? Which one typically manages the real object's lifecycle?
5. Why does a virtual proxy take a *factory* instead of the real instance?
6. Where do caching, authorization, and serialization belong — client, proxy, or real subject? Why?
7. What is cache stampede, and how do you prevent it in a caching proxy?
8. Why must the client be typed as the Subject and not as the concrete proxy?
9. Name three real framework features that are secretly proxies.
10. Give two situations where a Proxy is the *wrong* choice, and say which pattern fits instead.
