// ============================================================================
// LeetCode 516 — Longest Palindromic Subsequence
// ============================================================================
//
// PROBLEM
// -------
// Given a string s, find the length of the longest subsequence of s that is
// a palindrome. A subsequence may SKIP characters but must preserve order.
//
// Example: s = "bbbab" -> 4   ("bbbb", using indices 0,1,2,4 — skipping 'a')
//          s = "cbbd" -> 2   ("bb")
//
// APPROACH — Interval DP over a single string
// -------------------------------------------
// Because gaps are allowed, expand-around-center cannot work: expansion walks
// ADJACENT characters outward, so it only ever sees contiguous substrings.
// Instead we define the answer over every interval:
//
//     dp[i][j] = length of the longest palindromic subsequence inside s[i..j]
//
// and peel the interval's two endpoints:
//   - If s[i] == s[j]: both endpoints can wrap around the best palindrome of
//     the INNER interval, contributing dp[i+1][j-1] + 2. We never need to
//     compare this against dropping an endpoint — wrapping two equal chars
//     around the inner optimum is always at least as good as discarding them,
//     because any palindrome built without one endpoint can be rebuilt with
//     both (replace that endpoint's mirror partner with our matched pair).
//   - If s[i] != s[j]: the endpoints cannot both be in the same palindrome,
//     so drop exactly one: max(dp[i+1][j], dp[i][j-1]).
//
// Base cases: dp[i][i] = 1 (a single character). For adjacent matching chars
// (j == i+1) the formula reads dp[i+1][j-1], which sits BELOW the diagonal
// and holds the initialization value 0 — so the pair correctly scores 0+2=2
// with no special-case code.
//
// FILL ORDER IS THE WHOLE GAME: dp[i][j] depends on dp[i+1][j-1], a strictly
// smaller interval nested inside [i..j]. Row-by-row top-down filling would
// read row i+1 before it exists. Iterating i from n-1 DOWN to 0 (and j from
// i+1 up) guarantees row i+1 is fully finished before row i starts, and
// dp[i][j-1] is finished earlier within the same row.
//
// COMPLEXITY
// ----------
// Time:  O(n^2) — each of the ~n^2/2 upper-triangle cells computed in O(1).
// Space: O(n^2) — the full table (an O(n) rolling-row optimization exists
//        because row i only reads row i+1, but it obscures the recurrence).
//
// Cross-check shortcut: LPS(s) == LCS(s, reverse(s)) — see
// ../longest-common-subsequence/. Same answer, different table semantics.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int longestPalindromeSubseq(const std::string& s) {
  int n = static_cast<int>(s.size());
  if (n == 0) return 0;

  // dp[i][j] for i <= j; below-diagonal cells stay 0 and are read only by
  // the j == i+1 case (see banner above).
  std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

  for (int i = n - 1; i >= 0; --i) {      // i DESCENDING: row i+1 ready first
    dp[i][i] = 1;                          // single char is a palindrome of len 1
    for (int j = i + 1; j < n; ++j) {      // j ASCENDING: dp[i][j-1] ready first
      if (s[i] == s[j]) {
        dp[i][j] = dp[i + 1][j - 1] + 2;   // endpoints join inner palindrome
      } else {
        dp[i][j] = std::max(dp[i + 1][j], dp[i][j - 1]);  // drop one endpoint
      }
    }
  }

  return dp[0][n - 1];  // whole-string answer
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

  {
    // The canonical example: skip the 'a', keep all four b's.
    check(longestPalindromeSubseq("bbbab") == 4,
          "\"bbbab\" -> 4 (\"bbbb\")");
  }

  {
    // Even-length palindrome from a middle pair; c and d are unusable ends.
    check(longestPalindromeSubseq("cbbd") == 2,
          "\"cbbd\" -> 2 (\"bb\")");
  }

  {
    // Single character edge case.
    check(longestPalindromeSubseq("a") == 1, "\"a\" -> 1");
  }

  {
    // Empty string edge case.
    check(longestPalindromeSubseq("") == 0, "\"\" -> 0");
  }

  {
    // Whole string already a palindrome — every endpoint match chains inward.
    check(longestPalindromeSubseq("racecar") == 7,
          "\"racecar\" -> 7 (whole string)");
  }

  {
    // All identical characters: every pair matches, answer is the full length.
    check(longestPalindromeSubseq("aaaa") == 4, "\"aaaa\" -> 4");
  }

  {
    // All distinct characters: no pair ever matches, answer stays 1.
    check(longestPalindromeSubseq("abcd") == 1,
          "\"abcd\" -> 1 (all distinct)");
  }

  {
    // Nested wrapping in action: inner interval [2..4] holds "bdb" (3), then
    // both 'a' endpoints wrap it -> indices 0,2,3,4,5 spell "abdba".
    check(longestPalindromeSubseq("agbdba") == 5,
          "\"agbdba\" -> 5 (\"abdba\")");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
