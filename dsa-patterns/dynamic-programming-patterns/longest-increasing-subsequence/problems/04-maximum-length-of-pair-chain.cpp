// ============================================================================
// LeetCode 646 — Maximum Length of Pair Chain
// ============================================================================
//
// PROBLEM
// -------
// You are given `n` pairs of numbers. In every pair, the first number is
// always smaller than the second (pairs[i] = [left_i, right_i], left_i <
// right_i). A chain of pairs [p1, p2, ..., pk] is one where
// p1.right < p2.left < p2.right < p3.left < ... — i.e. each next pair's
// LEFT value must be strictly greater than the previous pair's RIGHT
// value. Return the length of the longest such chain you can form (you may
// select the pairs in any order, not necessarily the order given).
//
// Example: pairs = [[1,2],[2,3],[3,4]] -> 2
//          (chain [1,2] -> [3,4]; note [2,3] cannot follow [1,2] because
//           2 is not STRICTLY greater than 2)
//
// APPROACH — LIS generalized to a custom "compatibility" condition
// --------------------------------------------------------------------
// This problem is a direct generalization of the O(n^2) LIS recurrence:
// instead of comparing two numbers with `<`, we compare two PAIRS with a
// custom "can p2 follow p1?" rule (p1.right < p2.left). Everything else
// about the recurrence is identical to plain LIS:
//
// 1. Sort pairs by their LEFT value ascending. This plays the same role
//    sorting envelopes by width played in Russian Doll Envelopes: it lets
//    us build any valid chain by scanning left to right, because a pair
//    later in the sorted order can never have a smaller left value than
//    one earlier in it.
//
// 2. dp[i] = length of the longest chain ENDING at pair i (same meaning as
//    LIS's dp[i] = length of the longest increasing subsequence ending at
//    index i). Base case dp[i] = 1 (every pair is trivially a chain of
//    length 1 by itself).
//
// 3. For each i, scan every earlier j < i. If pairs[j].right < pairs[i].left
//    (pair j can be immediately followed by pair i), then dp[i] can become
//    dp[j] + 1. Take the best such j, exactly like LIS takes the best
//    dp[j] among all nums[j] < nums[i].
//
// 4. The answer is max(dp[i]) over all i, not dp[n-1] — the longest chain
//    might not end at the pair with the largest left value.
//
// Note: a simpler GREEDY O(n log n) solution also solves this optimally
// (sort by RIGHT value ascending, then greedily take a pair whenever its
// left value exceeds the previous chain's right value) — it is the
// interval-scheduling greedy, a well-known different pattern entirely.
// This file deliberately uses the O(n^2) LIS-style DP instead, because the
// POINT of including this problem in the LIS module is to show how the
// same dp[i]-looks-back-at-dp[j] recurrence generalizes to conditions
// other than plain numeric "<" — the greedy is a legitimate but unrelated
// optimization or this specific problem's extra structure (that pairs
// don't need to preserve original array order) affords.
//
// COMPLEXITY
// ----------
// Time:  O(n^2) — the nested loop, after an O(n log n) sort (dominated by
//        the O(n^2) term for n > a small constant).
// Space: O(n) — the dp array.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int findLongestChain(std::vector<std::vector<int>> pairs) {
  const size_t n = pairs.size();
  if (n == 0) return 0;

  // Sort by left value ascending -- mirrors sorting by width in Russian
  // Doll Envelopes, so a left-to-right scan can only see non-decreasing
  // left values.
  std::sort(pairs.begin(), pairs.end(),
            [](const std::vector<int>& a, const std::vector<int>& b) {
              return a[0] < b[0];
            });

  std::vector<int> dp(n, 1);
  int best = 1;

  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (pairs[j][1] < pairs[i][0]) {
        // pair j's right value is strictly less than pair i's left value:
        // pair i can validly follow pair j in a chain.
        dp[i] = std::max(dp[i], dp[j] + 1);
      }
    }
    best = std::max(best, dp[i]);
  }

  return best;
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](std::vector<std::vector<int>> pairs, int expected,
                    const std::string& label) {
    int result = findLongestChain(pairs);
    if (result == expected) {
      std::cout << "[PASS] " << label << " -> " << result << "\n";
      ++pass_count;
    } else {
      std::cout << "[FAIL] " << label << " -> got " << result
                << ", expected " << expected << "\n";
      ++fail_count;
    }
  };

  check({{1, 2}, {2, 3}, {3, 4}}, 2, "[[1,2],[2,3],[3,4]] -> 2");

  check({{1, 2}, {7, 8}, {4, 5}}, 3, "[[1,2],[7,8],[4,5]] -> 3 (all chain)");

  check({{1, 5}, {2, 3}, {4, 6}, {6, 8}}, 2,
        "[[1,5],[2,3],[4,6],[6,8]] -> 2 (e.g. [2,3] -> [4,6] or [6,8])");

  check({{1, 2}}, 1, "single pair -> 1");

  check({}, 0, "no pairs -> 0");

  check({{-10, -8}, {8, 9}, {-5, 0}, {6, 10}, {-6, -4}, {1, 7}, {9, 10}, {-4, -1}}, 4,
        "eight overlapping/negative pairs -> 4");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
