// ============================================================================
// LeetCode 63 — Unique Paths II
// ============================================================================
//
// PROBLEM
// -------
// You are given an m x n integer array `grid` where a cell with value 1 is an
// obstacle and value 0 is free. A robot starts at the top-left corner and
// wants to reach the bottom-right, moving only down or right. It cannot step
// on obstacles. How many unique paths are there? The answer is guaranteed to
// fit in an int for the given constraints.
//
// Example: [[0,0,0],
//           [0,1,0],
//           [0,0,0]]  ->  2   (the central obstacle leaves exactly two routes)
//
// APPROACH — Grid DP (counting recurrence + obstacle override)
// ------------------------------------------------------------
// This is Unique Paths (problems/01-unique-paths.cpp) with walls. The
// recurrence is unchanged — a free cell (r, c) is still reached only from
// above or from the left, so dp[r][c] = dp[r-1][c] + dp[r][c-1]. What changes
// is obstacle handling, and it must be EXPLICIT:
//
//     if grid[r][c] == 1:  dp[r][c] = 0   and SKIP the recurrence entirely.
//
// Why forced zero: dp[r][c] counts paths ENDING at (r, c). No path may pass
// through an obstacle, so the count of valid paths arriving there is exactly
// zero — and because downstream cells read this value as one of their two
// predecessors, a zero correctly contributes nothing to any route that would
// have to cross the wall. Running the normal recurrence on an obstacle cell
// instead would silently let paths flow straight through it.
//
// Base cases get sharper here than in the obstacle-free version:
//   - The START cell itself can be blocked (grid[0][0] == 1): then no path
//     exists at all and the answer is 0. The obstacle check must run BEFORE
//     seeding the start cell.
//   - Along the top row / left column, a single obstacle cuts off every cell
//     beyond it in that direction (their only predecessor chain is severed) —
//     which falls out automatically once those cells' dp values are computed
//     as "sum of available predecessors" rather than hard-coded to 1.
//
// Space compression: same rolling-row argument as before; an obstacle simply
// writes 0 into the rolling array at its column.
//
// COMPLEXITY
// ----------
// Time:  O(m * n) — one branch plus at most one addition per cell.
// Space: O(n) — single rolling row.
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// Returns the number of right/down paths avoiding all obstacle cells
// (grid[r][c] == 1). Returns 0 if start or destination is blocked.
int uniquePathsWithObstacles(const std::vector<std::vector<int>>& grid) {
  int rows = static_cast<int>(grid.size());
  int cols = static_cast<int>(grid[0].size());

  // Rolling row of path counts; everything starts unreachable (0).
  std::vector<long long> row(static_cast<std::size_t>(cols), 0);

  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (grid[r][c] == 1) {
        // Obstacle: force zero BEFORE any other logic — including the start
        // cell's seed below on iteration (0,0). A blocked start means the
        // whole table stays zero, which is exactly the correct answer.
        row[static_cast<std::size_t>(c)] = 0;
        continue;
      }

      if (r == 0 && c == 0) {
        row[0] = 1;  // start cell: one way to be "already there"
        continue;
      }

      // Free non-start cell: sum the available predecessors.
      // row[c]   not yet overwritten this pass -> count from the row above
      //          (0 on row 0, i.e. no such predecessor — adds nothing).
      // row[c-1] already overwritten this pass -> count from the left
      //          (skipped entirely at c == 0: column 0 has no left neighbor,
      //          so its only predecessor is the cell above).
      if (c > 0) {
        row[static_cast<std::size_t>(c)] += row[static_cast<std::size_t>(c - 1)];
      }
    }
    // Note we do NOT re-pin column 0 to 1 each row: an obstacle anywhere in
    // column 0 must zero out this and every later row's column 0 too.
  }

  return static_cast<int>(row[static_cast<std::size_t>(cols - 1)]);
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](bool condition, const std::string& label) {
    if (condition) {
      std::cout << "[PASS] " << label << "\n";
      ++pass_count;
    } else {
      std::cout << "[FAIL] " << label << "\n";
      ++fail_count;
    }
  };

  {
    // Classic example: central obstacle kills the two "through the middle"
    // walks of the 3x3 grid, leaving the two that hug opposite edges.
    std::vector<std::vector<int>> grid = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
    check(uniquePathsWithObstacles(grid) == 2,
          "3x3 grid, central obstacle -> 2");
  }

  {
    // Degenerate single open cell: start == destination, unobstructed.
    std::vector<std::vector<int>> grid = {{0}};
    check(uniquePathsWithObstacles(grid) == 1, "1x1 open grid -> 1");
  }

  {
    // Degenerate single BLOCKED cell: the start is an obstacle -> no paths.
    std::vector<std::vector<int>> grid = {{1}};
    check(uniquePathsWithObstacles(grid) == 0,
          "1x1 blocked grid -> 0 (blocked start)");
  }

  {
    // Blocked DESTINATION: paths exist up to the last step but none end
    // legally — answer must be 0, not a crash or a stale count.
    std::vector<std::vector<int>> grid = {{0, 0}, {0, 1}};
    check(uniquePathsWithObstacles(grid) == 0,
          "blocked bottom-right -> 0");
  }

  {
    // Obstacle on the top row severs the entire remainder of that row:
    // every route is forced through the left column first.
    std::vector<std::vector<int>> grid = {{0, 1, 0}, {0, 0, 0}};
    // Only path family: D,R,R (down first, since (0,1) is a wall) -> 1 way.
    check(uniquePathsWithObstacles(grid) == 1,
          "top-row obstacle forces the single lower route -> 1");
  }

  {
    // Obstacle in the left column: verifies we did NOT hard-code column 0
    // to 1 on every row (a classic bug in rolling-row implementations).
    std::vector<std::vector<int>> grid = {{0, 0}, {1, 0}};
    // Only route: R,D (right along the top, then down) -> 1 way.
    check(uniquePathsWithObstacles(grid) == 1,
          "left-column obstacle handled by rolling row -> 1");
  }

  {
    // Fully walled corridor: both routes into the bottom-right are cut.
    std::vector<std::vector<int>> grid = {{0, 1}, {1, 0}};
    check(uniquePathsWithObstacles(grid) == 0,
          "both approaches walled off -> 0");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
