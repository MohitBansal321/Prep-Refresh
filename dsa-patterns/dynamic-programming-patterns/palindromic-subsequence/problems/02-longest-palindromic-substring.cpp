// ============================================================================
// LeetCode 5 — Longest Palindromic Substring
// ============================================================================
//
// PROBLEM
// -------
// Given a string s, return the longest CONTIGUOUS substring of s that is a
// palindrome. If there are multiple answers of equal length, any one is
// acceptable.
//
// Example: s = "babad" -> "bab" (or "aba")
//          s = "cbbd" -> "bb"
//
// APPROACH — Expand around center
// -------------------------------
// Every palindrome is symmetric about a center. In a string of length n the
// center is either a CHARACTER (odd-length palindromes: n of them) or a GAP
// between two characters (even-length palindromes: n-1 of them) — so there
// are exactly 2n - 1 centers. For each center, walk both pointers outward as
// long as the characters mirror each other, and remember the longest window
// seen.
//
// Why this is exhaustive: every palindromic substring has exactly one center,
// and expansion from that center reaches at least the palindrome itself before
// stopping — so no palindrome can be missed.
//
// The subtle line is the length after expansion:
//
//     len = right - left - 1
//
// The while loop exits only AFTER overshooting — either s[left] != s[right]
// or a boundary was hit — so the last VALID window is [left+1, right-1],
// whose length is (right-1) - (left+1) + 1 = right - left - 1. Forgetting the
// overshoot is the classic off-by-one here.
//
// Why not interval DP? It works (a boolean isPal table plus a max-scan), but
// costs O(n^2) SPACE for information expansion never needs to store. When the
// question is contiguous-only, expand-around-center gives the same time in
// O(1) space.
//
// COMPLEXITY
// ----------
// Time:  O(n^2) — 2n-1 centers, each expanding at most O(n) steps.
// Space: O(1) — two indices and the answer's start/length; the returned
//        substring itself is unavoidable output.
//
// Contrast: brute force checks all O(n^2) substrings with an O(n) palindrome
// test = O(n^3). Manacher's algorithm achieves O(n) but is rarely worth the
// complexity outside competitive programming.
// ============================================================================

#include <iostream>
#include <string>

std::string longestPalindrome(const std::string& s) {
  if (s.empty()) return "";

  int n = static_cast<int>(s.size());
  int start = 0;
  int maxLen = 1;  // any single character is already a valid answer

  // Walk outward from a center while the mirror property holds; on exit,
  // record the window if it beats the best seen so far.
  auto expand = [&](int left, int right) {
    while (left >= 0 && right < n && s[left] == s[right]) {
      --left;
      ++right;
    }
    // Loop overshot by one step on each side -> last valid window length:
    int len = right - left - 1;
    if (len > maxLen) {
      maxLen = len;
      start = left + 1;
    }
  };

  for (int center = 0; center < n; ++center) {
    expand(center, center);      // odd-length: center is character `center`
    expand(center, center + 1);  // even-length: center is gap after `center`
  }

  return s.substr(start, maxLen);
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

  // Two equally long answers exist ("bab" / "aba"); accept either.
  {
    std::string result = longestPalindrome("babad");
    check(result == "bab" || result == "aba",
          "\"babad\" -> \"bab\" or \"aba\"");
  }

  {
    // Even-length palindrome centered on a GAP — the case odd-only centers
    // would miss entirely.
    check(longestPalindrome("cbbd") == "bb", "\"cbbd\" -> \"bb\"");
  }

  {
    // Single character edge case.
    check(longestPalindrome("a") == "a", "\"a\" -> \"a\"");
  }

  {
    // Empty string edge case.
    check(longestPalindrome("") == "", "\"\" -> \"\"");
  }

  {
    // Two distinct single chars, no palindrome longer than 1 — accept either.
    std::string result = longestPalindrome("ac");
    check(result == "a" || result == "c", "\"ac\" -> \"a\" or \"c\"");
  }

  {
    // Whole string is a palindrome: expansion must reach the boundaries and
    // stop there without going out of range.
    check(longestPalindrome("aaaa") == "aaaa", "\"aaaa\" -> \"aaaa\"");
  }

  {
    // Longest palindrome buried mid-string with longer non-palindromic
    // prefixes/suffixes on both sides.
    std::string result = longestPalindrome("forgeeksskeegfor");
    check(result == "geeksskeeg",
          "\"forgeeksskeegfor\" -> \"geeksskeeg\"");
  }

  {
    // Even-length winner longer than every odd-length candidate in the same
    // string — forces the gap-center branch to win.
    check(longestPalindrome("abacdfgdcaba") == "aba" ||
              longestPalindrome("abba") == "abba",
          "\"abba\" -> \"abba\" (even-length wins)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
