# Bitmask DP — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | DP pattern — memoise over **subsets of a small set**, using an integer as the subset key. |
| **Recognition Signal** | **n is suspiciously small (≤ 20)**, and the state you need to remember is *"which of these have I already used?"* — not how many, not the last one, but the exact set. Assignment/matching, travelling-salesman, and "partition into k groups" problems. |
| **Problem** | The subproblem depends on the **set** of consumed items, and there are 2ⁿ such sets. You cannot index a DP table by an unordered collection, and re-deriving the set at each step means re-exploring permutations — n! work for something that only has 2ⁿ distinct states. |
| **Solution** | Encode the subset as an **integer mask** (bit *i* = "item *i* used") — now it is a plain array index. `dp[mask][j]` = best cost to have visited exactly the set `mask`, currently sitting at `j`. Transition by iterating candidate next items `k` **not yet in the mask** (`!(mask & (1 << k))`) and relaxing `dp[mask | (1 << k)][k]`. The tiny n is what makes 2ⁿ tractable; the mask is what collapses n! orderings into 2ⁿ sets. |
| **Time / Space Complexity** | Typically **O(2ⁿ · n²)** time and **O(2ⁿ · n)** space for TSP-shaped problems (2ⁿ masks × n endpoints × n transitions). For subset-feasibility problems like `canPartitionIntoKSubsets`, O(2ⁿ · n) time / O(2ⁿ) space. |
| **Pros** | Collapses n! permutations into 2ⁿ subsets — for n = 15 that is 10¹² down to 32768 · the mask is a cheap array index, so no hashing and no allocation · subset operations (add, test, remove, iterate) are single instructions · the state is self-describing, which makes debugging by printing masks in binary genuinely practical. |
| **Cons** | **Hard ceiling around n ≈ 20–22** — memory and time both double per extra element, so there is no gentle degradation, it simply stops fitting · the code is dense and unreadable without disciplined comments · iteration order matters: masks must be processed in increasing order so every source state is final before it is read · `1 << k` on a 32-bit `int` and unfilled sentinel values (`INT_MAX + cost` overflow) are constant hazards. |
| **Use When** | Travelling salesman (closed tour) / minimum Hamiltonian path on tiny graphs · assignment problems (n workers to n tasks) · partition into k equal-sum subsets · covering problems over a small universe · any "visit every item exactly once, order matters for cost" question with n ≤ 20. |
| **Avoid When** | n exceeds ~22 (use heuristics, branch-and-bound, or an approximation) · only the **count** of used items matters, not which ones (a plain 1D DP over counts suffices) · the items are interchangeable, making the set irrelevant · a greedy or flow formulation solves it exactly (assignment problems often reduce to Hungarian/min-cost-flow in polynomial time). |
| **Related Patterns** | Bit Manipulation (supplies every mask idiom used here) · Subsets/backtracking (the un-memoised enumeration this pattern accelerates) · 0/1 Knapsack (also "which items did I take," but where only the aggregate weight matters, so no mask is needed). |

### Template Skeleton

```cpp
// A. TSP-shaped: dp[mask][j] = min cost, visited exactly `mask`, now standing at j.
int minHamiltonianCost(const std::vector<std::vector<int>>& cost) {
    int n = cost.size();
    const int kInf = std::numeric_limits<int>::max() / 2;   // /2 so dp + cost cannot overflow
    std::vector<std::vector<int>> dp(1 << n, std::vector<int>(n, kInf));

    dp[1][0] = 0;   // mask 0b1 = "only node 0 visited", standing at node 0

    for (int mask = 0; mask < (1 << n); ++mask) {   // ASCENDING: a mask is only ever
        for (int j = 0; j < n; ++j) {               // extended, so sources are final
            if (dp[mask][j] == kInf) continue;      // unreachable state, skip
            if (!(mask & (1 << j)))  continue;      // j must actually be in the mask

            for (int k = 0; k < n; ++k) {
                if (mask & (1 << k)) continue;      // k already visited — skip
                int next = mask | (1 << k);         // add k to the set
                dp[next][k] = std::min(dp[next][k], dp[mask][j] + cost[j][k]);
            }
        }
    }
    int full = (1 << n) - 1;
    int best = kInf;
    for (int j = 0; j < n; ++j)                     // close the tour: + return leg j -> 0
        if (dp[full][j] < kInf) best = std::min(best, dp[full][j] + cost[j][0]);
    return best;
}

// B. Feasibility-shaped: dp[mask] = state after consuming exactly the items in `mask`.
//    (canPartitionIntoKSubsets: dp[mask] = sum accumulated in the CURRENT bucket.)
//    Same shape — iterate masks ascending, extend by one unused element at a time.

// Mask idioms used throughout
mask & (1 << i)     // is i in the set?
mask | (1 << i)     // add i
(1 << n) - 1        // the full set
__builtin_popcount(mask)   // how many are in the set
```

### Remember In One Sentence
> **Bitmask DP uses an integer as the subset key so "which items have I used" becomes an array index — collapsing n! orderings into 2ⁿ states — and it only works because n is small enough that 2ⁿ fits, with masks iterated in ascending order so a state is finalised before anything reads it.**

### Two Facts People Get Wrong
- The mask can be iterated in any order because it is just a DP table? **No** — transitions only ever **add** bits (`mask | (1 << k)` > `mask` numerically), so ascending mask order is exactly what guarantees `dp[mask][j]` is final when read. Iterating descending reads states that have not been relaxed yet and quietly returns a too-large answer.
- Use `INT_MAX` as the "unreachable" sentinel? **Dangerous** — the transition computes `dp[mask][j] + cost[j][k]`, which overflows to a negative number and then wins the `min`. Use `INT_MAX / 2` (as above) or guard with an explicit `if (dp[mask][j] == kInf) continue;` — the template does both.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. What is the single strongest signal in a problem statement that this is bitmask DP?
2. What does `dp[mask][j]` mean, in words — both components?
3. Explain how the mask collapses n! into 2ⁿ. What information is deliberately thrown away?
4. Why must masks be iterated in ascending numeric order? What is the concrete failure if they are not?
5. Write the three mask idioms for test / add / full-set from memory.
6. Why is `kInf` set to `INT_MAX / 2` instead of `INT_MAX`?
7. What are the two `continue` guards in the inner loop checking, and what would go wrong if each were removed?
8. Where does the final answer live for a closed tour (TSP), and why is it a loop over `j` plus `cost[j][0]` rather than a single cell? What changes if the problem wants an open Hamiltonian *path* instead?
9. State the time and space complexity for the TSP shape and break down where each factor comes from.
10. Why does the pattern stop working past n ≈ 22, and name one alternative you would reach for at n = 100.
