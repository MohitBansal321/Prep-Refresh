# Singleton Pattern — Class Diagram

Shows the classic Singleton structure and, for contrast, the DI-managed variant.
The Singleton class references *itself* (it holds its own single static instance) —
that self-reference is the visual signature of the pattern.

```mermaid
classDiagram
    class ConfigManager {
        -static instance: ConfigManager
        -values: Map~string, string~
        -constructor()
        +static getInstance() ConfigManager
        +get(key) string
        +getNumber(key) number
        +getBoolean(key) boolean
    }

    class ServiceA {
        +doWork() void
    }
    class ServiceB {
        +doWork() void
    }

    ServiceA ..> ConfigManager : getInstance()
    ServiceB ..> ConfigManager : getInstance()
    ConfigManager --> ConfigManager : holds single static instance

    %% --- Contrast: DI-managed singleton (recommended in production) ---
    class AppConfigService {
        +get(key) string
    }
    class ReportService {
        -config: AppConfig
        +buildReport() string
    }
    class DIContainer {
        +resolve(token) instance
    }

    DIContainer --> AppConfigService : creates ONCE (default scope)
    DIContainer --> ReportService : injects the one AppConfigService
    ReportService --> AppConfigService : depends on (injected)
```

**How to read it**
- Top block = **classic Singleton**. `-constructor()` (private) blocks external `new`; `-static instance` is the single slot; `+static getInstance()` is the global accessor. `ServiceA`/`ServiceB` reach it via the dashed `..>` ("uses `getInstance()`").
- The `ConfigManager --> ConfigManager` self-association is the tell-tale sign: the class stores its own instance.
- Bottom block = **DI-managed singleton**. Note `AppConfigService` has **no** private ctor and **no** static field — it is a normal class. The **container** guarantees one instance and *injects* it into `ReportService`. Access is injected, not global, which is what restores testability.
