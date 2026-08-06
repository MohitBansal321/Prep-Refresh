// ============================================================================
// LeetCode 153 — Find Minimum in Rotated Sorted Array
// ============================================================================
//
// Problem: An ascending array with distinct values was rotated at some
// unknown pivot. Return the minimum element. Must run in O(log n) time.
//
// Approach (ties back to Modified Binary Search's "boundary between two
// monotonic regions" idea): the rotation point IS the minimum — it is the
// single index where "the sequence stops being smaller than what came
// before, relative to the un-rotated order" — concretely, it is the only
// index whose value is smaller than the array's very last element (every
// element in the "wrapped around" prefix is larger than the last element,
// and everything from the rotation point onward, including the last
// element, is ascending).
//
// The modified halving test: compare nums[mid] to nums[hi].
//   - If nums[mid] > nums[hi], the minimum cannot be in [lo..mid] (mid is
//     part of the "large prefix" before the wrap), so it must be to the
//     right — discard the left half, but keep mid itself in play by setting
//     lo = mid + 1 (mid is proven NOT the minimum, so excluding it is safe).
//   - If nums[mid] <= nums[hi], the segment [mid..hi] is already ascending
//     (no rotation point inside it), so the minimum is at mid or to its
//     left — discard the right half by setting hi = mid (mid MIGHT be the
//     minimum, so it must stay in the range, unlike a normal <= comparison).
//
// The loop ends when lo == hi, which is the minimum's index. Comparing
// against nums[hi] (not nums[lo]) is what makes the two-way branch
// sufficient here — no three-way comparison or exact-match check needed,
// since we are hunting for a boundary, not a value.
//
// Complexity: O(log n) time — one half is discarded every iteration.
// O(1) space.
// ============================================================================

#include <iostream>
#include <vector>

int findMin(const std::vector<int>& nums) {
  int lo = 0;
  int hi = static_cast<int>(nums.size()) - 1;

  while (lo < hi) {
    int mid = lo + (hi - lo) / 2;

    if (nums[mid] > nums[hi]) {
      // mid is in the "large prefix" before the rotation point: the minimum
      // is strictly to the right, and mid itself is provably not it.
      lo = mid + 1;
    } else {
      // [mid..hi] is already ascending: the minimum is at mid or to its
      // left, so mid must stay in the candidate range.
      hi = mid;
    }
  }

  return nums[lo];  // lo == hi: the boundary, i.e. the minimum's index.
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

  check(findMin({3, 4, 5, 1, 2}) == 1, "classic rotated example -> 1");
  check(findMin({4, 5, 6, 7, 0, 1, 2}) == 0, "rotation point mid-array -> 0");
  check(findMin({11, 13, 15, 17}) == 11, "zero rotation (already sorted) -> 11");
  check(findMin({1}) == 1, "single element -> 1");
  check(findMin({2, 1}) == 1, "two elements, rotated -> 1");
  check(findMin({1, 2}) == 1, "two elements, not rotated -> 1");
  check(findMin({5, 1, 2, 3, 4}) == 1, "rotation point near start -> 1");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
