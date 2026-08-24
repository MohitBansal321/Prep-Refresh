# Merge Intervals


> **In one line:** sort by start time, then sweep once: keep extending the current interval while the next one overlaps, flush and start fresh the moment it doesn't.

```cpp
std::sort(intervals.begin(), intervals.end(),
          [](auto& a, auto& b) { return a.first < b.first; });

std::pair<int, int> current = intervals[0];
for (size_t i = 1; i < intervals.size(); ++i) {
  const auto& next = intervals[i];
  if (next.first <= current.second) {
    current.second = std::max(current.second, next.second);   // overlap: extend
  } else {
    merged.push_back(current);   // no overlap: flush, start a new "current"
    current = next;
  }
}
merged.push_back(current);   // the loop never flushes the final one — do it here
```

**O(n log n)** time (the sort dominates) · **O(n)** space for the output. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Sort a list of `[start, end]` intervals by start time and sweep through them exactly once, merging any that overlap — turning what looks like an all-pairs comparison problem into a single linear pass after one sort.

## Real Life Analogy

Think about **merging calendar meeting blocks into "busy" blocks**. Suppose you have five separate meetings on your calendar today, and some of them overlap (a 2:00-3:00 meeting and a 2:30-3:30 meeting, say, because you got double-booked). If someone asks "when am I actually busy today?", you don't want to list all five raw meetings — you want the *merged* busy blocks: one continuous 2:00-3:30 block, not two overlapping ones.

The natural way anyone does this by hand is to first **sort the meetings by start time**, then walk through them left to right, keeping a running "current busy block" in your head. Each new meeting either extends the block you're already tracking (if it starts before the current block ends) or starts a brand new block (if there's a gap). By the time you reach the end of the list, you have exactly the merged, non-overlapping busy blocks — and you never had to compare every meeting against every other meeting.

## Problem

### What engineering problem exists?

A large family of problems presents you with a list of `[start, end]` ranges and asks a question about **overlap, merging, insertion, or scheduling conflict**:

- **Calendar/meeting systems**: given a list of booked time slots, merge overlapping ones into busy blocks, or determine the minimum number of rooms needed to host all meetings without conflict.
- **Resource booking**: given reservations for a shared resource (a conference room, a rental car, a server time-slice), detect whether a new booking conflicts with an existing one.
- **CPU/IO scheduling**: given a set of process execution windows, merge overlapping windows to compute total busy time, or find idle gaps.
- **Genomic/interval data analysis**: merging overlapping genomic regions (e.g. gene annotations) that were reported by different sources.

> **Term: Interval.** A closed range `[start, end]` (inclusive on both ends throughout this module) representing "everything from `start` to `end`." Two intervals **overlap** if they share at least one point — including the boundary case where one interval's end equals another's start (`[1,3]` and `[3,5]` are treated as overlapping/touching, and merge into `[1,5]`).

The naive way to check whether any two intervals overlap is to compare every pair: for `n` intervals, that's `O(n^2)` comparisons.

### Why is this problem difficult?

- **All-pairs comparison is quadratic.** Without any structure imposed on the input, checking whether interval `i` overlaps interval `j` for every pair `(i, j)` costs `O(n^2)` — fine for a handful of intervals, unusable for thousands of calendar events or millions of genomic regions.
- **Overlap is not transitive in an obviously exploitable way at first glance.** `[1,3]` overlaps `[2,5]`, and `[2,5]` overlaps `[4,7]`, so all three end up in one merged block `[1,7]` — even though `[1,3]` and `[4,7]` do NOT directly overlap each other. Reasoning about "chains" of overlap correctly, without accidentally merging unrelated intervals or missing a real chain, is the crux of the pattern.
- **Insertion into an already-sorted list seems like it should be cheaper than a full re-sort-and-merge**, but figuring out exactly which existing intervals the new one touches (and only those) requires careful bucketing (see Architecture below) rather than an ad-hoc scan.

> **Term: Sweep.** A single linear pass over data that has been placed in a convenient order (here, sorted by start time), maintaining a small amount of running state (here, the "current" interval being grown) instead of re-examining every element against every other element.

### What happens if we ignore it?

- **Quadratic blowup on any real calendar or dataset.** A room-booking system with a few thousand reservations per day, checked pairwise, becomes a real performance problem at scale.
- **Incorrect scheduling decisions.** A missed overlap means double-booking a room; an incorrectly merged interval means reporting someone as "busy" during a gap when they were actually free.
- **Re-deriving the same sort-then-sweep logic from scratch for every new problem** (insert, merge, count conflicts, find minimum rooms) instead of recognizing they are all small variations on the same two building blocks.

## Why Not Other Approaches?

**"Compare every pair of intervals directly."** Correct, but `O(n^2)` — for `n` in the thousands or millions (genomic data, high-volume booking systems), this is far too slow. There's also no easy way to extend a pairwise check into "produce the final merged list" without additional bookkeeping.

**"Use a hash set / boolean array marking every covered point."** For intervals over small integer ranges this can work (mark every integer in `[start, end]` as covered, then read off contiguous runs), but it costs `O(range)` time and space rather than `O(n log n)` — if intervals span, say, `[0, 10^9]`, this is completely impractical regardless of how few intervals there actually are. It also doesn't generalize to non-integer or continuous-valued intervals (timestamps, floating-point ranges).

**"Skip the sort and just scan repeatedly until nothing changes (bubble-merge)."** You could repeatedly scan the list, merging any overlapping pair found, and repeat until a full pass produces no merges — but this can take `O(n)` passes in the worst case (each pass merging only one pair), giving `O(n^2)` again, and it's harder to reason about correctness than "sort once, then sweep."

**Tradeoff summary:** the one-time `O(n log n)` sort is what makes everything after it a simple `O(n)` linear sweep. Every alternative either stays quadratic, requires impractical extra space proportional to the value range, or reintroduces repeated re-scanning. Sorting by start time is the single move that makes the rest of the problem trivial.

## Solution

Sort all intervals by start time. Then walk through them once, left to right, maintaining exactly one **"current" interval** being actively grown:

- If the next interval's start is less than or equal to the current interval's end, they overlap (or touch) — extend the current interval's end to the larger of the two ends.
- Otherwise, the next interval starts strictly after the current one ends. Because the list is sorted by start time, **every subsequent interval will have an even larger start** — so the current interval can never be extended again. Flush it to the output and make the next interval the new "current."

That's the entire algorithm: one sort, one pass, one running variable.

## Architecture

The "participants" are:

1. **The unsorted (or, for insertion, already-sorted) input list of intervals.** Read-only aside from the initial sort.
2. **The sorted order (by start time).** This is what guarantees the sweep only ever needs to look one step ahead — once you've moved past an interval, nothing later in the sorted order can possibly connect back to something before it.
3. **The "current" interval being grown.** The only piece of mutable state the whole algorithm needs. It always represents the fully-merged interval built so far from everything examined up to this point.
4. **The output list.** Receives a flushed copy of "current" every time the sweep determines it can never grow again.

For **insertion into an already-sorted, non-overlapping list** (a common follow-up), the sweep is reframed as three buckets, processed in order without ever needing a re-sort:
1. Existing intervals that end strictly before the new interval starts — copy through unchanged.
2. Existing intervals that overlap the new interval — absorb each one by expanding the new interval's own bounds (`min` of starts, `max` of ends).
3. Existing intervals that start strictly after the new interval (now expanded) ends — copy through unchanged.

Because the input is already sorted, once the sweep leaves bucket 1 it is in bucket 2 until it leaves into bucket 3 — no interval can jump backward a bucket.

## Execution Flow

1. **Sort** all intervals by start time — `O(n log n)`.
2. **Initialize** "current" to the first (smallest-start) interval.
3. **For each subsequent interval:** if its start `<=` current's end, extend current's end to `max(current.end, next.end)`. Otherwise, push current to the output and set current to this next interval.
4. **After the loop**, push whatever "current" holds — the loop body only flushes when it finds a non-overlapping interval, so the very last interval is never flushed inside the loop itself.
5. **(Insertion variant)** Walk the already-sorted list once, classifying each existing interval into bucket 1 (before), bucket 2 (overlapping — absorbed into the new interval's bounds), or bucket 3 (after), emitting bucket 1 as-is, the absorbed new interval once at the bucket-2/3 boundary, then bucket 3 as-is.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the flowchart distinguishing Merge Intervals from Two Heaps (meeting-room-style counting problems) and Greedy (general sort-and-commit problems) based on what the problem is actually asking for.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the sort-then-sweep-and-merge-or-flush loop.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of the sweep merging a concrete list of overlapping intervals.

## Implementation

[code.cpp](code.cpp) is a generic, problem-agnostic template (not tied to one specific LeetCode problem), providing two functions:

- `mergeIntervals` — sorts an unordered list of intervals and merges all overlaps in a single sweep.
- `insertInterval` — given an already-sorted, non-overlapping list plus one new interval, splices it in and merges whatever it now touches, in one `O(n)` pass with no re-sort needed.

The worked, problem-specific solutions in [problems/](problems/) apply this exact sweep to four named LeetCode problems.

## Code Walkthrough

**`mergeIntervals(intervals)`** (in [code.cpp](code.cpp)). Sorts the input by start time, then walks it once maintaining a single `current` pair. On overlap (`next.first <= current.second`), extends `current.second` to the max of the two ends. On no overlap, flushes `current` to the output and starts a new `current` at `next`. Flushes the final `current` after the loop, since the loop only flushes when it *finds* a break — the very last interval never triggers that condition on its own.

**`insertInterval(sortedNonOverlapping, newInterval)`** (in [code.cpp](code.cpp)). Implements the three-bucket walk described in Architecture: copies through everything strictly before the new interval, absorbs everything that overlaps it by expanding its bounds, then copies through everything strictly after. Because the existing list is already sorted and non-overlapping, this never needs to re-sort or look backward.

**Files in [problems/](problems/).** Each is a complete, standalone solution to one named LeetCode problem, reusing the sort-then-sweep idea with a small variation per problem — merging (01), insertion (02), counting removals to eliminate all overlaps (03), and finding the minimum number of arrows needed to burst every balloon (04). See [problems/README.md](problems/README.md) for the full index.

## Advantages

- **Turns an `O(n^2)` all-pairs comparison into `O(n log n)`.** The one-time sort is what makes the sweep afterward linear.
- **Constant extra state.** The sweep only ever needs one "current" interval in memory — no auxiliary data structure beyond the output list itself.
- **The insertion variant avoids a full re-sort.** Because the existing list is already ordered, splicing in one new interval is a single `O(n)` pass, not `O(n log n)`.
- **The same sweep generalizes cleanly** to counting conflicts, finding minimum removals, and other interval-scheduling variants — see [problems/](problems/).

## Disadvantages

- **Requires a sort, so there's a hard `O(n log n)` floor** — this pattern can never be faster than that, even though the sweep itself is linear.
- **Doesn't handle true streaming/unsorted-online insertion cheaply.** If new intervals keep arriving in arbitrary order (not just "one new interval into an already-sorted list"), you either re-sort from scratch each time or need a different structure (a balanced tree or interval tree) to stay efficient.
- **Closed/inclusive endpoint semantics must be decided up front and applied consistently** — whether `[1,3]` and `[3,5]` count as "touching" (and thus mergeable) is a modeling decision that changes the comparison operator (`<=` vs `<`) throughout, and getting it inconsistent between the sort comparator and the merge check is a common source of subtle bugs.

## Tradeoffs

**What we gain:** replacing an `O(n^2)` pairwise comparison with an `O(n log n)` sort followed by an `O(n)` sweep — a genuine asymptotic improvement for any non-trivial `n`.

**What we lose:** the ability to handle intervals arriving in a truly unsorted, continuous stream without paying a sort (or an equivalent ordered-structure insertion cost) somewhere. For a single batch of intervals known up front, this is a non-issue; for a live system with intervals trickling in indefinitely, a different data structure (e.g. an interval tree, or a balanced BST keyed by start time) may be worth the added complexity.

## Complexity

**Time:** `O(n log n)` for the sort, `O(n)` for the sweep — dominated by the sort. The insertion variant on an already-sorted list is `O(n)` (no sort needed).

**Space:** `O(n)` for the output list (and, depending on the sort implementation, `O(log n)` to `O(n)` auxiliary space for the sort itself).

**Comparison to the brute force it replaces:**

| Operation | Brute force (all-pairs) | Sort + sweep |
|---|---|---|
| Merge all overlaps | `O(n^2)` | `O(n log n)` |
| Insert one interval into an already-sorted list | `O(n log n)` (re-sort) or `O(n^2)` (re-check all pairs) | `O(n)` (no re-sort needed) |

## Common Mistakes

- **Comparing `next.first` against `current.second` with the wrong operator.** Using `<` instead of `<=` (or vice versa) silently changes whether touching intervals (`[1,3]` and `[3,5]`) are treated as overlapping. This must match the problem's stated semantics (open vs. closed intervals) and must be applied *consistently* between the merge check and anywhere else intervals are compared.
- **Forgetting to sort first.** Applying the sweep logic to an unsorted list produces garbage — the entire correctness argument (`current` can never be extended again once a break is found) depends on every later interval having an even larger start.
- **Mutating the input list while iterating over it**, or holding a reference into a vector that then gets resized — always build a fresh output list rather than trying to merge in place unless you've deliberately reasoned through the aliasing.
- **Forgetting to flush the final "current" interval after the loop.** The loop body only flushes when it *finds* a non-overlapping next interval; the very last interval in the sorted list never triggers that condition on its own and must be pushed to the output explicitly after the loop ends.

## When To Use

- **Merging a list of possibly-overlapping ranges into disjoint blocks** (calendar busy-blocks, merged genomic regions, merged time-series gaps).
- **Inserting one new range into an already-sorted, non-overlapping list** without a full re-sort.
- **Counting how many intervals must be removed (or how many rooms are needed) to eliminate all conflicts** — direct extensions of the same sweep (see [problems/](problems/)).
- **Any problem whose input is explicitly described as a list of `[start, end]` pairs** and whose question involves overlap, merging, or scheduling.

## When NOT To Use

- **The problem needs the running median or another middle-order statistic as data streams in** — that's Two Heaps ([../../searching-sorting-patterns/two-heaps/](../../searching-sorting-patterns/two-heaps/)), not this pattern.
- **The problem is about making one irrevocable locally-optimal choice per step with a general proof obligation**, rather than specifically merging/sweeping ranges — that's the general Greedy pattern ([../../greedy-patterns/greedy/](../../greedy-patterns/greedy/)), which Merge Intervals is best understood as one concrete application of.
- **Intervals arrive continuously and unsorted, and you need efficient point/range queries at any time** — an interval tree or segment-tree-based structure is a better fit than re-sorting on every arrival.

## Real Interview/Production Examples

- **Calendar and meeting-room scheduling systems** (Google Calendar, Outlook-style booking) merge overlapping bookings into busy blocks and detect conflicts before confirming a new meeting.
- **Resource reservation systems** (shared conference rooms, rental fleets, cloud instance reservations) use the same overlap-detection logic to reject or accept new bookings against existing ones.
- **CPU/IO scheduling and log analysis** merge overlapping execution windows to compute total busy time or find idle gaps.
- **Genomic data pipelines** merge overlapping annotated regions reported by different sources into a canonical, non-overlapping set of regions.

## Where I Can Use This

Five realistic ideas for your own backend projects:

1. **A meeting-room booking API** that merges a user's existing bookings into busy blocks before checking whether a new requested slot conflicts.
2. **A rate-limit or maintenance-window scheduler** that merges overlapping planned-downtime windows across services into a single combined outage calendar.
3. **A log-based uptime calculator** that merges overlapping "service was up" intervals reconstructed from health-check timestamps to compute true total uptime (avoiding double-counting overlapping checks).
4. **A resource-allocation dashboard** that inserts a newly-requested reservation into an already-sorted list of existing ones, flagging exactly which existing reservations it would conflict with.
5. **A feature-flag rollout scheduler** that merges overlapping rollout windows for different flags targeting the same user segment, to detect unintended simultaneous exposure.

## Similar Patterns

- **Two Heaps** ([../../searching-sorting-patterns/two-heaps/](../../searching-sorting-patterns/two-heaps/)): also deals with intervals in "meeting-room" style problems (e.g. minimum rooms needed), but answers a *counting/streaming* question — how many meetings are simultaneously active at any point — using two heaps of start/end times rather than a single sort-and-sweep. Reach for Two Heaps when the question is fundamentally about a live "how many overlapping right now" count rather than producing a merged list.
- **Greedy** ([../../greedy-patterns/greedy/](../../greedy-patterns/greedy/)): Merge Intervals is itself a specific, provably-correct greedy strategy (sort by start, commit to extending or flushing). The general Greedy pattern covers other sort-and-commit problems (activity selection, jump games) that don't happen to be phrased as `[start, end]` ranges.

| Pattern | What it answers | Core mechanism |
|---|---|---|
| Merge Intervals | "What are the disjoint merged ranges?" | Sort by start, single sweep with one running interval |
| Two Heaps | "How many things are simultaneously active?" | Two heaps (or sorted start/end arrays) tracking concurrent count |
| Greedy (general) | "What's the locally-optimal irrevocable choice at each step?" | Sort by the right key, commit once per step |

## Interview Discussion

Experienced engineers treat the sort-then-sweep mechanism as trivial once understood, and instead probe whether you can correctly reason about **edge cases in the overlap condition** and **recognize the variations** (insertion without re-sorting, counting removals, minimum rooms) as the same underlying idea.

Common follow-up questions:
- *"Are touching intervals (`[1,3]` and `[3,5]`) considered overlapping?"* — expects recognizing this is a modeling decision (open vs. closed intervals) that must be applied consistently, and stating which convention you're using before writing the comparison.
- *"Can you insert a new interval into an already-sorted list without re-sorting the whole thing?"* — expects the three-bucket `O(n)` walk, not "just insert and re-sort."
- *"What if you need the minimum number of meeting rooms, not just the merged busy blocks?"* — expects recognizing this needs a different technique (Two Heaps, or a sorted start/end-times sweep with a counter), not a simple merge.
- *"What's the time complexity, and what's the actual bottleneck?"* — expects identifying the sort as the `O(n log n)` floor, with the sweep itself being `O(n)`.

Common misconceptions:
- "You can merge intervals without sorting if you're clever about it." You cannot, in general — the sort is precisely what guarantees the linear sweep's correctness.
- "Merging and counting overlaps at a point in time are the same problem." They're related but distinct — merging produces a final disjoint list; counting concurrent overlaps (minimum rooms) needs to track how many intervals are "active" at any given moment, which the simple current-interval sweep does not do.

## Summary

- Sort intervals by start time, then sweep once, maintaining a single "current" interval that either extends (on overlap) or flushes (on no overlap).
- The one-time `O(n log n)` sort is what makes the sweep afterward `O(n)` — the entire asymptotic win comes from sorting once instead of comparing every pair.
- Inserting one new interval into an already-sorted, non-overlapping list is a three-bucket `O(n)` walk that needs no re-sort.
- Touching endpoints (`[1,3]` + `[3,5]`) are a modeling decision (open vs. closed intervals) that must be applied consistently.
- Many "interval scheduling" variants (counting removals, minimum rooms, minimum arrows) are small extensions of the same sweep — not unrelated problems.

## Key Takeaways

1. Sort by start time first — this single step is what turns an `O(n^2)` pairwise comparison into an `O(n)` linear sweep.
2. The sweep needs only one piece of mutable state: the "current" interval being grown.
3. Overlap check: `next.start <= current.end` extends; otherwise flush and start fresh.
4. Don't forget to flush the final "current" interval after the loop ends.
5. Inserting into an already-sorted list is a three-bucket `O(n)` walk (before / absorbed / after) — no re-sort needed.
6. Whether touching endpoints count as overlapping is a modeling decision — apply it consistently.
7. Complexity is `O(n log n)` overall, dominated entirely by the sort; the sweep itself is `O(n)`.
8. Two Heaps answers a different question (concurrent count) — don't reach for Merge Intervals when you actually need "how many meetings overlap right now."
9. Merge Intervals is a specific, provably-correct instance of the general Greedy pattern.
10. Real production uses: calendar/booking systems, resource reservation conflict checks, uptime calculation from health-check logs.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — interval scheduling and greedy algorithm correctness proofs (exchange argument style reasoning applicable here).
- *The Algorithm Design Manual* — Steven Skiena — covers interval scheduling and related greedy techniques with practical framing.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — community-maintained classic algorithm implementations, including interval-merging style problems.

**Official Documentation**
- LeetCode — Merge Intervals (problem 56).
- LeetCode — Insert Interval (problem 57).
- LeetCode — Non-overlapping Intervals (problem 435).
- LeetCode — Minimum Number of Arrows to Burst Balloons (problem 452).

**Blog Articles**
- GeeksforGeeks — "Merging Intervals" explainer, a widely used walkthrough of the sort-then-sweep technique.
- Educative.io — "Grokking the Coding Interview," the Merge Intervals pattern chapter — one of the most widely referenced pattern-based framings of this exact technique.
