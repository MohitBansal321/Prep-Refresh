# Longest Common Subsequence — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of recognizing "two sequences, order-preserving comparison" and mapping it onto the `dp[i][j]` table shape, then adapting the base case, cell formula, or post-processing for each variation.

> Rule of thumb for every exercise: first write down, in one sentence, what `dp[i][j]` *means* for this specific problem. If you can't state that sentence, you are not ready to write the recurrence yet.

---

## Easy — Is Subsequence

Given two strings `s` and `t`, determine whether `s` is a subsequence of `t` (LeetCode 392).

**Requirements:**
- Do NOT build a full LCS table for this one — think about why a simple linear two-pointer scan over both strings answers this in `O(|s| + |t|)`, with no table at all.
- Then, as a check on your understanding, explain in a comment why this *could* also be solved by computing `longestCommonSubsequence(s, t)` and checking whether the result equals `s.length()` — and why that would be strictly worse.

**Then answer in a comment:** why does this problem NOT need the full DP machinery, even though it is superficially "about" subsequences?

---

## Medium — Longest Palindromic Subsequence via LCS

Compute the length of the longest palindromic subsequence of a single string `s`, but do it by calling `longestCommonSubsequence(s, reverse(s))` rather than writing a fresh interval-DP table.

**Task:**
1. Prove to yourself (in a comment) why the LCS of a string and its own reverse always equals the length of its longest palindromic subsequence.
2. Implement it as a one-line call into your existing `longestCommonSubsequence` function.
3. Compare this approach's complexity to the dedicated interval-DP solution in `../palindromic-subsequence/`. Which is asymptotically better, and why might you still prefer the interval-DP version in production code?

---

## Hard — Longest Repeating Subsequence

Given a single string `s`, find the length of the longest subsequence that appears **at least twice** in `s`, where the two occurrences must NOT reuse the same character position (i.e., if a character repeats at the same index in both copies of the subsequence, that's invalid — the two occurrences must be genuinely distinct index sets, not just distinct index sets that happen to include the same position twice).

**Requirements:**
- This is LCS of `s` with itself, with one extra constraint added to the recurrence: on a "match" (`s[i-1] == s[j-1]`), only extend the diagonal if `i != j` (the two positions are genuinely different indices).
- Explain in a comment exactly why the ordinary LCS recurrence's match case would otherwise (incorrectly) match every character against itself, always producing `dp[n][n] = n`.

**Prove it:** for `s = "aabb"`, the longest repeating subsequence is `"a"` or `"b"` (length 1) — walk through by hand why plain self-LCS would wrongly report 4.

---

## Real-World Challenge — Diff Two Config Files

You have two versions of a JSON-like config file, each pre-tokenized into a list of `"key=value"` strings (order matters; the file is a flat list, not nested).

**Task:**
1. Compute the LCS of the two token lists — this is the "unchanged" skeleton.
2. Using the walk-back reconstruction, produce a simple diff output: lines present only in the old version are marked `- line`, lines present only in the new version are marked `+ line`, and shared lines (the LCS) are printed unmarked.
3. Test it on a config with a line inserted in the middle, a line removed, and a line whose value changed (which should show up as a `-`/`+` pair for that key, since it's a different token even if the key matches) — confirm the "changed" region shown is minimal (a real diff shouldn't mark the entire file as changed just because one line moved).
4. **Distributed-systems framing:** two microservices independently generate an ordered event log for what should be the "same" business transaction, but due to retries or reordering of asynchronous calls, the logs aren't byte-identical. Explain in writing how you'd use LCS-based diffing to identify which events are genuinely missing from one side versus which are just reordered — and why "reordered" events would show up as both a `-` and a `+` in a naive LCS diff, even though nothing was actually lost.

---

## Bonus Challenge — Three Related Problems, One Table

Implement all three of the following using the *exact same* `(n+1) x (m+1)` table-building loop as your `longestCommonSubsequence` function, changing only the base case and/or the per-cell formula:

1. **Edit Distance** (LeetCode 72) — minimum insertions/deletions/substitutions to turn one string into another.
2. **Shortest Common Supersequence length** (the length-only version of LeetCode 1092) — the length of the shortest string that has both inputs as subsequences.
3. **Delete Operation for Two Strings** (LeetCode 583) — minimum deletions (from either string) to make the two strings equal.

For each, write one sentence stating: (a) what `dp[i][j]` means for that specific problem, (b) how the base case (`dp[i][0]`, `dp[0][j]`) differs from plain LCS's "always 0," and (c) how the per-cell formula differs on a mismatch.

**Then, the punch line:** show that Shortest Common Supersequence's length and Delete Operation's answer can BOTH be derived from plain LCS's length alone, using simple arithmetic (`n + m - lcs_length` for supersequence length; `n + m - 2 * lcs_length` for total deletions) — without a separate table at all. Explain in a comment why Edit Distance, in contrast, genuinely needs its own per-cell formula (substitution cost) rather than being derivable from the plain LCS length after the fact.

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
