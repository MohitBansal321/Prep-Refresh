// ============================================================================
// LeetCode 152 — Maximum Product Subarray
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums`, find a contiguous non-empty subarray that
// has the largest PRODUCT, and return that product.
//
// Example: nums = [2,3,-2,4]  ->  6   (subarray [2,3])
// Example: nums = [-2,3,-4]   ->  24  (subarray [-2,3,-4], the whole array --
//                                      two negatives cancel into a positive)
//
// APPROACH — Kadane's Algorithm, adapted: track running MAX *and* MIN
// ---------------------------------------------------------------------
// This is the sharpest illustration in this module of "looks like Kadane's,
// but needs real adaptation" (see README.md, Disadvantages / Interview
// Discussion). For SUMS, a negative running total can only ever hurt what
// comes after it, so "restart" is always the only alternative to "extend."
// For PRODUCTS, that argument breaks: a large-magnitude NEGATIVE running
// product can become the largest POSITIVE product the instant you multiply
// it by one more negative number.
//
// The fix: track TWO running values at every index, not one --
//   current_max -- the largest product of a subarray ending here.
//   current_min -- the SMALLEST (most negative) product of a subarray
//                  ending here.
// At each step, if nums[i] is negative, multiplying flips the sign, so what
// was the running minimum can become the new maximum and vice versa --
// swap current_max and current_min BEFORE folding in nums[i]. Then each of
// current_max/current_min is recomputed as the best of "extend" (multiply)
// vs. "restart" (just nums[i] alone), exactly mirroring the sum-based
// extend-vs-restart choice, but for both directions of extremity.
//
// A zero in the array acts as a hard reset for both trackers to nums[i]
// itself (which is 0) -- this falls out naturally from the same max/min
// formulas, since multiplying by 0 always loses to "restart at 0."
//
// COMPLEXITY
// ----------
// Time:  O(n) -- one pass, O(1) work per element.
// Space: O(1) -- a fixed number of running scalars.
//
// Brute force (check every contiguous subarray's product) is O(n^2). This
// adaptation keeps Kadane's O(n) time by doubling the tracked state (max AND
// min) rather than changing the overall shape of the algorithm.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int maxProduct(const std::vector<int>& nums) {
  long long current_max = nums[0];
  long long current_min = nums[0];
  long long best = nums[0];

  for (size_t i = 1; i < nums.size(); ++i) {
    long long value = nums[i];

    // A negative value flips which running tracker is "the bigger risk" --
    // swap before combining so the formulas below stay symmetric.
    if (value < 0) {
      std::swap(current_max, current_min);
    }

    // Extend (multiply into the previous run) or restart at value alone.
    current_max = std::max(value, current_max * value);
    current_min = std::min(value, current_min * value);

    best = std::max(best, current_max);
  }

  return static_cast<int>(best);
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
    std::vector<int> nums = {2, 3, -2, 4};
    check(maxProduct(nums) == 6, "[2,3,-2,4] -> 6");
  }
  {
    // Two negatives cancel into the largest product, spanning the whole array.
    std::vector<int> nums = {-2, 3, -4};
    check(maxProduct(nums) == 24, "[-2,3,-4] -> 24 (whole array, signs cancel)");
  }
  {
    // A zero splits the array into independent segments: [0], [2,-3,4], [0],
    // [-1]. The middle segment's best subarray is [4] alone (product 4) --
    // taking all three ([2,-3,4] = -24) is a large-magnitude negative, and
    // there is no further negative number after it to flip the sign back.
    std::vector<int> nums = {0, 2, -3, 4, 0, -1};
    check(maxProduct(nums) == 4, "[0,2,-3,4,0,-1] -> 4 (best is [4] alone)");
  }
  {
    std::vector<int> nums = {-1, -2, -3, 0};
    check(maxProduct(nums) == 6, "[-1,-2,-3,0] -> 6 ([-1,-2] or [-2,-3] both give 6)");
  }
  {
    std::vector<int> nums = {-2};
    check(maxProduct(nums) == -2, "[-2] -> -2 (single element)");
  }
  {
    std::vector<int> nums = {-2, 0, -1};
    check(maxProduct(nums) == 0, "[-2,0,-1] -> 0 (zero beats any single negative)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
