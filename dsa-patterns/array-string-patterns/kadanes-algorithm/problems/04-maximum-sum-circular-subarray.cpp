// ============================================================================
// LeetCode 918 — Maximum Sum Circular Subarray
// ============================================================================
//
// PROBLEM
// -------
// Given a CIRCULAR integer array `nums` (the end connects back to the
// beginning), find the maximum possible sum of a non-empty subarray, where
// the subarray may wrap around from the end of the array back to the start.
//
// Example: nums = [1,-2,3,-2]   ->  3   (best is [3], no wraparound needed)
// Example: nums = [5,-3,5]      ->  10  (wraps: [5, ..wrap.., 5] = 5+5=10)
// Example: nums = [-3,-2,-3]    ->  -2  (all negative -- must pick one element)
//
// APPROACH — Vanilla Kadane's PLUS the "total minus minimum" trick
// -------------------------------------------------------------------
// A circular array's best subarray is one of exactly two shapes:
//   (a) a NON-wrapping subarray -- solved directly by ordinary (vanilla)
//       Kadane's Algorithm on the array as given.
//   (b) a WRAPPING subarray -- one that takes a suffix of the array plus a
//       prefix of the array, wrapping around the boundary.
//
// The key trick for case (b): the elements NOT included in a wrapping
// subarray form a single, ordinary, NON-wrapping subarray in the middle of
// the array. So:
//     best_wrapping_sum = total_sum - (minimum-sum NON-wrapping subarray)
// Removing the smallest possible "hole" from the total leaves the largest
// possible wrapping subarray. Finding "the minimum-sum non-wrapping
// subarray" is just Kadane's Algorithm again, but minimizing instead of
// maximizing (track a running current_min / best_min instead of max).
//
// The final answer is max(best_non_wrapping_sum, best_wrapping_sum) --
// EXCEPT for one degenerate case: if every element in the array is
// negative, then the "minimum subarray" IS the entire array, so
// total_sum - minimum_subarray_sum = 0 -- but 0 is not a valid answer,
// because the subarray must be non-empty and every element is negative.
// In that case, the wrapping trick must be discarded, and the answer is
// simply the ordinary (non-wrapping) Kadane's result, which correctly
// finds the single least-negative element.
//
// COMPLEXITY
// ----------
// Time:  O(n) -- two linear (Kadane's-style) passes over the array (or one
//               combined pass, as done here), not a doubled/unrolled array.
// Space: O(1) -- a fixed number of running scalars.
//
// A naive approach that "unrolls" the array into a length-2n array and reruns
// a window-bounded scan would still be workable but adds unnecessary
// complexity; this module's approach reuses vanilla Kadane's exactly twice
// (once for max, once for min) and combines the results algebraically.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int maxSubarraySumCircular(const std::vector<int>& nums) {
  int total_sum = 0;

  int current_max = nums[0];
  int best_max = nums[0];

  int current_min = nums[0];
  int best_min = nums[0];

  for (size_t i = 0; i < nums.size(); ++i) {
    total_sum += nums[i];

    if (i == 0) continue;  // current_max/current_min already seeded with nums[0].

    // Ordinary (maximizing) Kadane's pass -- best NON-wrapping subarray.
    current_max = std::max(nums[i], current_max + nums[i]);
    best_max = std::max(best_max, current_max);

    // Mirror (minimizing) Kadane's pass -- worst NON-wrapping subarray,
    // used to find the largest possible "hole" to remove for wraparound.
    current_min = std::min(nums[i], current_min + nums[i]);
    best_min = std::min(best_min, current_min);
  }

  // Degenerate case: every element is negative. Then best_min equals
  // total_sum (the minimum subarray is the whole array), so
  // total_sum - best_min would be 0 -- not a valid non-empty-subarray sum.
  // Fall back to the ordinary (non-wrapping) Kadane's result, which
  // correctly returns the single least-negative element.
  if (best_max < 0) {
    return best_max;
  }

  int best_wrapping_sum = total_sum - best_min;
  return std::max(best_max, best_wrapping_sum);
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
    std::vector<int> nums = {1, -2, 3, -2};
    check(maxSubarraySumCircular(nums) == 3, "[1,-2,3,-2] -> 3 (no wraparound needed)");
  }
  {
    std::vector<int> nums = {5, -3, 5};
    check(maxSubarraySumCircular(nums) == 10, "[5,-3,5] -> 10 (wraps: 5 + 5)");
  }
  {
    // All-negative: must return the least-negative element, not 0.
    std::vector<int> nums = {-3, -2, -3};
    check(maxSubarraySumCircular(nums) == -2, "[-3,-2,-3] -> -2 (least-negative, NOT 0)");
  }
  {
    // Best is the non-wrapping subarray [3,-1,2] = 4; the wrapping
    // alternative (total 3, minus minimum non-wrap subarray -1) also
    // evaluates to 4, so the two happen to tie here.
    std::vector<int> nums = {3, -1, 2, -1};
    check(maxSubarraySumCircular(nums) == 4, "[3,-1,2,-1] -> 4 (non-wrap [3,-1,2], ties with wrap)");
  }
  {
    std::vector<int> nums = {3, -2, 2, -3};
    check(maxSubarraySumCircular(nums) == 3, "[3,-2,2,-3] -> 3 (best is [3] alone)");
  }
  {
    // Best is the non-wrapping subarray [9,-3,3] = 9; total sum is -1 and
    // the best possible wrap (total minus the minimum non-wrap subarray,
    // -6) only reaches 5, so the non-wrapping answer wins here.
    std::vector<int> nums = {-2, 4, -5, 4, -5, 9, -3, 3, -6};
    check(maxSubarraySumCircular(nums) == 9, "[-2,4,-5,4,-5,9,-3,3,-6] -> 9 (non-wrap [9,-3,3])");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
