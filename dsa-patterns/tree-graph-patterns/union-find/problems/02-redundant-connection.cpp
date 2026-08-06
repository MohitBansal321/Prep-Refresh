// ============================================================================
// LeetCode 684 — Redundant Connection
// ============================================================================
//
// PROBLEM
// -------
// A graph started as a tree with n nodes (labeled 1..n) and n-1 edges, and
// then exactly ONE extra edge was added, creating exactly one cycle. Given
// the edges in the order they were added (as [u, v] pairs), return the edge
// that, if removed, restores the graph to a tree. If multiple edges could be
// removed, return the one that occurs LAST in the input.
//
// Example: edges = [[1,2],[1,3],[2,3]] -> [2,3]
//          (1-2 and 1-3 already connect all three nodes into a tree;
//           2-3 is redundant — it closes a cycle 1-2-3-1.)
//
// APPROACH — Union Find (cycle detection as edges arrive one at a time)
// ------------------------------------------------------------------------
// This is the single cleanest illustration of Union Find's core party
// trick: process edges IN ORDER, and for each edge (u, v), ask "are u and v
// ALREADY connected before this edge is added?"
//   - If NOT connected: this edge is a genuine tree edge. Union them.
//   - If ALREADY connected: u and v can already reach each other through
//     edges processed so far, so this edge closes a cycle. Since we process
//     edges in input order and the problem guarantees exactly one redundant
//     edge, the FIRST edge for which unionSets() returns false (meaning "no
//     merge happened, they were already in the same group") is the answer —
//     and because we process left-to-right, it is automatically the LAST
//     such edge in the input order that the problem asks for.
//
// Why this beats DFS/BFS-per-edge: re-running a traversal from u to check
// "can u already reach v?" after every single edge is O(V+E) per edge,
// O((V+E) * E) overall. Union Find answers "already connected?" in near-O(1)
// amortized time per edge via find(), because it incrementally MAINTAINS
// the connectivity information instead of recomputing it from scratch.
//
// COMPLEXITY
// ----------
// Time:  O(E * alpha(V)) — one union/find pair per edge, near-O(1) each.
// Space: O(V) — the DisjointSet's internal arrays.
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

  // Returns false if x and y were ALREADY connected (this edge is
  // redundant / closes a cycle); true if a real merge happened.
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

std::vector<int> findRedundantConnection(const std::vector<std::vector<int>>& edges) {
  int n = static_cast<int>(edges.size());  // n nodes labeled 1..n, n edges given
  DisjointSet dsu(n + 1);                  // index 0 unused; nodes are 1-indexed

  for (const auto& edge : edges) {
    int u = edge[0];
    int v = edge[1];
    if (!dsu.unionSets(u, v)) {
      // u and v were already connected before this edge -> redundant edge.
      // Because we scan in input order, this is guaranteed to be the LAST
      // valid answer if multiple edges could be removed, matching the spec.
      return {u, v};
    }
  }

  return {};  // Problem guarantees a redundant edge exists; unreachable.
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
    std::vector<std::vector<int>> edges = {{1, 2}, {1, 3}, {2, 3}};
    std::vector<int> expected = {2, 3};
    check(findRedundantConnection(edges) == expected,
          "[[1,2],[1,3],[2,3]] -> [2,3]");
  }

  {
    std::vector<std::vector<int>> edges = {{1, 2}, {2, 3}, {3, 4}, {1, 4}, {1, 5}};
    std::vector<int> expected = {1, 4};
    check(findRedundantConnection(edges) == expected,
          "[[1,2],[2,3],[3,4],[1,4],[1,5]] -> [1,4]");
  }

  {
    // Simplest possible cycle: two nodes, two edges between them.
    std::vector<std::vector<int>> edges = {{1, 2}, {2, 1}};
    std::vector<int> expected = {2, 1};
    check(findRedundantConnection(edges) == expected, "[[1,2],[2,1]] -> [2,1]");
  }

  {
    // Star topology plus one extra closing edge among leaves.
    std::vector<std::vector<int>> edges = {{1, 2}, {1, 3}, {1, 4}, {1, 5}, {3, 4}};
    std::vector<int> expected = {3, 4};
    check(findRedundantConnection(edges) == expected,
          "star graph with one closing leaf edge -> [3,4]");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
