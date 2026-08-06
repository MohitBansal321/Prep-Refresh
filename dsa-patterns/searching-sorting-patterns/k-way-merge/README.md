# K-way Merge

## Intent

Merge K already-sorted sequences into one fully sorted sequence — or find the k-th smallest element across all of them — in O(n log k) time, by using a min-heap of size K that always knows, in O(log k), which of the K current "front" candidates is smallest.

## Real Life Analogy

Picture a grading assistant at the end of a semester with **K stacks of graded exams**, one stack per teaching assistant. Each TA already sorted their own stack by score before handing it over — stack 1 is sorted, stack 2 is sorted, ..., stack K is sorted — but the stacks have never been combined with each other.

The assistant's job is to produce **one single stack**, sorted from lowest score to highest, containing every exam from every TA. The naive move would be to dump all the papers from all K stacks into one pile and re-sort the whole pile from scratch — but that throws away the fact that each individual stack was already in order.

Instead, the assistant does something smarter: they look at the **top paper of every stack** — just K papers, not all n of them — and pick whichever one has the lowest score. That paper goes onto the output stack. Now that stack's *new* top paper becomes a fresh candidate, and the assistant repeats: look at the current top of all K stacks (still only K comparisons' worth of "the smallest of these"), take the lowest, repeat. Each stack only ever exposes one candidate paper at a time, and the assistant never needs to look at a whole stack at once — just its current top.

A **min-heap** is exactly the data structure that answers "which of these K current candidates is smallest?" in O(log K) instead of O(K) — the assistant doesn't scan all K top papers by eye every time; they keep them in a small ordered tray (the heap) that hands back the smallest one instantly and cheaply re-sorts itself when a new paper is dropped in. That tray, holding one "current top" per stack, tagged with which stack it came from, is the entire mechanism behind K-way Merge.

## Problem

### What engineering problem exists?

You are given **K separate sequences that are each individually sorted**, and you need one of two things:

- **Merge them into a single sorted output** containing every element from every sequence, in overall sorted order.
- **Find the k-th smallest element** across all of them combined, without necessarily materializing the entire merged output.

> **Term: K-way merge.** The generalization of the familiar "merge two sorted arrays" step (from merge sort) to combining **K** sorted sequences at once, instead of exactly two.

The brute-force instinct is: **concatenate every list into one big array, then sort it.** If the K lists together hold `n` elements total, a comparison sort over all of them costs **O(n log n)**. That is correct, but it is wasteful — it completely ignores the fact that each of the K lists arrived *already sorted*. You are paying to re-discover ordering information you were handed for free.

> **Term: n vs. k in this pattern.** Throughout this module, `n` is the **total number of elements across all K lists combined** (not the size of any single list), and `k` (lowercase) is the **number of lists/sources** being merged — the two variables that show up in the O(n log k) complexity bound are deliberately different letters for a reason: one counts elements, the other counts sources.

### Why is this problem difficult?

- **The "just sort everything" instinct is strong and looks correct.** It produces the right answer, so many engineers stop there — the problem doesn't announce that it is throwing away free information, because concatenate-then-sort still works, it just works harder than it needs to.
- **You have to track "which list did this element come from" at all times.** Unlike a two-list merge (where you only ever need two pointers, `i` and `j`), a K-list merge needs K independent positions, one per source, and you must know which one to advance every time you emit a value. Losing track of the source index is the single most common implementation bug in this pattern.
- **The right data structure is not obvious the first time.** "Compare K current candidates and pick the smallest, repeatedly" sounds like it needs to scan all K of them every time — an O(K) operation per output element, giving O(n·K) overall. Recognizing that a min-heap turns that per-step cost into O(log K) is the specific insight this pattern teaches.

### What happens if we ignore it?

- **Wasted CPU on data that was already sorted.** Concatenating K lists of `n/K` elements each and running `std::sort` costs O(n log n). For K = 100 sorted server log files with a million lines total, that is roughly `10^6 * 20 ≈ 2×10^7` comparisons versus `10^6 * log2(100) ≈ 6.6×10^6` comparisons for K-way merge — over 3x more work for no benefit, and the gap widens as K grows relative to n.
- **Memory pressure from materializing everything at once.** "Concatenate then sort" typically requires holding all n elements in one contiguous structure before you can even begin sorting. K-way merge only ever needs O(K) elements resident in the heap at any moment, streaming the rest — critical when n is too large to fit in memory (see External Sorting under Real Interview/Production Examples).
- **Silent correctness bugs from ad-hoc merging.** Without the discipline of "one heap slot per source, always replenished from the same source," engineers often write nested loops that either drop elements (forgetting to re-check an exhausted list) or duplicate them (advancing the wrong list's pointer). The heap-based approach removes this class of bug by construction — there is exactly one place a new candidate can come from.

## Why Not Other Approaches?

**"Concatenate everything into one array, then sort it."**
Correct, but O(n log n) time and, depending on the sort implementation, O(n) extra space for the concatenated buffer. It is *strictly dominated* by K-way merge whenever K < n (which is essentially always true — you rarely have as many source lists as you have total elements). The entire value the sortedness of each individual list offered you is discarded and paid for again from scratch.

**"Merge two lists at a time, repeated K-1 times" (pairwise/sequential merging).**
This is the natural first instinct once you remember merge sort's two-list merge step: merge list 1 and list 2 into a combined result, then merge that combined result with list 3, then with list 4, and so on, K-1 times total. Each individual merge is linear in the size of its two inputs, so this looks like "just do the merge-sort merge step K-1 times" — but the accumulated result keeps growing. If the K lists are each roughly `n/K` elements, the first merge costs O(2n/K), the second costs O(3n/K) (merging the growing accumulator against the next list), and so on up to the final merge costing close to O(n). Summed across all K-1 merges, the total work is **O(n·K)** in the worst arrangement — for K = 1000 lists, that is a thousand-fold worse constant than the O(n log k) ≈ O(n · 10) that a heap-based merge achieves for the same K. The mechanical idea (advance whichever side is smaller) is exactly right; the mistake is applying it sequentially over K-1 rounds instead of holding all K candidates in one structure simultaneously.

**"Round-robin scan: check all K lists' current fronts every step, in a simple loop (no heap)."**
This is a valid *correctness* strategy — it produces the right merged output — but each step does a full O(K) linear scan over the K current candidates to find the minimum, and there are n steps total (one per output element), giving **O(n·K)** overall, same complexity class as pairwise merging and for the same reason: you are re-discovering "which of these K values is smallest" from scratch every single time instead of maintaining that answer incrementally. A min-heap is precisely the structure that maintains "what's currently smallest" across insertions/removals in O(log K) instead of O(K).

**Tradeoff summary:** every alternative either discards the fact that each list is pre-sorted (concatenate + sort), or re-derives "which of K candidates is smallest" the hard way on every step (pairwise merging, round-robin scan) — both land at a worse complexity than necessary once K is nontrivially large. K-way merge wins specifically because a min-heap turns "find the smallest of K current candidates" into an O(log K) operation instead of an O(K) one, and that is the *entire* asymptotic gain: O(n·K) → O(n log k).

## Solution

The core idea: maintain a **min-heap that always holds exactly one "current candidate" element per still-active source list**, tagged with enough information to know which list (and which position in that list) it came from. Repeatedly take the smallest candidate out of the heap, emit it to the output, and immediately replace it with the *next* element from the **same source list** it came from (if that list has more elements left).

Think of the heap as a small tournament bracket with at most K competitors at any time — one representative per source. Every time a representative "wins" (is the current smallest) and steps out to be recorded in the output, their team sends in their next player to take their place in the bracket. The bracket never needs to hold more than K competitors, no matter how large n is, because a team only ever has one player in the bracket at a time.

The key realization that makes this correct: because each individual list is already sorted, the *only* candidate from list `i` that could possibly be the next-smallest element overall is the one currently sitting at the front of list `i` — every element behind it in that same list is guaranteed to be `>=` the front element, so it can never be smaller than something else currently in the heap. That means you never need to look further than "the current front of each list" to be certain you're not missing a smaller candidate anywhere.

## Architecture

1. **The min-heap (priority queue).** Holds at most K entries at any moment — one "current candidate" per source list that still has unconsumed elements. Each entry is a tuple: `(value, list_index, element_index)`. The heap's ordering is entirely driven by `value` (the actual data being merged); `list_index` and `element_index` ride along purely as bookkeeping so that, once this entry is popped, the algorithm knows exactly where to fetch that source's *next* candidate from.

2. **The `list_index` field.** Identifies *which* of the K source lists this heap entry came from. Without it, popping the minimum value tells you the value, but not which list to advance — you would have no way to know where to find the next candidate to refill the heap.

3. **The `element_index` field.** Identifies *where within that source list* this entry sits. Advancing means checking `element_index + 1` against that list's length and, if there is a next element, pushing `(list[element_index + 1], list_index, element_index + 1)` onto the heap.

4. **The "advance the source pointer" step.** This is the operation performed immediately after every pop: look up which list the popped entry came from, check whether that list has a next element, and if so push it onto the heap as the new candidate representing that list. If the list is now exhausted, nothing is pushed for it — the heap simply has one fewer active candidate from then on, and the algorithm continues with the remaining K-1 (or fewer) sources.

5. **The output sequence (or the k-th-smallest counter).** For the "merge everything" variant, every popped value is appended, in order, to a growing output list — since values are always popped in non-decreasing order, the output is sorted by construction, with no separate sort step ever required. For the "find the k-th smallest" variant, there is no output list at all — you simply count pops, and the value popped on the k-th pop *is* the answer, letting you stop early without materializing the rest.

Responsibilities in one line each:
- **Min-heap:** always answers "which of the currently active candidates is smallest?" in O(log K).
- **`list_index` / `element_index`:** bookkeeping that lets a popped value be traced back to its exact source and position, so the correct replacement can be pushed.
- **Advance step:** the mechanism that keeps the heap "topped up" with exactly one fresh candidate per still-active list after every pop.

## Execution Flow

1. Given K sorted lists, create an empty min-heap of `(value, list_index, element_index)` tuples.
2. For each list `i` from `0` to `K-1`: if list `i` is non-empty, push `(list[i][0], i, 0)` onto the heap — its first element, tagged with its own index and position 0. (Skip empty lists entirely; they contribute nothing.)
3. While the heap is not empty:
   a. Pop the smallest tuple `(value, list_index, element_index)` from the heap.
   b. Append `value` to the output sequence (or, for the k-th-smallest variant, increment a pop counter and return immediately if this is the k-th pop).
   c. Check whether `list_index` has a next element, i.e. whether `element_index + 1 < length(list[list_index])`.
   d. If it does, push `(list[list_index][element_index + 1], list_index, element_index + 1)` onto the heap — this replaces the just-popped entry with the next candidate from the *same* source.
   e. If it does not (that list is now exhausted), push nothing for that list; the heap simply carries one fewer active candidate going forward.
4. When the heap becomes empty, every element from every list has been popped exactly once, in fully sorted order — the output sequence is the complete merged result.
5. (K-th-smallest variant only) If the loop reaches the k-th pop before the heap empties, stop immediately and return that value — there is no need to keep merging past the point you already have the answer.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full flowchart distinguishing K-way Merge from Top K Elements and from a plain "concatenate and sort," based on the signals in a problem statement.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the seed-heap-then-pop-and-replace loop described above.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of the heap's exact contents while merging three small concrete sorted lists.

## Implementation

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the *shape* of the pattern clearly, separated from any one problem's details, before looking at the worked, problem-specific solutions in [problems/](problems/).

It provides two function templates over `std::vector<std::vector<int>>`, deliberately using plain `int` values and `std::vector<std::vector<int>>` (rather than a fully generic template) to keep the heap tuple's meaning ("value, which list, which position") immediately readable — the four worked problems in `problems/` show the same idea adapted to linked lists, a matrix, and pair-sums:

- `mergeKSortedLists` — seeds a min-heap of `{value, listIdx, elemIdx}` tuples with the first element of every non-empty input list, then repeatedly pops the smallest, appends it to the result, and pushes the next element from the same source list if one exists. Returns the fully merged, sorted vector.
- `kthSmallestInKSortedLists` — identical seeding and replenishment logic, but stops as soon as the k-th value has been popped, returning it directly instead of building the whole merged output — the early-exit variant described in Execution Flow step 5.

## Code Walkthrough

**`mergeKSortedLists`** (in [code.cpp](code.cpp)). Takes `std::vector<std::vector<int>>& lists` (each inner vector already sorted ascending). Builds a `std::priority_queue<std::tuple<int,int,int>, std::vector<std::tuple<int,int,int>>, std::greater<>>` — the `std::greater<>` comparator is what turns `std::priority_queue` (a *max*-heap by default) into a *min*-heap, so the top of the queue is always the smallest tuple. Because `std::tuple`'s comparison operators compare element-by-element in declaration order, ordering by `{value, listIdx, elemIdx}` means the heap orders primarily by `value` — exactly what merging needs — and only falls back to `listIdx`/`elemIdx` to break ties between equal values, which never changes correctness (any tie-break order among equal values is a valid sorted output). The seeding loop pushes `(lists[i][0], i, 0)` for every non-empty list. The main loop pops the top tuple, appends its value to `result`, and pushes `(lists[listIdx][elemIdx + 1], listIdx, elemIdx + 1)` whenever `elemIdx + 1` is still in bounds for that list. This function exists to show the "merge everything" variant in its purest, most generic form — the direct ancestor of [problems/01-merge-k-sorted-lists.cpp](problems/01-merge-k-sorted-lists.cpp) and [problems/02-kth-smallest-element-in-a-sorted-matrix.cpp](problems/02-kth-smallest-element-in-a-sorted-matrix.cpp).

**`kthSmallestInKSortedLists`** (in [code.cpp](code.cpp)). Identical heap setup and replenishment logic to `mergeKSortedLists`, but instead of accumulating a result vector, it keeps a `popped_count` and returns the value of the tuple popped when `popped_count == k`. This function exists to demonstrate the early-exit optimization: when you only need the k-th smallest (not the full merge), you can stop as soon as you have it, which matters when `k` is much smaller than the total element count `n` — you do not pay for merging the remaining `n - k` elements you were never going to look at.

**`main()`** (in [code.cpp](code.cpp)). Exercises both functions against small, hand-checkable inputs — including lists of uneven length and a list that runs out early — and prints `[PASS]`/`[FAIL]` for each assertion, proving the template compiles and runs correctly end to end.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem — not using the generic templates above directly (to keep each file dependency-free and independently readable), but implementing the *same* heap-based logic inline, with problem-specific comments tying every decision back to the general principles established in this README. See [problems/README.md](problems/README.md) for the index. Briefly: `01` is the pure "merge everything" variant applied to linked lists instead of vectors; `02` recognizes that a row-sorted (and column-sorted) matrix is K sorted rows in disguise; `03` treats each element of one array, paired against the other's ascending elements, as its own implicit sorted "list"; `04` is the hardest variant, tracking a running maximum across the heap's current candidates to find the smallest range that touches every list.

## Advantages

- **Exploits information you already have.** Each list's existing sort order is used directly instead of being thrown away and re-derived by a general-purpose sort.
- **O(n log k) instead of O(n log n) or O(n·k).** Strictly better than concatenate-and-sort whenever K < n (virtually always), and strictly better than pairwise/round-robin merging whenever K is more than a small constant.
- **Bounded auxiliary memory.** The heap never holds more than K elements at once, regardless of how large `n` is — critical for merging data too large to fit entirely in memory (see External Sorting below).
- **Naturally supports early exit.** Finding just the k-th smallest element does not require materializing the full merged sequence — stop as soon as you have popped k values.
- **Generalizes cleanly past two lists.** Unlike a hand-written two-pointer merge (which only knows how to compare exactly two candidates), the heap-based approach scales to any K with the same code shape, just a bigger heap.

## Disadvantages

- **Heap overhead is not worth it for very small K.** For K = 2 or K = 3, a plain multi-pointer merge (compare 2 or 3 values directly, no heap) is simpler to write, has less constant-factor overhead (no heap push/pop bookkeeping), and is just as fast in practice — the log K heap advantage is meaningless when K itself is tiny.
- **Requires careful, explicit bookkeeping of source and position.** Every heap entry must carry enough information (`list_index`, `element_index`) to know where its replacement comes from; losing or mis-tracking this is the most common source of bugs in this pattern (see Common Mistakes).
- **Not a plain array-of-values heap — it needs tuples or custom comparators.** This adds a small amount of ceremony compared to a simple `std::priority_queue<int>`, and gets worse for problems where you need to compare by something other than raw value (e.g. LeetCode 632's running max, which needs auxiliary tracked state alongside the heap).
- **Heap operations are O(log K) each, not O(1).** For extremely performance-sensitive code merging a small, fixed number of sources repeatedly (e.g. a hot loop merging exactly 2 buffers millions of times), a specialized non-heap merge can outperform the general heap-based approach.

## Tradeoffs

**What we gain versus concatenate-and-sort:** the same correct, fully sorted output, but in O(n log k) instead of O(n log n) — a real asymptotic win whenever K is meaningfully smaller than n (the near-universal case), plus bounded O(K) auxiliary memory instead of needing all n elements materialized together before sorting can even begin.

**What we gain versus pairwise/round-robin merging:** the same O(n log k) time instead of O(n·k), because the heap answers "which candidate is smallest right now" in O(log K) instead of re-scanning all K candidates from scratch on every output element.

**What we lose:** implementation simplicity for the small-K case (a hand-written 2- or 3-way pointer merge is easier to read and just as fast when K is tiny), and a small constant-factor cost for maintaining the heap's tuples versus a flatter structure. There is no complexity regression versus any alternative — K-way merge is never asymptotically worse than the approaches it replaces, only occasionally not worth the extra bookkeeping when K is small.

## Complexity

**Time:**
- Seeding the heap: **O(K log K)** — pushing K initial elements, each an O(log K) heap operation (the heap never exceeds size K at seeding time).
- Main loop: **O(n log k)** — there are exactly `n` pops (one per element across all lists) and at most `n` pushes (each pop is immediately followed by at most one push), and every heap operation on a heap of size at most K costs O(log k). Total: O(n log k), dominating the O(K log K) seeding cost since `n >= K` whenever every list has at least one element.
- Contrast with concatenate-then-sort: **O(n log n)** — worse whenever `k < n`, which is virtually always true (you would need almost as many source lists as total elements for the two bounds to converge).
- Contrast with pairwise/round-robin merging: **O(n·k)** in the worst arrangement — worse than O(n log k) by a factor of `k / log k`, which grows without bound as K increases (e.g. K = 1000 gives roughly a 100x worse constant than K-way merge's O(n log k)).

**Space:**
- **O(K)** for the heap itself, at any point in time — bounded by the number of source lists, independent of `n`.
- **O(n)** for the output sequence, if you are building the full merged result (unavoidable — the output itself has n elements). The k-th-smallest variant needs no output storage at all beyond O(K) for the heap.

| Approach | Time | Extra space |
|---|---|---|
| Concatenate + sort | O(n log n) | O(n) (buffer + sort's own overhead) |
| Pairwise/round-robin merge | O(n·k) worst case | O(n) (accumulated result) |
| K-way merge (min-heap) | O(n log k) | O(K) (the heap) + O(n) only if materializing full output |

## Common Mistakes

- **Forgetting to push the next element from the same source list after popping.** This is the single most common bug: pop a value, append it to the output, and move on — without checking whether that list has more elements and pushing the next one. The result silently drops every element that came after the first one in each list. *Avoid:* make "check for and push the replacement" a mandatory, unconditional step immediately after every pop, not something you remember to do only "when it seems needed."
- **Off-by-one errors on list/index bookkeeping.** Pushing `(list[elemIndex], listIdx, elemIndex + 1)` instead of `(list[elemIndex + 1], listIdx, elemIndex + 1)` (pushing the *same* value again instead of the *next* one), or checking `elemIndex < length` instead of `elemIndex + 1 < length` before advancing, either infinite-loops on the last element or skips it entirely. *Avoid:* write the bounds check and the index arithmetic on the same line/comment so they visibly match, and test explicitly with a list of length 1 (the case most likely to expose an off-by-one).
- **Comparator direction mistakes with tuples/pairs in a min-heap.** `std::priority_queue` is a **max-heap by default**; forgetting to pass `std::greater<>` (or an equivalent custom comparator) silently gives you the largest current candidate instead of the smallest, producing output in descending order or outright wrong merge results. *Avoid:* explicitly state and test the heap's ordering direction — a one-line sanity check (push {3,1,2}, pop, expect 1) catches this immediately.
- **Comparing only the value field when the tuple carries more than value.** If you switch from `std::tuple` (which compares all fields in order) to a custom `struct` with a hand-written comparator, forgetting to compare *only* by `value` (or getting the comparator's `<`/`>` backward) either breaks the ordering guarantee or produces heap entries that tie-break inconsistently. *Avoid:* keep the comparator to a single, clearly-named field comparison, and if you must break ties, make the tie-break rule explicit in a comment.
- **Assuming all K lists are non-empty.** Seeding the heap by unconditionally pushing `list[i][0]` for every `i` from `0` to `K-1`, without checking `list[i]` is non-empty first, dereferences an out-of-bounds index the moment any input list happens to be empty. *Avoid:* guard the seeding loop (and every "does this list have a next element" check) with an explicit length check, every single time.
- **Using K-way merge machinery when K is 1 or 2.** Building a full heap-based solution for merging exactly two sorted arrays reaches for far more machinery than a simple two-pointer merge needs, adding both code complexity and (small) constant-factor runtime overhead for zero asymptotic benefit. *Avoid:* recognize when K is small and fixed, and fall back to a direct multi-pointer comparison instead (see When NOT To Use).

## When To Use

- You have **K (three or more) already-sorted sequences** and need one fully merged, sorted output — the textbook signal for this pattern.
- You need the **k-th smallest (or largest) element** across several sorted sequences without needing the full merged result — the early-exit variant saves real work when `k << n`.
- You are merging data that **does not fit in memory all at once**, where each "list" is actually a sorted chunk/file/stream and you can only afford to hold one current element per chunk in memory at a time (external sorting).
- The problem gives you a **matrix where rows (or columns) are individually sorted** — recognize this as K sorted lists in disguise (see [problems/02](problems/02-kth-smallest-element-in-a-sorted-matrix.cpp)).
- You are combining **results from multiple pre-sorted sources that arrive incrementally** (e.g. paginated, already-ordered API responses from several backends) and need a single ordered stream without buffering everything first.

## When NOT To Use

- **Only 1 or 2 lists to merge.** A plain single comparison (1 list — nothing to merge) or a simple two-pointer merge (2 lists) is simpler to write, has less overhead, and is exactly as fast — a heap adds ceremony for zero benefit when K is this small.
- **K is large but every individual list is tiny** (e.g. K = 10,000 lists of 2 elements each). The O(K log K) heap-seeding cost and the constant-factor overhead of tuple-based heap operations may not be worth it versus simpler approaches (like sorting the small total element count directly) — do the arithmetic on your actual K and list sizes before assuming the heap wins.
- **The lists are not actually sorted, and you cannot sort them first.** The entire mechanism depends on "the front of each list is the only candidate that could be next" — if that invariant is false, the merge produces silently wrong output.
- **You need something other than a total order merge** — e.g. deduplicating while merging, or merging with a custom conflict-resolution rule for equal keys from different sources — the base pattern still applies but needs explicit extension; don't reach for it unmodified and assume ties are handled the way you want.
- **You actually need the full sorted K largest/smallest of one array (Top K Elements' job), not to merge several pre-sorted sequences.** These are easy to conflate because both use a heap — see Similar Patterns below for the precise distinction.

## Real Interview/Production Examples

- **Merging sorted log files from multiple servers.** Each server writes its own log file with monotonically increasing timestamps (already sorted by nature of how logs are appended). Reconstructing a single, globally time-ordered view across many servers for a postmortem or an audit is exactly a K-way merge, where K is the number of log sources and n is the total number of log lines.
- **External sorting** (sorting data too large to fit in memory). The classic approach: split the data into chunks small enough to fit in memory, sort each chunk in place, write each sorted chunk to disk, then K-way merge all the sorted chunks by streaming just their current fronts into a heap — never needing more than O(K) elements resident in memory regardless of the total data size. This is the textbook explanation of how database engines and big-data tools sort datasets that dwarf available RAM.
- **The merge/shuffle step of a distributed sort or MapReduce job.** After a distributed sort's "map" phase produces many sorted partitions (one per worker/reducer-input), the final "reduce" phase must combine those sorted partitions into an overall ordered result — a direct application of K-way merge where K is the number of partitions and each partition is one already-sorted "list."
- **Merging paginated results from multiple sharded databases**, where each shard returns its own page of results already sorted by the query's `ORDER BY` clause, and the application layer must produce one globally sorted page — a common pattern in horizontally-sharded backend systems.
- **Database merge joins across more than two sorted inputs** and multi-way `ORDER BY`/`UNION ALL ... ORDER BY` execution plans, where a query engine merges several already-sorted index scans into one ordered stream instead of buffering everything and re-sorting.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **A log-aggregation utility** that merges each microservice's locally sorted (by timestamp) log file into one chronologically ordered timeline for debugging a distributed incident, without loading every file fully into memory at once.
2. **A batch ETL job merging sorted Parquet/CSV shards** produced by parallel workers (each worker sorts its own shard independently) into one final sorted output file, streaming rather than buffering the whole dataset.
3. **A search/ranking service combining pre-sorted result lists from several backend shards** (each shard already returns its results sorted by relevance score) into one final top-N ranked list for the client, using the k-th-smallest/largest variant to stop as soon as you have the top N.
4. **A price-comparison or inventory-aggregation feature** that merges several suppliers' already price-sorted product feeds into one combined, sorted catalog view.
5. **A time-series metrics dashboard** merging multiple sorted per-host or per-region metric streams into a single chronological feed for alert correlation, using the streaming (external-sort-style) version so memory stays bounded regardless of how much history is being merged.

## Similar Patterns

- **Top "K" Elements** ([../top-k-elements/](../top-k-elements/)): also uses a heap, but the question it answers is fundamentally different. Top K Elements maintains a **single, fixed-size-K heap over one collection**, discarding everything outside the current top/bottom K as it scans — it answers "what are the K largest/smallest/most-frequent elements in this one dataset?" K-way Merge maintains a heap with **one entry per source list** (its size is K = number of *lists*, not a chosen cutoff), and answers "how do I combine K already-sorted sequences into one sorted sequence (or find the k-th smallest across them)?" The two patterns are easy to conflate because both are "a heap sized around some K," but one K is a *result-size cutoff* and the other K is a *count of input sources*.
- **Merge Intervals** ([../../array-string-patterns/merge-intervals/](../../array-string-patterns/merge-intervals/)): both patterns involve "merging" and both typically start by sorting, but they merge fundamentally different things. Merge Intervals takes **one list of [start, end] ranges**, sorts it once by start time, and sweeps linearly, combining any ranges that overlap — there is no heap and no notion of "K separate sequences." K-way Merge takes **K separate already-sorted sequences of individual elements** and interleaves them into one ordered stream using a heap. Sharing the word "merge" is a coincidence of naming, not of mechanism.
- **Two Pointers' same-direction merge** ([../../array-string-patterns/two-pointers/](../../array-string-patterns/two-pointers/)): the K=2 special case of this exact idea — merge two sorted arrays by always advancing whichever pointer currently points at the smaller value. K-way Merge is the direct generalization of that same-direction two-pointer merge to K > 2 sources, replacing "compare two candidates directly" with "maintain a min-heap of K candidates" once direct comparison of all K fronts becomes too expensive to do by hand every step.
- **Merge Sort:** its merge step is literally the K=2 building block that K-way Merge generalizes; merge sort itself typically only ever merges two runs at a time (repeatedly, in a binary tree of merges), whereas K-way Merge collapses all K sources in a single pass using the heap instead of a binary merge tree.

| Pattern | What it merges/tracks | Structure used | Primary question answered |
|---|---|---|---|
| K-way Merge | K already-sorted sequences | Min-heap sized to K (one slot per source list) | "Combine K sorted sequences into one sorted sequence" / "find the k-th smallest across all of them" |
| Top K Elements | One dataset, scanned once | Heap sized to a fixed K (a result-size cutoff) | "What are the K largest/smallest/most frequent elements?" |
| Merge Intervals | One list of [start, end] ranges | Sort + linear sweep (no heap) | "Which ranges overlap, and what do they collapse into?" |
| Two Pointers (same-direction merge) | Exactly 2 already-sorted sequences | Two index pointers (no heap) | "Merge two sorted arrays into one" |

## Interview Discussion

Experienced engineers rarely spend interview time on "how do you write the heap push/pop" — that is mechanical. What they actually probe is whether you can **explain why a heap beats the alternatives**, and whether you correctly track the bookkeeping (source list + position) that makes replenishment correct.

Common follow-up questions:
- *"Why not just concatenate everything and sort?"* — expects you to name the complexity gap (O(n log n) vs. O(n log k)) and explain precisely *why* it exists: each list's existing order is being discarded and paid for again.
- *"What if K is very large — does the heap approach still win?"* — expects recognizing that O(n log k) grows slowly with K (logarithmically), so it remains efficient even for large K, unlike pairwise/round-robin merging whose O(n·k) cost scales linearly with K.
- *"How would you merge these if they didn't all fit in memory?"* — expects connecting this pattern to external sorting: stream each source, keep only O(K) elements resident at once, and this is precisely why the heap-based approach (not concatenate-and-sort) is the production answer for very large datasets.
- *"Can you find just the k-th smallest without merging everything?"* — expects the early-exit variant: stop after the k-th pop, do not build the full output, saving O((n-k) log k) of unnecessary work.
- *"What's the difference between this and the Top K Elements pattern? Both use heaps of size K."* — expects the precise distinction: here K is the *number of input lists*, and the heap holds one representative per list; in Top K Elements, K is a *chosen cutoff size* for the answer, over a single dataset with no separate "sources."

Common misconceptions:
- "K-way merge and merge sort are the same algorithm." Merge sort's merge step is the K=2 special case; K-way merge is a distinct generalization using a heap instead of a fixed two-way comparison, specifically because comparing K > 2 candidates by hand every step would cost O(K) instead of O(log K).
- "You need to know all K lists' full contents up front." You only ever need the *current front* of each list — this is what makes the pattern streaming-friendly and suitable for data that does not fit in memory.
- "The heap needs to store whole lists." It stores exactly one tuple per active source (`value`, `list_index`, `element_index`) — never the list itself.
- "This pattern is only for the 'merge two sorted lists' LeetCode problem." That is the K=2 base case; the pattern's real value shows up once K exceeds a small constant, and in production contexts like external sorting and log merging where K can be dozens or thousands.

## Summary

- K-way Merge combines K already-sorted sequences into one sorted output (or finds the k-th smallest across them) using a min-heap that always holds one "current candidate" per still-active source.
- The heap is seeded with the first element of every non-empty list, tagged with `(list_index, element_index)` so a popped value's replacement can always be found.
- Every pop is immediately followed by pushing the next element from the *same* source list, if one remains — the single most important, and most often forgotten, step.
- Complexity: **O(n log k)** where n is the total element count across all lists and k is the number of lists — beating both concatenate-then-sort (O(n log n)) and pairwise/round-robin merging (O(n·k)) whenever K is meaningfully smaller than n.
- Auxiliary space is bounded by **O(K)** for the heap itself, independent of n — the key property that makes this pattern suitable for external sorting and merging data too large to fit in memory.
- Not worth the overhead for K = 1 or 2 lists — a direct comparison or simple two-pointer merge is simpler and just as fast there.
- Real production uses: merging sorted log files, external sorting, the merge/shuffle step of distributed sort and MapReduce, merging sharded/paginated sorted query results.
- Closely related but distinct: Top K Elements (heap sized to a result cutoff over one dataset, not K input sources) and Merge Intervals (sort-and-sweep over one list of ranges, no heap and no multiple sources at all).

## Key Takeaways

1. K-way Merge combines K already-sorted sequences using a min-heap holding one current candidate per source list, achieving O(n log k) instead of O(n log n) or O(n·k).
2. `n` = total elements across all lists; `k` (lowercase) = number of source lists — keep these straight, they are the two different variables in the complexity bound.
3. Heap entries must carry `(value, list_index, element_index)` — value drives the ordering, the other two fields tell you where the next candidate for that source comes from.
4. After every pop, immediately push the next element from the *same* source list (if one remains) — forgetting this silently drops elements.
5. The k-th-smallest variant can stop after the k-th pop — you never need to merge the remaining elements you were never going to look at.
6. `std::priority_queue` is a max-heap by default; you must explicitly supply `std::greater<>` (or an equivalent comparator) to get min-heap behavior.
7. Auxiliary space is O(K) for the heap itself — bounded and independent of total data size, which is why this pattern underlies external sorting.
8. Not worth the machinery for K = 1 or 2 — use direct comparison or a simple two-pointer merge instead.
9. Don't confuse this with Top K Elements — here K counts input *sources*; there, K is a chosen result-size cutoff over a single dataset.
10. Real systems use this directly: log merging across servers, external sorting of oversized datasets, and the merge/shuffle phase of distributed sort and MapReduce.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — covers heaps/priority queues and the merge step of merge sort, the direct mechanical ancestors of K-way merge.
- *The Art of Computer Programming, Volume 3: Sorting and Searching* — Donald Knuth — the classical, in-depth treatment of external sorting and multi-way merging, the original motivating use case for this pattern.
- *Designing Data-Intensive Applications* — Martin Kleppmann — covers how sorting and merging large datasets that exceed memory is handled in real systems (relevant background for the External Sorting production example).
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes a worked K-way merge treatment with C++-specific implementation notes and heap usage.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including heap and k-way-merge-style implementations, useful for seeing varied implementation styles.
- The C++ Standard Library's `<queue>` header (`std::priority_queue`) — the production-grade min/max-heap implementation used throughout this module; reading libstdc++'s or libc++'s source for it is a direct look at heap internals.

**Official Documentation**
- LeetCode — Merge k Sorted Lists (problem 23).
- LeetCode — Kth Smallest Element in a Sorted Matrix (problem 378).
- LeetCode — Find K Pairs with Smallest Sums (problem 373).
- LeetCode — Smallest Range Covering Elements from K Lists (problem 632).
- cppreference.com — `std::priority_queue` — precise documentation of its default max-heap behavior and how to invert it with `std::greater<>`.

**Blog Articles**
- GeeksforGeeks — "K-way Merge" pattern explainer — covers the general technique and its common problem shapes.
- NeetCode — heap/priority-queue pattern videos covering Merge K Sorted Lists and related problems with visual walkthroughs.
- Educative.io — "Grokking the Coding Interview" K-way Merge pattern chapter — one of the most widely referenced pattern-based framings of this exact technique.
- External Sorting explainers (e.g. GeeksforGeeks — "External Sorting") — cover how K-way merge is used in practice to sort data that does not fit in memory.
