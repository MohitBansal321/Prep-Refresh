// ============================================================================
// LeetCode 416 — Partition Equal Subset Sum
// https://leetcode.com/problems/partition-equal-subset-sum/
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array nums containing only positive integers, return true
// if you can partition the array into two subsets such that the sum of the
// elements in both subsets is equal.
//
// Example: nums = [1,5,11,5] -> true  ([1,5,5] and [11], both sum to 11)
//          nums = [1,2,3,5]  -> false (total 11 is odd; no equal split exists)
//
// APPROACH — 0/1 Knapsack, the SUBSET-SUM (feasibility) framing
// --------------------------------------------------------------
// Nothing here mentions weights, values, or capacity. The reformulation is
// the whole exercise, and it comes in two steps.
//
// STEP 1 — turn "split into two equal halves" into "hit one exact number".
// If the two halves must have equal sums, and their sums together are the
// array total, then each half must sum to exactly total / 2. So the question
// "can I split the array in two equal halves?" is EXACTLY the question "is
// there a subset that sums to total / 2?" -- once you find one such subset,
// its complement is forced to sum to the same amount, so you never have to
// think about the second half at all.
//
// Immediate corollary: if total is ODD, the answer is false with no work at
// all. Two integers cannot be equal and also sum to an odd number.
//
// STEP 2 — recognize that "is there a subset summing to exactly T?" is 0/1
// Knapsack with value == weight and max() replaced by "reachable or not".
// Map onto ../README.md's recurrence:
//     item i's WEIGHT   = nums[i]
//     item i's VALUE    = nums[i]   (identical -- we only care about the sum)
//     CAPACITY          = total / 2
// and instead of dp[i][w] = "best value achievable", we track
//     dp[i][s] = "is a sum of exactly s reachable using only the first i
//                 numbers?"  (a bool, not an int)
// The two-choice structure is unchanged: s is reachable using the first i
// numbers if it was already reachable without nums[i-1] (skip), OR if
// s - nums[i-1] was reachable without nums[i-1] (take it exactly once).
//
// Both versions are implemented below and cross-checked against each other,
// plus against a 2^n brute-force enumerator on small inputs -- because the
// brute force IS the Subsets pattern's enumeration, which is precisely what
// this DP replaces (see ../README.md, "Why Not Other Approaches?").
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// In the 1D version, the capacity loop MUST run BACKWARD (target down to
// num). Sweeping forward would let a single number be reused: dp[s - num]
// would already have been updated by THIS number's own pass, so dp[s] would
// record a sum built from two copies of one array element. With nums = [3]
// and target = 6, a forward sweep marks dp[3] reachable, then reads that
// fresh dp[3] to mark dp[6] reachable -- claiming [3] can be split into two
// equal halves. Backward, dp[3] is read before it is written, so dp[6] stays
// false. That is the 0/1-vs-Unbounded boundary in one line of code.
//
// A second, quieter detail: dp[0] must be initialized to TRUE (the empty
// subset sums to 0). Forget it and the whole table stays false forever,
// because every reachable sum is ultimately built up from the empty subset.
//
// COMPLEXITY
// ----------
// Time:  O(n * total/2) -- one pass per number over a target-sized row.
// Space: O(n * total/2) for the 2D table, O(total/2) for the 1D version.
//        Pseudo-polynomial: it scales with the MAGNITUDE of the sum, not
//        with how many bits the sum takes to write down.
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Shared helper: total of the array. Kept separate so both implementations
// agree on the odd-total early exit without duplicating the reasoning.
// ----------------------------------------------------------------------------
static int arraySum(const std::vector<int>& nums) {
  int total = 0;
  for (size_t k = 0; k < nums.size(); ++k) {
    total += nums[k];
  }
  return total;
}

// ----------------------------------------------------------------------------
// canPartition2D — the literal 2D translation of the recurrence.
//
// dp[i][s] = can a sum of exactly s be formed using only the first i numbers?
//
// Note the use of std::vector<char> rather than std::vector<bool>: the latter
// is a bit-packed specialization whose element access is slower and whose
// references are proxy objects, which makes it a poor default for DP tables.
// ----------------------------------------------------------------------------
bool canPartition2D(const std::vector<int>& nums) {
  int total = arraySum(nums);
  if (total % 2 != 0) return false;  // Odd total: no equal split can exist.

  int target = total / 2;
  int n = static_cast<int>(nums.size());

  std::vector<std::vector<char> > dp(n + 1, std::vector<char>(target + 1, 0));

  // Base column: a sum of 0 is always reachable -- take nothing. This is the
  // dp[*][0] = 0-value base case from ../README.md's Architecture section,
  // wearing its feasibility-flavored hat ("reachable" instead of "worth 0").
  for (int i = 0; i <= n; ++i) {
    dp[i][0] = 1;
  }
  // Base row dp[0][s > 0] stays 0: with no numbers available, no positive
  // sum is reachable. Already handled by the zero-initialization above.

  for (int i = 1; i <= n; ++i) {
    int num = nums[i - 1];  // Index shift: row i means "first i numbers",
                            // so the i-th number lives at index i - 1.
    for (int s = 0; s <= target; ++s) {
      // Choice 1 -- skip nums[i-1]: inherit the answer from the frozen row.
      dp[i][s] = dp[i - 1][s];
      // Choice 2 -- take nums[i-1], if it fits inside the sum s we are after.
      // Note both reads are from row i - 1, never row i. That is the entire
      // mechanism enforcing "each number used at most once".
      if (!dp[i][s] && num <= s) {
        dp[i][s] = dp[i - 1][s - num];
      }
    }
  }

  return dp[n][target] != 0;
}

// ----------------------------------------------------------------------------
// canPartition1D — space-optimized: one row, swept BACKWARD.
//
// Mirrors knapsack01Optimized in ../code.cpp, with max() replaced by an OR.
// ----------------------------------------------------------------------------
bool canPartition1D(const std::vector<int>& nums) {
  int total = arraySum(nums);
  if (total % 2 != 0) return false;

  int target = total / 2;

  std::vector<char> dp(target + 1, 0);
  dp[0] = 1;  // The empty subset. Omit this and nothing is ever reachable.

  for (size_t k = 0; k < nums.size(); ++k) {
    int num = nums[k];
    // BACKWARD sweep. dp[s - num] must still describe "reachable WITHOUT
    // nums[k]" at the moment we read it, i.e. it must be a row-(i-1) value.
    // Going high-to-low, we only ever write cells at or above s, so the cell
    // at s - num is guaranteed untouched by this number's own pass.
    for (int s = target; s >= num; --s) {
      if (dp[s - num]) {
        dp[s] = 1;
      }
    }
  }

  return dp[target] != 0;
}

// ----------------------------------------------------------------------------
// canPartitionBruteForce — the O(2^n) enumeration the DP replaces.
//
// Present only as an independent oracle for the tests: for each of the 2^n
// subsets (one bitmask per subset, bit k set == take nums[k]), sum it and
// check for total/2. This is the Subsets pattern's enumeration with a filter
// bolted on, exactly as described in ../README.md.
//
// Guarded to small n because 2^n is exactly the blowup we are avoiding.
// ----------------------------------------------------------------------------
bool canPartitionBruteForce(const std::vector<int>& nums) {
  int n = static_cast<int>(nums.size());
  if (n > 20) return false;  // Refuse rather than hang; tests stay small.

  int total = arraySum(nums);
  if (total % 2 != 0) return false;
  int target = total / 2;

  int subsetCount = 1 << n;
  for (int mask = 0; mask < subsetCount; ++mask) {
    int sum = 0;
    for (int k = 0; k < n; ++k) {
      if (mask & (1 << k)) sum += nums[k];
    }
    if (sum == target) return true;
  }
  return false;
}

// ============================================================================
// Tests
// ============================================================================
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

  // Every case runs all three implementations and demands they agree, so a
  // divergence between the 2D table, the 1D reverse sweep, and the brute
  // force shows up as a failure rather than as a silently plausible number.
  auto checkAll = [&](const std::vector<int>& nums, bool expected,
                      const std::string& label) {
    bool a = canPartition2D(nums);
    bool b = canPartition1D(nums);
    bool c = canPartitionBruteForce(nums);
    check(a == expected, "2D  : " + label);
    check(b == expected, "1D  : " + label);
    check(c == expected, "brute: " + label);
  };

  {
    std::vector<int> nums;
    nums.push_back(1); nums.push_back(5); nums.push_back(11); nums.push_back(5);
    checkAll(nums, true, "[1,5,11,5] -> true ([1,5,5] | [11], each sums to 11)");
  }

  {
    std::vector<int> nums;
    nums.push_back(1); nums.push_back(2); nums.push_back(3); nums.push_back(5);
    checkAll(nums, false, "[1,2,3,5] -> false (total 11 is odd)");
  }

  {
    // Even total, yet still unsplittable -- the odd-total shortcut is only a
    // shortcut, never the whole answer. total = 8, target = 4, and the
    // reachable sums are {0,1,2,3,5,6,7,8}: 4 is missing.
    std::vector<int> nums;
    nums.push_back(1); nums.push_back(2); nums.push_back(5);
    checkAll(nums, false, "[1,2,5] -> false (total 8 is EVEN but 4 is unreachable)");
  }

  {
    // Single element: total is that element, always trivially unsplittable
    // (an odd value fails on parity, an even value has no second piece).
    std::vector<int> nums;
    nums.push_back(1);
    checkAll(nums, false, "[1] -> false (single element, odd total)");
  }

  {
    // The forward-sweep trap in its smallest form. total = 2, target = 1.
    // A forward sweep over [2] would not fire here, but a forward sweep over
    // [3] with target 3 would wrongly reuse the 3; see the header comment.
    std::vector<int> nums;
    nums.push_back(2);
    checkAll(nums, false, "[2] -> false (single element cannot be split in two)");
  }

  {
    std::vector<int> nums;
    nums.push_back(1); nums.push_back(1);
    checkAll(nums, true, "[1,1] -> true (duplicates, one to each side)");
  }

  {
    // All-same values, even count -> always splittable.
    std::vector<int> nums;
    for (int k = 0; k < 4; ++k) nums.push_back(2);
    checkAll(nums, true, "[2,2,2,2] -> true (all-same, even count)");
  }

  {
    // All-same values, odd count -> total is an odd multiple, never splittable.
    std::vector<int> nums;
    for (int k = 0; k < 3; ++k) nums.push_back(5);
    checkAll(nums, false, "[5,5,5] -> false (all-same, odd count -> odd total)");
  }

  {
    // Needs three of the five numbers on one side: 3 + 3 + 3 = 9 = 18 / 2.
    std::vector<int> nums;
    nums.push_back(3); nums.push_back(3); nums.push_back(3);
    nums.push_back(4); nums.push_back(5);
    checkAll(nums, true, "[3,3,3,4,5] -> true (3+3+3 = 9 = half of 18)");
  }

  {
    // Zeros are legal weights and must not break the reverse sweep: a zero
    // contributes nothing and cannot make an odd total even.
    std::vector<int> nums;
    nums.push_back(0); nums.push_back(0); nums.push_back(4); nums.push_back(4);
    checkAll(nums, true, "[0,0,4,4] -> true (zero-weight items are harmless)");
  }

  {
    // Empty input: total 0, target 0, and the empty subset reaches 0 -- so
    // two empty halves both sum to 0. LeetCode never sends this, but the
    // implementations must still agree rather than crash on an empty table.
    std::vector<int> nums;
    checkAll(nums, true, "[] -> true (two empty halves, both summing to 0)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
