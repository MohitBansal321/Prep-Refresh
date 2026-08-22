# Advanced Data Structure Patterns

These four don't share one unifying idea the way the other families do — each introduces a **purpose-built data structure** that turns an otherwise slow repeated operation into something fast, at the cost of extra code to build and maintain that structure.

| Pattern | Structure | Turns this slow operation... | ...into this fast one |
|---------|-----------|-------------------------------|------------------------|
| [Trie (Prefix Tree)](trie/README.md) | Tree keyed by characters | Scanning every word for a prefix match — O(n · length) | O(length), independent of dictionary size |
| [Monotonic Stack/Queue](monotonic-stack-queue/README.md) | Stack/deque kept sorted | Finding next-greater/smaller for every index — O(n²) | O(n) total |
| [Bit Manipulation](bit-manipulation/README.md) | The integer itself as a bitset | Extra array/set for tracking small state — O(n) space | O(1) space |
| [Segment Tree / Fenwick Tree](segment-tree-fenwick-tree/README.md) | Binary tree / implicit tree over indices | Range query + update, each O(n) if rebuilt naively | O(log n) each |
| [LRU Cache (Design With Data Structures)](lru-cache/README.md) | Hash map (key -> node) + doubly linked list ordered by recency | O(1) get/put with a recency- or frequency-based eviction policy |

## How to tell them apart

- **Prefixes of strings, autocomplete, dictionary search?** → Trie.
- **"Next greater/smaller element" for every position, or sliding-window max?** → Monotonic Stack/Queue.
- **Problem hints at binary representation, XOR tricks, or subset masks over a small set?** → Bit Manipulation.
- **Range queries interleaved with updates, where a plain prefix-sum array would need an O(n) rebuild per update?** → Segment Tree / Fenwick Tree.
- **The problem literally says "design a \<cache/structure\>" with an O(1) access + eviction/ordering requirement?** → LRU Cache — the hash-map-plus-linked-list combo is the template for this entire family of "design" questions (LFU Cache, browser history, etc.).

## Recommended study order

1. **Monotonic Stack/Queue** — smallest conceptual jump from arrays you already know.
2. **Bit Manipulation** — a toolbox of tricks more than a single algorithm; useful to have early since it reappears inside other patterns (subset generation, single-number problems).
3. **Trie** — a new tree shape, but built from familiar tree-traversal instincts.
4. **LRU Cache** — combines a hash map (which you already know) with a doubly linked list to get O(1) on both lookup and reordering; good bridge before the heavier tree structure below.
5. **Segment Tree / Fenwick Tree** — the most involved structure here; save it for last, and only after Prefix Sum (in Array & String Patterns) so you feel *why* prefix sums aren't enough once updates enter the picture.

All five are built as Full-tier modules: Monotonic Stack/Queue, Bit Manipulation, Trie, Segment Tree/Fenwick Tree, and LRU Cache. See [`../INDEX.md`](../INDEX.md) for details.
