// ============================================================================
// LeetCode 673 — Number of Longest Increasing Subsequence
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums`, return the NUMBER of distinct longest
// strictly increasing subsequences (counting subsequences by the indices
// they use, not just by their resulting value sequence).
//
// Example: nums = [1,3,5,4,7] -> 2
//          (the two longest increasing subsequences, both length 4, are
//           [1,3,4,7] and [1,3,5,7])
//
// APPROACH — O(n^2) DP, tracking counts alongside lengths
// ---------------------------------------------------------
// This is the O(n^2) LIS recurrence from the README, EXTENDED to also
// track how many ways each length is achieved — the reason this problem is
// rated "hard" despite reusing the exact same dp[i] recurrence as LeetCode
// 300.
//
// Two parallel arrays:
//   length[i] = length of the longest increasing subsequence ending at i
//               (same meaning as dp[i] in the O(n^2) LIS solution).
//   count[i]  = NUMBER OF DISTINCT WAYS to achieve that best length ending
//               at i.
//
// Both start at 1 for every i (every element is trivially a subsequence of
// length 1, achieved in exactly 1 way — itself).
//
// For each i, scan every earlier j < i with nums[j] < nums[i]:
//   - if length[j] + 1 > length[i]: we just found a STRICTLY LONGER way to
//     end at i via j. Adopt it: length[i] = length[j] + 1, and since this
//     is a brand-new best length, the count resets to whatever count[j]
//     was (every way of achieving length[j] at j becomes a new distinct
//     way of achieving length[i] at i by appending nums[i]).
//   - else if length[j] + 1 == length[i]: j offers ANOTHER way to achieve
//     the SAME best length already known at i (a tie, not an improvement).
//     Add count[j] to count[i] — every distinct way of reaching length[j]
//     at j is a distinct way of reaching length[i] at i.
//   - else (length[j] + 1 < length[i]): j is irrelevant — it cannot
//     contribute to the best-known length ending at i, so it is ignored.
//
// After filling both arrays, find max_length = max(length[i]) over all i,
// then sum count[i] for every i where length[i] == max_length — because the
// overall longest subsequence can legitimately END at more than one index.
//
// COMPLEXITY
// ----------
// Time:  O(n^2) — same nested loop as the plain LIS length DP; counting
//        adds only O(1) extra work per (i, j) pair.
// Space: O(n) — the two parallel arrays.
//
// (An O(n log n) solution exists using Binary Indexed Trees / segment
// trees over compressed values, but it is materially more complex; the
// O(n^2) DP is the standard interview-level answer and is what is shown
// here.)
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int findNumberOfLIS(const std::vector<int>& nums) {
  const size_t n = nums.size();
  if (n == 0) return 0;

  std::vector<int> length(n, 1);
  std::vector<int> count(n, 1);

  int max_length = 1;

  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (nums[j] < nums[i]) {
        if (length[j] + 1 > length[i]) {
          // Strictly longer subsequence ending at i found via j: adopt its
          // length, and inherit ALL of j's ways as i's ways (a fresh count).
          length[i] = length[j] + 1;
          count[i] = count[j];
        } else if (length[j] + 1 == length[i]) {
          // Another, equally-long way to reach the same best length at i.
          count[i] += count[j];
        }
        // else: j cannot beat the best length already recorded at i, skip.
      }
    }
    max_length = std::max(max_length, length[i]);
  }

  int total = 0;
  for (size_t i = 0; i < n; ++i) {
    if (length[i] == max_length) {
      total += count[i];
    }
  }
  return total;
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](const std::vector<int>& nums, int expected, const std::string& label) {
    int result = findNumberOfLIS(nums);
    if (result == expected) {
      std::cout << "[PASS] " << label << " -> " << result << "\n";
      ++pass_count;
    } else {
      std::cout << "[FAIL] " << label << " -> got " << result
                << ", expected " << expected << "\n";
      ++fail_count;
    }
  };

  check({1, 3, 5, 4, 7}, 2, "[1,3,5,4,7] -> 2 ([1,3,4,7] and [1,3,5,7])");
  check({2, 2, 2, 2, 2}, 5, "all equal -> every element is its own LIS of length 1, 5 ways");
  check({1, 2, 4, 3, 5, 4, 7, 2}, 3, "[1,2,4,3,5,4,7,2] -> 3");
  check({1}, 1, "single element -> 1");
  check({1, 1, 1, 2, 2, 2, 3, 3, 3}, 27,
        "[1,1,1,2,2,2,3,3,3] -> 3 choices per level, 3 levels -> 27");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
