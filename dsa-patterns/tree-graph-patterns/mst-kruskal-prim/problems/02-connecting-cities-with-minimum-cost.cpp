// ============================================================================
// LeetCode 1135 — Connecting Cities With Minimum Cost
// https://leetcode.com/problems/connecting-cities-with-minimum-cost/
// ============================================================================
//
// PROBLEM
// -------
// n cities (1..n), connections[i] = [city1, city2, cost] -- a possible
// bidirectional road and its cost. Unlike LeetCode 1584, the graph here is
// NOT complete: only the given connections exist. Return the minimum total
// cost to connect all cities, or -1 if it is IMPOSSIBLE (the given
// connections do not form a connected graph no matter which are chosen).
//
// APPROACH -- Kruskal's, with an explicit "is it even possible" check
// -----------------------------------------------------------------
// This is the same Kruskal's loop as LeetCode 1584, but because the graph
// is sparse and given directly (not implicit and complete), the algorithm
// must also detect and report the case where fewer than n-1 edges can ever
// be kept -- meaning the input connections simply do not span all n
// cities, regardless of which subset is chosen.
//
// Time:  O(E log E) -- dominated by sorting the given connections.
// Space: O(V + E).
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

int minimumCost(int n, std::vector<std::vector<int>> connections) {
  std::sort(connections.begin(), connections.end(),
            [](const std::vector<int>& a, const std::vector<int>& b) {
              return a[2] < b[2];
            });

  // Cities are 1-indexed in the problem statement; DisjointSet is
  // 0-indexed, so index n+1 slots and simply leave slot 0 unused.
  DisjointSet dsu(n + 1);
  int totalCost = 0;
  int edgesUsed = 0;

  for (const auto& c : connections) {
    if (dsu.unionSets(c[0], c[1])) {
      totalCost += c[2];
      ++edgesUsed;
      if (edgesUsed == n - 1) break;
    }
  }

  return edgesUsed == n - 1 ? totalCost : -1;
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
    // n=3, connections=[[1,2,5],[1,3,6],[2,3,1]] -- the cheap edge 2-3 (1)
    // plus either 1-2 (5) or 1-3 (6); Kruskal's picks the cheaper of the
    // two remaining options.
    std::vector<std::vector<int>> connections = {{1, 2, 5}, {1, 3, 6}, {2, 3, 1}};
    check(minimumCost(3, connections) == 6,
          "3 cities -> MST picks edges 2-3 (1) and 1-2 (5), total 6");
  }

  {
    // n=4, connections=[[1,2,3],[3,4,4]] -- two disjoint pairs, no edge
    // connects {1,2} to {3,4} at all. Must return -1, not a partial total.
    std::vector<std::vector<int>> connections = {{1, 2, 3}, {3, 4, 4}};
    check(minimumCost(4, connections) == -1,
          "connections leave two components disconnected -> -1");
  }

  {
    // Single city, no connections needed at all: n-1=0 edges required,
    // trivially satisfied, cost 0.
    std::vector<std::vector<int>> connections = {};
    check(minimumCost(1, connections) == 0, "single city -> 0, nothing to connect");
  }

  {
    // A cycle where one edge is strictly redundant: n=3, a triangle with
    // weights 1, 1, 100 -- the MST must use the two cheap edges and skip
    // the expensive one, exactly as in code.cpp's own triangle test.
    std::vector<std::vector<int>> connections = {{1, 2, 1}, {2, 3, 1}, {1, 3, 100}};
    check(minimumCost(3, connections) == 2,
          "triangle with one expensive redundant edge -> MST skips it, total 2");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
