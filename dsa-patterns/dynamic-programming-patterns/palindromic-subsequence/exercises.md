# Palindromic Subsequence / Substring — Exercises

Work through these in order. The goal is to build two reflexes: (1) instantly splitting "palindrome in one string" problems into **contiguous (substring)** vs **gaps-allowed (subsequence)**, because that fork decides the entire technique; and (2) writing the interval DP fill order correctly on the first try — `i` descending, `j` ascending from `i+1` — without copying it.

> Rule of thumb for every exercise: before writing a single line, ask "does the palindrome have to be contiguous?" and "do I need just one answer, or a reusable is-palindrome table over all intervals?" If you cannot answer both questions, you are not ready to pick between expand-around-center and interval DP yet.

---

## Easy — Longest Palindrome from Character Pool

**LeetCode 409 — Longest Palindrome.**

Given a string of uppercase and lowercase letters, return the length of the longest palindrome that can be **built with those letters** — rearrangement allowed, letters used at most as many times as they appear.

**Constraints to notice:** this is *not* a subsequence question — you may reorder freely. A palindrome consumes characters in matched pairs, plus at most one unpaired character dead-center. Case matters (`'A'` ≠ `'a'`).

**Task:** solve it with a frequency count in O(n) time — no DP table, no expansion. Then answer: why does this problem sit *outside* both techniques of this module, even though its title contains the word "palindrome"? What property of the problem makes pairing-counting sufficient?

---

## Medium — Valid Palindrome After Deleting At Most One Character

**LeetCode 680 — Valid Palindrome II.**

Given a string, return true if it can be a palindrome after deleting **at most one** character.

**Constraints to notice:** only lowercase letters; strings up to 10⁵ mean an O(n²) interval table is wasteful — you want O(n). The classic two-pointer scan from both ends works until it hits the first mismatch, and then there are exactly two candidate deletions to verify.

**Task:** implement the two-pointer scan, and on the first mismatch check whether either remaining substring (`left+1..right` or `left..right-1`) is a plain palindrome. Reuse your own `isPalindromeRange` helper rather than building substrings.

**Think about:** why is checking only the two substrings around the *first* mismatch sufficient — no need to consider deleting any other character? What does each of those two checks correspond to in terms of the interval `[i..j]`?

---

## Hard — Palindrome Partitioning II

**LeetCode 132 — Palindrome Partitioning II.**

Given a string, partition it so every substring is a palindrome, returning the **minimum number of cuts** needed.

**Constraints to notice:** n up to ~2000, so O(n²) time/space is acceptable but O(n³) (checking each candidate piece inside the cut-DP) is not. The palindrome test must be precomputed once and reused.

**Task:** first build the boolean interval table `isPal[i][j]` using the recurrence from [code.cpp](code.cpp)'s substring discussion (`s[i] == s[j] && (length ≤ 2 || isPal[i+1][j-1])`). Then run a second 1-D DP over prefixes: `minCuts[j]` = minimum cuts for `s[0..j]`, transitioning through every palindromic suffix start `i`.

**Think about:** the problem decomposes into *two* DPs sharing one table. Why can't the cut DP itself reuse the LPS-style integer table from [code.cpp](code.cpp), and what exactly does the boolean variant store that the integer variant does not? Also: why does filling `isPal` by increasing interval length remain mandatory even though the final answer comes from the other DP?

---

## Real-World Challenge — Typo-Tolerant Palindrome Detection for SKU Codes

Your warehouse prints mirror-symmetric SKU codes (e.g. `AB3BA`) because scanners read them correctly upside-down. Occasionally a printing glitch corrupts exactly one character. You receive a batch of codes and must flag every code that is **either already a palindrome or becomes one if any single character were different** — those batches need reprinting.

**Task:**
1. Implement detection using expand-around-center: run the normal center expansion, but allow **one mismatch budget** during expansion instead of stopping at the first mismatch (try continuing past a mismatch on either side, i.e. skip `left` once or skip `right` once).
2. Decide and justify: should a corrupted code count if the fix would change its *length*? State your rule precisely before coding it.
3. Discuss: your expansion-with-budget runs in O(n²) per code. Codes are short (< 50 chars) and batches are millions long — where does your time actually go, and what trivial preprocessing could reject most non-candidates before any expansion starts? (Hint: character-frequency parity.)

---

## Bonus Challenge — Manacher's Algorithm

No LeetCode number — this is the O(n) upgrade to [code.cpp](code.cpp)'s expand-around-center.

**Task:** implement Manacher's algorithm for longest palindromic substring: transform the string by inserting separators (`#`) between every pair of characters so all palindromes become odd-length, then maintain the rightmost-reaching palindrome `(center, right)` seen so far and initialize each position's radius from its mirror around `center`, expanding only when necessary.

**Then, generalize in writing (no code required):** compare Manacher's amortized argument to the naive expansion in [code.cpp](code.cpp). Naive expansion re-walks characters covered by earlier palindromes; explain in your own words what bookkeeping lets Manacher skip that re-walking, and why the same idea does *not* transfer to the subsequence version of the problem (what breaks when gaps are allowed?).

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
