// ============================================================================
// LeetCode 547 — Number of Provinces
// ============================================================================
//
// PROBLEM
// -------
// There are n cities. Some are directly connected (a "1" in the adjacency
// matrix isConnected[i][j]), and connection is transitive: if city A is
// connected to city B, and city B is connected to city C, then A, B, C are
// in the same province, even if there is no direct A-C edge. Return the
// total number of provinces (connected components).
//
// Example: isConnected = [[1,1,0],[1,1,0],[0,0,1]] -> 2 provinces
//          (cities 0 and 1 are one province; city 2 is its own province).
//
// APPROACH — Union Find (the purest "count connected components" shape)
// -----------------------------------------------------------------------
// This is the textbook Union Find problem: we are handed a full adjacency
// matrix (every direct edge available up front, not streamed one at a time),
// but the question — "how many connected components are there?" — is
// exactly what Union Find answers without ever needing to run a traversal.
//
// Initialize a DisjointSet with n singleton groups (n provinces to start).
// For every pair (i, j) with isConnected[i][j] == 1, call unionSets(i, j).
// Every SUCCESSFUL union (one that actually merges two previously separate
// groups) means two provinces became one, so the running province count
// drops by exactly one — which is exactly what componentCount() tracks
// internally. At the end, componentCount() IS the answer.
//
// Why not BFS/DFS here instead? BFS/DFS would also work fine for a single,
// one-shot query like this (no incremental edges, no repeated connectivity
// queries) — see the README's "Why Not Other Approaches" section. Union
// Find is used here specifically because this module is about Union Find,
// and because the matrix-scan + union loop is the cleanest way to see the
// "count components by counting successful unions" trick that reappears in
// harder problems (02, 03, 04 below).
//
// COMPLEXITY
// ----------
// Time:  O(n^2 * alpha(n)) — we scan the n x n matrix once; each cell that
//        is a "1" triggers a near-O(1) amortized union/find call.
// Space: O(n) — the DisjointSet's parent/rank/size arrays.
// ============================================================================

#include <iostream>
#include <numeric>
#include <string>
#include <vector>

class DisjointSet {
 public:
  explicit DisjointSet(int n) : parent_(n), rank_(n, 0), component_count_(n) {
    std::iota(parent_.begin(), parent_.end(), 0);
  }

  int find(int x) {
    if (parent_[x] != x) {
      parent_[x] = find(parent_[x]);  // path compression
    }
    return parent_[x];
  }

  void unionSets(int x, int y) {
    int root_x = find(x);
    int root_y = find(y);
    if (root_x == root_y) return;  // already in the same province

    if (rank_[root_x] < rank_[root_y]) std::swap(root_x, root_y);
    parent_[root_y] = root_x;
    if (rank_[root_x] == rank_[root_y]) ++rank_[root_x];
    --component_count_;  // two provinces just merged into one
  }

  int componentCount() const { return component_count_; }

 private:
  std::vector<int> parent_;
  std::vector<int> rank_;
  int component_count_;
};

int findCircleNum(const std::vector<std::vector<int>>& isConnected) {
  int n = static_cast<int>(isConnected.size());
  DisjointSet dsu(n);

  // Only need the upper triangle: the matrix is symmetric and the diagonal
  // (a city connected to itself) tells us nothing new.
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      if (isConnected[i][j] == 1) {
        dsu.unionSets(i, j);
      }
    }
  }

  return dsu.componentCount();
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
    std::vector<std::vector<int>> isConnected = {{1, 1, 0}, {1, 1, 0}, {0, 0, 1}};
    check(findCircleNum(isConnected) == 2, "[[1,1,0],[1,1,0],[0,0,1]] -> 2 provinces");
  }

  {
    std::vector<std::vector<int>> isConnected = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    check(findCircleNum(isConnected) == 3, "identity matrix (no connections) -> 3 provinces");
  }

  {
    std::vector<std::vector<int>> isConnected = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
    check(findCircleNum(isConnected) == 1, "fully connected 3x3 -> 1 province");
  }

  {
    // Transitive chain: 0-1, 1-2, 2-3 connected; 4 is isolated.
    std::vector<std::vector<int>> isConnected = {
        {1, 1, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 1, 0},
        {0, 0, 0, 0, 1},
    };
    check(findCircleNum(isConnected) == 2,
          "transitive chain 0-1-2-3 plus isolated 4 -> 2 provinces");
  }

  {
    std::vector<std::vector<int>> isConnected = {{1}};
    check(findCircleNum(isConnected) == 1, "single city -> 1 province");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
