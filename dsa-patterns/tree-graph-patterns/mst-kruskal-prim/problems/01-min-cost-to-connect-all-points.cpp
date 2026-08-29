// ============================================================================
// LeetCode 1584 — Min Cost to Connect All Points
// https://leetcode.com/problems/min-cost-to-connect-all-points/
// ============================================================================
//
// PROBLEM
// -------
// Given n points on a 2D plane, the cost to connect any two points i and j
// is the MANHATTAN distance between them: |xi-xj| + |yi-yj|. Every pair of
// points is implicitly connected (a complete graph). Return the minimum
// total cost to connect all points -- a minimum spanning tree.
//
// APPROACH -- the canonical MST shape, on an IMPLICIT complete graph
// -----------------------------------------------------------------
// The graph here is never handed to you as an edge list -- it is implicit:
// every pair of points is an edge, weighted by their Manhattan distance.
// With n points there are n*(n-1)/2 implicit edges, which for LeetCode's
// constraints (n <= 1000) means up to ~500,000 edges -- entirely
// manageable for Kruskal's O(E log E) sort. This file builds the edge list
// explicitly and reuses the exact same Kruskal's logic as code.cpp.
//
// Time:  O(n^2 log n) -- generating all n^2 pairs, then sorting them.
// Space: O(n^2) for the edge list.
// ============================================================================

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct Edge {
  int u, v, weight;
};

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

int minCostConnectPoints(const std::vector<std::vector<int>>& points) {
  int n = static_cast<int>(points.size());
  std::vector<Edge> edges;
  edges.reserve(static_cast<size_t>(n) * (n - 1) / 2);

  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      int dist = std::abs(points[i][0] - points[j][0]) +
                 std::abs(points[i][1] - points[j][1]);
      edges.push_back({i, j, dist});
    }
  }

  std::sort(edges.begin(), edges.end(),
            [](const Edge& a, const Edge& b) { return a.weight < b.weight; });

  DisjointSet dsu(n);
  int totalCost = 0;
  int edgesUsed = 0;
  for (const Edge& e : edges) {
    if (dsu.unionSets(e.u, e.v)) {
      totalCost += e.weight;
      ++edgesUsed;
      if (edgesUsed == n - 1) break;
    }
  }
  return totalCost;
}

// ============================================================================
// main() -- printed, verifiable output.
// ============================================================================

int g_pass = 0;
int g_fail = 0;

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
    // points = [[0,0],[2,2],[3,10],[5,2],[7,0]]
    std::vector<std::vector<int>> points = {{0, 0}, {2, 2}, {3, 10}, {5, 2}, {7, 0}};
    check(minCostConnectPoints(points) == 20, "5-point classic example -> 20");
  }

  {
    // Two points: the only possible MST is the single edge between them.
    std::vector<std::vector<int>> points = {{0, 0}, {3, 4}};
    check(minCostConnectPoints(points) == 7,
          "two points -> Manhattan distance between them (3+4=7)");
  }

  {
    // Single point: no edges needed, cost is trivially 0.
    std::vector<std::vector<int>> points = {{5, 5}};
    check(minCostConnectPoints(points) == 0, "single point -> 0, nothing to connect");
  }

  {
    // Collinear points: the MST must be the chain connecting them in
    // order, since any "skip ahead" edge costs strictly more than the sum
    // of the shorter segments it would replace.
    std::vector<std::vector<int>> points = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};
    check(minCostConnectPoints(points) == 3,
          "collinear points -> chain of unit-distance edges, total 3");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
