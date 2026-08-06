// ============================================================================
// LeetCode 37 — Sudoku Solver (Hard)
// ============================================================================
//
// PROBLEM
// -------
// Given a 9x9 board partially filled with digits '1'-'9' and empty cells
// marked '.', fill the empty cells so that every row, every column, and
// every 3x3 sub-box contains each digit 1-9 exactly once. Solve the board
// IN PLACE. Exactly one solution is guaranteed to exist for valid inputs.
//
// APPROACH — Backtracking
// ------------------------
// Sudoku is a constraint-satisfaction problem, the category Backtracking
// exists for (see ../README.md "Problem"). The decision tree scans the board
// for the next empty cell and tries digits 1-9 in it. Most digits conflict
// with an existing row/column/box entry, so we PRUNE before recursing rather
// than filling the whole board and checking validity only at the end:
//
//   for each empty cell (scanned in row-major order):
//     for digit in '1'..'9':
//       if digit conflicts with row/column/box: skip (PRUNE)
//       else:
//         1) CHOOSE  — place digit in this cell
//         2) RECURSE — solve the rest of the board
//         3) UNDO    — if the recursive call could not complete a solution,
//                      erase the digit (reset the cell to '.') before trying
//                      the next candidate digit
//
// Because we mutate the board argument in place (as LeetCode requires),
// forgetting the UNDO step here is the single easiest way to corrupt shared
// state -- a failed digit choice would otherwise leak into every sibling
// branch's row/column/box checks (see ../README.md "Common Mistakes").
//
// solveSudoku returns bool (found a valid completion or not) so recursion
// can short-circuit: once one recursive call succeeds, every enclosing call
// simply returns true without undoing its own last successful choice --
// only FAILED choices get undone, because Sudoku's contract guarantees a
// unique solution and we stop at the first (only) one found.
//
// COMPLEXITY
// ----------
// Worst case is exponential in the number of empty cells: up to 9 candidate
// digits per empty cell, so O(9^m) for m empty cells in the absolute worst
// case. In practice, row/column/box pruning eliminates the overwhelming
// majority of candidate digits at every cell almost immediately (a mostly
// filled board leaves only 1-3 plausible digits per empty cell), which is
// why this simple backtracking solver finishes near-instantly on real
// puzzles despite the exponential worst-case bound -- see ../README.md
// "Complexity" for the general argument.
// Time:  O(9^m) worst case, m = number of empty cells; far smaller in
//        practice due to constraint pruning.
// Space: O(m) recursion depth in the worst case (at most 81).
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr int kSize = 9;
constexpr int kBoxSize = 3;

bool isValidPlacement(const std::vector<std::vector<char>>& board, int row,
                       int col, char digit) {
  for (int i = 0; i < kSize; ++i) {
    if (board[row][i] == digit) return false;  // same row
    if (board[i][col] == digit) return false;  // same column
  }

  int boxRowStart = (row / kBoxSize) * kBoxSize;
  int boxColStart = (col / kBoxSize) * kBoxSize;
  for (int r = boxRowStart; r < boxRowStart + kBoxSize; ++r) {
    for (int c = boxColStart; c < boxColStart + kBoxSize; ++c) {
      if (board[r][c] == digit) return false;  // same 3x3 box
    }
  }
  return true;
}

// Returns true once the board (from this cell onward) is completely and
// validly filled. Mutates `board` in place.
bool backtrack(std::vector<std::vector<char>>& board, int row, int col) {
  if (row == kSize) {
    return true;  // Fell off the last row -> every cell filled validly.
  }

  int nextRow = (col == kSize - 1) ? row + 1 : row;
  int nextCol = (col == kSize - 1) ? 0 : col + 1;

  if (board[row][col] != '.') {
    // Already filled from the puzzle's input -- nothing to choose here.
    return backtrack(board, nextRow, nextCol);
  }

  for (char digit = '1'; digit <= '9'; ++digit) {
    if (!isValidPlacement(board, row, col, digit)) {
      continue;  // Prune: this digit conflicts with row/column/box.
    }

    board[row][col] = digit;  // 1) CHOOSE

    if (backtrack(board, nextRow, nextCol)) {  // 2) RECURSE
      return true;  // Solution found downstream -- keep this choice, do not undo it.
    }

    board[row][col] = '.';  // 3) UNDO -- this digit did not lead anywhere; retract it.
  }

  return false;  // No digit worked in this cell -> this whole branch is a dead end.
}

}  // namespace

void solveSudoku(std::vector<std::vector<char>>& board) {
  backtrack(board, 0, 0);
}

namespace {

bool boardIsFullyValid(std::vector<std::vector<char>> board) {
  // Takes `board` BY VALUE (a deliberate copy) so we can freely blank out
  // each cell to re-validate it against the rest of the board without
  // disturbing the caller's copy.
  for (int row = 0; row < kSize; ++row) {
    for (int col = 0; col < kSize; ++col) {
      if (board[row][col] == '.') return false;
      char digit = board[row][col];
      // Temporarily "remove" this cell and re-check that placing `digit`
      // back here is still valid -- a cheap way to confirm no duplicate
      // slipped past construction.
      board[row][col] = '.';
      bool ok = isValidPlacement(board, row, col, digit);
      board[row][col] = digit;
      if (!ok) return false;
    }
  }
  return true;
}

void printBoard(const std::vector<std::vector<char>>& board) {
  for (const auto& rowVec : board) {
    std::string line;
    for (char c : rowVec) line += c;
    std::cout << "  " << line << "\n";
  }
}

}  // namespace

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

  // Classic LeetCode example board.
  std::vector<std::vector<char>> board = {
      {'5', '3', '.', '.', '7', '.', '.', '.', '.'},
      {'6', '.', '.', '1', '9', '5', '.', '.', '.'},
      {'.', '9', '8', '.', '.', '.', '.', '6', '.'},
      {'8', '.', '.', '.', '6', '.', '.', '.', '3'},
      {'4', '.', '.', '8', '.', '3', '.', '.', '1'},
      {'7', '.', '.', '.', '2', '.', '.', '.', '6'},
      {'.', '6', '.', '.', '.', '.', '2', '8', '.'},
      {'.', '.', '.', '4', '1', '9', '.', '.', '5'},
      {'.', '.', '.', '.', '8', '.', '.', '7', '9'},
  };

  std::vector<std::vector<char>> expected = {
      {'5', '3', '4', '6', '7', '8', '9', '1', '2'},
      {'6', '7', '2', '1', '9', '5', '3', '4', '8'},
      {'1', '9', '8', '3', '4', '2', '5', '6', '7'},
      {'8', '5', '9', '7', '6', '1', '4', '2', '3'},
      {'4', '2', '6', '8', '5', '3', '7', '9', '1'},
      {'7', '1', '3', '9', '2', '4', '8', '5', '6'},
      {'9', '6', '1', '5', '3', '7', '2', '8', '4'},
      {'2', '8', '7', '4', '1', '9', '6', '3', '5'},
      {'3', '4', '5', '2', '8', '6', '1', '7', '9'},
  };

  solveSudoku(board);
  std::cout << "Solved board:\n";
  printBoard(board);

  check(board == expected, "classic LeetCode example solves to the known unique solution");
  check(boardIsFullyValid(board), "solved board satisfies row/column/box constraints");

  // A board that is already fully solved should be returned unchanged.
  std::vector<std::vector<char>> alreadySolved = expected;
  solveSudoku(alreadySolved);
  check(alreadySolved == expected, "already-solved board is left unchanged");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
