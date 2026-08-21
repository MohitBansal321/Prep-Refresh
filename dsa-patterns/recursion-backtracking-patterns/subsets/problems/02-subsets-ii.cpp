// ============================================================================
// LeetCode 90 — Subsets II
// https://leetcode.com/problems/subsets-ii/
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array nums that MAY CONTAIN DUPLICATES, return all
// possible subsets (the power set). The solution set must not contain
// duplicate subsets; subsets may be returned in any order.
//
// Example: nums = [1,2,2]
//   -> [[], [1], [2], [1,2], [2,2], [1,2,2]]     -- 6 subsets, NOT 2^3 = 8
//
// Why 6 and not 8: the plain power set of three POSITIONS has 8 members, but
// positions 1 and 2 hold the same VALUE, so "take position 1" and "take
// position 2" describe the same subset {2}, and "take 1, take position 1" and
// "take 1, take position 2" both describe {1,2}. Two of the eight are
// duplicates, leaving 6 distinct subsets.
//
// APPROACH — sort, then skip same-level duplicates
// -------------------------------------------------
// The dedup rule IS this problem. Everything else is 01-subsets.cpp.
//
// Step 1: SORT. Duplicates must be adjacent for any "is this the same as the
//         previous one" test to work. Without sorting, [2,1,2] would hide its
//         two 2s on either side of the 1 and no local comparison could catch
//         them.
//
// Step 2: switch from the two-branch include/exclude shape to the START-INDEX
//         LOOP shape. Instead of "decide element i, then recurse," this shape
//         asks "which element comes NEXT in the subset I am building?" and
//         loops over every candidate from `start` onward. Every node of this
//         tree is itself a complete valid subset, so `current` is recorded at
//         the TOP of every call rather than only at a base case.
//
//         This reshaping is not cosmetic: the dedup rule needs a notion of
//         "sibling branches at one level," and the loop makes those siblings
//         explicit as the iterations of one for-loop. In the two-branch shape
//         there is no loop to compare against.
//
// Step 3: THE RULE.  if (i > start && nums[i] == nums[i - 1]) continue;
//
//         Read it as: "within this one loop (this one recursion level), only
//         the FIRST occurrence of a given value is allowed to start a
//         branch." The `i > start` guard is what limits the comparison to
//         siblings -- at i == start there is no earlier sibling, so that
//         occurrence always runs.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// The rule is NOT "don't reuse a value already present in `current`." For
// input [1,2,2] the subset {2,2} is REQUIRED output -- the same value must be
// allowed to repeat DOWN a branch (take nums[1], then nums[2]). What must be
// forbidden is two SIBLING branches at the same level both starting with 2,
// because their entire subtrees would then be identical.
//
// Concretely, for sorted [1,2,2] at the recursion level with start = 1:
//   i = 1 (value 2): i == start, so it runs. Explores {2} and {2,2}.
//   i = 2 (value 2): i > start AND nums[2] == nums[1], so SKIP. Had it run,
//                    it would have explored {2} a second time -- an exact
//                    duplicate of what i = 1 already produced.
// And at the level with start = 2 (reached from inside i = 1's branch):
//   i = 2 (value 2): i == start, so it RUNS -- this is what produces {2,2}.
// Same index, same value, opposite decision, because the two calls are at
// different levels. That is the whole rule in one example.
//
// This file also implements a second, independent formulation (group the
// sorted array into runs of equal values and choose HOW MANY copies of each
// run to take, 0..runLength) and asserts the two agree -- a cross-check that
// the skip condition is not accidentally dropping or duplicating anything.
// A deliberately UNGUARDED version is included too, only to demonstrate the
// over-generation the guard prevents.
//
// COMPLEXITY
// ----------
// Time:  O(2^n * n) worst case -- when all values are distinct, nothing is
//        skipped and this degenerates to the plain power set. With duplicates
//        the output (and therefore the runtime) is strictly smaller: for k
//        distinct values with multiplicities m_1..m_k, the answer has exactly
//        (m_1 + 1) * ... * (m_k + 1) subsets. Sorting adds O(n log n), which
//        is dominated.
// Space: O(2^n * n) output, plus O(n) for the recursion stack and `current`.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Formulation A — sort + start-index loop + skip same-level duplicates.
// This is the canonical answer and the one to be able to write from memory.
// `nums` MUST already be sorted when this is called.
// ----------------------------------------------------------------------------
void subsetsWithDupHelper(const std::vector<int>& nums, size_t start,
                          std::vector<int>& current,
                          std::vector<std::vector<int>>& result) {
  // Every node in this tree is a complete, valid subset -- record on entry.
  // (Contrast 01-subsets.cpp, where only the i == n base case is recorded,
  // because there a node means "some elements still undecided.")
  result.push_back(current);

  for (size_t i = start; i < nums.size(); ++i) {
    // ---- THE ENTIRE LESSON OF THIS PROBLEM ----
    // Skip nums[i] only if it duplicates the value an EARLIER SIBLING in this
    // same loop already branched on. `i > start` is what restricts the
    // comparison to siblings: at i == start, nums[i - 1] belongs to the
    // parent's branch, not to a sibling, so it must not be compared against.
    if (i > start && nums[i] == nums[i - 1]) continue;

    current.push_back(nums[i]);
    subsetsWithDupHelper(nums, i + 1, current, result);
    current.pop_back();  // undo, so the next sibling starts clean
  }
}

std::vector<std::vector<int>> subsetsWithDup(std::vector<int> nums) {
  // Taken by value and sorted here: duplicates must be adjacent for the skip
  // test above to see them at all.
  std::sort(nums.begin(), nums.end());

  std::vector<std::vector<int>> result;
  std::vector<int> current;
  subsetsWithDupHelper(nums, 0, current, result);
  return result;
}

// ----------------------------------------------------------------------------
// Formulation B — group into runs of equal values, then choose a COUNT.
//
// A different way to see the same answer: after sorting, the array is a
// sequence of runs of equal values. A distinct subset is fully determined by
// how many copies of each value it takes -- 0 to runLength for each run --
// because copies of the same value are interchangeable. So instead of two
// branches per element, there are (runLength + 1) branches per RUN.
//
// This makes the output count obvious: the product of (multiplicity + 1) over
// all distinct values. For [1,2,2] that is (1+1) * (2+1) = 6. It also makes
// duplicate-freeness structural rather than a rule to remember: two different
// count-vectors always describe two different subsets.
//
// Formulation A is what to write in an interview (it is shorter and it
// generalizes to combination-sum-style problems); B is what to reach for when
// explaining WHY A is correct, or when computing the expected count in a test.
// ----------------------------------------------------------------------------
void subsetsByRunCountHelper(const std::vector<int>& nums, size_t run_start,
                             std::vector<int>& current,
                             std::vector<std::vector<int>>& result) {
  if (run_start == nums.size()) {
    result.push_back(current);
    return;
  }

  // Find the extent of the run of equal values beginning at run_start.
  size_t run_end = run_start;
  while (run_end < nums.size() && nums[run_end] == nums[run_start]) ++run_end;
  const size_t run_length = run_end - run_start;

  // Branch on how many copies of this value to take: 0, 1, ..., run_length.
  for (size_t take = 0; take <= run_length; ++take) {
    for (size_t t = 0; t < take; ++t) current.push_back(nums[run_start]);
    subsetsByRunCountHelper(nums, run_end, current, result);
    for (size_t t = 0; t < take; ++t) current.pop_back();  // undo exactly what was added
  }
}

std::vector<std::vector<int>> subsetsByRunCount(std::vector<int> nums) {
  std::sort(nums.begin(), nums.end());
  std::vector<std::vector<int>> result;
  std::vector<int> current;
  subsetsByRunCountHelper(nums, 0, current, result);
  return result;
}

// ----------------------------------------------------------------------------
// Formulation C — DELIBERATELY WRONG: the same loop with the skip removed.
//
// Kept only so the test suite can demonstrate the exact failure mode: this
// returns 2^n subsets (one per position-subset) rather than the number of
// distinct VALUE-subsets, so [1,2,2] yields 8 with {2} and {1,2} each
// appearing twice. Never write this as a solution to LeetCode 90.
// ----------------------------------------------------------------------------
void subsetsNoDedupHelper(const std::vector<int>& nums, size_t start,
                          std::vector<int>& current,
                          std::vector<std::vector<int>>& result) {
  result.push_back(current);
  for (size_t i = start; i < nums.size(); ++i) {
    current.push_back(nums[i]);
    subsetsNoDedupHelper(nums, i + 1, current, result);
    current.pop_back();
  }
}

std::vector<std::vector<int>> subsetsNoDedup(std::vector<int> nums) {
  std::sort(nums.begin(), nums.end());
  std::vector<std::vector<int>> result;
  std::vector<int> current;
  subsetsNoDedupHelper(nums, 0, current, result);
  return result;
}

// ============================================================================
// Test harness
// ============================================================================

std::vector<std::vector<int>> normalized(std::vector<std::vector<int>> v) {
  for (size_t i = 0; i < v.size(); ++i) {
    std::sort(v[i].begin(), v[i].end());
  }
  std::sort(v.begin(), v.end());
  return v;
}

// Counts how many entries survive de-duplication -- used to prove that
// formulation A emits no duplicates in the first place, and that formulation
// C emits several.
size_t distinctCount(std::vector<std::vector<int>> v) {
  v = normalized(v);
  v.erase(std::unique(v.begin(), v.end()), v.end());
  return v.size();
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

  // --- The LeetCode example, checked against the exact expected answer ---
  {
    std::vector<int> nums;
    nums.push_back(1);
    nums.push_back(2);
    nums.push_back(2);

    std::vector<std::vector<int>> expected;
    expected.push_back(std::vector<int>());            // {}
    expected.push_back(std::vector<int>{1});
    expected.push_back(std::vector<int>{2});
    expected.push_back(std::vector<int>{1, 2});
    expected.push_back(std::vector<int>{2, 2});
    expected.push_back(std::vector<int>{1, 2, 2});

    std::vector<std::vector<int>> got = subsetsWithDup(nums);
    check(got.size() == 6, "[1,2,2] -> 6 distinct subsets (not 2^3 = 8)");
    check(normalized(got) == normalized(expected),
          "[1,2,2] matches the exact expected answer, including {2,2}");
    check(distinctCount(got) == got.size(),
          "[1,2,2] output contains no duplicates at all (not merely dedupable)");

    // The point about {2,2}: the same VALUE repeating down one branch is
    // required output. The rule forbids sibling duplicates, not repeats.
    bool has_two_twos = false;
    for (size_t i = 0; i < got.size(); ++i) {
      if (got[i].size() == 2 && got[i][0] == 2 && got[i][1] == 2) has_two_twos = true;
    }
    check(has_two_twos, "{2,2} IS produced -- the rule blocks siblings, not repeats down a branch");
  }

  // --- The unguarded version over-generates by exactly the duplicates ---
  {
    std::vector<int> nums;
    nums.push_back(1);
    nums.push_back(2);
    nums.push_back(2);

    std::vector<std::vector<int>> bad = subsetsNoDedup(nums);
    check(bad.size() == 8, "unguarded version emits 2^3 = 8 (one per POSITION-subset)");
    check(distinctCount(bad) == 6, "of those 8, only 6 are distinct -- 2 are duplicates");
    check(normalized(subsetsWithDup(nums)).size() == distinctCount(bad),
          "the guard emits exactly the distinct set, with zero wasted work");
  }

  // --- All values identical: the count collapses from 2^n to n+1 ---
  {
    std::vector<int> ones;
    ones.push_back(1);
    ones.push_back(1);
    ones.push_back(1);

    std::vector<std::vector<int>> got = subsetsWithDup(ones);
    check(got.size() == 4, "[1,1,1] -> 4 subsets: {}, {1}, {1,1}, {1,1,1} (n+1, not 2^n)");
    check(distinctCount(got) == 4, "[1,1,1] output has no duplicates");
  }

  // --- Duplicates NOT adjacent in the input: proves sorting is load-bearing ---
  {
    std::vector<int> scattered;
    scattered.push_back(4);
    scattered.push_back(4);
    scattered.push_back(4);
    scattered.push_back(1);
    scattered.push_back(4);

    // Sorted -> [1,4,4,4,4]. Distinct subsets = (1+1) * (4+1) = 10.
    std::vector<std::vector<int>> got = subsetsWithDup(scattered);
    check(got.size() == 10, "[4,4,4,1,4] -> 10 subsets = (1+1)*(4+1), duplicates found only after sorting");
    check(distinctCount(got) == 10, "[4,4,4,1,4] output has no duplicates");
  }

  // --- Distinct input: must degenerate to the plain power set of 01-subsets ---
  {
    std::vector<int> distinct;
    distinct.push_back(1);
    distinct.push_back(2);
    distinct.push_back(3);
    check(subsetsWithDup(distinct).size() == 8,
          "all-distinct input -> nothing is skipped, plain 2^3 = 8 power set");
  }

  // --- Edge cases: empty input and a single element ---
  {
    std::vector<int> empty_input;
    std::vector<std::vector<int>> got = subsetsWithDup(empty_input);
    check(got.size() == 1 && got[0].empty(), "empty input -> exactly one subset, the empty set");

    std::vector<int> single;
    single.push_back(0);
    check(subsetsWithDup(single).size() == 2, "single element [0] -> {} and {0}");
  }

  // --- Negatives mixed with duplicates ---
  {
    std::vector<int> mixed;
    mixed.push_back(-1);
    mixed.push_back(-1);
    mixed.push_back(0);
    // Sorted -> [-1,-1,0]. Distinct subsets = (2+1) * (1+1) = 6.
    check(subsetsWithDup(mixed).size() == 6,
          "[-1,-1,0] -> 6 subsets = (2+1)*(1+1) (signs are irrelevant to the rule)");
  }

  // --- Cross-check formulations A and B on several inputs ---
  {
    std::vector<std::vector<int>> inputs;
    inputs.push_back(std::vector<int>{1, 2, 2});
    inputs.push_back(std::vector<int>{1, 1, 1});
    inputs.push_back(std::vector<int>{4, 4, 4, 1, 4});
    inputs.push_back(std::vector<int>{5, 5, 3, 3, 3, 9});
    inputs.push_back(std::vector<int>{1, 2, 3, 4});
    inputs.push_back(std::vector<int>());

    bool all_agree = true;
    for (size_t i = 0; i < inputs.size(); ++i) {
      if (normalized(subsetsWithDup(inputs[i])) != normalized(subsetsByRunCount(inputs[i]))) {
        all_agree = false;
      }
    }
    check(all_agree,
          "skip-same-level-duplicate and choose-a-count-per-run agree on all 6 test inputs");

    // [5,5,3,3,3,9] -> sorted [3,3,3,5,5,9] -> (3+1)*(2+1)*(1+1) = 24.
    check(subsetsWithDup(inputs[3]).size() == 24,
          "[5,5,3,3,3,9] -> 24 subsets = (3+1)*(2+1)*(1+1)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
