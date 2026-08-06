# Singleton Pattern — Sequence Diagram

Runtime message exchange showing that construction happens once and every later
lookup returns the same reference, plus the DI-managed alternative and the
per-process reality across pods.

```mermaid
sequenceDiagram
    autonumber
    participant A as ServiceA (Client)
    participant B as ServiceB (Client)
    participant C as ConfigManager (Singleton class)

    Note over C: static instance = undefined at module load

    A->>C: getInstance()
    activate C
    Note over C: instance is undefined → create it
    C->>C: new ConfigManager() (private ctor, load + validate ONCE)
    C-->>A: instance #1
    deactivate C

    B->>C: getInstance()
    activate C
    Note over C: instance exists → skip construction
    C-->>B: instance #1 (same reference)
    deactivate C

    Note over A,B: (instanceA === instanceB) is true

    rect rgb(235, 245, 255)
    Note over A,C: Recommended alternative — DI-managed singleton
    participant D as DI Container
    D->>D: create AppConfigService ONCE (default scope)
    D->>A: inject the same AppConfigService
    D->>B: inject the same AppConfigService
    Note over A,B: same instance, but injected — mockable in tests
    end
```

**How to read it**
- The **first** `getInstance()` triggers the private constructor — the expensive load/validate runs exactly once.
- Every **later** call, from any client, skips construction and returns the already-built reference; the equality note captures the guarantee.
- The highlighted block shows the production-preferred path: the **DI container** creates one instance and *injects* it, so both services still share one object — but it can be replaced with a fake in tests, unlike the global `getInstance()`.
- Reality check (not drawn, but remember): run this in **3 pods** and you get **3** separate `ConfigManager` instances — one per process. Cross-process shared state belongs in Redis or the database.
