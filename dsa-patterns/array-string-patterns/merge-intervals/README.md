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

Think about **merging calendar meeting blocks into "busy" blocks**. Suppose five separate meetings today overlap in places (a 2:00-3:00 meeting and a 2:30-3:30 meeting, say, from a double-booking). If someone asks "when am I actually busy today?", you don't want to list all five raw meetings — you want the *merged* busy blocks: one continuous 2:00-3:30 block, not two overlapping ones.

The natural way to do this by hand is to first **sort the meetings by start time**, then walk through them left to right, keeping a running "current busy block" in your head. Each new meeting either extends the block already being tracked (if it starts before that block ends) or starts a brand new block (if there's a gap). By the end of the list, you have exactly the merged, non-overlapping busy blocks — without ever comparing every meeting against every other meeting.

## Problem

### What engineering problem exists?

A large family of problems presents a list of `[start, end]` ranges and asks a question about **overlap, merging, insertion, or scheduling conflict**:

- **Calendar/meeting systems**: merge overlapping booked slots into busy blocks, or determine the minimum number of rooms needed to host all meetings without conflict.
- **Resource booking**: given reservations for a shared resource (a conference room, a rental car, a server time-slice), detect whether a new booking conflicts with an existing one.
- **CPU/IO scheduling**: given process execution windows, merge overlapping ones to compute total busy time, or find idle gaps.
- **Genomic/interval data analysis**: merging overlapping genomic regions reported by different sources.

> **Term: Interval.** A closed range `[start, end]` (inclusive on both ends throughout this module). Two intervals **overlap** if they share at least one point — including the boundary case where one's end equals another's start (`[1,3]` and `[3,5]` are treated as overlapping/touching, and merge into `[1,5]`).

The naive way to check whether any two intervals overlap is to compare every pair — `O(n^2)` comparisons for `n` intervals.

### Why is this problem difficult?

- **All-pairs comparison is quadratic.** Without structure imposed on the input, checking every pair `(i, j)` costs `O(n^2)` — fine for a handful of intervals, unusable for thousands of calendar events or millions of genomic regions.
- **Overlap is not transitive in an obviously exploitable way at first glance.** `[1,3]` overlaps `[2,5]`, and `[2,5]` overlaps `[4,7]`, so all three end up in one merged block `[1,7]` — even though `[1,3]` and `[4,7]` do NOT directly overlap each other. Reasoning about "chains" of overlap correctly, without accidentally merging unrelated intervals or missing a real chain, is the crux of the pattern.
- **Insertion into an already-sorted list seems like it should be cheaper than a full re-sort-and-merge**, but figuring out exactly which existing intervals the new one touches requires careful bucketing (see Architecture) rather than an ad-hoc scan.

> **Term: Sweep.** A single linear pass over data placed in a convenient order (here, sorted by start time), maintaining a small amount of running state (the "current" interval being grown) instead of re-examining every element against every other.

### What happens if we ignore it?

- **Quadratic blowup on any real calendar or dataset.** A room-booking system with a few thousand reservations per day, checked pairwise, becomes a real performance problem at scale.
- **Incorrect scheduling decisions.** A missed overlap means double-booking a room; an incorrectly merged interval means reporting someone as "busy" during a gap when they were actually free.
- **Re-deriving the same sort-then-sweep logic from scratch for every new problem** (insert, merge, count conflicts, find minimum rooms) instead of recognizing they are all small variations on the same two building blocks.

## Solution

Sort all intervals by start time. Then walk through them once, left to right, maintaining exactly one **"current" interval** being actively grown:

- If the next interval's start is less than or equal to the current interval's end, they overlap (or touch) — extend the current interval's end to the larger of the two ends.
- Otherwise, the next interval starts strictly after the current one ends. Because the list is sorted by start time, **every subsequent interval will have an even larger start** — so the current interval can never be extended again. Flush it to the output and make the next interval the new "current."

That's the entire algorithm: one sort, one pass, one running variable. After the loop, push whatever "current" still holds — the loop body only flushes when it *finds* a non-overlapping interval, so the last interval in the sorted list never triggers that condition on its own.

For **insertion into an already-sorted, non-overlapping list** (a common follow-up), the same sweep is reframed as three buckets, walked once with no re-sort needed: existing intervals ending strictly before the new interval starts (copied through unchanged); existing intervals overlapping the new interval (absorbed by expanding the new interval's own bounds — `min` of starts, `max` of ends); and existing intervals starting strictly after the (now expanded) new interval ends (copied through unchanged). Because the input is already sorted, once the sweep leaves bucket 1 it is in bucket 2 until it leaves into bucket 3 — no interval can jump backward a bucket.

## Architecture

The "participants" are:

1. **The unsorted (or, for insertion, already-sorted) input list of intervals.** Read-only aside from the initial sort.
2. **The sorted order (by start time).** This is what guarantees the sweep only ever needs to look one step ahead — once you've moved past an interval, nothing later in the sorted order can possibly connect back to something before it.
3. **The "current" interval being grown.** The only piece of mutable state the whole algorithm needs. It always represents the fully-merged interval built so far from everything examined up to this point.
4. **The output list.** Receives a flushed copy of "current" every time the sweep determines it can never grow again.

The insertion variant's three buckets (see Solution) reuse the same cast: the existing sorted list plays the role of the input, and the expanding "new interval" plays the role of "current."

## Why Not Other Approaches?

**"Compare every pair of intervals directly."** Correct, but `O(n^2)` — for `n` in the thousands or millions (genomic data, high-volume booking systems), far too slow, with no easy way to extend a pairwise check into "produce the final merged list" without additional bookkeeping.

**"Use a hash set / boolean array marking every covered point."** For intervals over small integer ranges this can work (mark every integer in `[start, end]` as covered, then read off contiguous runs), but it costs `O(range)` time and space rather than `O(n log n)` — if intervals span, say, `[0, 10^9]`, this is completely impractical regardless of how few intervals there actually are, and it doesn't generalize to non-integer or continuous-valued intervals (timestamps, floating-point ranges).

**"Skip the sort and just scan repeatedly until nothing changes (bubble-merge)."** Repeatedly scan the list, merging any overlapping pair found, until a full pass produces no merges — but this can take `O(n)` passes in the worst case (each merging only one pair), giving `O(n^2)` again, and it's harder to reason about correctness than "sort once, then sweep."

**Net:** the one-time `O(n log n)` sort is what makes everything after it a simple `O(n)` linear sweep. Every alternative either stays quadratic, requires impractical extra space proportional to the value range, or reintroduces repeated re-scanning. Sorting by start time is the single move that makes the rest of the problem trivial.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart distinguishing Merge Intervals from Two Heaps (meeting-room-style counting problems) and Greedy (general sort-and-commit problems) based on what the problem is actually asking for.
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of the sort-then-sweep-and-merge-or-flush loop.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of the sweep merging a concrete list of overlapping intervals.

## The Code

[code.cpp](code.cpp) is a generic, problem-agnostic template (not tied to one specific LeetCode problem), providing two functions: `mergeIntervals`, which sorts an unordered list and merges all overlaps in a single sweep; and `insertInterval`, which given an already-sorted, non-overlapping list plus one new interval, splices it in and merges whatever it now touches, in one `O(n)` pass with no re-sort.

**`mergeIntervals(intervals)`.** Sorts the input by start time, then walks it once maintaining a single `current` pair. On overlap (`next.first <= current.second`), extends `current.second` to the max of the two ends; on no overlap, flushes `current` and starts a new one at `next`. Flushes the final `current` after the loop, since the loop only flushes when it *finds* a break — the last interval never triggers that on its own.

**`insertInterval(sortedNonOverlapping, newInterval)`.** Implements the three-bucket walk from Architecture: copies through everything strictly before the new interval, absorbs everything overlapping it by expanding its bounds, then copies through everything strictly after. Because the existing list is already sorted and non-overlapping, this never needs to re-sort or look backward.

**Files in [problems/](problems/).** Each is a complete, standalone solution to one named LeetCode problem, reusing the sort-then-sweep idea with a small variation per problem — merging (01), insertion (02), counting removals to eliminate all overlaps (03), and finding the minimum number of arrows needed to burst every balloon (04). See [problems/README.md](problems/README.md) for the full index.

## Tradeoffs

**What Merge Intervals buys you**

- **Turns an `O(n^2)` all-pairs comparison into `O(n log n)`.** The one-time sort is what makes the sweep afterward linear.
- **Constant extra state.** The sweep only ever needs one "current" interval in memory — no auxiliary data structure beyond the output list itself.
- **The insertion variant avoids a full re-sort.** Because the existing list is already ordered, splicing in one new interval is a single `O(n)` pass, not `O(n log n)`.
- **The same sweep generalizes cleanly** to counting conflicts, finding minimum removals, and other interval-scheduling variants — see [problems/](problems/).

**What it costs you**

- **Requires a sort, so there's a hard `O(n log n)` floor** — this pattern can never be faster than that, even though the sweep itself is linear.
- **Doesn't handle true streaming/unsorted-online insertion cheaply.** If new intervals keep arriving in arbitrary order (not just "one new interval into an already-sorted list"), you either re-sort from scratch each time or need a different structure (a balanced tree or interval tree) to stay efficient.
- **Closed/inclusive endpoint semantics must be decided up front and applied consistently** — whether `[1,3]` and `[3,5]` count as "touching" (and thus mergeable) is a modeling decision that changes the comparison operator (`<=` vs `<`) throughout, and getting it inconsistent between the sort comparator and the merge check is a common source of subtle bugs.
- **For a live system with intervals trickling in indefinitely,** a different data structure (an interval tree, or a balanced BST keyed by start time) may be worth the added complexity over paying a sort (or equivalent ordered-structure insertion) on every arrival.

## Complexity

**Time:** `O(n log n)` for the sort, `O(n)` for the sweep — dominated by the sort. The insertion variant on an already-sorted list is `O(n)` (no sort needed).

**Space:** `O(n)` for the output list (and, depending on the sort implementation, `O(log n)` to `O(n)` auxiliary space for the sort itself).

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

## Where This Shows Up

Calendar and meeting-room scheduling systems (Google Calendar, Outlook-style booking) merge overlapping bookings into busy blocks and detect conflicts before confirming a new meeting — the same logic used by resource reservation systems (shared conference rooms, rental fleets, cloud instance reservations) to reject or accept new bookings against existing ones. CPU/IO scheduling and log analysis merge overlapping execution windows to compute total busy time or find idle gaps, and genomic data pipelines merge overlapping annotated regions reported by different sources into a canonical, non-overlapping set.

Concretely, for your own backend projects: a **meeting-room booking API** that merges a user's existing bookings into busy blocks before checking whether a new requested slot conflicts; a **maintenance-window scheduler** that merges overlapping planned-downtime windows across services into a single combined outage calendar; a **log-based uptime calculator** that merges overlapping "service was up" intervals reconstructed from health-check timestamps to compute true total uptime without double-counting; a **resource-allocation dashboard** that inserts a newly-requested reservation into an already-sorted list, flagging exactly which existing reservations it would conflict with; and a **feature-flag rollout scheduler** that merges overlapping rollout windows for different flags targeting the same user segment, to detect unintended simultaneous exposure.

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
- *"Are touching intervals (`[1,3]` and `[3,5]`) considered overlapping?"* — expects recognizing this is a modeling decision (open vs. closed intervals) that must be applied consistently, stated up front before writing the comparison.
- *"Can you insert a new interval into an already-sorted list without re-sorting the whole thing?"* — expects the three-bucket `O(n)` walk, not "just insert and re-sort."
- *"What if you need the minimum number of meeting rooms, not just the merged busy blocks?"* — expects recognizing this needs Two Heaps, or a sorted start/end-times sweep with a counter, not a simple merge.

Common misconceptions: that you can merge intervals without sorting if you're clever about it (you cannot, in general — the sort is precisely what guarantees the linear sweep's correctness); and that merging and counting overlaps at a point in time are the same problem (they're related but distinct — merging produces a final disjoint list, while counting concurrent overlaps needs to track how many intervals are "active" at any given moment, which the simple current-interval sweep does not do).

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
11. Counting removals, minimum rooms, and minimum arrows are all small extensions of the same sweep — not unrelated problems requiring separate techniques.

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
