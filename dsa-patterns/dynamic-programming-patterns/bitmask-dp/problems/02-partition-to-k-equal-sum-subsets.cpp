// ============================================================================
// LeetCode 698 — Partition to K Equal Sum Subsets
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums` and an integer `k`, return true if it is
// possible to split all elements of `nums` into exactly `k` non-empty
// subsets whose sums are all equal.
//
// Example: nums = [4,3,2,3,5,2,1], k = 4 -> true
//   (partition: {5}, {1,4}, {2,3}, {2,3} — each sums to 5)
//
// APPROACH — Bottom-up bitmask DP with a "running sum mod target" value
// -------------------------------------------------------------------------
// The recognition signal is textbook bitmask DP: n <= 16, and whether the
// remaining elements can complete a valid partition depends on WHICH
// elements are already committed, not just how many — two different
// 3-element prefixes can leave very different remainders.
//
// First, quick arithmetic pruning: if total % k != 0, no partition exists;
// otherwise every subset must sum to target = total / k. Any element larger
// than target makes it impossible too (caught naturally by feasibility).
//
// State: dp[mask] = the current (possibly incomplete) group's running sum,
// taken modulo target, assuming exactly the elements in `mask` have been
// placed into finished-or-in-progress groups. dp[mask] == -1 means the mask
// was never reached by any valid placement.
//
// WHY THE MODULO WORKS — this is the clever part. Every completed group
// sums to exactly target, so it contributes 0 mod target. Therefore the
// running sum of the one in-progress group is fully determined by
// (sum of everything placed) % target. We do not need to track HOW many
// groups have closed: if we eventually place every element and the running
// value is 0 at full_mask, then total % target == 0 forces every group to
// have closed exactly on target with nothing left over. Different placement
// histories reaching the same mask are interchangeable because their future
// depends only on (used set, running sum) — which is exactly what the state
// stores.
//
// Transition: from each reachable mask, try adding any unused element i
// whose running sum stays within target; write dp[mask | (1<<i)].
// Iterating masks in increasing numeric order suffices because setting a
// bit strictly increases the integer (see ../images/flow-diagram.md).
//
// COMPLEXITY
// ----------
// Time:  O(2^n * n) — every mask scans n bits once.
// Space: O(2^n) for the dp array.
// ============================================================================
#include <iostream>
#include <string>
#include <vector>

bool canPartitionIntoKSubsets(const std::vector<int>& nums, int k) {
  int n = static_cast<int>(nums.size());
  long long total = 0;
  for (int x : nums) total += x;

  // Arithmetic impossibility checks: non-positive k, or a total that cannot
  // be divided evenly into k equal groups.
  if (k <= 0 || total % k != 0) return false;
  long long target = total / k;

  // -1 marks "this subset has never been reached by a valid placement".
  std::vector<int> dp(1 << n, -1);
  dp[0] = 0;  // empty set: fresh group, running sum 0

  for (int mask = 0; mask < (1 << n); ++mask) {
    if (dp[mask] == -1) continue;  // unreachable subset — skip entirely

    for (int i = 0; i < n; ++i) {
      if (mask & (1 << i)) continue;          // element i already placed
      long long nextSum = (long long)dp[mask] + nums[i];
      if (nextSum > target) continue;         // would overflow this group

      int nextMask = mask | (1 << i);         // strictly greater than mask:
      if (dp[nextMask] != -1) continue;       // already reached some other way
      dp[nextMask] = static_cast<int>(nextSum %
                                      target);  // completed group wraps to 0
    }
  }

  // Full set reached with running sum 0 => last group closed exactly too.
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
    // Classic LeetCode example; needs real grouping, not singletons.
    check(canPartitionIntoKSubsets({4, 3, 2, 3, 5, 2, 1}, 4) == true,
          "{4,3,2,3,5,2,1}, k=4 -> true ({5},{1,4},{2,3},{2,3})");
  }

  {
    // Simple even split.
    check(canPartitionIntoKSubsets({1, 2, 3, 4}, 2) == true,
          "{1,2,3,4}, k=2 -> true ({1,4},{2,3})");
  }

  {
    // Total 11 cannot divide into 3 equal groups — arithmetic short-circuit.
    check(canPartitionIntoKSubsets({1, 2, 3, 5}, 3) == false,
          "{1,2,3,5}, k=3 -> false (total not divisible by k)");
  }

  {
    // Divisible total but genuinely impossible: target 5, yet {2,2,2,2}
    // cannot reach 5 without a 3 or 5 partner, and there are not enough
    // odd values to pair them all. Known LeetCode test case.
    check(canPartitionIntoKSubsets({2, 2, 2, 2, 3, 4, 5}, 4) == false,
          "{2,2,2,2,3,4,5}, k=4 -> false (divisible total, no valid split)");
  }

  {
    // Edge cases: single element with k=1 works; an oversized element fails.
    check(canPartitionIntoKSubsets({7}, 1) == true,
          "{7}, k=1 -> true (single group takes everything)");
    check(canPartitionIntoKSubsets({9}, 2) == false,
          "{9}, k=2 -> false (total not divisible by k)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
