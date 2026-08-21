// ============================================================================
// LeetCode 448 — Find All Numbers Disappeared in an Array
// https://leetcode.com/problems/find-all-numbers-disappeared-in-an-array/
// ============================================================================
//
// PROBLEM
// -------
// Given an array `nums` of n integers where every value is in the range
// [1, n], return a list of all the integers in [1, n] that do NOT appear in
// `nums`. Some values appear twice; consequently, exactly as many values are
// absent as there are extra copies.
//
// Example: nums = [4, 3, 2, 7, 8, 2, 3, 1]   (n = 8, range [1, 8])
//          -> [5, 6]   (2 and 3 each appear twice, so 5 and 6 are absent)
//
// HOW THIS INPUT MATCHES THE RECOGNITION SIGNAL
// ----------------------------------------------
// The signal (see ../README.md, "When To Use") is: *the input is a permutation
// of 1..n or 0..n-1, possibly with duplicates/omissions, and O(1) extra space
// is required.* Here every clause is stated outright by the problem:
//
//   - n slots, every value guaranteed to lie in [1, n] — the range is tied to
//     the array's own length, so the home index of value v is v - 1, exactly
//     the convention used by ../code.cpp's `cyclic_sort`.
//   - "possibly with duplicates/omissions" is not a possibility here but the
//     entire subject of the question: the array is a permutation of 1..n that
//     has had some values overwritten by copies of others.
//   - LeetCode states the follow-up explicitly: O(n) time and no extra space
//     (the returned list does not count). That rules out the frequency array.
//
// The difference from 01-missing-number.cpp is only in the verification pass:
// the mechanism is unchanged, but instead of returning at the first mismatch
// we collect EVERY mismatch. That generalization — one sorting pass, many
// answers — is why this pattern is worth learning as a pattern.
//
// APPROACH — Cyclic Sort, then collect every mismatch
// ----------------------------------------------------
//   1. Sorting pass ([1..n] convention, home index = value - 1): walk cursor
//      i; if nums[i] is not already equal to the value its home slot holds,
//      swap it home and re-check position i WITHOUT advancing; otherwise
//      advance. A duplicate stalls the loop the moment its home is already
//      occupied by an identical value — that stall is the mechanism, not a
//      failure, and it is what leaves duplicates parked in the slots belonging
//      to the absent values.
//   2. Verification pass: every index j with nums[j] != j + 1 contributes
//      j + 1 to the answer. Each such slot is a value's home that the value
//      never reached, i.e. it is absent from the whole array; the wrong value
//      sitting there is a duplicate of something placed elsewhere.
//
// Because the sorting pass places every value that CAN be placed, and there
// are exactly as many unfillable slots as absent values, the mismatch list is
// exactly the answer — no post-filtering, no dedup needed.
//
// THE COMPETING O(1)-SPACE TRICK, AND WHY THIS FILE DOES NOT USE IT
// ------------------------------------------------------------------
// This problem is the classic home of the in-place MARKING trick: for each
// value v, negate the number at index |v| - 1, then report every index that
// is still positive. It is also O(n)/O(1) and is a fine answer, but it comes
// with strings attached that Cyclic Sort does not have:
//
//   - it only works because every value is guaranteed strictly positive, so
//     the sign bit happens to be a free scratch bit. Allow a 0 or a negative
//     in the input and the trick breaks outright — which is precisely the
//     input shape of 04-first-missing-positive.cpp, where marking needs extra
//     preprocessing but Cyclic Sort needs only its usual bounds check.
//   - it must read |nums[i]| everywhere afterwards, because the array is now
//     carrying two meanings in one field (the value AND the seen-flag), and
//     forgetting one abs() is the standard bug.
//   - it leaves the array in a state (sign-flipped) that means nothing to the
//     caller, whereas Cyclic Sort leaves it as sorted as the data permits.
//
// The marking trick and Cyclic Sort both "use the array as its own hash
// table"; marking stores one bit per value in the sign, Cyclic Sort stores
// the fact positionally. Positional is the more general of the two.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// The swap condition must compare VALUES (nums[i] != nums[correct_index]),
// not indices (i != correct_index). With duplicates present — and this problem
// guarantees them — the index test loops forever: two identical values keep
// swapping into each other's positions, each swap undoing the last. See
// ../README.md, "Common Mistakes", second bullet. Test case [1,1] below fails
// to terminate at all under the index test, which is why it is in the suite.
//
// COMPLEXITY
// ----------
// Time:  O(n) — total swaps across the whole run are bounded by n (each
//               successful swap permanently seats one value), plus at most n
//               cursor advances, plus the O(n) collection scan.
// Space: O(1) extra — the array is rearranged in place. The returned vector
//               is the required output, not working memory, so by the
//               problem's own accounting it does not count.
// ============================================================================

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// cyclic_sort
//
// Identical in shape to ../code.cpp's `cyclic_sort`, reproduced inline so this
// file stays standalone. [1..n] convention: value v's home index is v - 1, and
// the expected value at index i is i + 1.
// ----------------------------------------------------------------------------
void cyclic_sort(std::vector<int>& nums) {
  const int n = static_cast<int>(nums.size());
  int i = 0;

  while (i < n) {
    // Bounds-check before computing a home index. The problem guarantees
    // values in [1, n], so this never rejects anything here — but keeping the
    // check makes the loop identical to the one 04-first-missing-positive.cpp
    // needs, where it does real work.
    const bool in_range = nums[i] >= 1 && nums[i] <= n;

    if (in_range) {
      const int correct_index = nums[i] - 1;

      // VALUE comparison, not index comparison. This single expression covers
      // all three settle cases at once: already home (correct_index == i, so
      // the two reads are the same slot), and duplicate-whose-home-is-taken
      // (a different slot already holding this exact value).
      if (nums[i] != nums[correct_index]) {
        std::swap(nums[i], nums[correct_index]);
        continue;  // Do NOT advance: re-examine whatever just landed at i.
      }
    }

    ++i;
  }
}

// ----------------------------------------------------------------------------
// findDisappearedNumbers
//
// Sorting pass, then a collection scan over every mismatch. Takes `nums` by
// value so the test suite below can reuse literals readably; the O(1)-space
// claim is about not allocating a structure proportional to n, and holds for
// a by-reference call.
// ----------------------------------------------------------------------------
std::vector<int> findDisappearedNumbers(std::vector<int> nums) {
  cyclic_sort(nums);

  std::vector<int> missing;
  const int n = static_cast<int>(nums.size());

  // Verification pass: index j should hold j + 1. If it does not, then j + 1
  // never found its home, which (given every value is in range and every
  // placeable value WAS placed) means j + 1 is nowhere in the array.
  for (int j = 0; j < n; ++j) {
    if (nums[j] != j + 1) {
      missing.push_back(j + 1);
    }
  }

  // Ascending by construction: j walks upward, so no sort is needed even
  // though the problem's examples happen to be ordered.
  return missing;
}

// ============================================================================
// Test scaffolding
// ============================================================================
std::string to_string(const std::vector<int>& v) {
  std::string out = "[";
  for (size_t i = 0; i < v.size(); ++i) {
    if (i) out += ",";
    out += std::to_string(v[i]);
  }
  out += "]";
  return out;
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

  auto check_result = [&](std::vector<int> input, std::vector<int> expected,
                          const std::string& note) {
    const std::string label =
        to_string(input) + " -> " + to_string(expected) + "  (" + note + ")";
    check(findDisappearedNumbers(input) == expected, label);
  };

  // --- LeetCode's own examples -------------------------------------------
  check_result({4, 3, 2, 7, 8, 2, 3, 1}, {5, 6},
               "two duplicates -> two absent values");
  check_result({1, 1}, {2},
               "duplicate at its own home; index test would loop forever");

  // --- Nothing missing ---------------------------------------------------
  check_result({1, 2, 3, 4}, {},
               "clean permutation, already sorted -> nothing absent");
  check_result({3, 1, 4, 2}, {},
               "clean permutation, shuffled -> nothing absent");

  // --- Single element ----------------------------------------------------
  check_result({1}, {}, "single element, correct value -> nothing absent");

  // --- All values identical ----------------------------------------------
  check_result({2, 2}, {1},
               "all-same, value already home -> the other slot is unfillable");
  check_result({3, 3, 3}, {1, 2},
               "all-same -> every other value in [1,n] is absent");

  // --- One value appearing n times, at the far end of the range ----------
  check_result({1, 1, 1, 1}, {2, 3, 4},
               "four copies of the range minimum -> 2,3,4 absent");

  // --- Reverse-sorted, and a boundary-only gap ---------------------------
  check_result({4, 3, 2, 1}, {}, "reverse sorted permutation -> nothing absent");
  check_result({2, 3, 4, 4}, {1},
               "only the range minimum is absent");
  check_result({1, 2, 3, 3}, {4},
               "only the range maximum is absent");

  // --- Degenerate input --------------------------------------------------
  // Not reachable under LeetCode's constraints (n >= 1), but the loops must
  // not read out of bounds: an empty array has an empty [1, 0] range.
  check_result({}, {}, "empty array -> empty answer, no out-of-bounds read");

  // --- The sorting pass itself, inspected directly -----------------------
  {
    std::vector<int> nums = {4, 3, 2, 7, 8, 2, 3, 1};
    cyclic_sort(nums);
    // 1..4 and 7,8 all reach their homes. The two surplus copies (a second 2
    // and a second 3) are stalled and end up parked in the two homes nobody
    // claimed: index 4 (wants 5) and index 5 (wants 6).
    std::vector<int> expected = {1, 2, 3, 4, 3, 2, 7, 8};
    check(nums == expected,
          "sorting pass: [4,3,2,7,8,2,3,1] -> " + to_string(expected) +
              " (stalled duplicates park in the unfillable homes)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
