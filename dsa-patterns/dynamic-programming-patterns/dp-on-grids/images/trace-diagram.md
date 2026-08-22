# DP on Grids — Trace Diagram (Worked Example)

This traces the exact table states for the **Minimum Path Sum** example used in [code.cpp](../code.cpp) and [problems/02-minimum-path-sum.cpp](../problems/02-minimum-path-sum.cpp):

```
grid (cost per cell):        movement allowed: right or down only
1  3  1
1  5  1
4  2  1
```

The recurrence: `dp[r][c] = min(dp[r-1][c], dp[r][c-1]) + grid[r][c]`, with the start cell seeded directly and out-of-grid neighbors treated as INT_MAX.

## Fill, row by row

**Row 0** — no cell above exists, so each value is forced: accumulate leftward.

```
step (0,0): start cell -> dp = 1
step (0,1): min(INF, 1) + 3 = 4
step (0,2): min(INF, 4) + 1 = 5

dp so far:   1   4   5
             .   .   .
             .   .   .
```

**Row 1** — every cell now has both neighbors available; take the cheaper arrival and add the cell's own cost.

```
step (1,0): no left neighbor -> min(1, INF) + 1 = 2
step (1,1): min(up=4, left=2) + 5 = 7      <- down from 1 beats right from 4
step (1,2): min(up=5, left=7) + 1 = 6      <- down from 5 beats right from 7

dp so far:   1   4   5
             2   7   6
             .   .   .
```

**Row 2** — same rule; the answer accumulates at the bottom-right corner.

```
step (2,0): no left neighbor -> min(2, INF) + 4 = 6
step (2,1): min(up=7, left=6) + 2 = 8      <- left from 6 beats down from 7
step (2,2): min(up=6, left=8) + 1 = 7      <- down from 6 beats right from 8

final dp:    1   4   5
             2   7   6
             6   8   7     <- answer: dp[2][2] = 7
```

The optimal path is `1 -> 3 -> 1 -> 1 -> 1` (right along the top row, then straight down the right column), total **7**. Note it is *not* the path a greedy "always step onto the cheaper neighbor" would find everywhere: at `(1,1)` the greedy choice between stepping onto 2 (below) vs. staying the course is moot here, but at `(0,1)` stepping down onto 1 looks cheaper than right onto 1's neighbor 5 — yet the true optimum goes right first. Local cheapness does not compose; the table is what makes the global answer trustworthy.

## How to read it

Each "step" above is one iteration of the inner loop in [flow-diagram.md](flow-diagram.md): read the already-computed `up` and `left` values, combine them with the current cell's cost, store. Notice three things:

1. **Dependencies are always ready.** When we compute `(1,1)`, its two sources — `(0,1)` (= 4) and `(1,0)` (= 2) — were filled in strictly earlier steps. That is the fill order doing its job; nothing else enforces correctness.
2. **Every dp cell is a completed sub-answer.** `dp[1][1] = 7` means "the cheapest way to get *from the start to* `(1,1)` costs 7" — not anything about continuing onward. The final answer needs no extra logic because the destination's sub-answer *is* the answer.
3. **The losing branch is discarded, not traced.** At `(2,2)`, arriving via `left` (cost 8 + 1 = 9) loses to arriving via `up` (6 + 1 = 7); the algorithm never records or explores that worse path. This discard-per-cell behavior is exactly what collapses exponential path enumeration into one pass — brute force would have walked both routes to the corner.

For contrast, the counting variant of this same grid would run `dp[r][c] = dp[r-1][c] + dp[r][c-1]` instead and produce:

```
counting dp:  1   1   1
              1   2   3
              1   3   6     <- 6 distinct right/down paths, matching C(4,2)
```

Same loops, same fill order, different combination rule — which is why the two flavors share one template.
