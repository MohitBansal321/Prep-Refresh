// ============================================================================
// LeetCode 15 — 3Sum
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums`, return all the triplets [nums[i], nums[j],
// nums[k]] such that i != j, i != k, j != k, and nums[i] + nums[j] +
// nums[k] == 0. The solution set must NOT contain duplicate triplets.
//
// Example: nums = [-1,0,1,2,-1,-4]
//          -> [[-1,-1,2], [-1,0,1]]
//
// APPROACH — Sort + fix one element + Two Pointers on the rest
// -----------------------------------------------------------------------
// 3Sum is the standard way Two Pointers gets tested one level up from a
// plain pair-sum problem: it reduces "find a TRIPLET summing to zero" to
// "for each candidate first element, find a PAIR summing to its negation" —
// which is exactly problem 01 (Two Sum II) run inside a loop.
//
// Steps:
//   1. Sort `nums` ascending. This costs O(n log n) but is what makes the
//      inner two-pointer search possible AND makes duplicate-skipping a
//      simple adjacent-element check.
//   2. Fix an index `i` from 0 to n-3 as the smallest element of the
//      triplet. We now need left/right (converging) pointers over the
//      remaining sorted subarray (i+1 .. n-1) to find a pair summing to
//      exactly `-nums[i]` — identical logic to Two Sum II.
//   3. While scanning with left/right: if the three-sum is 0, record the
//      triplet, then advance BOTH pointers inward — but first skip over any
//      further copies of the same value at left and at right, so we never
//      emit the same triplet twice.
//   4. Skip duplicate values for `i` itself (if nums[i] == nums[i-1], the
//      same triplet family was already fully explored in the previous
//      outer iteration).
//
// Why the duplicate-skipping is necessary and specific to this problem: the
// pair-sum search (problem 01) has a UNIQUE solution guaranteed, so
// duplicate handling never comes up. Here, multiple index combinations can
// legally reconstruct the same VALUE triplet (e.g. two different -1's in
// the array), and the problem asks for unique triplets by value, not by
// index. This is exactly the "forgetting to skip duplicates in 3Sum"
// mistake called out in ../README.md Common Mistakes.
//
// COMPLEXITY
// ----------
// Time:  O(n^2) — O(n log n) sort, then for each of the n choices of `i`,
//        an O(n) two-pointer scan over the rest of the array.
// Space: O(1) extra (excluding the output list and the space the sort uses,
//        typically O(log n) for introsort's recursion).
//
// Contrast with brute force (three nested loops trying every i, j, k):
// O(n^3) time. Two Pointers turns the innermost two loops into a single
// linear scan, dropping one full order of complexity: O(n^3) -> O(n^2).
// A hashing variant (fix i, then hash-based two-sum for the rest) also
// reaches O(n^2) time but needs a hash set per outer iteration and much
// fussier duplicate bookkeeping since nothing is sorted.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::vector<int>> threeSum(std::vector<int> nums) {
  std::vector<std::vector<int>> result;
  std::sort(nums.begin(), nums.end());
  const int n = static_cast<int>(nums.size());

  for (int i = 0; i < n - 2; ++i) {
    // Once the smallest element is positive, no triplet starting here (or
    // later) can sum to zero — everything to the right is >= nums[i] > 0.
    if (nums[i] > 0) break;

    // Skip duplicate anchors: if this value is the same as the previous
    // one, every triplet reachable from it was already found in the
    // previous outer iteration.
    if (i > 0 && nums[i] == nums[i - 1]) continue;

    int left = i + 1;
    int right = n - 1;
    int target = -nums[i];  // Need nums[left] + nums[right] == target.

    while (left < right) {
      int sum = nums[left] + nums[right];

      if (sum == target) {
        result.push_back({nums[i], nums[left], nums[right]});

        // Skip duplicate values on the left side so we don't re-emit the
        // same triplet with a different index pointing at an identical value.
        while (left < right && nums[left] == nums[left + 1]) ++left;
        // Symmetric skip on the right side.
        while (left < right && nums[right] == nums[right - 1]) --right;

        ++left;
        --right;
      } else if (sum < target) {
        ++left;   // Sum too small -> need a larger left value.
      } else {
        --right;  // Sum too large -> need a smaller right value.
      }
    }
  }

  return result;
}

// Order-insensitive comparison of two lists of triplets (each triplet's
// internal order is already fixed by construction — ascending — so we only
// need to compare the SET of triplets, ignoring the order they were found).
bool sameTripletSet(std::vector<std::vector<int>> a, std::vector<std::vector<int>> b) {
  std::sort(a.begin(), a.end());
  std::sort(b.begin(), b.end());
  return a == b;
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
    std::vector<int> nums = {-1, 0, 1, 2, -1, -4};
    auto result = threeSum(nums);
    std::vector<std::vector<int>> expected = {{-1, -1, 2}, {-1, 0, 1}};
    check(sameTripletSet(result, expected), "[-1,0,1,2,-1,-4] -> {[-1,-1,2],[-1,0,1]}");
  }

  {
    std::vector<int> nums = {0, 1, 1};
    auto result = threeSum(nums);
    check(result.empty(), "[0,1,1] -> no triplet sums to zero");
  }

  {
    std::vector<int> nums = {0, 0, 0};
    auto result = threeSum(nums);
    std::vector<std::vector<int>> expected = {{0, 0, 0}};
    check(sameTripletSet(result, expected), "[0,0,0] -> {[0,0,0]}");
  }

  {
    // Many duplicate values -> stresses the duplicate-skipping logic.
    std::vector<int> nums = {-2, 0, 0, 2, 2};
    auto result = threeSum(nums);
    std::vector<std::vector<int>> expected = {{-2, 0, 2}};
    check(sameTripletSet(result, expected), "[-2,0,0,2,2] -> {[-2,0,2]} (no repeats)");
  }

  {
    std::vector<int> nums = {1, 2, -2, -1};
    auto result = threeSum(nums);
    check(result.empty(), "[1,2,-2,-1] -> no valid triplet (fewer than 3 usable combos)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
