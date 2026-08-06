# Factory Method Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of isolating every `new ConcreteClass()` into one place and letting callers depend only on an interface.

> Rule of thumb for every exercise: your factory must **return the Product interface, never a concrete type**, and no business/domain code may contain `new ConcreteProduct()`. Keep the three shapes straight — Simple Factory (switch/map), Factory Method (subclass overrides creation), Abstract Factory (families).

---

## Easy — Shape Factory

You have a `Shape` interface your app expects:

```ts
interface Shape {
  area(): number;
}
```

And two concrete products you can instantiate:

```ts
class Circle implements Shape {
  constructor(private r: number) {}
  area(): number { return Math.PI * this.r * this.r; }
}
class Square implements Shape {
  constructor(private side: number) {}
  area(): number { return this.side * this.side; }
}
```

**Task:** Write a **Simple Factory** function `createShape(kind, size): Shape` with a `switch` on `kind` (`"circle" | "square"`). Add a `default` branch with a `never`-typed exhaustiveness check so that adding a new kind without handling it fails to compile. Prove a `Shape`-typed variable can hold either result.

**Acceptance:** `createShape("circle", 2).area()` returns roughly `12.566`, and `createShape("square", 3).area()` returns `9`.

---

## Medium — Notification Sender via Registry

Reuse the module's `NotificationSender` interface with `EmailSender`, `SmsSender`, and `PushSender`.

**Task:**
- Build a `SenderRegistry` holding a `Map<string, (config) => NotificationSender>` of creator functions.
- Expose `register(channel, creator)` and `create(channel, config)`; `create` throws a clear error for an unregistered channel.
- Pre-populate email/sms/push via a `buildDefaultRegistry()` helper.
- Now add a **fourth** channel, `SlackSender`, by calling `register("slack", ...)` **without editing** the registry class or any existing creator.

**Bonus constraint:** Show, side by side, why the equivalent Simple Factory `switch` would have forced you to reopen and edit existing code — and name the principle that the registry honors and the switch violates.

---

## Hard — Factory Method with a Real Workflow

Your app must run a shared dispatch workflow (validate body → build sender → send → audit log → translate errors), differing only in *which* channel is used.

**Task:**
- Write an abstract `NotificationDispatcher` Creator with a `dispatch(request)` workflow and a `protected abstract createSender(): NotificationSender` factory method. The base class must never call `new` on a concrete product.
- Write `EmailDispatcher` and `SmsDispatcher` ConcreteCreators that each override `createSender()`.
- Add a `PushDispatcher` and confirm you changed **zero** lines of the base class or the other subclasses.

**Think about:** When is the Factory Method form justified over a plain Simple Factory function? (Hint: what does the Creator hold *besides* the `new`?) If your Creator has only `createSender()` and no shared workflow, what did you actually build?

---

## Real-World Challenge — Provider-Agnostic Document Exporter

Build an `Exporter` port used by a NestJS-style reporting service:

```ts
interface Exporter {
  contentType: string;
  export(rows: Record<string, unknown>[]): Promise<Buffer>;
}
```

Provide **three** exporters (simulate the encoding — no real libraries needed):
1. `CsvExporter` — joins rows into comma-separated text.
2. `JsonExporter` — `JSON.stringify` with indentation.
3. `XlsxExporter` — a stubbed binary buffer with a distinct content type.

**Requirements:**
- Business code (`ReportService.generate(format, rows)`) depends only on `Exporter`.
- Selection of the exporter happens exclusively through a factory keyed by the request's `format` field.
- Choose and justify a factory shape: Simple Factory `switch` vs a registry map. State which you picked and why.
- Write at least one unit test for `ReportService` using an in-memory fake `Exporter` — no real exporter involved. This proves the decoupling.

**Stretch:** Make the exporter set pluggable so a new format (`"pdf"`) can be added by a *different team* in a separate file without touching `ReportService` or the factory. Note which factory shape this forces you toward and why.

---

## Bonus Challenge — Distinguish All Three Shapes & Plain-Function JS

1. Take one scenario (payment clients: Stripe / Razorpay / PayPal behind a `PaymentClient` interface) and implement it **three ways**: as a Simple Factory, as a Factory Method (Creator + ConcreteCreators), and sketch how an **Abstract Factory** would differ if you also needed a matched `RefundClient` + `PayoutClient` per provider. Write one sentence per shape explaining what specifically makes it that shape.

2. Re-implement the Simple Factory in **idiomatic JavaScript with no classes** — a plain factory *function* returning an object literal that satisfies the interface. Explain why classes are optional here and when you would still reach for one.

3. Convert a hand-written `switch`-based factory into a **registry/map of creator functions**, then add a new product by registration only. Write down which SOLID principle this demonstrates and the tradeoff you gave up (hint: compile-time exhaustiveness vs runtime extensibility).

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
