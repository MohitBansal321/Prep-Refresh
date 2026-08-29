// ============================================================================
// LeetCode 1168 — Optimize Water Distribution in a Village
// https://leetcode.com/problems/optimize-water-distribution-in-a-village/
// ============================================================================
//
// PROBLEM
// -------
// n houses (1..n). For house i, wells[i-1] is the cost to build a well
// DIRECTLY at that house. pipes[i] = [house1, house2, cost] is the cost to
// lay a pipe connecting two houses. Every house must end up with water,
// either from its own well or via a chain of pipes to a house that has one.
// Return the minimum total cost.
//
// APPROACH -- the VIRTUAL NODE trick: two cost types become one graph
// -----------------------------------------------------------------
// The well costs and pipe costs are two structurally different kinds of
// cost -- one is "connect this house to water" (a well), the other is
// "connect two houses to each other" (a pipe). The trick that turns this
// into a single, ordinary MST problem: invent a VIRTUAL node 0 representing
// "the water source," and treat "build a well at house i" as an ORDINARY
// EDGE from the virtual node 0 to house i, with weight wells[i-1]. Now
// every cost in the problem -- wells included -- is just an edge weight in
// one graph with n+1 nodes (0 through n), and the answer is simply that
// graph's minimum spanning tree.
//
// WHY THIS IS VALID: a spanning tree connecting node 0 to every house is
// exactly a valid water distribution -- node 0 in the tree represents "has
// water," and every house reachable from node 0 in the tree has water via
// some chain of edges, each of which is either a real pipe or the one
// well-edge that actually got "built" (kept in the MST). A house can have
// at most one well built for it in an optimal solution, because a second
// well-edge to an already-watered house would be a pure cycle -- exactly
// what Kruskal's cycle check already rules out for free.
//
// Time:  O(E log E) where E = n (wells) + pipes.size().
// Space: O(V + E).
// ============================================================================

#include <algorithm>
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

int minCostToSupplyWater(int n, const std::vector<int>& wells,
                          const std::vector<std::vector<int>>& pipes) {
  std::vector<Edge> edges;
  edges.reserve(wells.size() + pipes.size());

  // Every well becomes an edge from the VIRTUAL node 0 to house i.
  for (int i = 0; i < n; ++i) {
    edges.push_back({0, i + 1, wells[i]});
  }
  for (const auto& p : pipes) {
    edges.push_back({p[0], p[1], p[2]});
  }

  std::sort(edges.begin(), edges.end(),
            [](const Edge& a, const Edge& b) { return a.weight < b.weight; });

  // n+1 nodes: node 0 (the virtual water source) plus houses 1..n.
  DisjointSet dsu(n + 1);
  int totalCost = 0;
  int edgesUsed = 0;
  for (const Edge& e : edges) {
    if (dsu.unionSets(e.u, e.v)) {
      totalCost += e.weight;
      ++edgesUsed;
      if (edgesUsed == n) break;  // n edges span n+1 nodes (0 plus n houses)
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
    // n=3, wells=[1,2,2], pipes=[[1,2,1],[2,3,1]].
    std::vector<int> wells = {1, 2, 2};
    std::vector<std::vector<int>> pipes = {{1, 2, 1}, {2, 3, 1}};
    check(minCostToSupplyWater(3, wells, pipes) == 3,
          "classic example -> 3 (well at house 1, pipes to 2 and 3)");
  }

  {
    // All wells are cheaper than any pipe -- every house gets its own
    // well, no pipes are used at all.
    std::vector<int> wells = {1, 1, 1};
    std::vector<std::vector<int>> pipes = {{1, 2, 100}, {2, 3, 100}};
    check(minCostToSupplyWater(3, wells, pipes) == 3,
          "wells cheaper than any pipe -> every house gets its own well");
  }

  {
    // A single house: always just its own well, no pipes possible or
    // needed.
    std::vector<int> wells = {5};
    std::vector<std::vector<int>> pipes = {};
    check(minCostToSupplyWater(1, wells, pipes) == 5,
          "single house -> cost is just that house's well");
  }

  {
    // A cheap pipe network with one expensive well: only ONE well is
    // needed (as the "entry point" for water), and every other house
    // connects via cheap pipes instead of also building a well.
    std::vector<int> wells = {100, 100, 100};
    std::vector<std::vector<int>> pipes = {{1, 2, 1}, {2, 3, 1}};
    check(minCostToSupplyWater(3, wells, pipes) == 102,
          "one well (100) plus two cheap pipes (1+1) beats three wells (300)");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
