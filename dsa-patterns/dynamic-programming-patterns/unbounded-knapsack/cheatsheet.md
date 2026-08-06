# Unbounded Knapsack — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Dynamic Programming pattern — 1D capacity-indexed dp table, "take-or-skip with reuse allowed." |
| **Recognition Signal** | Items/coins/pieces with a weight/value (or just a size) and a capacity or exact target, where **each item can be used any number of times** (unlimited supply) — contrast with 0/1 Knapsack's "each item used at most once." |
| **Problem** | Brute-force recursion re-explores the same remaining-capacity state through every distinct order of item choices that lands on it — exponential time without memoization (overlapping subproblems). |
| **Solution** | 1D `dp` table indexed by capacity/target. `dp[w] = combine(dp[w], dp[w - weight[i]] + contribution)`, filled with `w` **FORWARD** (increasing) so `dp[w - weight[i]]` may already reflect a prior use of item `i` in this same pass — that forward fill IS the reuse mechanism. |
| **Time / Space Complexity** | O(n * capacity) time, O(capacity) space — n items/coins, capacity forward-filled once. Same asymptotic class as 0/1 Knapsack; only the fill direction differs. |
| **Pros** | Turns exponential brute force into O(n*capacity) · no recursion/call-stack needed · one recurrence shape covers maximize/minimize-exact/count-exact by swapping only the combine rule · provably correct (unlike greedy). |
| **Cons** | Still pseudo-polynomial (huge capacity value alone can make it infeasible, even with few items) · trivially easy to accidentally implement 0/1 semantics via wrong fill direction · counting-combinations variants need the item loop OUTSIDE the capacity loop, or they silently count permutations instead. |
| **Use When** | Maximize value under a reusable-item capacity (rod cutting) · minimum pieces to hit an exact target (coin change, perfect squares) · count distinct ways to hit an exact target (coin change II) · reuse shows up structurally even without an explicit "unlimited" item list (integer break, word break). |
| **Avoid When** | Items are limited-use (→ 0/1 Knapsack, [../0-1-knapsack/](../0-1-knapsack/)) · comparing two separate sequences (→ LCS family) · capacity value itself is astronomically large regardless of item count (pseudo-polynomial ceiling) · a *provably* correct greedy exists for your specific item/coin system (rare — don't assume it without proof). |
| **Related Patterns** | 0/1 Knapsack (same recurrence shape, backward fill, no reuse) · Bounded Knapsack (reuse capped at a fixed count `k`, a middle ground) · LCS family (2D over two sequences, not one capacity axis) · DP on Grids (2D over spatial position, not abstract capacity). |

### Template Skeleton

```cpp
// Maximize value (rod cutting), capacity FORWARD fill
std::vector<long long> dp(capacity + 1, 0);   // dp[0] = 0 is a real base case
for (int w = 1; w <= capacity; ++w) {
    for (int i = 0; i < n; ++i) {
        if (weights[i] <= w) {
            dp[w] = std::max(dp[w], dp[w - weights[i]] + values[i]);
        }
    }
}
// answer: dp[capacity]

// Minimize count to an EXACT target (coin change)
const int UNREACHABLE = INT_MAX / 2;
std::vector<int> dp(amount + 1, UNREACHABLE);
dp[0] = 0;                                    // 0 coins needed to make amount 0
for (int w = 1; w <= amount; ++w) {
    for (int c : coins) {
        if (c <= w && dp[w - c] != UNREACHABLE) {
            dp[w] = std::min(dp[w], dp[w - c] + 1);
        }
    }
}
// answer: dp[amount] == UNREACHABLE ? -1 : dp[amount]

// Count DISTINCT COMBINATIONS to an exact target (coin change II)
// NOTE the loop order: coin OUTER, amount INNER — this is what makes it
// count combinations instead of permutations.
std::vector<long long> dp(amount + 1, 0);
dp[0] = 1;                                    // exactly 1 way to make 0: use nothing
for (int c : coins) {
    for (int w = c; w <= amount; ++w) {
        dp[w] += dp[w - c];
    }
}
// answer: dp[amount]
```

### Remember In One Sentence
> **Unbounded Knapsack is 0/1 Knapsack's exact recurrence with one change — the capacity axis is filled FORWARD instead of backward, so a smaller, already-finalized capacity may already include a previous use of the very item being considered, which is the entire mechanism that makes reuse possible.**

### Two Facts People Get Wrong
- Unbounded Knapsack is **slower or more complex** than 0/1 Knapsack? **No** — same O(n * capacity) time, same O(capacity) space; the only difference is a fill-direction flip, which is a correctness detail, not a performance one.
- Counting distinct combinations (Coin Change II) uses the **same loop order** as minimizing coins (Coin Change)? **No** — minimize/maximize aggregations are loop-order-invariant, but counting *combinations* specifically requires the item/coin loop on the outside, or you silently count *permutations* instead — a different, larger, wrong number.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the recognition signal that means "reach for Unbounded Knapsack" instead of 0/1 Knapsack.
2. Why does filling the capacity axis **forward** allow an item to be reused, while filling it **backward** (as 0/1 Knapsack does) prevents reuse?
3. What is `dp[0]` for the maximize-value flavor, and why is that a genuinely correct base case rather than a sentinel?
4. What is `dp[0]` for the minimize-count-to-exact-target flavor, and why do all *other* unreached capacities need a different starting value than `dp[0]`?
5. What is `dp[0]` for the count-distinct-combinations flavor, and in one sentence, why is it not `0`?
6. In Coin Change II, why does putting the coin loop on the outside (rather than the capacity loop) count combinations instead of permutations?
7. Give the brute-force complexity Unbounded Knapsack DP replaces, and explain in one sentence why that brute force is exponential rather than merely slow.
8. Name one real problem (from [problems/](problems/) or the README) where "reuse" appears structurally in the recurrence without the problem ever saying "unlimited supply" explicitly.
9. Why is greedy not a safe default replacement for this pattern, even though it is asymptotically faster when it happens to work?
10. What single implementation change turns this module's code into 0/1 Knapsack's code, and why does that one change remove the possibility of reuse?
