// ============================================================================
// LeetCode 743 — Network Delay Time
// https://leetcode.com/problems/network-delay-time/
// ============================================================================
//
// PROBLEM
// -------
// n nodes labeled 1..n. times[i] = (u, v, w): a signal sent from u reaches v
// in w time units (directed, positive weight). Given a source node k, return
// the time it takes for a signal starting at k to reach every node, i.e. the
// MAXIMUM of the shortest distances -- or -1 if some node is unreachable.
//
// APPROACH -- textbook Bellman-Ford, no bound needed
// ---------------------------------------------------
// This problem has no negative weights (times are positive) and no
// stop/hop limit, so it is normally taught with Dijkstra. It is included
// here specifically to demonstrate the baseline: Bellman-Ford's relax-every-
// edge, V-1-times loop gives the IDENTICAL answer to Dijkstra whenever
// weights happen to be non-negative -- because Bellman-Ford's correctness
// argument never assumed non-negative weights in the first place, it simply
// does not need the extra assumption to be right. The tradeoff is speed:
// this file is O(V*E), where a heap-based Dijkstra would be
// O((V+E) log V) -- strictly faster on this exact input, at the cost of
// being unable to handle negative weights should the problem ever change.
//
// Time:  O(V*E) -- V-1 relaxation passes, each visiting every edge once.
// Space: O(V) for the distance array, O(E) for the edge list.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

struct Edge {
  int from;
  int to;
  int weight;
};

int networkDelayTime(const std::vector<std::vector<int>>& times, int n, int k) {
  const long long kInf = std::numeric_limits<long long>::max();
  std::vector<Edge> edges;
  edges.reserve(times.size());
  for (const auto& t : times) {
    // LeetCode nodes are 1-indexed; shift to 0-indexed internally.
    edges.push_back({t[0] - 1, t[1] - 1, t[2]});
  }

  std::vector<long long> dist(n, kInf);
  dist[k - 1] = 0;

  for (int pass = 0; pass < n - 1; ++pass) {
    bool changed = false;
    for (const Edge& e : edges) {
      if (dist[e.from] == kInf) continue;
      if (dist[e.from] + e.weight < dist[e.to]) {
        dist[e.to] = dist[e.from] + e.weight;
        changed = true;
      }
    }
    if (!changed) break;
  }

  long long maxDist = 0;
  for (int i = 0; i < n; ++i) {
    if (dist[i] == kInf) return -1;  // some node never received the signal
    maxDist = std::max(maxDist, dist[i]);
  }
  return static_cast<int>(maxDist);
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
    // LeetCode's own worked example: times = [[2,1,1],[2,3,1],[3,4,1]], n=4, k=2
    std::vector<std::vector<int>> times = {{2, 1, 1}, {2, 3, 1}, {3, 4, 1}};
    check(networkDelayTime(times, 4, 2) == 2,
          "classic example -> 2 (node 4 is the farthest, reached in 2 hops)");
  }

  {
    // Single node, no edges needed -- signal "arrives" instantly.
    std::vector<std::vector<int>> times = {};
    check(networkDelayTime(times, 1, 1) == 0, "single node, no edges -> 0");
  }

  {
    // Unreachable node -> -1, not a crash or a huge sentinel value.
    std::vector<std::vector<int>> times = {{1, 2, 1}};
    check(networkDelayTime(times, 2, 2) == -1,
          "source cannot reach all nodes -> -1");
  }

  {
    // A longer direct edge beaten by a two-hop path -- confirms relaxation
    // actually improves the naive direct-edge distance, not just copies it.
    std::vector<std::vector<int>> times = {{1, 2, 10}, {1, 3, 1}, {3, 2, 1}};
    check(networkDelayTime(times, 3, 1) == 2,
          "two-hop path (1->3->2 = 2) beats the direct edge (1->2 = 10)");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
