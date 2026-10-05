# DSA in 6 Weeks — The Plan

**One pattern a day, Mon–Sat, 4 LeetCode problems each. Sunday = review + catch-up.**
**Start Mon 2026-10-05 → finish Sun 2026-11-15. 144 problems total.**

You **solve on LeetCode**. This repo only tells you *which* problems, gives you the pattern in 10 minutes
before you start, and has a worked solution to **compare against after** you've solved (or given up on) each one.

Don't think about what to do. Open Claude and type **`/today`** — it tells you the next problem, keeps the time,
gives hints when you're stuck, and ticks off the day.

---

## The 6 rules

1. **Code first, read second.** Before LeetCode you read only the cheatsheet (10 min). Never the full README.
2. **Stuck halfway through the time box? Ask for a hint, not the answer.** Paste your LeetCode code to Claude.
3. **Time box over? Learn it, then re-type it from memory.** Open the *compare* file, understand it, close it,
   write it on LeetCode yourself. Then add the problem to *Redo on Sunday* below. Never copy-paste.
4. **2 hours is the target. 1 hour is the minimum. Zero is not allowed.** A 1-hour day still keeps the streak.
5. **Don't improve the material during a session.** Spotted a typo or want a better diagram? One line under
   *Parked* at the bottom, and keep going. Polishing notes feels like progress — it isn't.
6. **Behind? Use Sunday.** Never start two new patterns on one weekday. Phone in another room, one timer running.

---

## A 2-hour day

| Time | Block | What you do |
|---|---|---|
| 0:00 – 0:10 | **Warm-up** | Recall questions for patterns due in [`INDEX.md`](./INDEX.md). Answer from memory, no reading. |
| 0:10 – 0:20 | **Meet the pattern** | Read `cheatsheet.md` + the README header (one-liner + snippet). You should know the template before LeetCode. |
| 0:20 – 1:50 | **LeetCode** | Today's 4 problems, in order (easiest first). Time box each: **Easy 15 · Medium 25 · Hard 35 min**. 5-min break after the 2nd. Each accepted solution → open its *compare* link for 2 min: what did theirs do differently? |
| 1:50 – 2:00 | **Lock it in** | 3 recall questions from the cheatsheet, tick the boxes, add one line to the *Log*. |

**1-hour day (minimum):** warm-up 5 → cheatsheet 5 → the first 2 problems (40) → lock it in 10.
Anything unticked moves to Sunday.

**Sunday:** (1) unticked problems from the week, (2) everything in *Redo on Sunday*, solved from scratch —
no compare file, (3) revisions due in `INDEX.md`. Then rest — you earned it.

Want more? Each pattern's `exercises.md` has extra LeetCode problems (Easy / Medium / Hard headings) with no
solutions — good for Sundays once you're caught up, or after week 6.

---

## Schedule

Tick each problem as you get **Accepted on LeetCode** (counts even if you needed the compare file — then also add it to *Redo*).
🔒 = LeetCode Premium; skip it if you don't have Premium and tick it as skipped.
↻ = same problem as an earlier day; solve it again using *today's* technique — that's the point.

### Week 1 — Array & String

**Day 1 · Mon 10-05 · [Two Pointers](array-string-patterns/two-pointers/)**

- [ ] [167. Two Sum II - Input Array Is Sorted](https://leetcode.com/problems/two-sum-ii-input-array-is-sorted/) · Easy · [compare](array-string-patterns/two-pointers/problems/01-pair-with-target-sum.cpp)
- [ ] [26. Remove Duplicates from Sorted Array](https://leetcode.com/problems/remove-duplicates-from-sorted-array/) · Easy · [compare](array-string-patterns/two-pointers/problems/02-remove-duplicates-from-sorted-array.cpp)
- [ ] [15. 3Sum](https://leetcode.com/problems/3sum/) · Medium · [compare](array-string-patterns/two-pointers/problems/03-3sum.cpp)
- [ ] [11. Container With Most Water](https://leetcode.com/problems/container-with-most-water/) · Medium · [compare](array-string-patterns/two-pointers/problems/04-container-with-most-water.cpp)

**Day 2 · Tue 10-06 · [Sliding Window](array-string-patterns/sliding-window/)**

- [ ] [643. Maximum Average Subarray I](https://leetcode.com/problems/maximum-average-subarray-i/) · Easy · [compare](array-string-patterns/sliding-window/problems/01-max-sum-subarray-of-size-k.cpp)
- [ ] [3. Longest Substring Without Repeating Characters](https://leetcode.com/problems/longest-substring-without-repeating-characters/) · Medium · [compare](array-string-patterns/sliding-window/problems/02-longest-substring-without-repeating-characters.cpp)
- [ ] [209. Minimum Size Subarray Sum](https://leetcode.com/problems/minimum-size-subarray-sum/) · Medium · [compare](array-string-patterns/sliding-window/problems/04-minimum-size-subarray-sum.cpp)
- [ ] [76. Minimum Window Substring](https://leetcode.com/problems/minimum-window-substring/) · Hard · [compare](array-string-patterns/sliding-window/problems/03-minimum-window-substring.cpp)

**Day 3 · Wed 10-07 · [Prefix Sum](array-string-patterns/prefix-sum/)**

- [ ] [303. Range Sum Query - Immutable](https://leetcode.com/problems/range-sum-query-immutable/) · Easy · [compare](array-string-patterns/prefix-sum/problems/01-range-sum-query-immutable.cpp)
- [ ] [560. Subarray Sum Equals K](https://leetcode.com/problems/subarray-sum-equals-k/) · Medium · [compare](array-string-patterns/prefix-sum/problems/02-subarray-sum-equals-k.cpp)
- [ ] [525. Contiguous Array](https://leetcode.com/problems/contiguous-array/) · Medium · [compare](array-string-patterns/prefix-sum/problems/03-contiguous-array.cpp)
- [ ] [238. Product of Array Except Self](https://leetcode.com/problems/product-of-array-except-self/) · Medium · [compare](array-string-patterns/prefix-sum/problems/04-product-of-array-except-self.cpp)

**Day 4 · Thu 10-08 · [Kadane's Algorithm](array-string-patterns/kadanes-algorithm/)**

- [ ] [121. Best Time to Buy and Sell Stock](https://leetcode.com/problems/best-time-to-buy-and-sell-stock/) · Easy · [compare](array-string-patterns/kadanes-algorithm/problems/03-best-time-to-buy-and-sell-stock.cpp)
- [ ] [53. Maximum Subarray](https://leetcode.com/problems/maximum-subarray/) · Medium · [compare](array-string-patterns/kadanes-algorithm/problems/01-maximum-subarray.cpp)
- [ ] [152. Maximum Product Subarray](https://leetcode.com/problems/maximum-product-subarray/) · Medium · [compare](array-string-patterns/kadanes-algorithm/problems/02-maximum-product-subarray.cpp)
- [ ] [918. Maximum Sum Circular Subarray](https://leetcode.com/problems/maximum-sum-circular-subarray/) · Medium · [compare](array-string-patterns/kadanes-algorithm/problems/04-maximum-sum-circular-subarray.cpp)

**Day 5 · Fri 10-09 · [Cyclic Sort](array-string-patterns/cyclic-sort/)**

- [ ] [268. Missing Number](https://leetcode.com/problems/missing-number/) · Easy · [compare](array-string-patterns/cyclic-sort/problems/01-missing-number.cpp)
- [ ] [448. Find All Numbers Disappeared in an Array](https://leetcode.com/problems/find-all-numbers-disappeared-in-an-array/) · Easy · [compare](array-string-patterns/cyclic-sort/problems/02-find-all-numbers-disappeared-in-an-array.cpp)
- [ ] [287. Find the Duplicate Number](https://leetcode.com/problems/find-the-duplicate-number/) · Medium · [compare](array-string-patterns/cyclic-sort/problems/03-find-the-duplicate-number.cpp)
- [ ] [41. First Missing Positive](https://leetcode.com/problems/first-missing-positive/) · Hard · [compare](array-string-patterns/cyclic-sort/problems/04-first-missing-positive.cpp)

**Day 6 · Sat 10-10 · [Merge Intervals](array-string-patterns/merge-intervals/)**

- [ ] [56. Merge Intervals](https://leetcode.com/problems/merge-intervals/) · Medium · [compare](array-string-patterns/merge-intervals/problems/01-merge-intervals.cpp)
- [ ] [57. Insert Interval](https://leetcode.com/problems/insert-interval/) · Medium · [compare](array-string-patterns/merge-intervals/problems/02-insert-interval.cpp)
- [ ] [435. Non-overlapping Intervals](https://leetcode.com/problems/non-overlapping-intervals/) · Medium · [compare](array-string-patterns/merge-intervals/problems/03-non-overlapping-intervals.cpp)
- [ ] [452. Minimum Number of Arrows to Burst Balloons](https://leetcode.com/problems/minimum-number-of-arrows-to-burst-balloons/) · Medium · [compare](array-string-patterns/merge-intervals/problems/04-minimum-arrows-to-burst-balloons.cpp)

**Sun 10-11 · Review + catch-up**

- [ ] Done

### Week 2 — Linked List, Searching & Sorting

**Day 7 · Mon 10-12 · [Fast & Slow Pointers](linked-list-patterns/fast-slow-pointers/)**

- [ ] [141. Linked List Cycle](https://leetcode.com/problems/linked-list-cycle/) · Easy · [compare](linked-list-patterns/fast-slow-pointers/problems/01-linked-list-cycle.cpp)
- [ ] [876. Middle of the Linked List](https://leetcode.com/problems/middle-of-the-linked-list/) · Easy · [compare](linked-list-patterns/fast-slow-pointers/problems/02-middle-of-the-linked-list.cpp)
- [ ] [202. Happy Number](https://leetcode.com/problems/happy-number/) · Easy · [compare](linked-list-patterns/fast-slow-pointers/problems/03-happy-number.cpp)
- [ ] [234. Palindrome Linked List](https://leetcode.com/problems/palindrome-linked-list/) · Easy · [compare](linked-list-patterns/fast-slow-pointers/problems/04-palindrome-linked-list.cpp)

**Day 8 · Tue 10-13 · [In-place Reversal](linked-list-patterns/in-place-reversal/)**

- [ ] [206. Reverse Linked List](https://leetcode.com/problems/reverse-linked-list/) · Easy · [compare](linked-list-patterns/in-place-reversal/problems/01-reverse-linked-list.cpp)
- [ ] [92. Reverse Linked List II](https://leetcode.com/problems/reverse-linked-list-ii/) · Medium · [compare](linked-list-patterns/in-place-reversal/problems/02-reverse-linked-list-ii.cpp)
- [ ] [24. Swap Nodes in Pairs](https://leetcode.com/problems/swap-nodes-in-pairs/) · Medium · [compare](linked-list-patterns/in-place-reversal/problems/04-swap-nodes-in-pairs.cpp)
- [ ] [25. Reverse Nodes in k-Group](https://leetcode.com/problems/reverse-nodes-in-k-group/) · Hard · [compare](linked-list-patterns/in-place-reversal/problems/03-reverse-nodes-in-k-group.cpp)

**Day 9 · Wed 10-14 · [Modified Binary Search](searching-sorting-patterns/modified-binary-search/)**

- [ ] [704. Binary Search](https://leetcode.com/problems/binary-search/) · Easy · [compare](searching-sorting-patterns/modified-binary-search/problems/01-binary-search.cpp)
- [ ] [34. Find First and Last Position of Element in Sorted Array](https://leetcode.com/problems/find-first-and-last-position-of-element-in-sorted-array/) · Medium · [compare](searching-sorting-patterns/modified-binary-search/problems/02-find-first-and-last-position-of-element-in-sorted-array.cpp)
- [ ] [33. Search in Rotated Sorted Array](https://leetcode.com/problems/search-in-rotated-sorted-array/) · Medium · [compare](searching-sorting-patterns/modified-binary-search/problems/03-search-in-rotated-sorted-array.cpp)
- [ ] [153. Find Minimum in Rotated Sorted Array](https://leetcode.com/problems/find-minimum-in-rotated-sorted-array/) · Medium · [compare](searching-sorting-patterns/modified-binary-search/problems/04-find-minimum-in-rotated-sorted-array.cpp)

**Day 10 · Thu 10-15 · [Top "K" Elements](searching-sorting-patterns/top-k-elements/)**

- [ ] [215. Kth Largest Element in an Array](https://leetcode.com/problems/kth-largest-element-in-an-array/) · Medium · [compare](searching-sorting-patterns/top-k-elements/problems/01-kth-largest-element-in-an-array.cpp)
- [ ] [347. Top K Frequent Elements](https://leetcode.com/problems/top-k-frequent-elements/) · Medium · [compare](searching-sorting-patterns/top-k-elements/problems/02-top-k-frequent-elements.cpp)
- [ ] [973. K Closest Points to Origin](https://leetcode.com/problems/k-closest-points-to-origin/) · Medium · [compare](searching-sorting-patterns/top-k-elements/problems/03-k-closest-points-to-origin.cpp)
- [ ] [692. Top K Frequent Words](https://leetcode.com/problems/top-k-frequent-words/) · Medium · [compare](searching-sorting-patterns/top-k-elements/problems/04-top-k-frequent-words.cpp)

**Day 11 · Fri 10-16 · [Two Heaps](searching-sorting-patterns/two-heaps/)**

- [ ] [703. Kth Largest Element in a Stream](https://leetcode.com/problems/kth-largest-element-in-a-stream/) · Easy · [compare](searching-sorting-patterns/two-heaps/problems/04-kth-largest-element-in-a-stream.cpp)
- [ ] [295. Find Median from Data Stream](https://leetcode.com/problems/find-median-from-data-stream/) · Hard · [compare](searching-sorting-patterns/two-heaps/problems/01-find-median-from-data-stream.cpp)
- [ ] [480. Sliding Window Median](https://leetcode.com/problems/sliding-window-median/) · Hard · [compare](searching-sorting-patterns/two-heaps/problems/02-sliding-window-median.cpp)
- [ ] [502. IPO](https://leetcode.com/problems/ipo/) · Hard · [compare](searching-sorting-patterns/two-heaps/problems/03-ipo.cpp)

**Day 12 · Sat 10-17 · [K-way Merge](searching-sorting-patterns/k-way-merge/)**

- [ ] [378. Kth Smallest Element in a Sorted Matrix](https://leetcode.com/problems/kth-smallest-element-in-a-sorted-matrix/) · Medium · [compare](searching-sorting-patterns/k-way-merge/problems/02-kth-smallest-element-in-a-sorted-matrix.cpp)
- [ ] [373. Find K Pairs with Smallest Sums](https://leetcode.com/problems/find-k-pairs-with-smallest-sums/) · Medium · [compare](searching-sorting-patterns/k-way-merge/problems/03-find-k-pairs-with-smallest-sums.cpp)
- [ ] [23. Merge k Sorted Lists](https://leetcode.com/problems/merge-k-sorted-lists/) · Hard · [compare](searching-sorting-patterns/k-way-merge/problems/01-merge-k-sorted-lists.cpp)
- [ ] [632. Smallest Range Covering Elements from K Lists](https://leetcode.com/problems/smallest-range-covering-elements-from-k-lists/) · Hard · [compare](searching-sorting-patterns/k-way-merge/problems/04-smallest-range-covering-elements-from-k-lists.cpp)

**Sun 10-18 · Review + catch-up**

- [ ] Done

### Week 3 — Trees & Shortest Paths

**Day 13 · Mon 10-19 · [Tree BFS](tree-graph-patterns/tree-bfs/)**

- [ ] [111. Minimum Depth of Binary Tree](https://leetcode.com/problems/minimum-depth-of-binary-tree/) · Easy · [compare](tree-graph-patterns/tree-bfs/problems/03-minimum-depth-of-binary-tree.cpp)
- [ ] [102. Binary Tree Level Order Traversal](https://leetcode.com/problems/binary-tree-level-order-traversal/) · Medium · [compare](tree-graph-patterns/tree-bfs/problems/01-binary-tree-level-order-traversal.cpp)
- [ ] [103. Binary Tree Zigzag Level Order Traversal](https://leetcode.com/problems/binary-tree-zigzag-level-order-traversal/) · Medium · [compare](tree-graph-patterns/tree-bfs/problems/02-binary-tree-zigzag-level-order-traversal.cpp)
- [ ] [117. Populating Next Right Pointers in Each Node II](https://leetcode.com/problems/populating-next-right-pointers-in-each-node-ii/) · Medium · [compare](tree-graph-patterns/tree-bfs/problems/04-populating-next-right-pointers-ii.cpp)

**Day 14 · Tue 10-20 · [Tree DFS](tree-graph-patterns/tree-dfs/)**

- [ ] [112. Path Sum](https://leetcode.com/problems/path-sum/) · Easy · [compare](tree-graph-patterns/tree-dfs/problems/01-path-sum.cpp)
- [ ] [257. Binary Tree Paths](https://leetcode.com/problems/binary-tree-paths/) · Easy · [compare](tree-graph-patterns/tree-dfs/problems/02-binary-tree-paths.cpp)
- [ ] [113. Path Sum II](https://leetcode.com/problems/path-sum-ii/) · Medium · [compare](tree-graph-patterns/tree-dfs/problems/03-path-sum-ii.cpp)
- [ ] [124. Binary Tree Maximum Path Sum](https://leetcode.com/problems/binary-tree-maximum-path-sum/) · Hard · [compare](tree-graph-patterns/tree-dfs/problems/04-binary-tree-maximum-path-sum.cpp)

**Day 15 · Wed 10-21 · [Graph BFS/DFS](tree-graph-patterns/graph-bfs-dfs/)**

- [ ] [200. Number of Islands](https://leetcode.com/problems/number-of-islands/) · Medium · [compare](tree-graph-patterns/graph-bfs-dfs/problems/01-number-of-islands.cpp)
- [ ] [547. Number of Provinces](https://leetcode.com/problems/number-of-provinces/) · Medium · [compare](tree-graph-patterns/graph-bfs-dfs/problems/02-number-of-provinces.cpp)
- [ ] [785. Is Graph Bipartite?](https://leetcode.com/problems/is-graph-bipartite/) · Medium · [compare](tree-graph-patterns/graph-bfs-dfs/problems/03-is-graph-bipartite.cpp)
- [ ] [127. Word Ladder](https://leetcode.com/problems/word-ladder/) · Hard · [compare](tree-graph-patterns/graph-bfs-dfs/problems/04-word-ladder.cpp)

**Day 16 · Thu 10-22 · [Dijkstra's Algorithm](tree-graph-patterns/dijkstras-algorithm/)**

- [ ] [743. Network Delay Time](https://leetcode.com/problems/network-delay-time/) · Medium · [compare](tree-graph-patterns/dijkstras-algorithm/problems/01-network-delay-time.cpp)
- [ ] [1631. Path With Minimum Effort](https://leetcode.com/problems/path-with-minimum-effort/) · Medium · [compare](tree-graph-patterns/dijkstras-algorithm/problems/02-path-with-minimum-effort.cpp)
- [ ] [787. Cheapest Flights Within K Stops](https://leetcode.com/problems/cheapest-flights-within-k-stops/) · Medium · [compare](tree-graph-patterns/dijkstras-algorithm/problems/03-cheapest-flights-within-k-stops.cpp)
- [ ] [1334. Find the City With the Smallest Number of Neighbors at a Threshold Distance](https://leetcode.com/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/) · Medium · [compare](tree-graph-patterns/dijkstras-algorithm/problems/04-find-the-city-with-smallest-number-of-neighbors-at-a-threshold-distance.cpp)

**Day 17 · Fri 10-23 · [Bellman-Ford](tree-graph-patterns/bellman-ford/)**

- [ ] [743. Network Delay Time](https://leetcode.com/problems/network-delay-time/) · Medium · ↻ seen on Day 16 — solve it again *with this pattern* · [compare](tree-graph-patterns/bellman-ford/problems/01-network-delay-time.cpp)
- [ ] [787. Cheapest Flights Within K Stops](https://leetcode.com/problems/cheapest-flights-within-k-stops/) · Medium · ↻ seen on Day 16 — solve it again *with this pattern* · [compare](tree-graph-patterns/bellman-ford/problems/02-cheapest-flights-within-k-stops.cpp)
- [ ] [2093. Minimum Cost to Reach City With Discounts](https://leetcode.com/problems/minimum-cost-to-reach-city-with-discounts/) · Medium · 🔒 Premium — skip if you don't have it · [compare](tree-graph-patterns/bellman-ford/problems/04-minimum-cost-to-reach-city-with-discounts.cpp)
- [ ] [1928. Minimum Cost to Reach Destination in Time](https://leetcode.com/problems/minimum-cost-to-reach-destination-in-time/) · Hard · [compare](tree-graph-patterns/bellman-ford/problems/03-minimum-cost-to-reach-destination-in-time.cpp)

**Day 18 · Sat 10-24 · [Floyd-Warshall](tree-graph-patterns/floyd-warshall/)**

- [ ] [1334. Find the City With the Smallest Number of Neighbors at a Threshold Distance](https://leetcode.com/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/) · Medium · ↻ seen on Day 16 — solve it again *with this pattern* · [compare](tree-graph-patterns/floyd-warshall/problems/01-find-the-city-with-smallest-number-of-neighbors-at-a-threshold-distance.cpp)
- [ ] [1462. Course Schedule IV](https://leetcode.com/problems/course-schedule-iv/) · Medium · [compare](tree-graph-patterns/floyd-warshall/problems/02-course-schedule-iv.cpp)
- [ ] [399. Evaluate Division](https://leetcode.com/problems/evaluate-division/) · Medium · [compare](tree-graph-patterns/floyd-warshall/problems/03-evaluate-division.cpp)
- [ ] [2101. Detonate the Maximum Bombs](https://leetcode.com/problems/detonate-the-maximum-bombs/) · Medium · [compare](tree-graph-patterns/floyd-warshall/problems/04-detonate-the-maximum-bombs.cpp)

**Sun 10-25 · Review + catch-up**

- [ ] Done

### Week 4 — Graphs, Recursion, DP begins

**Day 19 · Mon 10-26 · [Topological Sort](tree-graph-patterns/topological-sort/)**

- [ ] [207. Course Schedule](https://leetcode.com/problems/course-schedule/) · Medium · [compare](tree-graph-patterns/topological-sort/problems/01-course-schedule.cpp)
- [ ] [210. Course Schedule II](https://leetcode.com/problems/course-schedule-ii/) · Medium · [compare](tree-graph-patterns/topological-sort/problems/02-course-schedule-ii.cpp)
- [ ] [444. Sequence Reconstruction](https://leetcode.com/problems/sequence-reconstruction/) · Medium · 🔒 Premium — skip if you don't have it · [compare](tree-graph-patterns/topological-sort/problems/03-sequence-reconstruction.cpp)
- [ ] [269. Alien Dictionary](https://leetcode.com/problems/alien-dictionary/) · Hard · 🔒 Premium — skip if you don't have it · [compare](tree-graph-patterns/topological-sort/problems/04-alien-dictionary.cpp)

**Day 20 · Tue 10-27 · [Union Find](tree-graph-patterns/union-find/)**

- [ ] [547. Number of Provinces](https://leetcode.com/problems/number-of-provinces/) · Medium · ↻ seen on Day 15 — solve it again *with this pattern* · [compare](tree-graph-patterns/union-find/problems/01-number-of-provinces.cpp)
- [ ] [684. Redundant Connection](https://leetcode.com/problems/redundant-connection/) · Medium · [compare](tree-graph-patterns/union-find/problems/02-redundant-connection.cpp)
- [ ] [721. Accounts Merge](https://leetcode.com/problems/accounts-merge/) · Medium · [compare](tree-graph-patterns/union-find/problems/03-accounts-merge.cpp)
- [ ] [305. Number of Islands II](https://leetcode.com/problems/number-of-islands-ii/) · Hard · 🔒 Premium — skip if you don't have it · [compare](tree-graph-patterns/union-find/problems/04-number-of-islands-ii.cpp)

**Day 21 · Wed 10-28 · [Minimum Spanning Tree](tree-graph-patterns/mst-kruskal-prim/)**

- [ ] [1584. Min Cost to Connect All Points](https://leetcode.com/problems/min-cost-to-connect-all-points/) · Medium · [compare](tree-graph-patterns/mst-kruskal-prim/problems/01-min-cost-to-connect-all-points.cpp)
- [ ] [1135. Connecting Cities With Minimum Cost](https://leetcode.com/problems/connecting-cities-with-minimum-cost/) · Medium · 🔒 Premium — skip if you don't have it · [compare](tree-graph-patterns/mst-kruskal-prim/problems/02-connecting-cities-with-minimum-cost.cpp)
- [ ] [1168. Optimize Water Distribution in a Village](https://leetcode.com/problems/optimize-water-distribution-in-a-village/) · Hard · 🔒 Premium — skip if you don't have it · [compare](tree-graph-patterns/mst-kruskal-prim/problems/03-optimize-water-distribution-in-a-village.cpp)
- [ ] [1489. Find Critical and Pseudo-Critical Edges in Minimum Spanning Tree](https://leetcode.com/problems/find-critical-and-pseudo-critical-edges-in-minimum-spanning-tree/) · Hard · [compare](tree-graph-patterns/mst-kruskal-prim/problems/04-find-critical-and-pseudo-critical-edges.cpp)

**Day 22 · Thu 10-29 · [Subsets](recursion-backtracking-patterns/subsets/)**

- [ ] [78. Subsets](https://leetcode.com/problems/subsets/) · Medium · [compare](recursion-backtracking-patterns/subsets/problems/01-subsets.cpp)
- [ ] [90. Subsets II](https://leetcode.com/problems/subsets-ii/) · Medium · [compare](recursion-backtracking-patterns/subsets/problems/02-subsets-ii.cpp)
- [ ] [46. Permutations](https://leetcode.com/problems/permutations/) · Medium · [compare](recursion-backtracking-patterns/subsets/problems/03-permutations.cpp)
- [ ] [39. Combination Sum](https://leetcode.com/problems/combination-sum/) · Medium · [compare](recursion-backtracking-patterns/subsets/problems/04-combination-sum.cpp)

**Day 23 · Fri 10-30 · [Backtracking](recursion-backtracking-patterns/backtracking/)**

- [ ] [79. Word Search](https://leetcode.com/problems/word-search/) · Medium · [compare](recursion-backtracking-patterns/backtracking/problems/03-word-search.cpp)
- [ ] [131. Palindrome Partitioning](https://leetcode.com/problems/palindrome-partitioning/) · Medium · [compare](recursion-backtracking-patterns/backtracking/problems/04-palindrome-partitioning.cpp)
- [ ] [51. N-Queens](https://leetcode.com/problems/n-queens/) · Hard · [compare](recursion-backtracking-patterns/backtracking/problems/01-n-queens.cpp)
- [ ] [37. Sudoku Solver](https://leetcode.com/problems/sudoku-solver/) · Hard · [compare](recursion-backtracking-patterns/backtracking/problems/02-sudoku-solver.cpp)

**Day 24 · Sat 10-31 · [0/1 Knapsack](dynamic-programming-patterns/0-1-knapsack/)**

- [ ] [416. Partition Equal Subset Sum](https://leetcode.com/problems/partition-equal-subset-sum/) · Medium · [compare](dynamic-programming-patterns/0-1-knapsack/problems/01-partition-equal-subset-sum.cpp)
- [ ] [1049. Last Stone Weight II](https://leetcode.com/problems/last-stone-weight-ii/) · Medium · [compare](dynamic-programming-patterns/0-1-knapsack/problems/02-last-stone-weight-ii.cpp)
- [ ] [494. Target Sum](https://leetcode.com/problems/target-sum/) · Medium · [compare](dynamic-programming-patterns/0-1-knapsack/problems/03-target-sum.cpp)
- [ ] [474. Ones and Zeroes](https://leetcode.com/problems/ones-and-zeroes/) · Medium · [compare](dynamic-programming-patterns/0-1-knapsack/problems/04-ones-and-zeroes.cpp)

**Sun 11-01 · Review + catch-up**

- [ ] Done

### Week 5 — Dynamic Programming

**Day 25 · Mon 11-02 · [Unbounded Knapsack](dynamic-programming-patterns/unbounded-knapsack/)**

- [ ] [322. Coin Change](https://leetcode.com/problems/coin-change/) · Medium · [compare](dynamic-programming-patterns/unbounded-knapsack/problems/01-coin-change.cpp)
- [ ] [518. Coin Change II](https://leetcode.com/problems/coin-change-ii/) · Medium · [compare](dynamic-programming-patterns/unbounded-knapsack/problems/02-coin-change-ii.cpp)
- [ ] [279. Perfect Squares](https://leetcode.com/problems/perfect-squares/) · Medium · [compare](dynamic-programming-patterns/unbounded-knapsack/problems/03-perfect-squares.cpp)
- [ ] [343. Integer Break](https://leetcode.com/problems/integer-break/) · Medium · [compare](dynamic-programming-patterns/unbounded-knapsack/problems/04-integer-break.cpp)

**Day 26 · Tue 11-03 · [DP on Grids](dynamic-programming-patterns/dp-on-grids/)**

- [ ] [62. Unique Paths](https://leetcode.com/problems/unique-paths/) · Medium · [compare](dynamic-programming-patterns/dp-on-grids/problems/01-unique-paths.cpp)
- [ ] [64. Minimum Path Sum](https://leetcode.com/problems/minimum-path-sum/) · Medium · [compare](dynamic-programming-patterns/dp-on-grids/problems/02-minimum-path-sum.cpp)
- [ ] [63. Unique Paths II](https://leetcode.com/problems/unique-paths-ii/) · Medium · [compare](dynamic-programming-patterns/dp-on-grids/problems/03-unique-paths-ii.cpp)
- [ ] [931. Minimum Falling Path Sum](https://leetcode.com/problems/minimum-falling-path-sum/) · Medium · [compare](dynamic-programming-patterns/dp-on-grids/problems/04-minimum-falling-path-sum.cpp)

**Day 27 · Wed 11-04 · [Longest Common Subsequence](dynamic-programming-patterns/longest-common-subsequence/)**

- [ ] [1143. Longest Common Subsequence](https://leetcode.com/problems/longest-common-subsequence/) · Medium · [compare](dynamic-programming-patterns/longest-common-subsequence/problems/01-longest-common-subsequence.cpp)
- [ ] [72. Edit Distance](https://leetcode.com/problems/edit-distance/) · Medium · [compare](dynamic-programming-patterns/longest-common-subsequence/problems/02-edit-distance.cpp)
- [ ] [583. Delete Operation for Two Strings](https://leetcode.com/problems/delete-operation-for-two-strings/) · Medium · [compare](dynamic-programming-patterns/longest-common-subsequence/problems/04-delete-operation-for-two-strings.cpp)
- [ ] [1092. Shortest Common Supersequence](https://leetcode.com/problems/shortest-common-supersequence/) · Hard · [compare](dynamic-programming-patterns/longest-common-subsequence/problems/03-shortest-common-supersequence.cpp)

**Day 28 · Thu 11-05 · [Longest Increasing Subsequence](dynamic-programming-patterns/longest-increasing-subsequence/)**

- [ ] [300. Longest Increasing Subsequence](https://leetcode.com/problems/longest-increasing-subsequence/) · Medium · [compare](dynamic-programming-patterns/longest-increasing-subsequence/problems/01-longest-increasing-subsequence.cpp)
- [ ] [673. Number of Longest Increasing Subsequence](https://leetcode.com/problems/number-of-longest-increasing-subsequence/) · Medium · [compare](dynamic-programming-patterns/longest-increasing-subsequence/problems/02-number-of-longest-increasing-subsequence.cpp)
- [ ] [646. Maximum Length of Pair Chain](https://leetcode.com/problems/maximum-length-of-pair-chain/) · Medium · [compare](dynamic-programming-patterns/longest-increasing-subsequence/problems/04-maximum-length-of-pair-chain.cpp)
- [ ] [354. Russian Doll Envelopes](https://leetcode.com/problems/russian-doll-envelopes/) · Hard · [compare](dynamic-programming-patterns/longest-increasing-subsequence/problems/03-russian-doll-envelopes.cpp)

**Day 29 · Fri 11-06 · [Palindromic Subsequence](dynamic-programming-patterns/palindromic-subsequence/)**

- [ ] [516. Longest Palindromic Subsequence](https://leetcode.com/problems/longest-palindromic-subsequence/) · Medium · [compare](dynamic-programming-patterns/palindromic-subsequence/problems/01-longest-palindromic-subsequence.cpp)
- [ ] [5. Longest Palindromic Substring](https://leetcode.com/problems/longest-palindromic-substring/) · Medium · [compare](dynamic-programming-patterns/palindromic-subsequence/problems/02-longest-palindromic-substring.cpp)
- [ ] [647. Palindromic Substrings](https://leetcode.com/problems/palindromic-substrings/) · Medium · [compare](dynamic-programming-patterns/palindromic-subsequence/problems/03-palindromic-substrings.cpp)
- [ ] [1312. Minimum Insertion Steps to Make a String Palindrome](https://leetcode.com/problems/minimum-insertion-steps-to-make-a-string-palindrome/) · Hard · [compare](dynamic-programming-patterns/palindromic-subsequence/problems/04-minimum-insertion-steps-to-make-string-palindrome.cpp)

**Day 30 · Sat 11-07 · [Bitmask DP](dynamic-programming-patterns/bitmask-dp/)**

- [ ] [526. Beautiful Arrangement](https://leetcode.com/problems/beautiful-arrangement/) · Medium · [compare](dynamic-programming-patterns/bitmask-dp/problems/01-beautiful-arrangement.cpp)
- [ ] [698. Partition to K Equal Sum Subsets](https://leetcode.com/problems/partition-to-k-equal-sum-subsets/) · Medium · [compare](dynamic-programming-patterns/bitmask-dp/problems/02-partition-to-k-equal-sum-subsets.cpp)
- [ ] [464. Can I Win](https://leetcode.com/problems/can-i-win/) · Medium · [compare](dynamic-programming-patterns/bitmask-dp/problems/03-can-i-win.cpp)
- [ ] [2305. Fair Distribution of Cookies](https://leetcode.com/problems/fair-distribution-of-cookies/) · Medium · [compare](dynamic-programming-patterns/bitmask-dp/problems/04-fair-distribution-of-cookies.cpp)

**Sun 11-08 · Review + catch-up**

- [ ] Done

### Week 6 — Greedy & Advanced Data Structures

**Day 31 · Mon 11-09 · [Greedy](greedy-patterns/greedy/)**

- [ ] [55. Jump Game](https://leetcode.com/problems/jump-game/) · Medium · [compare](greedy-patterns/greedy/problems/01-jump-game.cpp)
- [ ] [45. Jump Game II](https://leetcode.com/problems/jump-game-ii/) · Medium · [compare](greedy-patterns/greedy/problems/02-jump-game-ii.cpp)
- [ ] [134. Gas Station](https://leetcode.com/problems/gas-station/) · Medium · [compare](greedy-patterns/greedy/problems/03-gas-station.cpp)
- [ ] [621. Task Scheduler](https://leetcode.com/problems/task-scheduler/) · Medium · [compare](greedy-patterns/greedy/problems/04-task-scheduler.cpp)

**Day 32 · Tue 11-10 · [Monotonic Stack/Queue](advanced-ds-patterns/monotonic-stack-queue/)**

- [ ] [496. Next Greater Element I](https://leetcode.com/problems/next-greater-element-i/) · Easy · [compare](advanced-ds-patterns/monotonic-stack-queue/problems/01-next-greater-element-i.cpp)
- [ ] [739. Daily Temperatures](https://leetcode.com/problems/daily-temperatures/) · Medium · [compare](advanced-ds-patterns/monotonic-stack-queue/problems/02-daily-temperatures.cpp)
- [ ] [84. Largest Rectangle in Histogram](https://leetcode.com/problems/largest-rectangle-in-histogram/) · Hard · [compare](advanced-ds-patterns/monotonic-stack-queue/problems/03-largest-rectangle-in-histogram.cpp)
- [ ] [239. Sliding Window Maximum](https://leetcode.com/problems/sliding-window-maximum/) · Hard · [compare](advanced-ds-patterns/monotonic-stack-queue/problems/04-sliding-window-maximum.cpp)

**Day 33 · Wed 11-11 · [Bit Manipulation](advanced-ds-patterns/bit-manipulation/)**

- [ ] [136. Single Number](https://leetcode.com/problems/single-number/) · Easy · [compare](advanced-ds-patterns/bit-manipulation/problems/01-single-number.cpp)
- [ ] [191. Number of 1 Bits](https://leetcode.com/problems/number-of-1-bits/) · Easy · [compare](advanced-ds-patterns/bit-manipulation/problems/02-number-of-1-bits.cpp)
- [ ] [338. Counting Bits](https://leetcode.com/problems/counting-bits/) · Easy · [compare](advanced-ds-patterns/bit-manipulation/problems/03-counting-bits.cpp)
- [ ] [260. Single Number III](https://leetcode.com/problems/single-number-iii/) · Medium · [compare](advanced-ds-patterns/bit-manipulation/problems/04-single-number-iii.cpp)

**Day 34 · Thu 11-12 · [Trie](advanced-ds-patterns/trie/)**

- [ ] [208. Implement Trie (Prefix Tree)](https://leetcode.com/problems/implement-trie-prefix-tree/) · Medium · [compare](advanced-ds-patterns/trie/problems/01-implement-trie.cpp)
- [ ] [211. Design Add and Search Words Data Structure](https://leetcode.com/problems/design-add-and-search-words-data-structure/) · Medium · [compare](advanced-ds-patterns/trie/problems/02-design-add-and-search-words-data-structure.cpp)
- [ ] [720. Longest Word in Dictionary](https://leetcode.com/problems/longest-word-in-dictionary/) · Medium · [compare](advanced-ds-patterns/trie/problems/03-longest-word-in-dictionary.cpp)
- [ ] [421. Maximum XOR of Two Numbers in an Array](https://leetcode.com/problems/maximum-xor-of-two-numbers-in-an-array/) · Medium · [compare](advanced-ds-patterns/trie/problems/04-maximum-xor-of-two-numbers-in-an-array.cpp)

**Day 35 · Fri 11-13 · [LRU Cache](advanced-ds-patterns/lru-cache/)**

- [ ] [146. LRU Cache](https://leetcode.com/problems/lru-cache/) · Medium · [compare](advanced-ds-patterns/lru-cache/problems/01-lru-cache.cpp)
- [ ] [1472. Design Browser History](https://leetcode.com/problems/design-browser-history/) · Medium · [compare](advanced-ds-patterns/lru-cache/problems/03-design-browser-history.cpp)
- [ ] [1797. Design Authentication Manager](https://leetcode.com/problems/design-authentication-manager/) · Medium · [compare](advanced-ds-patterns/lru-cache/problems/04-authentication-manager.cpp)
- [ ] [460. LFU Cache](https://leetcode.com/problems/lfu-cache/) · Hard · [compare](advanced-ds-patterns/lru-cache/problems/02-lfu-cache.cpp)

**Day 36 · Sat 11-14 · [Segment Tree / Fenwick Tree](advanced-ds-patterns/segment-tree-fenwick-tree/)**

- [ ] [307. Range Sum Query - Mutable](https://leetcode.com/problems/range-sum-query-mutable/) · Medium · [compare](advanced-ds-patterns/segment-tree-fenwick-tree/problems/01-range-sum-query-mutable.cpp)
- [ ] [315. Count of Smaller Numbers After Self](https://leetcode.com/problems/count-of-smaller-numbers-after-self/) · Hard · [compare](advanced-ds-patterns/segment-tree-fenwick-tree/problems/02-count-of-smaller-numbers-after-self.cpp)
- [ ] [493. Reverse Pairs](https://leetcode.com/problems/reverse-pairs/) · Hard · [compare](advanced-ds-patterns/segment-tree-fenwick-tree/problems/03-reverse-pairs.cpp)
- [ ] [327. Count of Range Sum](https://leetcode.com/problems/count-of-range-sum/) · Hard · [compare](advanced-ds-patterns/segment-tree-fenwick-tree/problems/04-count-of-range-sum.cpp)

**Sun 11-15 · Review + catch-up** — final review, all 36

- [ ] Done

**After week 6:** keep the 10-minute warm-up daily (the ladder in `INDEX.md` decides what's due), and spend the
rest on unseen mixed problems: paste a problem to `/pattern-triage` → solve on LeetCode → paste for `/review-attempt`.

---

## Redo on Sunday

Problems you needed the compare file for. Solve them again from scratch on Sunday; tick when Accepted with no help.

-

---

## Log

One line per session. Streak = consecutive days with at least 1 hour.

| Date | Minutes | Pattern — problems solved | Streak |
|---|---|---|---|

---

## Parked

Things to fix or improve in the material — **later, not during a session.**

- `images/trace-diagram.md` has no mermaid block in: bit-manipulation, dp-on-grids, palindromic-subsequence,
  dijkstras-algorithm (found by `module-audit`, 2026-10-05). Fix after week 6.
