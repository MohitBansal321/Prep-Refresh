# Two Heaps — Trace Diagram

Tracing the stream `41, 35, 62, 5, 97, 108` (from LeetCode 295's classic example) through both heaps, one insertion at a time.

```mermaid
sequenceDiagram
    autonumber
    participant S as Stream
    participant L as low_ (max-heap)
    participant H as high_ (min-heap)

    S->>L: insert 41 (low_ empty -> goes to low_)
    Note over L,H: low_=[41] high_=[] -- median = 41

    S->>H: insert 35 (35 <= low_.top=41 -> low_, then rebalance moves 35... )
    Note over L,H: low_=[35] high_=[41] -- median = (35+41)/2 = 38.0

    S->>H: insert 62 (62 > low_.top=35 -> high_)
    Note over L,H: low_=[35] high_=[41,62] -- sizes 1 vs 2, rebalance moves 41 to low_
    Note over L,H: low_=[35,41] high_=[62] -- median = 41

    S->>L: insert 5 (5 <= low_.top=41 -> low_)
    Note over L,H: low_=[5,35,41] high_=[62] -- sizes 3 vs 1, rebalance moves 41 to high_
    Note over L,H: low_=[5,35] high_=[41,62] -- median = (35+41)/2 = 38.0

    S->>H: insert 97 (97 > low_.top=35 -> high_)
    Note over L,H: low_=[5,35] high_=[41,62,97] -- rebalance moves 41 to low_
    Note over L,H: low_=[5,35,41] high_=[62,97] -- median = 41

    S->>H: insert 108 (108 > low_.top=41 -> high_)
    Note over L,H: low_=[5,35,41] high_=[62,97,108] -- median = 41
```

**How to read it:** watch how the size-balancing step (rebalance) fires every time one heap gets more than one element ahead of the other — it always moves exactly one element across, restoring the "differ by at most 1" invariant before the next number is read. The `low_` column always holds the smaller half (its *top*, via the max-heap ordering, is the largest of the small values — i.e. the boundary right below the median), and `high_`'s top is the smallest of the large values — the boundary right above the median. Reading the median is just looking at one or both of those two boundary values.
