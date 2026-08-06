# Adapter Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting incompatible interfaces and isolating the translation into one class.

> Rule of thumb for every exercise: your **Target interface** must be designed from *your application's needs*, never copied from a vendor's API. Keep adapters **thin** — translation only, no business logic.

---

## Easy — Temperature Sensor Adapter

You have a `Thermometer` interface your app expects:

```ts
interface Thermometer {
  getTemperatureCelsius(): number;
}
```

You are given a third-party sensor you cannot modify:

```ts
class FahrenheitSensor {
  readFahrenheit(): number { return 98.6; }
}
```

**Task:** Write a `FahrenheitSensorAdapter` that implements `Thermometer` and wraps a `FahrenheitSensor`, converting Fahrenheit to Celsius inside the adapter. Prove that a `Thermometer`-typed variable can hold your adapter.

**Acceptance:** Calling `getTemperatureCelsius()` on the adapter returns 37.

---

## Medium — Unified Logger

Your app expects a simple logger:

```ts
interface AppLogger {
  info(message: string): void;
  error(message: string): void;
}
```

You have two incompatible loggers you cannot change:

```ts
class WinstonLike {
  log(level: "info" | "error", msg: string): void { /* ... */ }
}
class ConsoleJsonLogger {
  write(payload: { severity: string; text: string }): void { /* ... */ }
}
```

**Task:** Write `WinstonAdapter` and `ConsoleJsonAdapter`, both implementing `AppLogger`. Add a factory that returns the correct adapter based on a config string. Show that switching the logger requires changing only the factory line, not any calling code.

**Bonus constraint:** The adapters must map `error` correctly to each backend's notion of an error level/severity.

---

## Hard — Cache Abstraction with Type-Safe Serialization

Your app expects:

```ts
interface Cache {
  get<T>(key: string): Promise<T | null>;
  set<T>(key: string, value: T, ttlSeconds: number): Promise<void>;
  delete(key: string): Promise<void>;
}
```

You must support two adaptees:
1. A `RedisLike` client whose API is `getString(k)`, `setString(k, v, "EX", ttl)`, `del(k)` — values must be JSON strings.
2. An in-memory `Map`-based store for tests, with no TTL support natively.

**Task:**
- Write `RedisCacheAdapter` (handle JSON serialization/deserialization inside the adapter).
- Write `InMemoryCacheAdapter` that *simulates* TTL using timestamps, since the underlying `Map` has none — this shows the adapter faking a capability the adaptee lacks.
- Ensure `get` returns `null` (not `undefined`) for missing or expired keys.

**Think about:** Where does serialization belong? Where does expiry logic belong? Keep both out of the client.

---

## Real-World Challenge — Provider-Agnostic File Storage

Build a `FileStorage` port used by a NestJS-style service:

```ts
interface FileStorage {
  upload(path: string, data: Buffer, contentType: string): Promise<{ url: string }>;
  download(path: string): Promise<Buffer>;
  delete(path: string): Promise<void>;
}
```

Provide **three** adapters against simulated SDKs (mock the network — no real cloud calls):
1. `S3Adapter` — SDK uses `putObject({ Bucket, Key, Body, ContentType })`, `getObject(...)`, `deleteObject(...)`.
2. `GcsAdapter` — SDK uses `bucket(name).file(path).save(buffer, opts)`, `.download()`, `.delete()`.
3. `LocalDiskAdapter` — uses Node's `fs/promises`, returns a `file://` URL.

**Requirements:**
- Business code (`AvatarService.updateAvatar(userId, buffer)`) depends only on `FileStorage`.
- Selection of the adapter happens exclusively at a composition root, driven by an env var.
- Each adapter translates errors into a shared `StorageError`.
- Write at least one unit test for `AvatarService` using an in-memory fake `FileStorage` — no adapters involved. This proves the decoupling.

**Stretch:** Add a `presignUrl(path, expirySeconds)` method to the Target. Notice that `LocalDiskAdapter` cannot truly presign — decide and document how it degrades (throw `NotSupportedError` vs return a plain URL). This exercises the "capability mismatch" tradeoff.

---

## Bonus Challenge — Two-Way Adapter & Hexagonal Wiring

1. Implement a **two-way adapter** between an old event format and a new one, where events flow *both* directions (a legacy system emits `LegacyEvent`, your system emits `DomainEvent`, and each side must consume the other). Your adapter implements both interfaces.

2. Refactor the payment example from [code.ts](code.ts) into an explicit **Ports-and-Adapters (Hexagonal)** layout:
   - `domain/` — `PaymentService`, `PaymentGateway` port, domain types (no vendor imports allowed).
   - `adapters/` — `StripeAdapter`, `RazorpayAdapter`.
   - `main.ts` — the composition root that wires ports to adapters.

   Add an ESLint rule (or a written policy) forbidding any import of vendor SDKs from inside `domain/`. Explain in a comment why this rule protects the architecture.

3. Add a new provider (`PayPalAdapter`) and confirm you did **not** modify `PaymentService` or any existing adapter — only added a new file and one factory branch. Write down which SOLID principles this demonstrates.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
