// ============================================================================
// LeetCode 1489 — Find Critical and Pseudo-Critical Edges in Minimum
// Spanning Tree
// https://leetcode.com/problems/find-critical-and-pseudo-critical-edges-in-minimum-spanning-tree/
// ============================================================================
//
// PROBLEM
// -------
// Given a weighted undirected graph, classify every edge as:
//   CRITICAL     -- removing it strictly increases the MST weight (or
//                    disconnects the graph). It is in EVERY minimum
//                    spanning tree.
//   PSEUDO-CRITICAL -- it is not critical, but SOME minimum spanning tree
//                    includes it (there exists an MST using this edge that
//                    still achieves the minimum weight).
//   Neither       -- no minimum spanning tree ever uses it at all.
// Return the lists of critical and pseudo-critical edge indices.
//
// APPROACH -- interrogate the MST algorithm by re-running it, not by
// inspecting a single run
// -----------------------------------------------------------------------
// A single Kruskal's run finds ONE minimum spanning tree and its weight,
// but classifying edges requires asking two different counterfactual
// questions per edge, answered by re-running Kruskal's with that edge
// artificially forced out or forced in:
//
//   CRITICAL test: run Kruskal's while EXCLUDING this edge entirely. If the
//   resulting MST is impossible (disconnected) or strictly heavier than the
//   true minimum, this edge was load-bearing -- every MST needs it.
//
//   PSEUDO-CRITICAL test: run Kruskal's while FORCING this edge to be
//   included first (pre-union its endpoints before considering any other
//   edge). If the resulting total weight still equals the true minimum,
//   this edge CAN be part of a minimum spanning tree, even if it is not
//   required by every one.
//
// This file computes the baseline minimum weight once, then re-runs a full
// Kruskal's pass per edge per test -- O(E) reruns, each O(E log E) -- which
// is the standard, accepted complexity for this problem.
//
// Time:  O(E^2 log E) -- E reruns of the O(E log E) algorithm.
// Space: O(V + E) per run.
// ============================================================================

#include <algorithm>
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
    if (parent_[x] != x) parent_[x] = find(parent_[x]);
    return parent_[x];
  }
  bool unionSets(int x, int y) {
    int rootX = find(x), rootY = find(y);
    if (rootX == rootY) return false;
    if (rank_[rootX] < rank_[rootY]) std::swap(rootX, rootY);
    parent_[rootY] = rootX;
    if (rank_[rootX] == rank_[rootY]) ++rank_[rootX];
    return true;
  }

 private:
  std::vector<int> parent_;
  std::vector<int> rank_;
};

// Runs Kruskal's over `order` (a permutation of edge indices already sorted
// by weight), optionally skipping `exclude` and optionally pre-including
// `forceInclude` before considering anything else. Returns the total
// weight, or -1 if the result is not a full spanning tree.
int kruskalWeight(int n, const std::vector<std::vector<int>>& edges,
                   const std::vector<int>& order, int exclude, int forceInclude) {
  DisjointSet dsu(n);
  int totalWeight = 0;
  int edgesUsed = 0;

  if (forceInclude != -1) {
    const auto& e = edges[forceInclude];
    dsu.unionSets(e[0], e[1]);
    totalWeight += e[2];
    ++edgesUsed;
  }

  for (int idx : order) {
    if (idx == exclude || idx == forceInclude) continue;
    const auto& e = edges[idx];
    if (dsu.unionSets(e[0], e[1])) {
      totalWeight += e[2];
      ++edgesUsed;
    }
  }

  return edgesUsed == n - 1 ? totalWeight : -1;
}

std::vector<std::vector<int>> findCriticalAndPseudoCriticalEdges(
    int n, const std::vector<std::vector<int>>& edges) {
  int m = static_cast<int>(edges.size());
  std::vector<int> order(m);
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(), [&](int a, int b) {
    return edges[a][2] < edges[b][2];
  });

  int baseline = kruskalWeight(n, edges, order, -1, -1);

  std::vector<int> critical, pseudo;
  for (int i = 0; i < m; ++i) {
    int withoutI = kruskalWeight(n, edges, order, i, -1);
    if (withoutI == -1 || withoutI > baseline) {
      critical.push_back(i);
      continue;  // critical edges are never also pseudo-critical
    }
    int forcedI = kruskalWeight(n, edges, order, -1, i);
    if (forcedI == baseline) {
      pseudo.push_back(i);
    }
  }

  return {critical, pseudo};
}

// ============================================================================
// main() -- printed, verifiable output.
// ============================================================================

int g_pass = 0;
int g_fail = 0;

bool sameSet(std::vector<int> a, std::vector<int> b) {
  std::sort(a.begin(), a.end());
  std::sort(b.begin(), b.end());
  return a == b;
}

void check(bool condition, const std::string& label) {
  if (condition) {
    std::cout << "[PASS] " << label << "\n";
    ++g_pass;
  } else {
    std::cout << "[FAIL] " << label << "\n";
    ++g_fail;
  }
}

int main() {
  {
    // n=5, edges=[[0,1,1],[1,2,1],[2,3,2],[0,3,2],[0,4,3],[3,4,3],[1,4,6]]
    // (LeetCode's own worked example). Expected: critical = [0,1], pseudo =
    // [2,3,4,5]. Edge 6 (1-4, weight 6) is neither.
    std::vector<std::vector<int>> edges = {
        {0, 1, 1}, {1, 2, 1}, {2, 3, 2}, {0, 3, 2}, {0, 4, 3}, {3, 4, 3}, {1, 4, 6}};
    auto result = findCriticalAndPseudoCriticalEdges(5, edges);
    check(sameSet(result[0], {0, 1}), "critical edges -> {0, 1}");
    check(sameSet(result[1], {2, 3, 4, 5}), "pseudo-critical edges -> {2, 3, 4, 5}");
  }

  {
    // A triangle where all three edges have DISTINCT weights: the two
    // cheapest are both critical (the unique MST needs exactly them), and
    // the most expensive edge is neither critical nor pseudo-critical
    // (no MST ever includes it, since it is strictly worse than the
    // alternative).
    std::vector<std::vector<int>> edges = {{0, 1, 1}, {1, 2, 2}, {0, 2, 3}};
    auto result = findCriticalAndPseudoCriticalEdges(3, edges);
    check(sameSet(result[0], {0, 1}),
          "distinct-weight triangle -> both cheap edges are critical");
    check(result[1].empty(),
          "distinct-weight triangle -> no pseudo-critical edges (unique MST)");
  }

  {
    // A triangle where all three edges have the SAME weight: none is
    // critical (any two of the three form a valid MST, so no single edge
    // is required by every MST), but all three are pseudo-critical (each
    // one appears in at least one valid MST).
    std::vector<std::vector<int>> edges = {{0, 1, 1}, {1, 2, 1}, {0, 2, 1}};
    auto result = findCriticalAndPseudoCriticalEdges(3, edges);
    check(result[0].empty(),
          "equal-weight triangle -> no edge is critical (ties allow swapping)");
    check(sameSet(result[1], {0, 1, 2}),
          "equal-weight triangle -> all three edges are pseudo-critical");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
