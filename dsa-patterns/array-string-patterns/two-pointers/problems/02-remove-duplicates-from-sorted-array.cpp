// ============================================================================
// LeetCode 26 — Remove Duplicates from Sorted Array
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums` sorted in non-decreasing order, remove the
// duplicates IN PLACE so that each unique element appears only once. The
// relative order of the elements should stay the same. Return `k`, the
// number of unique elements, after placing them at the front of `nums`
// (the "in-place" contract: nums[0..k-1] must hold the unique elements in
// order; anything past index k-1 is ignored by the grader).
//
// Example: nums = [0,0,1,1,1,2,2,3,3,4]  ->  k = 5, nums[0..4] = [0,1,2,3,4]
//
// APPROACH — Two Pointers (same-direction: slow "write" + fast "read")
// -----------------------------------------------------------------------
// This is the other flavor of Two Pointers: both pointers start at the
// beginning and move in the SAME direction, at different rates, instead of
// converging from opposite ends. The recognition signal here is different
// from problem 01: we are not searching for a pair, we need IN-PLACE
// COMPACTION of a sorted array under a "no extra array" space constraint.
//
//   - `write` (slow pointer) marks the boundary of the unique prefix built
//     so far — everything at index < write is final and already
//     deduplicated.
//   - `read` (fast pointer) scans every element exactly once, left to right.
//
// Because the input is SORTED, duplicates of any value are always
// contiguous. That means "is nums[read] a duplicate?" reduces to a single
// O(1) comparison: nums[read] == nums[write - 1]. If it's NOT a duplicate,
// copy it into nums[write] and advance write. If it IS a duplicate, skip it
// (read simply keeps moving; write stays put).
//
// Why this works: `write` never moves ahead of `read`, so we never
// overwrite a value before we have read it. Every element is read exactly
// once. The sortedness guarantee is what makes the "compare to the
// immediately preceding written value" check sufficient — on an UNSORTED
// array this single comparison would miss duplicates that are far apart
// (see ../README.md Common Mistakes: "using Two Pointers on unsorted data").
//
// COMPLEXITY
// ----------
// Time:  O(n) — `read` visits every element exactly once.
// Space: O(1) — no auxiliary array; compaction happens in place.
//
// Contrast with brute force (for each element, scan ahead to check for and
// erase duplicates, e.g. via repeated std::vector::erase): O(n^2) time,
// because each erase() shifts all subsequent elements down by one.
// Contrast with "copy unique values into a new array via a hash set":
// O(n) time but O(n) extra space — two pointers gets O(n) time in O(1)
// space by exploiting sortedness instead of hashing.
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// Removes duplicates in place, keeping at most one copy of each value.
// Returns k, the number of unique elements now occupying nums[0..k-1].
int removeDuplicates(std::vector<int>& nums) {
  if (nums.empty()) return 0;

  int write = 1;  // nums[0] is trivially unique; the written prefix starts at 1.

  for (size_t read = 1; read < nums.size(); ++read) {
    if (nums[read] != nums[write - 1]) {
      // Not a duplicate of the last kept value -> keep it.
      nums[write] = nums[read];
      ++write;
    }
    // Else: nums[read] duplicates nums[write-1]; skip it, `write` stays put.
  }

  return write;
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
    std::vector<int> nums = {1, 1, 2};
    int k = removeDuplicates(nums);
    std::vector<int> prefix(nums.begin(), nums.begin() + k);
    check(k == 2 && prefix == std::vector<int>({1, 2}),
          "[1,1,2] -> k=2, prefix=[1,2]");
  }

  {
    std::vector<int> nums = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
    int k = removeDuplicates(nums);
    std::vector<int> prefix(nums.begin(), nums.begin() + k);
    check(k == 5 && prefix == std::vector<int>({0, 1, 2, 3, 4}),
          "[0,0,1,1,1,2,2,3,3,4] -> k=5, prefix=[0,1,2,3,4]");
  }

  {
    std::vector<int> nums = {1};
    int k = removeDuplicates(nums);
    std::vector<int> prefix(nums.begin(), nums.begin() + k);
    check(k == 1 && prefix == std::vector<int>({1}), "single element [1] -> k=1, prefix=[1]");
  }

  {
    std::vector<int> nums = {7, 7, 7, 7, 7};
    int k = removeDuplicates(nums);
    std::vector<int> prefix(nums.begin(), nums.begin() + k);
    check(k == 1 && prefix == std::vector<int>({7}), "all duplicates [7,7,7,7,7] -> k=1, prefix=[7]");
  }

  {
    std::vector<int> nums = {};
    int k = removeDuplicates(nums);
    check(k == 0, "empty array -> k=0");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
