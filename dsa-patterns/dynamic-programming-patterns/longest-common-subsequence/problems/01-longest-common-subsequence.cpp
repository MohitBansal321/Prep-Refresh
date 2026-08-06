// ============================================================================
// LeetCode 1143 — Longest Common Subsequence
// ============================================================================
//
// PROBLEM
// -------
// Given two strings `text1` and `text2`, return the length of their longest
// common subsequence. If there is no common subsequence, return 0. A
// subsequence is a sequence derived from the original string by deleting
// some (or no) characters WITHOUT changing the relative order of the
// remaining characters. A "common" subsequence is one that is a subsequence
// of both strings.
//
// Example: text1 = "abcde", text2 = "ace"  ->  3 ("ace" is common)
//
// APPROACH — 2D DP over prefix pairs (the LCS shape)
// ---------------------------------------------------
// This IS the pattern in its purest, textbook form (see ../README.md and
// ../images/recognition-diagram.md): comparing two sequences for a longest
// shared subsequence is exactly the recognition signal.
//
// dp[i][j] = LCS length of text1[0..i) and text2[0..j).
//   - dp[0][j] = dp[i][0] = 0 (an empty prefix shares nothing with anything).
//   - If text1[i-1] == text2[j-1]: dp[i][j] = dp[i-1][j-1] + 1 (extend the
//     diagonal — this matched character can follow whatever the best
//     answer was for both strings one character shorter).
//   - Else: dp[i][j] = max(dp[i-1][j], dp[i][j-1]) (best of dropping the
//     current character from either string).
//
// Answer is dp[n][m].
//
// COMPLEXITY
// ----------
// Time:  O(n*m) — one constant-time decision per cell of an (n+1)x(m+1)
//        table.
// Space: O(n*m) for the full table (here); can be optimized to O(min(n,m))
//        with a rolling 1D array if only the LENGTH is needed (not the
//        reconstructed subsequence — see ../README.md's Complexity section).
//
// Contrast with brute force (try every subsequence of text1 against every
// subsequence of text2): O(2^n * 2^m) — completely infeasible past a
// handful of characters.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int longestCommonSubsequence(const std::string& text1, const std::string& text2) {
  const size_t n = text1.size();
  const size_t m = text2.size();

  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      if (text1[i - 1] == text2[j - 1]) {
        dp[i][j] = dp[i - 1][j - 1] + 1;
      } else {
        dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
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

  check(longestCommonSubsequence("abcde", "ace") == 3, "\"abcde\", \"ace\" -> 3");
  check(longestCommonSubsequence("abc", "abc") == 3, "\"abc\", \"abc\" -> 3");
  check(longestCommonSubsequence("abc", "def") == 0, "\"abc\", \"def\" -> 0");
  check(longestCommonSubsequence("", "abc") == 0, "\"\", \"abc\" -> 0");
  check(longestCommonSubsequence("bsbininm", "jmjkbkjkv") == 1,
        "\"bsbininm\", \"jmjkbkjkv\" -> 1");
  check(longestCommonSubsequence("ezupkr", "ubmrapg") == 2,
        "\"ezupkr\", \"ubmrapg\" -> 2");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
