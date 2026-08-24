# Merge Intervals — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "list of `[start, end]` ranges" signal that means "sort first, then sweep," and (2) correctly deciding, on every problem, *which key to sort by* — start (to build merged output) or end (to count groups/removals) — since picking the wrong one produces a plausible-looking but wrong answer.

> Rule of thumb for every exercise: before writing a single line, ask "do I need to know what the merged ranges *look like*, or do I just need a *count* (of groups, removals, or arrows)?" The former sorts by start; the latter sorts by end. If you cannot answer that question, you are not ready to write the sweep yet.

---

## Easy — Meeting Rooms

**LeetCode 252 — Meeting Rooms.**

Given an array of meeting time intervals `[[start1,end1], [start2,end2], ...]`, determine if a person could attend all meetings (i.e., no two meetings overlap).

**Constraints to notice:** you do not need to merge anything or count anything beyond a yes/no answer — this is the simplest possible use of the sort-and-sweep idea, one comparison away from the full merge sweep.

**Task:** sort the intervals by start time, then walk the sorted list once checking only whether each interval's start is `>= ` the previous interval's end. Return `false` the moment you find a violation.

**Think about:** why is checking only *adjacent* intervals in the sorted order sufficient, when there are far more than `n-1` possible pairs to compare in the unsorted input?

---

## Medium — Meeting Rooms II

**LeetCode 253 — Meeting Rooms II.**

Given an array of meeting time intervals, find the minimum number of conference rooms required.

**Task:** this is deliberately *not* solvable with the plain merge sweep in this module — the question asks for the maximum number of meetings happening **simultaneously** at any single instant, not just whether any two meetings overlap. Solve it either with a min-heap of currently-occupied rooms' end times (push a new room when the next meeting's start is before the heap's smallest end time; otherwise reuse the freed room), or with a sorted "start events (+1) and end events (-1)" sweep tracking a running counter.

**Think about:** why does the plain "sort by start, extend-or-flush a single running `current` interval" sweep from this module's README fail here? What specifically about "three meetings all overlapping at once" breaks the assumption that only *one* interval needs to be tracked at a time? (This is exactly the fork the Recognition Diagram calls out between Merge Intervals and Two Heaps/an event sweep.)

---

## Hard — Employee Free Time

**LeetCode 759 — Employee Free Time.**

Given a list of schedules, one schedule per employee (each a list of non-overlapping intervals sorted by start time for that employee), find the list of finite intervals representing common, positive-length free time for **all** employees, also sorted.

**Task:** flatten every employee's intervals into one big list, sort by start (the plain Merge Intervals sort key), run the standard merge sweep to get everyone's combined "busy" blocks, and then the **free time** is exactly the gaps between consecutive merged busy blocks.

**Think about:** why is it correct to flatten all employees' schedules together and merge them as if they were one person's calendar, rather than trying to compare each employee's schedule against every other employee's schedule pairwise? What does that flattening step cost you in complexity versus a naive pairwise cross-employee comparison?

---

## Real-World Challenge — Booking Conflict Detection with a Grace Period

You run a shared-resource booking service (think: conference rooms, or shared CI runners) where each booking is a `[start, end]` timestamp pair, but your business rule requires a **10-minute buffer** between any two bookings on the same resource (so back-to-back bookings that touch exactly at the boundary, like `[10:00,10:30]` and `[10:30,11:00]`, are actually a **conflict**, not a valid adjacency, because there is no time to reset the room).

**Task:**
1. Adapt the merge sweep from [code.cpp](code.cpp) so that two bookings are considered "overlapping" (and therefore must be merged/flagged) whenever `next.start < current.end + bufferMinutes`, not just `next.start <= current.end`.
2. Given a day's worth of bookings for one resource, output the list of merged "occupied-plus-buffer" blocks, so a scheduling UI can grey them out as unavailable.
3. Discuss: your buffer rule changes the overlap comparison but does not change the sort key (still sort by start). Explain, in your own words, why adding a buffer only affects the *comparison* inside the sweep and not the *sort* itself — what property of the sort would have to change for the buffer requirement to also require a different sort key?

---

## Bonus Challenge — Interval List Intersections

**LeetCode 986 — Interval List Intersections.**

Given two lists of closed, disjoint, and **sorted** intervals (each list individually non-overlapping and already sorted by start), return the list of intersections between the two lists (each intersection also as a closed interval).

**Task:** implement this with a **two-pointer walk across both lists simultaneously** (one pointer per list — not the single-list sweep this module's README describes), where at each step you compute the intersection of the two intervals currently pointed at (if any: `max(start1,start2) <= min(end1,end2)`), and then advance whichever list's current interval ends first (since it cannot possibly intersect anything further in the other list).

**Then, generalize in writing (no code required):** this problem uses *two* pointers into *two separate already-sorted* lists, rather than *one* pointer sweeping *one* list after a single sort. Compare this structurally to the merge step of merge sort (see the Two Pointers module's Real Interview/Production Examples section) and to this module's own sweep. Is "advance whichever side ends first" here the same underlying justification as "flush current and start a new one" in the plain Merge Intervals sweep, or a genuinely different argument? Justify your answer using the Architecture section of the [README](README.md).

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
