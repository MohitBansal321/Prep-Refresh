// ============================================================================
// LeetCode 72 — Edit Distance (Levenshtein Distance)
// ============================================================================
//
// PROBLEM
// -------
// Given two strings `word1` and `word2`, return the minimum number of
// operations required to convert `word1` into `word2`. You may INSERT,
// DELETE, or REPLACE a single character per operation.
//
// Example: word1 = "horse", word2 = "ros"  ->  3
//   horse -> rorse (replace 'h' with 'r')
//   rorse -> rose  (delete 'r')
//   rose  -> ros   (delete 'e')
//
// APPROACH — same dp[i][j] TABLE SHAPE as LCS, different cell meaning
// ---------------------------------------------------------------------
// This is the pattern this README calls out as the reason LCS is a
// "foundation": Edit Distance uses the identical (n+1) x (m+1) grid indexed
// by (i, j) = (prefix of word1, prefix of word2), filled in the same row-by-
// row order, but dp[i][j] means something different: the MINIMUM COST to
// turn word1[0..i) into word2[0..j), not the length of a shared subsequence.
//
// Base cases (the extra row/column, same idea as LCS's empty-prefix base
// case, but with a different value):
//   - dp[i][0] = i   (delete all i characters of word1 to reach the empty
//                      string word2[0..0) = "").
//   - dp[0][j] = j   (insert all j characters of word2 into the empty
//                      word1 prefix).
//
// Recurrence:
//   - If word1[i-1] == word2[j-1]: the characters already match, so no
//     operation is spent here — dp[i][j] = dp[i-1][j-1] (same diagonal move
//     as LCS's match case, just without "+1", because a match costs
//     nothing instead of extending a shared length).
//   - Else: try all three allowed operations and take the cheapest, each
//     costing 1 plus whatever the smaller subproblem already cost:
//       replace word1[i-1] with word2[j-1]: 1 + dp[i-1][j-1]
//       delete word1[i-1]:                  1 + dp[i-1][j]
//       insert word2[j-1] into word1:        1 + dp[i][j-1]
//     dp[i][j] = 1 + min(dp[i-1][j-1], dp[i-1][j], dp[i][j-1])
//
// Answer is dp[n][m].
//
// COMPLEXITY
// ----------
// Time:  O(n*m) — identical shape to LCS, one O(1) decision per cell.
// Space: O(n*m) here; optimizable to O(min(n,m)) with a rolling array since
//        only the final number is needed, not a reconstruction of the edit
//        script.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int minDistance(const std::string& word1, const std::string& word2) {
  const size_t n = word1.size();
  const size_t m = word2.size();

  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

  // Base cases: converting to/from the empty string costs one operation per
  // remaining character (all deletes, or all inserts).
  for (size_t i = 0; i <= n; ++i) dp[i][0] = static_cast<int>(i);
  for (size_t j = 0; j <= m; ++j) dp[0][j] = static_cast<int>(j);

  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      if (word1[i - 1] == word2[j - 1]) {
        // Characters already match: no operation needed at this position,
        // inherit the cost of the smaller prefixes unchanged.
        dp[i][j] = dp[i - 1][j - 1];
      } else {
        int replace_cost = dp[i - 1][j - 1];
        int delete_cost = dp[i - 1][j];
        int insert_cost = dp[i][j - 1];
        dp[i][j] = 1 + std::min({replace_cost, delete_cost, insert_cost});
      }
    }
  }

  return dp[n][m];
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

  check(minDistance("horse", "ros") == 3, "\"horse\" -> \"ros\" costs 3");
  check(minDistance("intention", "execution") == 5,
        "\"intention\" -> \"execution\" costs 5");
  check(minDistance("", "") == 0, "\"\" -> \"\" costs 0");
  check(minDistance("abc", "") == 3, "\"abc\" -> \"\" costs 3 (delete all)");
  check(minDistance("", "abc") == 3, "\"\" -> \"abc\" costs 3 (insert all)");
  check(minDistance("abc", "abc") == 0, "identical strings cost 0");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
