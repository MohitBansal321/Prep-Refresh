// ============================================================================
// LeetCode 200 — Number of Islands
// https://leetcode.com/problems/number-of-islands/
// ============================================================================
//
// PROBLEM
// -------
// Given an m x n 2D binary grid of '1' (land) and '0' (water), return the
// number of islands. An island is a group of '1' cells connected
// 4-directionally (up / down / left / right — NOT diagonally), and the grid is
// surrounded by water on all sides.
//
// Example: grid = { "11110",
//                   "11010",
//                   "11000",
//                   "00000" }  -> 1
//
//          grid = { "11000",
//                   "11000",
//                   "00100",
//                   "00011" }  -> 3
//
// RECOGNITION SIGNAL — "grid as an implicit graph" + "connected components"
// -------------------------------------------------------------------------
// Nothing here says "graph." There is no adjacency list, no edge list, no node
// ids. But the grid IS a graph: every cell is a node, and every cell has (at
// most) four edges — to its up, down, left, and right neighbours, provided
// they are in bounds and are also land. Once you see that, "count the islands"
// is literally "count the connected components," which is `dfsConnectedComponents`
// in ../code.cpp with a different way of enumerating neighbours.
//
// BFS vs DFS — WHICH ONE AND WHY
// -------------------------------
// **Either works, and that is the whole point of this file.** The question is
// "how many separate blobs are there," which is a pure *reachability /
// structure* question. It never asks how FAR anything is from anything else,
// so BFS's ring-by-ring shortest-hop guarantee buys us nothing here — we would
// pay for a queue and a distance concept we never read.
//
// So we pick DFS, for one reason: it is less code. A recursive flood fill is
// four recursive calls and a bounds check; the BFS equivalent needs an explicit
// queue plus push/pop bookkeeping for exactly the same answer.
//
// The one caveat, and it is a real production one: recursive DFS on a grid that
// is ALL land recurses up to m*n frames deep (a 1000x1000 all-land grid is a
// million nested calls) and will blow the call stack. The iterative BFS version
// — or an iterative DFS with an explicit std::stack — has no such limit. That
// tradeoff, not "shortest path," is the reason to switch traversals here.
// Contrast with 04-word-ladder.cpp, where BFS is not a preference but a
// correctness requirement.
//
// APPROACH
// --------
//   1. Scan every cell of the grid in row-major order (this is the "loop over
//      every node as a potential unvisited start" discipline from the README's
//      Common Mistakes — without it, a second island is never found).
//   2. When a cell is land AND not yet visited, that cell begins a brand-new
//      island: increment the counter, then flood-fill (DFS) outward from it,
//      marking every land cell reachable from it as visited.
//   3. Because step 2 marks the entire island, the outer scan will walk over
//      the rest of that island's cells and skip them — so the counter is
//      incremented exactly once per island.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// Marking visited at the WRONG time. The mark must happen the instant a cell
// is entered (on discovery), before recursing into its neighbours. If you
// instead check "am I visited?" only at the top and mark at the bottom (after
// the four recursive calls), then A recurses into B and B recurses straight
// back into A, which is not yet marked -> infinite mutual recursion. A grid has
// cycles in exactly this trivial two-cell way, which is why the guard matters
// even though a grid "looks" acyclic.
//
// COMPLEXITY
// ----------
// Time:  O(m*n) -- every cell is looked at by the outer scan once, and entered
//                  by the flood fill at most once (the visited mark guarantees
//                  it). Each entry examines 4 neighbours -> O(1) per cell.
// Space: O(m*n) -- the visited grid, plus the DFS recursion depth, which in the
//                  worst case (all land) is also O(m*n).
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// floodFill — recursive DFS from one land cell, marking the whole island.
//
// `grid` is taken by const reference and a SEPARATE `visited` grid is used,
// rather than the popular trick of overwriting '1' with '0' in place. Both are
// correct and the in-place version saves O(m*n) memory, but it destroys the
// caller's input — which matters here because main() runs several assertions
// against the same fixture grid, and would otherwise need to rebuild it every
// time. In an interview, say out loud which one you are doing and why.
// ----------------------------------------------------------------------------
void floodFill(const std::vector<std::string>& grid,
               std::vector<std::vector<bool> >& visited,
               int row,
               int col) {
  int rows = static_cast<int>(grid.size());
  int cols = rows == 0 ? 0 : static_cast<int>(grid[0].size());

  // Guard 1: off the edge of the grid. Checking bounds INSIDE the recursive
  // call (rather than at all four call sites) keeps the caller simple, at the
  // cost of one extra function call per out-of-bounds neighbour.
  if (row < 0 || row >= rows || col < 0 || col >= cols) return;

  // Guard 2: water is not part of any island — it is a non-edge, so the
  // traversal simply does not cross it.
  if (grid[row][col] != '1') return;

  // Guard 3: the `visited` check. This is what makes the traversal terminate:
  // cell A's recursion into its right neighbour B is immediately followed by
  // B's recursion back into its left neighbour A, and only this mark stops
  // that from bouncing forever.
  if (visited[row][col]) return;

  visited[row][col] = true;  // MARK ON DISCOVERY, before recursing. Not after.

  // The four 4-directional neighbours. Diagonals are deliberately absent:
  // LeetCode 200 defines connectivity as 4-directional, so two land cells
  // touching only at a corner are TWO islands, not one (see the
  // "diagonal touch" test case below). Compare with LeetCode 1091, which uses
  // 8 directions — the direction set is a per-problem decision, not a
  // property of grids.
  floodFill(grid, visited, row - 1, col);  // up
  floodFill(grid, visited, row + 1, col);  // down
  floodFill(grid, visited, row, col - 1);  // left
  floodFill(grid, visited, row, col + 1);  // right
}

int numIslands(const std::vector<std::string>& grid) {
  int rows = static_cast<int>(grid.size());
  if (rows == 0) return 0;
  int cols = static_cast<int>(grid[0].size());
  if (cols == 0) return 0;

  std::vector<std::vector<bool> > visited(rows, std::vector<bool>(cols, false));
  int islands = 0;

  // The outer scan over EVERY cell is the grid version of "loop over every
  // node as a potential unvisited start point." A single flood fill from the
  // first land cell would only ever find that one island; islands are
  // disconnected components by definition, so they can only be discovered by
  // this outer loop.
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      if (grid[row][col] == '1' && !visited[row][col]) {
        ++islands;  // A land cell nothing has reached yet => a new component.
        floodFill(grid, visited, row, col);
      }
    }
  }

  return islands;
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
    std::vector<std::string> grid;
    grid.push_back("11110");
    grid.push_back("11010");
    grid.push_back("11000");
    grid.push_back("00000");
    check(numIslands(grid) == 1, "LeetCode example 1: one large connected island -> 1");
  }

  {
    std::vector<std::string> grid;
    grid.push_back("11000");
    grid.push_back("11000");
    grid.push_back("00100");
    grid.push_back("00011");
    check(numIslands(grid) == 3, "LeetCode example 2: three separate islands -> 3");
  }

  {
    // Empty input: the m == 0 early return must fire before any indexing.
    std::vector<std::string> grid;
    check(numIslands(grid) == 0, "empty grid (no rows) -> 0");
  }

  {
    // A row of zero width: cols == 0 must also be handled, or grid[0][0]
    // reads out of bounds.
    std::vector<std::string> grid;
    grid.push_back("");
    check(numIslands(grid) == 0, "grid with one zero-width row -> 0");
  }

  {
    std::vector<std::string> grid;
    grid.push_back("1");
    check(numIslands(grid) == 1, "single cell of land -> 1");
  }

  {
    std::vector<std::string> grid;
    grid.push_back("0");
    check(numIslands(grid) == 0, "single cell of water -> 0");
  }

  {
    // All water: the outer scan visits every cell and never starts a fill.
    std::vector<std::string> grid;
    grid.push_back("000");
    grid.push_back("000");
    check(numIslands(grid) == 0, "all water -> 0");
  }

  {
    // All land: one island, and the deepest recursion this file performs
    // (every cell is one frame on the call stack in the worst ordering).
    std::vector<std::string> grid;
    grid.push_back("111");
    grid.push_back("111");
    grid.push_back("111");
    check(numIslands(grid) == 1, "all land -> 1 (also the max-recursion-depth case)");
  }

  {
    // THE 4-DIRECTIONAL TEST. These two land cells touch only at a corner.
    // Under 4-directional connectivity they are two islands; a solution that
    // sloppily also recursed diagonally would answer 1 and be wrong.
    std::vector<std::string> grid;
    grid.push_back("10");
    grid.push_back("01");
    check(numIslands(grid) == 2, "diagonal-only touch is NOT connected -> 2, not 1");
  }

  {
    // A "checkerboard" maximises the number of components: every land cell is
    // its own island. 5 land cells in a 3x3 checkerboard.
    std::vector<std::string> grid;
    grid.push_back("101");
    grid.push_back("010");
    grid.push_back("101");
    check(numIslands(grid) == 5, "3x3 checkerboard -> 5 single-cell islands");
  }

  {
    // Single row / single column: the bounds guards must not walk off either
    // end of a 1-wide grid.
    std::vector<std::string> row;
    row.push_back("10101");
    check(numIslands(row) == 3, "single row 10101 -> 3");

    std::vector<std::string> column;
    column.push_back("1");
    column.push_back("0");
    column.push_back("1");
    check(numIslands(column) == 2, "single column 1,0,1 -> 2");
  }

  {
    // A U-shape: proves the fill genuinely walks around a bend rather than
    // only travelling in straight lines.
    std::vector<std::string> grid;
    grid.push_back("101");
    grid.push_back("101");
    grid.push_back("111");
    check(numIslands(grid) == 1, "U-shape (connected only around the bottom) -> 1");
  }

  {
    // Input must survive the call unmodified — this is the payoff for using a
    // separate `visited` grid instead of overwriting '1' with '0' in place.
    std::vector<std::string> grid;
    grid.push_back("11");
    grid.push_back("11");
    std::vector<std::string> before = grid;
    numIslands(grid);
    check(grid == before, "input grid is not mutated by numIslands");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
