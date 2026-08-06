# Trace Diagram

A concrete trace of `coinChangeMinCoins` (see [../code.cpp](../code.cpp) and [../problems/01-coin-change.cpp](../problems/01-coin-change.cpp)) on `coins = [1, 2, 5]`, `amount = 11`. `dp[w]` = fewest coins summing to exactly `w`.

## The filled dp array, left to right (the actual forward fill order)

| w | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 |
|---|---|---|---|---|---|---|---|---|---|---|----|----|
| dp[w] | 0 | 1 | 1 | 2 | 2 | 1 | 2 | 2 | 3 | 3 | 2 | **3** |
| via coin | — | 1 | 2 | 1 | 2 | 5 | 1 or 5 | 2 or 5 | 1, 2, or 5 (tie) | 2 or 5 | 5 | 5 |

`dp[11] = 3` — the answer (one 5, one 5, one 1: `5 + 5 + 1 = 11`).

## Dependency graph for computing dp[11]

```mermaid
flowchart LR
    subgraph Filled already, left to right
        d0["dp[0]=0"] --> d1["dp[1]=1"] --> d2["dp[2]=1"] --> d3["dp[3]=2"] --> d4["dp[4]=2"] --> d5["dp[5]=1"] --> d6["dp[6]=2"] --> d7["dp[7]=2"] --> d8["dp[8]=3"] --> d9["dp[9]=3"] --> d10["dp[10]=2"]
    end

    d10 -- "coin 1: dp[10]+1 = 3" --> d11["dp[11] = min(3, 4, 3) = 3"]
    d9 -- "coin 2: dp[9]+1 = 4" --> d11
    d6 -- "coin 5: dp[6]+1 = 3" --> d11
```

## How to read it

The table shows every slot of the dp array **in the order it is actually computed** — left to right, `w = 0` through `w = 11` — because that is exactly the forward fill order the algorithm uses. Nothing to the right of the current `w` has a value yet when `w` is being computed; everything to the left is already final and available to read.

The dependency graph zooms into the very last step, `dp[11]`, and shows all three candidates the `min` considers: reading `dp[10]` (reachable via coin 1: one more coin of value 1 added to however `dp[10]` was best solved), `dp[9]` (via coin 2), and `dp[6]` (via coin 5). Each of `dp[10]`, `dp[9]`, and `dp[6]` was computed **earlier in the same left-to-right pass**, using the exact same forward-fill mechanism — `dp[6]`, for instance, was itself computed as `dp[1] + 1` using coin 5, meaning coin 5 already appears once in the path to `dp[6]`, and appears a **second time** on the edge into `dp[11]`. That second use of the same coin denomination, reachable only because forward-fill lets a later capacity read an earlier capacity that may already contain that coin, is "reuse" made completely concrete: not an abstraction, but two edges in this exact graph both labeled "coin 5."

Contrast this with what would happen under a *backward* fill (0/1 Knapsack's approach, see [../../0-1-knapsack/](../../0-1-knapsack/)): each coin denomination would be fully applied to all capacities from high to low *before* the next denomination is considered at all, and `dp[w - coin]` would then refer to a state that has not yet incorporated the current coin — making a second use of the same coin, within the same item's pass, structurally impossible. The trace above only produces `3` (rather than some larger, wrong value forced by single-use coins) because the fill direction specifically allows `dp[6]` to already carry one copy of coin 5 when it is read again for `dp[11]`.
