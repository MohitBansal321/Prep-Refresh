# Searching & Sorting Patterns

These patterns lean on **sorted input** or a **heap's ordering guarantee** to avoid the O(n log n)-per-query cost of re-sorting, or the O(n)-per-query cost of a linear scan, every time you need an extreme value or a boundary.

| Pattern | Core idea | Typical complexity win |
|---------|-----------|-------------------------|
| [Modified Binary Search](modified-binary-search/README.md) | Halve the search space using a problem-specific "which half is valid?" test | O(n) scan → O(log n) |
| [Two Heaps](two-heaps/README.md) | A max-heap (small half) + a min-heap (large half) track the running median | O(n log n) re-sort per query → O(log n) per insert |
| [Top "K" Elements](top-k-elements/README.md) | A fixed-size heap of K elements tracks the current top-K | O(n log n) full sort → O(n log k) |
| [K-way Merge](k-way-merge/README.md) | A min-heap seeded with one element per list merges K sorted lists | O(n·k) pairwise merging → O(n log k) |

## How to tell them apart

- **Input is sorted (or rotated-sorted) and you need one target/boundary?** → Modified Binary Search.
- **You need the running median (or a similar middle-order statistic) as data streams in?** → Two Heaps.
- **You need the K largest/smallest/most-frequent, not a full sort?** → Top K Elements.
- **You already have K separate sorted lists to combine or find the k-th smallest across?** → K-way Merge.

## Recommended study order

1. **Modified Binary Search** — the foundational halving skill; every other pattern here either uses a heap instead of halving, or halves along a different axis.
2. **Top K Elements** — the simplest heap application (one heap, fixed size).
3. **Two Heaps** — extends to two heaps working together for a median.
4. **K-way Merge** — a different heap use (one slot per source list) worth contrasting with #2 and #3.

All four are built: Modified Binary Search, Two Heaps, and Top K Elements are Full-tier modules; K-way Merge is Partial-tier (README + code + worked problems, missing exercises/cheatsheet/diagrams). See [`../INDEX.md`](../INDEX.md) for details.
