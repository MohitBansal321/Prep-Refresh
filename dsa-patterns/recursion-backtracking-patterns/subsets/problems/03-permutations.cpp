// ============================================================================
// LeetCode 46 — Permutations
// https://leetcode.com/problems/permutations/
// ============================================================================
//
// PROBLEM
// -------
// Given an array nums of DISTINCT integers, return all possible permutations
// (every ordering of all n elements). Permutations may be returned in any
// order.
//
// Example: nums = [1,2,3]
//   -> [[1,2,3], [1,3,2], [2,1,3], [2,3,1], [3,1,2], [3,2,1]]   (3! = 6)
//
// APPROACH — the same choose/recurse/undo skeleton, generalized
// -------------------------------------------------------------
// This is the third member of the subsets/combinations/permutations family
// and the one that most clearly shows what changes when ORDER MATTERS:
//
//   Subsets (01-subsets.cpp)   Permutations (this file)
//   ------------------------   ------------------------------------------
//   Decide element i:          Decide POSITION k: which of the remaining
//   in or out. 2 branches      elements goes here? Up to n branches per
//   per level, n levels.       level, n levels.
//   Base case: every element   Base case: every POSITION filled, i.e.
//   decided (index == n).      current.size() == nums.size().
//   Output size 2^n.           Output size n!.
//   Any length 0..n is a       Only length exactly n is a valid answer;
//   valid answer.              partial paths are never recorded.
//
// The push/recurse/pop bracket is identical. What is new is that a permutation
// must use every element exactly once, so the recursion needs to know which
// elements are already spoken for on the current path. Two ways to track that,
// both implemented below:
//
//   A. permuteUsedMarker — a std::vector<bool> used, parallel to nums. At each
//      level, loop over every index, skip the ones already used, mark / push /
//      recurse / pop / unmark. This is the shape in ../code.cpp and the one to
//      write in an interview, because it survives the extension to duplicate
//      inputs (LeetCode 47) with one extra condition.
//
//   B. permuteBySwapping — no auxiliary array at all. Treat nums itself as the
//      output buffer: at level k, swap each candidate index i (i >= k) into
//      position k, recurse on k+1, then swap back. Positions [0, k) hold the
//      prefix already chosen and positions [k, n) hold exactly the unchosen
//      elements, so "which are still available" is encoded in the array's own
//      layout. O(1) extra space beyond the recursion stack.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// used[i] = false must be restored AFTER the recursive call returns, in the
// same bracket as current.pop_back(). Forgetting it does not produce a wrong
// permutation -- it produces a MISSING one: element i stays marked used for
// every later sibling branch at this level, so those branches can never place
// it and the recursion never reaches its base case down those paths. The
// symptom is an output that is too SHORT (fewer than n! entries), which is
// much easier to miss than a visibly malformed entry.
//
// A second, subtler trap in formulation B: `nums` is mutated in place, so the
// array is only restored to its original order once the TOP-level call
// returns. Any code that reads nums mid-traversal sees a scrambled array, and
// for duplicate-containing inputs a pre-sort does not stay sorted -- which is
// exactly why formulation A, not B, is the base to extend for LeetCode 47.
//
// COMPLEXITY
// ----------
// Time:  O(n! * n) -- n! permutations, each of length n to build and copy.
// Space: O(n! * n) for the output. Auxiliary: O(n) for the recursion stack
//        plus O(n) for `current`/`used` in A, or O(n) stack only in B.
//
// Note how much faster this grows than subsets: at n = 10, 2^10 = 1,024 but
// 10! = 3,628,800. "Small enough to enumerate" means n <= 20-25 for subsets
// and only n <= 10-12 for permutations.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Formulation A — used[] marker.
//
// Invariant: `current` holds the prefix of the permutation chosen so far, and
// used[i] is true exactly when nums[i] is somewhere in that prefix. The two
// must be updated and undone together -- they are one piece of state split
// across two containers.
// ----------------------------------------------------------------------------
void permuteUsedMarkerHelper(const std::vector<int>& nums,
                             std::vector<bool>& used,
                             std::vector<int>& current,
                             std::vector<std::vector<int>>& result) {
  if (current.size() == nums.size()) {
    // Every position filled -> one complete permutation. Copy, because
    // `current` is a single shared buffer that keeps mutating.
    result.push_back(current);
    return;
  }

  for (size_t i = 0; i < nums.size(); ++i) {
    if (used[i]) continue;  // already placed earlier on THIS path

    // choose
    used[i] = true;
    current.push_back(nums[i]);

    // recurse: fill the next position from whatever is still unused
    permuteUsedMarkerHelper(nums, used, current, result);

    // UNDO -- both halves, or later siblings at this level lose access to
    // nums[i] and whole branches of the tree silently disappear.
    current.pop_back();
    used[i] = false;
  }
}

std::vector<std::vector<int>> permuteUsedMarker(const std::vector<int>& nums) {
  std::vector<std::vector<int>> result;
  std::vector<int> current;
  std::vector<bool> used(nums.size(), false);
  permuteUsedMarkerHelper(nums, used, current, result);
  return result;
}

// ----------------------------------------------------------------------------
// Formulation B — in-place swapping, no `used` array.
//
// At level k, positions [0, k) already hold the chosen prefix and positions
// [k, n) hold, in some order, exactly the elements not yet chosen. So the
// candidates for position k are simply indices k..n-1, and "marking used"
// becomes "swap it into position k."
//
// i == k is not special-cased: swapping an element with itself is a no-op and
// correctly represents "leave this element where it is." Skipping i == k would
// drop every permutation that keeps the current element in place.
// ----------------------------------------------------------------------------
void permuteBySwappingHelper(std::vector<int>& nums, size_t k,
                             std::vector<std::vector<int>>& result) {
  if (k == nums.size()) {
    result.push_back(nums);  // nums IS the completed permutation; copy it
    return;
  }

  for (size_t i = k; i < nums.size(); ++i) {
    std::swap(nums[k], nums[i]);            // choose: put nums[i] at position k
    permuteBySwappingHelper(nums, k + 1, result);
    std::swap(nums[k], nums[i]);            // undo: restore for the next sibling
  }
}

std::vector<std::vector<int>> permuteBySwapping(std::vector<int> nums) {
  // Taken BY VALUE: this formulation scrambles its input during the walk and
  // only restores it when the outermost call returns. Taking a copy keeps that
  // mutation from leaking out to the caller.
  std::vector<std::vector<int>> result;
  permuteBySwappingHelper(nums, 0, result);
  return result;
}

// ============================================================================
// Test harness
// ============================================================================

// Permutations may be returned in any order, but the order WITHIN each
// permutation is the answer itself and must never be sorted away. So only the
// outer vector is sorted here -- deliberately unlike normalized() in
// 01-subsets.cpp / 02-subsets-ii.cpp, which sorts both levels.
std::vector<std::vector<int>> outerSorted(std::vector<std::vector<int>> v) {
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

  // --- The LeetCode example, checked against all 6 expected orderings ---
  {
    std::vector<int> nums;
    nums.push_back(1);
    nums.push_back(2);
    nums.push_back(3);

    std::vector<std::vector<int>> expected;
    expected.push_back(std::vector<int>{1, 2, 3});
    expected.push_back(std::vector<int>{1, 3, 2});
    expected.push_back(std::vector<int>{2, 1, 3});
    expected.push_back(std::vector<int>{2, 3, 1});
    expected.push_back(std::vector<int>{3, 1, 2});
    expected.push_back(std::vector<int>{3, 2, 1});

    std::vector<std::vector<int>> a = permuteUsedMarker(nums);
    std::vector<std::vector<int>> b = permuteBySwapping(nums);

    check(a.size() == 6, "[1,2,3] used-marker -> 3! = 6 permutations");
    check(b.size() == 6, "[1,2,3] swapping    -> 3! = 6 permutations");
    check(outerSorted(a) == outerSorted(expected),
          "[1,2,3] used-marker matches all 6 expected orderings exactly");
    check(outerSorted(b) == outerSorted(expected),
          "[1,2,3] swapping matches all 6 expected orderings exactly");

    // The used-marker version emits in lexicographic order because it loops
    // indices 0..n-1 at every level; the swapping version does not, because
    // swapping reorders the tail. Same set, different visit order.
    check(a == expected, "used-marker emits in lexicographic order (loops indices in order)");
    check(!(b == expected), "swapping emits in a different order (the tail gets reordered)");
  }

  // --- Every permutation must be a genuine rearrangement: right length, ---
  // --- every element exactly once, and no two permutations identical.    ---
  {
    std::vector<int> nums;
    nums.push_back(7);
    nums.push_back(8);
    nums.push_back(9);
    nums.push_back(10);

    std::vector<std::vector<int>> got = permuteUsedMarker(nums);
    check(got.size() == 24, "[7,8,9,10] -> 4! = 24 permutations");

    bool all_valid = true;
    std::vector<int> sorted_input = nums;
    std::sort(sorted_input.begin(), sorted_input.end());
    for (size_t i = 0; i < got.size(); ++i) {
      std::vector<int> copy = got[i];
      std::sort(copy.begin(), copy.end());
      if (copy != sorted_input) all_valid = false;
    }
    check(all_valid, "every permutation uses all 4 elements exactly once (no drops, no repeats)");

    std::vector<std::vector<int>> sorted_got = outerSorted(got);
    bool has_duplicate = false;
    for (size_t i = 1; i < sorted_got.size(); ++i) {
      if (sorted_got[i] == sorted_got[i - 1]) has_duplicate = true;
    }
    check(!has_duplicate, "all 24 permutations are distinct orderings");
  }

  // --- Edge case: single element -> exactly one permutation ---
  {
    std::vector<int> single;
    single.push_back(42);
    std::vector<std::vector<int>> got = permuteUsedMarker(single);
    check(got.size() == 1 && got[0].size() == 1 && got[0][0] == 42,
          "single element [42] -> exactly one permutation, [42]");
  }

  // --- Edge case: empty input. 0! = 1, and the one permutation of nothing ---
  // --- is the empty sequence. LeetCode guarantees n >= 1, but the base    ---
  // --- case handles it correctly for free: current.size() == 0 == n.      ---
  {
    std::vector<int> empty_input;
    std::vector<std::vector<int>> got = permuteUsedMarker(empty_input);
    check(got.size() == 1 && got[0].empty(),
          "empty input -> exactly one (empty) permutation, since 0! = 1");
    check(permuteBySwapping(empty_input).size() == 1,
          "empty input, swapping -> same, one empty permutation");
  }

  // --- Negatives and zero: nothing in the algorithm inspects values ---
  {
    std::vector<int> mixed;
    mixed.push_back(-1);
    mixed.push_back(0);
    mixed.push_back(1);

    std::vector<std::vector<int>> expected;
    expected.push_back(std::vector<int>{-1, 0, 1});
    expected.push_back(std::vector<int>{-1, 1, 0});
    expected.push_back(std::vector<int>{0, -1, 1});
    expected.push_back(std::vector<int>{0, 1, -1});
    expected.push_back(std::vector<int>{1, -1, 0});
    expected.push_back(std::vector<int>{1, 0, -1});

    check(outerSorted(permuteUsedMarker(mixed)) == outerSorted(expected),
          "[-1,0,1] -> all 6 orderings (values never inspected, only positions)");
  }

  // --- The swapping formulation must leave the caller's array untouched ---
  {
    std::vector<int> nums;
    nums.push_back(1);
    nums.push_back(2);
    nums.push_back(3);
    std::vector<int> before = nums;
    permuteBySwapping(nums);
    check(nums == before,
          "permuteBySwapping takes nums by value -- caller's array is unchanged");
  }

  // --- Both formulations agree on several inputs, and the count is n! ---
  {
    bool all_agree = true;
    bool all_counts_correct = true;
    int factorial = 1;
    std::vector<int> nums;

    for (int n = 1; n <= 6; ++n) {
      nums.push_back(n);
      factorial *= n;

      if (outerSorted(permuteUsedMarker(nums)) != outerSorted(permuteBySwapping(nums))) {
        all_agree = false;
      }
      if (static_cast<int>(permuteUsedMarker(nums).size()) != factorial) {
        all_counts_correct = false;
      }
    }
    check(all_agree, "used-marker and swapping agree as sets for n = 1..6");
    check(all_counts_correct, "output count is exactly n! for n = 1..6 (up to 720)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
