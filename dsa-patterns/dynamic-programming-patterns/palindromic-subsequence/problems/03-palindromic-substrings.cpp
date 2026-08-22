// ============================================================================
// LeetCode 647 — Palindromic Substrings
// ============================================================================
//
// PROBLEM
// -------
// Given a string s, return the number of palindromic SUBSTRINGS of s
// (contiguous). Each occurrence counts separately even if the substring is
// the same text: "aaa" contains "a" three times, "aa" twice, "aaa" once = 6.
//
// Example: s = "abc" -> 3   ("a", "b", "c")
//          s = "aaa" -> 6   ("a"x3, "aa"x2, "aaa")
//
// APPROACH — Expand around center, counting instead of tracking the longest
// ---------------------------------------------------------------------------
// This is LeetCode 5 (longest palindromic substring) with one line changed:
// instead of remembering the longest window each expansion produced, we COUNT
// every successful expansion step.
//
// Why counting steps is correct: starting at center (left, right), each
// iteration of the while loop verifies exactly ONE palindrome — the window
// [left, right] after both pointers move. Every palindrome has exactly one
// center and is reached by exactly one chain of expansion steps from that
// center, so summing verified windows over all 2n - 1 centers counts every
// palindromic substring occurrence exactly once. No double counting, no
// misses.
//
// Concretely: a maximal expansion of length k from a given center contributes
// exactly k palindromes — all nested inside each other (e.g. "aaaa" centered
// on the middle gap yields "aa", "aaaa": two steps, two palindromes), and
// nesting is what makes the per-step count valid: every prefix of the
// outward walk is itself a palindrome.
//
// COMPLEXITY
// ----------
// Time:  O(n^2) — 2n-1 centers, each expanding at most O(n) steps; total work
//        equals total successful expansion steps.
// Space: O(1) — just the two walking indices and a counter.
//
// Contrast: brute force checks all O(n^2) substrings with an O(n) palindrome
// test = O(n^3). An interval-DP boolean table also works in O(n^2) time but
// pays O(n^2) space; when only a count is needed, expansion is strictly
// better here.
// ============================================================================

#include <iostream>
#include <string>

int countSubstrings(const std::string& s) {
  int n = static_cast<int>(s.size());
  int count = 0;

  // Each successful while-iteration certifies exactly one palindromic
  // substring (the current [left, right] window) — count it.
  auto expand = [&](int left, int right) {
    while (left >= 0 && right < n && s[left] == s[right]) {
      ++count;   // window [left..right] is a palindrome occurrence
      --left;
      ++right;
    }
  };

  for (int center = 0; center < n; ++center) {
    expand(center, center);      // odd-length palindromes (single-char centers)
    expand(center, center + 1);  // even-length palindromes (gap centers)
  }

  return count;
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
    // No repeats anywhere: only the three single characters qualify.
    check(countSubstrings("abc") == 3, "\"abc\" -> 3");
  }

  {
    // The banner example: "a"x3 + "aa"x2 + "aaa" = 6. Exercises repeated
    // nesting from both char and gap centers.
    check(countSubstrings("aaa") == 6, "\"aaa\" -> 6");
  }

  {
    // Single character edge case.
    check(countSubstrings("a") == 1, "\"a\" -> 1");
  }

  {
    // Empty string edge case.
    check(countSubstrings("") == 0, "\"\" -> 0");
  }

  {
    // Two identical adjacent chars: "b", "b", "bb" = 3.
    check(countSubstrings("bb") == 3, "\"bb\" -> 3");
  }

  {
    // Mixed: singles(4) + "bb"(1 occurrence) + "abba"(1) = 6. The full-string
    // even palindrome forces a long gap-centered expansion.
    check(countSubstrings("abba") == 6, "\"abba\" -> 6");
  }

  {
    // Palindromic structure buried among distractors: "xabax" is itself a
    // palindrome, and contains "aba". Count: 5 singles + "aba" + "xabax" = 7.
    // No length-2 or length-4 substring qualifies.
    check(countSubstrings("xabax") == 7,
          "\"xabax\" -> 7 (singles + \"aba\" + whole string)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
