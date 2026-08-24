# Palindromic Subsequence / Substring

> **5-min refresher instead?** [cheatsheet.md](cheatsheet.md) has the one-table summary and recall questions.

## Intent

Find or count palindromic structures within a single string, using either expand-around-center (for contiguous substrings) or interval DP (for both substrings and non-contiguous subsequences).

## Recognition Signal

The problem asks about palindromes within ONE string — longest palindromic substring/subsequence, or counting palindromic partitions. Contiguous (substring) and non-contiguous (subsequence) need different techniques.

## Core Idea

**Expand around center** (substrings only): a palindrome is symmetric around some center — either a single character (odd length) or a gap between two characters (even length). There are exactly `2n - 1` possible centers in a string of length `n`. For each center, expand outward while the characters on both sides match; track the longest match found. This is `O(n^2)` total (n centers, each expanding up to `O(n)`), with `O(1)` extra space.

**Interval DP** (substrings or subsequences): define `dp[i][j]` over the interval `[i, j]`. For a palindromic-substring check: `dp[i][j]` is true if `s[i] == s[j]` AND `dp[i+1][j-1]` is true (or the interval is length ≤ 2). For the longest palindromic *subsequence* (gaps allowed): `dp[i][j]` = the longest palindromic subsequence length within `s[i..j]`, built from `dp[i+1][j-1] + 2` (if `s[i] == s[j]`) or `max(dp[i+1][j], dp[i][j-1])` otherwise. The critical structural difference from LCS or DP-on-Grids: **the table must be filled by increasing interval length**, not row-by-row, because `dp[i][j]` depends on the strictly-smaller interval `[i+1, j-1]`.

**A genuinely useful shortcut:** the longest palindromic *subsequence* of a string `s` equals `LCS(s, reverse(s))` — so if you already have an LCS implementation, you can reuse it directly instead of writing a fresh interval-DP table.

## Template

```cpp
// Expand around center -- longest palindromic SUBSTRING.
std::string longestPalindromicSubstring(const std::string& s) {
  if (s.empty()) return "";
  int start = 0, maxLen = 1;
  auto expand = [&](int left, int right) {
    while (left >= 0 && right < (int)s.size() && s[left] == s[right]) { --left; ++right; }
    int len = right - left - 1;
    if (len > maxLen) { maxLen = len; start = left + 1; }
  };
  for (int center = 0; center < (int)s.size(); ++center) {
    expand(center, center);       // odd-length palindromes
    expand(center, center + 1);   // even-length palindromes
  }
  return s.substr(start, maxLen);
}

// Interval DP -- longest palindromic SUBSEQUENCE (gaps allowed).
int longestPalindromicSubsequenceLength(const std::string& s) {
  int n = s.size();
  std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
  for (int i = n - 1; i >= 0; --i) {
    dp[i][i] = 1;
    for (int j = i + 1; j < n; ++j) {
      dp[i][j] = (s[i] == s[j]) ? dp[i + 1][j - 1] + 2
                                 : std::max(dp[i + 1][j], dp[i][j - 1]);
    }
  }
  return dp[0][n - 1];
}
```

## Complexity

**Expand around center:** `O(n^2)` time, `O(1)` space.
**Interval DP:** `O(n^2)` time, `O(n^2)` space.

## Common Mistakes

- **Filling the interval DP table in the wrong order** (row-by-row instead of by increasing interval length, or equivalently `i` from n-1 down to 0 with `j` from `i+1` up) — causes reads of not-yet-computed cells.
- **Confusing "palindromic SUBSTRING" (contiguous) with "palindromic SUBSEQUENCE" (gaps allowed)** — these need different techniques entirely; expand-around-center only answers the substring version.
- **Off-by-one on single-character (`dp[i][i] = 1`) and two-character base cases** in the interval DP.

## When To Use

- Finding/counting palindromic substrings or subsequences within one string.

## When NOT To Use

- **Comparing TWO different strings** — that's Longest Common Subsequence ([../longest-common-subsequence/](../longest-common-subsequence/)), not this pattern.
- **Need O(n) longest palindromic substring specifically** — Manacher's algorithm achieves this, though it's rarely needed outside competitive programming.

## Similar Patterns

- **Longest Common Subsequence** ([../longest-common-subsequence/](../longest-common-subsequence/)): the longest palindromic subsequence of `s` equals `LCS(s, reverse(s))` — a genuinely useful cross-pattern shortcut. LCS's `dp[i][j]` indexes two independent sequences; this pattern's `dp[i][j]` indexes one interval within a single sequence — visually similar tables, structurally different fill orders.
- **DP on Grids** ([../dp-on-grids/](../dp-on-grids/)): also a 2D table, but filled row-by-row rather than by increasing interval length.

## Further Reading

- LeetCode — Longest Palindromic Substring (5), Longest Palindromic Subsequence (516), Palindromic Substrings (647), Palindrome Partitioning II (132, hard).
- Wikipedia — Manacher's algorithm, the O(n) technique for longest palindromic substring specifically.
