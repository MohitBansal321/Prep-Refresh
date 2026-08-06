# Prefix Sum — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "many range queries over a fixed array" signal that means "reach for prefix sum," and (2) noticing when the *combined* trick — a running prefix sum plus a hash map of prefix-sum values (or frequencies) seen so far — turns a subarray-existence question into a single O(n) pass.

> Rule of thumb for every exercise: before writing a single line, ask "will this array be queried more than once, and is it static (or can I treat it as static for this operation)?" and "am I being asked for a fixed range's sum, or does the range depend on some property I have to discover as I scan?" If you cannot answer both, you are not ready to write the prefix-sum logic yet.

---

## Easy — Find Pivot Index

**LeetCode 724 — Find Pivot Index.**

Given an array of integers `nums`, find the leftmost "pivot index" where the sum of all elements to the left of the index equals the sum of all elements to the right of the index (an index with no elements on one side has a sum of 0 there).

**Constraints to notice:** you need, for every candidate index, both "sum of everything before it" and "sum of everything after it" — computing either from scratch on every index is O(n) per index, O(n²) total.

**Task:** solve it in a single O(n) pass using a running prefix sum, where "sum to the right of `i`" is derived from the array's total sum minus the running prefix sum up to and including `i`, without a second array of suffix sums.

**Think about:** why can "sum to the right" always be expressed as `total - prefixSum(0, i)`, and what does that tell you about the relationship between prefix sums and suffix sums in general?

---

## Medium — Subarray Sum Equals K

**LeetCode 560 — Subarray Sum Equals K.**

Given an array of integers `nums` and an integer `k`, return the total number of contiguous subarrays whose sum equals `k`.

**Task:** this is deliberately listed here even though a fully worked version lives in [problems/02-subarray-sum-equals-k.cpp](problems/02-subarray-sum-equals-k.cpp) — solve it yourself first, from the README's Architecture and Solution sections alone, before checking the worked file. Use a running prefix sum and a hash map counting how many times each prefix-sum value has been seen so far.

**Think about:** the algebra `prefixSum[j] - prefixSum[i] == k` rearranges to `prefixSum[i] == prefixSum[j] - k`. What question does that rearrangement let you ask the hash map at each step, and why does counting (not just checking existence) matter for this specific problem?

---

## Hard — Maximum Size Subarray Sum Equals K

**LeetCode 325 — Maximum Size Subarray Sum Equals K.**

Given an array of integers `nums` and a target value `k`, find the **length** of the longest subarray that sums to exactly `k`. If there is no such subarray, return 0.

**Task:** adapt the prefix-sum-plus-hash-map idea from Subarray Sum Equals K, but this time the hash map must store, for each prefix-sum value, the **earliest index** at which it occurred — not a count. Solve it in O(n) time.

**Think about:** why does storing only the *first* occurrence of each prefix-sum value (rather than every occurrence, or the most recent one) guarantee you find the *longest* qualifying subarray, not just *a* qualifying one?

---

## Real-World Challenge — Suspicious Transaction Window Detector

You work on a fraud-detection team. You are given a finalized, static batch of a single account's transaction amounts for one day, as a `std::vector<long long>` (deposits positive, withdrawals negative). Compliance wants a report answering, for a large number of analyst-submitted `(startIndex, endIndex)` ranges (potentially thousands of them, submitted interactively as analysts explore the data), the net amount moved in that exact range — and, separately, whether *any* contiguous window within the whole day sums to exactly a specific "structuring" threshold amount (a legally significant amount analysts want to check for, e.g., just under a reporting threshold).

**Task:**
1. Build a 1D prefix-sum structure once over the day's transactions, and answer each analyst range query in O(1).
2. Using the prefix-sum-plus-hash-map technique, determine in a single O(n) pass whether any contiguous subarray of the day's transactions sums to exactly the threshold amount, and report the shortest such window if multiple exist.
3. Discuss: the transaction log for **today** is still being appended to in real time when compliance wants a preliminary report. What breaks if you serve range queries against a prefix-sum structure built at 9am while new transactions keep arriving through the day? What would you change about your approach (hint: think about *when* you are willing to say the array is "static enough" to build the prefix array, versus reaching for a structure that tolerates ongoing appends).

---

## Bonus Challenge — 2D Region Sum with Repeated Queries

**LeetCode 304 — Range Sum Query 2D - Immutable.**

Given a 2D matrix that does not change, design a structure to efficiently answer many queries of "what is the sum of the elements inside the rectangle `(row1, col1)` to `(row2, col2)`, inclusive."

**Task:** implement the 2D prefix sum (as shown in [code.cpp](code.cpp)) from scratch yourself, without looking at the provided implementation, then verify your answers against it on at least one non-trivial matrix.

**Then, generalize in writing (no code required):** the README describes the 2D formula as needing a "subtract twice-counted region back in" correction term that the 1D formula does not need. Explain, in your own words, why going from 1D ranges to 2D rectangles introduces this extra term, and speculate (or look up) what the equivalent formula would need for a 3D "box sum" over a 3D array. Is the pattern still fundamentally "the same trick," or does something qualitatively new appear as dimensions increase?

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
