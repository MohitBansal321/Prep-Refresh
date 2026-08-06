// ============================================================================
// LeetCode 704 — Binary Search
// ============================================================================
//
// Problem: Given an array `nums` sorted in ascending order, and a `target`,
// return the index of `target` if it exists, otherwise return -1. Must run
// in O(log n) time.
//
// Approach (ties back to Modified Binary Search's foundational skeleton):
// this is the textbook case the whole pattern generalizes from. Maintain a
// closed search range [lo, hi] that is guaranteed to contain `target` if it
// exists at all. At each step, look at the midpoint: if it IS the target,
// stop. If it is too small, the target (if present) must be to the right,
// so the entire left half including mid is provably irrelevant and gets
// discarded (lo = mid + 1). If it is too large, the symmetric argument
// discards the right half (hi = mid - 1). The loop ends when lo > hi,
// meaning the search range is empty and the target is not present.
//
// Complexity: O(log n) time — the range halves every iteration, so at most
// log2(n) iterations. O(1) space — only a few index variables, no recursion
// stack (this is written iteratively specifically to keep space O(1); a
// recursive version would cost O(log n) stack frames).
// ============================================================================

#include <iostream>
#include <vector>

int search(const std::vector<int>& nums, int target) {
  int lo = 0;
  int hi = static_cast<int>(nums.size()) - 1;

  while (lo <= hi) {
    // lo + (hi - lo) / 2 avoids signed overflow that (lo + hi) / 2 risks
    // when both indices are large (see README Common Mistakes).
    int mid = lo + (hi - lo) / 2;

    if (nums[mid] == target) {
      return mid;
    } else if (nums[mid] < target) {
      lo = mid + 1;  // target must be to the right, if it exists at all.
    } else {
      hi = mid - 1;  // target must be to the left, if it exists at all.
    }
  }

  return -1;  // lo > hi: search range is empty, target is absent.
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

  check(search({-1, 0, 3, 5, 9, 12}, 9) == 4, "target 9 present -> index 4");
  check(search({-1, 0, 3, 5, 9, 12}, 2) == -1, "target 2 absent -> -1");
  check(search({5}, 5) == 0, "single element, present -> index 0");
  check(search({5}, -5) == -1, "single element, absent -> -1");
  check(search({}, 1) == -1, "empty array -> -1");
  check(search({1, 2, 3, 4, 5}, 1) == 0, "target is first element -> index 0");
  check(search({1, 2, 3, 4, 5}, 5) == 4, "target is last element -> index 4");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
