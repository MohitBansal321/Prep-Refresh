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

Compare two sequences by building a 2D table where each cell holds the best answer for a *pair of prefixes* (the first `i` elements of one sequence, the first `j` of the other), so that overlapping subproblems are computed once and reused instead of recomputed exponentially many times.

## Real Life Analogy

Think about how **`diff` and version control** (Git, in particular) show you what changed between two revisions of a file. When you run `git diff` on an old and a new version of a source file, Git does not treat the file as two unrelated blobs of text and give up — it finds the **lines that both versions share, in the same relative order**, and shows you everything else as an addition or a deletion around that shared skeleton. If you renamed a variable on line 12 but left lines 1-11 and 13-40 untouched, `diff` shows one changed line, not forty.

That "shared skeleton, in the same relative order" is exactly a **longest common subsequence**. The shared lines do not need to be *contiguous* in either file — you could have deleted ten lines in the middle of the old version and the shared lines on either side of that gap still count as part of the same subsequence, because subsequence only requires preserving relative order, not adjacency. Two engineers merging their branches are implicitly relying on this: Git's three-way merge algorithm finds the common ancestor lines between two diverging branches to figure out what each side *actually* changed, so it can combine both sets of changes without duplicating the parts neither side touched.

LCS is the general algorithmic idea behind that comparison: given two sequences, find the longest run of elements that appears in both, in the same order, allowing gaps. Everything else in this module is about how to compute that efficiently instead of brute-forcing it.

## Problem

### What engineering problem exists?

A large family of problems boils down to: **given two sequences (strings, arrays, lists of tokens), how much do they have in common, or what does it cost to turn one into the other?**

- **Version control / diff tools** need to find the longest shared skeleton between two file revisions to display (and merge) only the actual changes.
- **Spell-checkers and fuzzy search** need to know how many edits (inserts, deletes, substitutions) separate a typed word from a dictionary word, to rank "did you mean...?" suggestions.
- **DNA/protein sequence alignment** in bioinformatics needs to find how much of two genetic sequences match, in order, to measure evolutionary similarity between species or identify mutations.
- **Plagiarism and code-similarity detection** needs to measure how much of one document's token sequence reappears, in order, inside another — even if the plagiarist reordered paragraphs or inserted filler text between copied sentences.

> **Term: Subsequence.** A sequence derived from another by deleting zero or more elements *without changing the relative order* of the remaining elements. Crucially, a subsequence's elements do **not** need to be contiguous in the original sequence — `"ace"` is a subsequence of `"abcde"` (delete `b` and `d`), even though `"ace"` never appears as a contiguous block. This is the single most important distinction in this whole module: **subsequence is not substring.**

The naive way to answer "how much do these two sequences share?" is to enumerate every possible subsequence of the first string, every possible subsequence of the second string, and check which shared subsequences are longest. A string of length `n` has `2^n` possible subsequences (each character is either included or excluded, independently), so comparing every subsequence of a length-`n` string against every subsequence of a length-`m` string costs on the order of `2^n * 2^m` comparisons — doubly exponential, and unusable past strings of about 20-25 characters each.

### Why is this problem difficult?

- **The search space is exponential in both inputs.** Unlike a single-sequence problem (where the space of subsequences is merely `2^n`), LCS has to reason about two sequences simultaneously, so naive enumeration multiplies the two exponential spaces together.
- **Subsequence, not substring, is easy to get backward.** Many engineers' first instinct is to slide a window and look for the longest *contiguous* match (that is the Longest Common **Substring** problem — a real but different problem, usually solved with a different recurrence). LCS explicitly allows gaps, and that single relaxation is what makes it solvable in polynomial time at all — but it is also what makes the recurrence non-obvious the first time you see it: you cannot just "walk both strings together" the way you would for substring matching, because a mismatch does not mean failure, it means *branching* into two possible ways forward (skip a character from either string).
- **The two sequences advance independently.** A single-sequence DP (like Longest Increasing Subsequence) has one index to reason about. LCS has two independent indices into two independent sequences, and the relationship between them (do these two positions represent characters that "line up" in the final shared subsequence, or not) is what the whole recurrence has to capture.

### What happens if we ignore it?

- **Exponential blowup on any real input.** A brute-force LCS over two 30-character strings is already far beyond what any computer can finish in a reasonable time; real diffs, DNA sequences, and documents are thousands to millions of characters long.
- **Wrong problem solved.** Sliding-window substring matching where LCS's subsequence semantics were actually needed silently produces a shorter (or entirely wrong) answer whenever the true shared content has any gap in it — which is the common case, not the exception, in real diffs and edits.
- **No path to related problems.** Edit Distance, Shortest Common Supersequence, and dozens of other two-sequence problems are direct variations on the same `dp[i][j]` table. Without understanding the LCS table shape, each of those looks like an unrelated, from-scratch problem instead of "the same table, different cell meaning."

## Why Not Other Approaches?

**"Brute force: generate every subsequence of both strings and compare."**
Generating all `2^n` subsequences of the first string and all `2^m` subsequences of the second, then checking which shared subsequences are longest, costs `O(2^n * 2^m)` time (and at least that much space, if you materialize the subsequences instead of merely counting them). For strings of length 20 each, that is roughly `2^40` ~ one trillion combinations — already impractical, and real strings are far longer than 20 characters. This approach is correct but the complexity alone rules it out for any input size that matters.

**"Look for the longest common *substring* instead (sliding window / contiguous match)."**
This solves a genuinely different problem. Longest Common Substring requires the shared content to be *contiguous* in both strings, which is a strictly stronger (and often much shorter) requirement than "same relative order, gaps allowed." For `"abcde"` and `"aecd b"`-style inputs where the shared characters are scattered with insertions between them, a substring search will report something far shorter than the true LCS, or miss the relationship entirely. If the problem statement says "subsequence" (as diff tools, edit distance, and DNA alignment all effectively do), a substring approach answers the wrong question no matter how efficiently it runs.

**"Greedily match characters left to right, advancing whichever string 'needs' it."**
There is no local rule that is always safe. Given `"abc"` and `"bac"`, greedily matching `a` (from position 0 in both) commits you to comparing `"bc"` against `"ac"`, missing the true best alignment. Unlike Two Pointers (where a provable monotonic elimination rule exists), there is no such rule here in general — whether skipping a character from the first string or the second string is "correct" depends on what the *rest* of both strings look like, which a greedy left-to-right pass cannot see in advance. This is precisely why the problem needs a table that considers both options and keeps whichever turns out better, rather than committing early.

**"Recursion without memoization (plain divide and conquer on prefixes)."**
The recursive relationship (match => recurse on both prefixes shortened by one; mismatch => recurse on two smaller subproblems and take the best) is correct, but the *same* `(i, j)` prefix pair gets recomputed repeatedly through different recursive paths — the classic sign of overlapping subproblems. Without memoization this degenerates back to exponential time, even though the *recursion tree* itself is finite and much smaller than the brute-force subsequence enumeration above. This is the closest "almost right" approach, and it is exactly what motivates converting the recursion into a bottom-up table.

**Tradeoff summary:** brute-force subsequence enumeration and un-memoized recursion are both correct but exponential because they solve overlapping subproblems from scratch every time; sliding-window substring matching and greedy left-to-right matching are efficient but solve the *wrong problem* because they discard the "gaps allowed, no locally-safe choice" nature of subsequences. The table-based DP is the only approach that is both correct for the actual subsequence semantics *and* polynomial, because it computes each `(i, j)` prefix-pair subproblem exactly once and reuses it everywhere it is needed.

## Solution

The insight that turns an exponential search into a polynomial one is that the answer for two full sequences can be built entirely out of answers to smaller **prefix pairs** of those same two sequences — and there are only `(n+1) * (m+1)` distinct prefix pairs total, not `2^n * 2^m` subsequence pairs.

Define `dp[i][j]` to mean: **the length of the longest common subsequence between the first `i` characters of sequence A and the first `j` characters of sequence B.** Every other quantity in this module (edit distance, supersequence length, deletion counts) is a small variation on filling this exact same table.

The recurrence has exactly two cases, decided by comparing the *next* unconsidered character from each sequence (`A[i-1]` and `B[j-1]`, using 0-indexed sequences against a 1-indexed table — more on why in Common Mistakes):

- **The characters match.** If `A[i-1] == B[j-1]`, that character can always be included in the LCS of these two prefixes, extending whatever the best answer was for the two prefixes *one character shorter on both sides* — the cell diagonally up-and-left in the table. There is no need to consider any alternative here: including a matching character can never make the answer worse, so the recurrence commits to it unconditionally.
- **The characters do not match.** If `A[i-1] != B[j-1]`, then this pair of characters cannot both be part of the LCS at this position, so the LCS of these two prefixes must come from either dropping the last character of A (falling back to `dp[i-1][j]`) or dropping the last character of B (falling back to `dp[i][j-1]`). The recurrence takes whichever of those two possibilities is larger, because both are valid ways of shrinking the problem and only the better one can be part of an optimal answer.

Read together: **a match always extends the diagonal; a mismatch always takes the best of "give up on this character of A" or "give up on this character of B."** That single two-way branch, applied to every cell, is the entire algorithm — no code needed yet to see why it works.

## Architecture

The "participants" in this pattern are not objects or classes — they are the roles played by the table's structure and fill order:

1. **The two input sequences, A and B.** Read-only throughout; every cell of the table is defined purely in terms of characters compared from these two sequences and previously-computed cells. Nothing about them is ever mutated.

2. **The `dp` table itself, sized `(n+1) x (m+1)`.** This is the memory of every prefix-pair subproblem already solved. Its dimensions are one larger than the sequence lengths specifically so that row 0 and column 0 can represent the **empty-prefix base case** without any special-casing inside the main loop (see Execution Flow below).

3. **The match/no-match branching recurrence.** The single decision rule applied identically to every cell: extend the diagonal on a match, take the max of the up/left neighbors on a mismatch. This rule is the only "logic" in the entire algorithm; everything else is bookkeeping around applying it `n * m` times.

4. **The fill order.** Because `dp[i][j]` depends only on `dp[i-1][j-1]`, `dp[i-1][j]`, and `dp[i][j-1]` — cells strictly above, strictly to the left, or diagonally up-left — the table can be filled row by row (left to right within each row) or column by column (top to bottom within each column); either order guarantees every dependency is already computed by the time a cell is reached. This is what makes the table "fillable" at all: the dependency graph between cells has no cycles and always points toward the top-left corner.

5. **The optional walk-back path (reconstruction).** If the actual shared subsequence (not just its length) is needed, a second phase walks from `dp[n][m]` back toward `dp[0][0]`, retracing which branch (diagonal on a match, or whichever neighbor was larger on a mismatch) produced each cell's value. This phase requires the *entire* table to still exist in memory — a direct consequence explored further in Disadvantages and Complexity.

Responsibilities in one line each:
- **A, B:** the immutable source of every character comparison.
- **`dp` table:** memoized answer to every prefix-pair subproblem, computed once.
- **Recurrence:** the one rule (match => diagonal+1, mismatch => max of up/left) applied to every cell.
- **Fill order:** guarantees every cell's dependencies exist before the cell itself is computed.
- **Walk-back path:** reconstructs the actual answer (not just its length) by retracing the recurrence's decisions in reverse.

## Execution Flow

1. **Base case.** Initialize `dp[i][0] = 0` for every `i` and `dp[0][j] = 0` for every `j` — the LCS of *any* prefix with an empty sequence is always the empty sequence (length 0). Sizing the table `(n+1) x (m+1)` and dedicating row/column 0 to this case means the main loop never has to special-case "what if one string is empty."
2. **Fill order.** Iterate `i` from 1 to `n` (row by row); within each row, iterate `j` from 1 to `m` (column by column). Column-by-column-outer, row-by-row-inner works equally well — the only requirement is that `dp[i-1][j-1]`, `dp[i-1][j]`, and `dp[i][j-1]` are already filled before `dp[i][j]` is computed, and both traversal orders satisfy that.
3. **Apply the recurrence at each cell.** Compare `A[i-1]` to `B[j-1]`. On a match, set `dp[i][j] = dp[i-1][j-1] + 1`. On a mismatch, set `dp[i][j] = max(dp[i-1][j], dp[i][j-1])`.
4. **Read the answer.** Once every cell is filled, `dp[n][m]` holds the LCS length for the *entire* two sequences — no further computation needed if only the length is required.
5. **Reconstruct the actual subsequence (if needed).** Starting at `(i, j) = (n, m)`, repeatedly check: if `A[i-1] == B[j-1]`, this character is part of the LCS — record it, then move diagonally to `(i-1, j-1)`. Otherwise, move to whichever of `(i-1, j)` or `(i, j-1)` has the larger `dp` value (retracing the exact branch the forward pass took). Stop when `i == 0` or `j == 0`. The characters were recorded from the end of the subsequence toward the start, so reverse the collected list before returning it.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full flowchart distinguishing LCS from Longest Increasing Subsequence (one sequence, order relation) and Palindromic Subsequence (one sequence, interval-based) based on how many sequences the problem compares and what relationship is being asked for.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the 2D table fill loop — the match-extends-the-diagonal branch versus the no-match-best-of-either-skip branch — including how Edit Distance reuses the identical loop structure with a different cell formula.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a cell-by-cell trace of the `dp` table being filled on a small concrete example, plus the walk-back path that reconstructs the actual LCS string from the finished table.

## Implementation

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the *shape* of the pattern clearly (the table, the recurrence, the walk-back) before looking at the worked, problem-specific solutions in [problems/](problems/), each of which reuses this exact table shape with small, deliberate variations.

It provides two function templates:

- `longestCommonSubsequence` — builds the full `(n+1) x (m+1)` table and returns only `dp[n][m]`, the LCS **length**. Templated so it works over `std::string`, `std::vector<int>`, or any other type supporting `.size()` and `operator[]` with comparable elements.
- `reconstructLCS` — rebuilds the same table, then walks it **backwards** from `(n, m)` to `(0, 0)` to recover the actual shared subsequence, not just its length.

Both functions build the full 2D table (rather than a space-optimized rolling array) deliberately: `reconstructLCS` cannot walk backward through rows that a space-optimized version would have already overwritten, and keeping both functions structurally identical (aside from the walk-back) makes the relationship between "just the length" and "the actual answer" easy to see side by side.

## Code Walkthrough

**`longestCommonSubsequence(a, b)`** (in [code.cpp](code.cpp)). Allocates a `(n+1) x (m+1)` table of `int`, zero-initialized (which already satisfies the base case from Execution Flow step 1 for free, since row/column 0 should be all zeros anyway). The nested loop runs `i` from 1 to `n` and, within each row, `j` from 1 to `m`, applying the match/mismatch recurrence exactly as described in Solution. Returns `dp[n][m]`. This function exists to demonstrate the recurrence in its purest form, decoupled from any specific problem's framing.

**`reconstructLCS(a, b)`** (in [code.cpp](code.cpp)). Rebuilds the identical table (the length-only computation is cheap enough, and keeping the two functions independent keeps each one simple to read in isolation), then walks backward from `(i, j) = (n, m)`. On each step: if `a[i-1] == b[j-1]`, that character is unconditionally part of the LCS (this is exactly the case that would have written `dp[i][j] = dp[i-1][j-1] + 1` during the forward pass) — it is appended to a result buffer and both indices step diagonally. Otherwise, the function moves toward whichever neighbor (`dp[i-1][j]` or `dp[i][j-1]`) is larger, retracing the branch the forward recurrence actually took at that cell. Because the walk starts at the end of the sequence and moves toward the start, characters are collected in reverse order and the result is reversed once before returning. This function exists to show that "the actual answer," not just its length, is recoverable from the same table with no additional bookkeeping beyond the table itself.

**`main()`** (in [code.cpp](code.cpp)). Exercises both functions against hand-checkable inputs — including the classic `"ABCBDAB"` vs `"BDCABA"` textbook example (LCS length 4, with two equally valid length-4 answers, `"BCBA"` or `"BDAB"`, both accepted by the test) — and prints `[PASS]`/`[FAIL]` for each assertion, including a consistency check that `reconstructLCS`'s output length always matches `longestCommonSubsequence`'s returned length.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem, implemented independently (not calling into `code.cpp`) so every file compiles and runs on its own, with problem-specific comments tying every recurrence back to the general table shape established in this README. See [problems/README.md](problems/README.md) for the index and the "why these four" rationale. Briefly: `01` is the pure LCS length recurrence; `02` (Edit Distance) reuses the identical table and fill order with a different per-cell formula (cost instead of shared length); `03` (Shortest Common Supersequence) builds the LCS table and then walks it backward while emitting *every* character encountered (not just matches), producing an actual merged string; `04` (Delete Operation for Two Strings) uses the plain LCS length as a one-line subroutine inside a small arithmetic formula.

## Advantages

- **Polynomial time where brute force is doubly exponential.** Turns `O(2^n * 2^m)` subsequence enumeration into `O(n * m)` — the difference between "cannot finish" and "instant" for any string longer than a few dozen characters.
- **One table shape, many problems.** Edit Distance, Shortest Common Supersequence, Longest Common Substring's cousin problems, and Delete Operation for Two Strings all reuse the identical `dp[i][j]` grid and fill order; learning the table once pays off across a whole family of interview and production problems.
- **Both the length AND the actual answer are recoverable.** Unlike some DP patterns where reconstructing the actual solution requires substantially different machinery, LCS reconstruction is a direct backward walk through the same table already built for the length.
- **Correctness is easy to argue.** Every cell's value is justified by exactly two cases (match/no-match), each with a one-sentence proof — much easier to reason about (and explain in a review or interview) than a heuristic.
- **Naturally composable.** The table-building step is identical across every LCS-derived problem; only the base case, the per-cell formula, or the post-processing arithmetic changes.

## Disadvantages

- **`O(n * m)` time and space can be large for long strings.** Comparing two 10,000-character strings needs a table with 100 million cells — at 4 bytes per `int`, that is 400 MB, which can become a real constraint for large documents, long DNA sequences, or big diffs.
- **Reconstructing the actual subsequence (not just its length) requires keeping the full table**, or explicit extra bookkeeping (a separate "which direction did this cell come from" table, or Hirschberg's divide-and-conquer technique) if the full table's memory cost is unacceptable. The straightforward space optimization for the length (described in Complexity below) is *not* available for free once reconstruction is needed.
- **Quadratic growth means doubling either input roughly doubles total work along that axis.** Unlike an `O(n)` or `O(n log n)` pattern, LCS's cost grows with the *product* of both lengths, so it scales worse than most single-sequence patterns as inputs grow.
- **Off-by-one bookkeeping between the table's indices and the sequences' indices is a recurring source of bugs** (see Common Mistakes) — every access into `a` or `b` inside the loop must subtract 1 from the table index, and it is easy to forget in one branch but not another.

## Tradeoffs

**What we gain versus brute force:** an exponential search (`O(2^n * 2^m)`) collapses to a polynomial one (`O(n * m)`), by recognizing that only `(n+1) * (m+1)` distinct prefix-pair subproblems exist and computing each exactly once instead of re-deriving it inside every recursive branch that touches it.

**What we gain versus un-memoized recursion:** the same recurrence, but without the repeated recomputation of identical `(i, j)` subproblems — memoization (here, in its bottom-up table form) is the entire difference between exponential and polynomial time for an *identical* recursive relationship.

**What we lose versus a 1D DP pattern:** space and time both scale with the *product* of two input sizes rather than a single input size, so LCS is inherently more expensive than, say, Longest Increasing Subsequence's `O(n)` space for a similarly-sized single input.

**What we lose when we need reconstruction:** the ability to collapse the table to `O(min(n, m))` space — walking backward through a table requires the rows/columns the walk depends on to still exist, so reconstruction and space-optimization are in direct tension (see Complexity).

## Complexity

**Time:** `O(n * m)` — one constant-time decision (match or mismatch) per cell of an `(n+1) x (m+1)` table, so total work is proportional to the product of the two sequence lengths. This holds in the best, worst, and average case alike, because the loop bound is structural (every cell is visited exactly once), not data-dependent.

**Space:** `O(n * m)` for the full table, as used throughout this module's `code.cpp` and `problems/` (needed for reconstruction in `reconstructLCS`, `problems/03`, and generally whenever the actual answer, not just its length, is required).

**Space optimization (length only):** if only the LCS **length** is needed — never the reconstructed subsequence — the table can be collapsed to two 1D rows (or even one row updated carefully) of size `O(min(n, m))`, because `dp[i][j]` only ever depends on the *current* and *immediately previous* row. This is a genuine, common optimization; it is not used in this module's code because every file here either demonstrates reconstruction directly (`code.cpp`, `problems/03`) or intentionally shows the "default," most-general form of the table before specializing. In an interview, mentioning this optimization when only a length is asked for is a meaningful signal that you understand *why* the full table was needed in the first place.

**Comparison to the brute force it replaces:**

| Problem shape | Brute force | 2D Table DP |
|---|---|---|
| LCS length | `O(2^n * 2^m)` time, exponential space to enumerate subsequences | `O(n * m)` time, `O(n * m)` space (or `O(min(n,m))` if length-only) |
| LCS reconstruction (actual subsequence) | `O(2^n * 2^m)` time, same as above | `O(n * m)` time, `O(n * m)` space (full table required) |
| Edit Distance | Exponential (recursive insert/delete/replace tree, unmemoized) | `O(n * m)` time, `O(n * m)` space |

## Common Mistakes

- **Off-by-one between the table's indices and the sequences' indices.** The table is `(n+1) x (m+1)` so that row/column 0 can represent the empty-prefix base case, but the sequences themselves are still 0-indexed and only `n`/`m` characters long. This means every character access inside the loop must be `a[i - 1]` and `b[j - 1]`, **not** `a[i]`/`b[j]` — forgetting the `-1` in one branch (commonly the mismatch branch, since the match branch is checked first and gets more attention) silently shifts every comparison by one position and produces subtly wrong answers rather than a crash.
- **Confusing "characters match" with "substring match."** LCS is about *subsequence* — order preserved, gaps allowed — not contiguous substrings. A recurrence that only extends a match when the *previous* characters also matched (as a substring/longest-common-substring recurrence would) answers a different, usually much more restrictive, question. If a problem statement says "subsequence," the mismatch branch must fall back to `max(dp[i-1][j], dp[i][j-1])`, not reset to 0 the way Longest Common Substring's mismatch case does.
- **Forgetting this is a foundation for Edit Distance (and other variations), not a standalone trick.** Treating LCS, Edit Distance, and Shortest Common Supersequence as three unrelated problems to memorize independently is far more work than recognizing they share one table shape with three different cell formulas and/or post-processing steps — see [problems/README.md](problems/README.md) for exactly how each variation departs from the base LCS recurrence.
- **Trying to reconstruct the subsequence from a space-optimized (rolled) table.** If the table has been collapsed to `O(min(n,m))` rows for the length-only optimization, the earlier rows needed for a backward walk no longer exist — reconstruction and the rolling-array space optimization are mutually exclusive without extra bookkeeping (a separate direction-tracking structure, or Hirschberg's technique).
- **Assuming the LCS is unique.** Multiple distinct subsequences can share the same maximum length (see the `"ABCBDAB"`/`"BDCABA"` example in `code.cpp`, which has two valid length-4 answers). Code that asserts a single "the" LCS string, rather than "a" LCS string of the correct length, can fail tests that (correctly) accept any valid answer.

## When To Use

- **Comparing two sequences (strings, token lists, gene sequences) for a longest shared subsequence**, where matches do not need to be contiguous.
- **Computing an edit-distance-style transformation cost** between two sequences (insertions, deletions, substitutions) — Edit Distance reuses this exact table shape.
- **Building a shortest supersequence, or counting deletions/insertions needed to reconcile two sequences** — both are direct arithmetic or reconstruction variations on the LCS table (see `problems/03` and `problems/04`).
- **Diffing, merging, or reconciling two ordered data sources** (file revisions, sorted logs with occasional gaps, two versions of a config) where "what's shared, in order" is the question.
- **Any problem whose state naturally indexes by two independent prefix lengths** — if you catch yourself trying to define `dp[i][j]` over two different sequences, this is very likely the pattern you want.

## When NOT To Use

- **Comparing ONE sequence to itself for an internal property** — e.g. checking whether it is (or contains) a palindrome. That is the **Palindromic Subsequence/Substring** pattern (`../palindromic-subsequence/`), whose `dp[i][j]` indexes an *interval* `[i, j]` inside a single sequence, not two independent prefixes from two different sequences. Reaching for LCS's two-sequence framing here produces a table that does not even type-check conceptually — there is only one sequence to index into.
- **Finding the longest *contiguous* shared run (substring, not subsequence).** Longest Common Substring uses a visually similar table but a different recurrence (mismatches reset the running length to 0 instead of falling back to `max` of neighbors) — using LCS's mismatch rule here silently answers the wrong (too permissive) question.
- **The problem only involves ONE sequence, and the relationship is an ORDER property (increasing/decreasing), not equality.** That is Longest Increasing Subsequence's territory (`../longest-increasing-subsequence/`) — a 1D table indexed by a single position, not a 2D table over two sequences.
- **The inputs are far too large for `O(n * m)` to finish** (e.g. two multi-million-character genomes compared naively) — production bioinformatics tools use banded alignment, Hirschberg's linear-space technique, or approximate/heuristic alignment (BLAST-style seed-and-extend) rather than the textbook full table.
- **You only need to know whether one sequence contains the other as a subsequence at all (a yes/no question)**, not the longest shared length — a simple linear two-pointer scan answers that in `O(n + m)` without needing a table at all.

## Real Interview/Production Examples

LCS and its close variants (Edit Distance especially) are among the most frequently asked "hard" DP problems at major tech companies (Google, Amazon, Meta, and most FAANG-adjacent interview loops), precisely because getting the recurrence right under time pressure — and correctly distinguishing it from Longest Common Substring — genuinely separates candidates who understand the *shape* of two-sequence DP from those pattern-matching on memorized code.

Beyond interviews, the same table shows up directly in production systems:

- **`diff` tools and version control merge logic.** Git, and diff utilities generally, are built around finding the longest common subsequence (or a closely related edit-script algorithm, such as the Myers diff algorithm) between two file revisions, so that only genuinely changed lines are shown or merged — exactly the analogy this module opened with.
- **DNA/protein sequence alignment in bioinformatics.** Global sequence alignment (the Needleman-Wunsch algorithm) and local alignment (Smith-Waterman) are LCS/Edit-Distance-style dynamic programs over two biological sequences, used to measure evolutionary similarity, find conserved regions, and detect mutations.
- **Plagiarism and code-similarity detection.** Tools that compare submitted assignments or source files for copied content often tokenize both documents and compute a longest-common-subsequence-style similarity score, because it tolerates reordered whitespace, renamed variables (after normalization), and inserted filler between copied sections — something a naive substring search would miss entirely.
- **Spell-checkers and fuzzy autocomplete.** Edit distance between a typed query and dictionary entries ranks "did you mean...?" suggestions by how many operations separate the two strings.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **A lightweight "what changed" diff view for JSON/config files in an admin tool** — tokenize both versions (by line or by key), run LCS to find the shared skeleton, and render only the additions/removals around it, rather than diffing raw unstructured text.
2. **Deduplication-aware log reconciliation**, where two services log semantically similar (but not byte-identical) event sequences and you need to measure how much of one log's event order is reproduced in the other, tolerating occasional missing or reordered-adjacent events.
3. **Fuzzy search/autocomplete ranking** for an internal search tool — rank candidate matches by edit distance (or LCS length as a similarity score) against the user's query instead of requiring an exact substring match.
4. **A basic plagiarism/duplicate-submission checker** for a coding-exercise platform — tokenize submissions (by identifier/keyword, after stripping whitespace and comments) and flag pairs whose LCS length, relative to submission length, exceeds a suspicious threshold.
5. **API schema migration tooling** that compares an old and new API request/response shape (as an ordered list of fields) to compute the minimum insert/rename/remove operations needed to migrate client code — an Edit-Distance-style application of the same table.

## Similar Patterns

- **Longest Increasing Subsequence** ([../longest-increasing-subsequence/](../longest-increasing-subsequence/)): also finds a "longest subsequence," but over **one** sequence, constrained by an **order relation** (each next element must be larger than the last), not by matching against a second sequence. Its state is 1D (`dp[i]` = best subsequence ending at index `i`), versus LCS's 2D state over two independent prefix lengths. The two patterns share the word "subsequence" in their names and nothing else about their state shape.
- **Palindromic Subsequence** ([../palindromic-subsequence/](../palindromic-subsequence/)): also uses a 2D `dp[i][j]` table, which makes it the pattern most easily confused with LCS at a glance — but `i` and `j` here index the two **ends of an interval inside a single sequence** (`dp[i][j]` = the answer for the substring/subrange from `i` to `j`), not two independent prefixes of two different sequences. The fill order differs accordingly: Palindromic Subsequence fills by increasing *interval length*, while LCS fills row-by-row (or column-by-column) over two separate axes.

| Pattern | How many sequences? | State shape | Fill order | Primary question answered |
|---|---|---|---|---|
| Longest Common Subsequence | Two | `dp[i][j]` = best answer for prefix `i` of A, prefix `j` of B | Row-by-row or column-by-column (either works) | "What's the longest shared order-preserving run between A and B?" |
| Longest Increasing Subsequence | One | `dp[i]` = best subsequence ending at index `i` | Left to right, one pass | "What's the longest run obeying an order relation within one sequence?" |
| Palindromic Subsequence | One (indexed by an interval) | `dp[i][j]` = answer for the interval `[i, j]` | By increasing interval length | "What's the longest/best palindromic structure within one sequence?" |

## Interview Discussion

Experienced engineers rarely spend interview time on "can you write the double loop" — that is mechanical once the recurrence is understood. What they actually probe is whether you can **state the recurrence's two cases and justify each one**, and whether you can recognize when a "new" problem is actually this same table wearing a different cell formula.

Common follow-up questions:
- *"Can you also return the actual longest common subsequence, not just its length?"* — expects recognizing that the full table (not a space-optimized rolled array) must be kept, and describing (or implementing) the backward walk.
- *"How would you reduce the space complexity if you only need the length?"* — expects naming the `O(min(n, m))` rolling-row optimization and correctly identifying *why* it is unavailable once reconstruction is required.
- *"How is this related to Edit Distance?"* — expects recognizing the identical table shape and fill order, differing only in the base case (0 vs. `i`/`j`) and the per-cell formula (`max` of two skips vs. `1 + min` of three operations).
- *"What if the strings are very large (millions of characters), and `O(n*m)` memory is too much?"* — expects mentioning Hirschberg's algorithm (linear-space LCS via divide and conquer) or banded/heuristic alignment (as used in real bioinformatics tools), even without full implementation detail.
- *"Is the LCS always unique?"* — expects a clear "no," with a concrete example of two equally valid maximum-length answers.

Common misconceptions:
- "LCS is about finding the longest common *substring*." It is not — LCS explicitly allows gaps; substring problems use a different recurrence entirely (mismatches reset to 0 rather than fall back to a `max`).
- "The recurrence needs to consider dropping characters from *both* strings at once on a mismatch." It only ever needs the better of the two single-character skips (`dp[i-1][j]` or `dp[i][j-1]`) — dropping from both simultaneously is redundant, since that state is already reachable (and already accounted for) via either single-skip path in a later step.
- "You can always reduce LCS's space to O(min(n,m))." Only true when you need the length alone; the moment reconstruction is required, the full table (or an equivalent bookkeeping structure) is necessary.
- "Edit Distance and LCS are unrelated problems that both happen to be 'hard DP.'" They share the exact same table shape and fill order — treating them as unrelated means re-deriving the wheel for every new two-sequence problem instead of recognizing a shared foundation.

## Summary

- LCS defines `dp[i][j]` as the best (shared subsequence) length over the first `i` characters of one string and the first `j` characters of another, filling a table of `(n+1) * (m+1)` cells instead of enumerating `2^n * 2^m` subsequence pairs.
- The recurrence has exactly two cases: a match unconditionally extends the diagonal (`dp[i-1][j-1] + 1`); a mismatch takes the better of dropping a character from either string (`max(dp[i-1][j], dp[i][j-1])`).
- The table can be filled row-by-row or column-by-column — either order is valid because every cell's dependencies (up, left, diagonal) are always already computed.
- Subsequence means order-preserved-with-gaps, which is a fundamentally different (and easier) problem than Longest Common Substring's contiguous-match requirement.
- The full table (not a space-optimized rolling array) is required whenever the actual shared subsequence, not just its length, must be reconstructed.
- Edit Distance, Shortest Common Supersequence, and Delete Operation for Two Strings all reuse this exact table shape and fill order, differing only in the base case, the per-cell formula, or a small post-processing step.
- Real production uses include diff/version-control tooling, DNA/protein sequence alignment, and plagiarism/similarity detection — not just interview questions.
- Closely related but distinct: Longest Increasing Subsequence (one sequence, order relation, 1D state) and Palindromic Subsequence (one sequence, interval-indexed 2D state).

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
