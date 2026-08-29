// ============================================================================
// Minimum Spanning Tree — Kruskal's and Prim's Algorithms
// ============================================================================
//
// Given a connected, undirected, weighted graph, find a spanning tree (a
// subset of edges connecting every node, with no cycles) whose total edge
// weight is as small as possible. This is NOT a shortest-path question --
// an MST minimizes the sum of edges used to connect everything, not the
// distance between any particular pair of nodes.
//
// Two independent, structurally different algorithms both solve this
// correctly, and both are shown here to make the contrast concrete:
//   - Kruskal's: sort ALL edges globally, add each one unless it would
//     create a cycle (checked via Union-Find).
//   - Prim's: grow ONE tree outward from a single start node, always adding
//     the cheapest edge leaving the tree so far (checked via a min-heap).
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

struct Edge {
  int u;
  int v;
  int weight;
};

// ----------------------------------------------------------------------------
// A minimal Union-Find (Disjoint Set), just enough for Kruskal's cycle
// check. See ../union-find/README.md for the full explanation of path
// compression and union by rank -- this is a stripped-down copy so this
// module's code.cpp stays standalone.
// ----------------------------------------------------------------------------
class DisjointSet {
 public:
  explicit DisjointSet(int n) : parent_(n), rank_(n, 0) {
    std::iota(parent_.begin(), parent_.end(), 0);
  }

  int find(int x) {
    if (parent_[x] != x) parent_[x] = find(parent_[x]);  // path compression
    return parent_[x];
  }

  // Returns true if x and y were in DIFFERENT sets (a real merge happened).
  // Returns false if they were ALREADY connected -- adding this edge would
  // create a cycle, which is exactly the check Kruskal's needs.
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

// ----------------------------------------------------------------------------
// kruskalMST — sort every edge by weight once, then greedily add each edge
// unless both endpoints are already connected (which would form a cycle).
//
// WHY SORTING GLOBALLY WORKS: the cheapest edge in the ENTIRE graph can
// always safely be added to some MST -- if it created a cycle in every MST,
// removing any other edge from that cycle and adding this cheaper one would
// produce a strictly lighter spanning tree, contradicting that we started
// with a minimum one. The same exchange argument applies inductively to the
// next-cheapest edge that does not close a cycle with what has been chosen
// so far, and so on -- so simply walking edges cheapest-to-priciest and
// skipping cycle-forming ones is provably optimal.
//
// Returns {totalWeight, edgesUsed}; edgesUsed.size() < n - 1 means the graph
// was disconnected and no spanning tree exists.
// ----------------------------------------------------------------------------
struct MSTResult {
  long long totalWeight;
  std::vector<Edge> edgesUsed;
};

MSTResult kruskalMST(int n, std::vector<Edge> edges) {
  std::sort(edges.begin(), edges.end(),
            [](const Edge& a, const Edge& b) { return a.weight < b.weight; });

  DisjointSet dsu(n);
  MSTResult result{0, {}};

  for (const Edge& e : edges) {
    if (dsu.unionSets(e.u, e.v)) {  // true only if this edge connects two
                                     // previously-separate components
      result.totalWeight += e.weight;
      result.edgesUsed.push_back(e);
      if (static_cast<int>(result.edgesUsed.size()) == n - 1) break;  // tree complete
    }
  }

  return result;
}

// ----------------------------------------------------------------------------
// primMST — grow one tree outward from node 0. At every step, add the
// cheapest edge connecting a node ALREADY in the tree to a node NOT yet in
// the tree, using a min-heap to find that cheapest edge in O(log E).
//
// WHY THIS ALSO WORKS: for any partial tree grown so far, the cheapest edge
// crossing the boundary between "in the tree" and "not in the tree" can
// always safely be added -- an argument structurally identical to
// Kruskal's, applied to the current frontier instead of to the whole edge
// list at once. Kruskal's chooses cheap edges GLOBALLY, in any order;
// Prim's chooses cheap edges LOCALLY, always adjacent to what has already
// been grown.
// ----------------------------------------------------------------------------
MSTResult primMST(int n, const std::vector<std::vector<std::pair<int, int>>>& adj) {
  std::vector<bool> inTree(n, false);
  // Min-heap of {weight, from, to}. std::priority_queue is a MAX-heap by
  // default, so std::greater<> is supplied to invert it into a min-heap.
  using HeapEntry = std::tuple<int, int, int>;
  std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<>> pq;

  MSTResult result{0, {}};
  inTree[0] = true;
  for (const std::pair<int, int>& edge : adj[0]) {
    pq.push(std::make_tuple(edge.second, 0, edge.first));
  }

  while (!pq.empty() && static_cast<int>(result.edgesUsed.size()) < n - 1) {
    const HeapEntry top = pq.top();
    const int weight = std::get<0>(top);
    const int from = std::get<1>(top);
    const int to = std::get<2>(top);
    pq.pop();
    if (inTree[to]) continue;  // stale entry: `to` was already added via a
                                // cheaper edge found later in the heap

    inTree[to] = true;
    result.totalWeight += weight;
    result.edgesUsed.push_back(Edge{from, to, weight});

    for (const std::pair<int, int>& edge : adj[to]) {
      if (!inTree[edge.first]) {
        pq.push(std::make_tuple(edge.second, to, edge.first));
      }
    }
  }

  return result;
}

// ============================================================================
// main() -- demonstrates both algorithms with printed, verifiable output.
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
  // A 5-node graph (classic textbook shape):
  //   0-1 (2), 0-3 (6), 1-2 (3), 1-3 (8), 1-4 (5), 2-4 (7), 3-4 (9)
  std::vector<Edge> edges = {
      {0, 1, 2}, {0, 3, 6}, {1, 2, 3}, {1, 3, 8}, {1, 4, 5}, {2, 4, 7}, {3, 4, 9},
  };

  {
    MSTResult result = kruskalMST(5, edges);
    check(result.totalWeight == 16,
          "Kruskal: 5-node graph -> MST total weight 16");
    check(result.edgesUsed.size() == 4, "Kruskal: MST uses exactly n-1 = 4 edges");
  }

  {
    std::vector<std::vector<std::pair<int, int>>> adj(5);
    for (const Edge& e : edges) {
      adj[e.u].push_back({e.v, e.weight});
      adj[e.v].push_back({e.u, e.weight});
    }
    MSTResult result = primMST(5, adj);
    check(result.totalWeight == 16,
          "Prim: same 5-node graph -> identical MST total weight 16");
    check(result.edgesUsed.size() == 4, "Prim: MST also uses exactly n-1 = 4 edges");
  }

  {
    // A graph with a cheaper alternative that must be preferred:
    // three nodes, a triangle 0-1 (1), 1-2 (1), 0-2 (10) -- the MST must
    // use the two cheap edges and skip the expensive one entirely.
    std::vector<Edge> triangle = {{0, 1, 1}, {1, 2, 1}, {0, 2, 10}};
    MSTResult result = kruskalMST(3, triangle);
    check(result.totalWeight == 2,
          "triangle graph: MST picks the two cheap edges (1+1), skips the "
          "expensive one (10)");
  }

  {
    // A disconnected graph: no spanning tree exists. edgesUsed.size() must
    // be less than n-1, not a crash or a wrong total.
    std::vector<Edge> disconnected = {{0, 1, 5}};  // node 2 is isolated
    MSTResult result = kruskalMST(3, disconnected);
    check(result.edgesUsed.size() == 1,
          "disconnected graph: only 1 edge used, not the full n-1 = 2 -- no "
          "spanning tree exists");
  }

  {
    // A graph with a redundant, more expensive parallel path: confirms
    // Kruskal's correctly skips an edge that would close a cycle even
    // when it looks locally tempting (a 4-node cycle where one edge must
    // be dropped).
    std::vector<Edge> cycle = {
        {0, 1, 1}, {1, 2, 1}, {2, 3, 1}, {3, 0, 1}};  // a 4-cycle, all weight 1
    MSTResult result = kruskalMST(4, cycle);
    check(result.totalWeight == 3,
          "4-node cycle, all edges weight 1 -> MST uses exactly 3 of the 4 "
          "edges (any one dropped), total weight 3");
  }

  {
    // Single node, no edges: an MST of one node is trivially itself, with
    // zero edges and zero weight.
    MSTResult result = kruskalMST(1, {});
    check(result.totalWeight == 0 && result.edgesUsed.empty(),
          "single node, no edges -> trivial MST, weight 0");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
