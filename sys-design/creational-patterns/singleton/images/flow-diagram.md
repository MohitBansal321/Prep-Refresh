# Singleton Pattern — Flow Diagram

Control flow of a `getInstance()` call, including the lazy first-time branch and
the async-init race guard used by a resource-holding singleton (the pool).

```mermaid
flowchart TD
    Start([Client calls getInstance]) --> HasInstance{instance already created?}
    HasInstance -- Yes --> Return[Return existing instance]
    HasInstance -- No --> HasPromise{"init already in progress?<br/>(async case)"}
    HasPromise -- Yes --> Await[Await the SAME init promise]
    HasPromise -- No --> Create["Run private constructor:<br/>expensive one-time setup<br/>(load config / open pool)"]
    Create --> Store[Store instance in static field]
    Store --> Return
    Await --> Return
    Return --> End([Client uses the one shared instance])
```

**Key idea:** the first diamond is the core of the pattern (create-once vs reuse).
The second diamond is the production hardening for **async** initialization: by
caching the initialization *promise* (not just the resolved instance), two
concurrent first-time callers await the same setup and you never open two pools.
The expensive constructor box runs **exactly once per process** — remember, that
is per pod/worker, not per cluster.
