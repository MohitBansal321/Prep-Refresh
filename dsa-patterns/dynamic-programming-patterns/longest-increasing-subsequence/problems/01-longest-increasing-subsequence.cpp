// ============================================================================
// LeetCode 300 — Longest Increasing Subsequence
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums`, return the length of the longest STRICTLY
// increasing subsequence. A subsequence is derived by deleting zero or more
// elements without changing the order of the remaining elements — the kept
// elements do NOT need to be contiguous.
//
// Example: nums = [10,9,2,5,3,7,101,18] -> 4
//          (the subsequence [2,3,7,101] is one longest increasing one)
//
// APPROACH — patience sorting (O(n log n))
// -----------------------------------------
// This is the pattern in its purest, textbook form — the exact problem the
// pattern module is named after. We maintain a `tails` array where
// `tails[k]` is the smallest tail value seen so far among all increasing
// subsequences of length k+1. For each new number x:
//   - binary search `tails` for the first entry >= x (std::lower_bound,
//     because we need STRICT increase — see the README's Common Mistakes
//     section on lower_bound vs upper_bound);
//   - if every entry is smaller than x, x extends the longest run found so
//     far: append it, growing tails.size() by one;
//   - otherwise, x can replace that entry with an equal-or-smaller value
//     without breaking anything, which can only make future extensions
//     easier (a smaller tail is never harder to beat than a larger one).
//
// tails.size() at the end IS the answer. Note `tails` is a bookkeeping
// array, not an actual valid subsequence of `nums` — see the README's
// Disadvantages section for why reconstructing the real subsequence needs
// extra parent-pointer tracking that this file deliberately omits (the
// problem only asks for the length).
//
// COMPLEXITY
// ----------
// Time:  O(n log n) — one binary search (O(log n)) per element (n of them).
// Space: O(n) worst case for `tails` (a strictly increasing input keeps
//        every element).
//
// Contrast with the O(n^2) DP (see ../code.cpp's lengthOfLIS_On2): same
// answer, but this is a genuine algorithmic upgrade (better big-O), not
// just a constant-factor speedup.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int lengthOfLIS(const std::vector<int>& nums) {
  std::vector<int> tails;
  tails.reserve(nums.size());

  for (int x : nums) {
    auto it = std::lower_bound(tails.begin(), tails.end(), x);
    if (it == tails.end()) {
      tails.push_back(x);
    } else {
      *it = x;
    }
  }

  return static_cast<int>(tails.size());
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](const std::vector<int>& nums, int expected, const std::string& label) {
    int result = lengthOfLIS(nums);
    if (result == expected) {
      std::cout << "[PASS] " << label << " -> " << result << "\n";
      ++pass_count;
    } else {
      std::cout << "[FAIL] " << label << " -> got " << result
                << ", expected " << expected << "\n";
      ++fail_count;
    }
  };

  check({10, 9, 2, 5, 3, 7, 101, 18}, 4, "classic example");
  check({0, 1, 0, 3, 2, 3}, 4, "[0,1,0,3,2,3]");
  check({7, 7, 7, 7, 7, 7, 7}, 1, "all equal (strict increase only)");
  check({1}, 1, "single element");
  check({}, 0, "empty array");
  check({4, 10, 4, 3, 8, 9}, 3, "[4,10,4,3,8,9]");
  check({1, 3, 6, 7, 9, 4, 10, 5, 6}, 6, "[1,3,6,7,9,4,10,5,6]");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
