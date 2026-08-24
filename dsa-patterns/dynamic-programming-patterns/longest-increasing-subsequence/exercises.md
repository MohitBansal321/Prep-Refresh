# Longest Increasing Subsequence — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of recognizing "one sequence, order relation, gaps allowed" and choosing between the `O(n^2)` DP and the `O(n log n)` patience-sorting approach depending on what the problem actually needs.

> Rule of thumb: if you only need the LENGTH, always reach for patience sorting first. If you need the actual subsequence (or a count of subsequences), you likely need the `O(n^2)` DP (or extra bookkeeping bolted onto patience sorting).

---

## Easy — Longest Increasing Subsequence (Length Only)

Implement `lengthOfLIS_On2` from scratch without looking at `code.cpp`.

**Requirements:**
- `dp[i]` = length of the LIS ending at index `i`.
- Return `max(dp)`, not `dp[n-1]`.

**Then answer in a comment:** why is it wrong to return `dp[n-1]`? Give a concrete array where `dp[n-1]` and `max(dp)` differ.

---

## Medium — Longest Non-Decreasing Subsequence

Adapt the patience-sorting approach to allow **equal** consecutive elements (non-decreasing, not strictly increasing) — e.g. `[1, 3, 3, 5]` should count as length 4, not 3.

**Task:**
1. Identify exactly which binary search call (`lower_bound` vs `upper_bound`) needs to change, and explain why in a comment.
2. Verify your change on `[2, 2, 2, 2]` (should return 4) and on `[1, 3, 2, 3]` (should return 3, via `[1,2,3]` or `[1,3,3]`).

---

## Hard — Reconstruct the Actual LIS from the O(n log n) Approach

Implement a version of `lengthOfLIS_NLogN` that also returns the actual longest increasing subsequence (not just its length), using parent-pointer bookkeeping alongside the `tails` updates.

**Requirements:**
- Alongside `tails`, maintain a `predecessors` array recording, for each array element, the index of the element that precedes it in the subsequence ending there.
- Alongside `tails`, maintain an array recording which original array INDEX currently holds each tail value (since `tails` itself stores values, but reconstruction needs positions).
- After the main loop, walk backward from the index that produced the final (largest) tail, following `predecessors`, and reverse the collected result.

**Prove it:** on `[10, 9, 2, 5, 3, 7, 101, 18]`, confirm your reconstruction produces a valid length-4 strictly increasing subsequence using indices from the original array (there may be more than one correct answer — any valid one of length 4 is acceptable).

---

## Real-World Challenge — Longest Chain of Compatible API Versions

You have a list of API version compatibility records, each `(version_number, min_required_version)`, arriving in an arbitrary order. A "compatible chain" is a sequence of versions where each next version's `min_required_version` is satisfied by staying within an increasing sequence of `version_number`s already chosen (i.e., you're finding the longest chain of versions you could roll through, upgrading one at a time, where each upgrade is valid).

**Task:**
1. Model this as an LIS-style problem: sort appropriately, then determine what "increasing" means in this context (it may not be simple numeric increase — think about what property must hold for a valid upgrade chain).
2. Implement it and test on a small synthetic list of 6-8 version records with at least one "trap" record that looks like it should extend the chain but actually can't (due to the compatibility constraint).
3. **Distributed-systems framing:** explain in writing why a rolling upgrade across a fleet of services might use exactly this kind of analysis to compute the longest safe upgrade path through a partially-ordered set of version compatibility constraints, and why greedily upgrading to the "next available" version without this analysis could strand you at a version with no further safe upgrade path.

---

## Bonus Challenge — Three Related Problems, One Technique

Implement all three, each choosing the more appropriate of the two techniques in this module (state which one you chose and why, in a comment):

1. **Longest Increasing Subsequence** (LeetCode 300) — the base case.
2. **Russian Doll Envelopes** (LeetCode 354) — sort by one dimension, LIS the other, with the tie-break trick.
3. **Maximum Length of Pair Chain** (LeetCode 646) — a close relative solvable more simply by a greedy sort (not full LIS machinery).

**Then, the punch line:** for problem 3, explain in a comment exactly why a simple greedy (sort by right endpoint, always extend if the next pair's left exceeds the current chain's last right endpoint) is provably correct here, whereas the general "longest increasing subsequence" problem has no such safe greedy rule. What's structurally different about problem 3 that makes greedy safe?

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
