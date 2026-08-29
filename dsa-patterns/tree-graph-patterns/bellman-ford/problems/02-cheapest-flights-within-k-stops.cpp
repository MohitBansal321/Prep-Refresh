// ============================================================================
// LeetCode 787 — Cheapest Flights Within K Stops
// https://leetcode.com/problems/cheapest-flights-within-k-stops/
// ============================================================================
//
// PROBLEM
// -------
// n cities, flights[i] = (from, to, price). Given src, dst, and k (the
// maximum number of STOPS, i.e. at most k+1 edges), return the cheapest
// price to reach dst within that limit, or -1 if impossible.
//
// APPROACH -- Bellman-Ford, bounded to K+1 passes instead of V-1
// -----------------------------------------------------------------
// This is Bellman-Ford's signature real-world variant. Plain Bellman-Ford
// runs V-1 passes because a shortest SIMPLE path uses at most V-1 edges --
// but here the problem imposes its OWN, tighter bound: at most k+1 edges.
// Running exactly k+1 relaxation passes (not V-1) computes the cheapest
// price reachable using AT MOST k+1 edges, which is precisely what "at
// most k stops" means. Fewer passes than the unbounded algorithm needs is
// not a shortcut here -- it is the correct way to encode "and don't use
// more than this many edges" directly into the algorithm's own structure.
//
// THE ONE CRITICAL DIFFERENCE FROM PLAIN BELLMAN-FORD: within a single
// pass, you must relax every edge using a SNAPSHOT of the distances from
// the END of the previous pass, not distances already updated earlier in
// the SAME pass. Updating dist[] in place while a pass is still running
// would let one pass silently use more than one additional edge's worth of
// improvement, breaking the "at most k+1 edges" guarantee. This is the
// single most common bug in this problem (see the header comment on
// `findCheapestPrice` below).
//
// Time:  O(K * E) -- K+1 passes (bounded by k, not V), each visiting every
//        edge once.
// Space: O(V) for the two distance snapshots.
// ============================================================================

#include <iostream>
#include <limits>
#include <string>
#include <vector>

// Returns the cheapest price from src to dst using at most k+1 edges, or -1.
int findCheapestPrice(int n, const std::vector<std::vector<int>>& flights,
                       int src, int dst, int k) {
  const long long kInf = std::numeric_limits<long long>::max();
  std::vector<long long> dist(n, kInf);
  dist[src] = 0;

  for (int pass = 0; pass <= k; ++pass) {
    // CRITICAL: relax against a snapshot from the end of the previous pass.
    // Mutating `dist` in place mid-pass would let a single pass chain two
    // improvements together, silently using k+2 edges instead of k+1.
    std::vector<long long> next = dist;
    for (const auto& f : flights) {
      int u = f[0], v = f[1], price = f[2];
      if (dist[u] == kInf) continue;
      if (dist[u] + price < next[v]) {
        next[v] = dist[u] + price;
      }
    }
    dist = next;
  }

  return dist[dst] == kInf ? -1 : static_cast<int>(dist[dst]);
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
    // n=4, edges=[[0,1,100],[1,2,100],[2,3,100],[0,2,500]], src=0, dst=3,
    // k=1 (at most 2 edges). The cheap route 0->1->2->3 needs 3 edges
    // (2 stops), exceeding the budget, so the algorithm must fall back to
    // 0->2 (500) -> 3 (100) = 600, which uses exactly 2 edges (1 stop).
    std::vector<std::vector<int>> flights = {
        {0, 1, 100}, {1, 2, 100}, {2, 3, 100}, {0, 2, 500}};
    check(findCheapestPrice(4, flights, 0, 3, 1) == 600,
          "k=1 stop -> 600 (0->2->3), the cheap 3-edge route is out of budget");
  }

  {
    // Same graph, k=0 (direct flights only, at most 1 edge) -- no edge
    // 0->3 exists directly, so the destination is unreachable.
    std::vector<std::vector<int>> flights = {
        {0, 1, 100}, {1, 2, 100}, {2, 3, 100}, {0, 2, 500}};
    check(findCheapestPrice(4, flights, 0, 3, 0) == -1,
          "k=0 stops, no direct edge to dst -> -1");
  }

  {
    // Same graph, k=2 (unlimited for this graph) -- the cheap 3-edge route
    // (0->1->2->3 = 300) is now within budget and should win over 600.
    std::vector<std::vector<int>> flights = {
        {0, 1, 100}, {1, 2, 100}, {2, 3, 100}, {0, 2, 500}};
    check(findCheapestPrice(4, flights, 0, 3, 2) == 300,
          "k=2 stops -> 300, the cheap 3-edge route now fits the budget");
  }

  {
    // src == dst: zero edges needed, cost is trivially 0 regardless of k.
    std::vector<std::vector<int>> flights = {{0, 1, 100}};
    check(findCheapestPrice(2, flights, 0, 0, 0) == 0,
          "src == dst -> 0, no flight needed");
  }

  {
    // A cheaper multi-hop path exists but is blocked purely by the stop
    // budget -- confirms the algorithm respects k even when a cheaper,
    // over-budget path exists, rather than always returning the global
    // cheapest path regardless of hop count.
    std::vector<std::vector<int>> flights = {
        {0, 1, 1}, {1, 2, 1}, {2, 3, 1}, {3, 4, 1}, {0, 4, 100}};
    check(findCheapestPrice(5, flights, 0, 4, 2) == 100,
          "cheap 4-edge path (cost 4) exceeds k=2 budget -> falls back to "
          "the pricier direct edge (100)");
    check(findCheapestPrice(5, flights, 0, 4, 3) == 4,
          "k=3 stops -> the cheap 4-edge path (cost 4) now fits");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
