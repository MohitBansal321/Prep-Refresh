// ============================================================================
// LeetCode 1928 — Minimum Cost to Reach Destination in Time
// https://leetcode.com/problems/minimum-cost-to-reach-destination-in-time/
// ============================================================================
//
// PROBLEM
// -------
// n cities (0..n-1). edges[i] = (u, v, time) -- an undirected road between u
// and v taking `time` minutes to cross (can be crossed either direction, any
// number of times). passingFees[i] is the fee to BE PRESENT in city i (paid
// once per visit, including the start and the destination). Starting at city
// 0 at time 0, reach city n-1 at or before `maxTime`, minimizing the total
// fees paid. Return -1 if it cannot be done within maxTime.
//
// APPROACH -- Bellman-Ford relaxation bounded by a RESOURCE, not a hop count
// -----------------------------------------------------------------------
// LeetCode 787 bounds the relaxation by NUMBER OF EDGES (at most k+1). This
// problem bounds it by a different, continuously-valued resource: TOTAL TIME
// SPENT. The generalization is the same idea Bellman-Ford's proof always
// rested on -- relax edges enough times to account for every path using at
// most the budgeted resource -- but here "budget" is a time limit, not an
// edge count, so the state that must be tracked per node is dist[node][time]
// (the minimum fee to reach `node` having spent exactly or at most `time`
// minutes), not a single scalar per node.
//
// dp[t][u] = minimum total fee to be standing at city u after spending
// exactly `t` minutes of travel so far (t ranges 0..maxTime). The relaxation
// rule for each edge (u, v, w) is a two-way version of Bellman-Ford's rule:
// for every t from w to maxTime, dp[t][v] can improve from
// dp[t-w][u] + fee[v], and symmetrically dp[t][u] can improve from
// dp[t-w][v] + fee[u] (the road is undirected). The final answer is the
// minimum of dp[t][n-1] over every t from 0 to maxTime.
//
// Time:  O(maxTime * E) -- one relaxation attempt per (time value, edge)
//        pair, directly analogous to Bellman-Ford's O(V*E) with "V-1 passes"
//        replaced by "maxTime distinct time values."
// Space: O(maxTime * n) for the dp table.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

int minCost(int maxTime, const std::vector<std::vector<int>>& edges,
            const std::vector<int>& passingFees) {
  const int n = static_cast<int>(passingFees.size());
  const long long kInf = std::numeric_limits<long long>::max() / 2;

  // dp[t][u]: minimum fee to be at city u having spent exactly t minutes.
  std::vector<std::vector<long long>> dp(
      maxTime + 1, std::vector<long long>(n, kInf));
  dp[0][0] = passingFees[0];  // pay to stand at the start, at time 0

  for (int t = 0; t <= maxTime; ++t) {
    for (const auto& e : edges) {
      int u = e[0], v = e[1], w = e[2];
      if (t - w < 0) continue;  // not enough time budget for this edge yet
      if (dp[t - w][u] != kInf) {
        dp[t][v] = std::min(dp[t][v], dp[t - w][u] + passingFees[v]);
      }
      if (dp[t - w][v] != kInf) {
        dp[t][u] = std::min(dp[t][u], dp[t - w][v] + passingFees[u]);
      }
    }
  }

  long long best = kInf;
  for (int t = 0; t <= maxTime; ++t) {
    best = std::min(best, dp[t][n - 1]);
  }
  return best >= kInf ? -1 : static_cast<int>(best);
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
    // maxTime=30, edges=[[0,1,10],[1,2,10],[2,5,10],[0,3,1],[3,4,10],
    // [4,5,15]], passingFees=[5,1,2,20,20,3]. Two routes to city 5:
    // 0->1->2->5 costs time 30, fee 5+1+2+3=11; 0->3->4->5 costs time 21,
    // fee 5+20+20+3=48. The cheaper-fee route (11) fits exactly at maxTime.
    std::vector<std::vector<int>> edges = {
        {0, 1, 10}, {1, 2, 10}, {2, 5, 10}, {0, 3, 1}, {3, 4, 10}, {4, 5, 15}};
    std::vector<int> fees = {5, 1, 2, 20, 20, 3};
    check(minCost(30, edges, fees) == 11,
          "cheap-fee route (0->1->2->5, fee 11) fits exactly at maxTime=30");
  }

  {
    // Same graph, tighter maxTime=29 -- the cheap route needs exactly 30
    // minutes and no longer fits, forcing the more expensive 21-minute
    // route (fee 48).
    std::vector<std::vector<int>> edges = {
        {0, 1, 10}, {1, 2, 10}, {2, 5, 10}, {0, 3, 1}, {3, 4, 10}, {4, 5, 15}};
    std::vector<int> fees = {5, 1, 2, 20, 20, 3};
    check(minCost(29, edges, fees) == 48,
          "tighter maxTime=29 excludes the cheap route -> falls back to fee 48");
  }

  {
    // maxTime too small to reach the destination at all -> -1, not a huge
    // sentinel value or a crash.
    std::vector<std::vector<int>> edges = {{0, 1, 10}};
    std::vector<int> fees = {1, 1};
    check(minCost(5, edges, fees) == -1, "not enough time to travel -> -1");
  }

  {
    // src == dst (n == 1): only the starting fee is ever paid, regardless
    // of maxTime.
    std::vector<std::vector<int>> edges = {};
    std::vector<int> fees = {7};
    check(minCost(0, edges, fees) == 7,
          "single city, no travel needed -> just the starting fee");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
