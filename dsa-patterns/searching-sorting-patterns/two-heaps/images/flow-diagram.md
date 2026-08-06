# Two Heaps — Flow Diagram

```mermaid
flowchart TD
    Start([New number arrives]) --> Check{low_ is empty,<br/>or number <= low_.top()?}
    Check -- Yes --> PushLow[Push into low_<br/>-- max-heap, lower half]
    Check -- No --> PushHigh[Push into high_<br/>-- min-heap, upper half]
    PushLow --> Balance{low_.size &gt; high_.size + 1?}
    PushHigh --> Balance2{high_.size &gt; low_.size?}
    Balance -- Yes --> MoveToHigh[Pop low_'s top,<br/>push it into high_]
    Balance -- No --> Done
    Balance2 -- Yes --> MoveToLow[Pop high_'s top,<br/>push it into low_]
    Balance2 -- No --> Done
    MoveToHigh --> Done([Sizes differ by at most 1 --<br/>median is ready to read])
    MoveToLow --> Done
    Done --> ReadMedian{low_.size == high_.size?}
    ReadMedian -- Yes, even total --> Avg["median = (low_.top + high_.top) / 2"]
    ReadMedian -- No, low_ has one extra --> Single["median = low_.top"]
```

**How to read it:** every insertion is exactly two steps — route the new number to whichever half it belongs in (top branch), then restore the size balance if that push made one heap more than one element larger than the other (middle branch). The bottom branch is the payoff: because the invariant "sizes differ by at most 1, and every value in low_ is <= every value in high_" is restored after every single insertion, reading the median is always an O(1) peek at one or both heap tops — no scanning, no re-sorting.
