// ============================================================================
// Two Pointers — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// two families of "two pointers" you will re-derive on almost every problem
// that fits this pattern:
//
//   1. CONVERGING pointers — start at opposite ends of a SORTED array and
//      move toward each other. Used for: pair-sum search, container-with-
//      most-water style area maximization, palindrome checks, reversing
//      in place.
//
//   2. SAME-DIRECTION pointers (slow/fast, "write/read" pointers) — both
//      start at the beginning and walk forward at different rates. Used
//      for: in-place de-duplication / compaction, partitioning (Dutch
//      national flag style), moving zeroes.
//
// Both variants are expressed as small, generic, reusable function templates
// so you can see the *shape* of the pattern independent of any one problem.
// The worked, problem-specific solutions live in problems/*.cpp.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_tp_code && /tmp/out_tp_code
// ============================================================================

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Variant 1: Converging pointers on sorted data.
//
// Generic "find a pair whose combined value matches a target" search over any
// sorted random-access container. This is the archetypal converging-pointer
// routine: one pointer anchored at each end, and on every step exactly one of
// them moves inward based on a three-way comparison against the target.
//
// Template parameters:
//   Container   — any random-access container (std::vector<int>, etc.)
//   Combine     — a function (a, b) -> value combining two elements
//                 (default: a + b, i.e. classic "pair sum")
//   value_type  — the type Combine returns, compared against `target`
//
// Returns the pair of INDICES (left, right) if found, std::nullopt otherwise.
// The container MUST already be sorted in ascending order by whatever
// ordering makes `combine(a, b)` monotonic as the pointers move — for plain
// numeric addition, that means sorted ascending by value.
// ----------------------------------------------------------------------------
template <typename Container, typename Combine = std::plus<>>
std::optional<std::pair<size_t, size_t>> two_pointer_find_pair(
    const Container& sorted, typename Container::value_type target,
    Combine combine = Combine{}) {
  if (sorted.size() < 2) {
    return std::nullopt;  // Need at least two elements to form a pair.
  }

  size_t left = 0;
  size_t right = sorted.size() - 1;

  while (left < right) {
    auto current = combine(sorted[left], sorted[right]);

    if (current == target) {
      return std::make_pair(left, right);
    } else if (current < target) {
      // Current combined value is too small. Since the array is sorted
      // ascending, the only way to increase the combined value is to move
      // `left` inward to a larger element. Moving `right` inward would only
      // make the value smaller or equal — never helps.
      ++left;
    } else {
      // Current combined value is too large. Symmetric argument: move
      // `right` inward to a smaller element.
      --right;
    }
  }

  return std::nullopt;  // Pointers crossed without finding a match.
}

// ----------------------------------------------------------------------------
// Variant 1b: Converging pointers for area/capacity maximization.
//
// Same skeleton as above, but instead of stopping at an exact match, we keep
// a running best answer and shrink the search space by always discarding the
// pointer that *limits* the current answer (the shorter of the two "walls").
// This generalizes to "container with most water" / "trapping rain water"
// style problems.
// ----------------------------------------------------------------------------
template <typename Container>
long long two_pointer_max_area(const Container& heights) {
  if (heights.size() < 2) return 0;

  size_t left = 0;
  size_t right = heights.size() - 1;
  long long best = 0;

  while (left < right) {
    long long width = static_cast<long long>(right - left);
    long long shorter_wall = std::min(heights[left], heights[right]);
    best = std::max(best, width * shorter_wall);

    // Move the pointer at the SHORTER wall. Keeping the shorter wall in
    // place can never produce a larger area than we already have (width can
    // only shrink from here, and the shorter wall still caps the height).
    // Moving the taller wall's pointer would waste width without any chance
    // of raising the limiting height. So the shorter wall must move.
    if (heights[left] < heights[right]) {
      ++left;
    } else {
      --right;
    }
  }

  return best;
}

// ----------------------------------------------------------------------------
// Variant 2: Same-direction pointers for in-place compaction.
//
// Generic "keep only elements that satisfy `keep_predicate`, compacted to
// the front, preserving relative order" — the shape behind "remove
// duplicates from sorted array," "move zeroes," and "remove element."
//
// `write` is the slow pointer: it marks the boundary of the compacted
// region built so far. `read` is the fast pointer: it scans every element
// exactly once. Whenever `read` finds something worth keeping, it is copied
// back to `write`'s position and `write` advances.
//
// Returns the new logical length of the compacted prefix. The container is
// modified in place; elements past the returned length are leftover/unused.
// ----------------------------------------------------------------------------
template <typename Container, typename Predicate>
size_t two_pointer_compact(Container& arr, Predicate keep_predicate) {
  size_t write = 0;

  for (size_t read = 0; read < arr.size(); ++read) {
    if (keep_predicate(arr, read, write)) {
      arr[write] = arr[read];
      ++write;
    }
  }

  return write;
}

// ----------------------------------------------------------------------------
// Variant 2b: Convenience wrapper — de-duplicate a SORTED range in place,
// keeping at most `max_allowed_duplicates` copies of each value. This is the
// generalized shape behind "Remove Duplicates from Sorted Array" (max 1) and
// "Remove Duplicates from Sorted Array II" (max 2).
// ----------------------------------------------------------------------------
template <typename Container>
size_t two_pointer_dedupe_sorted(Container& arr, size_t max_allowed_duplicates = 1) {
  return two_pointer_compact(arr, [&](const Container& a, size_t read, size_t write) {
    if (write < max_allowed_duplicates) {
      // Not enough elements written yet to compare against — always keep
      // the first `max_allowed_duplicates` elements.
      return true;
    }
    // Keep a[read] only if it differs from the value that is
    // `max_allowed_duplicates` slots back in the already-written prefix.
    return a[read] != a[write - max_allowed_duplicates];
  });
}

// ============================================================================
// main() — demonstrates both variants with printed, verifiable output.
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

  std::cout << "--- Variant 1: two_pointer_find_pair (converging, sorted) ---\n";
  {
    std::vector<int> nums = {2, 7, 11, 15, 18, 24};
    // Requires both pointers to move before converging: 7 + 15 = 22.
    auto result = two_pointer_find_pair(nums, 22);
    check(result.has_value() && result->first == 1 && result->second == 3,
          "find pair summing to 22 -> indices (1, 3)");

    auto no_match = two_pointer_find_pair(nums, 100);
    check(!no_match.has_value(), "no pair sums to 100 -> nullopt");
  }

  std::cout << "\n--- Variant 1b: two_pointer_max_area (converging, area) ---\n";
  {
    std::vector<int> heights = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    long long area = two_pointer_max_area(heights);
    check(area == 49, "max area for classic example -> 49");

    std::vector<int> flat = {4, 4};
    check(two_pointer_max_area(flat) == 4, "two equal walls -> width(1) * height(4) = 4");
  }

  std::cout << "\n--- Variant 2: two_pointer_compact (same-direction) ---\n";
  {
    std::vector<int> nums = {0, 1, 0, 3, 12};
    size_t new_len = two_pointer_compact(nums, [](const std::vector<int>& a, size_t read, size_t) {
      return a[read] != 0;  // Keep everything that is not zero.
    });
    std::vector<int> nonzero_prefix(nums.begin(), nums.begin() + new_len);
    std::vector<int> expected = {1, 3, 12};
    check(nonzero_prefix == expected, "compact non-zero elements -> {1, 3, 12}");
  }

  std::cout << "\n--- Variant 2b: two_pointer_dedupe_sorted (same-direction) ---\n";
  {
    std::vector<int> sorted_nums = {1, 1, 2, 2, 2, 3, 4, 4};
    size_t new_len = two_pointer_dedupe_sorted(sorted_nums, /*max_allowed_duplicates=*/1);
    std::vector<int> deduped(sorted_nums.begin(), sorted_nums.begin() + new_len);
    std::vector<int> expected = {1, 2, 3, 4};
    check(deduped == expected, "dedupe with max 1 copy -> {1, 2, 3, 4}");
  }
  {
    std::vector<int> sorted_nums = {1, 1, 1, 2, 2, 3};
    size_t new_len = two_pointer_dedupe_sorted(sorted_nums, /*max_allowed_duplicates=*/2);
    std::vector<int> deduped(sorted_nums.begin(), sorted_nums.begin() + new_len);
    std::vector<int> expected = {1, 1, 2, 2, 3};
    check(deduped == expected, "dedupe with max 2 copies -> {1, 1, 2, 2, 3}");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
