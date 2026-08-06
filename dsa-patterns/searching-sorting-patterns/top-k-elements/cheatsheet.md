# Top K Elements — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Searching/Sorting pattern — heap-based bounded selection. |
| **Recognition Signal** | The question asks for the **K largest / K smallest / K most-frequent** elements out of a much larger collection (K << n), not the full sorted order of everything. |
| **Problem** | A full sort costs O(n log n) to answer a question that only needs K values out of n — wasted ordering work, wasted memory, and it cannot process a live/streaming input incrementally. |
| **Solution** | Maintain a heap of **exactly size K**. Push every element; if the heap's size now exceeds K, pop once. **Inverted heap type:** min-heap for "K largest" (so the weakest of the current top-K is instantly evictable), max-heap for "K smallest." "K most-frequent" uses the same min-heap shape, keyed by a precomputed frequency. |
| **Time / Space Complexity** | O(n log k) time (n pushes/pops, each O(log k) since the heap never exceeds size k), O(k) extra space (plus O(d) for a frequency map, if used, where d = distinct values). |
| **Pros** | O(n log k) beats a full O(n log n) sort whenever k << n · O(k) space, independent of n · works on streaming/unbounded input · simple uniform control flow (push, then evict-if-oversized) · generalizes to any derived ordering key (frequency, distance, score). |
| **Cons** | Does not give sorted output for free (extra O(k log k) sort needed if order matters) · benefit disappears as k approaches n · the min-heap/max-heap inversion is a common source of bugs · real per-element constant-factor overhead versus a plain running max/min when k = 1. |
| **Use When** | K is meaningfully smaller than n · input may be streaming/live · the ordering key is a derived quantity (frequency, distance) · you need a bounded "leaderboard"/"top N" maintained incrementally. |
| **Avoid When** | You need the full sorted order of every element (sort everything instead) · K is close to n (plain sort is simpler, same cost) · the input is fully in memory and needed only once (quickselect/`std::nth_element` beats a heap in average case) · you need the running median, not a top/bottom K (use Two Heaps). |
| **Related Patterns** | Two Heaps (median tracking via two balanced heaps, not a bounded top-K) · K-way Merge (heap seeded one-slot-per-sorted-source-list, for merging, not filtering to extremes). |

### Template Skeleton

```cpp
// "K largest" -- MIN-heap of size K (inverted relative to intuition)
std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
for (int value : nums) {
    minHeap.push(value);
    if (static_cast<int>(minHeap.size()) > k) {
        minHeap.pop();   // evict the current weakest of the top-K-so-far
    }
}
// minHeap now holds exactly the K largest values (NOT sorted order).

// "K smallest" -- MAX-heap of size K (the mirror image)
std::priority_queue<int> maxHeap;   // default priority_queue is already a max-heap
for (int value : nums) {
    maxHeap.push(value);
    if (static_cast<int>(maxHeap.size()) > k) {
        maxHeap.pop();   // evict the current largest of the bottom-K-so-far
    }
}

// "K most frequent" -- same min-heap shape, keyed by frequency
std::unordered_map<int, int> freq;
for (int value : nums) ++freq[value];
std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>,
                     std::greater<std::pair<int,int>>> minHeapByFreq;  // (count, value)
for (auto& [value, count] : freq) {
    minHeapByFreq.push({count, value});
    if (static_cast<int>(minHeapByFreq.size()) > k) {
        minHeapByFreq.pop();
    }
}
```

### Remember In One Sentence
> **Top K Elements keeps a heap of exactly size K, pushing every element and popping whenever the heap grows past K — and the heap type is always the OPPOSITE of what you're searching for: min-heap for "K largest," max-heap for "K smallest," because you need instant access to the weakest member of the current top-K for eviction, not the strongest.**

### Two Facts People Get Wrong
- "K largest" wants a **max-heap** because "max" sounds right? **No** — it wants a **min-heap**, because the min-heap's top is the weakest member of your current top-K, exactly the element you need instant access to for eviction.
- A size-K heap always beats sorting? **No** — the win only exists when K is meaningfully smaller than n; as K approaches n, O(n log k) approaches O(n log n) and a plain sort becomes simpler for the same cost.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the recognition signal that means "reach for Top K Elements" instead of a full sort.
2. Why does "K largest" use a min-heap instead of a max-heap? State the one-sentence justification.
3. What heap type does "K smallest" use, and why is it the mirror image of question 2's answer?
4. What is the exact eviction rule applied on every single element of the input?
5. Does the heap hold the final K elements in sorted order when the scan finishes? If not, what extra step is needed, and what does it cost?
6. What preliminary step does "K most frequent" require before the heap logic can even begin, and why?
7. State the time complexity of this pattern and explain, in terms of k versus n, why the gap versus a full sort grows as k shrinks.
8. Name one situation where this pattern's benefit basically disappears, and explain why.
9. Name one alternative (non-heap) approach that can beat this pattern's complexity, and state the tradeoff it makes to do so.
10. What is the key structural difference between Top K Elements and Two Heaps, given that both use a heap (or heaps)?
