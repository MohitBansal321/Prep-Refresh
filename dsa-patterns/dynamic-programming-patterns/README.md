# Dynamic Programming Patterns

Every DP pattern is the same three questions applied to a different "shape" of subproblem: **what does state `[i]` (or `[i][j]`) mean, what does it depend on, and in what order do I fill it in?** These six patterns cover the shapes that recur constantly across interview problems.

| Pattern | State shape | Used for |
|---------|-------------|----------|
| [0/1 Knapsack](0-1-knapsack/README.md) | `dp[i][capacity]` — first i items, capacity left, each item used once | Subset-sum, partition-equal-subset, target-sum |
| [Unbounded Knapsack](unbounded-knapsack/README.md) | Same as 0/1 but items reusable | Coin change, rod cutting, minimum coins |
| [Longest Common Subsequence](longest-common-subsequence/README.md) | `dp[i][j]` — best answer for prefixes of *two* sequences | Edit distance, LCS, shortest common supersequence |
| [Longest Increasing Subsequence](longest-increasing-subsequence/README.md) | `dp[i]` — best answer for subsequences ending at i | LIS, Russian doll envelopes, longest chain |
| [Palindromic Subsequence/Substring](palindromic-subsequence/README.md) | `dp[i][j]` — is/what is the answer for the interval `[i, j]` | Longest palindromic substring/subsequence, palindrome partitioning |
| [DP on Grids](dp-on-grids/README.md) | `dp[row][col]` — best answer reachable at this cell | Unique paths, min path sum, edit-distance-as-a-grid |
| [Bitmask DP](bitmask-dp/README.md) | `dp[mask]` (or `dp[mask][last]`) — best answer for exactly this *subset* of a small item set | TSP/Hamiltonian path, equal-sum partitioning, assignment problems |

## How to tell them apart

- **Items with weight/value + a capacity, each usable once or unlimited times?** → 0/1 or Unbounded Knapsack (the "once vs. unlimited" distinction is the whole difference between them).
- **Comparing two sequences?** → LCS-family (2D table over two indices).
- **One sequence, want the longest subsequence obeying an order relation?** → LIS.
- **One string, question is about a palindrome inside it?** → Palindromic Subsequence/Substring (2D table over one interval).
- **Explicit 2D grid, moving right/down?** → DP on Grids.
- **`n` is small (≲20) and the state depends on *which specific* items/cities have been used, not just how many?** → Bitmask DP — the identity of the used subset is encoded as the bits of an integer.

## Recommended study order

1. **0/1 Knapsack** — the canonical DP template (include/exclude with a capacity axis); most other patterns are variations on this shape.
2. **Unbounded Knapsack** — one small but important change (reuse) to the same template.
3. **DP on Grids** — the most visual/intuitive 2D DP, good bridge before two-sequence DP.
4. **Longest Common Subsequence** — the two-sequence 2D template (edit distance, LCS).
5. **Longest Increasing Subsequence** — a 1D DP with a subtler recurrence, plus the O(n log n) optimization.
6. **Palindromic Subsequence/Substring** — interval DP, usually tackled last since it requires filling the table by increasing interval length rather than row-by-row.
7. **Bitmask DP** — save for last; it depends on being comfortable with Subsets' enumeration (Recursion & Backtracking family) plus the 0/1 Knapsack include/exclude instinct, applied to a subset-identity-keyed state.

All seven are Full-tier modules. See [`../INDEX.md`](../INDEX.md) for details.
