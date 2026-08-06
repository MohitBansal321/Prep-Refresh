# Tree BFS — Worked Problems

| # | Name | LeetCode | Difficulty | Approach | Complexity |
|---|------|----------|------------|----------|------------|
| 01 | [Binary Tree Level Order Traversal](01-binary-tree-level-order-traversal.cpp) | [102](https://leetcode.com/problems/binary-tree-level-order-traversal/) | Medium | Queue-based BFS, snapshot queue size per level | `O(n)` time, `O(n)` space |
| 02 | [Binary Tree Zigzag Level Order Traversal](02-binary-tree-zigzag-level-order-traversal.cpp) | [103](https://leetcode.com/problems/binary-tree-zigzag-level-order-traversal/) | Medium | Same level-order BFS, reverse alternate levels | `O(n)` time, `O(n)` space |
| 03 | [Minimum Depth of Binary Tree](03-minimum-depth-of-binary-tree.cpp) | [111](https://leetcode.com/problems/minimum-depth-of-binary-tree/) | Easy | BFS, return as soon as the first leaf is reached | `O(n)` time, `O(n)` space |

**Why these three:** 01 is the canonical level-order traversal (the size-snapshot trick). 02 shows the same traversal with a per-level post-processing twist. 03 shows BFS's key advantage over DFS for "minimum" questions — it can return the moment it finds the first leaf, without exploring the rest of the tree.
