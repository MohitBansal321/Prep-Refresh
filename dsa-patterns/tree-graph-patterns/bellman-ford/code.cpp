// ============================================================================
// Bellman-Ford (Single-Source Shortest Path with Negative Weights)
// ============================================================================
//
// Finds the shortest distance from a single source to every other node in a
// graph whose edges may carry NEGATIVE weights (the case Dijkstra cannot
// handle), and detects whether the graph contains a negative-weight cycle
// reachable from the source (in which case "shortest path" is undefined —
// you could loop the cycle forever, subtracting cost each time).
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <limits>
#include <string>
#include <vector>

struct Edge {
  int from;
  int to;
  int weight;
};

struct BellmanFordResult {
  std::vector<long long> dist;
  bool hasNegativeCycle;
};

// ----------------------------------------------------------------------------
// bellmanFord — relax every edge, V-1 times, then run one extra pass to
// detect a negative cycle.
//
// WHY V-1 PASSES: the shortest path between any two nodes in a graph with V
// nodes uses at most V-1 edges (a simple path visits each node at most once,
// so it has at most V-1 edges between V nodes). Each full pass over all E
// edges is guaranteed to correctly finalize the shortest path using AT MOST
// one additional edge compared to the previous pass's best-known distances.
// After V-1 passes, every shortest path — no matter which edges it uses —
// has been fully relaxed into dist[], provided no negative cycle exists.
//
// WHY ONE MORE PASS DETECTS A NEGATIVE CYCLE: if a distance can still improve
// on pass V, then some path used MORE than V-1 edges to achieve that
// improvement. A simple path cannot exceed V-1 edges, so the only way to use
// more edges is to loop back through a cycle — and the only way looping
// through a cycle helps is if that cycle's total weight is negative.
// ----------------------------------------------------------------------------
BellmanFordResult bellmanFord(int n, int src, const std::vector<Edge>& edges) {
  const long long kInf = std::numeric_limits<long long>::max();
  std::vector<long long> dist(n, kInf);
  dist[src] = 0;

  for (int pass = 0; pass < n - 1; ++pass) {
    bool changed = false;
    for (const Edge& e : edges) {
      if (dist[e.from] == kInf) continue;  // source side not yet reachable
      if (dist[e.from] + e.weight < dist[e.to]) {
        dist[e.to] = dist[e.from] + e.weight;
        changed = true;
      }
    }
    // Early exit: if a full pass relaxes nothing, no further pass can
    // relax anything either — dist[] has already converged.
    if (!changed) break;
  }

  // The V-th pass: if anything STILL relaxes, a negative cycle is reachable
  // from src and "shortest path" is not well-defined for affected nodes.
  bool hasNegativeCycle = false;
  for (const Edge& e : edges) {
    if (dist[e.from] == kInf) continue;
    if (dist[e.from] + e.weight < dist[e.to]) {
      hasNegativeCycle = true;
      break;
    }
  }

  return {dist, hasNegativeCycle};
}

// ============================================================================
// main() -- demonstrates bellmanFord with printed, verifiable output.
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
  const long long kInf = std::numeric_limits<long long>::max();

  {
    // A graph with one negative edge but NO negative cycle:
    //   0 --6--> 1
    //   0 --7--> 2
    //   1 --5--> 2
    //   1 -4-->  3   (via edge 1->3 weight 4, wait -- see edges below)
    // Concrete edges (classic CLRS-style example):
    //   0->1 (6), 0->2 (7), 1->2 (5), 1->3 (-4), 1->4 (8),
    //   2->3 (-3), 2->4 (9), 3->1 (7), 4->0 (2), 4->3 (7)
    std::vector<Edge> edges = {
        {0, 1, 6}, {0, 2, 7}, {1, 2, 5}, {1, 3, -4}, {1, 4, 8},
        {2, 3, -3}, {2, 4, 9}, {3, 1, 7}, {4, 0, 2}, {4, 3, 7},
    };
    BellmanFordResult result = bellmanFord(5, 0, edges);
    check(!result.hasNegativeCycle, "classic 5-node graph -> no negative cycle detected");
    check(result.dist[0] == 0, "dist to source is 0");
    check(result.dist[1] == 6, "dist[1] == 6 (direct edge, nothing shorter)");
    check(result.dist[2] == 7, "dist[2] == 7 (direct edge, not 0->1->2 = 11)");
    check(result.dist[3] == 2, "dist[3] == 2 (via 0->1->3 = 6 + -4 = 2)");
    check(result.dist[4] == 14, "dist[4] == 14 (via 0->1->4 = 6 + 8 = 14)");
  }

  {
    // A graph WITH a negative cycle reachable from the source:
    //   0 -> 1 (weight 1)
    //   1 -> 2 (weight -1)
    //   2 -> 1 (weight -1)   <- cycle 1->2->1 has total weight -2
    std::vector<Edge> edges = {
        {0, 1, 1},
        {1, 2, -1},
        {2, 1, -1},
    };
    BellmanFordResult result = bellmanFord(3, 0, edges);
    check(result.hasNegativeCycle,
          "reachable negative cycle (1->2->1, weight -2) -> detected");
  }

  {
    // A negative cycle that exists in the graph but is NOT reachable from
    // the chosen source -- must NOT be reported, because it can never
    // affect any path actually taken from src.
    //   0 -> 1 (weight 3)      (src=0 reaches only node 1)
    //   2 -> 3 (weight -1)
    //   3 -> 2 (weight -1)     <- negative cycle, but unreachable from 0
    std::vector<Edge> edges = {
        {0, 1, 3},
        {2, 3, -1},
        {3, 2, -1},
    };
    BellmanFordResult result = bellmanFord(4, 0, edges);
    check(!result.hasNegativeCycle,
          "negative cycle exists but is unreachable from src -> not reported");
    check(result.dist[1] == 3, "dist[1] == 3 (the only reachable node)");
    check(result.dist[2] == kInf, "dist[2] stays INF (unreachable from src)");
  }

  {
    // Disconnected node: must stay at infinity, not crash or read garbage.
    std::vector<Edge> edges = {{0, 1, 5}};
    BellmanFordResult result = bellmanFord(3, 0, edges);
    check(result.dist[2] == kInf, "disconnected node 2 stays at infinity");
    check(!result.hasNegativeCycle, "no edges to a cycle -> no false positive");
  }

  {
    // Single-node graph, no edges at all.
    std::vector<Edge> edges = {};
    BellmanFordResult result = bellmanFord(1, 0, edges);
    check(result.dist[0] == 0, "single node graph: source distance is 0");
    check(!result.hasNegativeCycle, "no edges -> trivially no negative cycle");
  }

  {
    // Early-exit check: a graph that converges in fewer than V-1 passes
    // must still produce correct distances (the `changed` flag must not
    // cause an early return with a wrong, half-relaxed distance array).
    //   0 -> 1 (1) -> 2 (1) -> 3 (1) -> 4 (1)   (a simple chain)
    std::vector<Edge> edges = {
        {0, 1, 1}, {1, 2, 1}, {2, 3, 1}, {3, 4, 1},
    };
    BellmanFordResult result = bellmanFord(5, 0, edges);
    check(result.dist[4] == 4, "chain of 4 edges converges early, still correct");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
