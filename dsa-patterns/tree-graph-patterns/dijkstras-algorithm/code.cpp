// ============================================================================
// Dijkstra's Algorithm — generic reusable template (C++17)
// ============================================================================
//
// Single-source shortest path on a weighted graph with non-negative edge
// weights, using a min-heap to always finalize the closest not-yet-settled
// node next.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <limits>
#include <queue>
#include <vector>

std::vector<int> dijkstra(int n, int src,
                           const std::vector<std::vector<std::pair<int, int>>>& adj) {
  const int kInf = std::numeric_limits<int>::max();
  std::vector<int> dist(n, kInf);
  dist[src] = 0;

  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                       std::greater<>>
      pq;
  pq.push({0, src});

  while (!pq.empty()) {
    auto [d, u] = pq.top();
    pq.pop();
    if (d > dist[u]) continue;

    for (auto [v, w] : adj[u]) {
      if (dist[u] != kInf && dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        pq.push({dist[v], v});
      }
    }
  }
  return dist;
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

  const int kInf = std::numeric_limits<int>::max();

  {
    // 0 -(4)-> 1, 0 -(1)-> 2, 2 -(2)-> 1, 1 -(1)-> 3, 2 -(5)-> 3
    // Cheapest 0->1 is via 2 (1+2=3), not the direct edge (4).
    std::vector<std::vector<std::pair<int, int>>> adj(4);
    adj[0] = {{1, 4}, {2, 1}};
    adj[2] = {{1, 2}, {3, 5}};
    adj[1] = {{3, 1}};

    auto dist = dijkstra(4, 0, adj);
    check(dist[0] == 0, "dist to source is 0");
    check(dist[1] == 3, "dist[1] == 3 (via node 2, not the direct weight-4 edge)");
    check(dist[2] == 1, "dist[2] == 1 (direct edge)");
    check(dist[3] == 4, "dist[3] == 4 (0->2->1->3 = 1+2+1)");
  }

  {
    // Disconnected node stays unreachable.
    std::vector<std::vector<std::pair<int, int>>> adj(3);
    adj[0] = {{1, 10}};
    auto dist = dijkstra(3, 0, adj);
    check(dist[1] == 10, "reachable node gets its cost");
    check(dist[2] == kInf, "unreachable node stays at infinity");
  }

  {
    // Single node, no edges.
    std::vector<std::vector<std::pair<int, int>>> adj(1);
    auto dist = dijkstra(1, 0, adj);
    check(dist[0] == 0, "single node graph: source distance is 0");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
