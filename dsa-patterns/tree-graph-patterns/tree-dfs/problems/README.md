# Tree DFS — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Tree DFS across its two flavors (preorder carry-state-down, and postorder combine-on-the-way-up). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-path-sum.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Path Sum | [112](https://leetcode.com/problems/path-sum/) | Easy | Preorder, carry a running remainder down; a leaf matches if the remainder hits exactly 0. | O(n) time, O(h) space | [01-path-sum.cpp](01-path-sum.cpp) |
| Binary Tree Paths | [257](https://leetcode.com/problems/binary-tree-paths/) | Easy | Preorder, carry the path string down (copied per call); record it at every leaf. | O(n·h) time, O(h) + O(n·h) output space | [02-binary-tree-paths.cpp](02-binary-tree-paths.cpp) |
| Path Sum II | [113](https://leetcode.com/problems/path-sum-ii/) | Medium | Preorder with a SHARED mutable path vector; push before recursing, pop (backtrack) after. | O(n·h) time, O(h) + O(n·h) output space | [03-path-sum-ii.cpp](03-path-sum-ii.cpp) |
| Binary Tree Maximum Path Sum | [124](https://leetcode.com/problems/binary-tree-maximum-path-sum/) | Hard | Postorder; return the best one-sided downward extension to the parent, track the best "bends through both children" value separately. | O(n) time, O(h) space | [04-binary-tree-maximum-path-sum.cpp](04-binary-tree-maximum-path-sum.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the textbook preorder, carry-a-scalar-down search — the pattern in its purest form, and the natural first stop before anything harder.
- **02** is the textbook preorder, collect-every-path flavor — building up and recording the literal sequence of values, not just checking a sum.
- **03** takes 01/02 one step further by using a **shared, mutable** path structure instead of copying per call, which makes the push-then-pop backtracking discipline explicit — the direct bridge to full Backtracking (see the family README's Similar Patterns section).
- **04** is the hardest postorder aggregation in the set: the path can bend through any node using both children, which means the function's return value (what a parent may use) and the problem's actual answer (tracked separately) are genuinely different things — the single most commonly mishandled detail in this entire pattern.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
