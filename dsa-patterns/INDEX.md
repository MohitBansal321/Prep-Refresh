# DSA Patterns — Index & Revision Tracker

This is the master index for the DSA patterns repo. Use it three ways:
1. **Find** a pattern quickly.
2. **Revise** on a spaced-repetition schedule so patterns move into long-term memory.
3. **Identify** which pattern a new problem needs — see [`PATTERN-RECOGNITION-GUIDE.md`](./PATTERN-RECOGNITION-GUIDE.md), a single lookup table across all 33 patterns by input shape and problem signal.

New here, or refreshing after a long break? [`LEARNING-PATHS.md`](./LEARNING-PATHS.md) has a specific walkthrough for how to use this index and the rest of the repo depending on where you're starting from — read that before diving into the tables below.

All 33 patterns share the same module shape: README + `code.cpp` + `exercises.md` + `cheatsheet.md` + `images/` (recognition / flow / trace diagrams) + `problems/` with 4 fully worked LeetCode solutions. Every README now opens with a one-line summary, a runnable snippet, and a "pick your depth" table — some (the shorter, code-forward ones) stop there; others follow with a longer deep-dive. Either way, you never have to read the whole file to find the loop.

Every `.cpp` file in the repo (165 total) compiles with `g++ -std=c++17 -Wall` and passes its own test assertions.

---

## How to revise (the flow)

For a **full module**, revise in tiers — do NOT re-read the full README:
1. **60 sec** — read `cheatsheet.md`.
2. **5 min** — skim the 3 diagrams in `images/` (recognition, flow, trace).
3. **Active recall** — answer the *Recall Questions* at the bottom of `cheatsheet.md` from memory, then re-derive one `problems/` solution from scratch without looking at the file.
4. **Only if you miss something** — open the relevant README section. Nothing else.

## Spaced-repetition schedule

After each successful revision, set the next date using the ladder:

`Learned → +1 day → +3 days → +1 week → +2 weeks → +1 month → +3 months`

If you fail recall on a pattern, drop it back one rung. Update the two date columns each time you revise.

---

## Array & String Patterns

| Pattern | Tier | Status | Last Revised | Next Revision | Confidence (1-5) |
|---------|------|--------|---------------|----------------|-------------------|
| [Two Pointers](array-string-patterns/two-pointers/README.md) | Full | Not started | — | Study next | — |
| [Sliding Window](array-string-patterns/sliding-window/README.md) | Full | Not started | — | Study next | — |
| [Prefix Sum](array-string-patterns/prefix-sum/README.md) | Full | Not started | — | Study next | — |
| [Cyclic Sort](array-string-patterns/cyclic-sort/README.md) | Full | Not started | — | Study next | — |
| [Merge Intervals](array-string-patterns/merge-intervals/README.md) | Full | Not started | — | Study next | — |
| [Kadane's Algorithm](array-string-patterns/kadanes-algorithm/README.md) | Full | Not started | — | Study next | — |

### Recommended study order (array & string)

1. **Two Pointers** — two indices over a sorted array.
2. **Sliding Window** — generalizes to a variable-size contiguous range.
3. **Prefix Sum** — precompute vs. scan, a useful contrast with Sliding Window.
4. **Kadane's Algorithm** — running-state thinking, close cousin of Sliding Window/DP.
5. **Cyclic Sort** — narrow but efficient once you spot the `[1..n]` signal.
6. **Merge Intervals** — its own small sort-and-sweep family.

---

## Linked List Patterns

| Pattern | Tier | Status | Last Revised | Next Revision | Confidence (1-5) |
|---------|------|--------|---------------|----------------|-------------------|
| [Fast & Slow Pointers](linked-list-patterns/fast-slow-pointers/README.md) | Full | Not started | — | Study next | — |
| [In-place Reversal](linked-list-patterns/in-place-reversal/README.md) | Full | Not started | — | Study next | — |

### Recommended study order (linked list)

1. **Fast & Slow Pointers** — the "two speeds meet" insight (Floyd's cycle detection).
2. **In-place Reversal** — pointer-rewiring mechanics, easiest once #1 is comfortable.

---

## Searching & Sorting Patterns

| Pattern | Tier | Status | Last Revised | Next Revision | Confidence (1-5) |
|---------|------|--------|---------------|----------------|-------------------|
| [Modified Binary Search](searching-sorting-patterns/modified-binary-search/README.md) | Full | Not started | — | Study next | — |
| [Two Heaps](searching-sorting-patterns/two-heaps/README.md) | Full | Not started | — | Study next | — |
| [Top "K" Elements](searching-sorting-patterns/top-k-elements/README.md) | Full | Not started | — | Study next | — |
| [K-way Merge](searching-sorting-patterns/k-way-merge/README.md) | Full | Not started | — | Study next | — |

### Recommended study order (searching & sorting)

1. **Modified Binary Search** — the foundational halving skill.
2. **Top K Elements** — simplest heap application.
3. **Two Heaps** — two heaps working together for a median.
4. **K-way Merge** — a different heap use, one slot per source list.

---

## Tree & Graph Patterns

| Pattern | Tier | Status | Last Revised | Next Revision | Confidence (1-5) |
|---------|------|--------|---------------|----------------|-------------------|
| [Tree BFS](tree-graph-patterns/tree-bfs/README.md) | Full | Not started | — | Study next | — |
| [Tree DFS](tree-graph-patterns/tree-dfs/README.md) | Full | Not started | — | Study next | — |
| [Graph BFS/DFS](tree-graph-patterns/graph-bfs-dfs/README.md) | Full | Not started | — | Study next | — |
| [Topological Sort](tree-graph-patterns/topological-sort/README.md) | Full | Not started | — | Study next | — |
| [Union Find](tree-graph-patterns/union-find/README.md) | Full | Not started | — | Study next | — |
| [Dijkstra's Algorithm](tree-graph-patterns/dijkstras-algorithm/README.md) | Full | Not started | — | Study next | — |

### Recommended study order (tree & graph)

1. **Tree BFS** and **Tree DFS** together — the two traversal shapes on the simplest structure.
2. **Graph BFS/DFS** — the same traversals generalized with a `visited` set.
3. **Dijkstra's Algorithm** — swap BFS's FIFO queue for a min-heap once edges carry non-negative weights.
4. **Topological Sort** — builds on graph BFS (Kahn's) or DFS (post-order + reverse).
5. **Union Find** — a genuinely different tool: connectivity without traversal.

---

## Recursion & Backtracking Patterns

| Pattern | Tier | Status | Last Revised | Next Revision | Confidence (1-5) |
|---------|------|--------|---------------|----------------|-------------------|
| [Subsets](recursion-backtracking-patterns/subsets/README.md) | Full | Not started | — | Study next | — |
| [Backtracking](recursion-backtracking-patterns/backtracking/README.md) | Full | Not started | — | Study next | — |

### Recommended study order (recursion & backtracking)

1. **Subsets** — include/exclude recursion tree with no pruning.
2. **Backtracking** — add the constraint check and undo step to the same shape.

---

## Dynamic Programming Patterns

| Pattern | Tier | Status | Last Revised | Next Revision | Confidence (1-5) |
|---------|------|--------|---------------|----------------|-------------------|
| [0/1 Knapsack](dynamic-programming-patterns/0-1-knapsack/README.md) | Full | Not started | — | Study next | — |
| [Unbounded Knapsack](dynamic-programming-patterns/unbounded-knapsack/README.md) | Full | Not started | — | Study next | — |
| [Longest Common Subsequence](dynamic-programming-patterns/longest-common-subsequence/README.md) | Full | Not started | — | Study next | — |
| [Longest Increasing Subsequence](dynamic-programming-patterns/longest-increasing-subsequence/README.md) | Full | Not started | — | Study next | — |
| [Palindromic Subsequence](dynamic-programming-patterns/palindromic-subsequence/README.md) | Full | Not started | — | Study next | — |
| [DP on Grids](dynamic-programming-patterns/dp-on-grids/README.md) | Full | Not started | — | Study next | — |
| [Bitmask DP](dynamic-programming-patterns/bitmask-dp/README.md) | Full | Not started | — | Study next | — |

### Recommended study order (dynamic programming)

1. **0/1 Knapsack** — the canonical DP template.
2. **Unbounded Knapsack** — one change (reuse) to the same template.
3. **DP on Grids** — the most visual 2D DP.
4. **Longest Common Subsequence** — the two-sequence 2D template.
5. **Longest Increasing Subsequence** — 1D DP with a subtler recurrence + O(n log n) optimization.
6. **Palindromic Subsequence/Substring** — interval DP, tackled last.
7. **Bitmask DP** — subset-identity-keyed state, once Subsets and 0/1 Knapsack are both comfortable.

---

## Greedy Patterns

| Pattern | Tier | Status | Last Revised | Next Revision | Confidence (1-5) |
|---------|------|--------|---------------|----------------|-------------------|
| [Greedy](greedy-patterns/greedy/README.md) | Full | Not started | — | Study next | — |

### Recommended study order (greedy)

1. **Greedy** — start with interval scheduling (clearest exchange-argument proof), then jump games and assignment problems.

---

## Advanced Data Structure Patterns

| Pattern | Tier | Status | Last Revised | Next Revision | Confidence (1-5) |
|---------|------|--------|---------------|----------------|-------------------|
| [Monotonic Stack/Queue](advanced-ds-patterns/monotonic-stack-queue/README.md) | Full | Not started | — | Study next | — |
| [Bit Manipulation](advanced-ds-patterns/bit-manipulation/README.md) | Full | Not started | — | Study next | — |
| [Trie](advanced-ds-patterns/trie/README.md) | Full | Not started | — | Study next | — |
| [Segment Tree / Fenwick Tree](advanced-ds-patterns/segment-tree-fenwick-tree/README.md) | Full | Not started | — | Study next | — |
| [LRU Cache (Design With Data Structures)](advanced-ds-patterns/lru-cache/README.md) | Full | Not started | — | Study next | — |

### Recommended study order (advanced data structures)

1. **Monotonic Stack/Queue** — smallest conceptual jump from plain arrays.
2. **Bit Manipulation** — a toolbox that reappears inside other patterns.
3. **Trie** — a new tree shape built from familiar traversal instincts.
4. **LRU Cache** — hash map + doubly linked list combined for O(1) design questions.
5. **Segment Tree / Fenwick Tree** — most involved; save for last, after Prefix Sum.

> **Legend — Status:** `Not started` = ready to study, not yet studied · `Learned` = studied at least once, on the revision ladder.

---

## Status overview

All 33 patterns are now **Full tier** — every one has README + `code.cpp` + `exercises.md` + `cheatsheet.md` + `images/` (recognition / flow / trace) + `problems/` (4 worked solutions each).

| Family | Patterns | Full tier |
|--------|----------|-----------|
| Array & String | 6 | Two Pointers, Sliding Window, Prefix Sum, Cyclic Sort, Merge Intervals, Kadane's Algorithm |
| Linked List | 2 | Fast & Slow Pointers, In-place Reversal |
| Searching & Sorting | 4 | Modified Binary Search, Two Heaps, Top K Elements, K-way Merge |
| Tree & Graph | 6 | Tree BFS, Tree DFS, Graph BFS/DFS, Topological Sort, Union Find, Dijkstra's Algorithm |
| Recursion & Backtracking | 2 | Subsets, Backtracking |
| Dynamic Programming | 7 | 0/1 Knapsack, Unbounded Knapsack, LCS, LIS, Palindromic Subsequence, DP on Grids, Bitmask DP |
| Greedy | 1 | Greedy |
| Advanced Data Structures | 5 | Monotonic Stack/Queue, Bit Manipulation, Trie, Segment Tree/Fenwick Tree, LRU Cache |
| **Total** | **33** | **33** |

---

*Convention: every pattern lives in its own folder `pattern-name/` with README + `code.cpp` + `exercises.md` + `cheatsheet.md` + `images/` + `problems/`. See [`../repository_template_prompt.md`](../repository_template_prompt.md) for the master content template.*
