# Adapter Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Structural design pattern (GoF). |
| **Intent** | Convert the interface of a class into another interface a client expects, so incompatible interfaces can work together. |
| **Problem** | You must use existing/third-party/legacy code whose interface does not match what your code expects — and you cannot (or should not) change either side. |
| **Solution** | Add a translator class (Adapter) that *implements the interface you want* (Target) and *wraps the incompatible object* (Adaptee), converting calls and data both ways. |
| **Participants** | **Target** (interface you want) · **Client** (uses Target) · **Adaptee** (incompatible existing code) · **Adapter** (implements Target, wraps Adaptee). |
| **Flow** | Client → calls Target method on Adapter → Adapter translates args → calls Adaptee → Adaptee returns vendor result → Adapter maps result to Target shape → returns to Client. |
| **Pros** | Reuse incompatible code unchanged · decouples client from vendors · SRP (translation isolated) · OCP (new vendor = new adapter) · easy to test & swap. |
| **Cons** | Extra indirection/class · more files · can become a "fat" adapter · tiny runtime cost · may have to fake missing capabilities. |
| **Use When** | Integrating a third-party SDK/legacy module with a mismatched interface · building a provider-agnostic abstraction (storage, payments, notifications, LLMs) · making two unchangeable subsystems cooperate. |
| **Avoid When** | You control both sides and can just fix one · only one implementation ever (YAGNI) · interfaces already match · you actually need Facade (simplify) or Decorator (add behavior). |
| **Real Examples** | NestJS platform adapters (Express/Fastify) · TypeORM/Sequelize DB drivers · `keyv` storage backends · Spring `HandlerAdapter` · AWS/GCS SDKs behind a `FileStorage` port · LangChain LLM wrappers. |
| **Related Topics** | Facade (simplify subsystem) · Decorator (add behavior, same interface) · Proxy (control access, same interface) · Bridge (decouple abstraction/impl, by design) · Strategy (swap algorithms) · Hexagonal / Ports-and-Adapters architecture. |

### Object vs Class Adapter
- **Object adapter (composition)** — wraps the adaptee via a field. Preferred in TS/JS. Flexible, no inheritance limits.
- **Class adapter (inheritance)** — extends the adaptee. Limited by single inheritance; drags in the adaptee's whole surface.

### Skeleton
```ts
interface Target { request(x: number): string; }           // what client wants
class Adaptee { specificRequest(y: string): number { /*...*/ } } // incompatible
class Adapter implements Target {
  constructor(private adaptee: Adaptee) {}                  // inject adaptee
  request(x: number): string {
    return String(this.adaptee.specificRequest(String(x))); // translate both ways
  }
}
```

### Remember In One Sentence
> **An Adapter is a plug converter: it makes something with the wrong-shaped interface fit into the socket your code already expects — without changing either side.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the four participants and give each a one-line responsibility.
2. What is the single most important rule when designing the Target interface? (Hint: whose needs?)
3. Adapter vs Facade vs Decorator vs Proxy — give the one-line difference for each. Which two *change* the interface? Which two *keep* it?
4. Object adapter vs class adapter — which do you prefer in TypeScript and why?
5. Why inject the adaptee into the adapter's constructor instead of `new`-ing it inside?
6. Where do error translation and unit conversion belong — client, adapter, or adaptee? Why?
7. If you switch payment vendors, how many files should change, and which one(s)?
8. Which SOLID principles does adding a new vendor-via-new-adapter demonstrate?
9. What architecture pattern is Adapter the backbone of, and what are its "ports"?
10. When is an Adapter the *wrong* choice? (Give at least two situations.)
