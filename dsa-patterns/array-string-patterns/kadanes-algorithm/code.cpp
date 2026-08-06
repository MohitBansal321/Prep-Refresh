// ============================================================================
// Kadane's Algorithm — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// core shape of Kadane's Algorithm you will re-derive on almost every problem
// that fits this pattern: track the best sum ENDING HERE (current_sum) and
// the best sum ANYWHERE SO FAR (best_sum), extending or restarting the
// current run at every index based on a single comparison.
//
// The worked, problem-specific solutions (including the product and circular
// variants, which need real adaptation) live in problems/*.cpp.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Result bundle: the maximum sum AND the inclusive [start_index, end_index]
// of the winning subarray. Most real uses of this pattern need to know WHERE
// the best run is, not just its magnitude (see README.md, "Implementation").
// ----------------------------------------------------------------------------
struct KadaneResult {
  long long max_sum;
  size_t start_index;
  size_t end_index;
};

// ----------------------------------------------------------------------------
// maxSubarraySum — the generic, reusable core of Kadane's Algorithm.
//
// Returns the maximum sum of any contiguous, non-empty subarray of `nums`,
// along with the inclusive index range [start_index, end_index] that
// achieves it.
//
// The algorithm keeps two running numbers as it scans left to right:
//   - current_sum: the best sum of a subarray ENDING exactly at the current
//     index. At each step it either EXTENDS the previous run (add nums[i])
//     or ABANDONS it and RESTARTS at nums[i] alone -- whichever is larger.
//   - best_sum: the best sum seen at ANY index scanned so far. It never
//     resets; it only ever grows (or stays the same).
//
// Both current_sum and best_sum are initialized from nums[0] (never from 0)
// so that an all-negative array correctly returns its single least-negative
// element instead of an incorrect 0 -- see README.md, "Common Mistakes".
// ----------------------------------------------------------------------------
KadaneResult maxSubarraySum(std::vector<int>& nums) {
  if (nums.empty()) {
    // "The maximum sum subarray of nothing" is not a well-defined question.
    // Silently returning 0 here would be indistinguishable from a legitimate
    // all-negative-array answer of 0 -- reject it explicitly instead.
    throw std::invalid_argument("maxSubarraySum: input array must not be empty");
  }

  long long current_sum = nums[0];
  long long best_sum = nums[0];

  size_t current_start = 0;
  size_t best_start = 0;
  size_t best_end = 0;

  for (size_t i = 1; i < nums.size(); ++i) {
    // Extend the previous run, or abandon it and restart at nums[i] alone.
    // Restarting is better exactly when the run carried into this index was
    // negative -- a negative prefix can only subtract from what follows.
    if (current_sum + nums[i] >= nums[i]) {
      current_sum += nums[i];
    } else {
      current_sum = nums[i];
      current_start = i;
    }

    // best_sum is checked (and possibly updated) on EVERY iteration, not
    // just when a restart happens -- the best run can occur mid-run.
    if (current_sum > best_sum) {
      best_sum = current_sum;
      best_start = current_start;
      best_end = i;
    }
  }

  return KadaneResult{best_sum, best_start, best_end};
}

// ============================================================================
// main() -- demonstrates maxSubarraySum with printed, verifiable output.
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

  std::cout << "--- Typical mixed-sign array ---\n";
  {
    std::vector<int> nums = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    auto result = maxSubarraySum(nums);
    std::cout << "  max_sum=" << result.max_sum << " range=[" << result.start_index
              << ", " << result.end_index << "] (expected sum=6, range=[3, 6])\n";
    check(result.max_sum == 6 && result.start_index == 3 && result.end_index == 6,
          "[-2,1,-3,4,-1,2,1,-5,4] -> sum 6, subarray [4,-1,2,1]");
  }

  std::cout << "\n--- All-positive array (answer is the whole array) ---\n";
  {
    std::vector<int> nums = {1, 2, 3, 4, 5};
    auto result = maxSubarraySum(nums);
    std::cout << "  max_sum=" << result.max_sum << " range=[" << result.start_index
              << ", " << result.end_index << "] (expected sum=15, range=[0, 4])\n";
    check(result.max_sum == 15 && result.start_index == 0 && result.end_index == 4,
          "[1,2,3,4,5] -> sum 15, whole array");
  }

  std::cout << "\n--- All-negative array (edge case: must NOT return 0) ---\n";
  {
    std::vector<int> nums = {-5, -3, -8, -1, -9};
    auto result = maxSubarraySum(nums);
    std::cout << "  max_sum=" << result.max_sum << " range=[" << result.start_index
              << ", " << result.end_index << "] (expected sum=-1, range=[3, 3])\n";
    check(result.max_sum == -1 && result.start_index == 3 && result.end_index == 3,
          "[-5,-3,-8,-1,-9] -> sum -1 (least-negative single element), NOT 0");
  }

  std::cout << "\n--- Single-element array ---\n";
  {
    std::vector<int> nums = {7};
    auto result = maxSubarraySum(nums);
    check(result.max_sum == 7 && result.start_index == 0 && result.end_index == 0,
          "[7] -> sum 7, range [0, 0]");
  }

  std::cout << "\n--- Best subarray sits in the middle ---\n";
  {
    std::vector<int> nums = {-1, -2, 5, 6, -1, 7, -20};
    auto result = maxSubarraySum(nums);
    std::cout << "  max_sum=" << result.max_sum << " range=[" << result.start_index
              << ", " << result.end_index << "] (expected sum=17, range=[2, 5])\n";
    check(result.max_sum == 17 && result.start_index == 2 && result.end_index == 5,
          "[-1,-2,5,6,-1,7,-20] -> sum 17, subarray [5,6,-1,7]");
  }

  std::cout << "\n--- Empty array must throw ---\n";
  {
    std::vector<int> nums;
    bool threw = false;
    try {
      maxSubarraySum(nums);
    } catch (const std::invalid_argument&) {
      threw = true;
    }
    check(threw, "empty array -> throws std::invalid_argument");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
