# Longest Common Subsequence (LCS)


> **In one line:** `dp[i][j]` = LCS length of the first `i` and first `j` characters — extend the diagonal on a match, else take the better of dropping one character from either side.

```cpp
std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

for (size_t i = 1; i <= n; ++i) {
  for (size_t j = 1; j <= m; ++j) {
    if (a[i - 1] == b[j - 1]) {
      dp[i][j] = dp[i - 1][j - 1] + 1;              // extend the diagonal match
    } else {
      dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);  // best of dropping one side
    }
  }
}
return dp[n][m];
```

**O(n·m)** time and space, for strings of length n and m. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Compare two sequences by building a 2D table where each cell holds the best answer for a *pair of prefixes* (the first `i` elements of one sequence, the first `j` of the other), so overlapping subproblems are computed once and reused instead of recomputed exponentially many times.

## Real Life Analogy

Think about how **`diff` and version control** (Git, in particular) show what changed between two revisions of a file. `git diff` doesn't treat the file as two unrelated blobs — it finds the **lines both versions share, in the same relative order**, and shows everything else as an addition or deletion around that shared skeleton. Rename a variable on line 12 and leave lines 1-11 and 13-40 untouched, and `diff` shows one changed line, not forty.

That "shared skeleton, in the same relative order" is exactly a **longest common subsequence**. The shared lines need not be *contiguous* in either file — delete ten lines in the middle of the old version and the shared lines on either side of that gap still count, because subsequence only requires preserving relative order, not adjacency. Git's three-way merge algorithm relies on exactly this: finding the common ancestor lines between two diverging branches to combine both sides' changes without duplicating what neither side touched.

LCS is the general algorithmic idea behind that comparison: given two sequences, find the longest run of elements that appears in both, in the same order, allowing gaps.

## Problem

A large family of problems boils down to: **given two sequences, how much do they have in common, or what does it cost to turn one into the other?**

- **Version control / diff tools** need the longest shared skeleton between two file revisions to display (and merge) only the actual changes.
- **Spell-checkers and fuzzy search** need how many edits (insert/delete/substitute) separate a typed word from a dictionary word, to rank "did you mean...?" suggestions.
- **DNA/protein sequence alignment** needs how much of two genetic sequences match, in order, to measure evolutionary similarity or detect mutations.
- **Plagiarism/code-similarity detection** needs how much of one document's token sequence reappears, in order, inside another — even if paragraphs were reordered or filler text inserted between copied sentences.

> **Term: Subsequence.** A sequence derived from another by deleting zero or more elements *without changing relative order* — `"ace"` is a subsequence of `"abcde"` (delete `b`, `d`), even though it never appears as a contiguous block. **Subsequence is not substring** — the single most important distinction in this module.

The naive way to answer "how much do these share?" enumerates every subsequence of the first string (`2^n` of them) against every subsequence of the second (`2^m`) — `O(2^n * 2^m)`, doubly exponential and unusable past strings of 20-25 characters.

Three things make this hard:

- **The search space is exponential in both inputs.** Unlike a single-sequence problem (`2^n`), LCS reasons about two sequences simultaneously, multiplying the two exponential spaces together.
- **Subsequence, not substring, is easy to get backward.** The instinct to slide a window for the longest *contiguous* match solves the different Longest Common **Substring** problem instead. LCS's gap-allowing relaxation is exactly what makes it solvable in polynomial time — but also what makes the recurrence non-obvious: a mismatch doesn't mean failure, it means *branching* into two ways forward.
- **The two sequences advance independently.** A single-sequence DP (Longest Increasing Subsequence) has one index; LCS has two independent indices, and capturing whether two positions "line up" in the shared subsequence is the whole recurrence's job.

Ignoring it means: a brute-force LCS over two 30-character strings is already beyond reasonable time, and real diffs/DNA sequences/documents run thousands to millions of characters. Sliding-window substring matching where subsequence semantics were needed silently produces a shorter or wrong answer whenever the true shared content has any gap — the common case, not the exception. And without understanding this table shape, Edit Distance, Shortest Common Supersequence, and dozens of other two-sequence problems each look like an unrelated, from-scratch problem instead of "the same table, different cell meaning."

## Solution

The insight that turns an exponential search into a polynomial one: the answer for two full sequences builds entirely out of answers to smaller **prefix pairs** of those same sequences, and there are only `(n+1) * (m+1)` distinct prefix pairs, not `2^n * 2^m` subsequence pairs.

Define `dp[i][j]` = **the length of the longest common subsequence between the first `i` characters of A and the first `j` characters of B.** Every other quantity in this module (edit distance, supersequence length, deletion counts) is a small variation on filling this same table.

The recurrence has exactly two cases, decided by comparing `A[i-1]` and `B[j-1]` (0-indexed sequences against a 1-indexed table — see Common Mistakes):

- **Match.** If `A[i-1] == B[j-1]`, that character can always be included, extending the best answer for the two prefixes one character shorter on both sides — the cell diagonally up-left. Including a matching character can never make the answer worse, so the recurrence commits unconditionally.
- **Mismatch.** If `A[i-1] != B[j-1]`, this pair of characters can't both be in the LCS at this position, so the answer comes from either dropping A's last character (`dp[i-1][j]`) or B's (`dp[i][j-1]`) — whichever is larger, since both are valid ways of shrinking the problem and only the better one can be part of an optimal answer.

A match always extends the diagonal; a mismatch always takes the best of "give up on this character of A" or "give up on this character of B." That single two-way branch, applied to every cell, is the entire algorithm.

**Running it:** initialize `dp[i][0] = dp[0][j] = 0` for all `i, j` (the empty-prefix base case, needing no special-casing thanks to the table being sized one larger than each sequence). Fill row by row or column by column — either works, since every cell depends only on cells strictly above, left, or diagonally up-left, and both orders guarantee those are already computed. Read `dp[n][m]` as the answer. If the actual subsequence (not just its length) is needed, walk backward from `(n, m)`: on a match, record the character and step diagonally to `(i-1, j-1)`; otherwise step to whichever of `(i-1, j)` or `(i, j-1)` has the larger `dp` value, retracing the branch the forward pass took. Stop at `i == 0` or `j == 0`, then reverse the collected characters (they were recorded end-to-start).

## Architecture

1. **The two input sequences, A and B.** Read-only throughout; every cell is defined purely from characters compared here and previously-computed cells.
2. **The `dp` table**, sized `(n+1) x (m+1)` — one larger than the sequence lengths specifically so row 0 and column 0 can represent the empty-prefix base case with no special-casing in the main loop.
3. **The match/no-match branching recurrence** — the single decision rule applied identically to every cell; everything else is bookkeeping around applying it `n * m` times.
4. **The fill order.** Because `dp[i][j]` depends only on cells strictly above, left, or diagonally up-left, the dependency graph has no cycles and always points toward the top-left corner — which is what makes the table fillable in either row-major or column-major order.
5. **The optional walk-back path (reconstruction).** A second phase retracing which branch produced each cell's value, from `dp[n][m]` back to `dp[0][0]`. This requires the *entire* table to still exist in memory — a direct consequence explored in Tradeoffs and Complexity.

## Why Not Other Approaches?

**Brute force: generate every subsequence of both strings and compare.** `O(2^n * 2^m)` time (and at least that much space, if materialized). For strings of length 20 each, that's `2^40` ~ one trillion combinations — impractical even before accounting for real strings being far longer.

**Look for the longest common *substring* instead (sliding window / contiguous match).** A genuinely different problem: substring requires the shared content to be *contiguous*, a strictly stronger (and often much shorter) requirement than "same relative order, gaps allowed." For inputs where the shared characters are scattered with insertions between them, a substring search reports something far shorter than the true LCS, or misses the relationship entirely. If the problem says "subsequence," a substring approach answers the wrong question no matter how efficiently it runs.

**Greedily match characters left to right, advancing whichever string 'needs' it.** No local rule is always safe. Given `"abc"` and `"bac"`, greedily matching `a` (position 0 in both) commits to comparing `"bc"` against `"ac"`, missing the true best alignment. Unlike Two Pointers' provable monotonic elimination rule, whether skipping a character from the first or second string is "correct" depends on what the *rest* of both strings look like — invisible to a greedy left-to-right pass.

**Recursion without memoization (plain divide and conquer on prefixes).** The recursive relationship (match => recurse on both prefixes shortened by one; mismatch => recurse on two smaller subproblems, take the best) is correct, but the *same* `(i, j)` prefix pair recomputes repeatedly through different recursive paths — the classic overlapping-subproblems sign. Without memoization this degenerates back to exponential time, even though the recursion tree is finite and much smaller than brute-force enumeration. This is the closest "almost right" approach, and it's exactly what motivates converting the recursion into a bottom-up table.

**Net:** brute-force enumeration and un-memoized recursion are correct but exponential, re-solving overlapping subproblems from scratch; substring matching and greedy left-to-right matching are efficient but answer the *wrong problem*, discarding the "gaps allowed, no locally-safe choice" nature of subsequences. The table-based DP is the only approach both correct for actual subsequence semantics *and* polynomial, because it computes each `(i, j)` prefix-pair subproblem exactly once.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart distinguishing LCS from Longest Increasing Subsequence (one sequence, order relation) and Palindromic Subsequence (one sequence, interval-based), based on how many sequences the problem compares and what relationship is asked for.
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of the 2D table fill loop — match-extends-the-diagonal versus no-match-best-of-either-skip — including how Edit Distance reuses the identical loop structure with a different cell formula.
- [images/trace-diagram.md](images/trace-diagram.md) — cell-by-cell trace of the `dp` table on a small concrete example, plus the walk-back path reconstructing the actual LCS string from the finished table.

## The Code

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the *shape* (the table, the recurrence, the walk-back) before the worked, problem-specific solutions in [problems/](problems/), each of which reuses this exact table with small, deliberate variations.

- **`longestCommonSubsequence(a, b)`** — builds the full `(n+1) x (m+1)` table, zero-initialized (already satisfying the base case for free), applies the match/mismatch recurrence, and returns `dp[n][m]`, the LCS **length**. Templated to work over `std::string`, `std::vector<int>`, or any type supporting `.size()` and `operator[]` with comparable elements.
- **`reconstructLCS(a, b)`** — rebuilds the identical table (cheap enough, and keeping the functions independent keeps each simple to read alone), then walks it **backwards** from `(n, m)` to `(0, 0)` to recover the actual shared subsequence, appending matched characters and reversing the result once at the end.

Both functions build the full 2D table rather than a space-optimized rolling array deliberately: `reconstructLCS` cannot walk backward through rows a rolled version would have already overwritten, and keeping both functions structurally identical (aside from the walk-back) makes "just the length" versus "the actual answer" easy to compare side by side.

**`main()`** exercises both against hand-checkable inputs, including the classic `"ABCBDAB"` vs `"BDCABA"` textbook example (LCS length 4, with two equally valid answers, `"BCBA"` or `"BDAB"`, both accepted), and prints `[PASS]`/`[FAIL]`, including a consistency check that `reconstructLCS`'s output length always matches `longestCommonSubsequence`'s return value.

Each file in [problems/](problems/) is a complete, standalone solution (not calling into `code.cpp`), with problem-specific comments tying every recurrence back to the general table shape — see [problems/README.md](problems/README.md). Briefly: `01` is the pure LCS length recurrence; `02` (Edit Distance) reuses the identical table and fill order with a different per-cell formula (cost instead of shared length); `03` (Shortest Common Supersequence) builds the LCS table then walks it backward emitting *every* character encountered, not just matches, producing an actual merged string; `04` (Delete Operation for Two Strings) uses the plain LCS length as a one-line subroutine inside a small arithmetic formula.

## Tradeoffs

**What the table buys you**

- **Polynomial time where brute force is doubly exponential.** `O(2^n * 2^m)` subsequence enumeration collapses to `O(n * m)` — the difference between "cannot finish" and "instant" for any string longer than a few dozen characters, by recognizing only `(n+1) * (m+1)` distinct prefix-pair subproblems exist and computing each exactly once.
- **One table shape, many problems.** Edit Distance, Shortest Common Supersequence, and Delete Operation for Two Strings all reuse the identical `dp[i][j]` grid and fill order — learning the table once pays off across a whole family of interview and production problems.
- **Both the length AND the actual answer are recoverable.** LCS reconstruction is a direct backward walk through the same table already built for the length, unlike DP patterns where reconstruction needs substantially different machinery.
- **Correctness is easy to argue.** Every cell's value is justified by exactly two cases (match/no-match), each with a one-sentence proof.
- **Naturally composable** — the table-building step is identical across every LCS-derived problem; only the base case, per-cell formula, or post-processing arithmetic changes.

**What it costs you**

- **`O(n * m)` time and space can be large for long strings.** Two 10,000-character strings need a table with 100 million cells — at 4 bytes per `int`, 400 MB, a real constraint for large documents, long DNA sequences, or big diffs. Growth is quadratic in the *product* of both lengths, so it scales worse than most single-sequence patterns as inputs grow.
- **Reconstructing the actual subsequence requires keeping the full table**, or explicit extra bookkeeping (a direction table, or Hirschberg's divide-and-conquer technique) if the full table's memory cost is unacceptable — the length-only space optimization below is *not* available for free once reconstruction is needed.
- **Off-by-one bookkeeping between the table's indices and the sequences' indices is a recurring bug source** — every access into `a` or `b` inside the loop must subtract 1 from the table index, and it's easy to forget in one branch but not another (see Common Mistakes).

## Complexity

**Time:** `O(n * m)` — one constant-time decision per cell of an `(n+1) x (m+1)` table, in the best, worst, and average case alike, since the loop bound is structural, not data-dependent.

**Space:** `O(n * m)` for the full table, as used throughout `code.cpp` and `problems/` (needed for reconstruction in `reconstructLCS` and `problems/03`).

**Space optimization (length only):** if only the LCS **length** is needed — never the reconstructed subsequence — the table collapses to two 1D rows (or one row updated carefully), `O(min(n, m))`, because `dp[i][j]` only ever depends on the *current* and *immediately previous* row. This module's code doesn't use it, since every file here either demonstrates reconstruction directly or intentionally shows the most general form before specializing — but mentioning it when only a length is asked for is a meaningful interview signal.

| Problem shape | Brute force | 2D Table DP |
|---|---|---|
| LCS length | `O(2^n * 2^m)` time, exponential space to enumerate | `O(n * m)` time, `O(n * m)` space (or `O(min(n,m))` if length-only) |
| LCS reconstruction | `O(2^n * 2^m)` time, same as above | `O(n * m)` time, `O(n * m)` space (full table required) |
| Edit Distance | Exponential (unmemoized insert/delete/replace tree) | `O(n * m)` time, `O(n * m)` space |

## Common Mistakes

- **Off-by-one between the table's indices and the sequences' indices.** The table is `(n+1) x (m+1)` so row/column 0 can be the empty-prefix base case, but the sequences are still 0-indexed with only `n`/`m` characters. Every character access inside the loop must be `a[i-1]` and `b[j-1]`, **not** `a[i]`/`b[j]` — forgetting the `-1` in one branch (commonly the mismatch branch, since the match branch gets checked, and attention, first) silently shifts every comparison by one position and produces subtly wrong answers rather than a crash.
- **Confusing "characters match" with "substring match."** LCS is about *subsequence* — order preserved, gaps allowed — not contiguous substrings. A recurrence that only extends a match when the *previous* characters also matched (a substring recurrence) answers a different, usually much more restrictive, question. If the problem says "subsequence," the mismatch branch must fall back to `max(dp[i-1][j], dp[i][j-1])`, not reset to 0 the way Longest Common Substring's mismatch case does.
- **Forgetting this is a foundation for Edit Distance (and other variations), not a standalone trick.** Treating LCS, Edit Distance, and Shortest Common Supersequence as three unrelated problems to memorize independently is far more work than recognizing they share one table shape with different cell formulas — see [problems/README.md](problems/README.md) for exactly how each variation departs from the base recurrence.
- **Trying to reconstruct the subsequence from a space-optimized (rolled) table.** If the table has been collapsed to `O(min(n,m))` rows for the length-only optimization, the earlier rows a backward walk needs no longer exist — reconstruction and the rolling-array optimization are mutually exclusive without extra bookkeeping.
- **Assuming the LCS is unique.** Multiple distinct subsequences can share the same maximum length (see the `"ABCBDAB"`/`"BDCABA"` example, with two valid length-4 answers). Code that asserts a single "the" LCS string, rather than "a" LCS string of the correct length, can fail tests that correctly accept any valid answer.

## When To Use

- **Comparing two sequences (strings, token lists, gene sequences) for a longest shared subsequence**, where matches need not be contiguous.
- **Computing an edit-distance-style transformation cost** between two sequences — Edit Distance reuses this exact table shape.
- **Building a shortest supersequence, or counting deletions/insertions needed to reconcile two sequences** — both direct variations on the LCS table (see `problems/03` and `problems/04`).
- **Diffing, merging, or reconciling two ordered data sources** (file revisions, sorted logs with occasional gaps, two versions of a config) where "what's shared, in order" is the question.
- **Any problem whose state naturally indexes by two independent prefix lengths** — catching yourself defining `dp[i][j]` over two different sequences is a strong signal this is the pattern.

## When NOT To Use

- **Comparing ONE sequence to itself for an internal property** — e.g. checking whether it is (or contains) a palindrome. That's the **Palindromic Subsequence/Substring** pattern ([../palindromic-subsequence/](../palindromic-subsequence/)), whose `dp[i][j]` indexes an *interval* `[i, j]` inside a single sequence, not two independent prefixes from two sequences — there is only one sequence to index into.
- **Finding the longest *contiguous* shared run (substring, not subsequence).** Longest Common Substring uses a visually similar table but a different recurrence (mismatches reset the running length to 0 instead of falling back to `max` of neighbors) — using LCS's mismatch rule here silently answers the wrong (too permissive) question.
- **The problem involves ONE sequence, and the relationship is an ORDER property (increasing/decreasing), not equality.** That's Longest Increasing Subsequence's territory ([../longest-increasing-subsequence/](../longest-increasing-subsequence/)) — a 1D table over a single position, not a 2D table over two sequences.
- **The inputs are far too large for `O(n * m)` to finish** (two multi-million-character genomes compared naively) — production bioinformatics tools use banded alignment, Hirschberg's linear-space technique, or approximate/heuristic alignment (BLAST-style seed-and-extend) rather than the textbook full table.
- **You only need a yes/no** — whether one sequence contains the other as a subsequence at all, not the longest shared length — a simple linear two-pointer scan answers that in `O(n + m)` without a table.

## Where This Shows Up

LCS and its close variants (Edit Distance especially) are among the most frequently asked "hard" DP problems at major tech companies, precisely because getting the recurrence right under time pressure — and correctly distinguishing it from Longest Common Substring — genuinely separates candidates who understand the *shape* of two-sequence DP from those pattern-matching on memorized code.

In production systems:

- **`diff` tools and version control merge logic.** Git and diff utilities generally are built around finding the longest common subsequence (or a closely related edit-script algorithm, such as the Myers diff algorithm) between two file revisions, so only genuinely changed lines are shown or merged — the analogy this module opened with.
- **DNA/protein sequence alignment in bioinformatics.** Global alignment (Needleman-Wunsch) and local alignment (Smith-Waterman) are LCS/Edit-Distance-style dynamic programs over two biological sequences, used to measure evolutionary similarity, find conserved regions, and detect mutations.
- **Plagiarism and code-similarity detection.** Tools comparing submitted assignments or source files for copied content often tokenize both documents and compute an LCS-style similarity score, tolerating reordered whitespace, renamed variables (after normalization), and inserted filler between copied sections — something a naive substring search would miss.
- **Spell-checkers and fuzzy autocomplete.** Edit distance between a typed query and dictionary entries ranks "did you mean...?" suggestions by how many operations separate the two strings.
- **A "what changed" diff view** for JSON/config files in an admin tool — tokenize both versions (by line or key), run LCS to find the shared skeleton, render only the additions/removals around it.
- **Deduplication-aware log reconciliation**, measuring how much of one service's event order is reproduced in another's semantically similar (not byte-identical) log, tolerating occasional missing or reordered-adjacent events.
- **A basic plagiarism/duplicate-submission checker** for a coding-exercise platform — flag submission pairs whose LCS length, relative to length, exceeds a suspicious threshold.
- **API schema migration tooling** comparing an old and new request/response shape (as an ordered field list) to compute the minimum insert/rename/remove operations needed to migrate client code — an Edit-Distance-style application of the same table.

## Similar Patterns

- **Longest Increasing Subsequence** ([../longest-increasing-subsequence/](../longest-increasing-subsequence/)): also finds a "longest subsequence," but over **one** sequence, constrained by an **order relation** (each next element larger than the last), not by matching against a second sequence. Its state is 1D (`dp[i]` = best subsequence ending at `i`), versus LCS's 2D state over two independent prefix lengths — the two patterns share the word "subsequence" and nothing else about their state shape.
- **Palindromic Subsequence** ([../palindromic-subsequence/](../palindromic-subsequence/)): also a 2D `dp[i][j]` table, which makes it the pattern most easily confused with LCS at a glance — but `i` and `j` here index the two **ends of an interval inside a single sequence**, not two independent prefixes of two different sequences. The fill order differs accordingly: Palindromic Subsequence fills by increasing *interval length*, while LCS fills row-by-row (or column-by-column) over two separate axes.

| Pattern | How many sequences? | State shape | Fill order | Primary question |
|---|---|---|---|---|
| Longest Common Subsequence | Two | `dp[i][j]` = best answer for prefix `i` of A, prefix `j` of B | Row-by-row or column-by-column (either works) | "What's the longest shared order-preserving run between A and B?" |
| Longest Increasing Subsequence | One | `dp[i]` = best subsequence ending at index `i` | Left to right, one pass | "What's the longest run obeying an order relation within one sequence?" |
| Palindromic Subsequence | One (indexed by an interval) | `dp[i][j]` = answer for the interval `[i, j]` | By increasing interval length | "What's the longest/best palindromic structure within one sequence?" |

## Interview Discussion

Experienced engineers rarely spend interview time on "can you write the double loop" — mechanical once the recurrence is understood. What they probe is whether you can **state the recurrence's two cases and justify each one**, and recognize when a "new" problem is actually this same table wearing a different cell formula.

Follow-ups worth rehearsing:
- *"Can you also return the actual longest common subsequence, not just its length?"* — expects recognizing that the full table (not a rolled array) must be kept, and describing or implementing the backward walk.
- *"How is this related to Edit Distance?"* — expects the identical table shape and fill order, differing only in the base case (`0` vs. `i`/`j`) and the per-cell formula (`max` of two skips vs. `1 + min` of three operations).
- *"What if the strings are very large (millions of characters) and `O(n*m)` memory is too much?"* — expects mentioning Hirschberg's algorithm (linear-space LCS via divide and conquer) or banded/heuristic alignment, even without full implementation detail.
- *"Is the LCS always unique?"* — expects a clear "no," with a concrete example of two equally valid maximum-length answers.

Misconceptions worth killing early:
- **"The recurrence needs to consider dropping characters from *both* strings at once on a mismatch."** It only ever needs the better of the two single-character skips — dropping from both simultaneously is redundant, since that state is already reachable via either single-skip path in a later step.
- **"Edit Distance and LCS are unrelated problems that both happen to be 'hard DP.'"** They share the exact same table shape and fill order — treating them as unrelated means re-deriving the wheel for every new two-sequence problem instead of recognizing a shared foundation.

## Key Takeaways

1. LCS replaces an `O(2^n * 2^m)` brute-force subsequence enumeration with an `O(n * m)` table, because only `(n+1) * (m+1)` distinct prefix-pair subproblems actually exist.
2. `dp[i][j]` = LCS length of the first `i` characters of A and the first `j` characters of B; row/column 0 is the empty-prefix base case (always 0).
3. The recurrence is exactly two cases: match => extend the diagonal (`+1`); mismatch => `max` of dropping a character from either string.
4. Subsequence allows gaps; substring does not — confusing the two leads to the wrong recurrence (reset-to-0 on mismatch is a *substring* rule, not LCS's).
5. Reconstructing the actual subsequence (not just its length) requires the full table and a backward walk retracing which branch produced each cell.
6. Space can be optimized to `O(min(n, m))` if only the length is needed — but that optimization is incompatible with reconstruction.
7. Edit Distance, Shortest Common Supersequence, and Delete Operation for Two Strings are the same table shape with a different base case, cell formula, or post-processing step — not unrelated problems.
8. Off-by-one between table indices (`i`, `j`) and sequence indices (`i-1`, `j-1`) is the most common implementation bug.
9. Don't confuse this with Longest Increasing Subsequence (one sequence, order relation, 1D) or Palindromic Subsequence (one sequence, interval-indexed 2D) — related-looking, structurally different.
10. Real systems use this directly: `diff`/version-control merge logic, DNA/protein sequence alignment (Needleman-Wunsch/Smith-Waterman), and similarity/plagiarism detection.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — Section 15.4, "Longest common subsequence," the standard formal treatment of the exact recurrence used in this module.
- *The Algorithm Design Manual* — Steven Skiena — covers edit distance and sequence alignment as a worked "war story," with practical framing beyond the textbook recurrence.
- *Bioinformatics Algorithms: An Active Learning Approach* — Compeau & Pevzner — covers global/local sequence alignment (Needleman-Wunsch, Smith-Waterman), the direct production application of this table shape in genomics.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes worked dynamic programming problems in the same family (string alignment/edit-distance style), with C++-specific implementation notes.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including LCS and edit-distance style dynamic programming implementations, useful for seeing varied implementation styles.
- Biopython (`Bio.Align` module) — an open-source bioinformatics library implementing pairwise sequence alignment (Needleman-Wunsch/Smith-Waterman) via the same dynamic programming table shape, applied to real biological sequence data.
- Git's own diff machinery (and GNU diffutils) — built around the Myers diff algorithm, a close relative of the LCS/edit-script family, used to compute the "changed lines" view this module's real-life analogy opened with.

**Official Documentation**
- LeetCode — Longest Common Subsequence (problem 1143).
- LeetCode — Edit Distance (problem 72).
- LeetCode — Shortest Common Supersequence (problem 1092).
- LeetCode — Delete Operation for Two Strings (problem 583).

**Blog Articles**
- GeeksforGeeks — "Longest Common Subsequence" dynamic programming explainer — a widely used walkthrough of the table-based recurrence and its variations.
- NeetCode — Dynamic Programming pattern videos/playlist covering LCS and Edit Distance with visual table-fill walkthroughs.
- Educative.io — "Grokking Dynamic Programming Patterns for Coding Interviews," the two-sequence DP chapter — one of the most widely referenced pattern-based framings of this exact technique.
