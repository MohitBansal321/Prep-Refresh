# Prefix Sum — Flow Diagram (Build-Then-Query Lifecycle)

This traces the control flow of the general Prefix Sum lifecycle — one build phase, followed by any number of O(1) queries, with an explicit path back to "rebuild" if the underlying array ever mutates. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual numbers.

```mermaid
flowchart TD
    Start([Start: fixed array arr of length n]) --> Build

    subgraph Build["BUILD PHASE -- runs once, O(n)"]
        direction TB
        Init["Allocate prefix array P of length n + 1<br/>Set P[0] = 0"]
        Init --> Loop["For i = 1 to n:<br/>P[i] = P[i-1] + arr[i-1]"]
    end

    Loop --> Ready([P is ready:<br/>P[k] = sum of arr[0 .. k-1]])

    Ready --> Query

    subgraph Query["QUERY PHASE -- runs any number of times, O(1) each"]
        direction TB
        Receive["Receive a range request [i, j]<br/>(inclusive, 0-indexed)"]
        Receive --> Lookup["Look up P[j+1] and P[i]"]
        Lookup --> Compute["Return P[j+1] - P[i]"]
    end

    Compute --> More{Another query?}
    More -- Yes --> Receive
    More -- No --> Mutated{Did the underlying<br/>array mutate?}

    Mutated -- No --> Idle([Stay ready -- P is still valid])
    Idle --> More
    Mutated -- "Yes -- a value changed" --> Rebuild["Rebuild: re-run the BUILD PHASE<br/>from scratch (or from the changed<br/>index onward), O(n) again"]
    Rebuild --> Ready
```

## How to read it

The diagram has exactly two phases, and the entire point of the pattern is that they run at wildly different frequencies: the **build phase** (top box) runs once, costs O(n), and produces the prefix array `P`. The **query phase** (bottom box) can run any number of times afterward, and each individual run costs only O(1) — a fixed two lookups and one subtraction, completely independent of how wide the requested range `[i, j]` is.

The loop back through "Another query?" is where the pattern earns its keep: the more times you go around that loop, the more the one-time O(n) build cost gets amortized across queries, until the *average* cost per query approaches zero. The branch at the bottom — "did the underlying array mutate?" — is the pattern's one real weak point, made explicit here rather than glossed over: if the array changes, `P` is now stale, and the diagram forces you back through the full O(n) build phase before any further query can be trusted. If mutations happen often enough that this rebuild loop fires on every other query, the amortization argument that makes Prefix Sum worthwhile breaks down — that is precisely the signal to switch to a Segment Tree or Fenwick Tree instead (see the Recognition Diagram).
