# K-way Merge — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Searching/sorting pattern — min-heap-driven simultaneous merge of K already-sorted sequences. |
| **Recognition Signal** | You are handed **several sequences that are each already sorted** and need one merged sorted output, or the k-th smallest across them · count the **disguised** sources too: rows of a row-sorted matrix · `nums1[i] + nums2[j]` for a fixed `i` · one sorted log file per server. |
| **Problem** | Concatenating everything and re-sorting costs O(n log n) and throws away the sortedness you were given for free; scanning all K fronts by hand every step costs O(K) per output element, so O(n·k) overall. |
| **Solution** | Min-heap holding **one current candidate per still-active source**, as `{value, list_index, element_index}`. Seed with every non-empty list's front. Then repeatedly: pop the smallest, emit it, and push the next element from the **same** `list_index` if one remains. Output is sorted by construction — there is never a sort step. |
| **Time / Space Complexity** | O(K log K) to seed + **O(n log k)** for the main loop (n pops, ≤ n pushes, each O(log k)) · **O(K)** heap space, independent of n · plus O(n) only if you materialize the full merged output. |
| **Pros** | Uses the pre-existing sort order instead of re-deriving it · O(n log k) beats both O(n log n) and O(n·k) · heap size bounded by K regardless of how large n is, so it works on data far larger than RAM · supports early exit at the k-th pop · scales past K = 2 with the same code shape. |
| **Cons** | Not worth the machinery for K = 1 or 2 (a two-pointer merge is simpler and just as fast) · needs explicit `list_index`/`element_index` bookkeeping, the main source of bugs · needs tuples or a custom comparator, not a bare `priority_queue<int>` · heap ops are O(log K), not O(1), so a specialized fixed-K merge can win in a hot loop. |
| **Use When** | K ≥ 3 already-sorted sequences to merge · k-th smallest/largest across sorted sequences (early exit) · data too large for memory, each "list" a sorted chunk on disk (external sorting) · row-sorted matrix · combining pre-sorted paginated results from several backends. |
| **Avoid When** | K is 1 or 2 (direct comparison / two-pointer merge) · K huge but every list tiny (do the arithmetic; a plain sort may win) · the sequences are not actually sorted and you cannot sort them first (the core invariant fails and output is silently wrong) · you need dedup or custom conflict resolution while merging (needs explicit extension) · you actually want Top K Elements over one dataset. |
| **Related Patterns** | Top K Elements (heap sized to a **result cutoff** over one dataset, not to a count of sources) · Merge Intervals (sort-and-sweep over one list of ranges, no heap) · Two Pointers same-direction merge (the K = 2 special case this generalizes) · Merge Sort (its merge step is that same K = 2 building block). |

### Template Skeleton

```cpp
using HeapEntry = std::tuple<int, int, int>;  // {value, list_index, element_index}
// std::priority_queue is a MAX-heap by default -- std::greater<> inverts it.
using MinHeap = std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<> >;

std::vector<int> mergeK(const std::vector<std::vector<int> >& lists) {
    MinHeap heap;

    // SEED: one representative per non-empty list. Must complete BEFORE the
    // first pop, or the first "minimum" is only a minimum of what you seeded.
    for (int i = 0; i < static_cast<int>(lists.size()); ++i) {
        if (!lists[i].empty()) {           // the emptiness guard is mandatory:
            heap.emplace(lists[i][0], i, 0);  // lists[i][0] would be out of bounds
        }
    }

    std::vector<int> out;
    while (!heap.empty()) {
        const HeapEntry top = heap.top();
        const int value = std::get<0>(top);
        const int li    = std::get<1>(top);   // WHICH list -- tells you where to refill
        const int ei    = std::get<2>(top);   // WHERE in it -- tells you the next index
        heap.pop();

        out.push_back(value);                 // or: ++popped; if (popped == k) return value;

        // REPLENISH from the SAME list. Pop and replenish are ONE unit --
        // skip this even once and that list's whole tail is silently lost.
        const int next = ei + 1;              // note: ei + 1 < size, NOT ei < size
        if (next < static_cast<int>(lists[li].size())) {
            heap.emplace(lists[li][next], li, next);
        }
        // else: that list is exhausted. Push nothing; the heap just shrinks.
        // This is the normal wind-down, not an error path -- which is why
        // uneven list lengths need no special-casing anywhere.
    }
    return out;
}
```

### Remember In One Sentence
> **Keep a min-heap holding exactly one current candidate per still-active sorted source, tagged with `{value, list_index, element_index}`; pop the smallest and immediately refill from that same source, and you merge K sorted sequences in O(n log k) using only O(K) memory — because each list being pre-sorted means its front is the only element from it that could possibly be next.**

### Two Facts People Get Wrong

- The heap has to hold the **lists**, or at least a chunk of each? **No** — it holds exactly one tuple per still-active source, never more than K entries total, no matter how large n is. That O(K) bound *independent of n* is precisely why this pattern (not concatenate-and-sort) is how external sorting and multi-file log merging actually work: each "list" can be a multi-gigabyte file streamed off disk while only K values sit in RAM.
- Top K Elements and K-way Merge are the same thing since both use a size-K heap? **No** — the two Ks mean different things. In Top K Elements, K is a **result-size cutoff** you were asked to produce, over a single input collection with no notion of sources. Here, K is a **count of input sources**, and the heap's size is dictated by how many lists are still active, not by how much output you want. Confusing them leads to capping the heap at the wrong number and dropping data.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. In the O(n log k) bound, what exactly does `n` count and what does `k` count — and why are they deliberately different letters?
2. Why is the front of list `i` the *only* element from list `i` that could possibly be the next-smallest overall? Name the input property this argument depends on.
3. `mergeKSortedLists` in [code.cpp](code.cpp) stores `{value, list_index, element_index}`. What specifically breaks if you drop `list_index`? What breaks if you drop `element_index`?
4. State the single most commonly forgotten step in this pattern, and describe exactly how the output is wrong when you omit it (does it crash, or fail silently?).
5. Why does `std::priority_queue` need `std::greater<>` here, and what is the observable symptom of forgetting it? Name a one-line sanity check that catches it immediately.
6. The replenishment guard is `element_index + 1 < size`, not `element_index < size`. Describe the bug produced by each of the two possible off-by-one errors, and name the input size most likely to expose them.
7. `kthSmallestInKSortedLists` returns `MaybeInt` rather than a bare `int`. Why would returning `-1` as "not found" be wrong?
8. Pairwise merging (merge list 1 with 2, then that result with 3, ...) uses the exact right mechanical idea and still lands at O(n·k). Where does the extra work come from?
9. In `problems/03-find-k-pairs-with-smallest-sums.cpp`, what plays the role of a "sorted list," given that no list of sums is ever materialized in memory? Why is each such implicit list guaranteed sorted?
10. Which property of this pattern makes it the foundation of external sorting, and why does concatenate-then-sort fail at that job even though it produces the same answer?
