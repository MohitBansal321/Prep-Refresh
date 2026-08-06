# Abstract Factory Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Creational design pattern (GoF). |
| **Intent** | Provide an interface for creating **families of related objects** without specifying their concrete classes, so swapping one factory swaps the whole family consistently. |
| **Problem** | You must support multiple interchangeable ecosystems (multi-cloud, cross-database, cross-OS UI) where each provides *several* related objects that must be used together — and mixing members of two families is a bug. |
| **Solution** | Define one factory object per family, each with a `create*()` method per product type. Client depends only on the factory interface + product interfaces; it picks a *family*, never individual products. |
| **Participants** | **AbstractFactory** (family contract) · **ConcreteFactory** (one per family) · **AbstractProduct(s)** (vendor-neutral interfaces) · **ConcreteProduct(s)** (one set per family) · **Client** (uses factory + products via abstractions only). |
| **Flow** | Composition root reads config → builds the matching ConcreteFactory (once) → injects it as the AbstractFactory type → Client calls each `create*()` → gets abstract-typed products, all from the same factory → uses them purely through interfaces. |
| **Pros** | Guaranteed family consistency (can't mix AWS queue + GCP bucket) · single config-driven choice point · decouples client from concrete classes · OCP: new family = new factory, no client changes · highly testable via a fake factory. |
| **Cons** | **Rigidity when adding a new product type** (must touch every factory) · many classes/interfaces · indirection (can't see the concrete class without tracing to the root) · overkill for a single family (YAGNI). |
| **Use When** | Multiple interchangeable ecosystems each supplying *several related* objects · consistency across a set of products matters · the family is chosen once (config/region/tenant) and propagates to many constructions · you want to swap the whole family for tests. |
| **Avoid When** | Only one family and no realistic second (YAGNI) · you vary only *one* product type (use Factory Method) · product types churn often (rigidity dominates) · products are independent and never need to be consistent · you are assembling one complex object (use Builder). |
| **Real Examples** | knex per-dialect client families · TypeORM/Prisma/Sequelize drivers · .NET `DbProviderFactory` (matching `DbConnection`/`DbCommand`/`DbDataAdapter`) · NestJS dynamic modules / `useFactory` · Spring `@Configuration` + `@Profile` bean families · AWS/GCP/Azure SDK client families sharing one auth context. |
| **Related Topics** | Factory Method (one product via subclassing) · Builder (assemble one complex object) · Prototype (create by cloning) · Facade (simplify a subsystem) · Dependency Injection / Composition Root. |

### Factory Method vs Abstract Factory (get this crisp)
- **Factory Method** — creates **one** product type, via a method subclasses override. Vary *one* product.
- **Abstract Factory** — creates a **FAMILY** of related products, via one object with *several* `create*()` methods. Vary *a whole set together*. It is essentially several coordinated factory methods on one object — that coordination is what guarantees the family stays consistent.

### The two axes of change (the core tradeoff)
- Adding a new **family** (Azure) is **cheap** — write a new factory + products, add one branch at the root; existing code is untouched (OCP-friendly).
- Adding a new **product type** (`SecretManager`) is **expensive** — the factory interface widens, so *every* concrete factory must implement the new method. Choose this pattern only when families change more often than product types.

### Skeleton
```ts
// Abstract products — vendor-neutral interfaces the client depends on
interface BlobStorage { put(key: string, data: Buffer): Promise<{ url: string }>; }
interface MessageQueue { publish(topic: string, msg: string): Promise<void>; }

// Abstract factory — one create*() per product type = the family contract
interface CloudFactory {
  createBlobStorage(): BlobStorage;
  createMessageQueue(): MessageQueue;
}

// Concrete factory — returns ONLY its own family's products (mixing is impossible)
class AwsFactory implements CloudFactory {
  constructor(private cfg: CloudConfig) {}           // shared config → whole family
  createBlobStorage(): BlobStorage { return new S3Storage(this.cfg); }
  createMessageQueue(): MessageQueue { return new SqsQueue(this.cfg); }
}

// Client — depends only on the abstractions; never `new`s a concrete product
class Pipeline {
  private storage: BlobStorage;
  private queue: MessageQueue;
  constructor(factory: CloudFactory) {               // injected once
    this.storage = factory.createBlobStorage();      // same family, guaranteed
    this.queue = factory.createMessageQueue();
  }
}

// Composition root — the ONE place the family is chosen, config-driven
function buildFactory(p: "aws" | "gcp", cfg: CloudConfig): CloudFactory {
  switch (p) {
    case "aws": return new AwsFactory(cfg);
    case "gcp": return new GcpFactory(cfg);
    default: { const _never: never = p; throw new Error(_never); } // exhaustive
  }
}
```

### Remember In One Sentence
> **Abstract Factory is hiring one design studio: you pick a *style* (family) once, and every piece it hands you is guaranteed to match — you never mix a Victorian sofa with a modern glass chair.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the five participants and give each a one-line responsibility.
2. What is the single crispest difference between Factory Method and Abstract Factory? (Hint: one vs a family.)
3. What consistency guarantee does the pattern give, and why is it *structural* rather than just conventional? Give the "AWS queue + GCP bucket" example.
4. Where — and how many times — is the family chosen? Why does choosing it in more than one place re-create the original problem?
5. Which axis of change is cheap and which is expensive? Explain *why* adding a new product type touches every factory.
6. Why does the client depend only on the abstract factory and abstract product interfaces, never on concrete classes?
7. How do you add a new provider (Azure) without modifying any existing code, and which SOLID principle does that demonstrate?
8. How does one fake factory make the entire client unit-testable with no cloud or credentials?
9. What is the purpose of the `never` exhaustiveness check in the composition root's `switch`?
10. Give two situations where Abstract Factory is the *wrong* choice, and say which pattern fits each instead.
