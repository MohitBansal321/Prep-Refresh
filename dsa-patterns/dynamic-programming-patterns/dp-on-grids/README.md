# DP on Grids

> **5-min refresher instead?** [cheatsheet.md](cheatsheet.md) has the one-table summary and recall questions.

## Intent

Fill a 2D table matching the shape of an input grid, where each cell's answer depends only on the cell above and/or to its left — turning path-counting and min/max-cost-path problems into a single pass instead of exponential path enumeration.

## Recognition Signal

The problem describes moving through a 2D grid (typically only right/down) and asks for the number of distinct paths, or the minimum/maximum cost path, from one corner to another.

## Core Idea

Define `dp[row][col]` = the best answer *reachable at* that cell. Because movement is restricted to right/down, the only way to arrive at `(row, col)` is from `(row-1, col)` or `(row, col-1)` — so `dp[row][col]` is built purely from those two already-computed neighbors. The top row and left column are the base case (only one way to reach them: a straight line from the start), and the fill order is row by row, left to right, guaranteeing every cell's dependencies exist before it's computed.

- **Counting paths:** `dp[row][col] = dp[row-1][col] + dp[row][col-1]`.
- **Min/max cost path:** `dp[row][col] = min(dp[row-1][col], dp[row][col-1]) + cost[row][col]`.
- **Obstacles:** any obstacle cell is forced to 0 (path count) or infinity (cost), and simply skips the normal recurrence.

This only works because movement is one-directional (right/down) — there are no cycles in the dependency graph between cells, which is exactly what makes the table "fillable" in a single pass.

## Template

```cpp
int uniquePaths(int rows, int cols) {
  std::vector<std::vector<int>> dp(rows, std::vector<int>(cols, 0));
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (r == 0 || c == 0) { dp[r][c] = 1; continue; }  // one way along an edge
      dp[r][c] = dp[r - 1][c] + dp[r][c - 1];
    }
  }
  return dp[rows - 1][cols - 1];
}

int minPathSum(std::vector<std::vector<int>>& grid) {
  int rows = grid.size(), cols = grid[0].size();
  std::vector<std::vector<int>> dp(rows, std::vector<int>(cols));
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (r == 0 && c == 0) { dp[r][c] = grid[r][c]; continue; }
      int fromTop = (r > 0) ? dp[r - 1][c] : INT_MAX;
      int fromLeft = (c > 0) ? dp[r][c - 1] : INT_MAX;
      dp[r][c] = std::min(fromTop, fromLeft) + grid[r][c];
    }
  }
  return dp[rows - 1][cols - 1];
}
```

## Complexity

**Time:** `O(rows * cols)` — one constant-time decision per cell, versus exponential brute-force path enumeration.
**Space:** `O(rows * cols)` for the full table, or `O(cols)` with a rolling 1D array since each row only depends on the row above.

## Common Mistakes

- **Forgetting obstacle cells** need to be explicitly zeroed (path count) or set to infinity (cost) and excluded from the normal recurrence, not silently included.
- **Off-by-one on the single top-row/left-column base case** — these have exactly one way to be reached (a straight line), not zero.
- **Applying this pattern to a grid that allows all 4 directions or revisiting cells** — that breaks the "no cycles in the dependency graph" assumption this pattern relies on; use BFS/Dijkstra instead.

## When To Use

- Counting distinct paths, or finding min/max cost paths, through a grid with movement restricted to right/down (or a similarly monotonic direction).

## When NOT To Use

- **Movement allows all 4 directions or revisiting cells** — needs BFS/Dijkstra (a distance table with a priority queue), not simple DP, since cycles break the "depends only on earlier cells" assumption.
- **The state isn't naturally a 2D grid** — if you're comparing two sequences, that's Longest Common Subsequence, a structurally similar table with a different underlying shape.

## Similar Patterns

- **Longest Common Subsequence** ([../longest-common-subsequence/](../longest-common-subsequence/)): a strikingly similar 2D table with a "look at the neighbor(s)" recurrence, but indexed by two independent sequence prefixes rather than a literal grid.
- **Graph BFS/DFS** ([../../tree-graph-patterns/graph-bfs-dfs/](../../tree-graph-patterns/graph-bfs-dfs/)): the right tool once movement isn't restricted to right/down — a grid with free movement in all 4 directions is really just a graph.

## Further Reading

- LeetCode — Unique Paths (62), Minimum Path Sum (64), Unique Paths II (63, obstacles), Dungeon Game (174, hard — fills backward from the bottom-right).
- *Introduction to Algorithms* (CLRS) — general dynamic programming foundations applicable to this table shape.
