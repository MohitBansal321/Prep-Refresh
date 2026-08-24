# Top K Elements — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "K largest/smallest/most-frequent out of a much bigger collection" signal that means "reach for a size-K heap," and (2) correctly deciding, every time, which heap type (min or max) belongs on this specific problem — the inversion that trips up almost everyone at first.

> Rule of thumb for every exercise: before writing a single line, say out loud "the heap's top must be the element I would evict first" — then decide whether that means min-heap or max-heap for *this* problem. If you cannot answer that in one sentence, you are not ready to write the heap logic yet.

---

## Easy — Kth Largest Element in a Stream

**LeetCode 703 — Kth Largest Element in a Stream.**

Design a class that, given an integer `k` and an initial stream of integers, supports an `add(val)` method that inserts `val` into the stream and returns the current k-th largest element in the stream after insertion.

**Constraints to notice:** unlike LeetCode 215 (a one-shot array), this is a genuinely *streaming* problem — the heap must persist across calls, not be rebuilt from scratch on every `add`.

**Task:** maintain a single min-heap of size k as a class member. On each `add`, push the new value, then pop if the heap exceeds size k, exactly like the pattern's core rule — but here the heap lives across calls instead of being local to one function.

**Think about:** why is a persistent size-K heap a natural fit for a class with an `add` method, in a way that "sort the whole stream so far on every call" clearly is not?

---

## Medium — Sort Characters By Frequency

**LeetCode 451 — Sort Characters By Frequency.**

Given a string `s`, sort it in decreasing order based on the frequency of the characters. Return any string with characters sorted by frequency, with characters of equal frequency in any order.

**Task:** count character frequencies with a hash map, then use a **max-heap** keyed by frequency (not a size-bounded heap, since here you need *all* distinct characters back, not just the top K) to repeatedly pop the most frequent remaining character and append it to the result the correct number of times.

**Think about:** this problem does not bound the heap to size K at all — every distinct character must appear in the output. Why does the "push then evict if oversized" rule not apply here, and what does that tell you about when a full (unbounded) heap, rather than a size-K heap, is the right tool?

---

## Hard — Find K Pairs with Smallest Sums

**LeetCode 373 — Find K Pairs with Smallest Sums.**

Given two integer arrays `nums1` and `nums2` sorted in ascending order and an integer `k`, return the `k` pairs `(u, v)` with `u` from `nums1` and `v` from `nums2` that have the smallest sums.

**Task:** naively generating all `len(nums1) * len(nums2)` pairs and running a size-K max-heap over them works, but is wasteful when both input arrays can be large. Instead, exploit that both arrays are already sorted: seed a min-heap with the k smallest-looking candidate pairs (starting from index-0 pairs) and expand outward, popping the smallest sum and pushing its "neighbor" candidates, stopping once k pairs have been extracted.

**Then answer:** this hard variant blends Top K Elements with an idea from K-way Merge (seeding a heap with the "next candidate" from each sorted source rather than scanning everything). Explain, in your own words, why exploiting the sortedness of the two input arrays changes the complexity compared to the naive size-K-heap-over-all-pairs approach.

---

## Real-World Challenge — Top-K Slowest Endpoints Dashboard

You operate a backend service that logs a `(endpoint_name, latency_ms)` pair for every incoming HTTP request — potentially millions per hour. You need to power a live dashboard widget showing the **10 slowest individual requests** observed in the current rolling hour, updated continuously as new requests complete, without ever holding the full hour of raw request logs in memory at once.

**Task:**
1. Design (and implement, reading from a `std::vector<std::pair<std::string,double>>` standing in for a live request-completion stream) a size-10 heap that tracks the 10 slowest requests seen so far, choosing the correct heap type and justifying it in a comment.
2. Extend the design so that entries older than the rolling hour are eventually evicted even if they were once in the top 10 (a size-K heap alone does not model *time-based* expiry — discuss what additional structure, such as a time-indexed secondary structure or periodic rebuild, you would need in production).
3. Discuss: if two requests to different endpoints tie exactly on latency, does your chosen heap comparator need an explicit tie-break to produce deterministic output? Why or why not, given the problem only asks for "the 10 slowest," not a fully deterministic ranking?

---

## Bonus Challenge — Kth Smallest Element in a Sorted Matrix

**LeetCode 378 — Kth Smallest Element in a Sorted Matrix.**

Given an `n x n` matrix where each of the rows and columns is sorted in ascending order, return the `k`-th smallest element in the matrix.

**Task:** implement a size-K max-heap solution (push every element, evict the largest whenever the heap exceeds size k, and the remaining top is the answer) and get it working first. Then, **without writing the code**, describe in your own words a smarter approach that exploits the matrix's row/column sortedness (a binary-search-on-value approach, or a heap seeded with only one candidate per row rather than every element) to avoid ever pushing all `n²` elements onto a heap.

**Then, generalize in writing:** the straightforward size-K heap solution here technically works but is not the "intended" solution for this LeetCode problem at large `n`. Explain, using the Complexity section of the [README](../README.md), exactly why a plain size-K heap over *all* `n²` elements stops being the efficient choice once the matrix is sorted in a way a smarter approach can exploit — is this a case where the pattern applies but is suboptimal, or a case where the pattern does not really apply at all? Justify your answer.

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
