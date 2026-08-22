// ============================================================================
// LeetCode 743 — Network Delay Time
// ============================================================================
//
// PROBLEM
// -------
// You are given a network of `n` nodes, labeled 1 through n, and a list of
// directed edges `times[i] = (ui, vi, wi)` meaning a signal takes `wi` time
// units to travel from node `ui` to node `vi`. A signal is sent from node
// `k`. Return the minimum time it takes for ALL n nodes to receive the
// signal, or -1 if it is impossible.
//
// Example: n = 4, times = [(2,1,1), (2,3,1), (3,4,1)], k = 2  ->  2
//          (signal leaves 2 at t=0; nodes 1 and 3 get it at t=1;
//           node 4 gets it at t=2 via 3.)
//
// APPROACH — Textbook Dijkstra (single source, sum of weights)
// ------------------------------------------------------------
// Translate the problem statement literally into graph terms:
//   - "minimum time for all nodes to receive the signal" = the MAXIMUM over
//     all nodes of the shortest distance from k to that node. The signal
//     propagates along cheapest paths simultaneously, so each node receives
//     it exactly at its shortest-path time, and "all received" happens when
//     the slowest (largest-distance) node finally gets it.
//   - "-1 if impossible" = some node is unreachable, i.e. its finalized
//     distance stays at infinity.
//
// So this is single-source shortest path on a weighted graph with strictly
// positive weights — the exact recognition signal for Dijkstra (see
// ../README.md and ../images/recognition-diagram.md).
//
// The algorithm replaces BFS's FIFO queue with a min-heap keyed on tentative
// distance: pop the closest not-yet-finalized node, and its distance is now
// provably final (any alternative path must enter through a heap key that is
// already >= this distance, so nothing cheaper can appear later). Then relax
// each outgoing edge: if dist[u] + w beats dist[v], record the improvement
// and push a new heap entry. Old, superseded entries are left in place and
// discarded later by comparing the popped distance against dist[u] — the
// lazy-deletion pattern (see ../images/flow-diagram.md for the full loop and
// ../images/trace-diagram.md for a worked trace of this exact routine).
//
// Why non-negative weights matter here: every weight wi >= 1 in this problem,
// so the argument "a popped node can never be reached more cheaply later"
// holds. With negative weights a longer path could subtract enough to beat
// an already-finalized shorter one, and Dijkstra's answer would be wrong.
//
// COMPLEXITY
// ----------
// Time:  O(E log V) — each edge causes at most one push (O(log V)), and each
//        pop costs O(log V); stale pops cost O(log V) each but there are at
//        most E of them total since pushes are bounded by E + 1.
// Space: O(V + E) — adjacency list, dist array, and the heap (which holds at
//        most E entries under lazy deletion).
// ============================================================================

#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <utility>
#include <vector>

const int kInf = std::numeric_limits<int>::max();

typedef std::vector<std::vector<std::pair<int, int>>> AdjList;

// Returns shortest distances from src to every node (kInf if unreachable).
std::vector<int> dijkstra(int n, int src, const AdjList& adj) {
  std::vector<int> dist(n, kInf);
  dist[src] = 0;

  // Min-heap of {distance, node}; std::greater<> flips the default max-heap.
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<> >
      pq;
  pq.push(std::make_pair(0, src));

  while (!pq.empty()) {
    // .first/.second instead of structured bindings (GCC 6.3 has no C++17
    // structured bindings support).
    const int d = pq.top().first;   // distance recorded when pushed
    const int u = pq.top().second;  // the node that distance belongs to
    pq.pop();
    if (d > dist[u]) continue;  // stale entry — a better one already won

    for (size_t i = 0; i < adj[u].size(); ++i) {
      const int v = adj[u][i].first;   // neighbour node
      const int w = adj[u][i].second;  // weight of the u -> v edge
      // Relax: found a cheaper path to v through u.
      if (dist[u] != kInf && dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        pq.push(std::make_pair(dist[v], v));  // lazy: old entry stays put
      }
    }
  }
  return dist;
}

int networkDelayTime(const std::vector<std::vector<int> >& times, int n, int k) {
  // Nodes are labeled 1..n per LeetCode's contract; shift to 0-based.
  AdjList adj(n);
  for (size_t i = 0; i < times.size(); ++i) {
    const int u = times[i][0] - 1;
    const int v = times[i][1] - 1;
    const int w = times[i][2];
    adj[u].push_back(std::make_pair(v, w));
  }

  const std::vector<int> dist = dijkstra(n, k - 1, adj);

  // Answer = maximum shortest distance across all nodes; -1 if any is INF.
  int answer = 0;
  for (int i = 0; i < n; ++i) {
    if (dist[i] == kInf) return -1;
    if (dist[i] > answer) answer = dist[i];
  }
  return answer;
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
    // Classic example: chain 2->1->... wait, actually 2->{1,3}, 3->4.
    // Shortest times from 2: node1=1, node3=1, node4=2. Max = 2.
    std::vector<std::vector<int> > times;
    times.push_back({2, 1, 1});
    times.push_back({2, 3, 1});
    times.push_back({3, 4, 1});
    check(networkDelayTime(times, 4, 2) == 2,
          "chain example, all reachable -> max delay 2");
  }

  {
    // Edge points AWAY from node 2 (2 -> 1), so when the signal starts at
    // node 1, node 2 never receives it: unreachable -> -1.
    std::vector<std::vector<int> > times;
    times.push_back({2, 1, 1});
    check(networkDelayTime(times, 2, 1) == -1,
          "unreachable node -> -1");
  }

  {
    // Single node, no edges: only the source itself, which trivially
    // "receives" the signal at time 0.
    std::vector<std::vector<int> > times;
    check(networkDelayTime(times, 1, 1) == 0,
          "single node graph -> 0");
  }

  {
    // Indirect path beats direct edge (the case plain BFS gets wrong):
    // 1->2 w10 direct, but 1->3 w1 -> 3->2 w2 gives cost 3.
    std::vector<std::vector<int> > times;
    times.push_back({1, 2, 10});
    times.push_back({1, 3, 1});
    times.push_back({3, 2, 2});
    check(networkDelayTime(times, 3, 1) == 3,
          "indirect path (cost 1+2=3) beats direct edge (cost 10)");
  }

  {
    // Repeated improvement (heavy lazy-deletion churn): node 2 is first
    // discovered at cost 7, then improved to 6 via node 3, then improved
    // again to 4 via node 4. Both superseded heap entries must be skipped.
    // Final distances: 2 -> 4, 3 -> 1, 4 -> 2; max = 4.
    std::vector<std::vector<int> > times;
    times.push_back({1, 2, 7});
    times.push_back({1, 3, 1});
    times.push_back({3, 2, 5});
    times.push_back({3, 4, 1});
    times.push_back({4, 2, 2});
    check(networkDelayTime(times, 4, 1) == 4,
          "repeated improvements force multiple stale pops -> 4");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
