# Abstract Factory Pattern — Flow Diagram

Step-by-step control flow from application startup: read config, pick a family once,
inject the factory, and let the client build and use a guaranteed-consistent family.

```mermaid
flowchart TD
    Start([App starts]) --> Cfg["Read CLOUD_PROVIDER from config"]
    Cfg --> Which{Which family?}
    Which -- aws --> Aws["factory = new AwsFactory(cfg)"]
    Which -- gcp --> Gcp["factory = new GcpFactory(cfg)"]
    Aws --> Inject["Inject factory into EventPipelineService<br/>(typed as CloudResourceFactory)"]
    Gcp --> Inject
    Inject --> Build["Client calls createBlobStorage(),<br/>createMessageQueue(), createDatabase()"]
    Build --> Guarantee{{"All products come from the SAME factory<br/>→ same family, guaranteed consistent"}}
    Guarantee --> Use["Client uses products via abstract interfaces only"]
    Use --> End([Same client code runs on any cloud])
```

**Key idea:** the only branch on provider (`Which family?`) happens once, at the
composition root. Everything after `Inject` is provider-agnostic — the client never
sees `aws`/`gcp` again. Adding a new family (Azure) adds one branch here plus a new
factory and products; the client code below the branch never changes.
