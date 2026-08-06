# Two Pointers — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the sorted-input + pair/triplet or in-place-compaction signal that means "reach for two pointers," and (2) correctly deciding, on every step, *which* pointer is safe to move and why.

> Rule of thumb for every exercise: before writing a single line, ask "is this array sorted (or can I sort it without losing information I need)?" and "am I searching for a pair/triplet/optimum, or compacting in place?" If you cannot answer both questions, you are not ready to write the pointer logic yet.

---

## Easy — Squares of a Sorted Array

**LeetCode 977 — Squares of a Sorted Array.**

Given an integer array `nums` sorted in non-decreasing order, return an array of the squares of each number, also sorted in non-decreasing order.

**Constraints to notice:** the input can contain negative numbers, so squaring does not preserve sort order directly (e.g. `[-4, -1, 0, 3, 10]` squared is `[16, 1, 0, 9, 100]` — not sorted). The largest squares always come from the elements *furthest from zero*, which live at the two ends of a sorted array.

**Task:** solve it in O(n) time using two pointers, filling the result array **from the back**. Do not sort the squared values afterward — that would cost O(n log n) and defeats the point of the exercise.

**Think about:** why does filling the output array back-to-front (rather than front-to-back) fall out naturally from a converging two-pointer scan?

---

## Medium — 3Sum Closest

**LeetCode 16 — 3Sum Closest.**

Given an integer array `nums` and an integer `target`, find three integers in `nums` such that the sum is closest to `target`. Return that sum.

**Task:** adapt the 3Sum approach from [problems/03-3sum.cpp](problems/03-3sum.cpp) — sort, fix one element, converge two pointers over the rest — but instead of stopping on an exact match, track the running closest sum seen so far and decide which pointer to move based on whether the current sum is above or below `target`.

**Think about:** in problems/03-3sum.cpp, an exact match lets you skip past duplicate values immediately. Here there is no exact match to trigger that skip. Do you still need duplicate-skipping logic at all? Justify your answer.

---

## Hard — Trapping Rain Water

**LeetCode 42 — Trapping Rain Water.**

Given `n` non-negative integers representing an elevation map where the width of each bar is 1, compute how much water it can trap after raining.

**Task:** solve it with two pointers in O(n) time and O(1) extra space (the classic DP solution using two precomputed `leftMax`/`rightMax` arrays is O(n) time but O(n) space — your job is to collapse that to O(1) space).

**Hint toward the invariant, not the solution:** maintain a running `leftMax` and `rightMax` as you go, and — similar to Container With Most Water — always advance the pointer on the side with the smaller current max, because water trapped above that position is bounded by the smaller of the two maxes, and you already know which one that is without needing to know the other side's exact value yet.

**Then answer:** why is it safe to process the side with the smaller max without knowing the exact max on the other side? What invariant guarantees your computed water level at that position is still correct?

---

## Real-World Challenge — Streaming Log Merge with Deduplication

You operate two services that each write timestamped, already-sorted log lines to disk (each service's own log file is sorted by timestamp because it appends in real time). For an incident postmortem, you need a single merged, sorted, de-duplicated timeline across both files — some events get logged by both services with the identical timestamp and message due to a retry-on-timeout bug, and those duplicates must not appear twice in the final report.

**Task:**
1. Design (and implement, reading from two `std::vector<std::string>` standing in for the files) a two-pointer merge that walks both sorted logs simultaneously, always advancing the pointer with the earlier timestamp — the same participant/invariant structure as the merge step of merge sort.
2. Layer in-place deduplication (same-direction write/read pointer, as in [problems/02](problems/02-remove-duplicates-from-sorted-array.cpp)) on the merged output so that two adjacent identical entries collapse into one.
3. Discuss: your two log files are each individually sorted, but what happens to your algorithm's correctness guarantee if clock drift between the two services means a small number of entries are **not** perfectly ordered by wall-clock time across files? Where exactly does the two-pointer invariant break, and what would you do about it in production (hint: bounded out-of-order tolerance / a small sort-then-merge window)?

---

## Bonus Challenge — Partition an Array Around a Pivot (Dutch National Flag)

**LeetCode 75 — Sort Colors.**

Given an array with only the values 0, 1, and 2, sort it in place in a single pass, without using a library sort.

**Task:** implement the classic **three-way partition** (Dutch National Flag algorithm) using **three** pointers: `low`, `mid`, and `high` — a generalization of the same-direction slow/fast pointer idea to three regions instead of two. `low` marks the end of the "all 0s" region, `high` marks the start of the "all 2s" region, and `mid` scans forward, swapping elements into the correct region as it goes.

**Then, generalize in writing (no code required):** the two-pointer template in [code.cpp](code.cpp) has exactly two moving boundaries. Explain, in your own words, what changes structurally when you go from a two-region partition (e.g. "keep vs. discard" in `two_pointer_compact`) to a three-region partition. Is this still "the same pattern," or a distinct one? Justify your answer using the Architecture section of the [README](README.md).

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
