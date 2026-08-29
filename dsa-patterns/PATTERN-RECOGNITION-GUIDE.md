# Pattern Recognition Guide

The hard part of pattern-based prep is never the code once you know which pattern applies — it's the 30 seconds *before* you start typing, where you decide which of the 36 patterns this problem actually is. This page is a single lookup table for that decision, built from every pattern's own Recognition Signal. Use it as your entry point on a new problem; once it points you at a pattern, go to that pattern's own README for the Recognition Diagram/full depth.

## The 3-step process

1. **Classify the input shape.** Array/string? Linked list? Tree? Graph? 2D grid? Small fixed set (`n ≤ ~20`)? Or is the question actually "design a structure," not "solve for a value"? This alone eliminates most of the 8 families.
2. **Classify what's being asked.** A count? A boundary/index? The best/optimal value? All possible arrangements? A yes/no reachability check? This narrows it to 2-4 candidate patterns within the family.
3. **Check the constraints to break the tie.** Size of `n`, whether the input is sorted, whether edges are weighted, whether there's a fixed capacity/budget, whether updates are interleaved with queries. These are almost always the deciding signal between two similar-looking patterns (see the table at the bottom).

Below is Step 1 and Step 2 combined into one table, grouped by input shape.

---

## Array / String input

| If the problem says or implies... | Pattern | Family |
|---|---|---|
| Sorted array, find a pair/triplet summing to a target | [Two Pointers](array-string-patterns/two-pointers/README.md) | Array & String |
| "Contiguous subarray/substring" with a size or property constraint (max/min length, at most K distinct, no repeats) | [Sliding Window](array-string-patterns/sliding-window/README.md) | Array & String |
| Same array, many range-sum queries, **no updates** | [Prefix Sum](array-string-patterns/prefix-sum/README.md) | Array & String |
| Values are exactly `1..n` or `0..n-1`, need the missing/duplicate one | [Cyclic Sort](array-string-patterns/cyclic-sort/README.md) | Array & String |
| A list of `[start, end]` ranges that might overlap | [Merge Intervals](array-string-patterns/merge-intervals/README.md) | Array & String |
| "Maximum/best contiguous sum," no window size given | [Kadane's Algorithm](array-string-patterns/kadanes-algorithm/README.md) | Array & String |
| Array/rotated array is sorted (or piecewise sorted); need one target, a first/last boundary, or "smallest feasible answer" | [Modified Binary Search](searching-sorting-patterns/modified-binary-search/README.md) | Searching & Sorting |
| Need the running median (or similar middle order-statistic) as data streams in | [Two Heaps](searching-sorting-patterns/two-heaps/README.md) | Searching & Sorting |
| Need the K largest/smallest/most-frequent — not a full sort | [Top "K" Elements](searching-sorting-patterns/top-k-elements/README.md) | Searching & Sorting |
| Already have K separate sorted lists/arrays to merge or find the k-th smallest across | [K-way Merge](searching-sorting-patterns/k-way-merge/README.md) | Searching & Sorting |
| "Next greater/smaller element" for every index, or a sliding-window max/min | [Monotonic Stack/Queue](advanced-ds-patterns/monotonic-stack-queue/README.md) | Advanced Data Structures |
| Binary representation, XOR tricks, or "appears an odd number of times" | [Bit Manipulation](advanced-ds-patterns/bit-manipulation/README.md) | Advanced Data Structures |
| Prefix/autocomplete/dictionary search over many strings | [Trie](advanced-ds-patterns/trie/README.md) | Advanced Data Structures |
| Range query interleaved with **updates** (a plain prefix sum would need an O(n) rebuild per update) | [Segment Tree / Fenwick Tree](advanced-ds-patterns/segment-tree-fenwick-tree/README.md) | Advanced Data Structures |
| "Return all subsets/combinations/permutations," no constraint to prune on | [Subsets](recursion-backtracking-patterns/subsets/README.md) | Recursion & Backtracking |
| Same, but there's a constraint that can invalidate a partial solution early (N-Queens, Sudoku, word search) | [Backtracking](recursion-backtracking-patterns/backtracking/README.md) | Recursion & Backtracking |

## Linked List input

| If the problem says or implies... | Pattern | Family |
|---|---|---|
| Detect a cycle, find the middle, or find where two pointers "meet" | [Fast & Slow Pointers](linked-list-patterns/fast-slow-pointers/README.md) | Linked List |
| Reverse the whole list, a `[left, right]` sub-range, or reverse in groups of k, in O(1) space | [In-place Reversal](linked-list-patterns/in-place-reversal/README.md) | Linked List |

## Tree / Graph input

| If the problem says or implies... | Pattern | Family |
|---|---|---|
| Input is explicitly a tree (one root, no cycles), question is about **levels** | [Tree BFS](tree-graph-patterns/tree-bfs/README.md) | Tree & Graph |
| Same, but question is about **paths/depth/subtree properties** | [Tree DFS](tree-graph-patterns/tree-dfs/README.md) | Tree & Graph |
| General graph (possibly cyclic), need fewest hops or reachability, **every edge costs the same** | [Graph BFS/DFS](tree-graph-patterns/graph-bfs-dfs/README.md) | Tree & Graph |
| Same, but edges carry **different (non-negative) costs/weights** | [Dijkstra's Algorithm](tree-graph-patterns/dijkstras-algorithm/README.md) | Tree & Graph |
| Same, but edge weights **can be negative**, or you need to detect a **reachable negative cycle** | [Bellman-Ford](tree-graph-patterns/bellman-ford/README.md) | Tree & Graph |
| Need shortest distances between **every pair** of nodes, not just from one source | [Floyd-Warshall](tree-graph-patterns/floyd-warshall/README.md) | Tree & Graph |
| Directed "must come before" edges, need a valid order (or to detect a cycle in that ordering) | [Topological Sort](tree-graph-patterns/topological-sort/README.md) | Tree & Graph |
| Edges arrive one at a time; you keep asking "connected?" or "would this create a cycle?" | [Union Find](tree-graph-patterns/union-find/README.md) | Tree & Graph |
| "Minimum cost to connect everything," no privileged source/destination — a network, not a path | [Minimum Spanning Tree](tree-graph-patterns/mst-kruskal-prim/README.md) | Tree & Graph |

## 2D Grid input

| If the problem says or implies... | Pattern | Family |
|---|---|---|
| Move only right/down (or similar constrained directions) across a grid, optimize a path sum/count | [DP on Grids](dynamic-programming-patterns/dp-on-grids/README.md) | Dynamic Programming |
| Grid represents a graph in disguise (islands, flood fill, shortest path through open cells) | [Graph BFS/DFS](tree-graph-patterns/graph-bfs-dfs/README.md) (or [Dijkstra's](tree-graph-patterns/dijkstras-algorithm/README.md) if cells have different costs) | Tree & Graph |

## "Optimize a choice over items with constraints" (Dynamic Programming / Greedy)

| If the problem says or implies... | Pattern | Family |
|---|---|---|
| Items with weight/value + a capacity, each item usable **once** | [0/1 Knapsack](dynamic-programming-patterns/0-1-knapsack/README.md) | Dynamic Programming |
| Same, but items are reusable (unlimited supply) | [Unbounded Knapsack](dynamic-programming-patterns/unbounded-knapsack/README.md) | Dynamic Programming |
| Comparing **two** sequences (edit distance, common subsequence, shortest common supersequence) | [Longest Common Subsequence](dynamic-programming-patterns/longest-common-subsequence/README.md) | Dynamic Programming |
| **One** sequence, want the longest subsequence obeying an order relation | [Longest Increasing Subsequence](dynamic-programming-patterns/longest-increasing-subsequence/README.md) | Dynamic Programming |
| One string, question is specifically about a palindrome inside it | [Palindromic Subsequence/Substring](dynamic-programming-patterns/palindromic-subsequence/README.md) | Dynamic Programming |
| `n` is small (≲ 20-24) and the state depends on **which specific** subset of items/cities is used, not just a count (TSP, equal-sum partition, assignment) | [Bitmask DP](dynamic-programming-patterns/bitmask-dp/README.md) | Dynamic Programming |
| You can sort by some key and commit to each locally-best choice **without ever revisiting it** (interval scheduling, jump games, gas station) | [Greedy](greedy-patterns/greedy/README.md) | Greedy |
| Same setup, but you find yourself needing to compare "take it" vs. "skip it" and can't commit without looking ahead | → it's actually **DP**, not Greedy (see the DP rows above) | Dynamic Programming |

## "Design a data structure"

| If the problem says or implies... | Pattern | Family |
|---|---|---|
| "Design a cache" with O(1) get/put and an eviction policy tied to recency (or frequency) | [LRU Cache](advanced-ds-patterns/lru-cache/README.md) | Advanced Data Structures |
| "Design a structure supporting insert/search by prefix" | [Trie](advanced-ds-patterns/trie/README.md) | Advanced Data Structures |
| "Design a structure supporting fast range query + point update" | [Segment Tree / Fenwick Tree](advanced-ds-patterns/segment-tree-fenwick-tree/README.md) | Advanced Data Structures |
| "Design a structure that answers 'are these two connected?' as edges are added" | [Union Find](tree-graph-patterns/union-find/README.md) | Tree & Graph |

---

## Constraint-driven tie-breakers

When two patterns look equally plausible from the wording alone, the constraints almost always decide it:

| Constraint you notice | What it tells you |
|---|---|
| `n ≤ ~20` in the limits | Probably **Bitmask DP** — brute-force-ish over subsets becomes tractable at this size, and interviewers size `n` deliberately to hint at it. |
| Input is already sorted | **Two Pointers** or **Modified Binary Search** over a full O(n) or O(n log n) scan-from-scratch approach. |
| "In-place" / "O(1) extra space" on a linked list | **Fast & Slow Pointers** or **In-place Reversal** — both exist specifically to avoid an O(n) auxiliary structure. |
| Repeated queries against the *same static* array, no mutation | **Prefix Sum** (or a one-time sort + **Modified Binary Search**) — amortize preprocessing across many queries. |
| Queries interleaved *with* updates to the same array | **Segment Tree / Fenwick Tree** — Prefix Sum's O(n) rebuild per update stops being cheap. |
| Graph edges have different costs | **Dijkstra's Algorithm**, not plain BFS. |
| Graph edges can be **negative**, or a "refund/credit/gain" is modeled as a cost | **Bellman-Ford**, not Dijkstra's — a single negative edge invalidates Dijkstra's correctness argument outright. |
| Need distances between **every pair**, not one fixed source | **Floyd-Warshall**, not `V` runs of Dijkstra/Bellman-Ford — same answer, one triple loop instead of managing V separate runs. |
| "Minimize total cost to connect everything," no source/destination named at all | **Minimum Spanning Tree**, not Dijkstra's or Bellman-Ford — those minimize cost *to reach* a node; MST minimizes cost *to connect* every node, and the two can disagree on the same graph. |
| Asked for "all" outputs (subsets/permutations/paths) with no way to prune | **Subsets**, not Backtracking. |
| Asked for "all valid" outputs, and invalid states can be detected early | **Backtracking**, not Subsets. |
| You need to choose between two branches and compare results before committing | **Dynamic Programming**, not Greedy. |

## Worked example

> "You are given a list of flights `[from, to, price]` and want the cheapest price from city `A` to city `B` using at most `K` stops."

Walking the 3 steps:
1. **Input shape** — cities and flights is a graph (nodes = cities, edges = flights).
2. **What's being asked** — cheapest *cost*, not fewest hops, so it's not plain BFS. It's a shortest-path question with weighted edges.
3. **Constraint** — edge weights (prices) differ, and prices are non-negative (real-world flight prices can't be negative) → **Dijkstra's Algorithm**, with the state extended to track `(cost, node, stops_used)` so the `K`-stop limit is respected.

That reasoning chain — shape, ask, constraint — is the whole skill. The more problems you run it against (via NeetCode, per your own plan), the faster it becomes automatic; this page is the reference to fall back on until it is.
