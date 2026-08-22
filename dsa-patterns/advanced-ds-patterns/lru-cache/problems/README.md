# LRU Cache — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions showing the "hash map + a second structure whose O(1) operation you actually need" composition across its main variants: the canonical recency cache, its frequency-based sibling, and two design problems that reuse the same primitives (a doubly linked list with a cursor, and time-driven expiry with lazy pruning). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against LeetCode's published examples.

```bash
g++ -std=c++17 -Wall problems/01-lru-cache.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| LRU Cache | [146](https://leetcode.com/problems/lru-cache/) | Medium | Hash map `key -> Node*` indexing into a sentinel-bracketed doubly linked list in recency order; hit/overwrite unlinks and re-pushes to front, insertion at capacity evicts `tail_->prev`. | O(1) avg per op, O(capacity) space | [01-lru-cache.cpp](01-lru-cache.cpp) |
| LFU Cache | [460](https://leetcode.com/problems/lfu-cache/) | Hard | Same skeleton plus frequency buckets: `map freq -> doubly linked list of nodes` with a tracked `minFreq`; a use bumps the node one bucket up, eviction takes `bucket[minFreq]->tail_->prev`, breaking ties by recency within the bucket. | O(1) avg per op, O(capacity) space | [02-lfu-cache.cpp](02-lfu-cache.cpp) |
| Design Browser History | [1472](https://leetcode.com/problems/design-browser-history/) | Medium | Array of visited URLs plus a cursor index: `visit` truncates everything after the cursor and appends; `back`/`forward` clamp the cursor — O(1) unlink-free movement, the list pattern without any pointer surgery. | O(1) visit / O(steps) back-forward, O(n) space | [03-design-browser-history.cpp](03-design-browser-history.cpp) |
| Design Authentication Manager | [1797](https://leetcode.com/problems/design-authentication-manager/) | Medium | Hash map `token -> expiry` plus a min-heap of expiries pruned lazily on count; renew only when the stored expiry is still in the future, stale heap entries are skipped by comparing against the map. | O(log n) amortized per op, O(n) space | [04-authentication-manager.cpp](04-authentication-manager.cpp) |

## Why these four

They cover the recognition signals called out in the [README](../README.md):

- **01** is the pattern itself — the shortest problem that forces composing two structures and defending the coupling invariant between them.
- **02** shows the composition generalizing rather than being memorized: keep the identical node/list machinery, change *what the buckets mean*, and add exactly one tracked value (`minFreq`) to preserve O(1) eviction.
- **03** isolates the doubly linked list's core primitive — O(1) removal/relink around a moving cursor — while deliberately dropping the hash map, showing which half of LRU each sub-problem actually needs; the array version also demonstrates when index arithmetic beats pointer surgery outright.
- **04** is the other axis entirely: eviction driven by an external deadline instead of capacity/recency. Its lazy-pruning heap exists precisely because a min-heap is the right tool when you do not control the ordering key — the exact contrast the README's "Why Not Other Approaches" section draws.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
