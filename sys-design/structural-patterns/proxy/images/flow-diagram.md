# Proxy Pattern — Flow Diagram

Step-by-step control flow of a single `generateReport()` call through the stacked
proxies: authorization first, then cache lookup, then lazy creation and delegation
to the RealSubject only when truly needed.

```mermaid
flowchart TD
    Start([Client calls generateReport]) --> Auth{ProtectionProxy:<br/>has required role?}
    Auth -- No --> Deny[Throw AccessDeniedError<br/>cache & DB never touched]
    Auth -- Yes --> Lookup["CachingProxy:<br/>cache.get(key)"]
    Lookup --> Hit{Cache hit?}
    Hit -- Yes --> Return[Return cached Report<br/>RealSubject NOT called]
    Hit -- No --> Lazy{RealSubject<br/>created yet?}
    Lazy -- No --> Create[Lazily create RealSubject<br/>via factory - first miss only]
    Lazy -- Yes --> Delegate
    Create --> Delegate[Delegate to RealSubject]
    Delegate --> Work[RealSubject runs<br/>expensive aggregation]
    Work --> Store["cache.set(key, result, ttl)"]
    Store --> Return
    Return --> End([Client receives Report,<br/>unaware of proxies])
    Deny --> End
```

**Key idea:** every decision box before "Delegate to RealSubject" is *access control*,
not business work. Authorization short-circuits before any expensive resource is touched;
a cache hit skips the RealSubject entirely; a virtual proxy builds the RealSubject only on
the first genuine miss. Swapping, removing, or reordering these boxes changes the policy
without changing the client or the RealSubject.
