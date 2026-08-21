# K-way Merge — Exercises

Work through these in order. The goal is to build two reflexes: (1) **spotting the K sorted sequences when nobody labels them as such** — rows of a matrix, an implicit sequence generated on demand from a pair of indices, one sorted file per producer; and (2) never separating a pop from its replenishment, so the heap always holds exactly one current candidate per still-active source.

> Rule of thumb for every exercise: before writing a line, answer out loud — "what are my K sorted lists, and for a popped entry, what *exactly* is the next candidate from that same list?" If you cannot name both, you are not ready to write the loop yet. The four problems in [problems/](problems/) are your reference implementations; each exercise below points at the specific one to adapt.

---

## Easy — Merge Two Sorted Lists

**LeetCode 21 — Merge Two Sorted Lists.**

Given the heads of two sorted linked lists, merge them into one sorted linked list and return its head. The merged list should be made by splicing together the nodes of the two input lists.

**Task:** solve it **twice**. First, adapt [problems/01-merge-k-sorted-lists.cpp](problems/01-merge-k-sorted-lists.cpp) essentially unchanged — seed the min-heap with both heads, pop-and-replace until empty — to confirm the general machinery degenerates correctly at K = 2. Then throw that away and write the two-pointer version: two cursors, compare `a->val` against `b->val`, splice the smaller, advance only that cursor. Keep both.

**Then answer:** the heap version is O(n log 2) = O(n) and the two-pointer version is O(n) — asymptotically identical. So state concretely what the heap version costs you here that the two-pointer version does not (count the actual operations per emitted node: heap push, sift-down, tuple construction), and reconcile that with the README's "When NOT To Use" first bullet. Then flip it: at what value of K would you stop hand-writing pointer comparisons and reach for the heap, and what makes that the crossover — asymptotics, or readability?

---

## Medium — Ugly Number II

**LeetCode 264 — Ugly Number II.**

An ugly number is a positive integer whose prime factors are limited to 2, 3, and 5. Return the `n`-th ugly number. The sequence begins 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, ...

**Task:** recognize this as **K = 3 sorted lists that do not exist yet**. If `U` is the ascending sequence of ugly numbers you are building, then `2*U`, `3*U`, and `5*U` are each ascending sequences, and the next ugly number is always the smallest unconsumed value across those three — a three-way merge whose sources are generated from the answer you are currently producing. Use the early-exit shape from `kthSmallestInKSortedLists` in [code.cpp](code.cpp): stop at the `n`-th emission rather than building anything larger. Solve it once with a min-heap and once with three explicit index pointers into `U`.

**Think about:** this problem needs something the base pattern does not give you — **deduplication while merging**, the case the README's "When NOT To Use" flags as needing explicit extension. `2*3 = 6` and `3*2 = 6` arrive from two different sources as the same value. Write down, in one sentence each, how the heap version handles that (what do you do when the popped value equals the last emitted value?) and how the three-pointer version handles it (which pointers advance when several sources tie?). **Then answer:** which of the two is easier to get right, and why is that not the same question as which is faster?

---

## Hard — Find the Kth Smallest Sum of a Matrix With Sorted Rows

**LeetCode 1439 — Find the Kth Smallest Array Sum of a Matrix With Sorted Rows.**

Given an `m x n` matrix `mat` where every row is sorted ascending, and an integer `k`, you may choose exactly one element from each row. Each choice produces an array whose sum is one candidate. Return the `k`-th smallest such sum among all `n^m` possible arrays.

**Task:** this is the full generalization of [problems/03-find-k-pairs-with-smallest-sums.cpp](problems/03-find-k-pairs-with-smallest-sums.cpp) from two arrays to `m` of them, and the trick is that you do **not** merge all `m` rows at once. Merge **incrementally, row by row**: hold a sorted list of the best-so-far sums using rows `0..i-1`, then merge it against row `i` using exactly the `problems/03` heap (each element of the running list defines an implicit sorted sequence when paired against row `i`'s ascending elements), and **truncate the result to its smallest `k` entries** before moving on to row `i+1`. Repeat for all `m` rows; the answer is the `k`-th entry of the final list.

**Then answer:** the truncation to `k` is what makes this tractable rather than exponential — justify it rigorously. Why can a sum that is already outside the best `k` after processing row `i` never re-enter the best `k` after processing row `i+1`? Name the property of the values being added that this argument depends on, and state precisely what breaks in your proof (and in your code) if the matrix were allowed to contain negative entries.

---

## Real-World Challenge — Chronological Incident Timeline Service

A production incident spanned 40 minutes across 12 NestJS microservices. Each service wrote its own append-only log file to object storage, so each file is **already sorted by timestamp** (logs are appended in time order) but the files know nothing about each other. Total volume for the window is ~4 GB — far more than you want resident in a Node process. You are building an internal endpoint that streams back one globally time-ordered timeline.

**Task:**

1. Implement the merge as the streaming form of this pattern: open a buffered reader per file, seed a min-heap with one parsed log line per file keyed on `(timestamp, serviceName)`, then pop-and-replace, writing each popped line to the response stream. Assert the invariant explicitly in your head: how many log lines are resident in memory at once, as a function of the number of services and of the 4 GB total? Which of those two numbers does it depend on, and which does it not?
2. Handle the ugly parts the base pattern does not: a file that is **empty** for the window (the seeding guard from [code.cpp](code.cpp)); two services logging the **identical** timestamp (pick and justify a deterministic tie-break, then say why any tie-break is correct for sortedness but not necessarily for the reader's understanding of causality); and a single **corrupt/unparseable line** mid-file (does that file get dropped, skipped-and-continued, or does the whole request fail — and what does your choice do to the sortedness guarantee?).
3. The endpoint also needs a cheap `?limit=500` mode returning only the earliest 500 lines. Implement it as the early-exit variant and quantify the win: with 12 services and ~20 million total lines, how many heap operations does `limit=500` do versus a full merge, and roughly how many bytes do you avoid reading off object storage?
4. Product now wants the timeline **sorted by severity, then timestamp**. Answer without writing code: does this pattern still apply? Each file is sorted by timestamp, not by severity — so which invariant from the README's Solution section does the new ordering violate, and what would you have to do to the inputs (or the query) to make a K-way merge legal again?

**Discuss:** a colleague proposes something simpler — download all 12 files into a temp directory, `cat` them together, and pipe through `sort -k1`. It is three lines and it is correct. Make the case for the heap-based merge anyway, in terms your colleague will accept: peak memory, time-to-first-byte for a streaming HTTP response, behavior when the window is 4 hours instead of 40 minutes, and what happens when one service's file is 3.5 GB of the 4 GB total. Then argue the *other* side: name a concrete set of inputs where the colleague is right and you are over-engineering.

---

## Bonus Challenge — K-th Smallest Prime Fraction

**LeetCode 786 — K-th Smallest Prime Fraction.**

Given a sorted array `arr` of unique primes (plus the integer 1) and an integer `k`, consider every fraction `arr[i] / arr[j]` with `i < j`. Return the `k`-th smallest such fraction as `[arr[i], arr[j]]`.

**Task:** find the K sorted sequences. They are not the obvious ones, and getting the direction right is the whole exercise: for a **fixed numerator** `arr[i]`, the fractions `arr[i]/arr[j]` get *smaller* as `j` increases (a larger denominator), so that sequence is descending — while for a **fixed denominator** `arr[j]`, the fractions `arr[i]/arr[j]` ascend as `i` increases. Pick the framing that gives you K ascending sequences, seed a min-heap with one candidate per sequence, and run the early-exit loop from [code.cpp](code.cpp) to the `k`-th pop, adapting the index-advancing step from [problems/03-find-k-pairs-with-smallest-sums.cpp](problems/03-find-k-pairs-with-smallest-sums.cpp).

**Then, generalize in writing (no code required):** two things here are genuinely new relative to every problem in [problems/](problems/). First, the heap must order by a **derived key** (a ratio) rather than by a stored value, and comparing ratios as `double` risks precision loss — write down the integer cross-multiplication comparison that avoids floating point entirely (`a/b < c/d` ⟺ ?), and state the overflow condition you must check before trusting it. Second, this problem admits a completely different O((m+n) log(max)) solution by **binary searching on the answer value** rather than merging at all. Explain, using the README's Complexity section, when each approach wins as a function of `k` relative to the total pair count — and say which one you would bring to an interview first, and why.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
