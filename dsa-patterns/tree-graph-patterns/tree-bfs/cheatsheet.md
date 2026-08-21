# Tree BFS — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — iterative, queue-based level-by-level (breadth-first) traversal. |
| **Recognition Signal** | Problem says **"level order"**, **"level by level"**, **"each row/depth"**, asks for a **per-level aggregate** (sum, average, max, width), asks to **connect same-level nodes**, or asks a **minimum/shallowest/fewest-hops** question — "minimum depth," "nearest matching node." |
| **Problem** | The answer's natural unit is a **level** (a set of nodes equidistant from the root), but recursion's call stack tracks *how you got here* (a path), not *how far you have spread out* — so DFS has to synthesize levels after the fact from a threaded `depth` parameter, and can never early-exit on "shallowest." |
| **Solution** | A `std::queue` seeded with the root, plus the **level-size snapshot**: read `level_size = q.size()` once into a named local *before* the inner loop, process exactly that many nodes (dequeue · do the per-node work · push non-null children), then commit the level. Children pushed during the loop land behind the snapshot and become the next level. |
| **Time / Space Complexity** | O(n) time — every node enqueued once and dequeued once, O(1) work each · O(n) space worst case for the queue: the widest level of a complete tree holds ~n/2 nodes at once · `minDepth` keeps O(n) worst case but gains an O(1)-relative-to-n best case via early exit · problem 04's second solution reaches **O(1)** space by using the already-linked level as its own queue. |
| **Pros** | A level exists as a concrete, iterable collection at every step — no reconstruction pass · genuine early exit on minimum/shallowest questions that DFS structurally cannot match · fully iterative, so no recursion-depth limit on a deep narrow tree · one skeleton absorbs collecting, reversing, aggregating, and linking by changing only the per-node line · deterministic order (shallow-to-deep, left-to-right) that is trivial to state and prove. |
| **Cons** | O(n) queue space regardless of tree shape, where DFS's stack is O(h) and shape-sensitive · path questions need a path manually bolted onto every queue entry, which DFS gets free from the call stack · bottom-up subtree answers (height, balance, diameter) run against BFS's top-down order · no per-level result exists until that level is fully drained, so no streaming partial output · requires an explicit queue you must manage and terminate correctly. |
| **Use When** | Level-order output · per-level aggregation · connecting or comparing same-level nodes (next-right pointers, zigzag, per-level max width) · minimum depth or nearest-match-by-hops, where early exit pays · you specifically need an iterative traversal. |
| **Avoid When** | Root-to-leaf paths, path sums, or lowest common ancestor (use Tree DFS) · bottom-up subtree properties like height, balance, or diameter (Tree DFS, postorder) · the input can have cycles, be disconnected, or lack a single root (Graph BFS/DFS — you need a `visited` set) · memory is tight and the tree is wide and shallow (DFS's O(h) may be far cheaper). |
| **Related Patterns** | Tree DFS (path-by-path via the call stack, O(h) space, natural for paths and bottom-up aggregation) · Graph BFS/DFS (the identical queue mechanic plus a mandatory `visited` set, because a graph can revisit a node through a cycle and a tree cannot). |

### Template Skeleton

```cpp
// The one skeleton every function in this module reuses. Only the marked
// per-node and per-level lines change from problem to problem.
ResultType bfs(TreeNode* root) {
    ResultType result{};
    if (root == nullptr) return result;      // No root -> zero levels. Do this FIRST.

    std::queue<TreeNode*> q;
    q.push(root);
    int depth = 1;                           // Convention: a single node has depth 1.

    while (!q.empty()) {
        size_t level_size = q.size();        // *** THE SNAPSHOT ***
                                             // Read ONCE, into a named local, BEFORE
                                             // consuming. Never `i < q.size()` inline:
                                             // the pushes below grow q mid-loop and the
                                             // loop would slide into the next level.

        // *** PER-LEVEL STATE -- must be reset HERE, inside the outer loop. ***
        std::vector<int> level_values;        // fresh vector per level (problems 01/02)
        TreeNode* prev = nullptr;             // reset link cursor per level (problem 04)
        level_values.reserve(level_size);

        for (size_t i = 0; i < level_size; ++i) {
            TreeNode* node = q.front();
            q.pop();

            // *** PER-NODE WORK -- the only genuinely problem-specific line(s). ***
            level_values.push_back(node->val);      // collect  (01, 02)
            if (prev) prev->next = node;            // link     (04)
            prev = node;

            // Early exit (03): safe because BFS dequeues in non-decreasing depth
            // order, so the first match is provably the shallowest.
            if (!node->left && !node->right) return depth;   // leaf = BOTH null

            // Null-check every push: a nullptr in the queue crashes on dereference.
            if (node->left)  q.push(node->left);
            if (node->right) q.push(node->right);
        }

        // *** PER-LEVEL COMMIT -- after the inner loop, not inside it. ***
        result.push_back(std::move(level_values));
        ++depth;
    }
    return result;
}
```

### Remember In One Sentence
> **Tree BFS walks a tree level by level with a FIFO queue, and the entire pattern is one line — `level_size = q.size()` read into a named local *before* the inner loop — which freezes the current level's membership so the children pushed during that loop become the *next* level instead of silently merging into this one; you pay O(n) space for the widest level in exchange for levels as a first-class unit and a real early exit on "shallowest" questions.**

### Two Facts People Get Wrong
- The level-size snapshot is a minor implementation detail you could write either way? **No** — it *is* the mechanism that converts "a queue of nodes" into "a queue of levels." Writing `for (size_t i = 0; i < q.size(); ++i)` instead still visits every node in correct FIFO order and never crashes; it just fuses adjacent levels together, so you get a silent correctness bug rather than a visible failure. Trace the [trace diagram](images/trace-diagram.md)'s level 0 by hand: the snapshot is 1, but `q.size()` is already 2 by the end of that single iteration.
- BFS on a tree needs a `visited` set, just like graph BFS? **No** — a tree has exactly one path from the root to any node, so no node can ever be reached (and therefore enqueued) twice, making `visited` pure overhead. A general graph needs it because a cycle lets you arrive at the same node again and loop forever. Reaching for `visited` on a tree out of habit is the tell that *why* graphs need it has not clicked yet.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. Why must `level_size = q.size()` be read into a named local before the inner loop, and what exactly is the output if you write `for (size_t i = 0; i < q.size(); ++i)` instead — a crash, or something worse?
2. `levelOrder` and `minDepth` in [code.cpp](code.cpp) share an identical skeleton. Name every line that differs between them, and say which of those differences is the actual pattern and which is problem-specific.
3. Why is `minDepth`'s early return provably correct — what property of the queue, not of the leaf, justifies stopping there? What does a Tree DFS solution have to do instead, and why?
4. In `03-minimum-depth-of-binary-tree.cpp`, why is the leaf check `&&` and not `||`? Sketch the one-sided chain from its tests and give the wrong answer the `||` version would print.
5. In `04-populating-next-right-pointers-ii.cpp`, why is `prev` declared inside the outer `while` loop? Describe precisely what the tree looks like afterward if that declaration is hoisted above the loop — and why nothing crashes.
6. Problem 04's second implementation drops the queue entirely and reaches O(1) extra space. What replaces the queue, and what is the role of the `dummy` node? Why does that solution handle "cousins with distant parents" with no extra code?
7. `02-binary-tree-zigzag-level-order-traversal.cpp` never changes the traversal order. What does it change instead, and what does that tell you about where the pattern's variability actually lives?
8. State Tree BFS's worst-case space complexity, the tree shape that causes it, and how it compares to Tree DFS's worst case on (a) a balanced tree and (b) a long left-skewed chain.
9. Why does Tree BFS need no `visited` set while Graph BFS always does — what structural property of a tree is doing the work?
10. Name two question shapes that mention no levels at all but should still be solved with Tree BFS, and two that sound tree-shaped but belong to Tree DFS instead.
