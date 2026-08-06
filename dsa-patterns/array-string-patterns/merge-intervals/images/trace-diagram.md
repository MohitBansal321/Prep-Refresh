# Merge Intervals — Trace Diagram (Worked Example)

This traces the exact sweep state for the classic **Merge Intervals** example used in [problems/01-merge-intervals.cpp](../problems/01-merge-intervals.cpp):

```
input (unsorted) = [[1,3], [8,10], [2,6], [15,18]]
after sorting by start = [[1,3], [2,6], [8,10], [15,18]]
```

```mermaid
sequenceDiagram
    autonumber
    participant S as Sorted list (sweep source)
    participant C as current (running merged interval)
    participant R as result (output list)

    Note over S: sorted = [1,3], [2,6], [8,10], [15,18]

    S->>C: current = [1,3]  (first interval seeds "current")
    Note over C: current = [1,3]

    S->>C: next = [2,6]
    C->>C: next.start(2) <= current.end(3)? YES -> overlap
    C->>C: EXTEND: current.end = max(3, 6) = 6
    Note over C: current = [1,6]

    S->>C: next = [8,10]
    C->>C: next.start(8) <= current.end(6)? NO -> no overlap
    C->>R: FLUSH current = [1,6] into result
    Note over R: result = [1,6]
    C->>C: current = [8,10]  (start a new running interval)
    Note over C: current = [8,10]

    S->>C: next = [15,18]
    C->>C: next.start(15) <= current.end(10)? NO -> no overlap
    C->>R: FLUSH current = [8,10] into result
    Note over R: result = [1,6], [8,10]
    C->>C: current = [15,18]  (start a new running interval)
    Note over C: current = [15,18]

    Note over S: sweep exhausted - no more intervals
    C->>R: FLUSH final current = [15,18] into result
    Note over R: result = [1,6], [8,10], [15,18]  (FINAL)
```

## How to read it

Each "round trip" is one iteration of the sweep in [flow-diagram.md](flow-diagram.md): `current` is compared against the next interval in sorted order, and exactly one of two things happens — it either **extends** (absorbs the next interval by growing its end) or **flushes** (the next interval cannot possibly connect, given sortedness, so `current` is finalized into `result` and a brand-new `current` starts at the next interval). Notice `current.end` only ever moves in one direction across its own lifetime — up, via `max()` — and once it is flushed it is never revisited; that one-way movement is exactly why the whole sweep is a single O(n) pass with no backtracking.

Also notice the trace's final step: `[15,18]` never gets a chance to be compared against anything after it (the sweep runs out of input), so it must be flushed *after* the loop ends, not inside it. This is the exact off-by-one this module's Common Mistakes section warns about — forgetting that final flush silently drops the last merged interval from the output, and because the bug only manifests on the very last group, it is easy for a quick manual test (which often only checks two or three intervals) to miss it entirely.
