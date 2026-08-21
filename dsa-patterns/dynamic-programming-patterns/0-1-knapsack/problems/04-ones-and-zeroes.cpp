// ============================================================================
// LeetCode 474 — Ones and Zeroes
// https://leetcode.com/problems/ones-and-zeroes/
// ============================================================================
//
// PROBLEM
// -------
// You are given an array of binary strings strs and two integers m and n.
// Return the size of the LARGEST subset of strs such that the subset contains
// at most m zeros and at most n ones in total.
//
// Example: strs = ["10","0001","111001","1","0"], m = 5, n = 3 -> 4
//          (the subset {"10","0001","1","0"} uses 5 zeros and 3 ones)
//
// APPROACH — 0/1 Knapsack where the CAPACITY IS TWO NUMBERS
// ----------------------------------------------------------
// This is the problem in this module where the "capacity" axis is least
// obvious, and it is the reason it comes last. Nothing here looks like a
// weight limit -- there are no numbers in the input at all, only strings.
// The mapping (see ../README.md, "Interview Discussion", which calls this
// out explicitly):
//     item i's WEIGHT = the PAIR (zeros in strs[i], ones in strs[i])
//     item i's VALUE  = 1        (every string counts equally toward "size")
//     CAPACITY        = the PAIR (m, n)
// Two independent budgets are consumed simultaneously by a single decision,
// and neither may be exceeded. Because value is a constant 1, "maximize
// value" degenerates into "maximize COUNT" -- which is why the answer is a
// subset size rather than a sum of anything.
//
// The recurrence is the same two-choice shape as ../code.cpp, with the single
// capacity index replaced by a pair of indices:
//     dp[i][j] = max(dp[i][j],  dp[i - zeros][j - ones] + 1)
// where dp[i][j] is "the largest subset achievable with a budget of i zeros
// and j ones". A conceptually 3D table (item, zeros, ones) collapses to a 2D
// table by the same "row i only reads row i - 1" argument used for the 1D
// optimization everywhere else in this module.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// BOTH capacity axes must be swept BACKWARD, and it must be both -- getting
// one right and one wrong is the classic half-fix. The rule is not "the inner
// loop goes backward"; it is "every dimension that indexes a CAPACITY must go
// from high to low, so that dp[i - zeros][j - ones] is still a
// previous-item value when read." If i descends but j ascends, then for a
// string with ones > 0 the cell dp[i - zeros][j - ones] sits at a SMALLER j
// that this string's own pass has already overwritten, and the string gets
// counted twice within one item pass -- silently answering the Unbounded
// version of the problem ("reuse strings freely"), exactly the failure mode
// described in ../README.md's Common Mistakes for the 1D case.
//
// A second, quieter detail: the loops must run down to i >= zeros and
// j >= ones, i.e. INCLUSIVE of the exact-fit cell. Stopping at i > zeros
// loses the case where the string consumes the entire remaining budget --
// often precisely the optimal packing (as in LeetCode's own example, which
// uses all 5 zeros and all 3 ones exactly).
//
// COMPLEXITY
// ----------
// Time:  O(L + k * m * n) where k = strs.size() and L is the total length of
//        all strings (counting digits once, up front).
// Space: O(m * n) for the collapsed table -- the 3D version would be
//        O(k * m * n).
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// countDigits — turn one binary string into its (zeros, ones) "weight" pair.
//
// Written with an explicit std::pair<int,int> return type rather than relying
// on class template argument deduction, and unpacked with .first/.second
// rather than structured bindings, so this compiles under older toolchains.
// ----------------------------------------------------------------------------
static std::pair<int, int> countDigits(const std::string& s) {
  int zeros = 0;
  int ones = 0;
  for (size_t k = 0; k < s.size(); ++k) {
    if (s[k] == '0') {
      ++zeros;
    } else if (s[k] == '1') {
      ++ones;
    }
  }
  return std::pair<int, int>(zeros, ones);
}

// ----------------------------------------------------------------------------
// findMaxForm — collapsed 2D table over the two capacity axes.
//
// dp[i][j] = largest subset size achievable with a budget of i zeros and
//            j ones, considering the strings processed so far.
//
// Base case: dp is all zeros. With no strings considered, no budget buys any
// strings -- the same "dp[0][*] = 0" base row as the 2D table in ../code.cpp,
// here spread across a plane instead of a row.
// ----------------------------------------------------------------------------
int findMaxForm(const std::vector<std::string>& strs, int m, int n) {
  std::vector<std::vector<int> > dp(m + 1, std::vector<int>(n + 1, 0));

  for (size_t k = 0; k < strs.size(); ++k) {
    std::pair<int, int> cost = countDigits(strs[k]);
    int zeros = cost.first;
    int ones = cost.second;

    // A string that cannot fit even in the FULL budget can never fit in any
    // sub-budget either, so skip it outright. Not required for correctness
    // (the loop bounds would simply never execute), but it makes the intent
    // explicit and mirrors the "weights[i] > w -> forced skip" branch of the
    // 2D recurrence in ../README.md's Solution section.
    if (zeros > m || ones > n) continue;

    // BOTH axes descend. See the header note: this is the whole reason the
    // problem is a 0/1 knapsack rather than an unbounded one.
    for (int i = m; i >= zeros; --i) {
      for (int j = n; j >= ones; --j) {
        // Choice 1 (implicit): skip strs[k] -- dp[i][j] keeps its value.
        // Choice 2: take strs[k], paying (zeros, ones) from the budget and
        //           gaining exactly 1 toward the subset size.
        dp[i][j] = std::max(dp[i][j], dp[i - zeros][j - ones] + 1);
      }
    }
  }

  return dp[m][n];
}

// ----------------------------------------------------------------------------
// findMaxFormBruteForce — the O(2^k) oracle the DP replaces.
//
// One bitmask per subset; sum its zeros and ones; keep the largest subset
// that fits both budgets. Included so the tests validate the DP against an
// independent implementation rather than only against hand-computed numbers.
// ----------------------------------------------------------------------------
int findMaxFormBruteForce(const std::vector<std::string>& strs, int m, int n) {
  int k = static_cast<int>(strs.size());
  if (k > 18) return -1;  // Refuse rather than hang.

  std::vector<std::pair<int, int> > costs;
  for (int idx = 0; idx < k; ++idx) {
    costs.push_back(countDigits(strs[idx]));
  }

  int best = 0;
  int subsetCount = 1 << k;
  for (int mask = 0; mask < subsetCount; ++mask) {
    int zeros = 0;
    int ones = 0;
    int size = 0;
    for (int idx = 0; idx < k; ++idx) {
      if (mask & (1 << idx)) {
        zeros += costs[idx].first;
        ones += costs[idx].second;
        ++size;
      }
    }
    if (zeros <= m && ones <= n) {
      best = std::max(best, size);
    }
  }
  return best;
}

// ============================================================================
// Tests
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

  auto checkBoth = [&](const std::vector<std::string>& strs, int m, int n,
                       int expected, const std::string& label) {
    check(findMaxForm(strs, m, n) == expected, "dp   : " + label);
    check(findMaxFormBruteForce(strs, m, n) == expected, "brute: " + label);
  };

  {
    // LeetCode's first example. The winning subset uses the budget EXACTLY
    // (5 zeros, 3 ones), which is why the loop bounds must be inclusive.
    std::vector<std::string> strs;
    strs.push_back("10"); strs.push_back("0001"); strs.push_back("111001");
    strs.push_back("1"); strs.push_back("0");
    checkBoth(strs, 5, 3, 4, "[\"10\",\"0001\",\"111001\",\"1\",\"0\"], m=5 n=3 -> 4");
  }

  {
    // LeetCode's second example: "10" costs (1,1) and would eat both budgets
    // for a subset of size 1, while "0" + "1" costs (1,1) total for size 2.
    // Greedily taking the first string that fits would answer 1 -- the DP
    // considers skipping it and answers 2.
    std::vector<std::string> strs;
    strs.push_back("10"); strs.push_back("0"); strs.push_back("1");
    checkBoth(strs, 1, 1, 2, "[\"10\",\"0\",\"1\"], m=1 n=1 -> 2 (skipping \"10\" wins)");
  }

  {
    // Single string, exact fit on both axes.
    std::vector<std::string> strs;
    strs.push_back("10");
    checkBoth(strs, 1, 1, 1, "[\"10\"], m=1 n=1 -> 1 (single string, exact fit)");
  }

  {
    // Single string, one axis short: zero budget for the '0' it needs.
    std::vector<std::string> strs;
    strs.push_back("10");
    checkBoth(strs, 0, 1, 0, "[\"10\"], m=0 n=1 -> 0 (out of zeros, not ones)");
  }

  {
    // Both budgets zero and every string costs something -> nothing fits.
    std::vector<std::string> strs;
    strs.push_back("0"); strs.push_back("1");
    checkBoth(strs, 0, 0, 0, "[\"0\",\"1\"], m=0 n=0 -> 0 (no budget at all)");
  }

  {
    // Identical strings: the answer is a COUNT, so duplicates are separate
    // items competing for the same budget. Three "0"s with 2 zeros of budget
    // buys 2 of them, and the reverse sweep must not let one "0" be counted
    // twice to claim 2 from a budget of 1.
    std::vector<std::string> strs;
    for (int k = 0; k < 3; ++k) strs.push_back("0");
    checkBoth(strs, 2, 0, 2, "[\"0\",\"0\",\"0\"], m=2 n=0 -> 2 (duplicates are distinct items)");
  }

  {
    // The single most important reuse test: ONE string, a budget large enough
    // to hold it three times over. A forward sweep on either axis would
    // report 3. The correct 0/1 answer is 1 -- the string exists once.
    std::vector<std::string> strs;
    strs.push_back("1");
    checkBoth(strs, 0, 3, 1, "[\"1\"], m=0 n=3 -> 1 (budget fits it 3x; it exists ONCE)");
  }

  {
    // All-same values again, on the other axis, with a spare budget.
    std::vector<std::string> strs;
    for (int k = 0; k < 3; ++k) strs.push_back("1");
    checkBoth(strs, 0, 2, 2, "[\"1\",\"1\",\"1\"], m=0 n=2 -> 2 (all-same, budget caps at 2)");
  }

  {
    // Only the cheaper of two overlapping strings fits.
    std::vector<std::string> strs;
    strs.push_back("111"); strs.push_back("1111");
    checkBoth(strs, 0, 4, 1, "[\"111\",\"1111\"], m=0 n=4 -> 1 (either one, never both)");
  }

  {
    // Empty strings cost (0,0), so every one of them is free and all three
    // belong in the optimal subset even with a zero budget. The reverse
    // sweep handles a (0,0) weight without a special case: each cell is
    // visited once per string and incremented by 1.
    std::vector<std::string> strs;
    for (int k = 0; k < 3; ++k) strs.push_back("");
    checkBoth(strs, 0, 0, 3, "[\"\",\"\",\"\"], m=0 n=0 -> 3 (zero-cost items are all free)");
  }

  {
    // Empty input: no strings, so the largest subset is the empty one.
    std::vector<std::string> strs;
    checkBoth(strs, 5, 5, 0, "[], m=5 n=5 -> 0 (no strings to choose from)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
