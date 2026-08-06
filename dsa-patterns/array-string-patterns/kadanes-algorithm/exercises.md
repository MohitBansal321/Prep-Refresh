# Kadane's Algorithm — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing when a problem's "best contiguous sum" shape is close enough to vanilla Kadane's that the extend-or-restart core applies directly, and (2) correctly identifying *what needs to change* when a variant looks similar but isn't identical (product instead of sum, circular wraparound, a running count instead of a running total).

> Rule of thumb for every exercise: before writing a single line, ask "is this actually about a *contiguous* run, with no fixed window size?" and "does plain addition/sum still behave monotonically here, or does something (negative numbers flipping a product, wraparound, a different aggregation) break the vanilla extend-vs-restart argument?" If you cannot answer both, re-read the Architecture and Common Mistakes sections of the [README](README.md) before coding.

---

## Easy — Maximum Subarray

**LeetCode 53 — Maximum Subarray.**

Given an integer array `nums`, find the contiguous subarray (containing at least one number) that has the largest sum, and return that sum.

**Constraints to notice:** the array can contain negative numbers, and it can be entirely negative — the answer must still be a valid, non-empty subarray's sum, not `0`.

**Task:** solve it in O(n) time, O(1) extra space, using the vanilla extend-or-restart Kadane's core described in the [README](README.md).

**Think about:** why must both `current_sum` and `best_sum` initialize from `nums[0]` rather than `0`? Construct a concrete all-negative test array where initializing from `0` would silently produce the wrong answer.

---

## Medium — Maximum Product Subarray

**LeetCode 152 — Maximum Product Subarray.**

Given an integer array `nums`, find a contiguous non-empty subarray that has the largest product, and return that product.

**Task:** adapt Kadane's core idea to products instead of sums. A single comparison against "restart here" is no longer enough on its own — think carefully about why.

**Think about:** for sums, a negative running total can only hurt what comes after it, so "restart" only ever competes against "extend." For *products*, a large-magnitude **negative** running product can become the largest **positive** product the moment you multiply it by another negative number. What running value do you need to track *in addition to* the running maximum product, and why? What happens at the exact moment you encounter a `0` in the array?

---

## Hard — Maximum Sum Circular Subarray

**LeetCode 918 — Maximum Sum Circular Subarray.**

Given a **circular** integer array `nums` (the end connects back to the beginning), find the maximum possible sum of a non-empty subarray, where the subarray may wrap around from the end of the array back to the start.

**Task:** solve it in O(n) time, O(1) extra space, by combining a normal (non-wrapping) Kadane's pass with a second pass that finds the **minimum** sum subarray. Do not attempt to physically "unroll" the array into a doubled array and re-run Kadane's over a sliding window of length `n` — that changes the complexity profile and defeats the purpose of this exercise.

**Hint toward the invariant, not the solution:** the best *wrapping* subarray's sum equals `total_sum - (the minimum sum of any non-wrapping subarray)`, because the elements NOT included in the minimum-sum subarray are exactly the elements that would be included in the best wrap-around subarray. The final answer is the larger of (a) the ordinary non-wrapping Kadane's result and (b) this wraparound quantity.

**Then answer:** there is exactly one case where the "total minus minimum" trick produces a wrong (empty-subarray) answer. What is that case, and why does it happen? (Hint: what does `total_sum - minimum_subarray_sum` evaluate to when every element in the array is negative?)

---

## Real-World Challenge — Latency Regression Window Finder

You operate a service that reports per-minute p99 latency. For each minute, you compute `delta = this_minute_p99 - rolling_baseline_p99` (a signed number: positive means latency is elevated above baseline, negative means it is better than baseline). You have a full day's worth of these deltas (1440 values) from an incident.

**Task:**
1. Using the vanilla Kadane's core (adapted to also report indices, as in [code.cpp](code.cpp)), find the single worst *contiguous* regression window — the stretch of consecutive minutes with the largest cumulative positive delta — and report both the total excess latency and the exact minute range.
2. Extend your solution to report the **top 3 non-overlapping** worst windows, not just the single worst one. (Hint: after finding and recording the best window, what is the simplest correct way to prevent your next search from re-selecting overlapping minutes? Consider zeroing out or excluding the already-reported range and re-running, versus a more sophisticated one-pass approach — discuss the tradeoff between the two in a comment.)
3. Discuss: your rolling baseline itself is computed from a trailing 7-day average. If the baseline calculation has a bug that makes it lag by a few minutes during a genuine incident, what would that do to your reported "regression window" boundaries? Would the *existence* of a bad window still be detected correctly, even if its exact boundaries were slightly off?

---

## Bonus Challenge — Maximum Sum Sub-Matrix (2D Kadane's)

**LeetCode 363 — Max Sum of Rectangle No Larger Than K** (or, for a simpler warm-up, the classic unconstrained "maximum sum sub-matrix" problem without the `K` bound).

Given a 2D matrix of integers, find the sum of the maximum-sum rectangular sub-matrix (a contiguous block of rows and columns).

**Task:** implement the classic reduction to 1D Kadane's: for every pair of row boundaries `(top, bottom)`, collapse each column's values across those rows into a single 1D array (by summing), then run 1D Kadane's (your `maxSubarraySum` from [code.cpp](code.cpp)) on that collapsed array to find the best contiguous *column* range for this particular row pair. Track the best result across all `O(rows²)` row-boundary pairs.

**Then, generalize in writing (no code required):** state the overall time complexity of this 2D approach in terms of `rows` and `cols`, and explain precisely which part of the algorithm is the 1D Kadane's core reused verbatim, versus which part is genuinely new machinery layered on top. Referencing the Similar Patterns section of the [README](README.md), is this "the same pattern as 1D Kadane's," or a distinct one that merely *uses* Kadane's as a subroutine? Justify your answer.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
