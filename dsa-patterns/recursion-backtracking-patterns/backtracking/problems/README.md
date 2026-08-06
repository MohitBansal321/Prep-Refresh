# Backtracking — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Backtracking across constraint-satisfaction problems of increasing shape variety: board placement, board filling, grid path-finding, and string partitioning. Each file is self-contained: compile and run it directly to see printed `[PASS]`/`[FAIL]` output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-n-queens.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Complexity | File |
|------|-----------|------------|--------------------|------------|------|
| N-Queens | [51](https://leetcode.com/problems/n-queens/) | Hard | Place one queen per row; prune any column that shares a column/diagonal with an already-placed queen before recursing to the next row. | O(n!) worst case, O(n) space | [01-n-queens.cpp](01-n-queens.cpp) |
| Sudoku Solver | [37](https://leetcode.com/problems/sudoku-solver/) | Hard | Scan to the next empty cell, try digits 1-9, prune any digit that conflicts with its row/column/3x3 box, undo on failure. | O(9^m) worst case (m = empty cells), O(m) space | [02-sudoku-solver.cpp](02-sudoku-solver.cpp) |
| Word Search | [79](https://leetcode.com/problems/word-search/) | Medium | DFS from each starting cell, marking cells used on the current path in place and unmarking them on the way back out. | O(m·n·4^L) worst case (L = word length), O(L) space | [03-word-search.cpp](03-word-search.cpp) |
| Palindrome Partitioning | [131](https://leetcode.com/problems/palindrome-partitioning/) | Medium | Try every prefix of the remaining string as the next piece, prune any prefix that is not itself a palindrome, recurse on the rest. | O(n·2^n) worst case, O(n) space | [04-palindrome-partitioning.cpp](04-palindrome-partitioning.cpp) |

## Why these four

They cover every recognition signal and every shape variation called out in the [README](../README.md):
- **01 (N-Queens)** is the canonical, purest form of the pattern -- one decision per row, a clean column/diagonal constraint, and the textbook example used throughout the README and [code.cpp](../code.cpp).
- **02 (Sudoku Solver)** shows the SAME skeleton applied to a 2D grid with a more elaborate (three-part: row/column/box) constraint, plus the "undo only on failure, keep on success" short-circuit variation of the undo step.
- **03 (Word Search)** shows the partial state living **in the grid itself** (an in-place "used" marker) rather than in a separate array, and is the sharpest illustration of why forgetting to undo corrupts sibling branches.
- **04 (Palindrome Partitioning)** shows the constraint check moving away from "board position" entirely, onto a **substring property**, and is the one problem here whose adversarial worst case (an all-same-character string) genuinely reaches the exponential ceiling instead of pruning far below it.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
