// ============================================================================
// LeetCode 583 — Delete Operation for Two Strings
// ============================================================================
//
// PROBLEM
// -------
// Given two strings `word1` and `word2`, return the minimum number of steps
// required to make `word1` and `word2` the same, where in one step you can
// delete exactly one character from either string (no inserts, no
// replaces — deletion only).
//
// Example: word1 = "sea", word2 = "eat"  ->  2
//   Delete 's' from "sea" -> "ea". Delete 't' from "eat" -> "ea". Now equal,
//   in 2 total deletions.
//
// APPROACH — LCS length directly gives the answer
// -------------------------------------------------
// This is the simplest possible application of LCS as a subroutine: the
// characters that should NEVER be deleted are exactly the ones both strings
// already agree on IN ORDER — i.e. their longest common subsequence. Every
// other character (everything not part of the LCS) must be deleted from
// whichever string it belongs to, because keeping it would break equality
// with the other string.
//
//   answer = len(word1) + len(word2) - 2 * LCS(word1, word2)
//
// Why the "2 *": each string independently needs to delete down to the
// shared LCS. word1 deletes (len(word1) - LCS) characters, and word2
// deletes (len(word2) - LCS) characters, for a combined total of
// len(word1) + len(word2) - 2*LCS. This is the same LCS table (see
// ../code.cpp / ../README.md) used with zero modification — only the final
// arithmetic on the answer changes from problem to problem, which is the
// point of learning the pattern's shape rather than one formula.
//
// COMPLEXITY
// ----------
// Time:  O(n*m) — one LCS-table pass, then O(1) arithmetic.
// Space: O(n*m) here (or O(min(n,m)) if the table is rolled, since only the
//        final LCS length is needed and no reconstruction is required).
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int longestCommonSubsequenceLength(const std::string& a, const std::string& b) {
  const size_t n = a.size();
  const size_t m = b.size();

  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      if (a[i - 1] == b[j - 1]) {
        dp[i][j] = dp[i - 1][j - 1] + 1;
      } else {
        dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
      }
    }
  }
  return dp[n][m];
}

int minDistance(const std::string& word1, const std::string& word2) {
  int lcs_len = longestCommonSubsequenceLength(word1, word2);
  return static_cast<int>(word1.size() + word2.size()) - 2 * lcs_len;
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

  check(minDistance("sea", "eat") == 2, "\"sea\", \"eat\" -> 2");
  check(minDistance("leetcode", "etco") == 4, "\"leetcode\", \"etco\" -> 4");
  check(minDistance("abc", "abc") == 0, "identical strings -> 0 deletions");
  check(minDistance("abc", "def") == 6, "no shared characters -> delete everything (6)");
  check(minDistance("", "abc") == 3, "empty vs \"abc\" -> delete all 3 from word2");
  check(minDistance("a", "a") == 0, "single matching character -> 0");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
