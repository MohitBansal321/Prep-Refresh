// ============================================================================
// LeetCode 62 — Unique Paths
// ============================================================================
//
// PROBLEM
// -------
// A robot is located at the top-left corner of an m x n grid. The robot can
// move only either down or right at any point in time. The robot is trying to
// reach the bottom-right corner of the grid. How many possible unique paths
// are there?
//
// Example: m = 3, n = 7  ->  28
//
// APPROACH — Grid DP (counting recurrence)
// ----------------------------------------
// Brute force enumerates every path: from each cell the walk branches into
// (down) and (right), so a grid with m+n-2 steps has up to 2^(m+n-2) walks —
// exponential. But those walks share enormous substructure: the number of
// ways to reach ANY given cell is independent of HOW you got there. That is
// the DP insight.
//
// Define dp[r][c] = number of distinct right/down paths from the top-left
// corner to cell (r, c). Since movement is restricted to right/down, the ONLY
// ways to arrive at (r, c) are:
//   - from the cell above (r-1, c), having just moved down, or
//   - from the cell to the left (r, c-1), having just moved right.
// These two route families are disjoint (their final step differs) and cover
// every possibility, so the counts simply add:
//
//     dp[r][c] = dp[r-1][c] + dp[r][c-1]
//
// Base cases: every cell in the top row or left column can be reached in
// exactly ONE way — a straight line from the start with no choices available.
// Getting this wrong (initializing to 0) corrupts every downstream count.
//
// Space compression: row r depends only on row r-1 and on the already-written
// prefix of row r itself. So one rolling 1D array of size n suffices: before
// overwriting, dp[c] still holds the previous row's value (= "from above"),
// and dp[c-1] was just written this row (= "from the left").
//
// COMPLEXITY
// ----------
// Time:  O(m * n) — one constant-time addition per cell.
// Space: O(n) — single rolling row; the full 2D table would be O(m * n).
//
// Overflow note: path counts grow combinatorially (the answer for large grids
// exceeds 32-bit range quickly), so the rolling row is kept in long long.
// LeetCode guarantees the answer fits in int for its constraints, but the
// intermediate arithmetic is safest in long long.
// ============================================================================

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// Returns the number of unique right/down paths from (0,0) to (m-1,n-1).
int uniquePaths(int m, int n) {
  // Rolling row: dp[c] ends each pass holding "paths to (currentRow, c)".
  std::vector<long long> dp(static_cast<std::size_t>(n), 1);

  // Row 0 is pre-initialized to all 1s (the straight-line base case).
  for (int r = 1; r < m; ++r) {
    for (int c = 1; c < n; ++c) {
      // dp[c]   not yet overwritten this pass -> value from the row above.
      // dp[c-1] already overwritten this pass -> value from the left.
      dp[static_cast<std::size_t>(c)] += dp[static_cast<std::size_t>(c - 1)];
    }
    // Column 0 stays 1 forever: exactly one way to reach any (r, 0).
  }

  return static_cast<int>(dp[static_cast<std::size_t>(n - 1)]);
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
    // Classic example: C(7, 2)-ish count across a 3x7 grid.
    check(uniquePaths(3, 7) == 28, "3x7 grid -> 28");
  }

  {
    // Degenerate: already standing on the destination -> exactly one path
    // (the empty path). A common off-by-one trap is returning 0 here.
    check(uniquePaths(1, 1) == 1, "1x1 grid -> 1 (start == destination)");
  }

  {
    // Single row: only one way — keep moving right.
    check(uniquePaths(1, 10) == 1, "1x10 grid -> 1 (single row)");
  }

  {
    // Single column: symmetric edge case — only one way, straight down.
    check(uniquePaths(10, 1) == 1, "10x1 grid -> 1 (single column)");
  }

  {
    // Smallest non-trivial grid: two moves, both orderings valid.
    check(uniquePaths(3, 2) == 3, "3x2 grid -> 3");
  }

  {
    // Larger square grid, verified against the closed form C(18, 9) = 48620.
    // Guards against subtle recurrence errors that small grids can mask.
    check(uniquePaths(10, 10) == 48620, "10x10 grid -> 48620 (= C(18,9))");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
