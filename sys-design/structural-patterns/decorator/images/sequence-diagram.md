# Decorator Pattern — Sequence Diagram

Runtime message exchange for one call to a *flaky* endpoint through the recommended
stack `Logging → Cache → Retry → RateLimit → Fetch`. The first network attempt fails
transiently; the retry decorator backs off and tries again; the second attempt
succeeds; the cache stores the result; and logging reports the total latency. Every
arrow is the same `send(request)` call — that uniformity is the whole point.

```mermaid
sequenceDiagram
    autonumber
    participant Client as UserApiClient
    participant Log as LoggingHttpClient
    participant Cache as CacheHttpClient
    participant Retry as RetryHttpClient
    participant RL as RateLimitHttpClient
    participant Fetch as FetchHttpClient

    Client->>Log: send(GET /flaky)
    activate Log
    Note over Log: log "-->", start timer
    Log->>Cache: send(request)
    activate Cache
    Note over Cache: cache MISS
    Cache->>Retry: send(request)
    activate Retry

    Note over Retry: attempt 1
    Retry->>RL: send(request)
    activate RL
    Note over RL: spend a token
    RL->>Fetch: send(request)
    Fetch-->>RL: throws 503 (transient)
    RL-->>Retry: throws 503
    deactivate RL
    Note over Retry: backoff, then attempt 2
    Retry->>RL: send(request)
    activate RL
    Note over RL: spend a token
    RL->>Fetch: send(request)
    Fetch-->>RL: 200 OK
    RL-->>Retry: 200 OK
    deactivate RL

    Retry-->>Cache: 200 OK
    deactivate Retry
    Note over Cache: store in cache
    Cache-->>Log: 200 OK
    deactivate Cache
    Note over Log: log "<--" + total latency
    Log-->>Client: 200 OK
    deactivate Log
```

**How to read it**
- Every participant exposes the identical `send(request)` method; the request travels
  *inward* (left to right in construction terms, top to bottom here) and the response
  unwinds *outward*.
- The **Retry** layer is the only one that loops: attempt 1 fails, it sleeps (backoff),
  attempt 2 succeeds. Both attempts pass through the **RateLimit** layer, so each real
  attempt spends a token — a consequence of Retry sitting *outside* RateLimit.
- The **Cache** stores the response only after a successful miss; a later identical GET
  would return at the `cache MISS` note as a hit and never reach Retry/RateLimit/Fetch.
- The **Logging** layer, being outermost, measures the latency of the entire operation
  — including the failed attempt and the backoff — which is exactly why it belongs on
  the outside.
