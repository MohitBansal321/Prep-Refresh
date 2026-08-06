# Two Pointers — Trace Diagram (Worked Example)

This traces the exact pointer positions and comparisons for the **Pair with Target Sum** example used in [problems/01-pair-with-target-sum.cpp](../problems/01-pair-with-target-sum.cpp):

```
numbers = [1, 2, 3, 4, 6, 8, 9, 14, 15]   (0-indexed)
index:      0  1  2  3  4  5  6   7   8
target  = 13
```

```mermaid
sequenceDiagram
    autonumber
    participant L as left pointer
    participant R as right pointer
    participant J as Judge (compare sum to target=13)

    Note over L,R: Step 0 — initialize: left=0 (value 1), right=8 (value 15)

    L->>J: value = 1
    R->>J: value = 15
    J->>J: sum = 1 + 15 = 16 > 13 (too large)
    J-->>R: move right inward
    Note over R: right = 7 (value 14)

    L->>J: value = 1
    R->>J: value = 14
    J->>J: sum = 1 + 14 = 15 > 13 (too large)
    J-->>R: move right inward
    Note over R: right = 6 (value 9)

    L->>J: value = 1
    R->>J: value = 9
    J->>J: sum = 1 + 9 = 10 < 13 (too small)
    J-->>L: move left inward
    Note over L: left = 1 (value 2)

    L->>J: value = 2
    R->>J: value = 9
    J->>J: sum = 2 + 9 = 11 < 13 (too small)
    J-->>L: move left inward
    Note over L: left = 2 (value 3)

    L->>J: value = 3
    R->>J: value = 9
    J->>J: sum = 3 + 9 = 12 < 13 (too small)
    J-->>L: move left inward
    Note over L: left = 3 (value 4)

    L->>J: value = 4
    R->>J: value = 9
    J->>J: sum = 4 + 9 = 13 == 13 (MATCH)
    Note over L,R: Found: indices (3, 6) 0-indexed -> (4, 7) 1-indexed per LeetCode's contract
```

## How to read it

Each "round trip" in the sequence diagram is one loop iteration of the algorithm in [flow-diagram.md](flow-diagram.md): both pointers report their current value to the "Judge" (the comparison logic), the judge computes the sum, and exactly one pointer is told to move — never both in the same step (except on a match, where the search simply ends). Notice the **monotonic narrowing**: `right` only ever decreases (15 -> 14 -> 9) and `left` only ever increases (1 -> 2 -> 3 -> 4); neither pointer ever backtracks. That monotonicity is precisely why the algorithm terminates in at most `n` steps and why it can never accidentally skip past the answer — every value discarded (15, 14, 1, 2, 3) is discarded because it is mathematically provable that it cannot be part of a valid pair given everything already ruled out.

Also notice this trace needed **six pointer moves across both directions** before converging on the answer — a deliberately less trivial example than "the first comparison happens to match," to make clear that both pointers actively participate in narrowing the search, not just one of them.
