# Palindromic Subsequence / Substring — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Dynamic Programming pattern — interval DP over a single string, paired with the expand-around-center technique for contiguous substrings. |
| **Recognition Signal** | The question is about **palindromes within ONE string**: longest palindromic substring/subsequence, counting palindromic substrings, minimum insertions/deletions/partitions to make or split palindromes. The decisive fork: **contiguous** (substring) vs **gaps allowed** (subsequence) — they need different techniques. |
| **Problem** | Checking every substring/subsequence for palindromicity by brute force costs O(n³) (all O(n²) substrings × O(n) palindrome checks) or worse for subsequences (2^n candidates); naive recursion without memoization recomputes overlapping intervals exponentially. |
| **Solution** | **Expand around center** (substrings): try all `2n − 1` centers (n chars + n−1 gaps), expand outward while characters match. **Interval DP** (either flavor): `dp[i][j]` answers the question over interval `[i..j]`, combining `s[i] == s[j]` with the strictly smaller interval `[i+1, j-1]`; fill by **increasing interval length**, not row-by-row. Shortcut: LPS(s) = LCS(s, reverse(s)). |
| **Time / Space Complexity** | Expand around center: O(n²) time, O(1) space. Interval DP: O(n²) time, O(n²) space. Derived answers are free once LPS length is known (e.g. min insertions = n − LPS). |
| **Pros** | Handles every "palindrome inside one string" phrasing with one mental model · expand-around-center needs no table at all · interval DP composes: the same boolean palindrome table powers counting (647), partitioning (132), and min-insertion (1312) answers · provably optimal substructure over intervals. |
| **Cons** | O(n²)/O(n²) blows past memory for n ≳ 10⁴ · interval fill order is easy to get wrong (reads uninitialized cells) · substring vs subsequence confusion silently produces wrong answers, not crashes · expand-around-center cannot answer subsequence questions at all · Manacher's O(n) exists but is rarely worth memorizing outside contests. |
| **Use When** | One string + palindrome property asked about its parts (longest/count/min-changes) · you need a reusable "is s[i..j] a palindrome?" table for follow-up logic (partitioning, cutting) · subsequences are allowed (interval DP or LCS-on-reversed). |
| **Avoid When** | **Two different strings** are compared — that is Longest Common Subsequence ([../longest-common-subsequence/](../longest-common-subsequence/)) · you need O(n) longest palindromic *substring* specifically (Manacher's) · the string is huge (n > ~5000) and only the count matters — consider Eertree/palindromic tree or Manacher-based counting. |
| **Related Patterns** | Longest Common Subsequence ([../longest-common-subsequence/](../longest-common-subsequence/)): LPS(s) = LCS(s, reverse(s)); visually similar 2D tables but different fill orders and different meanings of `[i][j]` · DP on Grids ([../dp-on-grids/](../dp-on-grids/)): also a 2D table, filled row-by-row instead of by interval length · Two Pointers (array-string family): the palindrome *check itself* (`left`/`right` converging inward) is a two-pointer subroutine inside this pattern. |

### Template Skeleton

```cpp
// Expand around center -- longest palindromic SUBSTRING (contiguous only).
int start = 0, maxLen = 1;
auto expand = [&](int left, int right) {
  while (left >= 0 && right < n && s[left] == s[right]) { --left; ++right; }
  int len = right - left - 1;              // last valid window
  if (len > maxLen) { maxLen = len; start = left + 1; }
};
for (int c = 0; c < n; ++c) {
  expand(c, c);        // odd-length: center is a character
  expand(c, c + 1);    // even-length: center is a gap between chars
}

// Interval DP -- longest palindromic SUBSEQUENCE (gaps allowed).
std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
for (int i = n - 1; i >= 0; --i) {         // i DESCENDING guarantees
  dp[i][i] = 1;                            //   [i+1][*] is ready before [i][*]
  for (int j = i + 1; j < n; ++j) {
    dp[i][j] = (s[i] == s[j]) ? dp[i + 1][j - 1] + 2
                              : std::max(dp[i + 1][j], dp[i][j - 1]);
  }
}
return dp[0][n - 1];
```

### Remember In One Sentence

> **Palindromic interval DP answers "what is the best palindrome inside s[i..j]?" by peeling the two endpoints — match them and add 2 to the inner interval's answer, or drop one endpoint and take the max — provided the table is filled shortest-interval-first so every inner interval is finished before any interval containing it begins.**

### Two Facts People Get Wrong

- Interval DP can be filled row-by-row just like LCS? **No** — `dp[i][j]` reads `dp[i+1][j-1]`, a *strictly smaller interval nested inside*, not "the cell above-left." Row-by-row leaves those cells uncomputed; you must iterate `i` from n−1 down (or by increasing interval length).
- Expand-around-center also solves the subsequence version? **No** — expansion walks *adjacent* characters outward, so it only ever sees contiguous substrings; a subsequence skips characters, which breaks the walk entirely. Subsequence questions require interval DP (or the LCS(s, reverse(s)) shortcut).

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. How many possible palindrome centers does a string of length n have, and why is it not just n?
2. Write (from memory) the recurrence for `dp[i][j]` in longest-palindromic-subsequence, including what happens when `s[i] == s[j]`.
3. Why must the interval DP table be filled by increasing interval length rather than row-by-row? Which exact dependency forces this?
4. In the recurrence, when `s[i] == s[j]` the code takes `dp[i+1][j-1] + 2` and never compares against dropping an endpoint — why is skipping that comparison still correct?
5. What do `dp[i][i]` and `dp[i][i+1]` represent, and what happens in the code if `s[i] != s[i+1]` for the latter?
6. State the shortcut connecting this pattern to LCS, and explain why it produces the same answer.
7. Minimum insertions to make a string a palindrome equals what expression involving LPS? Give the one-line argument.
8. In expand-around-center, after the while loop exits, why is the palindrome length exactly `right - left - 1` and not `right - left + 1`?
9. You need "minimum cuts so every piece is a palindrome" (LeetCode 132). Which data structure from this module do you reuse, and what does the outer DP over it minimize?
10. Name the O(n)-time algorithm for longest palindromic substring, and give one reason it is usually not the interview answer of choice.
