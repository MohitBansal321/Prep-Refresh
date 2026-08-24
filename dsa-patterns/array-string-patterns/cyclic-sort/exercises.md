# Cyclic Sort — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting the `[1..n]`/`[0..n-1]` bounded-range signal and reaching for an in-place swap-to-home-index pass instead of sorting or hashing.

> Rule of thumb: before writing any code, answer "what is the home index for value `v`?" If you can't state that formula in one line (`v - 1`, `v`, `v % n`, etc.), you haven't found the mapping yet — don't start swapping.

---

## Easy — Find the Missing Number (0-indexed range)

Given an array of `n` distinct numbers taken from `[0, n]` (note: `n+1` possible values, `n` slots — exactly one is missing), find the missing number using cyclic sort.

**Requirements:**
- Adapt the home-index formula for a `[0, n]` range instead of `[1, n]` — think carefully about what changes when the range includes 0 and has one more possible value than slots.
- After sorting, scan for the first index where `nums[i] != i`; if none is found, the missing number is `n` itself (it would have lived at the one index past the end of the array).

**Then answer in a comment:** why does this problem need a different home-index formula than the classic `[1..n]` case, and what happens if you forget to adjust it?

---

## Medium — Set Mismatch

Given an array of `n` numbers from `[1, n]` where one number is duplicated (appearing twice) and, as a consequence, exactly one number is missing, return `[duplicate, missing]`.

**Task:**
1. Cyclic-sort the array.
2. Find the first index where `nums[i] != i + 1` — that slot's actual value is the duplicate (since the duplicate is what's occupying a home that isn't its own), and `i + 1` is the missing value.
3. Handle the edge case where the array has only one element (trivially, no mismatch structure applies the same way — think about whether this can even happen given the problem's guarantees).

---

## Hard — First Missing Positive, In-Place Without Extra Range Assumptions

Given an UNSORTED array that may contain negatives, zeros, duplicates, and values far outside `[1, n]`, find the smallest missing positive integer using `O(n)` time and `O(1)` extra space.

**Requirements:**
- The key insight: the answer is always in `[1, n+1]` regardless of what garbage values are in the array — reason through why in a comment before coding (what's the largest possible answer if an array of length `n` contains exactly `1..n`?).
- Apply cyclic sort's swap rule, but explicitly guard the in-range check so out-of-range values (negatives, zeros, values `> n`) are correctly left untouched rather than causing an out-of-bounds swap attempt.

**Prove it:** test on `[3, 4, -1, 1]` (expected: 2), `[1, 2, 0]` (expected: 3), and `[7, 8, 9, 11, 12]` (expected: 1 — every value is out of range).

---

## Real-World Challenge — Detect Corrupted Sequential IDs

You have a batch of records that were supposed to be assigned sequential IDs from `1` to `n` (one record per ID, no gaps, no duplicates) by an upstream system, loaded into an array indexed by their arrival order (not necessarily their ID order). Due to a bug, some IDs may be duplicated and others missing.

**Task:**
1. Use cyclic sort to rearrange the array so IDs land at their expected slot.
2. Report ALL duplicated IDs and ALL missing IDs (not just one of each) in a single `O(n)` pass after sorting.
3. **Distributed-systems framing:** explain in writing why this in-place approach (versus building a hash set of seen IDs) matters when processing this check on a very large batch in a memory-constrained environment (e.g. a serverless function with a tight memory limit) — and under what circumstances (e.g. IDs sparse across a huge range) the assumption behind cyclic sort breaks down and you'd have to fall back to hashing after all.

---

## Bonus Challenge — Cyclic Sort vs. Hashing vs. Fast & Slow Pointers

Implement "Find the Duplicate Number" (a single duplicate in `[1, n]`, array length `n+1`, all other values appearing exactly once) three different ways:

1. **Cyclic sort** — swap to home index, in place, `O(n)` time, `O(1)` space, but mutates the input.
2. **Hash set** — track seen values, `O(n)` time, `O(n)` space, does not mutate the input.
3. **Fast & Slow Pointers** (Floyd's cycle detection, treating the array as an implicit linked list via `next = nums[current]`) — `O(n)` time, `O(1)` space, does not mutate the input.

For each, answer: does it mutate the input? What's the space cost? Then explain in a comment why approach 3 is usually preferred in production code over approach 1, even though both are `O(1)` space — what does approach 1 give up that approach 3 doesn't?

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
