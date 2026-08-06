// ============================================================================
// LeetCode 1092 — Shortest Common Supersequence
// ============================================================================
//
// PROBLEM
// -------
// Given two strings `str1` and `str2`, return the SHORTEST string that has
// both `str1` and `str2` as subsequences. If multiple answers exist, return
// any of them.
//
// Example: str1 = "abac", str2 = "cab"  ->  "cabac" (length 5)
//   "cabac" contains "abac" as a subsequence (drop the leading 'c') and
//   "cab" as a subsequence (drop the 'a' and 'c' near the end).
//
// APPROACH — LCS as a subroutine, then WEAVE the strings around it
// --------------------------------------------------------------------
// The key insight: the shortest supersequence keeps every character of
// BOTH strings, but characters that belong to their longest common
// subsequence should be written ONCE (shared), while every other character
// must appear from whichever string it belongs to. So:
//
//   length(SCS) = len(str1) + len(str2) - length(LCS(str1, str2))
//
// (Every character in the LCS would otherwise be double-counted once from
// each string, so it is subtracted out once.) This mirrors LeetCode 583's
// insight (subtracting LCS length) but here we need the ACTUAL merged
// string, not just its length — so we build the full LCS table (exactly as
// in ../code.cpp / ../README.md) and then WALK IT BACKWARDS, but instead of
// only recording matched characters (as reconstructLCS does), we record
// EVERY character encountered along the walk-back path:
//
//   - If str1[i-1] == str2[j-1]: this character is shared — write it once,
//     then step diagonally (i--, j--), exactly as in LCS reconstruction.
//   - Else if dp[i-1][j] >= dp[i][j-1]: the LCS walk-back would step to
//     (i-1, j); before doing that, str1[i-1] is a character that belongs
//     ONLY to str1 at this point, so we must include it in the
//     supersequence unshared. Write it, then step (i--).
//   - Else: symmetric — str2[j-1] belongs only to str2 here. Write it,
//     then step (j--).
//
// After the walk-back loop ends (when i == 0 or j == 0), whichever string
// still has leftover unconsumed prefix is copied in wholesale — those
// characters have no counterpart to share with, so they must all appear.
//
// Characters are collected in REVERSE order (since the walk starts at the
// end and moves toward the start), so the final result is reversed once
// before returning.
//
// COMPLEXITY
// ----------
// Time:  O(n*m) to build the table, O(n+m) to walk it back and emit the
//        result -> O(n*m) overall.
// Space: O(n*m) for the full table (required here, unlike the length-only
//        variant, because reconstruction needs to walk backwards through
//        every cell that could have contributed to the answer).
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::string shortestCommonSupersequence(const std::string& str1, const std::string& str2) {
  const size_t n = str1.size();
  const size_t m = str2.size();

  // Step 1: build the standard LCS table (identical to ../code.cpp).
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      if (str1[i - 1] == str2[j - 1]) {
        dp[i][j] = dp[i - 1][j - 1] + 1;
      } else {
        dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
      }
    }
  }

  // Step 2: walk the table backwards, weaving both strings together and
  // writing shared (LCS) characters only once.
  std::string result;
  size_t i = n;
  size_t j = m;
  while (i > 0 && j > 0) {
    if (str1[i - 1] == str2[j - 1]) {
      result.push_back(str1[i - 1]);  // Shared character: write once.
      --i;
      --j;
    } else if (dp[i - 1][j] >= dp[i][j - 1]) {
      result.push_back(str1[i - 1]);  // Unshared: belongs to str1 here.
      --i;
    } else {
      result.push_back(str2[j - 1]);  // Unshared: belongs to str2 here.
      --j;
    }
  }

  // Step 3: whichever string has leftover prefix (i > 0 or j > 0, never
  // both) must be copied in wholesale — no more shared characters remain.
  while (i > 0) {
    result.push_back(str1[i - 1]);
    --i;
  }
  while (j > 0) {
    result.push_back(str2[j - 1]);
    --j;
  }

  std::reverse(result.begin(), result.end());
  return result;
}

// Validates that `candidate` truly contains `sub` as a subsequence (not
// necessarily contiguous) — used by main() to verify correctness even when
// multiple valid shortest supersequences exist for the same input.
bool isSubsequence(const std::string& sub, const std::string& candidate) {
  size_t k = 0;
  for (char c : candidate) {
    if (k < sub.size() && sub[k] == c) ++k;
  }
  return k == sub.size();
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
    std::string str1 = "abac", str2 = "cab";
    std::string result = shortestCommonSupersequence(str1, str2);
    check(result.size() == 5, "\"abac\"/\"cab\" -> supersequence length 5");
    check(isSubsequence(str1, result), "result contains \"abac\" as a subsequence");
    check(isSubsequence(str2, result), "result contains \"cab\" as a subsequence");
  }

  {
    std::string str1 = "aaaaaaaa", str2 = "aaaaaaaa";
    std::string result = shortestCommonSupersequence(str1, str2);
    check(result == "aaaaaaaa", "identical strings -> supersequence is the string itself");
  }

  {
    std::string str1 = "abc", str2 = "def";
    std::string result = shortestCommonSupersequence(str1, str2);
    check(result.size() == 6, "no shared characters -> supersequence is the full concatenation length");
    check(isSubsequence(str1, result) && isSubsequence(str2, result),
          "disjoint strings -> result still contains both as subsequences");
  }

  {
    std::string str1 = "bbbaaaba", str2 = "bbababbb";
    std::string result = shortestCommonSupersequence(str1, str2);
    // LCS length here is 5, so shortest supersequence length = 8 + 8 - 5 = 11.
    check(result.size() == 11, "\"bbbaaaba\"/\"bbababbb\" -> supersequence length 11");
    check(isSubsequence(str1, result) && isSubsequence(str2, result),
          "result contains both inputs as subsequences");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
