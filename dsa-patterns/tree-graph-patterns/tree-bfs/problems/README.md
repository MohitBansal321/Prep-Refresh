# Tree BFS — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Tree BFS across everything the level-size snapshot can be used for: collecting a level, reordering a level, exiting early on the first qualifying level, and mutating the tree by linking a level's nodes to each other. Each file is self-contained — compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-binary-tree-level-order-traversal.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Binary Tree Level Order Traversal | [102](https://leetcode.com/problems/binary-tree-level-order-traversal/) | Medium | Queue + level-size snapshot; collect each snapshot-bounded batch into its own vector. | O(n) time, O(n) space | [01-binary-tree-level-order-traversal.cpp](01-binary-tree-level-order-traversal.cpp) |
| Binary Tree Zigzag Level Order Traversal | [103](https://leetcode.com/problems/binary-tree-zigzag-level-order-traversal/) | Medium | Identical traversal; reverse the collected vector before committing it on odd levels. | O(n) time, O(n) space | [02-binary-tree-zigzag-level-order-traversal.cpp](02-binary-tree-zigzag-level-order-traversal.cpp) |
| Minimum Depth of Binary Tree | [111](https://leetcode.com/problems/minimum-depth-of-binary-tree/) | Easy | Same skeleton, no collection; return the instant the first leaf (both children null) is dequeued. | O(n) worst case, O(1)-relative-to-n best case; O(n) space | [03-minimum-depth-of-binary-tree.cpp](03-minimum-depth-of-binary-tree.cpp) |
| Populating Next Right Pointers in Each Node II | [117](https://leetcode.com/problems/populating-next-right-pointers-in-each-node-ii/) | Medium | Two solutions: (a) BFS, linking each dequeued node to the previous one with `prev` reset per level; (b) no queue at all — walk the already-linked level above as its own iterator, stitching the next level with a dummy head. | (a) O(n) time, O(n) space · (b) O(n) time, **O(1)** space | [04-populating-next-right-pointers-ii.cpp](04-populating-next-right-pointers-ii.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md), and each one uses the level-size snapshot for a different purpose:

- **01** is the canonical level-order traversal — the snapshot in its purest form, used only to group output. It is the shape every other file in this directory is a variation on, and the direct descendant of `levelOrder` in [code.cpp](../code.cpp).
- **02** shows that the traversal itself is *not* what varies. The queue still yields every level strictly left-to-right; zigzag changes only how a level's already-collected vector is **stored**. This is the clearest evidence that the snapshot discipline, not the per-node logic, is the pattern.
- **03** is the "minimum depth" signal, and the only place BFS beats DFS on *time* rather than convenience: because the queue dequeues nodes in non-decreasing depth order, the first leaf found is provably the shallowest, so the rest of the tree is never touched. It also pins down the leaf definition — **both** children null, not one — with a one-sided-chain test that a `||` check would fail.
- **04** is the hardest of the four and the only one that **mutates** the tree instead of reading it. The snapshot stops being a grouping convenience and becomes a correctness boundary: `prev` must be reset at every level, or each level's last node gets linked to the next level's first, permanently corrupting the structure with no crash and no visible symptom. The file then answers the classic follow-up by dropping the queue entirely — once a level is linked, that chain of `next` pointers *is* an iterator over the level, so the next level can be stitched together in **O(1)** extra space, directly retiring the O(n) queue cost the README's Disadvantages section names as this pattern's main weakness. Solving 117 (an arbitrary binary tree, where the two nodes to link are often cousins with distant parents) also solves 116 (a perfect tree) with no change, which is why 116 is the version named in the README's Further Reading.

For unguided practice on problems that are *not* worked out step by step — including LeetCode 637, 199, 297, and 863, none of which appear above — see [exercises.md](../exercises.md).
