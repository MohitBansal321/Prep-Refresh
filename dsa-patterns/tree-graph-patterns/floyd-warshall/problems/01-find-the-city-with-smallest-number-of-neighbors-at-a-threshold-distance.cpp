// ============================================================================
// LeetCode 1334 — Find the City With the Smallest Number of Neighbors at a
// Threshold Distance
// https://leetcode.com/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/
// ============================================================================
//
// PROBLEM
// -------
// n cities (0..n-1), bidirectional weighted roads. For every city, count how
// many OTHER cities are reachable within `distanceThreshold`. Return the city
// with the FEWEST such reachable neighbors; if tied, return the city with the
// LARGEST id.
//
// APPROACH -- textbook all-pairs distances, then a counting pass
// -----------------------------------------------------------------
// This is Floyd-Warshall in its purest LeetCode form: the question is
// inherently about EVERY pair of cities (for each city, how many others are
// within range), which is exactly what an all-pairs distance matrix answers
// in one shot. n is guaranteed small (<= 100), which is precisely the size
// where O(V^3) is not just acceptable but the simplest correct approach --
// V runs of Dijkstra would work too, but would be more code for no real
// speed benefit at this scale.
//
// Time:  O(V^3) for the all-pairs computation, O(V^2) for the counting pass.
// Space: O(V^2) for the distance matrix.
// ============================================================================

#include <iostream>
#include <limits>
#include <string>
#include <vector>

int findTheCity(int n, const std::vector<std::vector<int>>& edges,
                 int distanceThreshold) {
  const long long kInf = std::numeric_limits<long long>::max() / 4;
  std::vector<std::vector<long long>> dist(n, std::vector<long long>(n, kInf));
  for (int i = 0; i < n; ++i) dist[i][i] = 0;
  for (const auto& e : edges) {
    dist[e[0]][e[1]] = std::min(dist[e[0]][e[1]], static_cast<long long>(e[2]));
    dist[e[1]][e[0]] = std::min(dist[e[1]][e[0]], static_cast<long long>(e[2]));
  }

  for (int k = 0; k < n; ++k) {
    for (int i = 0; i < n; ++i) {
      if (dist[i][k] == kInf) continue;
      for (int j = 0; j < n; ++j) {
        if (dist[k][j] == kInf) continue;
        if (dist[i][k] + dist[k][j] < dist[i][j]) {
          dist[i][j] = dist[i][k] + dist[k][j];
        }
      }
    }
  }

  int bestCity = -1;
  int bestCount = n;  // fewer reachable neighbors than this is required to replace bestCity
  for (int city = 0; city < n; ++city) {
    int count = 0;
    for (int other = 0; other < n; ++other) {
      if (other != city && dist[city][other] <= distanceThreshold) {
        ++count;
      }
    }
    // <=, not <, so a tie prefers the LARGER city id (we scan ascending
    // and overwrite on ties).
    if (count <= bestCount) {
      bestCount = count;
      bestCity = city;
    }
  }
  return bestCity;
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
    // n=4, edges=[[0,1,3],[1,2,1],[1,3,4],[2,3,1]], threshold=4. City 3's
    // direct edge to 1 (weight 4) is beaten by the two-hop route 3->2->1
    // (1+1=2) -- exactly the kind of improvement only the waypoint
    // relaxation discovers, not a distance a single direct-edge scan would.
    std::vector<std::vector<int>> edges = {{0, 1, 3}, {1, 2, 1}, {1, 3, 4}, {2, 3, 1}};
    int result = findTheCity(4, edges, 4);
    check(result == 3, "4-city example with threshold 4 -> city 3 has fewest reachable neighbors");
  }

  {
    // n=5, a slightly larger example: a chain 0-1-2-3-4, each edge weight 2,
    // threshold 2. Only immediate neighbors are reachable for every city
    // except the two endpoints (which have only 1 neighbor within range),
    // so cities 0 and 4 tie on count=1 -- the larger id (4) wins the tie.
    std::vector<std::vector<int>> edges = {{0, 1, 2}, {1, 2, 2}, {2, 3, 2}, {3, 4, 2}};
    int result = findTheCity(5, edges, 2);
    check(result == 4, "chain graph, tie on fewest-neighbors -> larger city id (4) wins");
  }

  {
    // A fully disconnected pair of cities: each has zero reachable
    // neighbors, tying at the minimum possible count -- larger id wins.
    std::vector<std::vector<int>> edges = {{0, 1, 1}};
    int result = findTheCity(3, edges, 1);
    check(result == 2, "city 2 is fully disconnected -> 0 neighbors, wins the tie by id");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
