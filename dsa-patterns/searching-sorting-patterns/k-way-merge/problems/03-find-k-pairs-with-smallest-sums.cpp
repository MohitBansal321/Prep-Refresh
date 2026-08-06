// ============================================================================
// LeetCode 373 — Find K Pairs with Smallest Sums (Medium)
// ============================================================================
//
// PROBLEM
// -------
// Given two integer arrays `nums1` and `nums2`, both sorted in ascending
// order, and an integer k, return the k pairs (u, v) with u from nums1 and
// v from nums2, that have the smallest sums u + v.
//
// Example: nums1 = [1,7,11], nums2 = [2,4,6], k = 3
//          -> [[1,2],[1,4],[1,6]]
//
// APPROACH — K-way Merge in disguise (one implicit sorted "list" per nums1 element)
// -----------------------------------------------------------------------------
// The recognition step here is less obvious than problem 02's row-sorted
// matrix, but the same shape is hiding underneath. Fix an index i into
// nums1. Because nums2 is sorted ascending, the sequence of sums
//     nums1[i] + nums2[0], nums1[i] + nums2[1], nums1[i] + nums2[2], ...
// is ITSELF sorted ascending (adding a fixed constant nums1[i] preserves
// order). So each index i of nums1 defines its own implicit sorted "list"
// of candidate sums — exactly the K-way Merge setup, with K = len(nums1)
// "lists", except no list is ever materialized in memory; each one is
// generated on demand from (i, j) via nums1[i] + nums2[j].
//
// We seed a min-heap with one entry per i (only as many i's as we could
// possibly need — min(len(nums1), k), since we will never need more than
// k pairs' worth of starting candidates), each starting at j = 0: the pair
// (nums1[i], nums2[0]). We pop the smallest sum k times; each pop for a
// given i is immediately followed by pushing that same i's next candidate,
// (nums1[i], nums2[j+1]) — the "advance the source pointer" step, here
// advancing j while i (which list) stays fixed.
//
// Heap entries: {sum, i, j}. This mirrors ../code.cpp's HeapEntry
// {value, list_index, element_index} exactly, with list_index = i (which
// nums1 element defines this implicit list) and element_index = j (how far
// into nums2 we've advanced for that i).
//
// COMPLEXITY
// ----------
// Let m = len(nums1), n = len(nums2), and K = min(m, k) (we never seed more
// starting candidates than we could possibly need).
// Time:  O(k log K) — at most k pops (early exit once we have k pairs),
//        each an O(log K) heap operation since the heap never holds more
//        than K entries.
// Space: O(K) for the heap, plus O(k) for the output list of pairs.
//
// Contrast with brute force (generate all m*n pairs, sort by sum, take the
// first k): O(m*n log(m*n)) time and O(m*n) space — wasteful when k is much
// smaller than m*n, and it ignores that each i's row of sums is already
// sorted by construction.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

using HeapEntry = std::tuple<int, int, int>;  // {sum, i (nums1 index), j (nums2 index)}
using MinHeap = std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<>>;

std::vector<std::vector<int>> kSmallestPairs(const std::vector<int>& nums1,
                                              const std::vector<int>& nums2,
                                              int k) {
  std::vector<std::vector<int>> result;
  if (nums1.empty() || nums2.empty() || k <= 0) return result;

  MinHeap heap;

  // Seed: one entry per i, starting at j = 0. We never need more starting
  // candidates than min(len(nums1), k) -- if k is smaller than len(nums1),
  // some nums1 elements are guaranteed to never contribute a pair to the
  // final top-k, since nums1[i] + nums2[0] alone (its best possible pair)
  // could not possibly be needed once k smaller sums from other i's exist.
  // Seeding all of them anyway is also correct, just seeds a slightly
  // larger (though still bounded) heap; we cap it here purely to keep the
  // heap size tight.
  const int seed_count = std::min(static_cast<int>(nums1.size()), k);
  for (int i = 0; i < seed_count; ++i) {
    heap.emplace(nums1[i] + nums2[0], i, 0);
  }

  while (!heap.empty() && static_cast<int>(result.size()) < k) {
    auto [sum, i, j] = heap.top();
    heap.pop();
    (void)sum;  // The actual sum isn't needed in the output, only the pair.

    result.push_back({nums1[i], nums2[j]});

    // Advance the source pointer: this same i's next candidate is one
    // step further into nums2.
    const int next_j = j + 1;
    if (next_j < static_cast<int>(nums2.size())) {
      heap.emplace(nums1[i] + nums2[next_j], i, next_j);
    }
  }

  return result;
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

  // ---- Test 1: the classic LeetCode example -----------------------------------
  {
    std::vector<int> nums1 = {1, 7, 11};
    std::vector<int> nums2 = {2, 4, 6};
    auto result = kSmallestPairs(nums1, nums2, 3);
    std::vector<std::vector<int>> expected = {{1, 2}, {1, 4}, {1, 6}};
    check(result == expected, "nums1=[1,7,11], nums2=[2,4,6], k=3 -> [[1,2],[1,4],[1,6]]");
  }

  // ---- Test 2: k larger than total possible pairs -----------------------------
  {
    std::vector<int> nums1 = {1, 2};
    std::vector<int> nums2 = {3};
    auto result = kSmallestPairs(nums1, nums2, 10);
    // Only 2*1 = 2 pairs exist total: [1,3] and [2,3].
    std::vector<std::vector<int>> expected = {{1, 3}, {2, 3}};
    check(result == expected, "k larger than total pairs -> returns all available pairs, sorted");
  }

  // ---- Test 3: k = 1 -----------------------------------------------------------
  {
    std::vector<int> nums1 = {1, 1, 2};
    std::vector<int> nums2 = {1, 2, 3};
    auto result = kSmallestPairs(nums1, nums2, 1);
    std::vector<std::vector<int>> expected = {{1, 1}};
    check(result == expected, "k=1 returns the single smallest-sum pair");
  }

  // ---- Test 4: one input array is empty ----------------------------------------
  {
    std::vector<int> nums1 = {};
    std::vector<int> nums2 = {1, 2, 3};
    auto result = kSmallestPairs(nums1, nums2, 3);
    check(result.empty(), "empty nums1 -> no pairs possible");
  }

  // ---- Test 5: duplicate values in both arrays ----------------------------------
  {
    std::vector<int> nums1 = {1, 1};
    std::vector<int> nums2 = {1, 1};
    auto result = kSmallestPairs(nums1, nums2, 4);
    std::vector<std::vector<int>> expected = {{1, 1}, {1, 1}, {1, 1}, {1, 1}};
    check(result == expected, "all-duplicate inputs produce all-duplicate pairs, correct count");
  }

  // ---- Test 6: larger arrays, k somewhere in the middle -------------------------
  {
    std::vector<int> nums1 = {1, 2, 3};
    std::vector<int> nums2 = {1, 2, 3};
    auto result = kSmallestPairs(nums1, nums2, 5);
    // All sums: (1,1)=2 (1,2)=3 (1,3)=4 (2,1)=3 (2,2)=4 (2,3)=5 (3,1)=4 (3,2)=5 (3,3)=6
    // Sorted by sum (ties broken by heap order, but sum sequence must match):
    // 2,3,3,4,4,4,5,5,6 -> first 5 sums: 2,3,3,4,4
    std::vector<int> expected_sums = {2, 3, 3, 4, 4};
    std::vector<int> actual_sums;
    for (const auto& pair : result) actual_sums.push_back(pair[0] + pair[1]);
    check(actual_sums == expected_sums, "k=5 pairs have the correct non-decreasing sum sequence");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
