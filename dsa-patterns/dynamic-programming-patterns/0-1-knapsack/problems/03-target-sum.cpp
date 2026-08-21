// ============================================================================
// LeetCode 494 — Target Sum
// https://leetcode.com/problems/target-sum/
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array nums (non-negative) and an integer target, put a '+'
// or a '-' in front of every element and concatenate them into an expression.
// Return the NUMBER of different sign assignments that make the expression
// evaluate to target.
//
// Example: nums = [1,1,1,1,1], target = 3 -> 5
//          (-1+1+1+1+1, +1-1+1+1+1, +1+1-1+1+1, +1+1+1-1+1, +1+1+1+1-1)
//
// APPROACH — 0/1 Knapsack, the COUNT-THE-WAYS framing
// ----------------------------------------------------
// This is the one problem in this module where the recurrence's OPERATOR
// changes. The two-choice structure from ../README.md is untouched -- for
// each number, skip it or take it -- but instead of
//     dp[s] = max(dp[s], dp[s - num] + value)
// we write
//     dp[s] = dp[s] + dp[s - num]
// because we are no longer asking "what is the best?" but "how many distinct
// ways?". Two disjoint sets of ways (those that exclude num, those that
// include it) are combined by ADDING their counts, not by taking a max. Any
// time a problem says "count the number of ..." over a subset choice under a
// numeric budget, this is the substitution to reach for.
//
// STEP 1 — turn signs into a subset choice.
// Let P be the set of numbers that receive a '+' and N the set that receives
// a '-'. Then
//     sum(P) - sum(N) = target        and        sum(P) + sum(N) = total
// Adding the two equations:  2 * sum(P) = total + target,  so
//     sum(P) = (total + target) / 2
// Every valid sign assignment corresponds to exactly one subset P summing to
// that number, and vice versa -- so counting sign assignments IS counting
// subsets that sum to (total + target) / 2. The '-' side never needs to be
// considered again; it is whatever is left over.
//
// STEP 2 — count subsets summing to exactly P, with each number used once:
//     item i's WEIGHT = nums[i]
//     CAPACITY        = (total + target) / 2
//     dp[s]           = number of subsets of the numbers seen so far that
//                       sum to exactly s
//     base case       = dp[0] = 1   (the empty subset -- ONE way to make 0)
//
// THE DETAILS PEOPLE GET WRONG
// -----------------------------
// (1) The FEASIBILITY GUARDS on (total + target) / 2. Three separate things
//     can go wrong, and all three must be rejected before allocating a table:
//       - (total + target) can be ODD. Since sum(P) is an integer, an odd
//         numerator means no valid assignment exists at all -> return 0.
//         Truncating the division instead of rejecting silently produces a
//         count for the wrong target.
//       - (total + target) can be NEGATIVE, when target < -total. A negative
//         capacity is not just wrong, it crashes: vector(capacity + 1) with a
//         negative argument is a huge unsigned size.
//       - target can exceed total in magnitude -> unreachable, return 0.
//     Guarding with abs(target) > total handles the last two together.
//
// (2) dp[0] = 1, NOT 0. dp[0] is the count of ways to make a sum of zero,
//     and there is exactly one: choose nothing. Initialize it to 0 and every
//     count in the table stays 0 forever, because every count is ultimately
//     built by extending the empty subset.
//
// (3) ZEROS in nums are real, distinct choices. A 0 can take either sign and
//     both give the same value, so each zero DOUBLES the answer. The reverse
//     sweep handles this correctly without any special case: with num == 0
//     the inner loop runs over every s and performs dp[s] += dp[s - 0], i.e.
//     dp[s] += dp[s], which doubles each cell exactly once. That is why
//     nums = [0,0,0,0,0], target = 0 must return 2^5 = 32 and not 1. Adding
//     a well-meaning "if (num == 0) continue;" is a real bug.
//
// COMPLEXITY
// ----------
// Time:  O(n * (total + target) / 2).
// Space: O((total + target) / 2) for the 1D version.
// ============================================================================

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// findTargetSumWays — 1D counting knapsack, capacity swept BACKWARD.
// ----------------------------------------------------------------------------
int findTargetSumWays(const std::vector<int>& nums, int target) {
  int total = 0;
  for (size_t k = 0; k < nums.size(); ++k) total += nums[k];

  // Guard (1a): target is out of reach no matter how the signs fall.
  if (std::abs(target) > total) return 0;
  // Guard (1b): sum(P) would have to be a non-integer.
  if ((total + target) % 2 != 0) return 0;

  int positiveSum = (total + target) / 2;  // Guaranteed >= 0 by the guards.

  std::vector<int> dp(positiveSum + 1, 0);
  dp[0] = 1;  // Exactly one way to reach a sum of 0: pick nothing.

  for (size_t k = 0; k < nums.size(); ++k) {
    int num = nums[k];
    // BACKWARD sweep, for the same reason as every other file in this
    // module: dp[s - num] must still be the "before this number existed"
    // count when we read it. Forward would count a single number multiple
    // times, inflating the answer (it would answer the UNBOUNDED version:
    // "how many multisets, reusing numbers freely").
    //
    // Note s >= num, not s >= 1: when num == 0 the loop covers every s and
    // performs dp[s] += dp[s], doubling -- which is exactly right (see
    // header note 3).
    for (int s = positiveSum; s >= num; --s) {
      dp[s] += dp[s - num];
    }
  }

  return dp[positiveSum];
}

// ----------------------------------------------------------------------------
// findTargetSumWaysBruteForce — the O(2^n) oracle the DP replaces.
//
// Enumerate all 2^n sign assignments directly (bit k set == give nums[k] a
// minus) and count the ones that hit target. Used only to validate the DP on
// small inputs; this is the enumeration whose exponential cost is the entire
// motivation for the table (see ../README.md, "Why Not Other Approaches?").
// ----------------------------------------------------------------------------
int findTargetSumWaysBruteForce(const std::vector<int>& nums, int target) {
  int n = static_cast<int>(nums.size());
  if (n > 20) return -1;  // Refuse rather than hang.

  int ways = 0;
  int assignmentCount = 1 << n;
  for (int mask = 0; mask < assignmentCount; ++mask) {
    int value = 0;
    for (int k = 0; k < n; ++k) {
      if (mask & (1 << k)) {
        value -= nums[k];
      } else {
        value += nums[k];
      }
    }
    if (value == target) ++ways;
  }
  return ways;
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

  // Runs the DP and the brute-force oracle on the same input and requires
  // both to equal the hand-computed expectation.
  auto checkBoth = [&](const std::vector<int>& nums, int target, int expected,
                       const std::string& label) {
    check(findTargetSumWays(nums, target) == expected, "dp   : " + label);
    check(findTargetSumWaysBruteForce(nums, target) == expected, "brute: " + label);
  };

  {
    // LeetCode's own example.
    std::vector<int> nums;
    for (int k = 0; k < 5; ++k) nums.push_back(1);
    checkBoth(nums, 3, 5, "[1,1,1,1,1], target 3 -> 5");
  }

  {
    std::vector<int> nums;
    nums.push_back(1);
    checkBoth(nums, 1, 1, "[1], target 1 -> 1 (single element, +1)");
  }

  {
    // NEGATIVE target is perfectly legal and must not break the arithmetic:
    // total = 1, (1 + -1) / 2 = 0, count subsets summing to 0 -> 1 (empty).
    std::vector<int> nums;
    nums.push_back(1);
    checkBoth(nums, -1, 1, "[1], target -1 -> 1 (negative target, -1)");
  }

  {
    // Unreachable: |target| > total, caught by guard (1a) before allocating.
    std::vector<int> nums;
    nums.push_back(1);
    checkBoth(nums, 2, 0, "[1], target 2 -> 0 (|target| exceeds total)");
  }

  {
    // Far out of range in the NEGATIVE direction -- this is the case that
    // would compute a negative capacity and blow up vector allocation if the
    // abs(target) > total guard were missing.
    std::vector<int> nums;
    nums.push_back(1); nums.push_back(2);
    checkBoth(nums, -100, 0, "[1,2], target -100 -> 0 (guard prevents negative capacity)");
  }

  {
    // Parity rejection: total = 3, target = 2, total + target = 5 is odd.
    std::vector<int> nums;
    nums.push_back(1); nums.push_back(2);
    checkBoth(nums, 2, 0, "[1,2], target 2 -> 0 (total+target is odd)");
  }

  {
    // Two distinct assignments: +1+2-3 and -1-2+3.
    std::vector<int> nums;
    nums.push_back(1); nums.push_back(2); nums.push_back(3);
    checkBoth(nums, 0, 2, "[1,2,3], target 0 -> 2 (+1+2-3 and -1-2+3)");
  }

  {
    // Duplicates: positiveSum = 2, and the subsets summing to 2 are {2} and
    // {1,1} -- giving +1+1-2 and -1-1+2. Equal VALUES at different positions
    // are still distinct choices, so the count is over positions, never over
    // multisets of values.
    std::vector<int> nums;
    nums.push_back(1); nums.push_back(1); nums.push_back(2);
    checkBoth(nums, 0, 2, "[1,1,2], target 0 -> 2 (duplicates count by position)");
  }

  {
    // ALL ZEROS: every zero doubles the count, so the answer is 2^5 = 32.
    // The single most instructive test in this file -- it is what breaks if
    // you "optimize" zeros out of the loop.
    std::vector<int> nums;
    for (int k = 0; k < 5; ++k) nums.push_back(0);
    checkBoth(nums, 0, 32, "[0,0,0,0,0], target 0 -> 32 (each zero DOUBLES the count)");
  }

  {
    // Zeros mixed with real numbers: the single zero doubles the two ways
    // that [1,2,3] has of reaching 0.
    std::vector<int> nums;
    nums.push_back(0); nums.push_back(1); nums.push_back(2); nums.push_back(3);
    checkBoth(nums, 0, 4, "[0,1,2,3], target 0 -> 4 (one zero doubles the 2 ways)");
  }

  {
    // Even total, in-range target, correct parity -- but no subset reaches
    // the required positive sum. total = 100, positiveSum = 50, and 50 is
    // simply not a sum of any subset of [100].
    std::vector<int> nums;
    nums.push_back(100);
    checkBoth(nums, 0, 0, "[100], target 0 -> 0 (parity fine, sum 50 unreachable)");
  }

  {
    // Empty array: total 0, target 0, positiveSum 0, dp[0] = 1 -- there is
    // exactly one empty expression and it evaluates to 0.
    std::vector<int> nums;
    checkBoth(nums, 0, 1, "[], target 0 -> 1 (the empty expression evaluates to 0)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
