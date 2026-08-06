// ============================================================================
// LeetCode 51 — N-Queens
// ============================================================================
//
// PROBLEM
// -------
// Place n queens on an n x n chessboard such that no two queens attack each
// other (no shared row, column, or diagonal). Return ALL distinct solutions,
// each as a board of n strings ('Q' for a queen, '.' for empty).
//
// APPROACH — Backtracking
// ------------------------
// This is the textbook Backtracking problem (see ../README.md). The decision
// tree places one queen per row, left to right through the rows. At each row
// we have n candidate columns; most of them are invalid given the queens
// already placed above, so we PRUNE instead of exploring every full-depth
// combination and checking validity only at the leaves:
//
//   for each candidate column in this row:
//     if placing here conflicts with an already-placed queen: skip (PRUNE)
//     else:
//       1) CHOOSE  — place the queen here
//       2) RECURSE — solve the remaining rows
//       3) UNDO    — remove the queen before trying the next column
//
// The "is this partial state still valid?" check (isSafe below) only needs
// to compare the new queen against queens already placed in EARLIER rows,
// because we place exactly one queen per row -- two placed queens can never
// already share a row.
//
// COMPLEXITY
// ----------
// Worst case is still exponential: without any pruning there are n^n ways
// to drop n queens on an n x n board ignoring conflicts entirely, or n!
// if we only enforce "one per row and one per column" up front (as this
// solution effectively does by construction). Pruning on diagonals collapses
// the PRACTICALLY explored tree dramatically below n! -- for n=8, brute-force
// row/column placement considers 8! = 40320 permutations, but this solution
// prunes the vast majority of those before ever reaching row 8, leaving only
// 92 valid boards. See ../README.md "Complexity" for the full discussion of
// why pruning changes the practical runtime enormously without changing the
// asymptotic worst case.
// Time:  O(n!) candidate placements considered in the worst case (bounded
//        further in practice by diagonal pruning).
// Space: O(n) for the recursion stack and the colOfRow state, plus O(n^2)
//        per solution for the output boards.
// ============================================================================

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool isSafe(const std::vector<int>& colOfRow, int row, int col) {
  for (int prevRow = 0; prevRow < row; ++prevRow) {
    int prevCol = colOfRow[prevRow];
    if (prevCol == col) return false;                                      // same column
    if (std::abs(prevCol - col) == std::abs(prevRow - row)) return false;  // same diagonal
  }
  return true;
}

std::vector<std::string> buildBoard(const std::vector<int>& colOfRow, int n) {
  std::vector<std::string> board(n, std::string(n, '.'));
  for (int row = 0; row < n; ++row) {
    board[row][colOfRow[row]] = 'Q';
  }
  return board;
}

void backtrack(int n, int row, std::vector<int>& colOfRow,
               std::vector<std::vector<std::string>>& solutions) {
  if (row == n) {
    solutions.push_back(buildBoard(colOfRow, n));
    return;
  }

  for (int col = 0; col < n; ++col) {
    if (!isSafe(colOfRow, row, col)) {
      continue;  // Prune: doomed placement, do not recurse into it.
    }

    colOfRow[row] = col;                        // 1) CHOOSE
    backtrack(n, row + 1, colOfRow, solutions);  // 2) RECURSE
    colOfRow[row] = -1;                          // 3) UNDO
  }
}

}  // namespace

std::vector<std::vector<std::string>> solveNQueens(int n) {
  std::vector<std::vector<std::string>> solutions;
  if (n <= 0) return solutions;
  std::vector<int> colOfRow(n, -1);
  backtrack(n, 0, colOfRow, solutions);
  return solutions;
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

  check(solveNQueens(1).size() == 1, "n=1 -> 1 solution");
  check(solveNQueens(2).empty(), "n=2 -> 0 solutions");
  check(solveNQueens(3).empty(), "n=3 -> 0 solutions");
  check(solveNQueens(4).size() == 2, "n=4 -> 2 solutions");
  check(solveNQueens(5).size() == 10, "n=5 -> 10 solutions");
  check(solveNQueens(8).size() == 92, "n=8 -> 92 solutions");

  {
    auto solutions = solveNQueens(4);
    std::vector<std::vector<std::string>> expected = {
        {".Q..", "...Q", "Q...", "..Q."},
        {"..Q.", "Q...", "...Q", ".Q.."},
    };
    bool matches = true;
    for (const auto& sol : expected) {
      bool found = false;
      for (const auto& actual : solutions) {
        if (actual == sol) { found = true; break; }
      }
      matches = matches && found;
    }
    check(matches, "n=4 solutions match known board layouts exactly");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
