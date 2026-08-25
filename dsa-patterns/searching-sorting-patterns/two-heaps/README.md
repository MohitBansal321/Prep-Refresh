# Two Heaps


> **In one line:** split the stream into a max-heap of the lower half and a min-heap of the upper half, rebalancing sizes after every insert — the median is then O(1) to read off the top(s).

```cpp
void addNum(int num) {
  if (low_.empty() || num <= low_.top()) low_.push(num);   // route into the correct half
  else high_.push(num);

  if (low_.size() > high_.size() + 1) {        // rebalance: keep sizes within 1 of each other
    high_.push(low_.top()); low_.pop();
  } else if (high_.size() > low_.size()) {
    low_.push(high_.top()); high_.pop();
  }
}
// low_ is a MAX-heap (top = largest of the lower half); high_ is a MIN-heap
// (top = smallest of the upper half). Odd total: low_ holds the extra element.
```

**O(log n)** per insert · **O(1)** per median query · **O(n)** space. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Track a running median (or a similar "middle" order-statistic) of a growing set of numbers in O(log n) per insert, by splitting the data across two heaps that each hold one half of it.

## Real Life Analogy

Think of a dealer splitting a **deck of cards face-down into two piles on a table**, sorted by rank, and kept perfectly balanced in size: the pile on the left holds the lower-ranked half of every card dealt so far, the pile on the right holds the higher-ranked half. The dealer has one rule they never break: the highest card in the left pile is always visible on top, and the lowest card in the right pile is always visible on top. Whenever a new card arrives, the dealer glances at those two visible cards, decides which pile the new card belongs in (lower half or upper half), drops it there, and then — if one pile has grown two cards larger than the other — moves the top card of the bigger pile over to the smaller one to re-balance.

The dealer never needs to look at the whole deck to answer "what's the middle card?" They only ever look at the two cards sitting on top of the piles. If both piles are the same size, the median is the average of those two top cards; if one pile has one extra card, that pile's top card **is** the median. The expensive part — sorting the whole deck — never happens. Only the boundary between the two piles is maintained, one card at a time, as the deck grows.

Two Heaps is exactly that dealer's technique in code: a **max-heap** for the lower half (so its largest element — the boundary — is always on top) and a **min-heap** for the upper half (so its smallest element — the other boundary — is always on top), kept the same size or off-by-one at every step.

## Problem

### What engineering problem exists?

A stream of numbers is arriving continuously — sensor readings, request latencies, stock ticks, exam scores as they're graded — and at any point in time you need to answer: **"what is the median (or some other central order-statistic) of everything seen so far?"**

> **Term: Median.** The middle value of a sorted list. If the list has an odd count, it's the single middle element; if even, it's the average of the two middle elements. Unlike the mean (average), the median is not skewed by a handful of extreme outliers — one $50,000 salary in a list of $60,000 salaries barely moves the median, but it does move the mean.

> **Term: Order-statistic.** The k-th smallest (or largest) value in a collection. The median is the order-statistic at the middle position; "the 95th-percentile latency" is an order-statistic near the top.

The naive way to answer "what's the median right now" is: keep every number you've seen in a list, and whenever asked, **sort the whole list and read the middle element(s)**. That works, but it means every single query costs O(n log n) — and if a query happens after every insert (the streaming case), the whole process costs O(n² log n) for n inserts. For a monitoring system ingesting thousands of latency samples per second and computing a running median dashboard, that is not a theoretical concern — it is the difference between a dashboard that updates in real time and one that falls permanently behind.

### Why is this problem difficult?

- **The median isn't at a fixed position you can index into.** Unlike "give me the max" (trivial — track a running maximum) or "give me the sum" (trivial — a running total), the median's position **depends on how many elements exist and their relative order**, which changes with every insert. You cannot just remember one number and update it in O(1); you need enough structure to know what the middle *is* after every insertion.
- **A single sorted structure re-sorts too much on every insert.** Keeping everything in one sorted array means every insertion of a new number requires shifting up to n elements to keep the array sorted (or, if you use `std::sort` again, a full O(n log n) re-sort). Neither scales when inserts and queries interleave continuously.
- **You must maintain a *balance invariant*, not just an ordering.** It is not enough to keep numbers sorted somewhere — you specifically need to always know exactly where the "middle boundary" sits, and that boundary shifts by one position with almost every single insert. Getting the rebalancing rule wrong (letting one side grow too large, or shifting the wrong element across the boundary) silently produces a wrong median that still *looks* plausible.

### What happens if we ignore it?

- **Quadratic-ish overall cost on a streaming median.** Re-sorting on every insert turns what should be a real-time O(log n)-per-event operation into O(n log n) per event — at 10,000 events, that is roughly 10,000 × (10,000 × log 10,000) ≈ 1.3 billion operations for the full stream, versus roughly 10,000 × log(10,000) ≈ 130,000 for the heap-based approach. That is a four-to-five-order-of-magnitude difference, not a rounding error.
- **Backpressure and dropped data.** A monitoring pipeline that cannot compute its running median fast enough either falls behind (queries answer with stale data) or has to sample/drop incoming events to keep up — directly degrading the signal you built the dashboard to observe.
- **Wasted memory from repeated full copies.** Re-sorting an ever-growing array, or building a fresh sorted copy per query, allocates and discards memory proportional to n on every single query — memory churn that shows up as GC/allocator pressure in a long-running service.

## Solution

The core idea: split every number you've seen into exactly two groups, always keeping the split at (or one away from) the halfway point.

- **A max-heap holding the "lower half."** Every number in this heap is (in the sorted-order sense) less than or equal to every number in the other heap. Because it's a max-heap, its **top** element is the *largest* of the lower half — which is exactly the boundary value on the low side.
- **A min-heap holding the "upper half."** Every number here is greater than or equal to everything in the lower half. Its **top** element is the *smallest* of the upper half — the boundary value on the high side.

> **Term: Heap.** A tree-shaped data structure that keeps one specific element (the max, for a max-heap; the min, for a min-heap) always accessible at the root in O(1), while insertion and removal of that root cost O(log n). It does **not** keep everything else internally sorted — only the root property is guaranteed. This is exactly why it's cheaper than a fully sorted structure: you pay less because you're promising less (you only need the one boundary value fast, not a total order).

The rebalancing rule: after inserting a new number into whichever heap it belongs in, if the two heaps' sizes differ by more than one, move the top element of the larger heap across to the smaller heap. This keeps the sizes equal (even total count) or off by exactly one (odd total count) at all times.

Once that invariant holds:
- If the two heaps are the **same size**, the median is the average of both heaps' top elements (the true middle sits exactly between the two halves).
- If one heap has **exactly one more** element than the other, that larger heap's top element **is** the median outright (it is the single middle element of an odd-sized set).

No code yet — the key conceptual leap is realizing you never need to know the internal order of either half; you only ever need to know where the split sits, and a heap is precisely the structure that keeps one boundary value cheaply accessible without sorting anything else.

Step by step:

1. **Insert.** A new number arrives. If `low` is empty, or the number is less than or equal to `low`'s current top (its max), push it into `low`. Otherwise, push it into `high`.
2. **Rebalance.** Compare the sizes of `low` and `high`. If `low` has grown to hold two more elements than `high`, pop `low`'s top and push it into `high`. If `high` has grown to hold more elements than `low` (strictly more, since ties favor `low` holding the extra element by convention), pop `high`'s top and push it into `low`. After this step, the sizes differ by at most one, with `low` never smaller than `high`.
3. **Query median.** If `low` and `high` are the same size, return the average of `low.top()` and `high.top()`. If `low` has one more element than `high` (the odd-count case), return `low.top()` directly — no averaging needed.

Every insert costs O(log n) (one heap push, at most one heap pop-and-push during rebalancing). Every median query costs O(1) (reading up to two heap tops, no heap operation at all).

## Architecture

1. **Max-heap ("lower half"), conventionally named `low` or `left`.** Holds the smaller half of all numbers seen so far, ordered so its largest element sits at the top. Its invariant: every element in `low` is `<=` every element in `high`. Its responsibility: answer "what is the largest value among the smaller half?" in O(1).

2. **Min-heap ("upper half"), conventionally named `high` or `right`.** Holds the larger half of all numbers seen so far, ordered so its smallest element sits at the top. Its invariant: every element in `high` is `>=` every element in `low`. Its responsibility: answer "what is the smallest value among the larger half?" in O(1).

3. **The rebalancing rule (implicit participant).** Not a data structure itself, but the piece of logic that runs after every insert to restore the size invariant (`|low.size() - high.size()| <= 1`). Without it, one heap could grow unboundedly larger than the other, and the "top of the larger heap is the median" guarantee would break immediately.

4. **The insertion-routing decision.** Every new number must be placed into the *correct* heap on arrival (not just any heap, then fixed up later) — specifically, compare it against the current max of `low` (if `low` is non-empty) to decide whether it belongs in the lower or upper half before the size-rebalancing step even runs.

Responsibilities in one line each:
- **`low` (max-heap):** remembers the largest element of the smaller half.
- **`high` (min-heap):** remembers the smallest element of the larger half.
- **Rebalancing rule:** guarantees the two heaps never drift more than one element apart in size.
- **Insertion routing:** ensures every new element lands in the half it actually belongs to, so the size-based rebalancing move (top-of-larger to top-of-smaller) is also always a *value*-correct move, not just a size-correct one.

## Why Not Other Solutions?

**"Keep an unsorted list, sort it every time someone asks for the median."**
Simplest possible code, but each query costs O(n log n) — and if the median is queried after every insert (the realistic streaming case), total cost is O(n² log n). Correct, but it does not scale past toy input sizes; it is the "does it even finish in time" failure mode.

**"Keep a single sorted array (or `std::multiset`), insert new elements in sorted position, read the middle in O(1)."**
Reading the median is now O(1) — good — but **inserting** into a sorted `std::vector` while preserving order costs O(n) per insert (shifting elements). A `std::multiset` (a self-balancing tree) fixes the insert cost to O(log n), but you still need to *walk* to the middle element to read it, which costs O(n) in a tree without an order-statistics augmentation (indexed access), because a plain balanced BST does not know "how many elements are to my left" without extra bookkeeping. You could add that augmentation (an "order-statistics tree" / indexed skip list), but that is strictly more machinery than Two Heaps needs for exactly the same guarantee.

**"Recompute the median from scratch with a selection algorithm (e.g. quickselect) on every query."**
Quickselect finds the k-th order-statistic in expected O(n) time — better than a full sort, but still O(n) *per query*, and for a stream where queries interleave with inserts, that is O(n) work re-triggered every single time, with no reuse of work done on the previous query. Two Heaps instead pays for the "where is the boundary" bookkeeping incrementally, once per insert, and answers every query in O(1).

**"Use a single balanced BST / order-statistics tree keyed by value."**
This can be made to work (with subtree-size augmentation, giving true O(log n) insert and O(log n) median lookup), but it is considerably more code and conceptual overhead than two heaps for the same result. It is also solving a strictly harder problem than what's needed here — an order-statistics tree lets you find the k-th smallest for *any* k, not just the middle. If you only ever need the median (or a fixed split like "P50" of a fixed window), two heaps do it with two off-the-shelf `std::priority_queue`s and no augmented tree code at all.

**Net:** every alternative either pays O(n) somewhere per operation (shifting a sorted array, walking an unaugmented tree, or re-running quickselect), or reaches for more machinery than is needed (an order-statistics tree) to solve a narrower problem (just the median) than that machinery is built for. Two Heaps is the sweet spot specifically because it needs nothing more exotic than two `std::priority_queue`s, and it only ever tracks the one boundary that actually matters.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart distinguishing Two Heaps from Top K Elements and from a plain sorted-array-with-reinsertion approach, based on the signals in a problem statement.
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of insert -> rebalance -> query, showing exactly which heap each step touches and why.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of both heaps' contents as the numbers `5, 15, 1, 3, 8, 7, 9, 10` are inserted one at a time, showing the median after each insert.

## The Code

[code.cpp](code.cpp) provides a single, generic, reusable `MedianFinder` class built directly on `std::priority_queue`:

- `std::priority_queue<int>` for **`low`** (declared first, since `addNum`'s routing decision checks against it first: `low.empty() || num <= low.top()`) — this is a max-heap **by default** in C++ (the standard library's default comparator, `std::less<T>`, makes the *largest* element compare "highest priority" and sit at the top). By convention, when the total count is odd, `low` holds the extra element — which is why `findMedian` checks `low.size() > high.size()` (not `<`) to decide the odd-count branch.
- `std::priority_queue<int, std::vector<int>, std::greater<int>>` for **`high`** — passing `std::greater<int>` as the comparator inverts the ordering, making this a **min-heap** (the *smallest* element sits at the top). This explicit comparator is the one line of this class that is easy to get backwards (see Common Mistakes).

The class exposes exactly two public methods, matching LeetCode 295's contract directly:
- **`void addNum(int num)`** — first routes `num` into `low` or `high` based on the comparison against `low.top()` (or unconditionally into `low` if it's currently empty). Then rebalances: if `low` now holds two more elements than `high`, its top is popped and pushed into `high`; if `high` now holds strictly more elements than `low`, the symmetric move happens in reverse. This is the single insertion point that keeps both the *value* invariant and the *size* invariant true after every call.
- **`double findMedian()`** — reads (never pops) the top of one or both heaps depending on which is larger. If sizes are equal, returns `(low.top() + high.top()) / 2.0` (the `2.0` forces floating-point division rather than integer truncation). If `low` is larger by one, returns `static_cast<double>(low.top())` directly. This proves the entire payoff of the pattern: after paying O(log n) per insert, every query is O(1) — no heap operation, no scan, just reading up to two cached root values.

Templating on the element type is deliberately **not** done here (unlike the Two Pointers module's generic templates) because heap comparators and mixed-precision averaging (summing two `int`s and dividing by 2.0) are simplest and clearest when specialized to a concrete numeric type. `main()` exercises `addNum`/`findMedian` against several hand-checkable sequences — an odd-then-even count progression, a sequence requiring several rebalancing swaps, and a sequence including negative numbers and duplicate values — printing `[PASS]`/`[FAIL]` for each assertion.

## Tradeoffs

**What the two-heap split buys you**

- **O(log n) insert, O(1) median query.** After the one-time cost of building the two heaps as data arrives, every median query is free of any heap operation at all — just reading up to two roots.
- **No full re-sort, ever.** Unlike the sorted-array approach, no insertion ever shifts more than O(log n) worth of work (a single heap sift), regardless of how large the dataset has grown.
- **Naturally incremental / streaming-friendly.** The structure is built one element at a time as data arrives, matching exactly how real-world data (sensor readings, request latencies, live scores) actually shows up — no "batch it all up, then process" step required.
- **Small, fixed memory overhead per structure.** Two heaps store exactly the n elements seen so far (no duplicated copies, no auxiliary sorted array) — memory is O(n) total, same as any approach that must remember every value, but with no extra structural overhead.
- **Built from off-the-shelf structures.** `std::priority_queue` is standard-library, well-tested, and familiar — no custom balanced tree or augmented order-statistics structure needs to be written or trusted.

**What it costs you**

- **Only gives you the median (or a fixed split), not arbitrary percentiles cheaply.** The two-heap split point is fixed at "half of everything seen so far." If you need the 90th percentile as well as the median, this exact structure does not give it to you for free — you would need a fundamentally different (and far more awkward to keep balanced) split ratio, or a different structure entirely (a t-digest or an order-statistics tree that supports arbitrary-rank queries).
- **Two data structures to keep in sync.** Every insert must correctly route the new value into the right heap *and* correctly rebalance sizes — miss either step and the two invariants silently drift apart, producing a median that is subtly wrong rather than crashing loudly.
- **No support for deletion in the classic form.** `std::priority_queue` cannot efficiently remove an arbitrary element (only the root). A *sliding-window* median (old values must leave as new ones arrive — LeetCode 480) needs an added "lazy deletion" layer (tracking which values are logically removed and cleaning them off the top when encountered) — meaningfully more complexity than the plain streaming-median case.
- **No random access into either half.** You can read the boundary values in O(1), but you cannot ask "what is the 3rd-smallest value in the lower half" without popping through the heap (destroying it in the process).
- **Versus an order-statistics tree specifically:** you lose the ability to answer "what is the k-th smallest element" for an arbitrary k in O(log n) — two heaps only ever expose the one fixed boundary they were built to track. That extra machinery only pays for itself if you need arbitrary-rank queries, not just the median.

## Complexity

**Time:**
- `addNum` (insert): **O(log n)** — one heap push (O(log n)) plus, at most, one rebalancing pop-and-push pair (another O(log n)). Both are worst-case bounds, and they hold on every call, not just on average.
- `findMedian` (query): **O(1)** — reads at most two heap tops; no heap operation is performed.
- Processing a full stream of n numbers with a query after every insert: **O(n log n)** total — dramatically better than the O(n² log n) total cost of re-sorting on every query.

**Space:** **O(n)** — every number seen so far is stored in exactly one of the two heaps; no duplicate storage, no auxiliary sorted copy.

**Comparison to the brute force it replaces:**

| Approach | Per-insert cost | Per-query cost | n inserts + n queries (interleaved) |
|---|---|---|---|
| Re-sort on every query | O(1) | O(n log n) | O(n² log n) |
| Sorted array, shift-insert | O(n) | O(1) | O(n²) |
| Two Heaps | O(log n) | O(1) | O(n log n) |

## Common Mistakes

- **Forgetting to rebalance sizes after every single insert.** If rebalancing is skipped "just this once" (e.g. inside a conditional that is accidentally skippable), the size invariant silently drifts, and eventually `findMedian` reads a stale top from the wrong side — the bug does not crash, it just quietly returns a wrong median. *Avoid:* make rebalancing an unconditional step of `addNum`, never optional or conditionally skipped.
- **Off-by-one on which heap gets the extra element for odd counts.** You must pick a fixed convention (e.g. "`low` always holds the extra element when the total count is odd") and apply it consistently in *both* the rebalancing logic and the `findMedian` read logic. Mixing conventions — rebalancing to favor `low` but reading `findMedian` as if `high` might hold the extra — produces a median that is wrong specifically (and only) on odd-count inputs, which is exactly the kind of bug that slips past a test suite that happens to only test even-count cases. *Avoid:* write the convention down as a comment right next to both the rebalancing code and the query code, and test both odd and even total counts explicitly.
- **Using the wrong comparator direction for one of the heaps.** Writing `std::priority_queue<int>` for what should be the min-heap (`high`) instead of `std::priority_queue<int, std::vector<int>, std::greater<int>>` is a one-word typo that compiles cleanly and produces plausible-looking (but wrong) medians on the very first non-trivial input. *Avoid:* comment each heap declaration with which half it represents and which direction (max or min) it must be, and add an assertion or test case early that would catch the swap (e.g. insert a strictly increasing sequence and verify the median tracks the correct value at each step).
- **Routing a new number into the wrong heap on insert.** Comparing against the wrong heap's top (e.g. checking against `high.top()` instead of `low.top()` when deciding where a new number belongs) breaks the *value* invariant even if the *size* invariant stays correct — producing heaps that are correctly sized but contain the wrong elements. *Avoid:* always compare a new arrival against `low.top()` (the boundary on the low side) first, and only fall through to `high` when it fails that comparison or when `low` is empty.
- **Forgetting the empty-heap edge case on the very first insert.** Comparing against `low.top()` when `low` is empty is undefined behavior on a `std::priority_queue` (calling `.top()` on an empty queue). *Avoid:* explicitly check `low.empty()` before reading `low.top()` in the routing decision, and route the very first element into `low` unconditionally.

## When To Use

- You need the **running median** of a stream of numbers as they arrive, with queries interleaved with inserts (LeetCode 295 — Find Median from Data Stream, is the canonical example).
- You need a **fixed-position order-statistic near the middle** of a growing or sliding dataset — not an arbitrary percentile, specifically the middle (or a value defined relative to two "halves").
- You need to solve a problem that is naturally expressible as **"the smallest element of the top half must always be reachable, and the largest element of the bottom half must always be reachable"** — e.g. IPO-style problems where you repeatedly need "the highest-profit project among those currently affordable" (a max-heap of affordable projects) paired with "the cheapest not-yet-affordable project" (a min-heap by capital requirement), which is a close structural cousin of the median pattern even though it is not literally computing a median.
- You need a **sliding-window median** — the same two-heap core, extended with lazy deletion to handle elements leaving the window (LeetCode 480).

## When NOT To Use

- **You need arbitrary percentiles, not just the median.** If a monitoring system needs P50, P90, and P99 simultaneously, a fixed 50/50 heap split does not generalize cleanly — reach for a data sketch built for this (e.g. a t-digest or a quantile-estimation structure), or an order-statistics tree if you need exact (not approximate) arbitrary-rank queries.
- **You only need a running maximum, minimum, sum, or mean.** These have their own O(1)-per-update techniques (a single running variable) — reaching for two heaps here is solving an easy problem with unnecessary machinery.
- **The dataset is static and known up front, and you only need the median once.** A single `nth_element` (C++'s O(n) expected-time partial-sort / quickselect, `std::nth_element`) finds the median in one O(n) pass with no heap bookkeeping at all — simpler and just as fast for a one-shot query on a fixed array.
- **You need to efficiently remove arbitrary elements (not just the two boundary values) as they age out.** Plain `std::priority_queue` heaps cannot do this; you would need the lazy-deletion extension (or a different structure like an indexed/order-statistics tree) — worth explicitly considering whether that added complexity is justified before committing to two heaps.

## Where This Shows Up

Two Heaps (specifically, LeetCode 295 — Find Median from Data Stream) is a frequently asked interview question at companies including Amazon, Google, Bloomberg, and Two Sigma — precisely because it tests whether a candidate recognizes that "median of a stream" is a *fundamentally different* problem from "median of a fixed array," and whether they reach for the heap-balancing idea rather than a brute-force re-sort.

Beyond interviews, the same idea shows up directly in production systems, and maps onto realistic backend work:

- **Median/percentile latency tracking and real-time dashboards.** Observability tools that need a running median (or a fixed set of percentiles) of request latency, without storing and re-sorting every single latency sample, use structures built on this exact "maintain the boundary incrementally" idea — a `/metrics` endpoint reporting running median request latency, updated incrementally as requests complete, is a direct application. Full percentile-sketch libraries (like t-digest, used in some APM/observability backends) generalize the two-heap idea to arbitrary percentiles at the cost of exactness (they are approximate). The same access pattern powers dashboards showing "median order value in the last hour" or "median page-load time today," and a live leaderboard or exam-scoring system reporting the median score as results stream in.
- **Financial tick-data and pricing statistics.** Trading systems that compute a running median price (a more outlier-resistant summary statistic than a moving average) over a stream of ticks use the same incremental-balance idea, often combined with the sliding-window/lazy-deletion extension to age out old ticks — the same idea also tracks a median listing price in a pricing/inventory system as listings are added and removed throughout the day.
- **IPO-style project scheduling.** Picking the highest-profit project you can currently afford, using a min-heap of not-yet-affordable projects (by capital requirement) paired with a max-heap of currently affordable projects (by profit) — structurally the same "two heaps split by a moving boundary" idea as the median pattern, applied to a scheduling problem instead.
- **Sliding-window anomaly detection.** Flagging when the most recent value deviates far from the median of the last N samples, using the sliding-window-median extension (two heaps plus lazy deletion) to keep the window's median current in O(log n) per new sample.

## Similar Patterns

- **Top K Elements** ([../top-k-elements/](../top-k-elements/)): also uses one (or more) heaps, but for a *different* purpose — a single fixed-size heap of size K tracks the K largest/smallest/most-frequent elements overall, answering "what are the extreme K values," not "what is the middle value." Two Heaps splits the *entire* dataset roughly in half and tracks the boundary between the halves; Top K Elements keeps only a small, bounded window of K elements and discards everything outside it. If you find yourself needing "the top K" rather than "the middle one," you want Top K Elements, not Two Heaps.
- **Merge Intervals** ([../../array-string-patterns/merge-intervals/](../../array-string-patterns/merge-intervals/)): a different family of problems entirely (sorting intervals by start time and sweeping once), but the two patterns intersect in "meeting rooms" style scheduling problems: "how many meeting rooms are needed at once" can be solved by sorting intervals and sweeping (Merge Intervals' native technique) *or* by a min-heap of currently-occupied rooms' end times (a single-heap technique, structurally closer to Top K Elements than to Two Heaps, since it uses one heap, not two balanced against each other). The comparison is useful precisely because it is easy to over-apply "heaps" to every scheduling problem — Two Heaps' specific two-heap-balance structure is for tracking a *middle* value, not for counting overlaps or merging ranges.
- **Order-statistics tree (augmented balanced BST):** a strictly more powerful structure that answers "what is the k-th smallest element for *any* k" in O(log n), not just the median. Two Heaps is the right-sized tool when you only ever need the middle; an order-statistics tree is the right tool when you need arbitrary-rank queries.

| Pattern | Structure | What it answers | Handles arbitrary rank? |
|---|---|---|---|
| Two Heaps | Max-heap (lower half) + min-heap (upper half), balanced in size | "What is the median (or a similarly fixed middle statistic) right now?" | No — only the fixed middle split |
| Top K Elements | One fixed-size heap of size K | "What are the K largest/smallest/most-frequent elements?" | No — only the top/bottom K |
| Merge Intervals | Sorted intervals + a linear sweep (or a single heap of active end-times for the "min rooms" variant) | "Which intervals overlap, and how do I merge/count them?" | Not applicable — a different question shape entirely |
| Order-statistics tree | Augmented balanced BST (subtree sizes) | "What is the k-th smallest element, for any k?" | Yes |

## Interview Discussion

Experienced engineers do not spend interview time on "how do you get a max-heap out of `std::priority_queue`" — that's mechanical (know the default, know `std::greater<>`). What they actually probe is whether you can **state the two invariants precisely and explain why both are needed**: the *value* invariant (everything in `low` is `<=` everything in `high`) and the *size* invariant (`|low.size() - high.size()| <= 1`). A candidate who says "I keep two heaps roughly balanced" without being able to state exactly what "balanced" guarantees is reciting the pattern's shape without having understood its proof.

Common follow-up questions:
- *"What if the numbers are extremely skewed (e.g. mostly the same value repeated)?"* — expects recognizing that heap operations remain O(log n) regardless of value distribution (heaps do not degrade the way, say, an unbalanced BST built from sorted input would), because a binary heap's shape is determined purely by insertion count, not by value ordering.
- *"Extend this to a sliding-window median."* — expects recognizing the need for **lazy deletion**: since a `std::priority_queue` cannot remove an arbitrary element cheaply, you track "outgoing" values in a hash map and only actually pop them from a heap when they happen to surface at the top, cleaning up before every rebalance and every query.
- *"What's the time complexity if you called `findMedian` after every single `addNum`, for n numbers total?"* — expects O(n log n) total, and the ability to contrast that explicitly against the O(n² log n) of the re-sort-every-time approach.

Common misconceptions:
- "A heap keeps everything sorted internally." It does not — only the root property is guaranteed; that's precisely why heap operations are cheaper than maintaining a total order.
- "You can just use one heap and pop halfway when you need the median." A single heap only gives you fast access to *one* extreme (its root); finding "the middle" from one heap still requires popping roughly half its elements, which is neither fast nor non-destructive.
- "The size invariant only matters for the odd/even median formula, not for correctness of the values." It matters for both — if sizes drift, the *value* invariant (low half all `<=` high half) can also break, since rebalancing is also what moves values across the boundary when they landed in the "wrong" heap due to timing.

## Key Takeaways

1. Two Heaps tracks a running median in O(log n) per insert and O(1) per query, by splitting the dataset into a max-heap (lower half) and a min-heap (upper half).
2. Two invariants must both hold at all times: value ordering across the split, and size balance (differ by at most one) — the rebalancing step runs after **every** insert, unconditionally.
3. `std::priority_queue<int>` is a max-heap by default; `std::priority_queue<int, std::vector<int>, std::greater<int>>` is the min-heap — mixing these up is the most common implementation bug.
4. Pick and document a fixed convention for which heap holds the "extra" element on odd counts, and apply it consistently in both rebalancing and query code.
5. Complexity: O(log n) insert / O(1) query, versus O(n log n) per query for re-sort-on-demand or O(n) insert cost for a shift-insert sorted array (both trading the cost to the opposite side of the insert/query split).
6. The pattern only answers "what's the middle" — arbitrary percentiles need a different structure (t-digest, or an order-statistics tree), and it does not support removing an arbitrary aging-out element, which a sliding-window median (LeetCode 480) needs lazy deletion to work around.
7. Two Heaps is a structurally different tool from Top K Elements: middle value vs. extreme K values, balanced 50/50 vs. a fixed-size K.
8. Real production use: median/percentile latency tracking in monitoring systems, real-time analytics dashboards, and financial tick-data statistics — all cases that cannot afford to re-sort on every event.
9. Structurally the same "two heaps split by a moving boundary" idea also solves IPO-style scheduling (affordable-project max-heap paired with not-yet-affordable min-heap), not just the literal median.
10. A single, static, one-shot median query is better served by `std::nth_element` (quickselect) — Two Heaps earns its keep specifically when queries and inserts interleave over a growing stream.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — covers binary heaps and order statistics (including the general selection algorithm) in depth, the theoretical foundation this pattern builds on.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes a worked treatment of the online-median problem using two heaps, with C++-specific implementation notes.
- *Competitive Programmer's Handbook* — Antti Laaksonen — covers heaps and priority queues as a core data structure with practical usage patterns.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — includes the "median of a stream" problem as a heap-based exercise, alongside general interview framing.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including heap-based examples useful for seeing varied implementation styles.
- The C++ Standard Library's own `<queue>` header (`std::priority_queue`) — the production-grade heap implementation used directly throughout this module; reading libstdc++'s or libc++'s source for `std::priority_queue` and the underlying `<algorithm>` heap functions (`std::push_heap`, `std::pop_heap`) is a direct look at how a binary heap is actually implemented over a plain array.
- T-Digest reference implementation (e.g. `tdunning/t-digest` on GitHub) — a real-world, production-grade generalization of "track order statistics incrementally" to arbitrary approximate percentiles, worth comparing against the exact two-heap median approach.

**Official Documentation**
- LeetCode — Find Median from Data Stream (problem 295).
- LeetCode — Sliding Window Median (problem 480).
- LeetCode — IPO (problem 502).
- LeetCode — Kth Largest Element in a Stream (problem 703).
- cppreference.com — `std::priority_queue` — the standard library's heap adapter, with precise documentation of its default (max-heap) comparator and how to invert it with `std::greater<>`.

**Blog Articles**
- GeeksforGeeks — "Find Median from a stream of integers" — a widely used explainer of the two-heap technique with worked examples.
- NeetCode — "Find Median from Data Stream" walkthrough — a visual explanation of the insert/rebalance/query cycle.
- Educative.io — "Grokking the Coding Interview" Two Heaps pattern chapter — one of the most widely referenced pattern-based framings of this exact technique.
