// ============================================================================
// Modified Binary Search — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// core generalization that "modified binary search" is built on: classic
// binary search only answers "does this exact value exist, and where?" but
// the SAME halving mechanics answer a much bigger family of questions once
// you replace the exact-match test with a problem-specific predicate that
// tells you "which half of the search space is guaranteed to still contain
// what I'm looking for."
//
// Four functions, in increasing order of generalization:
//
//   1. binarySearch            — the textbook exact-match search. Establishes
//                                 the lo/hi/mid skeleton every variant reuses.
//   2. findFirstOccurrence     — first index of `target` in a sorted array
//                                 that may contain duplicates (a "boundary"
//                                 search: keep narrowing even after a match).
//   3. findLastOccurrence      — symmetric: last index of `target`.
//   4. searchRotated           — search in a sorted array that has been
//                                 rotated at an unknown pivot; the halving
//                                 rule changes from "compare to mid" to
//                                 "figure out which half is normally-ordered,
//                                 then check if target lives in that half."
//
// Every variant keeps the same three moving parts: `lo`, `hi`, `mid`, and a
// predicate that decides which half to discard. That predicate is the only
// thing that changes between problems — the loop shape is constant.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_mbs_code && /tmp/out_mbs_code
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// --- Portability shim -------------------------------------------------------
// std::optional is C++17, but libstdc++ only shipped <optional> in GCC 7. On
// GCC 6 the same facility lives in <experimental/optional>. Alias whichever is
// available so this file builds on both. Everything below uses `opt::optional`.
// The experimental version predates the member function has_value(), so the
// free function opt::has_value() below reads identically against either one.
#if __has_include(<optional>)
  #include <optional>
  namespace opt { using std::optional; using std::nullopt; }
#else
  #include <experimental/optional>
  namespace opt { using std::experimental::optional;
                  using std::experimental::nullopt; }
#endif
namespace opt {
template <typename T>
bool has_value(const optional<T>& maybe) { return static_cast<bool>(maybe); }
}  // namespace opt
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 1. Classic binary search: does `target` exist in a sorted (ascending)
//    array, and if so, at what index?
//
// Invariant maintained every iteration: if `target` is present, it lies in
// the closed range [lo, hi]. Each iteration either finds it or halves the
// remaining range by discarding the side of `mid` that provably cannot
// contain it.
// ----------------------------------------------------------------------------
opt::optional<int> binarySearch(const std::vector<int>& nums, int target) {
  int lo = 0;
  int hi = static_cast<int>(nums.size()) - 1;

  while (lo <= hi) {
    // lo + (hi - lo) / 2 instead of (lo + hi) / 2: avoids signed integer
    // overflow when lo and hi are both large (see Common Mistakes in the
    // README for why (lo + hi) / 2 is a real production bug, not a nitpick).
    int mid = lo + (hi - lo) / 2;

    if (nums[mid] == target) {
      return mid;
    } else if (nums[mid] < target) {
      // target must be to the right of mid (array is ascending), so the
      // entire left half including mid is provably not the answer.
      lo = mid + 1;
    } else {
      // Symmetric: target must be to the left of mid.
      hi = mid - 1;
    }
  }

  return opt::nullopt;  // lo > hi: range is empty, target is not present.
}

// ----------------------------------------------------------------------------
// 2. Find the FIRST occurrence of `target` in a sorted array that may
//    contain duplicates.
//
// The key generalization from classic binary search: finding *a* match is
// not enough. When nums[mid] == target, we cannot stop — there might be an
// earlier occurrence to the left. So on a match we RECORD the index and
// keep searching the LEFT half, exactly as if nums[mid] were too large.
// This is what makes it a "boundary" search rather than an exact-match
// search: we are looking for the leftmost edge of a run of equal values.
// ----------------------------------------------------------------------------
opt::optional<int> findFirstOccurrence(const std::vector<int>& nums, int target) {
  int lo = 0;
  int hi = static_cast<int>(nums.size()) - 1;
  opt::optional<int> result = opt::nullopt;

  while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;

    if (nums[mid] == target) {
      result = mid;      // Record this candidate...
      hi = mid - 1;       // ...but keep searching left for an earlier one.
    } else if (nums[mid] < target) {
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }

  return result;
}

// ----------------------------------------------------------------------------
// 3. Find the LAST occurrence of `target` — the mirror image of #2.
//
// On a match, keep searching the RIGHT half instead of the left half. Same
// three-way comparison, same lo/hi/mid skeleton, one flipped branch.
// ----------------------------------------------------------------------------
opt::optional<int> findLastOccurrence(const std::vector<int>& nums, int target) {
  int lo = 0;
  int hi = static_cast<int>(nums.size()) - 1;
  opt::optional<int> result = opt::nullopt;

  while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;

    if (nums[mid] == target) {
      result = mid;      // Record this candidate...
      lo = mid + 1;       // ...but keep searching right for a later one.
    } else if (nums[mid] < target) {
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }

  return result;
}

// ----------------------------------------------------------------------------
// 4. Search in a sorted array that was rotated at an unknown pivot, e.g.
//    [4,5,6,7,0,1,2] (originally [0,1,2,4,5,6,7], rotated left by 4).
//
// The array is no longer globally sorted, but it is "piecewise sorted": at
// every mid, AT LEAST ONE of the two halves [lo..mid] or [mid..hi] is a
// normally-sorted, non-rotated run. The generalization here is the halving
// TEST itself: instead of "is target < or > nums[mid]", the test becomes
// "which half is normally ordered, and does target fall inside that
// ordered half's range?" If yes, recurse into that half; if no, the answer
// (if it exists) must be in the OTHER half, which is guaranteed to still
// contain it precisely because we ruled out the ordered half correctly.
// ----------------------------------------------------------------------------
opt::optional<int> searchRotated(const std::vector<int>& nums, int target) {
  int lo = 0;
  int hi = static_cast<int>(nums.size()) - 1;

  while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;

    if (nums[mid] == target) {
      return mid;
    }

    if (nums[lo] <= nums[mid]) {
      // Left half [lo..mid] is normally sorted (no rotation point inside it).
      if (nums[lo] <= target && target < nums[mid]) {
        // target's value falls within the sorted left half's range: it can
        // only be there, so discard the right half.
        hi = mid - 1;
      } else {
        // target is outside the sorted left half's range, so it must be in
        // the (possibly rotated) right half.
        lo = mid + 1;
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

  return opt::nullopt;
}

// ============================================================================
// main() — demonstrates all four functions with printed, verifiable output.
// ============================================================================
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

  std::cout << "--- 1. binarySearch (classic exact match) ---\n";
  {
    std::vector<int> nums = {-1, 0, 3, 5, 9, 12};
    auto r1 = binarySearch(nums, 9);
    check(opt::has_value(r1) && *r1 == 4, "find 9 in sorted array -> index 4");

    auto r2 = binarySearch(nums, 2);
    check(!opt::has_value(r2), "2 is not present -> nullopt");

    std::vector<int> empty;
    check(!opt::has_value(binarySearch(empty, 5)), "empty array -> nullopt");
  }

  std::cout << "\n--- 2. findFirstOccurrence (leftmost boundary) ---\n";
  {
    std::vector<int> nums = {5, 7, 7, 8, 8, 8, 10};
    auto r1 = findFirstOccurrence(nums, 8);
    check(opt::has_value(r1) && *r1 == 3, "first occurrence of 8 -> index 3");

    auto r2 = findFirstOccurrence(nums, 7);
    check(opt::has_value(r2) && *r2 == 1, "first occurrence of 7 -> index 1");

    auto r3 = findFirstOccurrence(nums, 6);
    check(!opt::has_value(r3), "6 not present -> nullopt");
  }

  std::cout << "\n--- 3. findLastOccurrence (rightmost boundary) ---\n";
  {
    std::vector<int> nums = {5, 7, 7, 8, 8, 8, 10};
    auto r1 = findLastOccurrence(nums, 8);
    check(opt::has_value(r1) && *r1 == 5, "last occurrence of 8 -> index 5");

    auto r2 = findLastOccurrence(nums, 7);
    check(opt::has_value(r2) && *r2 == 2, "last occurrence of 7 -> index 2");

    auto r3 = findLastOccurrence(nums, 10);
    check(opt::has_value(r3) && *r3 == 6, "last occurrence of 10 (single copy) -> index 6");
  }

  std::cout << "\n--- 4. searchRotated (rotated sorted array) ---\n";
  {
    std::vector<int> nums = {4, 5, 6, 7, 0, 1, 2};
    auto r1 = searchRotated(nums, 0);
    check(opt::has_value(r1) && *r1 == 4, "find 0 in rotated array -> index 4");

    auto r2 = searchRotated(nums, 4);
    check(opt::has_value(r2) && *r2 == 0, "find pivot value 4 -> index 0");

    auto r3 = searchRotated(nums, 3);
    check(!opt::has_value(r3), "3 not present -> nullopt");

    std::vector<int> single = {1};
    check(opt::has_value(searchRotated(single, 1)), "single-element array, present");
    check(!opt::has_value(searchRotated(single, 2)), "single-element array, absent");

    std::vector<int> not_rotated = {1, 2, 3, 4, 5};
    auto r4 = searchRotated(not_rotated, 5);
    check(opt::has_value(r4) && *r4 == 4, "zero-rotation case still works -> index 4");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
