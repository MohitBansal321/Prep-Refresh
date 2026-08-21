// ============================================================================
// LeetCode 78 — Subsets
// https://leetcode.com/problems/subsets/
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array nums of UNIQUE elements, return all possible subsets
// (the power set). The solution set must not contain duplicate subsets;
// subsets may be returned in any order.
//
// Example: nums = [1,2,3]
//   -> [[], [1], [2], [1,2], [3], [1,3], [2,3], [1,2,3]]   (8 = 2^3 subsets)
//
// APPROACH — the Subsets pattern in its purest form, three ways
// --------------------------------------------------------------
// Because the input has no duplicates and every subset is valid output, there
// is nothing to check and nothing to prune: the ONLY job is to enumerate all
// 2^n combinations of yes/no decisions exactly once each. This file gives all
// three mechanics from ../README.md ("Solution") and asserts they agree:
//
//   1. subsetsRecursive — include/exclude recursion. At index i, recurse once
//      WITHOUT nums[i] and once WITH it (push / recurse / pop). Record a copy
//      of the shared buffer when every index has been decided (i == n).
//      This is the shape Backtracking extends by adding a validity check.
//
//   2. subsetsIterative — BFS-style doubling. Start with [[]]; for each new
//      element, append a copy of every subset that already exists with the
//      element added. n passes, result doubles each pass: 1 -> 2 -> 4 -> ...
//
//   3. subsetsBitmask — every subset corresponds to exactly one integer in
//      [0, 2^n): bit i of the mask means "nums[i] is in this subset." Walking
//      mask = 0 .. 2^n - 1 therefore walks every subset exactly once, with no
//      recursion and no growing-list bookkeeping at all.
//
// All three produce the same 2^n subsets. They differ only in VISIT ORDER --
// see ../images/flow-diagram.md for why the recursion emits masks in the order
// 0,4,2,6,1,5,3,7 while the bitmask loop emits them as 0,1,2,...,7.
//
// COMMON MISTAKES THIS FILE GUARDS AGAINST
// -----------------------------------------
// (a) Dropping the empty subset. {} is a legitimate member of the power set
//     and LeetCode requires it. The iterative version must START from {{}}
//     (not {}), and the recursive base case must record `current` even when
//     it is empty.
// (b) Re-reading result.size() inside the doubling loop. The loop appends to
//     the very container it is iterating, so the bound must be SNAPSHOTTED
//     first; otherwise the loop reaches its own freshly-appended entries and
//     never terminates. See ../images/trace-diagram.md.
// (c) Recording a reference/pointer to the shared `current` buffer instead of
//     a copy. `current` keeps mutating as the recursion continues, so all
//     recorded answers would end up aliasing one (eventually empty) vector.
// (d) Shifting a signed 1 in the bitmask version. `1 << 31` is undefined
//     behaviour on a 32-bit signed int; use an unsigned literal (1u << i).
//
// COMPLEXITY
// ----------
// Time:  O(2^n * n) -- 2^n subsets, each up to n elements to build/copy.
// Space: O(2^n * n) for the output (unavoidable: that IS the answer's size),
//        plus O(n) auxiliary for the recursion stack and `current` buffer.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// 1. Recursive include/exclude.
//
// Invariant on `index`: every element before `index` has already had an
// include/exclude decision made for it (baked into `current`); every element
// from `index` onward is still undecided. That is why the base case is
// `index == nums.size()` -- "all decisions made" -- and NOT "current is full."
// A subset can validly have any length from 0 to n, so there is no notion of
// `current` being full here. (Contrast 03-permutations.cpp, whose base case
// genuinely is current.size() == nums.size().)
// ----------------------------------------------------------------------------
void subsetsRecursiveHelper(const std::vector<int>& nums, size_t index,
                            std::vector<int>& current,
                            std::vector<std::vector<int>>& result) {
  if (index == nums.size()) {
    // Every element decided -> `current` is one complete subset.
    // push_back COPIES the vector by value. That copy is mandatory: `current`
    // is a single buffer shared by every call, and it will be mutated again
    // the moment we return.
    result.push_back(current);
    return;
  }

  // Branch 1: EXCLUDE nums[index]. Nothing to add and nothing to undo -- just
  // move the decision marker forward.
  subsetsRecursiveHelper(nums, index + 1, current, result);

  // Branch 2: INCLUDE nums[index]. choose -> recurse -> UNDO.
  // The pop_back is what keeps the sibling branches independent. Without it,
  // `current` would still be carrying nums[index] when control returns to the
  // caller, contaminating every subset produced by branches that come later.
  current.push_back(nums[index]);
  subsetsRecursiveHelper(nums, index + 1, current, result);
  current.pop_back();
}

std::vector<std::vector<int>> subsetsRecursive(const std::vector<int>& nums) {
  std::vector<std::vector<int>> result;
  std::vector<int> current;
  subsetsRecursiveHelper(nums, 0, current, result);
  return result;
}

// ----------------------------------------------------------------------------
// 2. Iterative doubling (BFS-style).
//
// Loop invariant: after the pass that folds in the k-th element, `result`
// holds exactly the 2^k subsets of the first k elements. Proof in one line: a
// subset of the first k elements either contains element k or it does not --
// the untouched entries cover "does not," the freshly appended copies cover
// "does," and the two halves cannot overlap.
// ----------------------------------------------------------------------------
std::vector<std::vector<int>> subsetsIterative(const std::vector<int>& nums) {
  // Starting from {{}} rather than {} is what puts the empty subset in the
  // answer. Start from {} and the outer loop has nothing to double, so the
  // function returns an empty list no matter what the input is.
  std::vector<std::vector<int>> result = {{}};

  for (size_t k = 0; k < nums.size(); ++k) {
    // THE critical line. `result` grows inside the inner loop below, so its
    // size must be frozen here. Writing `i < result.size()` instead would let
    // the loop reach entries it just appended and extend them a second time
    // with the same element -- runaway growth, not a subtle off-by-one.
    const size_t existing_count = result.size();

    for (size_t i = 0; i < existing_count; ++i) {
      std::vector<int> extended = result[i];  // copy, do NOT mutate result[i]
      extended.push_back(nums[k]);
      result.push_back(std::move(extended));
    }
  }

  return result;
}

// ----------------------------------------------------------------------------
// 3. Bitmask enumeration.
//
// One subset <-> one integer. Bit i of `mask` set means "nums[i] is included."
// Since every n-bit integer names a distinct include/exclude pattern and
// there are exactly 2^n of them, counting from 0 to 2^n - 1 enumerates the
// power set with no recursion and no auxiliary state.
//
// Practical ceiling: `mask` must fit in an unsigned int, so n <= 31 here --
// which is not a real restriction, because 2^31 subsets is already far beyond
// what any machine could materialize.
// ----------------------------------------------------------------------------
std::vector<std::vector<int>> subsetsBitmask(const std::vector<int>& nums) {
  const unsigned n = static_cast<unsigned>(nums.size());
  const unsigned total = 1u << n;  // 2^n; note 1u (UNSIGNED) -- 1 << 31 on a
                                   // signed int is undefined behaviour.

  std::vector<std::vector<int>> result;
  result.reserve(total);

  for (unsigned mask = 0; mask < total; ++mask) {
    std::vector<int> subset;
    for (unsigned i = 0; i < n; ++i) {
      if (mask & (1u << i)) {
        subset.push_back(nums[i]);
      }
    }
    result.push_back(std::move(subset));
  }

  return result;
}

// ============================================================================
// Test harness
// ============================================================================

// Subsets may be returned in any order, and the elements within a subset may
// also appear in any order, so every comparison below normalizes first: sort
// each inner vector, then sort the outer vector. This is the only honest way
// to compare two enumerations that are equal as SETS but differ in visit order.
std::vector<std::vector<int>> normalized(std::vector<std::vector<int>> v) {
  for (size_t i = 0; i < v.size(); ++i) {
    std::sort(v[i].begin(), v[i].end());
  }
  std::sort(v.begin(), v.end());
  return v;
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

  // --- The LeetCode example, checked against the exact expected power set ---
  {
    std::vector<int> nums;
    nums.push_back(1);
    nums.push_back(2);
    nums.push_back(3);

    std::vector<std::vector<int>> expected;
    expected.push_back(std::vector<int>());               // {}
    expected.push_back(std::vector<int>{1});
    expected.push_back(std::vector<int>{2});
    expected.push_back(std::vector<int>{3});
    expected.push_back(std::vector<int>{1, 2});
    expected.push_back(std::vector<int>{1, 3});
    expected.push_back(std::vector<int>{2, 3});
    expected.push_back(std::vector<int>{1, 2, 3});

    std::vector<std::vector<int>> rec = subsetsRecursive(nums);
    std::vector<std::vector<int>> itr = subsetsIterative(nums);
    std::vector<std::vector<int>> bit = subsetsBitmask(nums);

    check(rec.size() == 8, "[1,2,3] recursive -> 2^3 = 8 subsets");
    check(itr.size() == 8, "[1,2,3] iterative -> 2^3 = 8 subsets");
    check(bit.size() == 8, "[1,2,3] bitmask   -> 2^3 = 8 subsets");

    check(normalized(rec) == normalized(expected),
          "[1,2,3] recursive matches the exact expected power set");
    check(normalized(itr) == normalized(expected),
          "[1,2,3] iterative matches the exact expected power set");
    check(normalized(bit) == normalized(expected),
          "[1,2,3] bitmask matches the exact expected power set");

    // The three framings visit the same leaves in DIFFERENT orders. Proving
    // they agree only after normalizing is the point -- it confirms the
    // README's claim that they are one enumeration, not rival algorithms.
    check(normalized(rec) == normalized(itr) && normalized(itr) == normalized(bit),
          "all three framings agree as sets (visit order differs)");
    check(!(rec == itr),
          "recursive and iterative genuinely differ in visit order (0,4,2,6,... vs 0,1,2,...)");
  }

  // --- Edge case: empty input. The power set of {} is {{}} -- ONE subset, ---
  // --- the empty one. Not zero subsets. This is the "forgot the empty    ---
  // --- subset" bug at its most visible.                                  ---
  {
    std::vector<int> empty_input;
    check(subsetsRecursive(empty_input).size() == 1 &&
              subsetsRecursive(empty_input)[0].empty(),
          "empty input, recursive -> exactly one subset, the empty set");
    check(subsetsIterative(empty_input).size() == 1 &&
              subsetsIterative(empty_input)[0].empty(),
          "empty input, iterative -> exactly one subset, the empty set");
    check(subsetsBitmask(empty_input).size() == 1 &&
              subsetsBitmask(empty_input)[0].empty(),
          "empty input, bitmask -> exactly one subset (mask 0 only)");
  }

  // --- Edge case: single element, including the value 0 (a value that is  ---
  // --- easy to confuse with "absent" if anything is falsy-tested).        ---
  {
    std::vector<int> single;
    single.push_back(0);

    std::vector<std::vector<int>> expected;
    expected.push_back(std::vector<int>());
    expected.push_back(std::vector<int>{0});

    check(normalized(subsetsRecursive(single)) == normalized(expected),
          "single element [0] -> {} and {0} (zero is a value, not an absence)");
    check(normalized(subsetsBitmask(single)) == normalized(expected),
          "single element [0], bitmask -> {} and {0}");
  }

  // --- Edge case: negative values. Nothing in the pattern inspects the   ---
  // --- values at all -- only positions -- so signs are irrelevant. This  ---
  // --- test exists to make that indifference explicit.                   ---
  {
    std::vector<int> negatives;
    negatives.push_back(-3);
    negatives.push_back(0);
    negatives.push_back(7);

    std::vector<std::vector<int>> rec = subsetsRecursive(negatives);
    check(rec.size() == 8, "[-3,0,7] -> 8 subsets (values never inspected, only positions)");
    check(normalized(rec) == normalized(subsetsBitmask(negatives)),
          "[-3,0,7] recursive and bitmask agree");
  }

  // --- Structural checks on a larger input: exact count, and no duplicate ---
  // --- subsets anywhere in the output.                                     ---
  {
    std::vector<int> ten;
    for (int i = 1; i <= 10; ++i) ten.push_back(i);

    std::vector<std::vector<int>> rec = subsetsRecursive(ten);
    check(rec.size() == 1024u, "10 distinct elements -> 2^10 = 1024 subsets");

    std::vector<std::vector<int>> norm = normalized(rec);
    bool has_duplicate = false;
    for (size_t i = 1; i < norm.size(); ++i) {
      if (norm[i] == norm[i - 1]) has_duplicate = true;
    }
    check(!has_duplicate, "no duplicate subsets among all 1024 (distinct input)");

    // Every element must appear in exactly half of all subsets -- 2^(n-1) of
    // them -- because its own decision is independent of all the others. This
    // is the counting identity behind "each element doubles the result."
    int count_containing_first = 0;
    for (size_t i = 0; i < rec.size(); ++i) {
      if (std::find(rec[i].begin(), rec[i].end(), 1) != rec[i].end()) {
        ++count_containing_first;
      }
    }
    check(count_containing_first == 512,
          "element 1 appears in exactly 2^(n-1) = 512 subsets (decisions are independent)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
