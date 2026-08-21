// ============================================================================
// LeetCode 39 — Combination Sum
// https://leetcode.com/problems/combination-sum/
// ============================================================================
//
// PROBLEM
// -------
// Given an array of DISTINCT integers `candidates` and a target integer
// `target`, return a list of all UNIQUE combinations of candidates where the
// chosen numbers sum to target. The SAME candidate may be chosen an UNLIMITED
// number of times. Two combinations are the same if they use the same numbers
// with the same multiplicities, regardless of order.
//
// Example: candidates = [2,3,6,7], target = 7
//   -> [[2,2,3], [7]]
// Example: candidates = [2,3,5], target = 8
//   -> [[2,2,2,2], [2,3,3], [3,5]]
//
// WHY THIS FILE IS LAST IN THE MODULE
// ------------------------------------
// This is the first problem here where a PARTIAL answer can be proven doomed,
// which is exactly the line between Subsets and Backtracking (see
// ../images/recognition-diagram.md, third diamond). Everything structural is
// still the Subsets skeleton from 02-subsets-ii.cpp -- sorted input, a start
// index, a shared `current` buffer, push / recurse / pop. Two things are new:
//
//   1. UNBOUNDED REUSE. Recurse with `i` instead of `i + 1`. That single
//      character is the difference between "each candidate may be used at most
//      once" (Subsets II) and "each candidate may be used any number of
//      times." Passing `i` keeps the current candidate available at the next
//      level; passing `i + 1` retires it.
//
//   2. A REAL PRUNE. Once the input is sorted ascending, `candidates[i] >
//      remaining` means every later candidate is also too big, so the loop can
//      `break` -- abandoning not just this branch but every remaining sibling
//      at this level. This is monotone pruning: the remaining target only ever
//      shrinks as you go deeper (all candidates are positive), so a partial
//      sum that has overshot can NEVER come back down. Contrast a non-monotone
//      constraint like "every building's net transfer is zero" (LeetCode 1601,
//      in ../exercises.md), where adding more choices CAN restore validity and
//      therefore nothing may be pruned.
//
// WHY THE START INDEX IS WHAT PREVENTS DUPLICATES
// ------------------------------------------------
// `candidates` has no duplicate VALUES, so there is no sort-and-skip rule to
// apply here. The duplicate risk is different: [2,2,3] and [2,3,2] and [3,2,2]
// are the same combination, and a loop that always started at index 0 would
// emit all three. Recursing from `i` (never from anything smaller) forces
// every combination to be built in non-decreasing index order, so exactly one
// of those three orderings is ever constructed. The start index is doing the
// deduplication -- the same job `i > start` does in 02-subsets-ii.cpp, by a
// different mechanism.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// Recursing with `i + 1` when unlimited reuse is required. It compiles, it
// returns plausible-looking output, and for candidates = [2,3,6,7], target = 7
// it returns only [[7]] -- silently dropping [2,2,3], because 2 was retired
// after its first use. The failure is a MISSING answer, not a malformed one,
// so it survives a casual eyeball check. The test suite below asserts against
// exact expected answers precisely to catch this class of bug.
//
// A second trap: `if (remaining < 0) return;` at the top of the call is
// correct but is NOT the prune. It rejects a node only after paying for the
// call. The `break` on a sorted array rejects the node plus all its siblings
// before any of them is entered. The tests below count visited nodes both
// ways to make the difference concrete.
//
// COMPLEXITY
// ----------
// Hard to state tightly, because it depends on target and on the smallest
// candidate. A standard bound: O(n^(target/min_candidate)) time -- the
// recursion tree has depth at most target/min_candidate (each level adds at
// least min_candidate to the sum) and branching factor at most n. Space is
// O(target/min_candidate) for the recursion stack and `current`, plus the
// output size. Sorting adds O(n log n), dominated by everything else.
//
// The practical reading: this is exponential in target/min_candidate, not in
// n, which is why LeetCode's constraints cap target at 40 rather than capping
// the array length aggressively.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// The solution: sorted candidates, start index, unbounded reuse, break-prune.
//
// `nodes_visited` is threaded through only so the tests can measure pruning;
// it plays no part in the algorithm and would not appear in an interview
// answer.
// ----------------------------------------------------------------------------
void combinationSumHelper(const std::vector<int>& candidates /* SORTED asc */,
                          size_t start, int remaining,
                          std::vector<int>& current,
                          std::vector<std::vector<int>>& result,
                          long& nodes_visited) {
  ++nodes_visited;

  if (remaining == 0) {
    // `current` sums to exactly target -> one complete combination. Copy it;
    // `current` is a shared buffer and keeps mutating.
    result.push_back(current);
    return;
  }

  for (size_t i = start; i < candidates.size(); ++i) {
    // THE PRUNE. candidates is sorted ascending, so if this one already
    // overshoots the remaining target, so does every candidate after it --
    // `break`, not `continue`. All positive candidates means `remaining` is
    // monotonically non-increasing as we descend, so an overshoot is
    // permanent and the whole tail of this level is dead.
    if (candidates[i] > remaining) break;

    current.push_back(candidates[i]);

    // Recurse from `i`, NOT `i + 1`: candidates[i] stays available so it can
    // be chosen again. Starting at `i` (never lower) is simultaneously what
    // forbids the reordered duplicates [2,3,2] and [3,2,2].
    combinationSumHelper(candidates, i, remaining - candidates[i], current,
                         result, nodes_visited);

    current.pop_back();  // undo before the next sibling
  }
}

std::vector<std::vector<int>> combinationSum(std::vector<int> candidates,
                                             int target,
                                             long& nodes_visited) {
  // Sorting is what makes the `break` legal. Without it the prune would have
  // to be a `continue` (correct but far weaker), since a large candidate
  // could be followed by a small usable one.
  std::sort(candidates.begin(), candidates.end());

  std::vector<std::vector<int>> result;
  std::vector<int> current;
  nodes_visited = 0;
  combinationSumHelper(candidates, 0, target, current, result, nodes_visited);
  return result;
}

// Convenience overload for callers that do not care about the node count.
std::vector<std::vector<int>> combinationSum(std::vector<int> candidates,
                                             int target) {
  long ignored = 0;
  return combinationSum(std::move(candidates), target, ignored);
}

// ----------------------------------------------------------------------------
// The UNPRUNED version, for comparison only.
//
// Same enumeration, same output, but it detects overshoot by entering the node
// and testing `remaining < 0` rather than refusing to enter it. Correct, and
// measurably more work. Kept so the test suite can show the prune's value as a
// number instead of an assertion.
// ----------------------------------------------------------------------------
void combinationSumUnprunedHelper(const std::vector<int>& candidates,
                                  size_t start, int remaining,
                                  std::vector<int>& current,
                                  std::vector<std::vector<int>>& result,
                                  long& nodes_visited) {
  ++nodes_visited;

  if (remaining == 0) {
    result.push_back(current);
    return;
  }
  if (remaining < 0) return;  // detected only AFTER paying for the call

  for (size_t i = start; i < candidates.size(); ++i) {
    current.push_back(candidates[i]);
    combinationSumUnprunedHelper(candidates, i, remaining - candidates[i],
                                 current, result, nodes_visited);
    current.pop_back();
  }
}

std::vector<std::vector<int>> combinationSumUnpruned(std::vector<int> candidates,
                                                     int target,
                                                     long& nodes_visited) {
  std::sort(candidates.begin(), candidates.end());
  std::vector<std::vector<int>> result;
  std::vector<int> current;
  nodes_visited = 0;
  combinationSumUnprunedHelper(candidates, 0, target, current, result,
                               nodes_visited);
  return result;
}

// ----------------------------------------------------------------------------
// The classic BUG, kept only so a test can pin its exact failure mode:
// recursing from `i + 1` retires each candidate after one use, turning this
// into "each candidate at most once" (a different problem entirely).
// ----------------------------------------------------------------------------
void combinationSumNoReuseHelper(const std::vector<int>& candidates,
                                 size_t start, int remaining,
                                 std::vector<int>& current,
                                 std::vector<std::vector<int>>& result) {
  if (remaining == 0) {
    result.push_back(current);
    return;
  }
  for (size_t i = start; i < candidates.size(); ++i) {
    if (candidates[i] > remaining) break;
    current.push_back(candidates[i]);
    combinationSumNoReuseHelper(candidates, i + 1, remaining - candidates[i],
                                current, result);  // i + 1 == the bug
    current.pop_back();
  }
}

std::vector<std::vector<int>> combinationSumNoReuse(std::vector<int> candidates,
                                                    int target) {
  std::sort(candidates.begin(), candidates.end());
  std::vector<std::vector<int>> result;
  std::vector<int> current;
  combinationSumNoReuseHelper(candidates, 0, target, current, result);
  return result;
}

// ============================================================================
// Test harness
// ============================================================================

// Combinations may be returned in any order. Each combination is already built
// in non-decreasing value order by construction (the start index guarantees
// it), so only the outer vector needs sorting -- but the inner sort is kept
// anyway so a regression that breaks the ordering guarantee still compares
// correctly on content, and is caught by its own dedicated test instead.
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

  // --- LeetCode example 1: candidates = [2,3,6,7], target = 7 ---
  {
    std::vector<int> candidates;
    candidates.push_back(2);
    candidates.push_back(3);
    candidates.push_back(6);
    candidates.push_back(7);

    std::vector<std::vector<int>> expected;
    expected.push_back(std::vector<int>{2, 2, 3});
    expected.push_back(std::vector<int>{7});

    std::vector<std::vector<int>> got = combinationSum(candidates, 7);
    check(normalized(got) == normalized(expected),
          "[2,3,6,7] target 7 -> exactly [[2,2,3],[7]]");

    // The i-vs-(i+1) bug: 2 gets retired after one use, so [2,2,3] vanishes
    // and only [7] survives. A missing answer, not a malformed one.
    std::vector<std::vector<int>> buggy = combinationSumNoReuse(candidates, 7);
    check(buggy.size() == 1 && buggy[0].size() == 1 && buggy[0][0] == 7,
          "recursing from i+1 silently drops [2,2,3] -- reuse requires i, not i+1");
  }

  // --- LeetCode example 2: candidates = [2,3,5], target = 8 ---
  {
    std::vector<int> candidates;
    candidates.push_back(2);
    candidates.push_back(3);
    candidates.push_back(5);

    std::vector<std::vector<int>> expected;
    expected.push_back(std::vector<int>{2, 2, 2, 2});
    expected.push_back(std::vector<int>{2, 3, 3});
    expected.push_back(std::vector<int>{3, 5});

    check(normalized(combinationSum(candidates, 8)) == normalized(expected),
          "[2,3,5] target 8 -> exactly [[2,2,2,2],[2,3,3],[3,5]]");
  }

  // --- LeetCode example 3: no combination is possible ---
  {
    std::vector<int> candidates;
    candidates.push_back(2);
    check(combinationSum(candidates, 1).empty(),
          "[2] target 1 -> empty (no combination reaches an odd target from 2s)");
    check(combinationSum(candidates, 3).empty(),
          "[2] target 3 -> empty (2 and 4 straddle it; overshoot is permanent)");

    std::vector<std::vector<int>> got = combinationSum(candidates, 2);
    check(got.size() == 1 && got[0].size() == 1 && got[0][0] == 2,
          "[2] target 2 -> exactly [[2]]");

    std::vector<std::vector<int>> six = combinationSum(candidates, 6);
    check(six.size() == 1 && six[0].size() == 3,
          "[2] target 6 -> exactly [[2,2,2]] -- unbounded reuse of a single candidate");
  }

  // --- Unsorted input must still work: the function sorts internally ---
  {
    std::vector<int> unsorted;
    unsorted.push_back(8);
    unsorted.push_back(7);
    unsorted.push_back(4);
    unsorted.push_back(3);

    std::vector<std::vector<int>> expected;
    expected.push_back(std::vector<int>{3, 4, 4});
    expected.push_back(std::vector<int>{3, 8});
    expected.push_back(std::vector<int>{4, 7});

    check(normalized(combinationSum(unsorted, 11)) == normalized(expected),
          "unsorted [8,7,4,3] target 11 -> [[3,4,4],[3,8],[4,7]] (sorted internally)");
  }

  // --- Every returned combination must actually sum to the target, and be ---
  // --- built in non-decreasing order (the start-index guarantee).          ---
  {
    std::vector<int> candidates;
    candidates.push_back(7);
    candidates.push_back(3);
    candidates.push_back(2);

    std::vector<std::vector<int>> got = combinationSum(candidates, 18);

    bool all_sum_correctly = true;
    bool all_non_decreasing = true;
    for (size_t i = 0; i < got.size(); ++i) {
      int sum = 0;
      for (size_t j = 0; j < got[i].size(); ++j) {
        sum += got[i][j];
        if (j > 0 && got[i][j] < got[i][j - 1]) all_non_decreasing = false;
      }
      if (sum != 18) all_sum_correctly = false;
    }
    check(all_sum_correctly, "[7,3,2] target 18: every combination sums to exactly 18");
    check(all_non_decreasing,
          "every combination is emitted in non-decreasing order (start index guarantees it)");

    // Enumerated by hand from 2a + 3b + 7c = 18:
    //   c=0: (a,b) = (9,0),(6,2),(3,4),(0,6)          -> 4
    //   c=1: 2a + 3b = 11 -> (4,1),(1,3)              -> 2
    //   c=2: 2a + 3b = 4  -> (2,0)                    -> 1
    check(got.size() == 7, "[7,3,2] target 18 -> exactly 7 combinations (hand-enumerated)");

    // No two combinations may be identical: the start index is what rules out
    // the reordered duplicates [2,3,2] / [3,2,2] that a from-zero loop emits.
    std::vector<std::vector<int>> norm = normalized(got);
    bool has_duplicate = false;
    for (size_t i = 1; i < norm.size(); ++i) {
      if (norm[i] == norm[i - 1]) has_duplicate = true;
    }
    check(!has_duplicate, "no duplicate combinations -- start index blocks reorderings");
  }

  // --- Edge cases: empty candidate list, and target 0 ---
  {
    std::vector<int> none;
    check(combinationSum(none, 5).empty(), "empty candidate list -> empty result");

    std::vector<int> candidates;
    candidates.push_back(2);
    candidates.push_back(3);
    // LeetCode guarantees target >= 1, but remaining == 0 at the root is
    // handled correctly for free: the empty combination sums to 0.
    std::vector<std::vector<int>> zero = combinationSum(candidates, 0);
    check(zero.size() == 1 && zero[0].empty(),
          "target 0 -> exactly one combination, the empty one (base case at the root)");
  }

  // --- The prune's value, measured: same answers, strictly fewer nodes ---
  {
    std::vector<int> candidates;
    candidates.push_back(2);
    candidates.push_back(3);
    candidates.push_back(5);
    candidates.push_back(7);
    candidates.push_back(11);

    long pruned_nodes = 0;
    long unpruned_nodes = 0;
    std::vector<std::vector<int>> pruned = combinationSum(candidates, 30, pruned_nodes);
    std::vector<std::vector<int>> unpruned =
        combinationSumUnpruned(candidates, 30, unpruned_nodes);

    check(normalized(pruned) == normalized(unpruned),
          "pruned and unpruned versions return identical answers (target 30)");
    check(pruned_nodes < unpruned_nodes,
          "the sorted-break prune visits strictly fewer nodes than remaining<0 detection");

    std::cout << "       (pruned visited " << pruned_nodes << " nodes, unpruned visited "
              << unpruned_nodes << " -- "
              << (100 - (100 * pruned_nodes) / unpruned_nodes) << "% avoided)\n";

    // The prune is what makes the difference visible; it does not change the
    // asymptotics, only the constant -- worth saying out loud in an interview
    // rather than claiming pruning improves the bound here.
    check(!pruned.empty(), "target 30 has at least one combination");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
