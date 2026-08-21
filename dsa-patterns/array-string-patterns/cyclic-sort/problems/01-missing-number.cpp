// ============================================================================
// LeetCode 268 — Missing Number
// https://leetcode.com/problems/missing-number/
// ============================================================================
//
// PROBLEM
// -------
// Given an array `nums` containing n DISTINCT numbers in the range [0, n],
// return the one number in that range that is missing from the array.
//
// Example: nums = [3, 0, 1]  (n = 3, so the range is [0, 3])
//          -> 2, because 0, 1 and 3 are present and 2 is not.
//
// HOW THIS INPUT MATCHES THE RECOGNITION SIGNAL
// ----------------------------------------------
// The pattern's signal (see ../README.md, "When To Use") is: *the input is a
// permutation of 1..n or 0..n-1, possibly with duplicates/omissions, and O(1)
// extra space is required.* This problem hands it over almost verbatim:
//
//   - n slots holding values drawn from [0, n] — a range tied directly to the
//     array's own length, which is exactly the "bounded range" precondition.
//   - the values are distinct, so the array is a permutation of 0..n with
//     exactly one omission — the mildest possible deviation from a clean
//     permutation.
//   - LeetCode's own follow-up asks for O(1) extra space, which rules out the
//     hash-set / frequency-array answer and points straight at using the
//     array as its own hash table.
//
// This is the pattern in its purest form: the plain "place each value at its
// home index" pass, with a verification scan that reports a single gap.
//
// APPROACH — Cyclic Sort under the [0..n-1] convention
// -----------------------------------------------------
// The module's template (../code.cpp) uses the [1..n] convention, where value
// v belongs at index v - 1. This problem uses the OTHER convention: values
// start at 0, so the home index of value v is simply v, and the expected
// value at index i is i itself.
//
// The wrinkle that makes this problem interesting is the count mismatch: the
// range [0, n] contains n + 1 candidate values but the array has only n
// slots. So one value — whichever it is — has no slot at all. Concretely,
// the value n has no home index in an array whose last index is n - 1, and
// must be treated as out of range by the swap loop (left exactly where it is,
// like any other unplaceable value).
//
//   1. Sorting pass: walk cursor i. If nums[i] is in [0, n - 1] and its home
//      slot does not already hold that same value, swap it home and re-check
//      position i WITHOUT advancing. Otherwise advance.
//   2. Verification pass: the first index j where nums[j] != j is the answer —
//      j is the missing number, because every value that exists and has a home
//      is now sitting in it.
//   3. If no index mismatches, every one of 0..n-1 is present, so the missing
//      value must be n itself (the one candidate that never had a slot).
//
// ALTERNATIVES WITH THE SAME COMPLEXITY (and why they do not generalize)
// ----------------------------------------------------------------------
// For THIS problem specifically, two closed-form tricks match Cyclic Sort's
// O(n) time / O(1) space and additionally do not mutate the input:
//
//   sum:  answer = n*(n+1)/2 - sum(nums)      // beware int overflow for big n
//   xor:  answer = (0^1^...^n) ^ (nums[0]^...^nums[n-1])
//
// Both exploit "exactly one value is absent and nothing is duplicated." The
// moment a duplicate is possible (03-find-the-duplicate-number.cpp), or more
// than one value is absent (02-find-all-numbers-disappeared-in-an-array.cpp),
// or arbitrary out-of-range noise appears (04-first-missing-positive.cpp),
// they collapse. Cyclic Sort is the one mechanism that survives all three —
// which is the whole reason to learn this problem the "slower-looking" way.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// Two of them, both about the range check:
//   (a) Using the [1..n] formula (nums[i] - 1) out of habit. Here the formula
//       is nums[i], full stop — see ../README.md, "Common Mistakes",
//       "Off-by-one between value and index".
//   (b) Forgetting that value n is legitimately in range for the PROBLEM but
//       out of range for the ARRAY. Computing a home index for it and swapping
//       would write past the end of the array. The in-range test must be
//       nums[i] < n, not nums[i] <= n.
//
// COMPLEXITY
// ----------
// Time:  O(n) — total swaps across the whole run are bounded by n (each swap
//               permanently seats one value), plus at most n cursor advances,
//               plus the O(n) verification scan.
// Space: O(1) — a couple of index variables; the array is rearranged in place.
// ============================================================================

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// cyclic_sort_zero_based
//
// The sorting pass, using the [0..n-1] convention: value v's home index is v,
// and the expected value at index i is i. Values outside [0, n - 1] (here:
// only the value n can occur) are left untouched.
//
// Kept as its own function to preserve the module's two-pass framing: this
// function only rearranges, it does not answer anything.
// ----------------------------------------------------------------------------
void cyclic_sort_zero_based(std::vector<int>& nums) {
  const int n = static_cast<int>(nums.size());
  int i = 0;

  while (i < n) {
    // Bounds-check BEFORE deriving a home index. `n` is a valid value for
    // this problem but has no slot in an n-element array, so it is out of
    // range as far as the swap loop is concerned.
    const bool in_range = nums[i] >= 0 && nums[i] < n;

    if (in_range) {
      const int correct_index = nums[i];  // [0..n-1] convention: home == value

      // The guard `nums[i] != nums[correct_index]` also covers
      // `i == correct_index` (a value already at home), and would stop an
      // infinite swap loop if duplicates were possible. They are not here —
      // the problem guarantees distinct values — but the guard costs nothing
      // and keeps this loop identical in shape to the general template.
      if (nums[i] != nums[correct_index]) {
        std::swap(nums[i], nums[correct_index]);
        continue;  // Do NOT advance: re-examine whatever just landed at i.
      }
    }

    ++i;  // Settled: out of range, or already holding its correct value.
  }
}

// ----------------------------------------------------------------------------
// missingNumber
//
// Sorting pass, then the verification scan. Returns the single value in
// [0, n] absent from `nums`. Mutates `nums` (taken by value here so callers'
// test vectors stay readable; pass by reference in production if the mutation
// is acceptable, which is the whole point of the O(1)-space claim).
// ----------------------------------------------------------------------------
int missingNumber(std::vector<int> nums) {
  const int n = static_cast<int>(nums.size());

  cyclic_sort_zero_based(nums);

  // Verification pass: the expected value at index j is j.
  for (int j = 0; j < n; ++j) {
    if (nums[j] != j) {
      return j;  // j never made it into the array -> j is missing.
    }
  }

  // Every index holds its own value, so 0..n-1 are all present. The only
  // remaining candidate in [0, n] is n itself — the value that never had a
  // slot to be placed into.
  return n;
}

// ============================================================================
// main() — printed PASS/FAIL against known expected answers.
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

  // --- LeetCode's own examples -------------------------------------------
  check(missingNumber(std::vector<int>{3, 0, 1}) == 2,
        "[3,0,1] -> 2 (mid-range gap)");
  check(missingNumber(std::vector<int>{0, 1}) == 2,
        "[0,1] -> 2 (missing value is n, no slot existed for it)");
  check(missingNumber(std::vector<int>{9, 6, 4, 2, 3, 5, 7, 0, 1}) == 8,
        "[9,6,4,2,3,5,7,0,1] -> 8 (n=9 present, 8 absent)");

  // --- Single element: both branches of the answer ------------------------
  check(missingNumber(std::vector<int>{0}) == 1,
        "[0] -> 1 (single element, missing value is n)");
  check(missingNumber(std::vector<int>{1}) == 0,
        "[1] -> 0 (single element, the out-of-range value is the only one)");

  // --- Boundary answers: the very first and very last candidate -----------
  check(missingNumber(std::vector<int>{1, 2, 3}) == 0,
        "[1,2,3] -> 0 (missing value is the range minimum)");
  check(missingNumber(std::vector<int>{0, 1, 2}) == 3,
        "[0,1,2] -> 3 (missing value is the range maximum)");

  // --- Already-sorted and reverse-sorted inputs --------------------------
  check(missingNumber(std::vector<int>{0, 1, 2, 3, 5}) == 4,
        "[0,1,2,3,5] -> 4 (already sorted, one gap)");
  check(missingNumber(std::vector<int>{4, 3, 2, 1}) == 0,
        "[4,3,2,1] -> 0 (reverse sorted; the value 4 has no home and is skipped)");

  // --- Degenerate input: no elements at all ------------------------------
  // Not reachable under LeetCode's constraints (n >= 1), but the code must not
  // read out of bounds: with n = 0 the range is [0, 0] and 0 is missing.
  check(missingNumber(std::vector<int>{}) == 0,
        "[] -> 0 (empty array, range is [0,0])");

  // --- The sorting pass itself, inspected directly -----------------------
  {
    std::vector<int> nums = {3, 0, 1};
    cyclic_sort_zero_based(nums);
    // 3 is out of range for a 3-slot array and is never moved from index 0...
    // except that 0 and 1 both want homes, and placing them displaces it.
    // Expected end state: index 0 holds 0, index 1 holds 1, index 2 holds the
    // unplaceable 3.
    std::vector<int> expected = {0, 1, 3};
    check(nums == expected,
          "sorting pass: [3,0,1] -> [0,1,3] (unplaceable 3 ends up in the gap)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
