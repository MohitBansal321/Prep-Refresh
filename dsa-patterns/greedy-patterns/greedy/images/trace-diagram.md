# Greedy — Trace Diagram (Worked Examples)

Two traces: first the **success case** — Activity Selection on the exact seven-interval input asserted in [code.cpp](../code.cpp)'s first test — then a **side-by-side failure case**, greedy coin change with denominations {1, 3, 4} making 6, showing precisely where the greedy choice diverges from the optimum.

## Trace 1 — Interval scheduling (the rule that works)

```
intervals (unsorted): (1,3) (2,4) (3,5) (0,6) (5,7) (3,9) (8,10)
sorted by END time:   (1,3) (2,4) (3,5) (0,6) (5,7) (3,9) (8,10)
index:                   0     1     2     3     4     5     6
rule: keep iff start >= lastEnd;  lastEnd starts at INT_MIN
```

```mermaid
sequenceDiagram
    autonumber
    participant I as interval (sorted by end)
    participant J as Judge (start >= lastEnd?)

    Note over J: Step 0 — count=0, lastEnd=INT_MIN

    I->>J: (1,3)
    J->>J: 1 >= INT_MIN -> KEEP
    Note over J: count=1, lastEnd=3

    I->>J: (2,4)
    J->>J: 2 < 3 -> skip permanently
    Note over J: count=1, lastEnd=3

    I->>J: (3,5)
    J->>J: 3 >= 3 -> KEEP (touching endpoints are NOT overlapping)
    Note over J: count=2, lastEnd=5

    I->>J: (0,6)
    J->>J: 0 < 5 -> skip
    Note over J: count=2, lastEnd=5

    I->>J: (5,7)
    J->>J: 5 >= 5 -> KEEP
    Note over J: count=3, lastEnd=7

    I->>J: (3,9)
    J->>J: 3 < 7 -> skip
    Note over J: count=3, lastEnd=7

    I->>J: (8,10)
    J->>J: 8 >= 7 -> KEEP
    Note over J: count=4, lastEnd=10 -- FINAL ANSWER: 4
```

The exchange argument in action on step 2: `(2,4)` overlaps `(1,3)`, and since `(1,3)` ends no later than anything else available, keeping it instead can never shrink what remains — so skipping `(2,4)` is safe to do *permanently*, with no "what if I had skipped `(1,3)`" computation anywhere.

## Trace 2 — Coin change {1, 3, 4}, target 6 (the rule that fails)

```
denominations: {1, 3, 4}      target: 6
greedy rule:   always take the largest coin that fits (no sort needed --
               descending denomination IS the order)
```

```mermaid
flowchart TD
    subgraph GREEDY["Greedy (largest coin first)"]
        G0["remaining = 6"] --> G1["take 4<br/>remaining = 2"]
        G1 --> G2["take 1<br/>remaining = 1"]
        G2 --> G3["take 1<br/>remaining = 0"]
        G3 --> GA(["ANSWER: 3 coins {4,1,1}"])
    end
    subgraph OPT["Optimum"]
        O0["remaining = 6"] --> O1["take 3<br/>remaining = 3"]
        O1 --> O2["take 3<br/>remaining = 0"]
        O2 --> OA(["ANSWER: 2 coins {3,3}"])
    end
    GA -. "divergence point" .-> O1
```

## How to read it

In Trace 1, every decision is one comparison against one scalar (`lastEnd`), no edge ever goes backward, and each skipped interval is skipped *forever* — the monotonicity of `lastEnd` is why four keeps fall out in a single pass. Two details worth pausing on: step 3 keeps `(3,5)` because `start == lastEnd` counts as compatible (`>=`, not `>` — LeetCode's convention for this problem), and steps 4 and 6 discard intervals that look "longer-lived" than what was kept, which feels wasteful until you recall the proof: an earlier end always leaves at least as much room downstream.

Trace 2 is the same discipline pointed at itself. The divergence marker sits between the two graphs because it happens at the very **first** choice: greedy takes the 4, leaving 2 — which only two 1s can make. The exchange argument dies right there: swapping the 4 into the optimal `{3,3}` does not leave a solution that is no worse, it leaves `{3,1,1}`-at-best — strictly worse. The broken condition is the **greedy choice property** (no optimal solution for 6 contains a 4), while **optimal substructure still holds** — which is exactly why DP works on this problem where greedy does not: DP never assumes any particular first coin is safe, it computes both and compares. The practical lesson: nothing in greedy's code or output announces the failure — both traces terminate normally and print plausible answers. Only the proof (or a differential test against brute force) separates them.
