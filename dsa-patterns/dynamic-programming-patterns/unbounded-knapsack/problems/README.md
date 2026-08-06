# Unbounded Knapsack — Worked Problems

Four fully worked, standalone, compilable C++17 solutions. Each file has a header comment explaining the problem, why it maps to Unbounded Knapsack, and its complexity, followed by the solution and a `main()` that prints `[PASS]`/`[FAIL]` against known answers.

Compile and run any of them with:

```bash
g++ -std=c++17 -Wall path/to/file.cpp -o /tmp/out && /tmp/out
```

| # | Name | LeetCode # | Difficulty | One-line approach | Complexity | File |
|---|------|-----------|------------|--------------------|------------|------|
| 01 | Coin Change | 322 | Medium | `dp[w]` = fewest coins summing to exactly `w`; forward-fill capacity, min + 1 over reusable coins. | O(amount * coins) time, O(amount) space | [01-coin-change.cpp](01-coin-change.cpp) |
| 02 | Coin Change II | 518 | Medium | `dp[w]` = number of combinations summing to exactly `w`; coins **outer** loop, amount **inner** loop — the loop-order flip that avoids counting permutations. | O(amount * coins) time, O(amount) space | [02-coin-change-ii.cpp](02-coin-change-ii.cpp) |
| 03 | Perfect Squares | 279 | Medium | Same shape as Coin Change, with the "coin list" replaced by the perfect squares `<= n`. | O(n * sqrt(n)) time, O(n) space | [03-perfect-squares.cpp](03-perfect-squares.cpp) |
| 04 | Integer Break | 343 | Medium | `dp[i]` = best product breaking `i`; each split part `j` may be reused again inside `dp[i-j]`, the "reuse" idea applied to integer parts instead of coins/weights. | O(n^2) time, O(n) space | [04-integer-break.cpp](04-integer-break.cpp) |

## Why these four

- **01 (Coin Change)** is the purest, most direct instance of the pattern: minimize count, exact-sum capacity, unlimited reuse. It is the reference point every other file in this folder gets compared against.
- **02 (Coin Change II)** looks like it should be a copy-paste of `01` with `min` swapped for `+=`, but it is not — the loop nesting order itself carries meaning (combinations vs. permutations), and this is the single most common way engineers get an unbounded-knapsack-shaped counting problem subtly wrong. It exists specifically to make that failure mode concrete and testable, not just described in prose.
- **03 (Perfect Squares)** demonstrates that the "item list" does not need to be given explicitly in the input — it can be *derived* (every perfect square up to `n`) and the exact same recurrence still applies unchanged.
- **04 (Integer Break)** stretches the pattern furthest: there is no explicit "item" at all, only a split point, and "reuse" appears through the recurrence reading `dp[i - j]` — which may have already chosen `j` again — rather than through an explicit "unlimited supply" list. It is included to prove the pattern is about the **recurrence's reuse structure**, not about the problem statement literally saying "unlimited coins."

## Read them in this order

1. [01-coin-change.cpp](01-coin-change.cpp) — canonical exact-sum minimize.
2. [02-coin-change-ii.cpp](02-coin-change-ii.cpp) — same inputs, counting instead of minimizing, loop-order trap.
3. [03-perfect-squares.cpp](03-perfect-squares.cpp) — derived item list, otherwise identical to `01`.
4. [04-integer-break.cpp](04-integer-break.cpp) — reuse without an explicit item list at all.
