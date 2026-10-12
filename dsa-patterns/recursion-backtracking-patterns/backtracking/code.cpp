// ============================================================================
// Backtracking — generic reusable template (C++17): N-Queens solver
// ============================================================================
//
// This file is NOT a single worked LeetCode submission (see problems/01-n-queens.cpp
// for that, with PASS/FAIL tests against known answers). Instead it demonstrates
// the three-step BACKTRACKING skeleton in its clearest possible form, so you can
// see the shape of the pattern before looking at problem-specific variations:
//
//   1. CHOOSE  — commit to one candidate for the current decision slot.
//   2. RECURSE — try to complete the rest of the solution given that choice.
//   3. UNDO    — remove the choice before trying the next candidate, so
//                sibling branches see the state exactly as it was before we
//                touched it.
//
// Sitting in front of CHOOSE is the PRUNE step: before recursing, check
// whether the partial state built so far can still possibly lead to a valid
// solution. If it cannot, skip straight to the next candidate without
// recursing at all -- that early exit is the entire value of backtracking
// over generate-then-filter brute force (see ../README.md "Why Not Other
// Approaches").
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


namespace {

// The partial-solution state for N-Queens is deliberately compact: one
// column index per row already decided. colOfRow[r] == c means "the queen in
// row r sits in column c"; rows >= the current recursion depth are still
// unset (represented here as -1, though isSafe() never reads unset rows).
//
// Returns true if placing a queen at (row, col) creates NO conflict with any
// queen already placed in rows [0, row). This is the "is this partial state
// still valid?" constraint check described in the README's Architecture
// section. We only need to check column and both diagonals -- never the row
// itself -- because the recursion places exactly one queen per row, so two
// placed queens can never already share a row.
bool isSafe(const vector<int>& colOfRow, int row, int col) {
  for (int prevRow = 0; prevRow < row; ++prevRow) {
    int prevCol = colOfRow[prevRow];
    if (prevCol == col) {
      return false;  // Same column -> queens attack along the column.
    }
    if (abs(prevCol - col) == abs(prevRow - row)) {
      return false;  // Same diagonal -> queens attack along a diagonal.
    }
  }
  return true;
}

// Converts the compact colOfRow representation into the board-of-strings
// format LeetCode (and most textbook presentations) expect: n strings of
// length n, 'Q' for an occupied square and '.' for empty.
vector<string> buildBoard(const vector<int>& colOfRow, int n) {
  vector<string> board(n, string(n, '.'));
  for (int row = 0; row < n; ++row) {
    board[row][colOfRow[row]] = 'Q';
  }
  return board;
}

// The recursive engine. `row` is the decision slot we are currently filling:
// "which column gets the queen in this row?" Every row from 0..n-1 must get
// exactly one queen for a solution to be complete.
void backtrack(int n, int row, vector<int>& colOfRow,
               vector<vector<string>>& solutions) {
  if (row == n) {
    // Base case: every row has a queen placed with zero conflicts (every
    // placement along the way already passed isSafe()), so this is a
    // complete, valid solution. Record it. There is nothing to undo in this
    // frame -- the undo for the choice that got us here happens in the
    // CALLER's frame, immediately after this call returns.
    solutions.push_back(buildBoard(colOfRow, n));
    return;
  }

  for (int col = 0; col < n; ++col) {
    if (!isSafe(colOfRow, row, col)) {
      // PRUNE: this candidate cannot possibly lead to a valid solution, no
      // matter what we do in later rows, so we skip it WITHOUT recursing.
      // This is the entire complexity win over brute force: a brute-force
      // generator would still build out every remaining row for this
      // doomed placement before discovering the conflict at the leaf.
      continue;
    }

    // 1) CHOOSE: commit to a queen at (row, col).
    colOfRow[row] = col;

    // 2) RECURSE: try to complete rows [row+1, n) given this choice.
    backtrack(n, row + 1, colOfRow, solutions);

    // 3) UNDO: remove the queen so the NEXT candidate column in this same
    // row sees colOfRow exactly as it was before we chose `col`. Forgetting
    // this line does not crash -- it silently corrupts colOfRow so isSafe()
    // makes wrong decisions for every remaining sibling column, which is
    // exactly the "forgotten undo" mistake described in the README.
    colOfRow[row] = -1;
  }
}

}  // namespace

// Public API: returns every distinct way to place n non-attacking queens on
// an n x n board. Each solution is n strings of length n ('Q' for a queen,
// '.' for empty), matching LeetCode 51's expected output shape.
vector<vector<string>> solveNQueens(int n) {
  vector<vector<string>> solutions;
  if (n <= 0) return solutions;

  vector<int> colOfRow(n, -1);
  backtrack(n, 0, colOfRow, solutions);
  return solutions;
}

// ============================================================================
// main() — demonstrates solveNQueens() with printed, verifiable output.
// ============================================================================
int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](bool condition, const string& label) {
    if (condition) {
      cout << "[PASS] " << label << "\n";
      ++pass_count;
    } else {
      cout << "[FAIL] " << label << "\n";
      ++fail_count;
    }
  };

  cout << "--- n = 1 ---\n";
  {
    auto solutions = solveNQueens(1);
    check(solutions.size() == 1, "n=1 has exactly 1 solution (trivial single square)");
  }

  cout << "\n--- n = 2 and n = 3 (no solution exists) ---\n";
  {
    check(solveNQueens(2).empty(), "n=2 has 0 solutions");
    check(solveNQueens(3).empty(), "n=3 has 0 solutions");
  }

  cout << "\n--- n = 4 (classic textbook case) ---\n";
  {
    auto solutions = solveNQueens(4);
    check(solutions.size() == 2, "n=4 has exactly 2 solutions");

    for (size_t i = 0; i < solutions.size(); ++i) {
      cout << "Solution " << (i + 1) << ":\n";
      for (const auto& line : solutions[i]) {
        cout << "  " << line << "\n";
      }
    }

    // Known solution set for n=4 (order may vary by column-scan direction,
    // so we check membership rather than exact vector equality).
    vector<vector<string>> expected = {
        {".Q..", "...Q", "Q...", "..Q."},
        {"..Q.", "Q...", "...Q", ".Q.."},
    };
    bool all_expected_present = true;
    for (const auto& sol : expected) {
      bool found = false;
      for (const auto& actual : solutions) {
        if (actual == sol) {
          found = true;
          break;
        }
      }
      all_expected_present = all_expected_present && found;
    }
    check(all_expected_present, "n=4 solutions match the known board layouts");
  }

  cout << "\n--- n = 8 (standard 8-queens count) ---\n";
  {
    auto solutions = solveNQueens(8);
    check(solutions.size() == 92, "n=8 has exactly 92 solutions");
  }

  cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
