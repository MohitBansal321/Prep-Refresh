// ============================================================================
// LeetCode 33 — Search in Rotated Sorted Array
// ============================================================================
//
// Problem: An ascending array with all-distinct values was rotated at some
// unknown pivot (e.g. [0,1,2,4,5,6,7] -> [4,5,6,7,0,1,2]). Given the rotated
// array and a `target`, return its index, or -1 if absent. Must run in
// O(log n) time.
//
// Approach (ties back to Modified Binary Search's "which half is valid?"
// generalization): the array as a whole is no longer sorted, so a plain
// binary search's assumption ("everything left of mid is smaller, everything
// right is larger") no longer holds globally. What DOES still hold: at every
// `mid`, at least ONE of the two halves [lo..mid] or [mid..hi] is internally
// sorted (has no rotation point inside it) — because there is only one
// rotation point in the whole array, so at most one half can contain it.
//
// The modified halving test becomes:
//   1. Identify which half is normally sorted by comparing nums[lo] to
//      nums[mid].
//   2. Check whether `target` falls within THAT half's value range.
//   3. If yes, the answer (if it exists) must be in the sorted half, so
//      discard the other half. If no, target cannot be in the sorted half
//      (its range is fully known and target isn't in it), so it must be in
//      the OTHER half — discard the sorted one instead.
//
// This is still "discard a provably-irrelevant half every iteration," just
// with a fancier test for which half that is.
//
// Complexity: O(log n) time — one guaranteed half is discarded every
// iteration regardless of where the rotation point is. O(1) space.
// ============================================================================

#include <iostream>
#include <vector>

int search(const std::vector<int>& nums, int target) {
  int lo = 0;
  int hi = static_cast<int>(nums.size()) - 1;

  while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;

    if (nums[mid] == target) {
      return mid;
    }

    if (nums[lo] <= nums[mid]) {
      // Left half [lo..mid] is normally sorted (no rotation point inside).
      if (nums[lo] <= target && target < nums[mid]) {
        hi = mid - 1;  // target's range matches the sorted left half.
      } else {
        lo = mid + 1;  // target must be in the (possibly rotated) right half.
      }
    } else {
      // Right half [mid..hi] must instead be the normally sorted one.
      if (nums[mid] < target && target <= nums[hi]) {
        lo = mid + 1;
      } else {
        hi = mid - 1;
      }
    }
  }

  return -1;
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

  check(search({4, 5, 6, 7, 0, 1, 2}, 0) == 4, "target 0 -> index 4");
  check(search({4, 5, 6, 7, 0, 1, 2}, 3) == -1, "target 3 absent -> -1");
  check(search({1}, 0) == -1, "single element, absent -> -1");
  check(search({1}, 1) == 0, "single element, present -> index 0");
  check(search({5, 1, 3}, 5) == 0, "target is the pivot/max -> index 0");
  check(search({1, 2, 3, 4, 5, 6, 7}, 6) == 5, "zero rotation still works -> index 5");
  check(search({6, 7, 1, 2, 3, 4, 5}, 1) == 2, "target just after rotation point -> index 2");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
