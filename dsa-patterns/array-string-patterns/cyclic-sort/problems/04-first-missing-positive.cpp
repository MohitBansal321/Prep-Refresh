// ============================================================================
// LeetCode 41 — First Missing Positive
// https://leetcode.com/problems/first-missing-positive/
// ============================================================================
//
// PROBLEM
// -------
// Given an UNSORTED array `nums` of arbitrary integers — negatives, zeros,
// duplicates and values far outside the array's length are all allowed — return
// the smallest POSITIVE integer that does not appear in the array. Required:
// O(n) time and O(1) auxiliary space.
//
// Examples: [1, 2, 0]          -> 3
//           [3, 4, -1, 1]      -> 2
//           [7, 8, 9, 11, 12]  -> 1   (no small positive is present at all)
//
// HOW THIS INPUT MATCHES THE RECOGNITION SIGNAL
// ----------------------------------------------
// The signal (see ../README.md, "When To Use") is: *the input is a permutation
// of 1..n or 0..n-1, possibly with duplicates/omissions, and O(1) extra space
// is required.* Unlike 01, 02 and 03, this problem does NOT hand that over —
// the bounded range is not stated anywhere and has to be ESTABLISHED. That is
// exactly what makes it the hard one, and the reason it is the last file here.
//
// The derivation, which is the whole insight:
//
//   The answer is always somewhere in [1, n + 1], where n = nums.size().
//   Why: the array has n slots, so it can hold at most n distinct positive
//   values. If those n values happen to be exactly 1, 2, ..., n, then the
//   smallest absent positive is n + 1 — and no larger answer is reachable,
//   because producing an answer of n + 2 would require 1..n+1 all to be
//   present, which needs n + 1 slots. Therefore every value greater than n,
//   every zero and every negative is IRRELEVANT to the answer: it can never
//   be the answer, and its presence can never prevent a smaller value from
//   being the answer.
//
// Once that is established, the array IS the bounded-range array the pattern
// needs: values in [1, n] matter and have homes (value v -> index v - 1);
// everything else is noise to be left in place, exactly like the out-of-range
// values ../code.cpp already skips. The pattern's precondition was not given —
// it was constructed by proving the search space is [1, n + 1].
//
// Duplicates and noise together mean the input is a *partial* permutation of
// 1..n: some homes get filled, some do not, and the first unfilled home from
// the left is the answer.
//
// APPROACH — establish the range, cyclic-sort, then find the first gap
// --------------------------------------------------------------------
//   1. Sorting pass ([1..n] convention). For each cursor position: if the
//      value is in [1, n] and its home slot does not already hold that same
//      value, swap it home and re-check position i WITHOUT advancing.
//      Otherwise advance — which now genuinely fires, for three different
//      reasons: the value is out of range (negative, zero, or > n), it is
//      already home, or it is a duplicate whose home is taken.
//   2. Verification pass: return the first j (ascending) where nums[j] != j+1.
//      The answer is j + 1 — the value that never reached its home.
//   3. If no slot mismatches, then 1..n are all present and the answer is the
//      one candidate left: n + 1.
//
// Note how little changed from 01/02/03: the loop body is character-for-
// character the module's template. All the difficulty of this "hard" problem
// lives in step 0 — realising the search space is [1, n + 1] — not in the code.
//
// WHY THIS BEATS THE MARKING TRICK HERE
// --------------------------------------
// 02-find-all-numbers-disappeared-in-an-array.cpp discusses the sign-flip
// marking alternative. It can be made to work here too, but only after a
// preprocessing pass that overwrites every non-positive value with a harmless
// placeholder (typically n + 1), because the trick needs the sign bit free and
// this input can already contain negatives. Cyclic Sort needs no such
// preparation: its bounds check already leaves noise alone.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// Computing `correct_index = nums[i] - 1` BEFORE the range check. Two separate
// bugs come from that one habit:
//   - a value like 10^9 in a 3-element array produces an index of 999999999,
//     and the swap writes far outside the array — a segfault or silent memory
//     corruption, not a wrong answer.
//   - a value of INT_MIN makes `nums[i] - 1` overflow, which is undefined
//     behaviour: the compiler is entitled to assume it cannot happen and
//     optimise on that basis, so this can misbehave even when the resulting
//     index would never be dereferenced.
// The loop below therefore establishes `in_range` first and only derives
// `correct_index` inside that branch. The INT_MIN / INT_MAX test case in the
// suite exists specifically to pin this down. See ../README.md, "Common
// Mistakes", "Forgetting that input values can be duplicated or entirely out
// of range."
//
// COMPLEXITY
// ----------
// Time:  O(n) — total swaps across the whole run are bounded by n (each
//               successful swap permanently seats one value into its final
//               home), plus at most n cursor advances, plus the O(n) scan.
//               The nested-looking "swap without advancing" does NOT make this
//               quadratic; see ../README.md, "Interview Discussion", first
//               misconception.
// Space: O(1) extra — rearranged in place, no auxiliary structure.
// ============================================================================

#include <climits>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// cyclic_sort
//
// The module's standard pass ([1..n] convention: value v's home index is
// v - 1), inlined so this file stands alone. This is the file where the
// in-range guard does real work — arbitrary noise in the input must be left
// untouched rather than crashing the swap.
// ----------------------------------------------------------------------------
void cyclic_sort(std::vector<int>& nums) {
  const int n = static_cast<int>(nums.size());
  int i = 0;

  while (i < n) {
    // Establish the range FIRST. Nothing derived from nums[i] is computed
    // until we know nums[i] names a real slot in this array — that ordering
    // is what makes negatives, zeros, huge values and INT_MIN all safe.
    const bool in_range = nums[i] >= 1 && nums[i] <= n;

    if (in_range) {
      const int correct_index = nums[i] - 1;

      // Value comparison, not index comparison: duplicates are allowed here,
      // and an `i != correct_index` test would swap two equal values back and
      // forth forever.
      if (nums[i] != nums[correct_index]) {
        std::swap(nums[i], nums[correct_index]);
        continue;  // Do NOT advance: re-examine whatever just landed at i.
      }
    }

    ++i;  // Settled: out of range, already home, or a stalled duplicate.
  }
}

// ----------------------------------------------------------------------------
// firstMissingPositive
//
// Sorting pass, then the verification scan, then the n + 1 fallback. Takes
// `nums` by value so the tests below read cleanly; the O(1)-space claim is
// about allocating no structure proportional to n and holds for a
// by-reference call.
// ----------------------------------------------------------------------------
int firstMissingPositive(std::vector<int> nums) {
  const int n = static_cast<int>(nums.size());

  cyclic_sort(nums);

  // Verification pass. Scanning left to right matters: the FIRST unfilled home
  // is the smallest absent positive, so the answer is found at the earliest
  // mismatch, not at any mismatch.
  for (int j = 0; j < n; ++j) {
    if (nums[j] != j + 1) {
      return j + 1;
    }
  }

  // Every slot holds its own value, so 1..n are all present. By the [1, n + 1]
  // argument in the header, the answer can only be n + 1.
  return n + 1;
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
    check(firstMissingPositive(input) == expected, label);
  };

  // --- LeetCode's own examples -------------------------------------------
  check_result({1, 2, 0}, 3, "1..n-1 present, zero is noise -> answer is n");
  check_result({3, 4, -1, 1}, 2, "negative noise; 2 never reaches its home");
  check_result({7, 8, 9, 11, 12}, 1,
               "every value exceeds n -> nothing is placeable, answer is 1");

  // --- The n + 1 fallback: a clean, complete permutation -----------------
  check_result({1, 2, 3, 4}, 5, "complete permutation of 1..n -> n + 1");
  check_result({4, 3, 2, 1}, 5, "same, reverse sorted -> n + 1");
  check_result({2, 1}, 3, "two-element permutation -> n + 1");

  // --- Single element, every interesting shape ---------------------------
  check_result({1}, 2, "single element holding 1 -> n + 1");
  check_result({2}, 1, "single element out of range -> answer is 1");
  check_result({0}, 1, "single zero -> answer is 1");
  check_result({-5}, 1, "single negative -> answer is 1");

  // --- Duplicates, including all-same ------------------------------------
  check_result({1, 1}, 2, "duplicate stalls; 2's home is left unfilled");
  check_result({1, 1, 1, 1}, 2, "all-same at the range minimum");
  check_result({2, 2, 2}, 1, "all-same, and 1 is absent");
  check_result({1, 2, 2, 4}, 3, "duplicate plus a genuine gap");

  // --- Nothing positive at all -------------------------------------------
  check_result({-1, -2, -3}, 1, "all negative -> answer is 1");
  check_result({0, 0, 0}, 1, "all zero -> answer is 1");

  // --- Overflow / out-of-bounds bait -------------------------------------
  // If `correct_index` were computed before the range check, INT_MIN - 1 would
  // overflow (undefined behaviour) and INT_MAX - 1 would index far past the
  // end of a 3-element array.
  check_result({INT_MIN, INT_MAX, 1}, 2,
               "INT_MIN and INT_MAX as noise; only 1 is placeable");
  check_result({1000000000, 2, 1}, 3, "a huge value must never become an index");

  // --- Degenerate input --------------------------------------------------
  // Not reachable under LeetCode's constraints (n >= 1), but must return
  // cleanly: an empty array is missing 1.
  check_result({}, 1, "empty array -> 1 (n + 1 with n = 0)");

  // --- The sorting pass itself, inspected directly -----------------------
  {
    std::vector<int> nums = {3, 4, -1, 1};
    cyclic_sort(nums);
    // 1 and 3 reach their homes (indices 0 and 2); 4 reaches index 3; -1 is
    // noise and is simply parked wherever a swap left it — at index 1, which
    // is 2's home, and that unfilled home is the answer.
    std::vector<int> expected = {1, -1, 3, 4};
    check(nums == expected,
          "sorting pass: [3,4,-1,1] -> " + to_string(expected) +
              " (noise ends up parked in the unfilled home)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
