# Factory Method Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Creational design pattern (GoF). |
| **Intent** | Define an interface for creating an object, but let a subclass (or a dedicated factory) decide which concrete class to instantiate — so code that *uses* an object never depends on code that *builds* it. |
| **Problem** | You must create objects whose exact concrete type is chosen at runtime or is expected to grow, and scattering `new ConcreteClass()` couples callers to specifics, duplicates selection logic, and forces edits to working code per new type. |
| **Solution** | Isolate the `new` in one place — a factory function, a registry of creators, or a Creator with an abstract factory method its subclasses override — and have callers depend only on the Product interface. |
| **Participants** | **Product** (interface clients use) · **ConcreteProduct** (real impl, one way) · **Creator** (declares the factory method, holds the shared workflow, calls `this.createX()` never `new`) · **ConcreteCreator** (overrides the factory method to pick the product). |
| **Flow** | Composition root builds a ConcreteCreator (or calls a factory fn) → client calls workflow/factory → Creator calls its factory method (or switch/map lookup) → the matching ConcreteProduct is instantiated → returned **typed as the Product interface** → client uses it, never knowing the concrete class. |
| **Pros** | Decouples clients from concrete classes · SRP (creation logic isolated) · OCP with registry/subclasses (add a product without editing callers) · centralized/consistent construction · testable clients (inject fakes) · runtime type selection. |
| **Cons** | More indirection/classes · Factory Method needs a class hierarchy · a Simple Factory `switch` is not open/closed · can hide construction cost · over-application ("factory for everything") adds noise. |
| **Use When** | Concrete type is chosen at runtime (config, feature flag, request field) · the set of types will grow · construction has logic worth centralizing (defaults, validation, pooling) · you want clients testable via fakes · a framework needs an extension point users subclass. |
| **Avoid When** | Exactly one implementation with no realistic second (YAGNI) · a trivial value object with no construction logic · you need *families* of products (→ Abstract Factory) · step-by-step assembly of one complex object (→ Builder) · the type never varies and DI already injects it. |
| **Real Examples** | Node `crypto.createHash()`, `http.createServer()`, `Readable.from()` · NestJS `useFactory` custom providers · Express `express()` / `express.Router()` · Spring `BeanFactory` / `LoggerFactory.getLogger()` · .NET `IHttpClientFactory` · TypeORM `getRepository()` · an `LLMClient` factory for Claude/OpenAI/local by config string. |
| **Related Topics** | Abstract Factory (families of products) · Builder (step-by-step complex object) · Prototype (clone instead of construct) · Strategy (swap behavior, not create) · Simple Factory (idiom: function + switch) · Dependency Injection (containers *are* factories). |

### Three Shapes — Do Not Conflate
- **Simple Factory** (idiom, not GoF) — one function/class with a `switch`/map on a key. The 80% real-world case. Downside: the switch is not open/closed.
- **Factory Method** (the GoF pattern) — a Creator with a workflow declares an abstract `createX()`; **subclasses** override it. Decision made by *which subclass you use* (polymorphism), not a conditional.
- **Abstract Factory** (sibling GoF pattern) — creates *families* of related products used together; essentially a bundle of factory methods.

### Avoiding the Growing `switch`
Replace a hard-coded `switch` with a **registry/map of creator functions**: `register(key, creator)` adds a product from *outside*; `create(key)` looks one up and invokes it. This honors the **Open/Closed Principle** — new products register themselves, existing code is untouched. Tradeoff: you lose the `switch`'s compile-time exhaustiveness check in exchange for runtime extensibility.

### Skeleton
```ts
// Product (what the client depends on)
interface NotificationSender {
  readonly channel: string;
  send(to: string, subject: string, body: string): Promise<SendResult>;
}

// ConcreteProduct
class EmailSender implements NotificationSender {
  readonly channel = "email";
  constructor(private config: EmailConfig) {}
  async send(to: string, subject: string, body: string) { /* ... */ }
}

// (A) Simple Factory — return the INTERFACE, never EmailSender
function createSender(channel: ChannelName, cfg: NotificationConfig): NotificationSender {
  switch (channel) {
    case "email": return new EmailSender(cfg.email);
    case "sms":   return new SmsSender(cfg.sms);
    default: { const _never: never = channel; throw new Error(_never); } // exhaustive
  }
}

// (B) Factory Method — Creator defers creation to subclasses
abstract class NotificationDispatcher {
  protected abstract createSender(): NotificationSender;      // the factory method
  async dispatch(req: NotificationRequest) {
    const sender = this.createSender();                       // never `new` here
    return sender.send(req.to, req.subject, req.body);        // shared workflow
  }
}
class EmailDispatcher extends NotificationDispatcher {
  constructor(private cfg: EmailConfig) { super(); }
  protected createSender(): NotificationSender { return new EmailSender(this.cfg); }
}

// (C) Registry Factory — open/closed, no growing switch
const registry = new Map<string, (c: NotificationConfig) => NotificationSender>();
registry.set("email", (c) => new EmailSender(c.email));       // add products from outside

// Idiomatic JS — a plain factory function, no class required
const makeEmailSender = (cfg: EmailConfig): NotificationSender => ({
  channel: "email",
  async send(to, subject, body) { /* ... */ return result; },
});
```

### Remember In One Sentence
> **A Factory puts every `new` in one place behind an interface — a switch decides in Simple Factory, a subclass decides in Factory Method — so the code that uses an object never depends on the code that builds it.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the four participants of Factory Method and give each a one-line responsibility.
2. What is the single most important rule about a factory's return type, and why does it matter?
3. Simple Factory vs Factory Method vs Abstract Factory — give the one-line difference for each. Which one is *not* a formal GoF pattern?
4. In Factory Method, *how* is the concrete product chosen — and how does that differ from how a Simple Factory chooses?
5. Your Simple Factory's `switch` keeps growing with every new product. What does that violate, and how do you fix it?
6. What does a registry/map of creators give you that a `switch` does not — and what do you give up in return?
7. Where must `new ConcreteProduct()` live, and where must it *never* live?
8. In JavaScript, do you even need a class to write a factory? What is the idiomatic alternative?
9. How do factories make client code testable?
10. How does Factory relate to Dependency Injection — do they compete, and when do you still need an explicit factory even with a DI container?
11. When is a factory the *wrong* choice? Give at least two situations.
12. What does the `default` branch with `const _never: never = key` buy you in a Simple Factory `switch`?
