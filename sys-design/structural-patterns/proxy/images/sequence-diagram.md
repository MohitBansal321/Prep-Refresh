# Proxy Pattern — Sequence Diagram

Shows the runtime message exchange across the stacked proxies for three calls:
a cold-cache analyst request (miss + lazy build), a warm-cache analyst request
(hit, RealSubject skipped), and an unauthorized guest request (denied early).

```mermaid
sequenceDiagram
    autonumber
    participant C as ReportController (Client)
    participant P as ProtectionProxy
    participant K as CachingProxy
    participant R as DatabaseReportService (RealSubject)
    participant Cache as Redis

    Note over C,Cache: 1) First call — analyst, cache empty
    C->>P: generateReport("rev-06", {alice,[analyst]})
    activate P
    P->>P: has "analyst" role? yes
    P->>K: generateReport(...)
    activate K
    K->>Cache: get("report:rev-06")
    Cache-->>K: null (MISS)
    Note over K: lazily create RealSubject (first time only)
    K->>R: generateReport(...)
    activate R
    R-->>K: fresh Report (expensive query)
    deactivate R
    K->>Cache: set("report:rev-06", json, ttl=60)
    K-->>P: Report
    deactivate K
    P-->>C: Report
    deactivate P

    Note over C,Cache: 2) Second call — analyst, cache warm
    C->>P: generateReport("rev-06", {alice,[analyst]})
    activate P
    P->>K: generateReport(...)
    activate K
    K->>Cache: get("report:rev-06")
    Cache-->>K: json (HIT)
    K-->>P: Report (RealSubject NOT called)
    deactivate K
    P-->>C: Report
    deactivate P

    Note over C,Cache: 3) Third call — guest, unauthorized
    C->>P: generateReport("rev-06", {bob,[guest]})
    activate P
    P->>P: has "analyst" role? no
    P-->>C: throws AccessDeniedError (cache & DB untouched)
    deactivate P
```

**How to read it**
- The **Composition Root** (not shown) stacks the proxies once at startup: `Protection → Caching → Real`, then injects the outermost into the Client typed as `ReportService`.
- At runtime the Client only ever talks to the **outermost proxy**; it never references the cache or the RealSubject.
- Call 1 shows the *virtual* aspect: the RealSubject is lazily created on the first miss and the result is cached.
- Call 2 shows the *smart/caching* aspect: a hit returns immediately and the expensive RealSubject is skipped entirely.
- Call 3 shows the *protection* aspect: authorization short-circuits the whole chain before the cache or database is touched.
