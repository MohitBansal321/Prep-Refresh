// ============================================================================
// LeetCode 53 — Maximum Subarray
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums`, find the contiguous subarray (containing at
// least one number) which has the largest sum, and return that sum.
//
// Example: nums = [-2,1,-3,4,-1,2,1,-5,4]  ->  6
//          (subarray [4,-1,2,1] has the largest sum, 6)
//
// APPROACH — Kadane's Algorithm (vanilla, sum-based)
// ---------------------------------------------------
// This is the textbook, purest form of Kadane's Algorithm: track two running
// numbers while scanning left to right.
//
//   current_sum -- the best sum of a subarray ENDING exactly at index i.
//                  At each step, either EXTEND the previous run
//                  (current_sum + nums[i]) or ABANDON it and RESTART at
//                  nums[i] alone -- take whichever is larger.
//   best_sum     -- the best sum seen at ANY index scanned so far. Updated
//                  every iteration; never resets.
//
// Both are initialized from nums[0], NOT from 0 -- this is what correctly
// handles an all-negative array, where the answer must be the single
// least-negative element, not 0 (see README.md, "Common Mistakes").
//
// COMPLEXITY
// ----------
// Time:  O(n) -- one pass, O(1) work per element.
// Space: O(1) -- two running scalars, no auxiliary storage.
//
// Contrast with brute force (check every contiguous subarray): O(n^2) with a
// running inner sum, or O(n^3) re-summing each subarray from scratch. Kadane's
// collapses both down to O(n) by making the extend-vs-restart decision purely
// from current_sum -- never re-scanning any prefix.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int maxSubArray(const std::vector<int>& nums) {
  int current_sum = nums[0];
  int best_sum = nums[0];

  for (size_t i = 1; i < nums.size(); ++i) {
    // Extend the previous run, or restart at nums[i] alone.
    current_sum = std::max(nums[i], current_sum + nums[i]);
    // best_sum is checked every iteration, not just at a restart.
    best_sum = std::max(best_sum, current_sum);
  }

  return best_sum;
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
    std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    check(maxSubArray(nums) == 6, "[-2,1,-3,4,-1,2,1,-5,4] -> 6");
  }
  {
    std::vector<int> nums = {1};
    check(maxSubArray(nums) == 1, "[1] -> 1 (single element)");
  }
  {
    std::vector<int> nums = {5, 4, -1, 7, 8};
    check(maxSubArray(nums) == 23, "[5,4,-1,7,8] -> 23 (whole array)");
  }
  {
    // All-negative edge case: answer must be the least-negative element, not 0.
    std::vector<int> nums = {-3, -1, -2, -5};
    check(maxSubArray(nums) == -1, "[-3,-1,-2,-5] -> -1 (least-negative, NOT 0)");
  }
  {
    std::vector<int> nums = {-1};
    check(maxSubArray(nums) == -1, "[-1] -> -1");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
