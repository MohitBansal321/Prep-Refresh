// ============================================================================
// Floyd-Warshall (All-Pairs Shortest Path)
// ============================================================================
//
// Computes the shortest distance between EVERY pair of nodes in a graph
// whose edges may carry negative weights (but no negative cycle), in one
// pass over all pairs and all "intermediate node" choices -- and detects
// whether a negative cycle exists anywhere in the graph as a byproduct.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <limits>
#include <string>
#include <vector>

struct FloydWarshallResult {
  std::vector<std::vector<long long>> dist;
  bool hasNegativeCycle;
};

// ----------------------------------------------------------------------------
// floydWarshall — for every possible "intermediate" node k, ask: does
// routing through k improve the currently-known distance between i and j?
//
// dist[i][j] starts as: 0 if i == j, the direct edge weight if one exists,
// infinity otherwise. The algorithm then considers each node k, in order,
// as a candidate WAYPOINT for every pair (i, j): if going i -> k -> j is
// cheaper than the current best i -> j, update it.
//
// WHY THIS COVERS EVERY PATH: after the loop over k = 0 has finished,
// dist[i][j] is correct for any path that only uses node 0 as an
// intermediate stop. After k = 0 and k = 1 have both finished, dist[i][j]
// is correct for any path using only nodes {0, 1} as intermediate stops.
// By induction, after k has ranged over every node 0..n-1, dist[i][j] is
// correct for a path using ANY subset of nodes as intermediate stops --
// which is every possible path. The order of the three nested loops is
// not interchangeable: k MUST be the outermost loop, because dist[i][k]
// and dist[k][j] used inside the innermost check must already reflect
// every intermediate node considered SO FAR (0..k-1), not nodes considered
// later in the same pass.
// ----------------------------------------------------------------------------
FloydWarshallResult floydWarshall(
    int n, const std::vector<std::vector<int>>& edges) {
  const long long kInf = std::numeric_limits<long long>::max() / 4;
  std::vector<std::vector<long long>> dist(n, std::vector<long long>(n, kInf));

  for (int i = 0; i < n; ++i) {
    dist[i][i] = 0;  // distance from a node to itself is 0
  }
  for (const auto& e : edges) {
    // Keep the cheapest edge if the input has parallel edges between the
    // same pair of nodes.
    dist[e[0]][e[1]] = std::min(dist[e[0]][e[1]], static_cast<long long>(e[2]));
  }

  for (int k = 0; k < n; ++k) {
    for (int i = 0; i < n; ++i) {
      if (dist[i][k] == kInf) continue;  // no path to k yet -- nothing to route through
      for (int j = 0; j < n; ++j) {
        if (dist[k][j] == kInf) continue;
        if (dist[i][k] + dist[k][j] < dist[i][j]) {
          dist[i][j] = dist[i][k] + dist[k][j];
        }
      }
    }
  }

  // A negative cycle exists iff some node's distance to ITSELF has dropped
  // below zero -- the only way i -> i can cost less than 0 is by looping
  // through a cycle whose total weight is negative.
  bool hasNegativeCycle = false;
  for (int i = 0; i < n; ++i) {
    if (dist[i][i] < 0) {
      hasNegativeCycle = true;
      break;
    }
  }

  return {dist, hasNegativeCycle};
}

// ============================================================================
// main() -- demonstrates floydWarshall with printed, verifiable output.
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
  const long long kInf = std::numeric_limits<long long>::max() / 4;

  {
    // Classic 4-node example (CLRS-style), no negative cycle:
    //   0->1 (3), 0->3 (7), 1->0 (8), 1->2 (2),
    //   2->0 (5), 2->3 (1), 3->0 (2)
    std::vector<std::vector<int>> edges = {
        {0, 1, 3}, {0, 3, 7}, {1, 0, 8}, {1, 2, 2},
        {2, 0, 5}, {2, 3, 1}, {3, 0, 2},
    };
    FloydWarshallResult result = floydWarshall(4, edges);
    check(!result.hasNegativeCycle, "classic 4-node graph -> no negative cycle");
    check(result.dist[0][3] == 6, "dist[0][3] == 6 (via 0->1->2->3 = 3+2+1)");
    check(result.dist[2][0] == 3, "dist[2][0] == 3 (via 2->3->0 = 1+2, beats the direct edge of 5)");
    check(result.dist[1][3] == 3, "dist[1][3] == 3 (via 1->2->3 = 2+1)");
    check(result.dist[0][0] == 0, "distance from a node to itself is 0");
  }

  {
    // A negative edge with no negative cycle -- still correctly handled,
    // demonstrating Floyd-Warshall tolerates negative weights same as
    // Bellman-Ford.
    //   0->1 (1), 1->2 (-5), 0->2 (2)
    std::vector<std::vector<int>> edges = {{0, 1, 1}, {1, 2, -5}, {0, 2, 2}};
    FloydWarshallResult result = floydWarshall(3, edges);
    check(!result.hasNegativeCycle, "negative edge, no cycle -> not flagged");
    check(result.dist[0][2] == -4, "dist[0][2] == -4 (via 0->1->2 beats the direct edge)");
  }

  {
    // A negative cycle: 0->1 (1), 1->2 (-1), 2->1 (-1) -- the cycle 1->2->1
    // has total weight -2.
    std::vector<std::vector<int>> edges = {{0, 1, 1}, {1, 2, -1}, {2, 1, -1}};
    FloydWarshallResult result = floydWarshall(3, edges);
    check(result.hasNegativeCycle, "reachable negative cycle -> detected");
  }

  {
    // Disconnected pair: must stay at infinity, not crash or read garbage.
    std::vector<std::vector<int>> edges = {{0, 1, 5}};
    FloydWarshallResult result = floydWarshall(3, edges);
    check(result.dist[0][2] == kInf, "no path 0->2 -> stays at infinity");
    check(result.dist[2][0] == kInf, "no path 2->0 -> stays at infinity");
  }

  {
    // Single-node graph, no edges at all.
    std::vector<std::vector<int>> edges = {};
    FloydWarshallResult result = floydWarshall(1, edges);
    check(result.dist[0][0] == 0, "single node graph: distance to itself is 0");
    check(!result.hasNegativeCycle, "no edges -> trivially no negative cycle");
  }

  {
    // Parallel edges between the same pair: the cheaper one must win.
    std::vector<std::vector<int>> edges = {{0, 1, 10}, {0, 1, 3}};
    FloydWarshallResult result = floydWarshall(2, edges);
    check(result.dist[0][1] == 3, "parallel edges -> the cheaper one (3) wins");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
