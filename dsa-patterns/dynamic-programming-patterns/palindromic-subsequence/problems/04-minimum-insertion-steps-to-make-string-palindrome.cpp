// ============================================================================
// LeetCode 1312 — Minimum Insertion Steps to Make a String Palindrome
// ============================================================================
//
// PROBLEM
// -------
// Given a string s, in one step you can insert ANY character at ANY position.
// Return the minimum number of insertions needed to make s a palindrome.
//
// Example: s = "mbadm" -> 2   ("mbdadbm" or "mdbabdm")
//          s = "leetcode" -> 5
//          s = "zzazz" -> 0   (already a palindrome)
//
// APPROACH — Reduce to Longest Palindromic Subsequence, then interval DP
// ---------------------------------------------------------------------------
// The key insight is a reduction, not a new DP. Take the longest palindromic
// SUBSEQUENCE of s — call its length L. Those L characters already sit in a
// palindromic arrangement; they never need touching. Every one of the other
// n - L characters is "unpaired" with respect to that arrangement and needs
// exactly ONE insertion (its mirror partner) somewhere in the string:
//
//     answer = n - LPS(s)
//
// Why exactly one insertion per leftover character, and why this is optimal:
//   - Sufficiency: insert each leftover character's mirror opposite the
//     position where it sits relative to the kept subsequence; the result is
//     the kept palindrome with each stray character mirrored — still a
//     palindrome.
//   - Necessity: in any final palindrome, consider which original characters
//     are matched with an inserted partner or form the palindrome's core;
//     the original characters that end up paired among themselves form a
//     palindromic subsequence of s, so at most L originals can avoid needing
//     an insertion — at least n - L insertions are unavoidable.
//
// LPS itself comes from the standard interval DP (see problem 01):
//
//     dp[i][j] = longest palindromic subsequence inside s[i..j]
//     dp[i][j] = dp[i+1][j-1] + 2            if s[i] == s[j]
//              = max(dp[i+1][j], dp[i][j-1]) otherwise
//
// filled i from n-1 down to 0 so every nested smaller interval is finished
// before any interval containing it begins. Below-diagonal cells stay 0,
// which makes adjacent matching pairs (j == i+1) correctly score 0 + 2 = 2.
//
// COMPLEXITY
// ----------
// Time:  O(n^2) — one pass over the upper triangle of the table.
// Space: O(n^2) — the full DP table.
//
// Contrast: a direct O(n^2)-state DP over (i, j) meaning "min insertions for
// s[i..j]" exists and gives the same complexity, but the reduction reuses the
// LPS machinery unchanged and explains WHY the number is what it is.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int minInsertions(const std::string& s) {
  int n = static_cast<int>(s.size());
  if (n <= 1) return 0;  // "" and single chars are already palindromes

  // Standard LPS interval DP (identical to problems/01).
  std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
  for (int i = n - 1; i >= 0; --i) {
    dp[i][i] = 1;
    for (int j = i + 1; j < n; ++j) {
      if (s[i] == s[j]) {
        dp[i][j] = dp[i + 1][j - 1] + 2;
      } else {
        dp[i][j] = std::max(dp[i + 1][j], dp[i][j - 1]);
      }
    }
  }

  return n - dp[0][n - 1];  // every char outside the LPS needs one insertion
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
    // The canonical example: LPS("mbadm") = 3 ("mam" / "mbm"), so 5 - 3 = 2.
    check(minInsertions("mbadm") == 2, "\"mbadm\" -> 2");
  }

  {
    // LPS("leetcode") = 3 ("eee"), so 8 - 3 = 5.
    check(minInsertions("leetcode") == 5, "\"leetcode\" -> 5");
  }

  {
    // Already a palindrome: zero insertions.
    check(minInsertions("zzazz") == 0, "\"zzazz\" -> 0");
  }

  {
    // Empty string edge case.
    check(minInsertions("") == 0, "\"\" -> 0");
  }

  {
    // Single character edge case.
    check(minInsertions("a") == 0, "\"a\" -> 0");
  }

  {
    // Two distinct characters need both mirrors: LPS = 1, so 2 - 1 = 1...
    // actually only ONE insertion ("ab" -> "aba" or "bab") suffices.
    check(minInsertions("ab") == 1, "\"ab\" -> 1");
  }

  {
    // All identical characters: whole string is already a palindrome.
    check(minInsertions("aaaa") == 0, "\"aaaa\" -> 0");
  }

  {
    // All distinct characters: LPS = 1, every other char needs a mirror.
    check(minInsertions("abcd") == 3, "\"abcd\" -> 3");
  }

  {
    // Longer mixed case: LPS("racecarr") = 7 ("racecar"), so 8 - 7 = 1.
    check(minInsertions("racecarr") == 1,
          "\"racecarr\" -> 1 (insert 'a' mirror)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
