// ============================================================================
// LeetCode 64 — Minimum Path Sum
// ============================================================================
//
// PROBLEM
// -------
// Given an m x n grid filled with non-negative numbers, find a path from the
// top-left to the bottom-right that minimizes the sum of all numbers along
// its path. You can only move either down or right at any point in time.
//
// Example: [[1,3,1],
//           [1,5,1],
//           [4,2,1]]  ->  7   (path 1 -> 3 -> 1 -> 1 -> 1)
//
// APPROACH — Grid DP (cost recurrence)
// ------------------------------------
// Brute force walks every right/down path and sums it: exponentially many
// paths. The fix is the same substructure argument as in Unique Paths (see
// problems/01-unique-paths.cpp), but instead of COUNTING arrivals we take
// the BEST arrival.
//
// Define dp[r][c] = minimum sum of a path from (0,0) to (r, c). Any path
// arriving at (r, c) takes its final step either down from (r-1, c) or right
// from (r, c-1) — and whatever happened before that final step must itself
// have been optimal (otherwise you could splice in a cheaper prefix). Hence:
//
//     dp[r][c] = min(dp[r-1][c], dp[r][c-1]) + grid[r][c]
//
// The "+ grid[r][c]" is what distinguishes cost problems from counting ones:
// every arrival pays the current cell's toll exactly once.
//
// Base cases: there is no choice along the top row or left column — only one
// direction of travel exists — so their dp values are just the running sum
// of the costs seen so far. The start cell seeds everything: dp[0][0] =
// grid[0][0]. Out-of-grid neighbors are neutralized with INT_MAX so the min
// never selects them.
//
// Space compression: identical argument as Unique Paths — row r reads only
// row r-1 and its own already-written prefix — so one rolling array of size
// n suffices. Here we overwrite IN PLACE: before writing, row[c] holds the
// previous row's value ("from above"); row[c-1] was just updated ("from the
// left").
//
// COMPLEXITY
// ----------
// Time:  O(m * n) — one comparison plus one addition per cell.
// Space: O(n) — single rolling row; the full table would be O(m * n).
//
// Why greedy fails: stepping onto the locally cheapest neighbor can steer
// you into an expensive corridor (in the example above, heading toward the
// cheap 2 in the bottom row misses the 1-3-1 top corridor). DP's per-cell
// "best way HERE" table is what makes the global optimum trustworthy.
// ============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string>
#include <vector>

// Returns the minimum path sum from top-left to bottom-right of `grid`.
int minPathSum(const std::vector<std::vector<int>>& grid) {
  int rows = static_cast<int>(grid.size());
  int cols = static_cast<int>(grid[0].size());

  // Rolling row; seeded with row 0's running sums below.
  std::vector<int> row(static_cast<std::size_t>(cols), 0);

  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (r == 0 && c == 0) {
        // Start cell: pay its own toll, no predecessors exist.
        row[0] = grid[0][0];
        continue;
      }

      int fromAbove = (r > 0) ? row[static_cast<std::size_t>(c)] : INT_MAX;
      int fromLeft = (c > 0) ? row[static_cast<std::size_t>(c - 1)] : INT_MAX;

      // Overflow safety: at most ONE of the two candidates is INT_MAX (the
      // other always exists for any non-start cell), so std::min() never
      // returns INT_MAX here — the addition below cannot overflow.
      row[static_cast<std::size_t>(c)] =
          std::min(fromAbove, fromLeft) + grid[r][c];
    }
    // After this pass, `row` holds the complete dp values for row r.
  }

  return row[static_cast<std::size_t>(cols - 1)];
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
    // Classic example: the optimal route runs along the top row then down
    // the right column, NOT greedily toward the cheap bottom-row cells.
    std::vector<std::vector<int>> grid = {{1, 3, 1}, {1, 5, 1}, {4, 2, 1}};
    check(minPathSum(grid) == 7, "classic 3x3 grid -> 7");
  }

  {
    // Degenerate: single cell — you start on the destination, so you pay
    // exactly its toll and nothing more.
    std::vector<std::vector<int>> grid = {{5}};
    check(minPathSum(grid) == 5, "1x1 grid -> 5");
  }

  {
    // Single row: no choices, sum of everything.
    std::vector<std::vector<int>> grid = {{1, 2, 3, 4}};
    check(minPathSum(grid) == 10, "1x4 grid -> 10 (forced straight line)");
  }

  {
    // Single column: symmetric edge case.
    std::vector<std::vector<int>> grid = {{1}, {2}, {3}};
    check(minPathSum(grid) == 6, "3x1 grid -> 6 (forced straight line)");
  }

  {
    // A tempting-but-wrong greedy trap: the wide-open cheap first row (100
    // aside) lures a "cheapest visible cell" walk, but the true optimum
    // dives down the left edge immediately.
    // Optimal path: cells (0,0),(1,0),(2,0),(2,1),(3,1) = 1+1+0+0+1 = 3,
    // via moves Down, Down, Right, Right. Any route through (0,1)=100 or
    // (3,0)=9 is strictly worse — pinning the exact optimum here catches an
    // implementation that mishandles the left-column running sum.
    std::vector<std::vector<int>> grid = {{1, 100}, {1, 0}, {0, 0}, {9, 1}};
    check(minPathSum(grid) == 3, "detour down the left edge beats 100 -> 3");
  }

  {
    // Rectangular, wider than tall: verifies both loop bounds independently.
    // Grid:  1  2  5
    //        3  1  1
    // Paths: R,R,D -> (0,0),(0,1),(0,2),(1,2) = 1+2+5+1 = 9
    //        R,D,R -> (0,0),(0,1),(1,1),(1,2) = 1+2+1+1 = 5   <- minimum
    //        D,R,R -> (0,0),(1,0),(1,1),(1,2) = 1+3+1+1 = 6
    std::vector<std::vector<int>> grid = {{1, 2, 5}, {3, 1, 1}};
    check(minPathSum(grid) == 5, "2x3 grid, mid-path turn wins -> 5");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
