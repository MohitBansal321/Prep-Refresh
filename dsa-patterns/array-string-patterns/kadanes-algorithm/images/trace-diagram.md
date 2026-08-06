# Kadane's Algorithm — Trace Diagram (Worked Example)

This traces the exact `current_sum` / `best_sum` values across the classic Maximum Subarray example used in [problems/01-maximum-subarray.cpp](../problems/01-maximum-subarray.cpp):

```
nums  = [-2, 1, -3, 4, -1, 2, 1, -5, 4]
index:    0   1   2  3   4  5  6   7  8
```

```mermaid
sequenceDiagram
    autonumber
    participant E as Element (nums[i])
    participant C as current_sum
    participant B as best_sum

    Note over C,B: Step 0 -- initialize: current_sum = nums[0] = -2, best_sum = -2

    E->>C: i=1, nums[1]=1
    C->>C: extend = -2+1=-1, restart = 1 -> restart WINS (1 > -1)
    Note over C: current_sum = 1 (new run starts at index 1)
    C->>B: compare 1 vs -2 -> best_sum updates
    Note over B: best_sum = 1

    E->>C: i=2, nums[2]=-3
    C->>C: extend = 1-3=-2, restart = -3 -> extend WINS (-2 > -3)
    Note over C: current_sum = -2 (still running from index 1)
    C->>B: compare -2 vs 1 -> no change
    Note over B: best_sum = 1

    E->>C: i=3, nums[3]=4
    C->>C: extend = -2+4=2, restart = 4 -> restart WINS (4 > 2)
    Note over C: current_sum = 4 (new run starts at index 3)
    C->>B: compare 4 vs 1 -> best_sum updates
    Note over B: best_sum = 4 (range so far: [3, 3])

    E->>C: i=4, nums[4]=-1
    C->>C: extend = 4-1=3, restart = -1 -> extend WINS (3 > -1)
    Note over C: current_sum = 3 (still running from index 3)
    C->>B: compare 3 vs 4 -> no change
    Note over B: best_sum = 4

    E->>C: i=5, nums[5]=2
    C->>C: extend = 3+2=5, restart = 2 -> extend WINS (5 > 2)
    Note over C: current_sum = 5
    C->>B: compare 5 vs 4 -> best_sum updates
    Note over B: best_sum = 5 (range so far: [3, 5])

    E->>C: i=6, nums[6]=1
    C->>C: extend = 5+1=6, restart = 1 -> extend WINS (6 > 1)
    Note over C: current_sum = 6
    C->>B: compare 6 vs 5 -> best_sum updates
    Note over B: best_sum = 6 (range so far: [3, 6])

    E->>C: i=7, nums[7]=-5
    C->>C: extend = 6-5=1, restart = -5 -> extend WINS (1 > -5)
    Note over C: current_sum = 1 (still running from index 3)
    C->>B: compare 1 vs 6 -> no change
    Note over B: best_sum = 6

    E->>C: i=8, nums[8]=4
    C->>C: extend = 1+4=5, restart = 4 -> extend WINS (5 > 4)
    Note over C: current_sum = 5
    C->>B: compare 5 vs 6 -> no change
    Note over B: best_sum = 6 (FINAL)

    Note over C,B: Result: max_sum = 6, subarray = nums[3..6] = [4, -1, 2, 1]
```

## How to read it

Each "round trip" in the sequence diagram is one loop iteration of the algorithm in [flow-diagram.md](flow-diagram.md): the current element is folded into `current_sum` via the extend-vs-restart comparison, and then `current_sum` is unconditionally compared against `best_sum`. Notice the two **restart** points in this trace — at index 1 (`-2` extending to `1` loses to restarting at `1` alone) and at index 3 (`-2` extending to `4` loses to restarting at `4` alone). Both restarts happen precisely because the running sum being carried forward had gone negative (`-2` in both cases) — exactly the situation the README's Solution section describes: a negative prefix can only subtract from what follows, so abandoning it is always at least as good as keeping it.

Also notice that `best_sum` is compared **every single iteration**, not only at the two restart points — the winning value (`6`, reached at index 6) occurs in the *middle* of an ongoing extend streak (indices 3 through 6), not at a restart boundary. This is exactly the detail called out in this module's Common Mistakes: an implementation that only checks `best_sum` when a restart happens would have compared `4` (at the restart on index 3) and then never checked again until the next restart at some later negative dip — silently missing the true maximum of `6` found mid-run at index 6.
