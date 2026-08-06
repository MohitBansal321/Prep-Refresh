# Top K Elements — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether Top K Elements is the right tool, or whether the problem actually wants Two Heaps or a plain full sort instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Does it ask for the K<br/>largest / smallest / most-frequent<br/>elements out of a much larger<br/>collection?}

    Q1 -- No --> Q1b{Does it ask for the running<br/>median or another single<br/>middle order-statistic<br/>of a growing stream?}
    Q1b -- Yes --> TwoHeaps[["Use Two Heaps<br/>(max-heap for lower half +<br/>min-heap for upper half)"]]
    Q1b -- No --> Q1c{Do you need the FULL<br/>sorted order of every<br/>element, not just K of them?}
    Q1c -- Yes --> FullSort[["Use a full sort<br/>O(n log n) -- you need<br/>the whole ranking anyway"]]
    Q1c -- No --> Reconsider[Re-read the problem --<br/>it may want a different<br/>pattern entirely, e.g.<br/>Binary Search or K-way Merge]

    Q1 -- Yes --> Q2{Is K meaningfully SMALLER<br/>than n, e.g. top 10 out<br/>of a million?}

    Q2 -- No, K is close to n --> FullSort

    Q2 -- Yes --> Q3{What are you ranking by?}

    Q3 -- "Raw value (largest/smallest)" --> Q4{Largest K, or Smallest K?}
    Q4 -- "Largest K" --> MinHeap["Use Top K Elements<br/>MIN-heap of size K<br/>(inverted: weakest-of-best<br/>on top for eviction)"]
    Q4 -- "Smallest K" --> MaxHeap["Use Top K Elements<br/>MAX-heap of size K<br/>(mirror image of the above)"]

    Q3 -- "Frequency / a derived score<br/>(distance, custom comparator)" --> FreqHeap["Use Top K Elements<br/>(frequency/derived-key variant)<br/>count or compute the key first,<br/>then apply the same size-K<br/>min-heap push/evict rule"]

    MinHeap --> Done([Top K Elements applies])
    MaxHeap --> Done
    FreqHeap --> Done
```

## How to read it

Start at the top and answer each diamond honestly. The **first real fork** is the shape of the question itself: Top K Elements only applies when the answer is a *bounded* set of K extreme or frequent elements — if the problem actually wants a single *middle* value of a growing stream (the median), that is Two Heaps' territory, a different pattern that happens to also use heaps.

The **second fork** is the size relationship between K and n. This is the fork people skip most often, and skipping it is a real mistake: if K is close to n (say, K is 90% of n), the size-K heap's complexity advantage (O(n log k) vs. O(n log n)) evaporates almost entirely, and a plain full sort is simpler code for effectively the same cost. Top K Elements earns its keep specifically in the regime where K is small and fixed (a UI's "top 10" requirement) while n grows large (your data volume growing over time) — that gap is the pattern's entire reason to exist.

The **third fork** decides the heap type, and this is where the pattern's single most counter-intuitive rule lives: "largest K" pairs with a **min-heap**, and "smallest K" pairs with a **max-heap** — always the *opposite* of what first intuition suggests. If you cannot immediately state why (because the heap's top must be the weakest member of your current top-K, for cheap eviction), re-read the README's Solution section before writing any code; getting this backward either produces subtly wrong results or defeats the pattern's complexity advantage entirely. The "frequency/derived-key" branch is the same min-heap-of-size-K shape as "largest K" — only the thing being compared (a computed frequency or distance, instead of the raw value) changes.
