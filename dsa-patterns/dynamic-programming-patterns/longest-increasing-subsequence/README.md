# Longest Increasing Subsequence (LIS)


> **In one line:** `dp[i]` = length of the longest increasing subsequence *ending exactly at* `i` — look back at every earlier smaller element and extend its best.

```cpp
std::vector<int> dp(n, 1);   // every single element is a subsequence of length 1
int best = 1;

for (size_t i = 1; i < n; ++i) {
  for (size_t j = 0; j < i; ++j) {
    if (nums[j] < nums[i]) {
      dp[i] = std::max(dp[i], dp[j] + 1);   // place nums[i] after the subsequence ending at j
    }
  }
  best = std::max(best, dp[i]);
}
```

**O(n²)** for this version (shown here) · an O(n log n) patience-sorting variant lives alongside it in `code.cpp`. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Find the length (or the actual elements) of the longest subsequence of an array that is strictly increasing — not necessarily contiguous — using either an intuitive `O(n^2)` DP or a much sharper `O(n log n)` technique borrowed from a card-sorting trick called patience sorting.

## Real Life Analogy

Think about **patience solitaire**, the card game the `O(n log n)` technique is named after. You deal cards one at a time into piles, following one rule: a card can only be placed on top of a pile whose current top card is larger than it; if no such pile exists, start a new pile. After dealing every card, the *number of piles* equals the length of the longest increasing subsequence of the original deck order — a genuinely surprising fact the Solution section below proves out.

A simpler version: imagine building the tallest tower of moving boxes where each box must be strictly larger than the one above it. You don't need consecutive boxes from the pile — you can skip some — you just need each box you *do* use to be strictly larger than the last one placed. "What's the tallest tower you can build, in the original left-to-right pile order?" is exactly Longest Increasing Subsequence.

## Problem

Given a single sequence of numbers, find the length of the longest subsequence (elements needn't be adjacent, but relative order must be preserved) where each element is strictly larger than the one before it.

> **Term: Subsequence (again).** As in Longest Common Subsequence, order is preserved but gaps are allowed — `[2, 3, 7, 18]` is a valid increasing subsequence of `[10, 9, 2, 5, 3, 7, 101, 18]`, even scattered across non-adjacent positions.

The brute-force way is to enumerate every one of the `2^n` subsequences, check which are strictly increasing, and keep the longest.

Three things make this hard:

- **The search space is exponential**, just like Subsets — checking every one of `2^n` subsequences against "strictly increasing" is `O(2^n * n)`, unusable past a few dozen elements.
- **There's no simple greedy rule at the element level.** "Always extend with the next bigger number you see" sounds reasonable but doesn't hold in general: greedily grabbing the first bigger number can lock you into a worse overall subsequence than if you'd waited.
- **The `O(n log n)` optimization is genuinely non-obvious.** The DP formulation (`dp[i]` = best subsequence ending at `i`) is a reasonably natural first idea, but the leap to "maintain an array of smallest-possible tail values and binary search into it" needs a correctness argument that isn't visually obvious the first time — the single hardest part of this pattern to internalize.

Ignoring it costs real time: enumerating all subsequences of an array with even 30-40 elements is already impractical; on arrays with tens of thousands of elements or more, only the patience-sorting approach finishes in reasonable time (the `O(n^2)` DP, though correct, does not); and confusing this with the "longest increasing *run*" (contiguous) problem — a much simpler, unrelated `O(n)` linear scan — leads to solving, or being asked to solve, the wrong problem entirely.

## Solution

**Approach 1 — `O(n^2)` DP.** Define `dp[i]` = the length of the longest increasing subsequence ending *exactly* at index `i`. To compute it, look at every earlier index `j < i` where `nums[j] < nums[i]`; any such `j` means `nums[i]` could extend that subsequence, giving a candidate length `dp[j] + 1`. Take the best candidate over all valid `j` (or `1`, if none exists — `nums[i]` alone is always a subsequence of length 1). The final answer is `max(dp[0..n-1])`, **not** `dp[n-1]`, since the longest subsequence overall might end anywhere.

**Approach 2 — `O(n log n)` patience sorting.** Maintain an array `tails`, where `tails[k]` holds the *smallest possible tail value* among all increasing subsequences of length `k+1` found so far. For each new number, binary search `tails` for the first entry `>= number` (`std::lower_bound`): if found, replace that entry with `number` (a smaller tail for that length is always at least as good, leaving more room for future extension); if not found, `number` is larger than every current tail, so append it, extending the longest length found so far by one. The final LIS length is `tails.size()`.

The subtle part: **`tails` is not itself a valid subsequence** — it's a record of the *best possible tail value* per length, assembled from potentially many different underlying subsequences. That's exactly why reconstructing the actual LIS (not just its length) from `tails` needs extra parent-pointer bookkeeping (see Tradeoffs).

**Running it (Approach 1):** initialize `dp[i] = 1` for all `i`; for each `i` from 1 to `n-1`, for each `j` from `0` to `i-1`, if `nums[j] < nums[i]`, set `dp[i] = max(dp[i], dp[j]+1)`; the answer is `max(dp)`.

**Running it (Approach 2):** initialize an empty `tails`; for each number, binary search for the first entry `>= number` — if found at index `k`, set `tails[k] = number`; if not found, append. The answer is `tails.size()`.

## Architecture

1. **The input array**, read-only throughout.
2. **(Approach 1) The `dp` array**, indexed by position, holding "best subsequence length ending here."
3. **(Approach 2) The `tails` array**, holding "smallest tail value achievable for each subsequence length" — a fundamentally different kind of state than `dp`, indexed by *length*, not by *position*.
4. **The binary search step (Approach 2 only)**, which turns the `O(n)`-per-element scan of Approach 1 into an `O(log n)`-per-element lookup.

## Why Not Other Approaches?

**Brute force: enumerate every subsequence and check which are increasing.** Correct but `O(2^n * n)` — the same exponential wall every subsequence-enumeration problem hits.

**Greedily extend with the next larger element you see.** No provably-safe local rule exists here the way it does for, say, Merge Intervals' sort-then-sweep. A greedy choice that looks locally fine can foreclose a longer subsequence that only becomes visible later — there's no way to know, looking only at the current element, whether "take it" is globally correct without considering what comes after.

**Just scan for the longest contiguous increasing run.** A different, much easier problem (`O(n)`, no DP needed) — but not LIS. If the problem says "subsequence," gaps are allowed, and a contiguous-run scan reports something far shorter than the true answer whenever the true LIS has any gap in it (the common case, not the exception).

**Net:** brute-force enumeration is correct but exponential; a naive greedy has no safe local rule to fall back on; contiguous-run scanning answers the wrong question entirely. The DP formulation (`dp[i]` = best ending at `i`) is the first approach that is both correct and polynomial (`O(n^2)`), and the patience-sorting refinement pushes that down to `O(n log n)` by exploiting a structural fact about the DP's own state that isn't visible from the recurrence alone.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart distinguishing LIS (one sequence, order relation, gaps allowed) from Longest Common Subsequence (two sequences) and Kadane's Algorithm (one sequence, contiguous, sum-based).
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of the `O(n log n)` patience-sorting loop.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of the `tails` array being updated on the classic example `[10, 9, 2, 5, 3, 7, 101, 18]`.

## The Code

[code.cpp](code.cpp) provides both `lengthOfLIS_On2` (the `O(n^2)` DP) and `lengthOfLIS_NLogN` (the `O(n log n)` patience-sorting approach), deliberately implemented side by side so they can be directly compared — both return the same answer on every test input despite very different internal state.

- **`lengthOfLIS_On2(nums)`** — the straightforward two-nested-loop DP from Solution: `dp[i]` starts at 1 and is extended by checking every earlier smaller element. Returns the maximum value in `dp`, not the last one.
- **`lengthOfLIS_NLogN(nums)`** — maintains `tails` and uses `std::lower_bound` to find the insertion point for each new number in `O(log n)`. Returns `tails.size()`.

Each file in [problems/](problems/) is a complete, standalone solution to one named LeetCode problem building on LIS — see [problems/README.md](problems/README.md) for the full index. Briefly: `01` is the plain length; `02` counts how many distinct subsequences achieve that maximum length; `03` is a 2D variant requiring a sorting trick before LIS applies; `04` uses a different, non-numeric order relation.

## Tradeoffs

**What this pattern buys you**

- **Two complexity classes for the same problem** — the simpler `O(n^2)` DP when `n` is small, or the sharper `O(n log n)` approach when it isn't, a genuine algorithmic upgrade and not just a constant-factor tweak.
- **The DP state (`dp[i]`) is easy to explain and prove correct** — a natural first step before the harder optimization: exponential subsequence enumeration collapses to a polynomial nested loop by recognizing only `n` distinct "best ending here" subproblems exist.
- **The patience-sorting trick generalizes** to variants like Russian Doll Envelopes, where a sort-then-LIS combination handles two-dimensional ordering.

**What it costs you**

- **The `O(n log n)` `tails` array is not a valid subsequence itself** — reconstructing the actual LIS (not just its length) requires additional parent-pointer bookkeeping recorded alongside each update, not a direct read of `tails`. This is the direct cost of the `O(n^2)` -> `O(n log n)` upgrade: tracking the best tail value per achievable length, instead of the best value per array position, loses direct reconstructability.
- **The correctness of patience sorting is non-obvious** — unlike a straightforward greedy or two-pointer argument, it requires understanding *why* replacing a tail with a smaller value can never hurt (and can only help) future extensions.
- **Easy to confuse with the much simpler contiguous "longest increasing run" problem**, which needs no DP at all.

## Complexity

**Time:** `O(n^2)` for the DP approach; `O(n log n)` for patience sorting (`n` binary searches, each `O(log n)`).

**Space:** `O(n)` for either approach's main array (`dp` or `tails`).

| Approach | Time | Space |
|---|---|---|
| Brute force (enumerate all subsequences) | `O(2^n * n)` | Exponential |
| `O(n^2)` DP | `O(n^2)` | `O(n)` |
| `O(n log n)` patience sorting | `O(n log n)` | `O(n)` |

## Common Mistakes

- **Confusing "increasing subsequence" with "increasing subarray" (contiguous).** Different problems solved by completely different techniques — a subarray version is a simple `O(n)` linear scan with no DP needed at all.
- **Reading `tails` as an actual valid subsequence.** It records the best *tail value* per length, assembled from possibly many different underlying subsequences — it is not, in general, itself a real subsequence of the input.
- **Using the wrong binary search boundary** (`lower_bound` vs. `upper_bound`) depending on whether the problem wants a strictly increasing or a non-decreasing subsequence — this single detail flips between the two variants.
- **Returning `dp[n-1]` instead of `max(dp)`** in the `O(n^2)` approach — the longest subsequence overall does not necessarily end at the last array position.

## When To Use

- **Finding the longest run of elements (not necessarily contiguous) obeying an order relation** — increasing stock trends, improving priority sequences, and similar "each next pick must be strictly better" scenarios.
- **As a subroutine after sorting**, for two-dimensional ordering problems (Russian Doll Envelopes-style: sort by one dimension, then run LIS on the other).
- **Patience-sorting-based algorithms** in card-stacking and logistics-style problems, where the classical technique applies directly.

## When NOT To Use

- **You need a CONTIGUOUS run, not a subsequence** — that's a simpler `O(n)` linear scan, no DP required.
- **You're comparing TWO different sequences**, not looking for an order relation within one — that's Longest Common Subsequence ([../longest-common-subsequence/](../longest-common-subsequence/)).

## Where This Shows Up

- **Stock trend analysis** — identifying the longest run of strictly improving prices in a historical sequence (not necessarily on consecutive days).
- **Patience-sorting-based algorithms** used in some external-sorting and logistics/box-stacking optimization tools, applying the same "smallest tail per length" trick directly.
- **Scheduling non-conflicting, increasing-priority tasks** where task order must be preserved but not every task needs to be selected.
- **A version-monotonicity checker** — given a sequence of deployed version numbers with occasional rollbacks interspersed, find the longest run that was genuinely, strictly increasing.
- **A metrics dashboard "longest improving streak" widget** — the longest subsequence of strictly improving daily metric values, even with off days in between.
- **A Russian-Doll-style resource-nesting checker** — given container sizes (e.g. VM instance tiers across two dimensions), sort by one dimension and run LIS on the other to find the longest valid nesting chain.
- **A task-priority validator** — the longest valid subsequence of tasks whose priority strictly increases in scheduled order, flagging where the schedule violates that property.
- **A leaderboard "longest improving streak" feature** — for a single player's historical scores, the longest subsequence of strictly improving results.

## Similar Patterns

- **Longest Common Subsequence** ([../longest-common-subsequence/](../longest-common-subsequence/)): compares TWO sequences for a shared order-preserving run, using a 2D `dp[i][j]` table — versus LIS's ONE sequence and 1D `dp[i]` (or the `tails` array).
- **Kadane's Algorithm** ([../../array-string-patterns/kadanes-algorithm/](../../array-string-patterns/kadanes-algorithm/)): also a 1D single-sequence pattern, but answers a sum-based, CONTIGUOUS question — no order relation, no gaps allowed.

| Pattern | Sequences | Contiguous? | State shape |
|---|---|---|---|
| Longest Increasing Subsequence | One | No (gaps allowed) | `dp[i]` (or `tails[]`) |
| Longest Common Subsequence | Two | No (gaps allowed) | `dp[i][j]` |
| Kadane's Algorithm | One | Yes | Running sum, `O(1)` state |

## Interview Discussion

Experienced engineers expect the `O(n^2)` DP as a baseline, then specifically probe whether you know (and can explain the correctness of) the `O(n log n)` patience-sorting optimization — one of the clearest signals of DP depth versus pattern-memorization.

Follow-ups worth rehearsing:
- *"Is the `tails` array itself a valid subsequence?"* — expects a clear "no," with an explanation of what it actually represents.
- *"How would you reconstruct the actual longest subsequence, not just its length, from the `O(n log n)` approach?"* — expects recognizing that extra parent-pointer bookkeeping is needed alongside the `tails` updates.
- *"What if the subsequence needs to be non-decreasing rather than strictly increasing?"* — expects flipping `lower_bound` to `upper_bound` (or the reverse), and explaining why.

Misconceptions worth killing early:
- **"Longest Increasing Subsequence is about contiguous runs."** It is not — it explicitly allows gaps.
- **"The `tails` array from the O(n log n) approach is the answer."** Only its *length* is the answer; the array itself doesn't represent one real subsequence.

## Key Takeaways

1. `dp[i]` = length of the LIS ending exactly at index `i`; the final answer is `max(dp)`, not `dp[n-1]`.
2. The `O(n log n)` approach maintains `tails[k]` = smallest tail value achievable for subsequence length `k+1`.
3. Binary search (`lower_bound`) finds where each new element belongs in `tails` in `O(log n)`.
4. The `tails` array is NOT a real subsequence — only its length is directly meaningful.
5. Reconstructing the actual LIS from the `O(n log n)` approach needs extra parent-pointer bookkeeping.
6. Subsequence means gaps allowed — don't confuse with the much simpler contiguous "increasing subarray" problem.
7. LIS is a single-sequence, 1D-state pattern — contrast with LCS's two-sequence, 2D-state shape.
8. Sorting first, then running LIS on a second dimension, solves 2D nesting/ordering problems (Russian Doll Envelopes).
9. Complexity: `O(n^2)` DP or `O(n log n)` patience sorting — the latter is a genuine algorithmic upgrade, not a constant-factor tweak.
10. Real uses: stock trend analysis, patience-sorting-based logistics algorithms, version/metric monotonicity checks.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — dynamic programming foundations applicable to the `O(n^2)` formulation.
- *The Algorithm Design Manual* — Steven Skiena — covers LIS and patience sorting as a classical algorithmic technique.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — community-maintained classic algorithm implementations, including LIS in both complexity classes.

**Official Documentation**
- cppreference — `std::lower_bound` — the binary search primitive used by the `O(n log n)` approach.
- LeetCode — Longest Increasing Subsequence (problem 300).
- LeetCode — Number of Longest Increasing Subsequence (problem 673).
- LeetCode — Russian Doll Envelopes (problem 354).
- LeetCode — Maximum Length of Pair Chain (problem 646).

**Blog Articles**
- GeeksforGeeks — "Longest Increasing Subsequence" DP explainer, covering both the `O(n^2)` and `O(n log n)` approaches.
- Educative.io — "Grokking Dynamic Programming Patterns for Coding Interviews," the LIS pattern chapter.
