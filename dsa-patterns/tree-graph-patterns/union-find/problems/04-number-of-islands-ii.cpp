// ============================================================================
// LeetCode 305 — Number of Islands II  (Hard)
// ============================================================================
//
// PROBLEM
// -------
// You are given an m x n grid, initially entirely water. You are then given
// a sequence of `positions`, each turning one more cell into land, ONE AT A
// TIME. After EACH addition, report the current number of islands (a group
// of 1s connected 4-directionally, i.e. up/down/left/right; diagonals do not
// count). Return the list of island counts, one per position processed.
//
// Example: m=3, n=3, positions = [[0,0],[0,1],[1,2],[2,1]]
//   -> [1, 1, 2, 3]
//   (after [0,0]: 1 island. After [0,1]: still 1 island, since [0,0]-[0,1]
//    are adjacent. After [1,2]: a new, separate island -> 2. After [2,1]:
//    another new, separate island -> 3.)
//
// APPROACH — Union Find (ONLINE / incremental connectivity, no re-traversal)
// ------------------------------------------------------------------------
// This is the problem that most directly justifies Union Find's existence
// over BFS/DFS: land cells arrive ONE AT A TIME, and after EVERY single
// addition we must report a fresh, correct connectivity answer. Re-running a
// flood-fill (BFS/DFS) over the whole grid after every addition to recount
// islands would cost O(m*n) per query, O(m*n*k) total for k positions --
// for a large grid processed over many additions, that is exactly the
// "re-running a traversal on every query" cost this whole module exists to
// avoid (see the README's Problem and Why Not Other Approaches sections).
//
// Union Find instead maintains connectivity INCREMENTALLY:
//   1. Flatten the 2D grid into 1D indices: index(r, c) = r * n + c. This is
//      the standard technique whenever Union Find needs to sit "underneath"
//      a 2D structure -- the DisjointSet itself only ever deals with plain
//      integers.
//   2. Keep a `is_land` boolean array (size m*n) marking which cells have
//      been turned to land so far -- necessary because a DisjointSet slot
//      exists for every cell from the start, but not every cell IS land yet;
//      we must not union a new land cell with a water neighbor.
//   3. Maintain a running `island_count`. When a NEW land cell (r, c) is
//      added (skip if it was already land -- the input can contain
//      duplicate positions per the constraints):
//        a. Mark it land, and provisionally count it as its OWN new island
//           (++island_count).
//        b. Check its up to 4 neighbors. For each neighbor that is ALREADY
//           land, attempt unionSets((r,c), neighbor). Every SUCCESSFUL union
//           (the two were in different components) means two islands just
//           became one, so --island_count. A neighbor that is land but
//           ALREADY in the same component as (r, c) (e.g. two neighbors of
//           the new cell that were already connected to each other through
//           a longer path) correctly triggers no further decrement, because
//           unionSets() returns false when the two are already connected --
//           the same "returns false = redundant edge" signal used in
//           problem 02.
//   4. Record `island_count` after processing this position; that is the
//      answer for this step. Repeat for every position -- no traversal, no
//      recomputation from scratch, ever.
//
// COMPLEXITY
// ----------
// Time:  O(k * alpha(m*n)) for k positions -- each position does O(1) work
//        (mark land, check up to 4 neighbors, up to 4 near-O(1) unions).
//        Contrast with O(m*n) BFS/DFS per query -> O(k * m * n) overall,
//        which is the cost Union Find avoids entirely.
// Space: O(m*n) for the DisjointSet arrays and the `is_land` array.
// ============================================================================

#include <iostream>
#include <numeric>
#include <string>
#include <vector>

class DisjointSet {
 public:
  explicit DisjointSet(int n) : parent_(n), rank_(n, 0) {
    std::iota(parent_.begin(), parent_.end(), 0);
  }

  int find(int x) {
    if (parent_[x] != x) {
      parent_[x] = find(parent_[x]);  // path compression
    }
    return parent_[x];
  }

  // Returns true iff a merge actually happened (x and y were previously in
  // different components) -- the caller uses this to decide whether the
  // running island count should decrease.
  bool unionSets(int x, int y) {
    int root_x = find(x);
    int root_y = find(y);
    if (root_x == root_y) return false;

    if (rank_[root_x] < rank_[root_y]) std::swap(root_x, root_y);
    parent_[root_y] = root_x;
    if (rank_[root_x] == rank_[root_y]) ++rank_[root_x];
    return true;
  }

 private:
  std::vector<int> parent_;
  std::vector<int> rank_;
};

std::vector<int> numIslands2(int m, int n, const std::vector<std::vector<int>>& positions) {
  DisjointSet dsu(m * n);
  std::vector<bool> is_land(m * n, false);
  std::vector<int> result;
  result.reserve(positions.size());

  int island_count = 0;
  const int dr[4] = {-1, 1, 0, 0};
  const int dc[4] = {0, 0, -1, 1};

  for (const auto& pos : positions) {
    int r = pos[0];
    int c = pos[1];
    int idx = r * n + c;

    if (is_land[idx]) {
      // Duplicate position: already land, no new island, no change at all.
      result.push_back(island_count);
      continue;
    }

    is_land[idx] = true;
    ++island_count;  // provisionally a new island of its own

    for (int dir = 0; dir < 4; ++dir) {
      int nr = r + dr[dir];
      int nc = c + dc[dir];
      if (nr < 0 || nr >= m || nc < 0 || nc >= n) continue;  // out of grid
      int nidx = nr * n + nc;
      if (!is_land[nidx]) continue;  // water neighbor: nothing to union

      if (dsu.unionSets(idx, nidx)) {
        // A genuine merge happened: two previously separate islands (or a
        // separate island and this brand-new cell) just became one.
        --island_count;
      }
      // If unionSets returned false, (r,c) and this neighbor were already
      // in the same component (reached via another neighbor processed
      // earlier in this same loop) -- correctly no double-decrement.
    }

    result.push_back(island_count);
  }

  return result;
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
    std::vector<std::vector<int>> positions = {{0, 0}, {0, 1}, {1, 2}, {2, 1}};
    std::vector<int> expected = {1, 1, 2, 3};
    check(numIslands2(3, 3, positions) == expected,
          "3x3 grid, classic example -> [1,1,2,3]");
  }

  {
    // A single cell added, then another that merges with it diagonally is
    // NOT adjacent (diagonals don't count) -- both stay separate islands.
    std::vector<std::vector<int>> positions = {{0, 0}, {1, 1}};
    std::vector<int> expected = {1, 2};
    check(numIslands2(2, 2, positions) == expected,
          "diagonal-only neighbors never merge -> [1,2]");
  }

  {
    // A bridge cell connects two previously separate islands, testing the
    // "merge two existing islands" path (not just "extend one island").
    std::vector<std::vector<int>> positions = {{0, 0}, {0, 2}, {0, 1}};
    std::vector<int> expected = {1, 2, 1};
    check(numIslands2(1, 3, positions) == expected,
          "bridge cell merges two separate islands into one -> [1,2,1]");
  }

  {
    // Duplicate position: re-adding an already-land cell must not change
    // the island count or create a phantom new island.
    std::vector<std::vector<int>> positions = {{0, 0}, {0, 0}, {0, 1}};
    std::vector<int> expected = {1, 1, 1};
    check(numIslands2(2, 2, positions) == expected,
          "duplicate position is a no-op -> [1,1,1]");
  }

  {
    // A cell with 3 land neighbors already all connected to each other
    // (a U-shape) must merge exactly once, not triple-decrement.
    // Grid layout (r,c), 2 rows x 3 cols:
    //   (0,0) (0,1) (0,2)
    //   (1,0)  X    (1,2)
    // Add (0,0),(0,1),(0,2),(1,0),(1,2) forming a U, all one island already
    // via the top row; then add (1,1) which touches (0,1),(1,0),(1,2) --
    // all three of which are already the SAME component.
    std::vector<std::vector<int>> positions = {
        {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 2}, {1, 1}};
    std::vector<int> expected = {1, 1, 1, 1, 1, 1};
    check(numIslands2(2, 3, positions) == expected,
          "closing a U-shape touches 3 already-same-component neighbors -> stays 1 island");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
