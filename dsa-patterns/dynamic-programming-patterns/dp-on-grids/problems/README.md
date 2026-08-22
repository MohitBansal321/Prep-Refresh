# DP on Grids — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating DP on Grids across its flavors (path counting, min-cost path, obstacle handling, and a three-neighbor variant). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-unique-paths.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Unique Paths | [62](https://leetcode.com/problems/unique-paths/) | Medium | Counting recurrence `dp[r][c] = dp[r-1][c] + dp[r][c-1]` with 1s along the top row and left column. | O(m*n) time, O(n) space (rolling row) | [01-unique-paths.cpp](01-unique-paths.cpp) |
| Minimum Path Sum | [64](https://leetcode.com/problems/minimum-path-sum/) | Medium | Cost recurrence `dp[r][c] = min(up, left) + grid[r][c]`, seeded at the start cell. | O(m*n) time, O(n) space (rolling row) | [02-minimum-path-sum.cpp](02-minimum-path-sum.cpp) |
| Unique Paths II | [63](https://leetcode.com/problems/unique-paths-ii/) | Medium | Counting recurrence with an obstacle override: blocked cells are forced to 0 and skip the recurrence entirely. | O(m*n) time, O(n) space (rolling row) | [03-unique-paths-ii.cpp](03-unique-paths-ii.cpp) |
| Minimum Falling Path Sum | [931](https://leetcode.com/problems/minimum-falling-path-sum/) | Medium | Row-by-row cost fill where each cell reads up to **three** neighbors from the row above (up-left, up, up-right). | O(m*n) time, O(n) space (rolling row) | [04-minimum-falling-path-sum.cpp](04-minimum-falling-path-sum.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the textbook counting recurrence in its purest form — the pattern's canonical example, including the rolling-row space compression.
- **02** is the textbook min-cost recurrence — the other main flavor, showing how edge base cases accumulate running sums instead of storing counts.
- **03** shows how obstacles interact with the pattern: forced special values that cut routes, plus the degenerate cases (blocked start/end) that trip up naive implementations.
- **04** breaks the "exactly two neighbors" habit deliberately: same loops, same fill order, but three predecessors per cell — proof that the real rule is "read only already-filled neighbors," not literally "above and left."

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
