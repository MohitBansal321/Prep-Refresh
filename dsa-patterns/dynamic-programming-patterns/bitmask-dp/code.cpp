// ============================================================================
// Bitmask DP — generic reusable template (C++17)
// ============================================================================
//
// Traveling-salesman-style: dp[mask][last] = min cost to have visited
// exactly the set `mask` of nodes, currently standing at node `last`.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

int minHamiltonianCost(const std::vector<std::vector<int>>& cost) {
  int n = static_cast<int>(cost.size());
  const int kInf = std::numeric_limits<int>::max() / 2;
  std::vector<std::vector<int>> dp(1 << n, std::vector<int>(n, kInf));

  dp[1][0] = 0;

  for (int mask = 1; mask < (1 << n); ++mask) {
    for (int last = 0; last < n; ++last) {
      if (!(mask & (1 << last)) || dp[mask][last] == kInf) continue;

      for (int next = 0; next < n; ++next) {
        if (mask & (1 << next)) continue;
        int nextMask = mask | (1 << next);
        dp[nextMask][next] =
            std::min(dp[nextMask][next], dp[mask][last] + cost[last][next]);
      }
    }
  }

  int full = (1 << n) - 1;
  int best = kInf;
  for (int last = 0; last < n; ++last) {
    if (dp[full][last] < kInf) {
      best = std::min(best, dp[full][last] + cost[last][0]);
    }
  }
  return best;
}

// Can `nums` be split into exactly `k` subsets that each sum to the same
// value? dp[mask] = the current (incomplete) group's running sum once
// exactly the elements in `mask` have been placed into finished-or-in-
// progress groups; dp[mask] == -1 means that mask is not reachable by any
// valid placement. Reaching dp[full_mask] == 0 means every group closed out
// exactly on target, with nothing left over.
bool canPartitionIntoKSubsets(const std::vector<int>& nums, int k) {
  int n = static_cast<int>(nums.size());
  int total = 0;
  for (int x : nums) total += x;
  if (k <= 0 || total % k != 0) return false;
  int target = total / k;

  std::vector<int> dp(1 << n, -1);
  dp[0] = 0;

  for (int mask = 0; mask < (1 << n); ++mask) {
    if (dp[mask] == -1) continue;

    for (int i = 0; i < n; ++i) {
      if (mask & (1 << i)) continue;
      if (dp[mask] + nums[i] > target) continue;

      int nextMask = mask | (1 << i);
      if (dp[nextMask] != -1) continue;
      dp[nextMask] = (dp[mask] + nums[i]) % target;
    }
  }

  return dp[(1 << n) - 1] == 0;
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
    // 4-city symmetric TSP; optimal tour 0-1-3-2-0 costs 10+25+30+15=80.
    std::vector<std::vector<int>> cost = {
        {0, 10, 15, 20},
        {10, 0, 35, 25},
        {15, 35, 0, 30},
        {20, 25, 30, 0},
    };
    check(minHamiltonianCost(cost) == 80, "4-city TSP finds the optimal 80-cost tour");
  }

  {
    // 2-city trivial case: go there and back.
    std::vector<std::vector<int>> cost = {{0, 5}, {5, 0}};
    check(minHamiltonianCost(cost) == 10, "2-city TSP: round trip costs 5+5=10");
  }

  {
    check(canPartitionIntoKSubsets({4, 3, 2, 3, 5, 2, 1}, 4) == true,
          "{4,1},{3,2},{5},{3,2} split into 4 groups summing to 5 each");
    check(canPartitionIntoKSubsets({1, 2, 3, 4}, 2) == true,
          "{1,4} and {2,3} split into 2 groups summing to 5 each");
    check(canPartitionIntoKSubsets({1, 2, 3, 5}, 3) == false,
          "total sum 11 is not divisible into 3 equal groups");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
