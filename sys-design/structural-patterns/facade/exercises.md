# Facade Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting a multi-step task that spans several classes and hiding the correct sequence (and rollback) behind one simple, well-named method.

> Rule of thumb for every exercise: the facade only **sequences and delegates**. Every real action must be a subsystem method — no business logic, no reimplemented subsystem internals inside the facade. Inject subsystems via the constructor. Keep the facade **thin**.

---

## Easy — Home Theater Facade

You have five subsystem classes (already given, do not change their APIs):

```ts
class Amplifier { on(): void; off(): void; setVolume(level: number): void; }
class DvdPlayer { on(): void; off(): void; play(movie: string): void; stop(): void; }
class Projector { on(): void; off(): void; wideScreenMode(): void; }
class Lights { dim(percent: number): void; on(): void; }
class Screen { down(): void; up(): void; }
```

**Task:** Write a `HomeTheaterFacade` with two methods: `watchMovie(movie: string)` and `endMovie()`. `watchMovie` must dim lights, lower the screen, turn on and configure the projector, turn on the amp and set volume, then turn on the DVD and play. `endMovie` reverses it. Inject all five subsystems via the constructor.

**Acceptance:** A client can start and stop a movie with one call each and never touches a subsystem directly. Prove that the subsystems are still usable directly (call `amplifier.setVolume(5)` outside the facade) — the facade does not seal them off.

---

## Medium — Onboarding Facade with Rollback

Three subsystems (mock the bodies; no real I/O):

```ts
class AuthService { createAccount(email: string): Promise<{ userId: string }>; deleteAccount(userId: string): Promise<void>; }
class BillingService { startTrial(userId: string): Promise<{ subscriptionId: string }>; }
class EmailService { sendWelcome(email: string): Promise<void>; }
```

**Task:** Write an `OnboardingFacade.register(email)` that: (1) creates the account, (2) starts a trial, (3) sends a welcome email. Requirements:
- If `startTrial` fails, **delete the account** (compensating rollback) and throw a single `OnboardingError` tagged with the stage.
- The welcome email is **best-effort** — if it fails, log a warning and still return success.
- Return one result object `{ userId, subscriptionId }`.

**Bonus constraint:** The client must catch only `OnboardingError`, never `AuthService`/`BillingService`-specific errors.

**Think about:** Which steps are fatal vs best-effort? Where does that decision belong?

---

## Hard — Media Conversion Facade over a Real Subsystem

You are wrapping a genuinely awkward subsystem. Given these (mock the internals):

```ts
class FileIO { read(path: string): Promise<Buffer>; write(path: string, data: Buffer): Promise<void>; }
class Decoder { decode(input: Buffer, sourceCodec: string): Promise<RawFrames>; }
class Scaler { resize(frames: RawFrames, width: number, height: number): Promise<RawFrames>; }
class Encoder { encode(frames: RawFrames, targetCodec: string): Promise<Buffer>; }
class Muxer { mux(encoded: Buffer, container: string): Promise<Buffer>; }
```

**Task:** Write a `MediaConversionFacade.convert(opts)` where `opts` is `{ inputPath, outputPath, targetCodec, container, width?, height? }`. The facade must: read → decode → (optionally) scale if width/height given → encode → mux → write. Requirements:
- Detect the source codec from the file extension inside the facade's *coordination* logic only — do **not** reimplement any decode/encode work.
- Scaling is optional and must be skipped cleanly when no dimensions are provided.
- Translate any subsystem failure into a single `ConversionError { stage }`.

**Think about:** What is orchestration (belongs in the facade) versus real work (belongs in a subsystem)? Is codec detection orchestration or work? Justify your choice.

---

## Real-World Challenge — Checkout Facade as a NestJS Service Layer

Refactor the checkout example from [code.ts](code.ts) into a NestJS-style layout (you may stub the `@Injectable`/`@Module` decorators if not running Nest):

```
checkout/
  checkout.facade.ts        // OrderCheckoutFacade (@Injectable)
  inventory.service.ts      // @Injectable
  payment.service.ts        // @Injectable
  shipping.service.ts       // @Injectable
  notification.service.ts   // @Injectable
  order.controller.ts       // depends only on OrderCheckoutFacade
  checkout.module.ts        // wires providers (the composition root)
```

**Requirements:**
- The controller depends **only** on `OrderCheckoutFacade`. It must not import any of the four services.
- All subsystems are provided/injected through the Nest DI container (constructor injection).
- Add a **fraud check** step (`FraudService.assess(req)`) that runs *before* payment. If it flags the order, roll nothing back (nothing charged yet) and throw `CheckoutError(stage="FRAUD")`. Note: you edited **only the facade** to add this step — write down why the controller did not change (which SOLID principle).
- Write a unit test for `OrderCheckoutFacade` that injects **fake** subsystems and asserts: (a) happy path calls the four services in the correct order, and (b) a shipping failure triggers both `refund` and `release`.

**Stretch:** Add a second client — a `RetryOrderJob` (background worker) — that also places orders through the *same* facade. Confirm you reused the orchestration with zero duplication.

---

## Bonus Challenge — Facade Boundaries & the God-Object Trap

1. **Split the God object.** You are handed a 900-line `AppFacade` that fronts checkout, onboarding, refunds, and reporting in one class. Refactor it into four use-case facades. Write a short note on *why* one-facade-per-app is an anti-pattern and how you decided the split boundaries.

2. **Facade over adapters.** Make the checkout facade's `PaymentService` internally an **Adapter** over a vendor SDK (Stripe-shaped). Confirm the facade code does not change — it still calls `payment.charge(...)`. Explain in a comment how Facade and Adapter compose (which one is "inside" the other and why).

3. **Parallelize safely.** In the checkout flow, identify which steps *must* be sequential (ordering dependency) and which *could* run with `Promise.all`. Refactor the safe ones to run concurrently and document why the payment step can never move before reservation.

4. **Prove it is not a security boundary.** Write a small script that bypasses the facade and calls `PaymentService.refund()` directly. Use this to explain the difference between "the easy path" and "the only path," and what you would add (module encapsulation / access control) if you needed true enforcement.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
