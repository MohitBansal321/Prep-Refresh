# Top "K" Elements


> **In one line:** keep a heap of exactly size `k`, evicting the weakest candidate every time a new one arrives — a **min-heap** for \"top K largest\" (the weakest of the K is the smallest), inverted for \"top K smallest.\"

```cpp
std::priority_queue<T, std::vector<T>, std::greater<T>> minHeap;

for (const T& value : nums) {
  minHeap.push(value);
  if (static_cast<int>(minHeap.size()) > k) {
    minHeap.pop();   // evict the current smallest of the top-K-so-far
  }
}
// minHeap now holds exactly the K largest elements (unsorted among themselves).
```

**O(n log k)** time · **O(k)** space. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Find the K largest, K smallest, or K most-frequent elements out of a much bigger collection by maintaining a heap of exactly size K, instead of sorting everything.

## Real Life Analogy

Think of a **talent show judge** watching fifty contestants perform one after another. The judge only needs to announce the **top 3 scores** at the end of the night. A bad judge would write down every single score, wait until the last contestant finishes, sort all fifty scores from highest to lowest, and then read off the top three. A good judge does something much smarter: they keep a scratchpad with exactly **three slots**. After every performance, if the new score beats the *worst* of the three scores currently on the scratchpad, it replaces that worst score; otherwise the judge does not even bother writing it down. By the end of the night, the scratchpad holds the top 3 — without ever needing to remember, let alone rank, all fifty scores.

Notice what the judge tracks on that scratchpad: not the best score seen so far, but the **worst of the current best three** — because that is the only number that decides whether an incoming score is even worth considering. That detail is the entire engineering idea behind this pattern.

Another everyday version: a **content recommendation feed** that only shows you the top 10 trending posts. The system does not sort every post ever made; it keeps a small structure of exactly 10 "currently trending" candidates and swaps the weakest one out whenever a stronger candidate appears.

Top K Elements is that judge's scratchpad in code: a fixed-size structure that always knows its own weakest member, so it can decide in an instant whether a new arrival deserves a spot.

## Problem

### What engineering problem exists?

A large class of problems asks for a small, bounded answer out of a large input:

- **"What are the 10 largest values in this list of a million numbers?"**
- **"What are the 5 most frequently purchased products this hour, out of millions of orders?"**
- **"Which 3 points are closest to the user's current location, out of thousands of stored locations?"**

The naive instinct is: sort the entire collection, then take the first (or last) K elements. Sorting a list of size `n` costs **O(n log n)** — but you only actually *need* K values out of it, and in the realistic version of these problems, `K` is tiny (10, 20, 100) while `n` is enormous (millions). You are paying to fully order data you will mostly throw away.

> **Term: Heap.** A tree-shaped data structure (usually represented as an array) that keeps one specific element — either the minimum or the maximum, depending on the heap's type — instantly accessible at the root/top, in O(1) time. Inserting a new element or removing the top element both cost O(log n), because the structure only does enough reorganizing to preserve the "top is the min/max" guarantee, not full sorted order. C++'s `std::priority_queue` is a heap.

### Why is this problem difficult?

- **The natural instinct is "sort everything," which is exactly the waste this pattern eliminates.** Recognizing that you do not need a fully ordered result — only the top K, in whatever internal order a heap happens to produce — takes a shift in thinking the first few times you see it.
- **Deciding heap size is easy to get wrong by one.** The heap must hold *exactly* K elements once primed; too small and you lose correct candidates, too large and you defeat the entire memory/time benefit.
- **The "min-heap for largest K" inversion is genuinely counter-intuitive on first exposure.** Most engineers' first guess for "track the K largest values" is a max-heap, because "max" sounds like the right word for "largest." That guess is backwards — more on this in Solution below.

### What happens if we ignore it?

- **Wasted CPU on a full sort you do not need.** Sorting a million-element array to read off the top 10 costs O(n log n) ≈ 20 million comparisons when a size-10 heap would cost O(n log 10) ≈ 3.3 million — roughly a 6x difference, and the gap widens as `n` grows or K shrinks.
- **Unnecessary memory.** A full sort (or a sorted copy) holds all `n` elements in sorted form; a size-K heap holds only K elements at any time, which matters when `n` is large and the service is memory-constrained (a batch job processing millions of events, a request handler with a tight memory budget).
- **Worse behavior on streaming data.** If the data arrives incrementally (a live event stream, a series of incoming requests) rather than all at once, "sort everything, then take K" requires buffering the *entire* stream before you can answer anything. A size-K heap can report a correct, up-to-date top-K after every single new element, using bounded memory the whole time.

## Solution

The core idea: maintain a heap that holds **exactly K elements** at any point after the first K have been seen. As each new candidate arrives, compare it only against the *weakest* member currently held — the one sitting at the top of the heap — and decide in O(log k) whether it deserves a spot.

Here is the part that surprises almost everyone the first time: **to track the K *largest* elements, you use a MIN-heap of size K, not a max-heap.**

Why? Think through what question you actually need answered on every step: "is this new candidate strong enough to displace the *current weakest* member of my top-K?" The weakest member of a "top K largest" collection is its **smallest** element. A min-heap keeps its smallest element at the top in O(1) — exactly the value you need constant, instant access to. If you used a max-heap instead, its top would be the *largest* of your current top-K (the element you are *most* sure belongs in the answer, and the one you will never need to evict) — the wrong end of the heap for this job entirely. To find the weakest member in a max-heap you would have to scan all K elements, throwing away the whole point of using a heap.

So the rule is: **the heap type is inverted relative to what you are searching for.**
- Want the K **largest** elements → use a **min-heap** of size K (so the smallest-of-the-best is instantly evictable).
- Want the K **smallest** elements → use a **max-heap** of size K (so the largest-of-the-best is instantly evictable).
- Want the K **most frequent** elements → same min-heap-of-size-K shape, but ordered by *frequency* instead of raw value (compute frequencies first, then apply the identical push/evict rule).

The mechanism, once you accept the inversion, is a single repeated rule applied once per input element: **push the new element onto the heap; if the heap's size now exceeds K, pop.** That is it. There is no separate "should I even consider this element" check beforehand — every element gets pushed, and the heap itself, via the pop, decides whether it was worth keeping. This looks wasteful at first glance (why push something you might immediately discard?) but it is not: the push-then-maybe-pop sequence is exactly O(log k) either way, and it is simpler and less error-prone than writing a separate "is this candidate even competitive" comparison before deciding whether to push at all.

Step by step:

1. Decide the heap type based on what you are looking for: min-heap for "K largest," max-heap for "K smallest," min-heap-by-frequency for "K most frequent."
2. (Frequency variant only) Make one full pass over the input building a hash map from value to occurrence count — a separate O(n) step that happens *before* the heap pass begins.
3. For each element (or each distinct value, in the frequency variant): push it onto the heap, then if the heap's size is now greater than K, pop once — evicting the current weakest member. Repeat until every input element has been processed exactly once.
4. After the scan, the heap holds exactly the K best elements by whatever ordering key was chosen — but **not** in fully sorted order; a heap only guarantees the identity of its top element, not the relative order of everything beneath it.
5. If the problem needs the final K elements in sorted order (e.g., "return the top K sorted descending"), pop everything off into a list and sort that small list — an extra O(k log k) step, negligible because k is small, and strictly cheaper than sorting the original n-element input.

## Architecture

The participants in this pattern:

1. **The size-K heap.** The single data structure holding the current "best K seen so far." Its type (min vs. max) is chosen based on what "worst of the current best" means for this problem — see the inversion above. This is the only stateful structure in the whole pattern.

2. **The ordering key.** What the heap compares elements by. For "K largest/smallest," it is the raw value itself. For "K most frequent," it is a computed frequency (requiring a preliminary hash-map pass over the input to count occurrences before the heap pass begins). For problems like K Closest Points, it is a derived quantity (squared distance from the origin) rather than the raw data.

3. **The eviction rule ("push then pop-if-oversized").** After every push, check whether `heap.size() > K`. If so, pop once. This single check, run once per input element, is the entire control flow of the algorithm — there is no other branching logic needed.

4. **The input stream/source.** The (possibly very large) collection being scanned — an array, or genuinely a live stream, one element at a time. The pattern's O(k) space bound holds regardless of how large this source is, which is precisely why it also works on unbounded/streaming input where "sort everything first" cannot.

Responsibilities in one line each:
- **Size-K heap:** holds the current best-K candidates and always exposes the weakest one at O(1).
- **Ordering key:** defines what "weakest" and "best" mean for this specific problem.
- **Eviction rule:** the single mechanical step that keeps the heap at exactly size K.
- **Input source:** supplies candidates one at a time; the algorithm never needs to see more than one at once.

## Why Not Other Approaches?

**"Sort the whole array, then take the first/last K elements."**
Correct, and simple to write, but it costs **O(n log n)** time no matter how small K is. If `K` is 10 and `n` is a million, you did roughly 100,000x more ordering work than necessary — you fully resolved the relative order of every pair of elements, when the problem only ever asked "which K are the biggest," not "what is the exact rank of every single element." This is the single most common mistake engineers make on this problem shape: reaching for the tool they already know (`sort()`) instead of the tool sized to the actual question.

**"Scan the array K times, each time picking out the next-largest remaining element (selection sort style)."**
This avoids a full sort's log factor per element but costs **O(n·K)** overall (K passes, each O(n)) — worse than O(n log k) whenever K is more than a handful and n is large, and it still touches every element K times instead of once.

**"Use `std::nth_element` (quickselect) to partition around the K-th largest, then take everything past that partition."**
This is a legitimate, genuinely competitive alternative — average-case **O(n)** time, better than a heap's O(n log k) in the average case. Its downsides: worst-case O(n²) without care (though `std::nth_element` guards against this in practice), it needs the entire input materialized in memory up front (no streaming), and it gives you the K elements in **no particular order** with no easy way to maintain that top-K incrementally as new elements arrive. The heap-of-size-K approach trades a log(k) factor for the ability to process data as a single streaming pass and maintain a "top K so far" answer at every point in the stream — a real advantage whenever data arrives over time rather than sitting in memory all at once (log ingestion, live leaderboards, trending-content feeds).

**"Keep a plain sorted array/list of size K and insert new candidates with binary search + shift."**
Finding the insertion point is O(log k) via binary search, but *inserting* into a sorted array requires shifting up to K elements over — O(k) per insertion, O(n·k) overall. A heap does the equivalent work in O(log k) per insertion because it only needs to preserve the much weaker "root is the min/max" invariant, not full sorted order at every position.

**Net:** every alternative either pays for full ordering you do not need (sort), pays a worse multiplicative factor (K-pass selection, sorted-array insertion), or gives up the ability to process a stream incrementally (quickselect). A size-K heap is the only approach that gets O(n log k) time, O(k) space, *and* works correctly one element at a time on a live stream — that combination is its entire value proposition.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart deciding between Top K Elements, Two Heaps, and a full sort based on the signals in a problem statement.
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of the push-then-evict-if-oversized loop over the size-K heap.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of the size-K min-heap's contents as elements are processed one at a time, for a concrete "3 largest of `[3,1,5,12,2,11,9,7]`" example.

## The Code

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the *shape* of the pattern clearly, separated from any one problem's details, before looking at the worked, problem-specific solutions in [problems/](problems/). Templates (rather than hard-coded `int`/`vector<int>` signatures) are used deliberately so the value-keyed functions work over `vector<int>`, `vector<double>`, or any other comparable element type without rewriting the heap logic — the eviction rule is what matters, not the element type.

- **`topKLargest`** — the min-heap-of-size-K template for "K largest." Builds a `std::priority_queue<T, std::vector<T>, std::greater<T>>` — the `std::greater<T>` comparator is what flips the default max-heap into a min-heap. Pushes every element; after each push, if `size() > k`, pops once. After the scan, drains the heap into a vector and sorts it descending (an explicit, clearly-commented extra O(k log k) step) purely for convenient, predictable output — the heap logic itself never depended on that final order. Direct ancestor of [problems/01-kth-largest-element-in-an-array.cpp](problems/01-kth-largest-element-in-an-array.cpp).
- **`topKSmallest`** — the mirror-image max-heap-of-size-K template for "K smallest," with the default `std::priority_queue<T>` (already a max-heap, no comparator argument needed) and draining into an ascending-sorted vector. Exists purely to place the inversion side by side with `topKLargest` in one file, so the "opposite heap type for opposite question" rule is visible by direct comparison rather than only described in prose.
- **`topKFrequent`** — the frequency-keyed variant: first builds an `unordered_map<int,int>` counting occurrences (one O(n) pass), then pushes `(count, value)` pairs onto a min-heap ordered by `std::pair`'s natural lexicographic comparison (compares `.first`, i.e. frequency, before `.second`), evicting the lowest-frequency pair whenever the heap exceeds size K. Demonstrates that the *identical* push-then-evict rule applies when the ordering key is a derived quantity rather than the raw value itself. Direct ancestor of [problems/02-top-k-frequent-elements.cpp](problems/02-top-k-frequent-elements.cpp) and, with a custom tie-breaking comparator, [problems/04-top-k-frequent-words.cpp](problems/04-top-k-frequent-words.cpp).

**`main()`** exercises all three functions against small, hand-checkable inputs — including duplicate values, `k` larger than `n`, and a `double`-typed container to prove the templates are not hard-coded to `int` — and prints `[PASS]`/`[FAIL]` for each assertion.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem, implementing the same push-then-evict-if-oversized logic inline (not calling the generic templates directly, to keep each file dependency-free and independently readable), with problem-specific comments tying every decision back to the general principles established in this README. See [problems/README.md](problems/README.md) for the index and rationale.

## Tradeoffs

**What the size-K heap buys you**

- **Better time complexity than a full sort whenever K is meaningfully smaller than n.** O(n log k) versus O(n log n) — the gap grows as K shrinks relative to n, which is exactly the regime these problems live in (top 10 out of a million, not top 500,000 out of a million).
- **O(k) space, independent of n.** The heap never holds more than K elements, regardless of how large the input is.
- **Works on streaming/unbounded input.** Because the algorithm only ever needs one element at a time plus the current size-K heap, it can process a live event stream, a paginated API response, or a file too large to fit in memory, and produce a correct "top K so far" after every element.
- **Simple, uniform control flow.** "Push, then pop if oversized" is the entire loop body — no separate comparison-before-push logic to get subtly wrong.
- **Generalizes cleanly to derived ordering keys.** The exact same push/evict rule works whether the heap orders by raw value, by computed frequency, or by a derived distance — only the comparator changes, not the algorithm's shape.

**What it costs you**

- **Does not give you a fully sorted top-K for free.** A heap only guarantees the identity of its top element; the other K-1 elements are in heap-internal order, not sorted order. If the problem needs the answer sorted, that is an explicit extra O(k log k) step after the main scan.
- **Does not help once K approaches n.** As K gets close to n, O(n log k) approaches O(n log n) — the same cost as just sorting the whole array — so the pattern's benefit shrinks to nothing exactly when K stops being "much smaller than n." At that point, sorting everything is simpler code for the same asymptotic cost.
- **The min-heap-for-largest-K inversion is a real source of bugs**, not just an interview curiosity — see Common Mistakes below.
- **Heap operations have real constant-factor overhead per element** (pointer-chasing through the underlying array-backed tree, comparator calls) compared to a simple linear scan tracking a single running max/min — irrelevant for K > 1, but worth remembering that for the degenerate case K = 1, a single running variable beats a heap of size 1 in constant factor, even though both are technically O(n).
- **Versus quickselect (`std::nth_element`) specifically:** quickselect's average-case O(n) beats a heap's O(n log k) when the entire input is already materialized in memory and you need the answer only once, not incrementally — the price of a heap's incremental/streaming ability.

## Complexity

**Time:** O(n log k) — n elements, each processed with one push and (amortized) one possible pop, each heap operation costing O(log k) because the heap never holds more than K elements. Compare to a full sort's **O(n log n)**. The two expressions are identical in shape (`n log(something)`), but the "something" differs: k versus n. When k is a small constant (top 10) and n is large (millions), `log k` is a small constant (~3.3 for k=10) while `log n` keeps growing with the input (~20 for n=1,000,000) — the win is **not** a fixed multiplier, it *grows* as n grows with k held fixed, which is the realistic shape of "top K" problems in production (K is usually a fixed product requirement like "top 10 results," while n grows with your user base or data volume).

**Space:** O(k) for the heap itself, versus O(n) if you need a full sorted copy of the input (in-place sorts avoid this, but still cost O(n log n) time either way).

**Frequency variant:** add O(d) for the hash map, where d is the number of distinct values (d <= n) — this map exists only during the counting pass and is separate from the O(k) heap.

| Approach | Time | Space |
|---|---|---|
| Full sort, take first/last K | O(n log n) | O(n) (or O(1) extra for in-place sort, but still O(n log n) time) |
| Size-K heap (this pattern) | O(n log k) | O(k) |
| K-pass selection | O(n·k) | O(1) |
| Quickselect (`std::nth_element`) | O(n) average, O(n²) worst case | O(1) extra, needs full input in memory |

## Common Mistakes

- **Using a max-heap instead of a min-heap for "top K largest," and wondering why it is slow or wrong.** With a max-heap of unbounded size, you have no cheap way to know which element is the *weakest* of your current top-K, because the max-heap's top is the element you are *most* confident belongs in the answer — the opposite end from where you need to evict. This either forces an O(k) scan to find the minimum before every eviction decision (destroying the O(log k) benefit) or, if the size cap is forgotten entirely, silently degrades into holding the *whole* input in a heap. *Avoid:* before writing any code, say out loud "the heap's top must be the element I would evict first" — then pick min-heap or max-heap to match that sentence.
- **Forgetting to pop when the heap exceeds size K.** If you push every element but only conditionally pop (or forget the pop entirely), the heap silently grows to hold all n elements — you get a correct answer (the top of a full-size min-heap is still technically retrievable via extra work) but you have paid full O(n log n) cost and O(n) space, defeating the entire point of bounding the heap. *Avoid:* write the eviction check (`if (heap.size() > k) heap.pop();`) as an unconditional step immediately after every push, not as a special case.
- **Off-by-one on K itself.** Using `>= k` instead of `> k` in the eviction check evicts one element too early, leaving the heap at size K-1; using `k` where the problem means "K distinct values" versus "K elements including duplicates" produces a subtly wrong count. *Avoid:* explicitly state, in a comment, what "size K" means for this specific problem (K raw elements? K distinct values?) before writing the eviction condition.
- **Assuming heap order is sorted order.** Iterating a `std::priority_queue`'s underlying container directly (rather than popping) does not yield sorted output — only `top()` is guaranteed correct at any moment. *Avoid:* if sorted output is required, explicitly drain the heap via repeated `pop()` (or copy into a vector and sort it), and say so in a comment.
- **Reaching for this pattern when K is close to n.** If K is, say, 90% of n, O(n log k) is barely better than O(n log n) — the code complexity of a heap buys you almost nothing. *Avoid:* sanity-check that K is meaningfully smaller than n before reaching for a heap; if not, a plain sort is simpler and just as fast.
- **Forgetting the preliminary counting pass for frequency-based variants.** Trying to build a "top K frequent" heap directly from raw values (without first counting occurrences into a hash map) either double-counts or requires re-scanning to compute frequency on the fly, incorrectly. *Avoid:* always separate the two phases explicitly — count first (a full O(n) pass, hash map), then heap-scan the *distinct* values by their now-known frequency.

## When To Use

- You need the **K largest, K smallest, or K most-frequent** elements out of a much larger collection, and K is meaningfully smaller than n.
- The input may be a **live/streaming feed** (events, logs, incoming requests) and you need a correct "top K so far" answer at any point without buffering the entire stream.
- Memory is constrained relative to the size of the input, and you cannot afford to hold or sort the entire dataset at once.
- The ordering key is a **derived quantity** (frequency, distance, a computed score) rather than the raw value — the pattern generalizes cleanly by swapping the comparator.
- You are asked for a bounded "leaderboard" or "top N" answer that must be maintained incrementally as new data arrives (a live ranking, a trending-content list).

## When NOT To Use

- **You need the full sorted order of every element, not just the top K.** A size-K heap actively discards the ordering information for everything outside the top K; if you will need that later, sort everything up front instead of building a heap now and re-scanning later.
- **K is close to n.** The complexity win (O(n log k) vs. O(n log n)) evaporates, and a plain sort is simpler code for effectively the same cost.
- **The entire input already fits comfortably in memory and you need the answer exactly once (not incrementally).** Quickselect (`std::nth_element`) gets you the same K elements in average-case O(n), beating a heap's O(n log k), *if* you do not need streaming behavior.
- **You need the running median or another single "middle" order statistic of a growing stream**, not a top/bottom K — that is Two Heaps' job (a max-heap for the lower half plus a min-heap for the upper half working together), not a single size-K heap.

## Where This Shows Up

This is one of the most frequently asked coding-interview shapes at major tech companies (Amazon, Google, Meta, Microsoft) precisely because it tests whether a candidate reaches past "sort it" and can justify a bounded-heap alternative with the correct complexity argument — Kth Largest Element, Top K Frequent Elements, and K Closest Points to Origin are among the most commonly cited "everyone has seen this exact question" problems in interview-prep communities.

In production and in your own systems:

- **Top-K trending items / leaderboards.** Social platforms and e-commerce sites maintaining a "trending now" or "best sellers this hour" list, or a gaming/fitness-app "top 100 players this week" leaderboard out of millions of active users, run exactly this pattern over a stream of interaction events — or a persisted equivalent, like a Redis sorted set (`ZSET`), which offers the same "bounded top-N with cheap eviction" guarantee at the data-store level. The same idea builds a "top N slowest endpoints this hour" dashboard widget or a "most-active users" admin panel from a raw event stream, without buffering the full window in memory.
- **Top-K search and recommendation results.** A search engine or internal search service that scores many candidate documents but only needs to return the top 10-50 uses a bounded heap rather than sorting every scored candidate, especially when candidates are generated on the fly during an index scan. The same shape produces deduplicated top-K product recommendations from a model that scores thousands of candidates but only needs to return the top 20.
- **A rate-limiter's "top offenders" report** — tracking the K IP addresses or API keys with the highest request counts in a rolling window, using a size-K heap keyed by count so the report stays cheap to maintain under high request volume.
- **Nearest-neighbor prefiltering** — e.g. "show the 5 closest warehouses to this delivery address" before running a more expensive routing calculation, using a size-K max-heap keyed by distance, exactly as in K Closest Points to Origin.

## Similar Patterns

- **Two Heaps** ([../two-heaps/](../two-heaps/)): also uses heaps, but *two* of them (a max-heap for the smaller half, a min-heap for the larger half) working together to track a single **middle** order statistic (the running median) of a growing stream, not a bounded set of K extreme elements. Top K Elements answers "what are the K biggest/smallest/most-frequent"; Two Heaps answers "what is the value exactly in the middle right now." Structurally, Top K Elements uses one heap bounded to size K; Two Heaps uses two unbounded heaps balanced against each other in size.
- **K-way Merge** ([../k-way-merge/](../k-way-merge/)): also uses a heap, but seeded with exactly one element from each of K *already-sorted input lists*, repeatedly popping the smallest (or largest) and replacing it with the next element from the same source list — merging K sorted sequences into one, or finding the k-th smallest value *across* those lists. Top K Elements scans one (possibly unsorted) collection and bounds the heap to size K; K-way Merge scans K separate sorted collections with a heap bounded to size K (one slot per source list, not one slot per "answer" element) for an entirely different purpose (merging, not filtering to extremes).

| Pattern | Heap count | Heap size bound | Primary question answered |
|---|---|---|---|
| Top K Elements | One heap | Bounded to K | "What are the K largest/smallest/most-frequent elements?" |
| Two Heaps | Two heaps (balanced against each other) | Unbounded, kept balanced in size | "What is the median (or other middle order statistic) right now?" |
| K-way Merge | One heap | Bounded to K (one slot per source list) | "How do I merge K sorted lists, or find the k-th smallest across them?" |

## Interview Discussion

Experienced engineers do not spend interview time on "how do you use a `priority_queue`" — that is mechanical. What they actually probe is whether you can justify the **heap-type inversion** precisely: can you explain, without hesitating, why "K largest" wants a min-heap? A candidate who says "because the min-heap's top is the weakest member of my current top-K, and that is exactly the element I need instant access to for eviction" is demonstrating real understanding, not memorized code.

Common follow-up questions:
- *"What if the data arrives as a stream, not all at once?"* — expects recognizing that a size-K heap handles this naturally (bounded memory, correct "top K so far" at every point) while a full sort would need to buffer the entire stream first.
- *"Can you do better than O(n log k)?"* — expects mentioning quickselect's average-case O(n), along with the honest tradeoff: it needs the full input in memory and does not support incremental/streaming updates the way a heap does.
- *"How would you find the K most frequent elements instead of the K largest?"* — expects recognizing the identical heap mechanism applies, just keyed by a precomputed frequency instead of the raw value, and that this requires a preliminary counting pass.

Common misconceptions:
- "A max-heap is the natural choice for 'top K largest' because 'max' sounds right." This is the single most common wrong instinct — the heap type is *inverted* relative to the question, because you need instant access to the *weakest* member of the current top-K for eviction, not the strongest.
- "This is always strictly better than sorting." It is only better when K is meaningfully smaller than n; as K approaches n, the complexity advantage disappears.
- "You need a separate check before pushing to decide if a candidate is worth considering." You do not — push unconditionally, then pop if oversized; the heap's own eviction handles the "was this worth it" decision for you.

## Key Takeaways

1. Maintain a heap of exactly size K; push every element, pop whenever the heap exceeds size K — that single rule is the whole algorithm, in O(n log k) instead of a full O(n log n) sort.
2. For "K largest," use a **min-heap** — the inversion that surprises almost everyone the first time, because the weakest-of-the-best (the smallest) is what you need instant access to for eviction.
3. For "K smallest," use a **max-heap** — the mirror image of the rule above.
4. For "K most frequent," use the same min-heap shape, keyed by a precomputed frequency from a preliminary hash-map counting pass — that separate counting pass is easy to forget.
5. The heap does not produce sorted output on its own; sort the small final result separately if order matters (cheap, since k is small).
6. This pattern's benefit disappears as K approaches n — sanity-check that K is actually small before reaching for a heap; at that point a plain sort is simpler for the same cost.
7. Quickselect (`std::nth_element`) beats a heap's complexity on fully in-memory, one-shot inputs (average O(n) vs O(n log k)), but cannot process a stream incrementally the way a heap can.
8. Works cleanly on streaming/live input because it only ever needs one new element plus the current size-K heap at any moment — a full sort would need to buffer everything first.
9. Don't confuse this with Two Heaps (tracks a single middle order statistic, the median, via two balanced heaps) or K-way Merge (merges K already-sorted lists, one heap slot per source) — related "heap-based" shapes, different problems entirely.
10. One of the most frequently asked interview shapes precisely because it tests whether a candidate reaches past "sort it" for the correctly-sized tool — Kth Largest Element, Top K Frequent Elements, K Closest Points to Origin.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — the canonical treatment of heaps (binary heap structure, heapify, build-heap) and of selection algorithms (quickselect), both directly relevant background for this pattern.
- *Competitive Programmer's Handbook* — Antti Laaksonen — has a concise, practical treatment of priority queues and their use in "keep the best K seen so far" style problems.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes worked heap-based top-K problems with C++-specific implementation notes and complexity analysis.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — covers heaps and priority-queue-based problem shapes, including top-K style questions, with general interview framing.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including heap-based and priority-queue-based implementations useful for seeing varied styles.
- The C++ Standard Library's own `<queue>` header (`std::priority_queue`) — the production-grade heap implementation used throughout this module; reading libstdc++'s or libc++'s source for it is a direct look at a real, industrial binary-heap implementation.
- Redis (`redis/redis` on GitHub) — its **sorted set** (`ZSET`) data type, backed by a skip list, is the production data-store equivalent of "maintain a bounded, ordered top-N" at the persistence layer, directly relevant to the "leaderboard" and "trending items" real-world examples above.

**Official Documentation**
- LeetCode — Kth Largest Element in an Array (problem 215).
- LeetCode — Top K Frequent Elements (problem 347).
- LeetCode — K Closest Points to Origin (problem 973).
- LeetCode — Top K Frequent Words (problem 692).
- cppreference.com — `std::priority_queue` — precise documentation of the container adaptor, its comparator template parameter, and its complexity guarantees.
- cppreference.com — `std::nth_element` — the quickselect-based standard library alternative discussed above, with its complexity and guarantee documentation.
- Redis documentation — Sorted sets (`ZSET` commands) — the production data-structure analog for bounded top-N/leaderboard use cases.

**Blog Articles**
- GeeksforGeeks — "Heap Data Structure" and "K Largest(or Smallest) Elements in an Array" — widely used explainers covering the general technique and common problem shapes.
- NeetCode — Heap / Priority Queue pattern videos/playlist — walks through Kth Largest Element, Top K Frequent Elements, and K Closest Points to Origin with visual explanations.
- Educative.io — "Grokking the Coding Interview" Top K Elements pattern chapter — one of the most widely referenced pattern-based framings of this exact technique (the inspiration for organizing DSA study by pattern rather than by individual problem).
