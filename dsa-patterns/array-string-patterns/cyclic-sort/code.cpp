// ============================================================================
// Cyclic Sort — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// mechanism you will re-derive on every problem that fits this pattern: an
// array of size n holding values (mostly) confined to [1..n], where each
// value has exactly one legitimate "home" index (value - 1). Instead of
// sorting the whole array with a comparison sort, or hashing every value
// into a second structure, we swap each value directly to its home index in
// place, using the array itself as an implicit hash table.
//
// The worked, problem-specific solutions (Missing Number, Find All Numbers
// Disappeared, Find the Duplicate Number, First Missing Positive) live in
// problems/*.cpp and each implement this same idea inline, so they stay
// dependency-free and independently readable.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_cs_code && /tmp/out_cs_code
// ============================================================================

#include <iostream>
#include <string>
#include <utility>
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
// cyclic_sort
//
// Rearranges `nums` in place so that, wherever possible, nums[i] == i + 1
// (the [1..n] convention, n = nums.size()). Values outside [1, n] are left
// exactly where they are — they can never have a home in an array of this
// size. Duplicate values are also left in place once one copy has already
// claimed the home index, rather than looping forever trying to place both.
//
// Mechanism: walk cursor `i` from 0 to n-1. At each position, compute the
// home index for the current value. If the value is out of range, or the
// home slot already holds an identical value (including the case where the
// value is already at its own home), the position is settled and `i`
// advances. Otherwise, swap the current value into its home index and
// re-examine position `i` again WITHOUT advancing, because a new value has
// just arrived there that itself might need to move.
//
// Why this terminates in O(n) total work: every swap either places some
// value into its final home for good (at most n such swaps across the whole
// run, since there are only n slots and a placed value is never disturbed
// again), or is refused outright as a no-op. Total swaps + total cursor
// advances are both bounded by n, giving O(n) time overall.
// ----------------------------------------------------------------------------
void cyclic_sort(std::vector<int>& nums) {
  const int n = static_cast<int>(nums.size());
  int i = 0;

  while (i < n) {
    // Home index for a [1..n]-ranged value v is v - 1.
    const int correct_index = nums[i] - 1;

    // Only attempt to place values that are actually within [1, n]; anything
    // else can never be at home in this array and must be skipped.
    const bool in_range = nums[i] >= 1 && nums[i] <= n;

    if (in_range && nums[i] != nums[correct_index]) {
      // The current value is not yet home, and its home slot does not
      // already hold an identical value, so swapping makes real progress.
      std::swap(nums[i], nums[correct_index]);
      // Deliberately do NOT advance i: re-check whatever just landed here.
    } else {
      // Either out of range, or already correctly placed, or a duplicate
      // whose home is already taken by an identical value (swapping would
      // be a no-op and would loop forever if attempted) — settle and move on.
      ++i;
    }
  }
}

// ----------------------------------------------------------------------------
// find_first_misplaced
//
// The verification-pass helper. Scans an already-cyclically-sorted array
// (one that cyclic_sort has already processed) and returns the first index
// where nums[i] != i + 1 — i.e. the first slot that does not hold its
// expected [1..n] value. Returns opt::nullopt if every slot already holds
// its correct value (a clean permutation with nothing missing or
// duplicated).
//
// This is deliberately a separate function from cyclic_sort itself: sorting
// and verifying are two conceptually distinct O(n) passes. Every
// problem-specific solution in problems/ builds its own answer on top of
// this same "find where reality disagrees with expectation" scan, just
// interpreting the mismatch differently (the missing value, the duplicate
// value, etc.).
// ----------------------------------------------------------------------------
opt::optional<size_t> find_first_misplaced(const std::vector<int>& nums) {
  for (size_t i = 0; i < nums.size(); ++i) {
    if (nums[i] != static_cast<int>(i + 1)) {
      return i;
    }
  }
  return opt::nullopt;
}

// ============================================================================
// main() — demonstrates both functions with printed, verifiable output.
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

  std::cout << "--- cyclic_sort: complete permutation, no mismatches ---\n";
  {
    std::vector<int> nums = {3, 1, 5, 4, 2};
    cyclic_sort(nums);
    std::vector<int> expected = {1, 2, 3, 4, 5};
    check(nums == expected, "[3,1,5,4,2] -> [1,2,3,4,5]");
    check(!opt::has_value(find_first_misplaced(nums)),
          "fully sorted permutation -> no misplaced index");
  }

  std::cout << "\n--- cyclic_sort: duplicate value implies a missing value ---\n";
  {
    // 2 appears twice, 3 is missing.
    std::vector<int> nums = {1, 2, 2, 4};
    cyclic_sort(nums);
    // Index 2 (0-indexed) cannot hold both a placed 3 and the duplicate 2;
    // the duplicate is left in place because its home (index 1) already
    // holds a 2.
    std::vector<int> expected = {1, 2, 2, 4};
    check(nums == expected, "[1,2,2,4] settles with the duplicate left in place");

    auto mismatch = find_first_misplaced(nums);
    check(opt::has_value(mismatch) && *mismatch == 2,
          "first misplaced index is 2 (holds 2, should hold 3 -> reveals dup/missing)");
  }

  std::cout << "\n--- cyclic_sort: out-of-range values are left untouched ---\n";
  {
    // 0 is below range, and there is no value 4 anywhere -> 0 stays put,
    // and whichever slot cannot be filled reveals the story.
    std::vector<int> nums = {3, 0, 1};
    cyclic_sort(nums);
    // n = 3, valid range is [1,3]. 0 is out of range and skipped. Values 3
    // and 1 can still be placed at their homes (index 2 and index 0).
    std::vector<int> expected = {1, 0, 3};
    check(nums == expected, "[3,0,1] -> [1,0,3] (0 is out of range, left in place)");

    auto mismatch = find_first_misplaced(nums);
    check(opt::has_value(mismatch) && *mismatch == 1,
          "first misplaced index is 1 (holds out-of-range 0, should hold 2)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
