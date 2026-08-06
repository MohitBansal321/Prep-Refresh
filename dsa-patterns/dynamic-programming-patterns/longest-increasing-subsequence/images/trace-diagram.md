# Longest Increasing Subsequence — Trace Diagram

Tracing `tails` as the classic example `[10, 9, 2, 5, 3, 7, 101, 18]` is processed one number at a time.

```mermaid
sequenceDiagram
    autonumber
    participant N as Numbers
    participant T as tails

    N->>T: 10 (tails empty -> append)
    Note over T: tails = [10]

    N->>T: 9 (9 < 10 -> lower_bound finds 10 -> replace)
    Note over T: tails = [9]

    N->>T: 2 (2 < 9 -> replace)
    Note over T: tails = [2]

    N->>T: 5 (5 > 2, no entry >= 5 -> append)
    Note over T: tails = [2, 5]

    N->>T: 3 (3 < 5 -> replace tails[1])
    Note over T: tails = [2, 3]

    N->>T: 7 (7 > 3, no entry >= 7 -> append)
    Note over T: tails = [2, 3, 7]

    N->>T: 101 (101 > 7 -> append)
    Note over T: tails = [2, 3, 7, 101]

    N->>T: 18 (18 < 101 -> replace tails[3])
    Note over T: tails = [2, 3, 7, 18]

    Note over T: Final tails.size() = 4 -- LIS length is 4
```

**How to read it:** watch how `tails` never actually holds one single real subsequence at any point — by the end it reads `[2, 3, 7, 18]`, which *does* happen to be a valid increasing subsequence of the original array here, but that's a coincidence of this particular example, not a guarantee. What's guaranteed is only the *length* (4) and the *invariant* that `tails[k]` is always the smallest possible tail value achievable by any increasing subsequence of length `k+1` seen so far. Each "replace" (9, 2, 3, 18) makes some future extension easier by lowering a tail value; each "append" (10, 5, 7, 101) is the only action that actually grows the answer.
