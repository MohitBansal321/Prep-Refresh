# DP on Grids — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Dynamic Programming pattern — 2D table filled in place, one cell at a time. |
| **Recognition Signal** | Input is a **2D grid** and you move through it with **monotonic direction restriction** (typically only right/down), asked to **count distinct paths** or find the **min/max-cost path** between two corners. Any wording like "how many ways to reach the bottom-right" or "cheapest route from top-left." |
| **Problem** | Enumerating every path brute-force costs exponential time (each cell doubles the branch count); recomputing the best suffix from each cell recursively without caching re-solves the same subgrids repeatedly. |
| **Solution** | Define `dp[row][col]` = best answer *reachable at* that cell. Because movement is right/down only, `(row, col)` is reachable only from `(row-1, col)` or `(row, col-1)`, so `dp[row][col]` combines those two already-computed neighbors: **count** `dp[r][c] = dp[r-1][c] + dp[r][c-1]`; **cost** `dp[r][c] = min(dp[r-1][c], dp[r][c-1]) + cost[r][c]`. Fill row by row, left to right; top row and left column are the base case. Obstacles force `0` (count) or `INF` (cost) and skip the recurrence. |
| **Time / Space Complexity** | O(rows * cols) time — one constant-time decision per cell. O(rows * cols) space for the full table, reducible to O(cols) with a rolling 1D array since each row depends only on the row above. |
| **Pros** | Exponential-to-linear collapse · trivially correct fill order (dependencies always computed first) · recurrence is two lines · rolls to O(cols) space mechanically · extends naturally to extra per-cell state (add a dimension per piece of state). |
| **Cons** | Only works when movement is acyclic (right/down-style) — free 4-direction movement breaks it · full table costs O(rows*cols) memory if you don't roll · easy to botch the edge-row/column base case · counting versions can overflow `int` (use `long long`) · backward-fill variants (Dungeon Game) invert the intuition and need care. |
| **Use When** | Counting monotone paths through a grid · min/max cost path with right/down moves · any grid problem whose state at a cell depends only on already-filled up/left neighbors · layered decision problems that can be reshaped into a grid walk. |
| **Avoid When** | Movement allows all 4 directions or revisiting cells (use BFS for unweighted shortest path, Dijkstra for weighted) · you need the shortest path in an *unweighted* maze regardless of direction (BFS is simpler and faster) · a locally-best choice is provably globally optimal (greedy suffices) · the state is two sequences, not a grid (that's LCS — structurally similar, different shape). |
| **Related Patterns** | Longest Common Subsequence ([../longest-common-subsequence/](../longest-common-subsequence/)) — same neighbor-based 2D table, indexed by sequence prefixes instead of grid cells · Graph BFS/DFS ([../../tree-graph-patterns/graph-bfs-dfs/](../../tree-graph-patterns/graph-bfs-dfs/)) — the correct tool once movement is unrestricted · 0/1 Knapsack ([../0-1-knapsack/](../0-1-knapsack/)) — the same rolling-row trick applied to item-by-capacity tables. |

### Template Skeleton

```cpp
// Counting variant (e.g. Unique Paths)
std::vector<std::vector<long long>> dp(rows, std::vector<long long>(cols, 0));
for (int r = 0; r < rows; ++r) {
  for (int c = 0; c < cols; ++c) {
    if (r == 0 || c == 0) { dp[r][c] = 1; continue; }  // one straight-line way
    dp[r][c] = dp[r - 1][c] + dp[r][c - 1];
  }
}
// answer: dp[rows - 1][cols - 1]

// Cost variant (e.g. Minimum Path Sum)
for (int r = 0; r < rows; ++r) {
  for (int c = 0; c < cols; ++c) {
    if (r == 0 && c == 0) { dp[r][c] = grid[0][0]; continue; }
    int fromTop  = (r > 0) ? dp[r - 1][c] : INT_MAX;
    int fromLeft = (c > 0) ? dp[r][c - 1] : INT_MAX;
    dp[r][c] = std::min(fromTop, fromLeft) + grid[r][c];
  }
}

// Obstacle override (runs BEFORE the recurrences above):
if (obstacle[r][c]) { dp[r][c] = 0 /* count */ ; continue; }  // INF for cost
```

### Remember In One Sentence
> **DP on Grids replaces exponential path enumeration with a single row-by-row sweep of a table shaped like the grid, because right/down-only movement guarantees every cell's answer is a fixed combination of the answers directly above and directly to its left.**

### Two Facts People Get Wrong
- The top row and left column have **zero** ways to be reached? **No** — they have exactly **one** way each (a straight line from the start); initializing them to 0 instead of 1 makes every subsequent count wrong by construction.
- This pattern works on any grid pathfinding problem? **No** — it silently produces wrong answers the moment movement is allowed upward or leftward (or cells can be revisited), because then a cell's answer can depend on cells *not yet computed*; that dependency cycle means you need BFS/Dijkstra, not this table.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the recognition signal for DP on Grids in one sentence — what two properties must the movement and the question have?
2. Write both core recurrences from memory: the counting one and the min/max-cost one.
3. Why does the row-by-row, left-to-right fill order guarantee correctness? What property of the movement restriction makes this possible?
4. What are the base-case values for the top row and left column in (a) a counting problem, (b) a cost problem?
5. How do you handle an obstacle cell in a counting problem vs. a cost problem, and why does simply running the normal recurrence there fail?
6. How do you compress the O(rows * cols) table to O(cols)? Which previously-computed value does the in-place update overwrite, and why is that safe?
7. A problem lets you move right, down, AND diagonally down-right. What changes in the recurrence, and does the pattern still apply?
8. Your grid is 100x100 and counts paths. Why is `int` dangerous here, and what do you use instead?
9. Give the decisive test for choosing between this pattern and BFS/Dijkstra on a grid problem.
10. Dungeon Game (LeetCode 174) fills the table backward from the bottom-right. Why does the forward fill break for that problem when it works for Minimum Path Sum?

(End of file - total 61 lines)
