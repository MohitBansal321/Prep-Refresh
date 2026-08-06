# Modified Binary Search — Flow Diagram

This traces the control flow of the general lo/hi/mid halving loop — the shape shared by classic exact-match search, first/last-occurrence (boundary) search, and rotated-array search. The only thing that changes between variants is what happens inside the "which half is valid?" box. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual numbers.

```mermaid
flowchart TD
    Start([Start: sorted or piecewise-sorted array,<br/>a target/predicate to test against]) --> Init["Initialize lo = 0<br/>hi = n - 1"]
    Init --> Loop{lo <= hi?}

    Loop -- "No, range is empty" --> NotFound([Return: not found,<br/>or return the best boundary<br/>candidate recorded so far])

    Loop -- Yes --> Mid["mid = lo + (hi - lo) / 2<br/>(never (lo + hi) / 2 — avoids overflow)"]

    Mid --> Test{"Which half is valid?<br/>(the problem-specific predicate:<br/>exact match test, boundary test,<br/>or 'which side is normally ordered' test)"}

    Test -- "Exact match found<br/>(classic search only)" --> Found([Return mid])

    Test -- "Answer (if any) is<br/>in the LEFT half,<br/>discard the right" --> MoveHi["hi = mid - 1<br/>(or hi = mid if mid itself<br/>must stay a candidate)"]

    Test -- "Answer (if any) is<br/>in the RIGHT half,<br/>discard the left" --> MoveLo["lo = mid + 1"]

    Test -- "Match found, but this is a<br/>BOUNDARY search — record mid<br/>as a candidate, keep narrowing" --> Record["Record mid as best-so-far,<br/>then narrow into whichever<br/>side finds an earlier/later one"]

    MoveHi --> Loop
    MoveLo --> Loop
    Record --> Loop
```

## How to read it

The single loop condition `lo <= hi` is the entire lifetime of the algorithm — every iteration does a fixed amount of O(1) work (compute `mid`, run the predicate, move one pointer), and the loop runs at most `log2(n)` times because the range `[lo, hi]` is cut roughly in half every iteration, never scanned element by element. That is the whole argument for the O(log n) time bound: **the search space shrinks multiplicatively, not by a fixed amount**, which is the fundamental difference from a linear scan (where the space shrinks by exactly one element per step).

The box labeled "Which half is valid?" is where the four variants in [code.cpp](../code.cpp) diverge from each other, and it is the box worth studying hardest for any *new* problem you meet: classic search tests `nums[mid]` against `target` directly; boundary search (first/last occurrence) tests `nums[mid]` against `target` too, but on a match it does **not** stop — it records the candidate and keeps going, because "found *a* match" is not the same question as "found the *first* match"; rotated-array search cannot compare `nums[mid]` to `target` alone, because the array is not globally ordered, so it first asks "which half, [lo..mid] or [mid..hi], is internally sorted?" and only then checks whether `target`'s value falls inside that sorted half's range. In every case, exactly one of `lo` or `hi` moves per iteration (or `mid` is recorded and narrowing continues) — never both, and never neither, or the loop either misses the answer or never terminates.
