// ============================================================================
// LeetCode 79 — Word Search
// ============================================================================
//
// PROBLEM
// -------
// Given an m x n grid of characters and a string `word`, return true if
// `word` can be constructed from letters of sequentially adjacent cells
// (horizontally or vertically neighboring), where the same cell may not be
// used more than once within one word.
//
// APPROACH — Backtracking
// ------------------------
// The partial-solution state here is "the path traced through the grid so
// far, and which cells on that path are currently in use." At each grid cell
// we ask: does this cell's letter match the next needed letter in `word`,
// AND is this cell not already part of the current path? That is the
// constraint check. If it fails, we PRUNE -- there is no point exploring
// four more neighbors from a cell that was never a valid continuation:
//
//   at cell (r, c), needing word[index]:
//     if out of bounds, or grid[r][c] != word[index], or (r,c) already
//     used on this path: PRUNE, return false immediately
//     else:
//       1) CHOOSE  — mark (r, c) as used (so it cannot be reused later on
//                     this same path) and advance to the next needed letter
//       2) RECURSE — try all four neighbors for word[index + 1]
//       3) UNDO    — unmark (r, c) as used, REGARDLESS of whether the
//                     recursive search succeeded, so a DIFFERENT path that
//                     also wants to pass through (r, c) is not blocked by a
//                     failed attempt that happened to visit it first
//
// The UNDO step is the most instructive part of this problem: because the
// same grid is shared across every candidate starting cell and every
// candidate path, forgetting to unmark a cell after a failed attempt would
// make that cell permanently "used" for the rest of the search -- silently
// ruling out otherwise-valid paths for no real reason (see ../README.md
// "Common Mistakes").
//
// COMPLEXITY
// ----------
// Worst case is exponential in the word length: from each cell there are up
// to 4 neighbor choices per step, so O(4^L) for a word of length L, times
// O(m*n) possible starting cells -- O(m * n * 4^L) overall. In practice the
// grid-letter-mismatch and already-visited checks prune almost every branch
// within the first one or two steps for words that are not actually in the
// grid, which is why this runs fast on real m x n grids despite the
// exponential worst-case bound (see ../README.md "Complexity").
// Time:  O(m * n * 4^L) worst case, L = word length.
// Space: O(L) recursion depth; O(1) extra space per cell (an in-place mark
//        instead of a separate visited grid, see Code Walkthrough in the
//        README for why this avoids an O(m*n) auxiliary structure).
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

namespace {

bool backtrack(std::vector<std::vector<char>>& grid, const std::string& word,
                int row, int col, size_t index) {
  if (index == word.size()) {
    return true;  // Matched every character -> the word was fully traced.
  }

  int rows = static_cast<int>(grid.size());
  int cols = static_cast<int>(grid[0].size());

  if (row < 0 || row >= rows || col < 0 || col >= cols) {
    return false;  // Prune: off the grid.
  }
  if (grid[row][col] != word[index]) {
    return false;  // Prune: this cell's letter does not continue the word.
  }
  // grid[row][col] == '#' marks "already used on the CURRENT path". Since
  // '#' can never equal a real letter, this same check above also prunes
  // "already visited," with no extra visited-set data structure needed.

  char original = grid[row][col];
  grid[row][col] = '#';  // 1) CHOOSE: mark this cell as used on this path.

  // 2) RECURSE: try all four neighbors for the next needed letter.
  bool found = backtrack(grid, word, row + 1, col, index + 1) ||
               backtrack(grid, word, row - 1, col, index + 1) ||
               backtrack(grid, word, row, col + 1, index + 1) ||
               backtrack(grid, word, row, col - 1, index + 1);

  grid[row][col] = original;  // 3) UNDO: unconditionally, whether or not
                               // this path succeeded -- a later, different
                               // starting cell may need this cell free.

  return found;
}

}  // namespace

// Matches LeetCode 79's exact signature: `grid` is taken by reference and
// used as scratch space during the search (marked with '#' and restored),
// but is guaranteed fully restored to its original contents by the time
// this function returns -- every '#' mark placed during CHOOSE is undone
// before the corresponding recursive call's stack frame returns.
bool exist(std::vector<std::vector<char>>& grid, const std::string& word) {
  if (grid.empty() || grid[0].empty() || word.empty()) return false;

  int rows = static_cast<int>(grid.size());
  int cols = static_cast<int>(grid[0].size());

  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (backtrack(grid, word, r, c, 0)) {
        return true;
      }
    }
  }
  return false;
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

  std::vector<std::vector<char>> board = {
      {'A', 'B', 'C', 'E'},
      {'S', 'F', 'C', 'S'},
      {'A', 'D', 'E', 'E'},
  };

  check(exist(board, "ABCCED") == true, "\"ABCCED\" exists via a snaking path");
  check(exist(board, "SEE") == true, "\"SEE\" exists");
  check(exist(board, "ABCB") == false,
        "\"ABCB\" does not exist -- would require reusing cell (0,1) twice");
  check(exist(board, "ABCESEEEFS") == false, "word longer than any real path -> false");

  {
    std::vector<std::vector<char>> single = {{'A'}};
    check(exist(single, "A") == true, "single-cell grid matches single-letter word");
    check(exist(single, "AA") == false, "single-cell grid cannot match a 2-letter word");
  }

  {
    std::vector<std::vector<char>> board2 = {
        {'A', 'A'},
        {'A', 'A'},
    };
    std::vector<std::vector<char>> board2_original = board2;

    check(exist(board2, "AAAA") == true, "path can snake through all 4 cells of a 2x2 all-A grid");
    // The strongest direct proof of the UNDO step: `exist` mutates `board2`
    // as scratch space ('#' markers) DURING the search, but must leave it
    // byte-for-byte identical to its pre-call contents once it returns.
    check(board2 == board2_original, "grid is fully restored to its original contents after exist() returns (undo verified)");

    check(exist(board2, "AAAAA") == false, "5-letter word impossible on a 4-cell grid");
    check(board2 == board2_original, "grid still restored correctly even on a failing search");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
