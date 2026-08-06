# K-way Merge — Worked Problems

| # | Name | LeetCode | Difficulty | Approach | Complexity |
|---|------|----------|------------|----------|------------|
| 01 | [Merge k Sorted Lists](01-merge-k-sorted-lists.cpp) | [23](https://leetcode.com/problems/merge-k-sorted-lists/) | Hard | Min-heap seeded with each list's head node | `O(n log k)` time, `O(k)` space |
| 02 | [Kth Smallest Element in a Sorted Matrix](02-kth-smallest-element-in-a-sorted-matrix.cpp) | [378](https://leetcode.com/problems/kth-smallest-element-in-a-sorted-matrix/) | Medium | Rows are sorted lists — min-heap merge, stop at the k-th pop | `O(k log n)` time, `O(n)` space |
| 03 | [Find K Pairs with Smallest Sums](03-find-k-pairs-with-smallest-sums.cpp) | [373](https://leetcode.com/problems/find-k-pairs-with-smallest-sums/) | Medium | Min-heap over candidate pairs, expanding one index at a time | `O(k log k)` time, `O(k)` space |
| 04 | [Smallest Range Covering Elements from K Lists](04-smallest-range-covering-elements-from-k-lists.cpp) | [632](https://leetcode.com/problems/smallest-range-covering-elements-from-k-lists/) | Hard | Min-heap tracking current min across lists, sliding the max forward | `O(n log k)` time, `O(k)` space |

**Why these four:** 01 is the canonical k-way merge (heap seeded with one node per source). 02 shows the same idea applied to matrix rows instead of linked lists. 03 and 04 both show the pattern generalized beyond plain merging — 03 to enumerating best combinations, 04 to a sliding-range problem where the heap only ever tracks the current minimum across all lists.
