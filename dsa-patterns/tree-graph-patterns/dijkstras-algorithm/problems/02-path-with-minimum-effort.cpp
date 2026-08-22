// ============================================================================
// LeetCode 1631 — Path With Minimum Effort
// ============================================================================
//
// PROBLEM
// -------
// You are given an `rows x cols` grid of integers where grid[r][c] is the
// elevation of cell (r, c). You start at the top-left cell and want to reach
// the bottom-right cell. You may move up, down, left, or right (no
// diagonals), and the "effort" of a route is the MAXIMUM absolute difference
// in elevations between consecutive cells along it. Return the minimum
// effort required to travel from top-left to bottom-right.
//
// Example: grid = [[1,2,2],[3,8,2],[5,3,5]]  ->  2
//          (path [1,3,5,3,5] has maximum step difference |1-3|=2.)
//
// APPROACH — Dijkstra over grid cells with a max-based relaxation rule
// ---------------------------------------------------------------------
// A grid is just a graph in disguise: each cell is a node, and each pair of
// orthogonally adjacent cells is joined by two directed edges whose weight
// is the absolute elevation difference between them. That makes this a
// shortest-path problem on a weighted graph — but with one twist that makes
// it NOT the textbook sum-of-weights form:
//
//   the cost of a path is not the SUM of its edge weights; it is the MAXIMUM
//   edge weight on the path. We want the path minimizing that maximum.
//
// The fix is tiny: change one line of the relaxation rule.
//   Sum version:      dist[v] = dist[u] + w            (accumulate)
//   Max version:      dist[v] = max(dist[u], w)        (path cost = worst edge)
//
// The greedy correctness argument survives the twist intact: when the
// min-heap pops the smallest tentative "effort" e for a cell, no future path
// can do better — every other candidate still sitting in the heap already
// carries effort >= e, and appending more edges can only keep the max equal
// or raise it (max is monotone non-decreasing as you extend a path). So the
// first useful pop finalizes the cell exactly as in sum-Dijkstra, and all we
// did was swap which quantity plays "distance".
//
// This problem is also a good reminder that the graph is often IMPLICIT:
// we never build an adjacency list — neighbours are computed on the fly
// from (row, col) offsets, and the heap holds cells rather than node ids.
//
// COMPLEXITY
// ----------
// Time:  O(V log V) with V = rows*cols cells — each cell pushes at most 4
//        improved entries (one per neighbour relaxation), so the heap holds
//        O(V) entries and each push/pop is O(log V). (Equivalently
//        O(E log V) with E ~ 4V edges.)
// Space: O(V) — the effort array plus the heap.
//
// Contrast: binary-searching the answer over effort values + BFS feasibility
// check runs in O(V log(maxDiff)) and also works, but the Dijkstra form is
// the direct generalization of this module's template.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

const int kInf = std::numeric_limits<int>::max();

int minimumEffortPath(const std::vector<std::vector<int> >& grid) {
  const int rows = static_cast<int>(grid.size());
  const int cols = static_cast<int>(grid[0].size());

  // effort[r][c] = smallest known value of "maximum step along a path"
  // from (0,0) to (r,c).
  std::vector<std::vector<int> > effort(rows, std::vector<int>(cols, kInf));
  effort[0][0] = 0;

  // Min-heap of {effortSoFar, row, col}; greater<> flips to min-heap,
  // and tuple comparison orders by effortSoFar first — exactly what we need.
  std::priority_queue<std::tuple<int, int, int>,
                      std::vector<std::tuple<int, int, int> >,
                      std::greater<std::tuple<int, int, int> > >
      pq;
  pq.push(std::make_tuple(0, 0, 0));

  const int dr[4] = {-1, 1, 0, 0};
  const int dc[4] = {0, 0, -1, 1};

  while (!pq.empty()) {
    const int d = std::get<0>(pq.top());
    const int r = std::get<1>(pq.top());
    const int c = std::get<2>(pq.top());
    pq.pop();
    if (d > effort[r][c]) continue;  // stale entry — a better one already won

    // Early exit: destination popped usefully -> its effort is final.
    if (r == rows - 1 && c == cols - 1) return d;

    for (int i = 0; i < 4; ++i) {
      const int nr = r + dr[i];
      const int nc = c + dc[i];
      if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;

      // The ONLY structural change vs sum-Dijkstra lives on these two lines:
      // a path's cost is max(previous path cost, this edge's weight), not a
      // running sum.
      const int step = std::abs(grid[nr][nc] - grid[r][c]);
      const int newEffort = std::max(d, step);

      if (newEffort < effort[nr][nc]) {
        effort[nr][nc] = newEffort;
        pq.push(std::make_tuple(newEffort, nr, nc));
      }
    }
  }
  return effort[rows - 1][cols - 1];  // always reached on a connected grid
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
    std::vector<std::vector<int> > grid;
    grid.push_back({1, 2, 2});
    grid.push_back({3, 8, 2});
    grid.push_back({5, 3, 5});
    check(minimumEffortPath(grid) == 2,
          "classic example [[1,2,2],[3,8,2],[5,3,5]] -> 2");
  }

  {
    // Must route around the tall 8: go right along the top row (steps
    // |1-2|=1, |2-3|=1), then down the right column (|3-4|=1, |4-5|=1).
    // Every step costs <= 1 -> optimal effort 1.
    std::vector<std::vector<int> > grid;
    grid.push_back({1, 2, 3});
    grid.push_back({3, 8, 4});
    grid.push_back({5, 3, 5});
    check(minimumEffortPath(grid) == 1,
          "route around the peak -> 1");
  }

  {
    // Single cell: start IS the destination, zero effort, no moves at all.
    std::vector<std::vector<int> > grid;
    grid.push_back({7});
    check(minimumEffortPath(grid) == 0,
          "single cell grid -> 0");
  }

  {
    // Two cells with a huge cliff: no way around, forced single step.
    std::vector<std::vector<int> > grid;
    grid.push_back({1, 1000000});
    check(minimumEffortPath(grid) == 999999,
          "forced crossing of a large cliff");
  }

  {
    // Flat grid: every step costs 0 regardless of route shape.
    std::vector<std::vector<int> > grid;
    grid.push_back({4, 4, 4});
    grid.push_back({4, 4, 4});
    check(minimumEffortPath(grid) == 0,
          "flat grid -> 0 effort");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
