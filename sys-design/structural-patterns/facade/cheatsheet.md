# Facade Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Structural design pattern (GoF). |
| **Intent** | Provide one simplified, high-level interface to a set of interfaces in a complex subsystem, so clients do a common task with a single call. |
| **Problem** | A single business action spans several classes/services with tricky ordering and rollback; making every caller orchestrate them duplicates logic and couples clients to many classes. |
| **Solution** | Add one Facade class exposing task-oriented methods; behind each, it calls the injected subsystem classes in the correct order (and compensates on failure). It adds no new capability. |
| **Participants** | **Facade** (simplified entry point) · **Subsystem classes** (the real workers, independent, reusable) · **Client** (uses only the Facade). |
| **Flow** | Client → calls one Facade method → Facade fans out to subsystem calls in the right order → handles rollback on partial failure → returns one result / one domain error. |
| **Pros** | Simpler clients · decouples client from subsystems · one place for orchestration & rollback · clean service/application layer · testable · subsystems stay reusable. |
| **Cons** | Can become a God object · can hide too much · extra layer · leaky-facade risk · not a security boundary (subsystems still accessible). |
| **Use When** | A task spans several services (checkout, onboarding, media pipeline, RAG) · you want a service/application layer · non-trivial call order + rollback must not be duplicated · to decouple callers from subsystems. |
| **Avoid When** | Subsystem is already simple · callers need fine-grained per-step control · you actually need Adapter (change one interface), Decorator (add behavior), Proxy (control access), or Mediator (peer coordination) · you're tempted to build one mega-facade. |
| **Real Examples** | NestJS/Spring service layer · `JdbcTemplate` over JDBC · Node `fs.promises` over fd calls · ORMs (Prisma/TypeORM) · AWS SDK clients & API Gateway · RAG/LangChain pipelines. |
| **Related Topics** | Adapter (change one interface) · Mediator (peer coordination) · Proxy (same interface, access control) · Decorator (add behavior) · Abstract Factory (build the subsystems) · API Gateway / BFF · Saga / compensating transactions. |

### Thin vs Leaky Facade
- **Thin (correct):** each method is a short sequence of *delegations* to subsystems + rollback. Owns only orchestration knowledge.
- **Leaky (wrong):** reimplements subsystem internals or piles in business rules → becomes a God object; worse than no facade.

### Skeleton
```ts
class SubA { doA(): void {/*real work*/} }
class SubB { doB(): void {/*real work*/} }
class SubC { doC(): void {/*real work*/} }

class Facade {
  constructor(                          // inject subsystems (DI), never `new` inside
    private a: SubA,
    private b: SubB,
    private c: SubC,
  ) {}
  task(): void {                        // one task-oriented method
    this.a.doA();                       // sequence + delegate only
    this.b.doB();                       // (add rollback on partial failure)
    this.c.doC();
  }
}
// Advanced clients may STILL call a.doA() directly — facade is the easy path, not the only path.
```

### Remember In One Sentence
> **A Facade is a waiter: you order one dish and it quietly coordinates the whole kitchen for you — but you're still allowed to walk into the kitchen yourself.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the three participants and give each a one-line responsibility.
2. What is the single most important discipline that keeps a facade healthy? (Hint: thin vs …)
3. Facade vs Adapter — which simplifies *many* classes and which changes *one* interface?
4. Does a facade *hide* the subsystem so it cannot be used directly? Explain.
5. Where does the correct call-order and compensating-rollback knowledge live, and why there?
6. Why inject the subsystems into the facade's constructor instead of `new`-ing them inside?
7. Facade vs Mediator — describe the difference in *direction* of communication.
8. In the checkout example, which step is best-effort and which are fatal, and what happens on each failure?
9. What is the distributed-systems cousin of the Facade pattern, and what does it front?
10. Give two situations where a Facade is the *wrong* choice.
