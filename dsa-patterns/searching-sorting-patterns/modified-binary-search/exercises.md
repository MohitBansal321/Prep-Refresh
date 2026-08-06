# Modified Binary Search — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the sorted / piecewise-sorted / monotonic-boundary signal that means "reach for a halving search," and (2) correctly stating, before writing a single line, exactly which half of the search space a given comparison lets you discard.

> Rule of thumb for every exercise: before writing the loop, ask "is this data sorted, rotated-sorted, or does it have some other property where everything on one side of a boundary is false and everything on the other side is true?" and "am I searching for an exact value, a boundary/edge, or an extremum?" If you cannot answer both, you are not ready to write the halving logic yet.

---

## Easy — Search Insert Position

**LeetCode 35 — Search Insert Position.**

Given a sorted array of distinct integers and a `target`, return the index if found. If not, return the index where it would be inserted to keep the array sorted.

**Constraints to notice:** this is almost identical to classic binary search, but the "not found" case cannot simply return -1 — you need to know exactly where the value *would* land.

**Task:** solve it in O(log n) time using the classic lo/hi/mid skeleton, and reason about what `lo` equals at the moment the loop exits (`lo > hi`) when the target is absent.

**Think about:** why is `lo` (not `hi`) always the correct insertion index at loop exit, regardless of whether the target would be smaller than every element, larger than every element, or somewhere in the middle?

---

## Medium — Find Peak Element

**LeetCode 162 — Find Peak Element.**

A peak element is one strictly greater than its neighbors. Given an array `nums` where `nums[-1] = nums[n] = -infinity`, find any peak and return its index. Must run in O(log n) time.

**Task:** the array is **not sorted**, so this is the sharpest test of whether you actually understand the halving *argument* rather than pattern-matching "sorted array -> binary search." Show that a valid halving rule still exists: at any `mid`, compare `nums[mid]` to `nums[mid + 1]`. If `nums[mid] < nums[mid + 1]`, argue that a peak must exist somewhere to the right (the sequence is "climbing," and it must eventually stop climbing or hit the boundary). If `nums[mid] > nums[mid + 1]`, argue the symmetric case for the left half including `mid`.

**Think about:** what is the actual monotonic property being exploited here, if the array itself is not sorted? Write it in one sentence before coding.

---

## Hard — Median of Two Sorted Arrays

**LeetCode 4 — Median of Two Sorted Arrays.**

Given two sorted arrays `nums1` and `nums2` of sizes `m` and `n`, return the median of the two combined, in O(log(min(m, n))) time.

**Task:** this is a much more aggressive generalization than anything in [code.cpp](code.cpp) — you are binary searching over a **partition point** (an index into the smaller array), not over values or a single array's indices. At each candidate partition, check whether the max of the "left" elements from both arrays is <= the min of the "right" elements from both arrays. If not, the halving rule tells you which direction to move the partition.

**Then answer:** why must you binary search over the *smaller* of the two arrays to guarantee O(log(min(m, n))), and what goes wrong (not just "slower," but actually incorrect or out-of-bounds) if you binary search over the larger one without adjusting the partition math?

---

## Real-World Challenge — Rate Limiter Threshold Lookup

You run a backend service that logs, for every minute of the last 24 hours, a monotonically non-decreasing cumulative request count (an array of 1440 integers, since each minute's count only ever adds to the running total). Given a `budget` (the maximum cumulative requests allowed before throttling kicks in), you need to find the **first minute index** at which the cumulative count exceeds `budget`, so you can report exactly when throttling should have started.

**Task:**
1. Design (and implement, reading from a `std::vector<long long>` standing in for the 1440-entry log) a boundary search that finds the first index `i` such that `cumulative[i] > budget`, using the same "keep narrowing after finding a candidate" idea as `findFirstOccurrence` in [code.cpp](code.cpp) — except here there is no exact match to look for, only a true/false predicate (`cumulative[i] > budget`).
2. Handle the edge cases: `budget` is never exceeded in the whole day (no valid answer), and `budget` is exceeded from minute 0 onward.
3. Discuss: this predicate-based boundary search (sometimes generalized as "binary search on the answer" or mirroring `std::lower_bound`) is a different framing than "search for an exact value" — explain, in your own words, why the monotonicity of the *predicate* (`cumulative[i] > budget` is false, then eventually always true) is what licenses binary search here, even though you are not searching for a specific number in the array at all.

---

## Bonus Challenge — Search in a Sorted Array of Unknown Size

**LeetCode 702 — Search in a Sorted Array of Unknown Size** (the array is only accessible through an `ArrayReader` interface with no `size()` method, and out-of-bounds reads return a sentinel like `INT_MAX`).

**Task:** implement (in writing, or in code against a small hand-rolled `ArrayReader` stand-in) a two-phase approach: first, **find a valid `hi` bound** by doubling a candidate upper bound (1, 2, 4, 8, ...) until it either overshoots the array's true length or its value exceeds `target`; then run a **normal classic binary search** within `[lo, hi]`.

**Then, generalize in writing (no code required):** the doubling phase is itself a form of the pattern's "which half is valid?" question, just applied to an *unbounded* search space instead of a fixed `[0, n-1]` range. Explain, using the Architecture section of the [README](README.md), why the doubling phase is still O(log n) (not O(n)) even though you do not know `n` in advance, and why combining it with a second binary search keeps the whole algorithm at O(log n) rather than O(log n) + O(n).

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
