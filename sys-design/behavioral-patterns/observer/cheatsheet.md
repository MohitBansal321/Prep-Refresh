# Observer Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Behavioral design pattern (GoF). |
| **Intent** | Define a one-to-many dependency so that when one object (the Subject) changes state, all its dependents (Observers) are notified and updated automatically — without the Subject knowing their concrete types. |
| **Problem** | One meaningful event triggers many unrelated reactions (order placed → email + inventory + analytics + audit + loyalty…). Hard-coding them inside the subject couples it to every collaborator and forces edits to working code for each new reaction. |
| **Solution** | Invert the direction of knowledge: define an `Observer` interface (`update`); the Subject keeps a registry and exposes `subscribe`/`unsubscribe`/`notify`; on state change it broadcasts an event to whoever registered — depending only on the interface. |
| **Participants** | **Subject** (interface: subscribe/unsubscribe/notify) · **Observer** (interface: `update`) · **ConcreteSubject** (owns state, decides when to notify) · **ConcreteObserver** (one reaction) · **Composition Root** (wires who listens to whom). |
| **Flow** | State changes on ConcreteSubject → it builds an immutable event → calls `notify(event)` → notify snapshots the observer set → for each observer calls `update(event)` (push) → each observer reacts independently → one rejection is isolated, others still run. |
| **Pros** | Loose coupling (subject knows only the interface) · OCP (new reaction = new observer + one subscribe) · SRP (each observer owns one reaction) · runtime-dynamic subscriptions · reuse of one event across many reactions · both sides independently testable. |
| **Cons** | Indirection makes flow hard to trace · no guaranteed ordering · lapsed-listener memory leaks · cascade/update-storm risk · error handling is subtle (one throw can abort the rest) · hidden temporal coupling (sync vs async, in/out of transaction). |
| **Use When** | One event, many independent reactions that grow over time · the source must stay ignorant of who reacts · reactions pluggable per env/tenant/flag · in-process single-service events · DDD domain events · UI/state reactivity. |
| **Avoid When** | Exactly one fixed reaction (YAGNI) · reactions must be strictly ordered/pipelined (Chain of Responsibility) · reactions must be durable/retryable/survive a crash (broker) · reactions cross services (Pub/Sub) · stateful many-to-many coordination (Mediator). |
| **Real Examples** | Node `EventEmitter` (`on`/`emit`/`off`) · NestJS `@OnEvent` + `EventEmitter2` · Express `req`/`res` events · Spring `@EventListener` · .NET `event` + `IObservable<T>` · AWS SNS · GCP Pub/Sub · Redux/Zustand stores · Postgres `LISTEN`/`NOTIFY`. |
| **Related Topics** | Pub/Sub (distributed cousin, broker-mediated, durable, cross-process) · Mediator (many-to-many coordination) · Chain of Responsibility (ordered, one handles) · RxJS/Reactive Streams (composable streams of values) · Command (payload can be a command). |
| **Remember In One Sentence** | **Observer is a subscription list: the Subject shouts "this changed!" to everyone who signed up, without knowing or caring who they are or what they do about it.** |

### Push vs Pull
- **Push** — subject sends the changed data into `update(event)`. Simple; observers get exactly what they need; preferred with a well-designed immutable event (what our example uses).
- **Pull** — subject signals "I changed" via `update(this)` and each observer pulls what it needs back out. More flexible but couples observers to the subject's shape and can cause extra reads.

### Observer vs EventEmitter vs Pub/Sub vs RxJS
- **Observer** — in-process; subject holds direct references to observers.
- **EventEmitter** — Node's stdlib realization of Observer (`on`/`emit`/`off`); `MaxListenersExceededWarning` warns of a likely lapsed-listener leak.
- **Pub/Sub** — a broker/channel sits between publisher and subscriber; decoupled, durable, retryable, cross-service (Kafka, SNS, Redis).
- **RxJS Observable** — Observer generalized to composable streams of values over time, with operators (`map`, `filter`, `debounceTime`, `retry`) and back-pressure.

### Skeleton
```ts
interface Observer { readonly name: string; update(e: Event): Promise<void>; }
interface Subject {
  subscribe(o: Observer): void;
  unsubscribe(o: Observer): void;
  notify(e: Event): Promise<void>;
}
abstract class AbstractSubject implements Subject {
  private readonly observers = new Set<Observer>();          // Set: O(1), no dupes
  subscribe(o: Observer) { this.observers.add(o); }
  unsubscribe(o: Observer) { this.observers.delete(o); }     // pair with subscribe → no leak
  async notify(e: Event) {
    const snapshot = [...this.observers];                    // safe if set mutates mid-notify
    const results = await Promise.allSettled(                // allSettled: one throw ≠ all fail
      snapshot.map((o) => o.update(e)),
    );
    results.forEach((r, i) => {
      if (r.status === "rejected") this.onError(r.reason, snapshot[i], e); // isolate failure
    });
  }
}
// ConcreteSubject: owns state, builds immutable event, calls notify — never names an observer.
// Wire subject + observers ONCE at the composition root.
```

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the five participants and give each a one-line responsibility.
2. What exactly is "the decoupling" in Observer? (Hint: what does the Subject depend on, and what can it *not* do?)
3. Push vs pull model — describe each and say which our example uses and why.
4. Why store observers in a `Set` rather than an array? Give two bugs it prevents.
5. Why snapshot the observer set (`[...observers]`) before iterating in `notify`?
6. `Promise.all` vs `Promise.allSettled` in `notify` — what breaks with the wrong one, and what is the concrete "one flaky observer" framing?
7. What is the lapsed-listener memory leak, why does it happen, and what is the one habit that prevents it?
8. To add a new reaction (e.g. loyalty points), how many files change and which ones? Which SOLID principles does that demonstrate?
9. Observer vs Pub/Sub vs Mediator vs Chain of Responsibility vs RxJS — one-line difference for each. Which is in-process, which is durable/cross-service?
10. Why should you notify *after* a database transaction commits, and what bug appears if you notify mid-transaction?
