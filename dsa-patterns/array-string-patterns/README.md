# Array & String Patterns

These patterns cover the vast majority of array/string interview problems. What ties them together: they all replace an **O(n²) or worse brute-force scan** with a single linear (or near-linear) pass, by keeping a small amount of running state instead of re-scanning.

| Pattern | Core idea | Typical complexity win |
|---------|-----------|-------------------------|
| [Two Pointers](two-pointers/README.md) | Two indices moving toward/away from each other, usually over a **sorted** array | O(n²) → O(n) |
| [Sliding Window](sliding-window/README.md) | A window `[left, right]` that grows/shrinks over a **contiguous** range | O(n²) or O(n·k) → O(n) |
| [Prefix Sum](prefix-sum/README.md) | Precomputed running sums answer range queries in O(1) | O(n) per query → O(1) per query |
| [Cyclic Sort](cyclic-sort/README.md) | Place each value at its "home" index when values are in `[1..n]`/`[0..n-1]` | O(n log n) sort → O(n), O(1) space |
| [Merge Intervals](merge-intervals/README.md) | Sort by start, sweep once, merge overlaps | O(n²) pairwise comparison → O(n log n) |
| [Kadane's Algorithm](kadanes-algorithm/README.md) | Best subarray sum ending "here", drop a negative running sum | O(n²) all-subarrays → O(n) |

## How to tell them apart

- **Sorted + looking for a pair/triplet that sums to something?** → Two Pointers.
- **Contiguous subarray/substring with a size or property constraint?** → Sliding Window.
- **Same array, many range-sum queries, no updates?** → Prefix Sum.
- **Values are exactly `1..n` (or `0..n-1`) and you need the missing/duplicate one?** → Cyclic Sort.
- **A list of `[start, end]` ranges that might overlap?** → Merge Intervals.
- **Best contiguous sum, no window-size given?** → Kadane's.

## Recommended study order

1. **Two Pointers** — the simplest mental model (two indices, sorted input).
2. **Sliding Window** — directly generalizes Two Pointers to a variable-size contiguous range.
3. **Prefix Sum** — a different axis (precompute vs. scan) worth contrasting with Sliding Window.
4. **Kadane's Algorithm** — a special case of "running state" thinking, close cousin of Sliding Window/DP.
5. **Cyclic Sort** — a narrow but very efficient trick once you spot the `[1..n]` signal.
6. **Merge Intervals** — its own small family (sort + sweep) that shows up constantly in scheduling problems.

All six are built: Two Pointers, Sliding Window, Prefix Sum, Cyclic Sort, Merge Intervals, and Kadane's Algorithm are all Full-tier modules (see [`../INDEX.md`](../INDEX.md) for details).
