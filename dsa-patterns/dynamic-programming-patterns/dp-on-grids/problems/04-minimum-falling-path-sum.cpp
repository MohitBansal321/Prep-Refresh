// ============================================================================
// LeetCode 931 — Minimum Falling Path Sum
// ============================================================================
//
// PROBLEM
// -------
// Given an n x n array of integers `matrix`, return the minimum sum of any
// FALLING path through the matrix. A falling path starts at any element in
// the first row and chooses one element from each subsequent row; the chosen
// element must be directly below, diagonally below-left, or diagonally
// below-right of the previous row's choice.
//
// Example: [[2,1,3],
//           [6,5,4],
//           [7,8,9]]  ->  13   (paths 1 -> 4 -> 8 and 1 -> 5 -> 7)
//
// APPROACH — Grid DP (cost recurrence, THREE predecessors per cell)
// ------------------------------------------------------------------
// Same recognition signal as Minimum Path Sum (problems/02) — monotone,
// top-to-bottom movement, minimize accumulated cost — so the same table
// applies. The only structural difference is the fan-in: a cell (r, c) can
// now be reached from up to THREE cells in the previous row:
//
//     dp[r][c] = min(dp[r-1][c-1], dp[r-1][c], dp[r-1][c+1]) + matrix[r][c]
//
// with out-of-grid neighbors simply omitted (column 0 has no up-left
// predecessor, column n-1 no up-right one). The start row is its own base
// case: dp[0][c] = matrix[0][c], since the path may begin at ANY column —
// unlike corner-to-corner problems there is no single seed cell.
//
// Why this works: movement still strictly increases the row index, so the
// dependency graph remains acyclic and row-by-row fill order still computes
// every predecessor before its consumer. The final answer is the MIN over
// the entire last row, because a falling path may exit at any column.
//
// This problem is included deliberately to break the "grid DP = read exactly
// two neighbors (up, left)" habit. The real rule is: read only cells that
// are guaranteed already-filled — here that is "the three above," elsewhere
// it might be "above, left" or even "above, left, up-left diagonal" (see
// exercises.md, Maximal Square).
//
// COMPLEXITY
// ----------
// Time:  O(n^2) for an n x n matrix — up to three comparisons per cell.
// Space: O(n) — two rolling rows' worth is not even needed: since dp values
//        for row r overwrite only row r-1's slots, ONE rolling array updated
//        left-to-right suffices (cell c reads old values at c-1/c/c+1, and
//        slot c-1 was already overwritten — but slot c-1 belongs to THIS row
//        and is never read by column c... careful: column c DOES read slot
//        c-1! So we keep a single `prevLeft` scalar carrying the pre-overwrite
//        value of slot c-1).
//
// Overflow note: entries can be negative (|matrix[i][j]| <= 100 per LeetCode),
// so sums shrink as well as grow; int is safe within the given constraints.
// ============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string>
#include <vector>

// Returns the minimum sum of any falling path from the top row to the bottom
// row of `matrix`.
int minFallingPathSum(const std::vector<std::vector<int>>& matrix) {
  int n = static_cast<int>(matrix.size());
  if (n == 1) {
    // Single-row matrix: the path is just the single cheapest entry.
    return *std::min_element(matrix[0].begin(), matrix[0].end());
  }

  // Rolling array holding dp values of the previous row; seeded with row 0.
  std::vector<int> row(matrix[0].begin(), matrix[0].end());

  for (int r = 1; r < n; ++r) {
    // prevLeft carries row[r-1]'s value at column c-1 across the overwrite.
    // For c == 0 there is no up-left neighbor, so use INT_MAX as a neutral
    // "never pick this" sentinel (std::min discards it naturally).
    int prevLeft = INT_MAX;

    for (int c = 0; c < n; ++c) {
      int fromUpLeft = prevLeft;
      int fromUp = row[static_cast<std::size_t>(c)];
      int fromUpRight =
          (c + 1 < n) ? row[static_cast<std::size_t>(c + 1)] : INT_MAX;

      // Save the current slot BEFORE overwriting: it becomes prevLeft for
      // the next column (where it plays the role of "up-left" neighbor).
      int savedPrevRowValue = row[static_cast<std::size_t>(c)];

      row[static_cast<std::size_t>(c)] =
          std::min(fromUpLeft, std::min(fromUp, fromUpRight)) + matrix[r][c];

      prevLeft = savedPrevRowValue;
    }
    // After this pass, `row` holds complete dp values for matrix row r.
  }

  // A falling path may end at ANY column of the last row.
  return *std::min_element(row.begin(), row.end());
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
    // Official LeetCode Example 1, traced by hand:
    //   row0: 2 1 3            dp0: 2 1 3
    //   row1: 6 5 4            dp1: min(2,1)+6=7 | min(2,1,3)+5=6 | min(1,3)+4=5
    //   row2: 7 8 9            dp2: min(7,6)+7=13 | min(7,6,5)+8=13 | min(6,5)+9=14
    // Answer: min(13, 13, 14) = 13 (two optimal paths: 1->4->8, 1->5->7).
    std::vector<std::vector<int>> matrix = {{2, 1, 3}, {6, 5, 4}, {7, 8, 9}};
    check(minFallingPathSum(matrix) == 13,
          "official example [[2,1,3],[6,5,4],[7,8,9]] -> 13");
  }

  {
    // Older classic variant of the same problem: the optimum hugs an edge.
    //   row0: 1 2 3            dp0: 1 2 3
    //   row1: 4 5 6            dp1: min(1,2)+4=5 | min(1,2,3)+5=6 | min(2,3)+6=8
    //   row2: 7 8 9            dp2: min(5,6)+7=12 | min(5,6,8)+8=13 | min(6,8)+9=15
    // Answer: min(12, 13, 15) = 12, via the straight-left-edge path 1->4->7.
    std::vector<std::vector<int>> matrix = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    check(minFallingPathSum(matrix) == 12,
          "classic variant [[1,2,3],[4,5,6],[7,8,9]] -> 12 (left edge)");
  }

  {
    // Official LeetCode Example 2 — negative values: the optimum may require
    // picking locally large cells to reach even larger negatives below.
    //   row0: -19  57          dp1: min(-19,57) + (-40) = -59
    //   row1: -40  -5               min(-19,57) + (-5)  = -24
    // Answer: min(-59, -24) = -59.
    std::vector<std::vector<int>> matrix = {{-19, 57}, {-40, -5}};
    check(minFallingPathSum(matrix) == -59,
          "official negative example -> -59");
  }

  {
    // Degenerate single row (1 x n): outside LeetCode's square-matrix
    // contract, but handled defensively by the early return — the path is
    // just the cheapest entry of the only row.
    std::vector<std::vector<int>> matrix = {{3, -1, 2}};
    check(minFallingPathSum(matrix) == -1, "single row -> cheapest entry (-1)");
  }

  {
    // Degenerate 1x1 matrix: start == destination, answer is its own value.
    std::vector<std::vector<int>> matrix = {{7}};
    check(minFallingPathSum(matrix) == 7, "1x1 matrix -> 7");
  }

  {
    // Boundary-column correctness: the best path hugs column 0, exercising
    // the "no up-left neighbor exists" branch at every row.
    //   row0: 0  99  99        dp1: 0+1=1 | min(0,99,99)+99=99 ...
    //   row1: 1  99  99        dp2: 1+2=3 | ...
    //   row2: 2  99  99        answer = 3, straight down column 0.
    std::vector<std::vector<int>> matrix = {{0, 99, 99}, {1, 99, 99}, {2, 99, 99}};
    check(minFallingPathSum(matrix) == 3,
          "left-edge path exercises missing up-left neighbor -> 3");
  }

  {
    // Mirror of the previous test for the RIGHT boundary (missing up-right).
    std::vector<std::vector<int>> matrix = {{99, 99, 0}, {99, 99, 1}, {99, 99, 2}};
    check(minFallingPathSum(matrix) == 3,
          "right-edge path exercises missing up-right neighbor -> 3");
  }

  {
    // A zig-zag optimum: verifies diagonal moves are honored (not just
    // straight-down), by making straight-down expensive everywhere.
    // Optimal: 1 -> 10 (diag right)? Cheaper: 1 -> 2 (down) -> ... trace:
    //   row0: 1  50  50        dp1: 1+2=3? cells row1 = {2, 10, 50}:
    //   row1: 2  10  50               c0: min(1,50)+2 = 3
    //   row2: 40 3  50                c1: min(1,50,50)+10 = 11
    //                                 c2: min(50,50)+50 = 100
    //                          dp2: c0: min(3,11)+40 = 43
    //                               c1: min(3,11,100)+3 = 6
    //                               c2: min(11,100)+50 = 61
    // Answer 6 via 1(col0) -> 10(col1, diagonal) -> 3(col1, straight down):
    // requires the diagonal move; straight-down-only would give 43.
    std::vector<std::vector<int>> matrix = {{1, 50, 50}, {2, 10, 50}, {40, 3, 50}};
    check(minFallingPathSum(matrix) == 6,
          "zig-zag optimum requires diagonal moves -> 6");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
