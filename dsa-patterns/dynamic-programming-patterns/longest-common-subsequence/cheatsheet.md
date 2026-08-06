# Longest Common Subsequence (LCS) — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Dynamic Programming — two-sequence, prefix-pair state. |
| **Recognition Signal** | Comparing TWO sequences/strings for a longest shared or edit-distance-style relationship, where matches don't need to be contiguous. |
| **Problem** | Brute-force subsequence enumeration is `O(2^n * 2^m)` — doubly exponential, unusable past ~20-25 characters per string. |
| **Solution** | `dp[i][j]` = LCS length of the first `i` chars of A and first `j` chars of B. Match => extend diagonal (`dp[i-1][j-1] + 1`). Mismatch => `max(dp[i-1][j], dp[i][j-1])`. |
| **Participants** | Two read-only input sequences A, B · the `(n+1) x (m+1)` `dp` table (row/col 0 = empty-prefix base case) · the match/mismatch recurrence · the fill order (row-by-row or column-by-column, either works) · the optional backward walk-back to reconstruct the actual subsequence. |
| **Flow** | Init row 0 / col 0 to 0 -> fill every cell left-to-right, top-to-bottom, applying the recurrence -> `dp[n][m]` is the answer -> (optional) walk backward from `(n,m)` to `(0,0)` retracing which branch produced each cell to recover the actual subsequence. |
| **Pros** | Polynomial (`O(n*m)`) where brute force is exponential · one table shape covers Edit Distance, Shortest Common Supersequence, Delete Operation for Two Strings · both length AND actual answer recoverable from the same table. |
| **Cons** | `O(n*m)` time/space can be large for long strings · reconstruction requires the FULL table (space optimization to `O(min(n,m))` only works for length-only) · off-by-one between table indices and sequence indices is a common bug. |
| **Use When** | Diffing/merging two ordered sources (file revisions, logs) · edit-distance-style transformation cost · DNA/protein sequence alignment · plagiarism/similarity detection · any state naturally indexed by two independent prefix lengths. |
| **Avoid When** | Comparing ONE sequence to itself (palindrome) -> Palindromic Subsequence · need an ORDER relation within one sequence, not equality -> Longest Increasing Subsequence · need longest *contiguous* shared run -> Longest Common Substring (different recurrence: mismatch resets to 0) · just need a yes/no "is s a subsequence of t" -> plain two-pointer scan, no table needed. |
| **Related Topics** | Edit Distance (same table, cost-based cell formula) · Shortest Common Supersequence (`n+m-lcs` length, or full reconstruction) · Delete Operation for Two Strings (`n+m-2*lcs`) · Longest Increasing Subsequence (1D, one sequence) · Palindromic Subsequence (2D over one interval, not two sequences) · Hirschberg's algorithm (linear-space LCS reconstruction) · Needleman-Wunsch / Smith-Waterman (bioinformatics alignment). |

### Skeleton
```cpp
template <typename Seq>
int longestCommonSubsequence(const Seq& a, const Seq& b) {
  int n = a.size(), m = b.size();
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
  for (int i = 1; i <= n; ++i) {
    for (int j = 1; j <= m; ++j) {
      if (a[i - 1] == b[j - 1]) dp[i][j] = dp[i - 1][j - 1] + 1;       // match: extend diagonal
      else dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);            // mismatch: best of either skip
    }
  }
  return dp[n][m];
}
// Reconstruction: walk backward from (n,m); a[i-1]==b[j-1] -> record char, move diagonally;
// else move toward whichever of dp[i-1][j] / dp[i][j-1] is larger. Reverse the collected chars.
```

### Remember In One Sentence
> **LCS fills a `(n+1) x (m+1)` table where a character match always extends the diagonal and a mismatch always takes the better of dropping a character from either string — turning a doubly-exponential subsequence search into an `O(n*m)` table, with Edit Distance, Shortest Common Supersequence, and Delete Operation for Two Strings all being the same table wearing a different cell formula.**

### Two Facts People Get Wrong
- It is about the longest common **substring**? **No** — LCS explicitly allows gaps (order preserved, not contiguous). Substring problems use a different recurrence (mismatch resets to 0, not `max`).
- The LCS is always **unique**? **No** — multiple distinct subsequences can share the same maximum length (e.g. `"ABCBDAB"` vs `"BDCABA"` has two valid length-4 answers).

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. State what `dp[i][j]` means in one sentence. Why is the table sized `(n+1) x (m+1)` instead of `n x m`?
2. Give the exact recurrence for both the match case and the mismatch case.
3. Why can't the table be filled in an arbitrary order — what property of the recurrence requires row-by-row or column-by-column (rather than, say, diagonal-by-diagonal only)?
4. Why does reconstructing the actual subsequence require the FULL table, and what space optimization becomes unavailable once you need it?
5. How does Edit Distance's table differ from plain LCS's — same shape, but what changes in the base case and the per-cell formula?
6. Why is "longest common substring" a different, easier-to-get-wrong-with problem than "longest common subsequence"? What's the one-word recurrence difference?
7. Is the LCS between two strings always unique? Give a concrete counterexample.
8. What's the off-by-one bug that most commonly bites people implementing this, and why does it happen more often in one branch than the other?
9. How would you derive the length of the Shortest Common Supersequence from the plain LCS length alone, without a separate table?
10. When would you reach for Longest Increasing Subsequence or Palindromic Subsequence instead of LCS, even though all three involve the word "subsequence"?
