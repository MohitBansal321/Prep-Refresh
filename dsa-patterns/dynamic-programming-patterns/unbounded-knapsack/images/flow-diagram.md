# Flow Diagram

The control flow of the 1D dp forward-fill loop that is the entire mechanical core of Unbounded Knapsack, with the contrast against 0/1 Knapsack's backward fill made explicit at the one point where they diverge.

```mermaid
flowchart TD
    Init["Initialize dp[0..capacity]\ndp[0] = base case (0, or 1 for counting)\nall other dp[w] = 0 / unreachable-sentinel"] --> OuterStart

    subgraph Unbounded["Unbounded Knapsack: capacity FORWARD"]
        direction TB
        OuterStart["w = 1"] --> OuterCheck{"w <= capacity?"}
        OuterCheck -- Yes --> InnerLoop["For each item i with weight[i] <= w:\ndp[w] = combine(dp[w], dp[w - weight[i]] + value[i])"]
        InnerLoop --> Note1["dp[w - weight[i]] may ALREADY reflect\nitem i used earlier in THIS SAME pass\n(w - weight[i] < w, already processed)\n=> item i can be reused"]
        Note1 --> Incr["w = w + 1"]
        Incr --> OuterCheck
        OuterCheck -- No --> Done(["Answer is in dp[capacity]"])
    end

    Init -.->|"contrast: 0/1 Knapsack instead\nloops w from capacity DOWN to weight[i]\nfor EACH item, one item at a time"| ZeroOneNote["0/1 Knapsack (backward fill):\ndp[w - weight[i]] read here is from\nBEFORE item i was considered at all\n=> item i used at most once\nSee ../../0-1-knapsack/"]
```

## How to read it

Follow the `Unbounded` subgraph top to bottom first — this is the loop that appears, in some form, in every file in this module (`code.cpp`'s `unboundedKnapsack`/`coinChangeMinCoins`, and all four files in `problems/`). The critical box is `Note1`: because `w` increases and the inner update for capacity `w` reads `dp[w - weight[i]]`, and `w - weight[i]` is strictly smaller than `w`, that smaller-capacity slot **has already been updated during this same forward pass** — possibly using item `i` itself. That is the entire mechanism by which "reuse" happens. There is no explicit "use item i again" instruction anywhere in the code; reuse is a *consequence* of the fill direction, not a separate step.

The dashed arrow and the `ZeroOneNote` box exist purely as a contrast, not as part of this pattern's own flow: 0/1 Knapsack, when using the same style of 1D-array optimization, must iterate `w` **backward** (from `capacity` down to `weight[i]`) for each item, one item fully processed before the next. Filling backward means `dp[w - weight[i]]` is read while it still holds the value from **before** the current item was considered at all in this row — guaranteeing at most one use per item. If you accidentally fill Unbounded Knapsack backward, or 0/1 Knapsack forward, you silently get the other pattern's semantics without any error or warning — see the main [README](../README.md)'s Common Mistakes section, which calls this out as the single most important thing to get right in this whole module.
