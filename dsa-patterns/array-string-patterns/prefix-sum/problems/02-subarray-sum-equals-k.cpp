// ============================================================================
// LeetCode 560 - Subarray Sum Equals K
// ============================================================================
// Problem: given an integer array nums and an integer k, return the total
// number of contiguous subarrays whose sum equals k.
//
// Approach (Prefix Sum + hash map of frequencies): maintain a running prefix
// sum as we scan once, left to right. For the current running sum `curr`, a
// subarray ending here sums to k exactly when some earlier prefix sum equals
// `curr - k` (rearranging prefixSum[j] - prefixSum[i] == k). A hash map
// tracks how many times each prefix-sum value has been seen so far, so at
// each step we add seen[curr - k] to the running answer, then record `curr`
// itself. Seeding the map with {0: 1} accounts for subarrays that start at
// index 0 (an "empty prefix" seen once, before the array begins).
//
// Complexity: O(n) time (single pass), O(n) space (hash map can hold up to
// n+1 distinct prefix-sum values).
// ============================================================================

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int subarraySum(const std::vector<int>& nums, int k) {
  std::unordered_map<long long, int> seen;
  seen[0] = 1;  // empty prefix (before the array starts), seen once

  long long curr = 0;
  int count = 0;

  for (int x : nums) {
    curr += x;
    // If some earlier prefix sum equals curr - k, every subarray from just
    // after that earlier prefix through the current index sums to exactly k.
    auto it = seen.find(curr - k);
    if (it != seen.end()) {
      count += it->second;
    }
    ++seen[curr];
  }

  return count;
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

  check(subarraySum({1, 1, 1}, 2) == 2, "[1,1,1], k=2 -> 2");
  check(subarraySum({1, 2, 3}, 3) == 2, "[1,2,3], k=3 -> 2");
  check(subarraySum({1, -1, 0}, 0) == 3, "[1,-1,0], k=0 -> 3");
  check(subarraySum({0, 0, 0, 0, 0}, 0) == 15, "[0,0,0,0,0], k=0 -> 15");
  check(subarraySum({-1, -1, 1}, 0) == 1, "[-1,-1,1], k=0 -> 1");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
