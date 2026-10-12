// ============================================================================
// LeetCode 1334 — Find the City With the Smallest Number of Neighbors at a
//                 Threshold Distance
// ============================================================================
//
// PROBLEM
// -------
// There are `n` cities numbered 0..n-1, connected by bidirectional weighted
// edges `edges[i] = (from_i, to_i, weight_i)`. Given a distance `threshold`,
// return the city with the smallest number of cities reachable from it via
// some path whose TOTAL distance is at most `threshold`. If multiple such
// cities exist, return the one with the LARGEST number.
//
// Example: n = 4, edges = [(0,1,3),(1,2,1),(1,3,4),(2,3,1)],
//          threshold = 4  ->  3
//          (city 0 reaches {1 (3), 2 (4)} -> 2 neighbors; city 3 reaches
//           {1 (2), 2 (1)} -> 2 as well; tie broken toward larger id = 3.)
//
// APPROACH — Dijkstra from every node on a small graph
// -----------------------------------------------------
// "Number of cities within distance t of city c" requires knowing shortest
// distances from EVERY source — an all-pairs question. The standard options:
//
//   - Floyd-Warshall: O(V^3), one dense triple loop, no heap.
//   - Run Dijkstra V times: O(V * E log V).
//
// With n <= ~100 per this problem's constraints, both fit easily. We use
// repeated Dijkstra here because it reuses this module's template verbatim
// and is faster on sparse graphs — the typical interview expectation is to
// recognize the all-pairs need and consciously CHOOSE one of the two with a
// complexity justification, not to default blindly to Floyd-Warshall.
//
// Per-source bookkeeping is trivial: count finalized distances <= threshold,
// then keep the best (count, id) pair under the "minimize count, break ties
// toward larger id" rule — note that rule means we update our answer when
// the new candidate has fewer neighbors OR an equal count with a bigger id.
//
// Edge cases exercised in main(): disconnected components (unreachable
// cities simply never count), single-city graphs (count is 0 for everyone),
// and pure tie-breaking across all cities.
//
// COMPLEXITY
// ----------
// Time:  O(V * E log V) — V independent Dijkstra runs.
// Space: O(V + E) — adjacency list plus one distance array reused per run.
// ============================================================================

#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <utility>
#include <vector>

using namespace std;


const int kInf = numeric_limits<int>::max();

typedef vector<vector<pair<int, int> > > AdjList;

// Textbook lazy-deletion Dijkstra; identical shape to ../code.cpp.
vector<int> dijkstra(int n, int src, const AdjList& adj) {
  vector<int> dist(n, kInf);
  dist[src] = 0;

  priority_queue<pair<int, int>, vector<pair<int, int> >,
                      greater<> >
      pq;
  pq.push(make_pair(0, src));

  while (!pq.empty()) {
    // .first/.second instead of structured bindings (GCC 6.3 limitation).
    const int d = pq.top().first;
    const int u = pq.top().second;
    pq.pop();
    if (d > dist[u]) continue;  // stale entry — a better one already won

    for (size_t i = 0; i < adj[u].size(); ++i) {
      const int v = adj[u][i].first;
      const int w = adj[u][i].second;
      if (dist[u] != kInf && dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        pq.push(make_pair(dist[v], v));
      }
    }
  }
  return dist;
}

int findTheCity(int n, const vector<vector<int> >& edges,
                int distanceThreshold) {
  // Undirected graph: every edge appears in BOTH endpoints' lists.
  AdjList adj(n);
  for (size_t i = 0; i < edges.size(); ++i) {
    const int u = edges[i][0];
    const int v = edges[i][1];
    const int w = edges[i][2];
    adj[u].push_back(make_pair(v, w));
    adj[v].push_back(make_pair(u, w));
  }

  int bestCity = -1;
  int bestCount = kInf;  // minimize reachable count

  for (int src = 0; src < n; ++src) {
    const vector<int> dist = dijkstra(n, src, adj);

    // Count cities reachable within the threshold (src counts only if some
    // cycle brings it back within threshold; normally it contributes 0).
    int count = 0;
    for (int i = 0; i < n; ++i) {
      if (i != src && dist[i] <= distanceThreshold) ++count;
    }

    // Minimize count; break ties toward the LARGER city id.
    if (count < bestCount || (count == bestCount && src > bestCity)) {
      bestCount = count;
      bestCity = src;
    }
  }
  return bestCity;
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

  {
    // Classic example; cities 0 and 3 tie at 2 reachable neighbors each,
    // so the larger id (3) wins.
    vector<vector<int> > edges;
    edges.push_back({0, 1, 3});
    edges.push_back({1, 2, 1});
    edges.push_back({1, 3, 4});
    edges.push_back({2, 3, 1});
    check(findTheCity(4, edges, 4) == 3,
          "classic example: tie between 0 and 3 -> larger id 3");
  }

  {
    // Path graph 0-1-2-3-4 with every hop costing 2: threshold=2 admits
    // exactly ONE neighbour per city (distance 2); everyone ties at 1,
    // so the answer is the largest id, 4.
    vector<vector<int> > edges;
    edges.push_back({0, 1, 2});
    edges.push_back({1, 2, 2});
    edges.push_back({2, 3, 2});
    edges.push_back({3, 4, 2});
    check(findTheCity(5, edges, 2) == 4,
          "path graph, uniform hops: all tie at 1 neighbour -> 4");
  }

  {
    // Two disconnected components {0,1} and {2,3}: no cross-component paths,
    // so unreachable cities never inflate anyone's count. Each city has
    // exactly 1 neighbour within threshold 5 -> tie -> answer 3.
    vector<vector<int> > edges;
    edges.push_back({0, 1, 1});
    edges.push_back({2, 3, 1});
    check(findTheCity(4, edges, 5) == 3,
          "disconnected components handled -> 3");
  }

  {
    // Isolated city 4 with NO edges anywhere: every city has 0 reachable
    // neighbours, tie across ids -> largest id 4 wins.
    vector<vector<int> > edges;
    check(findTheCity(5, edges, 10) == 4,
          "no edges at all: everyone ties at 0 -> 4");
  }

  {
    // Single city, no edges: trivially the only candidate.
    vector<vector<int> > edges;
    check(findTheCity(1, edges, 100) == 0,
          "single city -> 0");
  }

  cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
