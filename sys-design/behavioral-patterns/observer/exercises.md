# Observer Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting "one event, many independent reactions" and inverting knowledge so the Subject never names its Observers.

> Rule of thumb for every exercise: the **Subject** must depend only on the `Observer` *interface* — never on a concrete observer. Adding a reaction should mean writing one new observer class and one `subscribe(...)` line, and touching nothing else. Always pair every `subscribe` with an `unsubscribe`, and never let one observer's failure break the rest.

---

## Easy — Temperature Station

You have an observer contract your app expects:

```ts
interface TemperatureObserver {
  readonly name: string;
  update(celsius: number): void;
}
```

And a subject that holds a reading:

```ts
class WeatherStation {
  // subscribe / unsubscribe / setTemperature go here
}
```

**Task:** Implement `WeatherStation` as the Subject: keep observers in a `Set`, expose `subscribe`, `unsubscribe`, and a `setTemperature(celsius)` that stores the value and notifies every observer. Write two concrete observers — `ConsoleDisplay` (prints the temperature) and `MaxTracker` (remembers the highest value seen). Subscribe both, push three readings, and show each observer reacted.

**Acceptance:** After `setTemperature(20)`, `setTemperature(25)`, `setTemperature(22)`, the `ConsoleDisplay` has printed three times and `MaxTracker` reports 25.

---

## Medium — Order Placed Fan-Out

Reuse the scenario from [code.ts](code.ts). Your app expects:

```ts
interface OrderObserver {
  readonly name: string;
  update(event: OrderPlacedEvent): Promise<void>;
}
```

**Task:** Build an `OrderService` subject (registry + `notify`) and three observers: `EmailObserver`, `InventoryObserver`, and `AnalyticsObserver`, each depending only on its own injected collaborator (a mailer, an inventory repo, an analytics client — use fakes). Wire them in a composition root and place one order so all three react.

**Bonus constraint:** Add a fourth observer — `AuditLogObserver` — *without editing `OrderService`*. Confirm the only changes are the new class plus one `subscribe(...)` line at the composition root. Name which SOLID principles this demonstrates.

---

## Hard — Error Isolation and Safe Notification

Start from your Medium solution.

**Task:**
- Add a deliberately flaky observer whose `update` always rejects (e.g. "loyalty service unavailable"). Prove that when it is subscribed, the *other* observers still complete for the same event.
- Make `notify` iterate a **snapshot** of the observer set, then run all observers with `Promise.allSettled` (not `Promise.all`), and route each rejection to an injectable `onObserverError(reason, observer, event)` handler instead of rethrowing.
- Add a test where an observer **unsubscribes another observer during notification** and show the iteration does not throw or skip incorrectly.

**Think about:** Why `Set` and not an array? Why a snapshot before iterating? Why `allSettled` over `all`? Where does the failure policy belong — in each observer, or in the subject's notify loop?

---

## Real-World Challenge — Domain Events in a NestJS-Style Service

Build a `user.registered` flow used by a NestJS-style `AuthService` (the Subject).

```ts
interface DomainEventObserver<E> {
  readonly name: string;
  update(event: E): Promise<void>;
}
```

Provide **three** observers against simulated collaborators (mock all I/O — no real network):
1. `WelcomeEmailObserver` — sends a welcome email.
2. `AnalyticsObserver` — tracks a `UserRegistered` event.
3. `FreeTrialObserver` — provisions a 14-day trial.

**Requirements:**
- `AuthService.register(...)` persists the user, builds an **immutable** `UserRegisteredEvent`, then notifies — and never imports any observer.
- Observers are wired exclusively at a composition root.
- Each observer owns exactly one reaction and is unit-testable with a fake collaborator.
- Notify **after** the user is persisted (simulate an "after commit" boundary) — write a comment explaining why emailing a welcome before commit would be a bug.
- Add a lifecycle teardown (`onModuleDestroy`-style) that unsubscribes a short-lived observer, and explain the lapsed-listener memory leak it prevents.

**Stretch:** Add a `LoyaltyObserver` and confirm you modified neither `AuthService` nor any existing observer — only a new file and one wiring line.

---

## Bonus Challenge — From In-Process Observer to Pub/Sub, and RxJS

1. Take your `OrderService` and reroute notification through a tiny in-memory **event bus** (`bus.publish('order.placed', event)` / `bus.subscribe('order.placed', handler)`). Explain what changed about coupling: the subject no longer holds references to observers at all. Then describe exactly what you would swap to make this cross-service and durable (Kafka / SNS / Redis), and which guarantees you gain and lose.

2. Reimplement the same fan-out with Node's built-in **`EventEmitter`** (`emitter.on(...)` = subscribe, `emitter.emit(...)` = notify, `off(...)` = unsubscribe). Trigger the `MaxListenersExceededWarning` on purpose by over-subscribing, and explain what real bug that warning is hinting at.

3. Model the temperature readings from the Easy exercise as an **RxJS `Observable`** (or a hand-rolled equivalent) and add operators — `filter` out readings below a threshold and `debounceTime`/throttle a burst. Write down the one-line difference between plain Observer (discrete state-change notifications) and RxJS (composable streams of values over time), and when you would reach for each.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
