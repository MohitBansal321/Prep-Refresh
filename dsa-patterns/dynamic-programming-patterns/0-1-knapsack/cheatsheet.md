# 0/1 Knapsack — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Dynamic Programming pattern — capacity-constrained subset selection. |
| **Recognition Signal** | A set of items each usable **at most once**, a **hard numeric budget/capacity** (weight, money, count — often disguised as "target sum," "partition," or "number of ways"), and an optimization/feasibility/counting question over the chosen subset. The words "weight," "value," "capacity" are usually absent. |
| **Problem** | Brute force enumerates all `2^n` subsets; greedy by value/weight ratio is provably wrong for 0/1 (correct only for fractional); the naive recursion recomputes the same `(items considered, remaining capacity)` state exponentially many times. |
| **Solution** | DP table where `dp[i][w]` = best value using the first `i` items with capacity `w`: `dp[i][w] = max(dp[i-1][w], dp[i-1][w - weight[i]] + value[i])` when the item fits, else copy `dp[i-1][w]`. Items outer, capacity inner; every cell reads only the previous, frozen row. Space-optimize to one row by sweeping capacity **backward**. |
| **Time / Space Complexity** | O(n · capacity) time for both 2D and 1D versions — **pseudo-polynomial** in the numeric value of capacity, not its bit-length. Space: O(n · capacity) for the full table, O(capacity) with the 1D reverse sweep. Brute force is O(2^n). |
| **Pros** | Exponential-to-polynomial jump over brute force · always correct, unlike ratio-greedy · space-optimizable to O(capacity) with no loss of correctness · recurrence doubles as a correctness proof · generalizes to an entire family via reformulation (feasibility: replace max with OR; counting: replace max with +=; multi-axis capacity). |
| **Cons** | Pseudo-polynomial — astronomically large capacity makes the table infeasible even though the number fits in 32 bits · recovering *which* items were chosen needs the full 2D table or extra bookkeeping · requires integer/discretizable weights · greedy fallback under time pressure has no approximation guarantee. |
| **Use When** | Items are indivisible, usable at most once, under a modest numeric budget · subset-sum feasibility ("does some subset hit exactly X?") · partition-minimization ("closest split of total") · counting subsets achieving a property · capacity realistically in the tens of thousands to low millions. |
| **Avoid When** | Items are reusable (Unbounded Knapsack — forward sweep, reads same row) · capacity is huge relative to n (meet-in-the-middle or approximation instead) · you need all subsets enumerated, not one optimal number (plain Subsets pattern) · no resource-under-budget axis exists at all. |
| **Related Patterns** | Unbounded Knapsack (`../unbounded-knapsack/`) — reuse allowed; recurrence reads `dp[i][...]` and the 1D sweep goes **forward** · Subsets (`../../recursion-backtracking-patterns/subsets/`) — the brute force this DP replaces · Bounded Knapsack — finite per-item limits, solvable by duplication into 0/1. |

### Template Skeleton

```cpp
// 2D table version — dp[i][w]: best value, first i items, capacity w.
// dp is (n+1) x (capacity+1), zero-initialized = base row/column.
for (int i = 1; i <= n; ++i) {
    for (int w = 0; w <= capacity; ++w) {
        dp[i][w] = dp[i - 1][w];                       // skip item i-1
        if (weights[i - 1] <= w) {                     // note i-1 shift!
            dp[i][w] = std::max(dp[i][w],
                dp[i - 1][w - weights[i - 1]] + values[i - 1]);
        }
    }
}
// answer: dp[n][capacity]

// 1D space-optimized — MUST sweep capacity BACKWARD (high to low).
std::vector<int> dp(capacity + 1, 0);
for (size_t i = 0; i < weights.size(); ++i) {
    for (int w = capacity; w >= weights[i]; --w) {
        dp[w] = std::max(dp[w], dp[w - weights[i]] + values[i]);
    }
}
// answer: dp[capacity]
```

### Remember In One Sentence
> **0/1 Knapsack replaces the O(2^n) enumeration of every subset with an O(n · capacity) table whose single rule — each row built only from the frozen row above it — enforces "each item at most once," and whose 1D compression survives only if you sweep capacity backward, because sweeping forward silently turns the problem into Unbounded Knapsack.**

### Two Facts People Get Wrong
- Greedy by value/weight ratio is a fast approximation worth trying when the DP is too slow? **No** — it has no bounded error for 0/1 (the classic 3-item counterexample loses over 20% of optimal); it is only correct for the *fractional* variant, where the exchange argument works because partial items can be traded.
- O(n · capacity) means the algorithm scales polynomially with input size? **No** — it is pseudo-polynomial: capacity of 1,000,000,000 takes ~30 bits to write down but ~a billion table columns to compute, so the running time grows with a number's *magnitude*, not its encoded length.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the exact recurrence for `dp[i][w]`, including what happens when item `i` does not fit, and both base cases.
2. Why does reading only from row `i - 1` (never row `i`) enforce the 0/1 constraint? What would reading row `i` change the problem into?
3. In the 1D space-optimized version, why must capacity be swept backward? Trace concretely what goes wrong sweeping forward with one item of weight 1, value 10, capacity 3.
4. Give the 3-item counterexample showing ratio-greedy fails for 0/1 Knapsack, and explain precisely which step of the fractional exchange argument collapses.
5. What does pseudo-polynomial mean, and why does a capacity that fits comfortably in a 32-bit integer still make the DP infeasible?
6. How do the recurrence's operator and base values change across the three framings: maximize value (knapsack proper), feasibility (Partition Equal Subset Sum), and counting (Target Sum)?
7. Why do you index `weights[i - 1]` inside the loop over row `i`, and what bug appears if you write `weights[i]`?
8. What extra cost does recovering the actual chosen subset impose, and why can't the 1D-optimized version do it alone?
9. How does Ones and Zeroes generalize the pattern's notion of "capacity," and how does the reverse-sweep rule extend to it?
10. Name the two structural differences between 0/1 and Unbounded Knapsack (recurrence read + sweep direction), and explain why they are two views of the same mechanism.
