// ============================================================================
// LeetCode 287 — Find the Duplicate Number
// https://leetcode.com/problems/find-the-duplicate-number/
// ============================================================================
//
// PROBLEM
// -------
// Given an array `nums` of n + 1 integers where every value is in the range
// [1, n], exactly one value is repeated (possibly many times) and every other
// value appears at most once. Return the repeated value.
//
// Example: nums = [1, 3, 4, 2, 2]   (5 slots, values drawn from [1, 4])
//          -> 2
//
// HOW THIS INPUT MATCHES THE RECOGNITION SIGNAL
// ----------------------------------------------
// The signal (see ../README.md, "When To Use") is: *the input is a permutation
// of 1..n or 0..n-1, possibly with duplicates/omissions, and O(1) extra space
// is required.* This problem is the "with duplicates" corner of it:
//
//   - the array has n + 1 slots but only n distinct candidate values, all in
//     [1, n]. That is a range tied to the array's length (length - 1), so the
//     home index of value v is still v - 1 — the ../code.cpp convention,
//     unchanged.
//   - by the pigeonhole principle, n + 1 slots over n candidate values force
//     at least one repeat: the array is a permutation of 1..n with one extra
//     copy squeezed in. The question is which value got copied.
//   - the problem's stated constraints are the pattern's signature demand:
//     solve it without modifying... see the caveat below... using only
//     constant extra space.
//
// ONE HONEST CAVEAT ABOUT THIS PROBLEM'S CONSTRAINTS
// --------------------------------------------------
// LeetCode 287 asks for constant extra space AND for the array not to be
// modified. Cyclic Sort satisfies the first and violates the second — that is
// the pattern's central limitation (../README.md, "Disadvantages", first
// bullet), and this problem is the cleanest place in the module to feel it.
//
// The intended non-mutating answer is Floyd's Cycle Detection, covered in
// ../../../linked-list-patterns/fast-slow-pointers/. It reads nums[i] as a
// pointer to index nums[i], which turns the array into a functional graph;
// because two slots hold the same value, two different nodes point at the same
// successor, so following pointers from index 0 must eventually enter a cycle,
// and the cycle's ENTRY node is exactly the duplicated value. Slow/fast
// pointers find a meeting point inside the cycle, then a second walk from the
// start finds the entry. O(n) time, O(1) space, zero writes.
//
// So why solve it here with Cyclic Sort at all? Two reasons:
//   1. It is the same mechanism as 01 / 02 / 04 with a different verification
//      question, so it costs nothing extra to know once the pattern is known —
//      whereas Floyd's is a separate algorithm you must recall in full.
//   2. Cyclic Sort answers the neighbouring questions Floyd's cannot: which
//      values are ALSO missing (02), or what the smallest absent positive is
//      (04). Floyd's is strictly a one-trick tool for "find the one duplicate."
// If an interviewer states the no-mutation constraint and means it, say so out
// loud and switch to Floyd's — recognising which constraint binds is the point
// of the exercise, not defending one implementation.
//
// APPROACH — Cyclic Sort, then read the value at the first mismatch
// -----------------------------------------------------------------
//   1. Sorting pass ([1..n] convention, home index = value - 1). Every value
//      is in range, so nothing is skipped for being out of bounds. The pass
//      seats every value it can; the surplus copy is the one value that can
//      never be seated, because its home is already occupied by an identical
//      value.
//   2. Verification pass: find the first index j where nums[j] != j + 1. Here
//      the answer is the value SITTING at j, not the value expected at j —
//      that inversion is the whole difference from 01 and 02.
//
// Why the value at a mismatched slot is guaranteed to be the duplicate: the
// cursor only leaves a position when the value there is out of range (cannot
// happen in this problem) or is a duplicate whose home already holds an
// identical value. And a position the cursor has already left can only ever be
// written again by a swap that puts that position's OWN correct value into it.
// So every slot that still mismatches at the end holds a stalled duplicate.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// Reporting `j + 1` (the expected value) instead of `nums[j]` (the actual
// value). In 01 and 02 the missing value is what the slot expected; here the
// duplicate is what the slot actually contains. Same scan, opposite reading —
// see ../README.md, "Common Mistakes", "Confusing the sorting pass with the
// answer."
//
// COMPLEXITY
// ----------
// Time:  O(n) — total swaps across the whole run are bounded by the array's
//               length (each successful swap permanently seats one value),
//               plus at most that many cursor advances, plus the O(n) scan.
// Space: O(1) extra — rearranged in place, no auxiliary structure.
// ============================================================================

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// cyclic_sort
//
// The same pass as ../code.cpp's `cyclic_sort`, inlined so this file stands
// alone. Note that the in-range test is written against the array's own length
// (`slots`), not against the problem's n = slots - 1. Both are correct here —
// no value equals slots, since values stop at slots - 1 — and using the
// array's length keeps the bound check identical to every other file in this
// directory: "does this value have a slot in THIS array?"
// ----------------------------------------------------------------------------
void cyclic_sort(std::vector<int>& nums) {
  const int slots = static_cast<int>(nums.size());
  int i = 0;

  while (i < slots) {
    const bool in_range = nums[i] >= 1 && nums[i] <= slots;

    if (in_range) {
      const int correct_index = nums[i] - 1;

      // Compare values, not indices: with a duplicate guaranteed to exist,
      // an `i != correct_index` test here would swap two identical values
      // back and forth forever. This is the check that makes the surplus copy
      // stall instead of spinning.
      if (nums[i] != nums[correct_index]) {
        std::swap(nums[i], nums[correct_index]);
        continue;  // Do NOT advance: re-examine whatever just landed at i.
      }
    }

    ++i;
  }
}

// ----------------------------------------------------------------------------
// findDuplicate
//
// Sorting pass, then the verification scan — reading the ACTUAL value at the
// first mismatched slot. Returns 0 if the array contains no duplicate at all,
// which cannot happen under LeetCode's constraints but keeps the function
// total rather than undefined.
// ----------------------------------------------------------------------------
int findDuplicate(std::vector<int> nums) {
  cyclic_sort(nums);

  const int slots = static_cast<int>(nums.size());
  for (int j = 0; j < slots; ++j) {
    if (nums[j] != j + 1) {
      return nums[j];  // The stalled value parked here is the duplicate.
    }
  }

  return 0;  // No mismatch -> a clean permutation -> no duplicate exists.
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

  auto check_result = [&](std::vector<int> input, int expected,
                          const std::string& note) {
    const std::string label = to_string(input) + " -> " +
                              std::to_string(expected) + "  (" + note + ")";
    check(findDuplicate(input) == expected, label);
  };

  // --- LeetCode's own examples -------------------------------------------
  check_result({1, 3, 4, 2, 2}, 2, "duplicate at the end of the array");
  check_result({3, 1, 3, 4, 2}, 3, "duplicate straddling the array");
  check_result({3, 3, 3, 3, 3}, 3, "every slot holds the same value");

  // --- Smallest legal input: n = 1, so 2 slots ---------------------------
  check_result({1, 1}, 1, "minimum size, the only possible value repeats");

  // --- Duplicate appearing more than twice -------------------------------
  check_result({2, 2, 2, 2, 2}, 2,
               "one value five times; the extras all stall on the same home");
  check_result({1, 2, 2, 2, 3}, 2, "three copies plus two singletons");

  // --- Duplicate at the boundaries of the value range --------------------
  check_result({1, 1, 2, 3, 4}, 1, "the range minimum is the duplicate");
  check_result({4, 1, 2, 3, 4}, 4, "the range maximum is the duplicate");

  // --- Already sorted, and reverse sorted --------------------------------
  check_result({1, 2, 3, 4, 4}, 4, "already sorted input");
  check_result({4, 3, 2, 1, 1}, 1, "reverse-sorted prefix");

  // --- Larger shuffled case, single duplicate deep in the middle ---------
  check_result({9, 1, 8, 2, 7, 3, 6, 4, 5, 5}, 5,
               "ten slots over [1,9]; only 5 repeats");

  // --- Degenerate inputs: no duplicate, and no elements ------------------
  // Neither is reachable under LeetCode's constraints; both must return
  // cleanly rather than read out of bounds or loop.
  check_result({1, 2, 3}, 0, "clean permutation, no duplicate -> sentinel 0");
  check_result({}, 0, "empty array -> sentinel 0, no out-of-bounds read");

  // --- The sorting pass itself, inspected directly -----------------------
  {
    std::vector<int> nums = {1, 3, 4, 2, 2};
    cyclic_sort(nums);
    // 1..4 all reach their homes at indices 0..3; the surplus 2 has nowhere to
    // go (index 1 already holds a 2) and stalls in the leftover slot.
    std::vector<int> expected = {1, 2, 3, 4, 2};
    check(nums == expected,
          "sorting pass: [1,3,4,2,2] -> " + to_string(expected) +
              " (the surplus copy is the only value left displaced)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
