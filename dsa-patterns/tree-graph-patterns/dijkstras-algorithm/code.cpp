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

using namespace std;


vector<int> dijkstra(int n, int src,
                           const vector<vector<pair<int, int>>>& adj) {
  const int kInf = numeric_limits<int>::max();
  vector<int> dist(n, kInf);
  dist[src] = 0;

  priority_queue<pair<int, int>, vector<pair<int, int>>,
                       greater<>>
      pq;
  pq.push({0, src});

  while (!pq.empty()) {
    // (structured bindings would be tidier here, but need GCC 7+)
    const pair<int, int> top = pq.top();
    const int d = top.first;   // tentative distance recorded when pushed
    const int u = top.second;  // the node that distance belongs to
    pq.pop();
    if (d > dist[u]) continue;

    for (const pair<int, int>& edge : adj[u]) {
      const int v = edge.first;   // neighbour node
      const int w = edge.second;  // weight of the u -> v edge
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

  auto check = [&](bool condition, const string& label) {
    if (condition) {
      cout << "[PASS] " << label << "\n";
      ++pass_count;
    } else {
      cout << "[FAIL] " << label << "\n";
      ++fail_count;
    }
  };

  const int kInf = numeric_limits<int>::max();

  {
    // 0 -(4)-> 1, 0 -(1)-> 2, 2 -(2)-> 1, 1 -(1)-> 3, 2 -(5)-> 3
    // Cheapest 0->1 is via 2 (1+2=3), not the direct edge (4).
    vector<vector<pair<int, int>>> adj(4);
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
    vector<vector<pair<int, int>>> adj(3);
    adj[0] = {{1, 10}};
    auto dist = dijkstra(3, 0, adj);
    check(dist[1] == 10, "reachable node gets its cost");
    check(dist[2] == kInf, "unreachable node stays at infinity");
  }

  {
    // Single node, no edges.
    vector<vector<pair<int, int>>> adj(1);
    auto dist = dijkstra(1, 0, adj);
    check(dist[0] == 0, "single node graph: source distance is 0");
  }

  cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
