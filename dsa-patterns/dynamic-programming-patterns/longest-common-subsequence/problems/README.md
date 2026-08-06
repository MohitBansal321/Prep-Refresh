# Longest Common Subsequence — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating the LCS table shape across its main uses: raw LCS length, the edit-distance variant (different cost, same table), building an actual merged string from the table (not just a number), and using LCS length as a subroutine inside a one-line formula. Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-longest-common-subsequence.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Longest Common Subsequence | [1143](https://leetcode.com/problems/longest-common-subsequence/) | Medium | Classic `dp[i][j]` table: match extends the diagonal, mismatch takes the best of skipping either string's current character. | O(n·m) time, O(n·m) space | [01-longest-common-subsequence.cpp](01-longest-common-subsequence.cpp) |
| Edit Distance | [72](https://leetcode.com/problems/edit-distance/) | Hard | Same `dp[i][j]` grid and fill order as LCS, but a match copies the diagonal unchanged (no `+1`) and a mismatch costs `1 + min(replace, delete, insert)` instead of `max` of two skips. | O(n·m) time, O(n·m) space | [02-edit-distance.cpp](02-edit-distance.cpp) |
| Shortest Common Supersequence | [1092](https://leetcode.com/problems/shortest-common-supersequence/) | Hard | Build the LCS table, then walk it backwards weaving both strings together — write shared (LCS) characters once, unshared characters from whichever string they belong to. | O(n·m) time, O(n·m) space | [03-shortest-common-supersequence.cpp](03-shortest-common-supersequence.cpp) |
| Delete Operation for Two Strings | [583](https://leetcode.com/problems/delete-operation-for-two-strings/) | Medium | `answer = len(word1) + len(word2) - 2 * LCS(word1, word2)` — everything outside the shared subsequence must be deleted from its own string. | O(n·m) time, O(n·m) space | [04-delete-operation-for-two-strings.cpp](04-delete-operation-for-two-strings.cpp) |

## Why these four

They cover every recognition signal and every "shape variation" called out in the [README](../README.md):

- **01** is the textbook LCS table in its purest form — the direct ancestor of every other file here.
- **02** shows the *exact same table shape* (dp[i][j] over two prefixes, same fill order) answering a *different question* (minimum edit cost, not shared-subsequence length) — proof that the table shape, not the specific recurrence, is the reusable part of the pattern.
- **03** is the canonical example of reconstructing an actual STRING (not just a length) by walking the LCS table backwards — a strictly harder bookkeeping problem than reconstructing the LCS itself, because every character (shared or not) must be emitted, not just the matched ones.
- **04** shows LCS length used as a one-line subroutine inside an unrelated-looking problem — recognizing "this is secretly LCS" is often the hardest part of solving it.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
