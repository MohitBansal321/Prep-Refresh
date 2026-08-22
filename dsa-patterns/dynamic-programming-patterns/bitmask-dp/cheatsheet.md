# Bitmask DP — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Dynamic Programming pattern — subset-state technique. |
| **Recognition Signal** | `n` is small (**`n <= ~20`**, sometimes `~24`) **and** the subproblem depends on *which specific* items have been used, not merely how many — visiting every city exactly once, assigning workers to tasks, partitioning into equal-sum groups, turn-based games over a shrinking pool of numbered choices. |
| **Problem** | Plain DP indexes states by prefix/count (`dp[i][w]`), but here two different choices of the same size lead to genuinely different futures — there is no ordering of the items under which "everything before me" captures the relevant history. Brute force over permutations costs `O(n!)`. |
| **Solution** | Encode the used-set as an `n`-bit integer `mask`; allocate a table over all `2^n` masks (`dp[mask]`, or `dp[mask][last]` when the most recent choice matters). Transitions extend the subset by one unset bit: `dp[mask | (1<<i)]` from `dp[mask]`. Iterating masks in increasing numeric order guarantees dependencies are ready, because setting a bit strictly increases the integer. |
| **Time-Space Complexity** | Time `O(2^n * n)` for simple shapes, `O(2^n * n^2)` when a `last` dimension is needed (TSP shape). Space `O(2^n)` or `O(2^n * n)` for the table — plus recursion stack if top-down. |
| **Pros** | Turns `O(n!)` permutation search into polynomial-in-`2^n` DP · exact answers where greedy/heuristics fail · uniform, hard-to-get-wrong iteration order · composes cleanly with memoization, BFS-over-states, and game-theory negamax · `__builtin_popcount`/bit tests make state queries O(1). |
| **Cons** | Exponential space — `n = 24` already means 16M+ states per dimension · completely intractable past `n ≈ 20-25` · easy to confuse "count needed" (use Knapsack) with "identity needed" (use bitmask) · subset-enumeration transitions (submasks of a mask) add another `3^n` factor if used carelessly. |
| **Use When** | Small `n` + assignment/partition/permutation/game problem whose future depends on the exact chosen set · counting valid arrangements position-by-position (Beautiful Arrangement) · optimal partitioning into `k` groups (Partition to K Equal Sum Subsets, Fair Distribution of Cookies) · adversarial turn games with finite removable resources (Can I Win). |
| **Avoid When** | `n > ~24` (`2^n` explodes — think greedy, graph algorithms, or meet-in-the-middle instead) · state only needs *how many* items used, not *which* (plain 0/1 Knapsack is smaller and simpler) · the problem asks to enumerate all solutions rather than count/optimize (backtracking without memoization is enough). |
| **Related Patterns** | 0/1 Knapsack (same include/exclude shape, count-only state) · Subsets enumeration (the generator bitmask DP memoizes) · Backtracking (same recursion tree, minus the memo table) · Game Theory minimax (bitmask encodes the shared, finite game state). |

### Template Skeleton

```cpp
// Shape 1 — flat: dp[mask] depends only on which items are used.
std::vector<long long> dp(1 << n, -1);   // -1 = not computed (top-down)
long long solve(int mask) {
    if (mask == (1 << n) - 1) return baseValue();   // all items used
    long long& ans = dp[mask];
    if (ans != -1) return ans;
    for (int i = 0; i < n; ++i) {
        if (mask & (1 << i)) continue;              // item i already used
        ans = std::min(ans, cost + solve(mask | (1 << i)));
    }
    return ans;
}

// Shape 2 — TSP-style: dp[mask][last] also tracks the most recent choice.
for (int mask = 1; mask < (1 << n); ++mask)
    for (int last = 0; last < n; ++last) {
        if (!(mask & (1 << last))) continue;        // last must be in mask
        for (int next = 0; next < n; ++next) {
            if (mask & (1 << next)) continue;       // already visited
            dp[mask | (1 << next)][next] =
                std::min(dp[mask | (1 << next)][next], dp[mask][last] + w);
        }
    }
```

### Remember In One Sentence
> **Bitmask DP makes "which subset of a small set has been used" a legal array index by packing it into the bits of an integer, converting exponential-permutation search into a `2^n`-state table filled in strictly increasing numeric order — because setting a bit always produces a larger number.**

### Two Facts People Get Wrong
- Bitmask DP is just "DP with bit tricks" and helps whenever bits appear? **No** — it is specifically a *state-space* technique for subset-dependent subproblems; if your state only needs a count or a running total, a mask adds `2^n` blowup for zero benefit and 0/1 Knapsack is the right tool.
- You need special handling to visit masks in dependency order? **No** — `mask | (1 << i)` is always strictly greater than `mask`, so a plain `for (mask = 0; mask < (1<<n); ++mask)` bottom-up loop, or ordinary memoized recursion, automatically respects the DAG; no topological sort or unordered_map iteration needed.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the two conditions that must hold simultaneously before reaching for bitmask DP — and name the pattern you should use when only the second fails.
2. Why does iterating masks in increasing numeric order guarantee every transition's dependency is already computed? Which bit operation creates the "strictly larger" guarantee?
3. When do you need the extra `last` dimension (`dp[mask][last]`) beyond `dp[mask]`, and what does the time complexity become?
4. What is the time and space complexity of the TSP-shaped DP for `n = 20`? Roughly how many entries does the table have?
5. In Partition to K Equal Sum Subsets, why does storing `(running sum) % target` in `dp[mask]` work even though several different placement histories map to the same mask?
6. How does a game-theory problem like Can I Win encode its state as a mask, and what single fact lets you memoize on the mask alone without tracking whose turn it is?
7. What does `__builtin_popcount(mask)` compute, and in Beautiful Arrangement, what does that value tell you about which position you are filling next?
8. Give the brute-force complexity that bitmask DP replaces for: (a) TSP, (b) counting valid beautiful arrangements, (c) checking all partitions into k groups.
9. Why is `int` safe for masks up to `n <= 20` but risky near `n = 31`, and what would you change?
10. What distinguishes bitmask DP from plain backtracking over the same choice tree — both explore subsets — and when is backtracking actually the better tool?

