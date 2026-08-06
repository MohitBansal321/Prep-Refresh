# Bitmask DP

## Intent

Extend DP's "table of subproblems" idea to state that depends on **which specific subset** of a small set of items has been used so far — not just how many, or which prefix — by encoding that subset as the bits of an integer, making `2^n` distinct states enumerable and indexable in `O(1)`.

## Recognition Signal

`n` is small (roughly `n <= 20`, sometimes up to `~24`), and the subproblem genuinely depends on **which** elements have been chosen, not merely how many — e.g. "visit every city exactly once" (a specific *set* of visited cities matters, not just a count), "partition these numbers into k subsets with equal sum," or "assign these workers to these tasks to minimize cost." If the state only needs a count (how many items used) rather than an identity (which items), plain 0/1 Knapsack-style DP ([../0-1-knapsack/](../0-1-knapsack/)) is enough and a bitmask is unnecessary overhead.

## Core Idea

Represent "which elements have been used" as an `n`-bit integer `mask`, where bit `i` is 1 if element `i` has been used. Because `n` is small, all `2^n` possible subsets fit as array indices, so `dp[mask]` (or `dp[mask][last]` when the *most recent* choice also matters, as in path problems) is a valid, directly-indexable DP table. Transitions move from a smaller subset to a larger one: for each unset bit `i` in `mask` (checked via `(mask & (1 << i)) == 0`), transition to `mask | (1 << i)` — a strictly larger integer, which is exactly why iterating masks in increasing numeric order guarantees every dependency (`mask` before `mask | (1<<i)`) is already computed when needed. This is `Subsets`' enumeration ([../../recursion-backtracking-patterns/subsets/](../../recursion-backtracking-patterns/subsets/)) with a memo table bolted on: instead of just generating every subset, each subset also carries a computed value.

## Template

```cpp
// Traveling-salesman-style: dp[mask][last] = min cost to have visited
// exactly the set `mask` of cities, currently standing at city `last`.
int minHamiltonianCost(const std::vector<std::vector<int>>& cost) {
  int n = static_cast<int>(cost.size());
  const int kInf = std::numeric_limits<int>::max() / 2;
  std::vector<std::vector<int>> dp(1 << n, std::vector<int>(n, kInf));

  dp[1][0] = 0;  // start at city 0, having visited only {0}

  for (int mask = 1; mask < (1 << n); ++mask) {
    for (int last = 0; last < n; ++last) {
      if (!(mask & (1 << last)) || dp[mask][last] == kInf) continue;

      for (int next = 0; next < n; ++next) {
        if (mask & (1 << next)) continue;  // already visited
        int nextMask = mask | (1 << next);
        dp[nextMask][next] = std::min(dp[nextMask][next],
                                       dp[mask][last] + cost[last][next]);
      }
    }
  }

  int full = (1 << n) - 1;
  int best = kInf;
  for (int last = 0; last < n; ++last) {
    if (dp[full][last] < kInf) {
      best = std::min(best, dp[full][last] + cost[last][0]);  // return to start
    }
  }
  return best;
}
```

## Complexity

**Time:** `O(2^n * n^2)` for the TSP shape above (every mask, every `last`, every `next`) — `O(2^n * n)` for shapes without the extra `last` dimension.
**Space:** `O(2^n * n)` (or `O(2^n)` without the `last` dimension) for the DP table.

## Common Mistakes

- **Iterating masks out of numeric order**, or recursing without memoization ordering guarantees. `mask | (1 << i)` is always strictly greater than `mask`, so a simple `for (mask = 0; mask < (1<<n); ++mask)` forward loop is sufficient and correct — no separate topological handling is needed, but skipping the increasing-order guarantee (e.g. processing masks in an arbitrary order via an unordered structure) breaks it.
- **Using `int` for a mask when `n` gets close to 31**, or not sizing the DP table to `1 << n` correctly — off-by-one on the shift amount silently truncates the state space.
- **Recomputing `__builtin_popcount(mask)` or looping over all `n` bits repeatedly inside a hot inner loop** when it could be hoisted out — a minor performance trap, not a correctness one, but it compounds at `2^20`+ states.
- **Forgetting the base case entirely** (e.g. `dp[1][0] = 0` above) — every transition depends on some seed state being valid before the first real transition can fire.

## When To Use

- Subproblems keyed by "which subset of a small (`n <= ~20`) set of items/cities/tasks has been used" — Hamiltonian path/TSP variants, set-partition problems, assignment problems where cost depends on which specific items are already paired off.

## When NOT To Use

- **`n` is larger than ~20-24** — `2^n` becomes intractable; the problem needs a different technique entirely (a greedy heuristic, a graph-specific polynomial algorithm, or approximation).
- **State only needs a count of items used, not their identity** — plain 0/1 Knapsack-style `dp[i][capacity]` ([../0-1-knapsack/](../0-1-knapsack/)) is simpler and uses far less memory.

## Similar Patterns

- **0/1 Knapsack** ([../0-1-knapsack/](../0-1-knapsack/)): same "include or exclude each item once" shape, but the state only needs a *count* (or running total), never *which* items — swap to bitmask DP only when the identity of the chosen subset itself matters to future transitions.
- **Subsets** ([../../recursion-backtracking-patterns/subsets/](../../recursion-backtracking-patterns/subsets/)): the enumeration this pattern memoizes — bitmask DP is Subsets plus a value cached per subset.

## Further Reading

- LeetCode — Partition to K Equal Sum Subsets (698), Shortest Path Visiting All Nodes (847), Minimum Cost to Connect Two Groups of Points (1595), Smallest Sufficient Team (1125).
- *Introduction to Algorithms* (CLRS) — the Traveling Salesman Problem dynamic-programming formulation.
