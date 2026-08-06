// ============================================================================
// LeetCode 525 - Contiguous Array
// ============================================================================
// Problem: given a binary array nums (containing only 0s and 1s), find the
// maximum length of a contiguous subarray with an equal number of 0s and 1s.
//
// Approach (Prefix Sum with a remapped alphabet): treat every 0 as -1 and
// every 1 as +1, then take a running prefix sum `curr`. A subarray (i, j]
// has an equal count of 0s and 1s exactly when its remapped sum is 0, i.e.
// prefixSum[j] == prefixSum[i] -- the SAME running total was seen at two
// different indices. So: track the FIRST index at which each running-total
// value was seen (seeding {0: -1} for "before the array starts"), and every
// time the current running total repeats a previously-seen value, the gap
// between those two indices is a candidate answer. Keep the maximum gap.
//
// Storing only the first occurrence of each running-total value (not every
// occurrence, and not overwriting with later ones) is what guarantees the
// LONGEST qualifying subarray is found, not merely a qualifying one -- the
// first time a value repeats, the gap back to its first occurrence is the
// widest possible gap for that value.
//
// Complexity: O(n) time (single pass), O(n) space (hash map of running
// totals to their first index).
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int findMaxLength(const std::vector<int>& nums) {
  // running total -> first index at which this running total occurred.
  // Seed with {0: -1}: a running total of 0 "at index -1" means the whole
  // prefix up to and including the current index is balanced.
  std::unordered_map<int, int> first_seen;
  first_seen[0] = -1;

  int curr = 0;
  int max_len = 0;

  for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
    curr += (nums[i] == 0) ? -1 : 1;

    auto it = first_seen.find(curr);
    if (it != first_seen.end()) {
      max_len = std::max(max_len, i - it->second);
    } else {
      first_seen[curr] = i;
    }
  }

  return max_len;
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

  check(findMaxLength({0, 1}) == 2, "[0,1] -> 2");
  check(findMaxLength({0, 1, 0}) == 2, "[0,1,0] -> 2");
  check(findMaxLength({0, 0, 1, 0, 0, 0, 1, 1}) == 6,
        "[0,0,1,0,0,0,1,1] -> 6");
  check(findMaxLength({1, 1, 1, 1}) == 0,
        "[1,1,1,1] -> 0 (no balance possible)");
  check(findMaxLength({0, 0, 1, 1}) == 4, "[0,0,1,1] -> 4");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
