// ============================================================================
// LeetCode 34 — Find First and Last Position of Element in Sorted Array
// ============================================================================
//
// Problem: Given a sorted array `nums` (ascending) that may contain
// duplicates, and a `target`, return [firstIndex, lastIndex] of `target`'s
// occurrences, or [-1, -1] if it does not appear. Must run in O(log n) time.
//
// Approach (ties back to Modified Binary Search's "boundary-finding"
// generalization): a plain binary search only proves EXISTENCE — it can
// land on any one of several equal elements and stop. Here we need the
// EDGES of a run of equal values, so we run two independent binary searches,
// each with a modified halving rule:
//
//   - Searching for the FIRST occurrence: on nums[mid] == target, do not
//     stop — record mid as a candidate, then keep narrowing into the LEFT
//     half (hi = mid - 1) to see if an earlier occurrence exists.
//   - Searching for the LAST occurrence: symmetric — on a match, record mid
//     and keep narrowing into the RIGHT half (lo = mid + 1).
//
// Both searches still discard exactly one guaranteed-irrelevant half per
// iteration, so each individually remains O(log n); doing it twice is still
// O(log n) overall (two constants, not two orders of growth).
//
// Complexity: O(log n) time (two binary searches), O(1) space.
// ============================================================================

#include <iostream>
#include <vector>

int findFirst(const std::vector<int>& nums, int target) {
  int lo = 0;
  int hi = static_cast<int>(nums.size()) - 1;
  int result = -1;

  while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;
    if (nums[mid] == target) {
      result = mid;
      hi = mid - 1;  // keep looking left for an earlier occurrence
    } else if (nums[mid] < target) {
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }
  return result;
}

int findLast(const std::vector<int>& nums, int target) {
  int lo = 0;
  int hi = static_cast<int>(nums.size()) - 1;
  int result = -1;

  while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;
    if (nums[mid] == target) {
      result = mid;
      lo = mid + 1;  // keep looking right for a later occurrence
    } else if (nums[mid] < target) {
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }
  return result;
}

std::vector<int> searchRange(const std::vector<int>& nums, int target) {
  return {findFirst(nums, target), findLast(nums, target)};
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
    auto r = searchRange({5, 7, 7, 8, 8, 10}, 8);
    check(r[0] == 3 && r[1] == 4, "target 8 -> [3, 4]");
  }
  {
    auto r = searchRange({5, 7, 7, 8, 8, 10}, 6);
    check(r[0] == -1 && r[1] == -1, "target 6 absent -> [-1, -1]");
  }
  {
    auto r = searchRange({}, 0);
    check(r[0] == -1 && r[1] == -1, "empty array -> [-1, -1]");
  }
  {
    auto r = searchRange({2, 2, 2, 2, 2}, 2);
    check(r[0] == 0 && r[1] == 4, "all elements equal target -> [0, 4]");
  }
  {
    auto r = searchRange({1, 3}, 1);
    check(r[0] == 0 && r[1] == 0, "single occurrence -> [0, 0]");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
