# Tree BFS — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — level-by-level (breadth-first) traversal driven by a **queue**. |
| **Recognition Signal** | The problem says **"level"** — level order, zigzag by level, right-side view, average per level, largest value per level — or asks for **minimum depth**, which is a shortest-path question in disguise. |
| **Problem** | Recursion naturally goes *down* one branch to the bottom before touching a sibling, so nodes at the same depth are visited far apart in time with no way to group them. And for minimum depth, DFS must explore *every* branch to the bottom before it can be sure, even when a shallow leaf sits two nodes from the root. |
| **Solution** | A queue processes nodes in arrival order, which is exactly non-decreasing depth. To recover level *boundaries*, **snapshot `queue.size()` at the top of each outer iteration** — that count is precisely the current level's width, because all of its children are enqueued only during the inner loop that follows. For minimum depth, **return the moment the first leaf is dequeued**: BFS reaches nodes in depth order, so the first leaf found is the shallowest. |
| **Time / Space Complexity** | **O(n)** time (each node enqueued and dequeued once). **O(w)** space where w is the maximum level width — **O(n/2) = O(n)** for a balanced tree's bottom level, but only O(1)–O(h) on a skewed one. |
| **Pros** | Level grouping is nearly free — one size snapshot per level · minimum depth short-circuits at the first leaf, often touching a tiny fraction of the tree where DFS would touch all of it · iterative, so no stack-overflow risk on deep trees · queue order gives left-to-right output naturally, and reversing the inner append handles zigzag · trees have no cycles, so **no visited set is needed** (unlike graph BFS). |
| **Cons** | Space is the mirror image of DFS's: O(n) worst case on a wide balanced tree, where DFS would use only O(log n) · forgetting the size snapshot — or reading `queue.size()` *inside* the inner loop while it grows — merges all levels into one flat list · null children must be filtered before enqueueing or the level widths are wrong · genuinely awkward for anything needing the root-to-node path or subtree aggregates, which are DFS's home ground. |
| **Use When** | Any per-level output (level order, zigzag, averages, maxima, right/left side view) · minimum depth · "nearest node satisfying P" · connecting level-order siblings (next-right pointers) · serialising a tree level by level · anything where shallower answers should be found first. |
| **Avoid When** | The question is about root-to-leaf **paths** or **path sums** (Tree DFS — the call stack carries the path) · you need subtree aggregates like height, balance, or diameter (postorder DFS) · **maximum** depth on a wide tree, where DFS uses far less memory for the same O(n) · memory is tight and the tree is bushy. |
| **Related Patterns** | Tree DFS (branch-first rather than level-first; use it for paths and aggregates) · Graph BFS (the same queue, plus the visited set that cycles force) · Two Heaps / K-way Merge (other "process in a controlled order via a container" designs). |

### Template Skeleton

```cpp
// A. LEVEL ORDER — the size snapshot is the entire trick.
std::vector<std::vector<int>> levelOrder(TreeNode* root) {
    std::vector<std::vector<int>> levels;
    if (!root) return levels;                 // empty tree: not an error, just no levels

    std::queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        int levelSize = q.size();             // SNAPSHOT before the inner loop.
                                              // Everything in the queue right now is
                                              // exactly one level; children pushed below
                                              // belong to the NEXT level.
        std::vector<int> currentLevel;
        for (int i = 0; i < levelSize; ++i) { // fixed count — q.size() grows in here
            TreeNode* node = q.front(); q.pop();
            currentLevel.push_back(node->val);

            if (node->left)  q.push(node->left);    // filter nulls at push time,
            if (node->right) q.push(node->right);   // or level widths go wrong
        }
        levels.push_back(currentLevel);
    }
    return levels;
}
// Zigzag: same loop, reverse currentLevel on alternate levels (or push_front).

// B. MINIMUM DEPTH — return at the FIRST leaf dequeued. This is why BFS wins here.
int minDepth(TreeNode* root) {
    if (!root) return 0;

    std::queue<TreeNode*> q;
    q.push(root);
    int depth = 1;

    while (!q.empty()) {
        int levelSize = q.size();
        for (int i = 0; i < levelSize; ++i) {
            TreeNode* node = q.front(); q.pop();

            // A leaf means BOTH children are null. A node with one child is NOT a leaf.
            if (!node->left && !node->right) return depth;   // shallowest, guaranteed

            if (node->left)  q.push(node->left);
            if (node->right) q.push(node->right);
        }
        ++depth;
    }
    return depth;   // unreachable for a non-null root
}
```

### Remember In One Sentence
> **Tree BFS uses a queue so nodes come out in non-decreasing depth — snapshot `queue.size()` before each inner loop to carve that stream into levels, and return at the first dequeued leaf for minimum depth, because the first thing BFS finds at a given depth is the shallowest.**

### Two Facts People Get Wrong
- You can read `q.size()` inside the inner loop instead of snapshotting it? **No** — the inner loop *pushes children into the same queue*, so the size is moving while you read it. The boundary between levels is destroyed and every level merges into one flat list. Capture the size **once**, before the inner loop begins.
- A node with one null child is a leaf, so `minDepth` can return there? **No** — a leaf has **both** children null. Returning at a one-child node gives a depth that no actual leaf occupies. (This is also exactly why the naive recursive `1 + min(left, right)` is wrong for minimum depth: a null child returns 0 and wins the `min` despite not being a leaf at all.)

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. What single word in a problem statement most reliably signals Tree BFS?
2. Why does a queue produce nodes in non-decreasing depth order?
3. Explain the size-snapshot trick: what does `levelSize` equal at that moment, and why?
4. Describe exactly what goes wrong if `q.size()` is read inside the inner loop.
5. Why must null children be filtered before pushing rather than skipped after popping?
6. Why can `minDepth` return immediately at the first leaf it dequeues? State the guarantee.
7. Define "leaf" precisely, and explain why the recursive `1 + min(left, right)` is wrong for minimum depth.
8. Compare BFS and DFS space usage on (a) a balanced tree and (b) a degenerate skewed one. Which wins in each case?
9. Why does Tree BFS need no visited set, while Graph BFS does?
10. Name two problems where you must use DFS instead, and say what BFS lacks in each.
