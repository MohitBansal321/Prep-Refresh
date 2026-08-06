# Tree DFS — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — recursive (or explicit-stack) depth-first traversal. |
| **Recognition Signal** | Problem mentions **root-to-leaf paths**, **path sums**, or a property that depends on **the full depth of one branch** or **a subtree's aggregated value** (height, balance, diameter, max path sum). |
| **Problem** | Need the path from root to a node, or the fully-resolved answer of both children, before you can answer for the current node — level-tracking data structures do not naturally carry either. |
| **Solution** | Recurse into `left`/`right`. **Preorder** (carry state down as a parameter): process the node, then recurse — used for path/sum collection. **Postorder** (combine on the way up via return value): recurse fully into both children first, then combine — used for aggregation. |
| **Time / Space Complexity** | O(n) time (every node visited once). O(h) space for the call stack, where h = tree height — O(log n) balanced, O(n) worst case (degenerate/skewed tree). |
| **Pros** | The call stack itself carries "path so far" — no explicit path/parent structure needed for the common case · O(h) space, often less than BFS's O(n) worst case on a wide balanced tree · code shape mirrors the problem's own recursive definition · composable (height, balance, diameter, max path sum share the same postorder shape). |
| **Cons** | Recursion depth = tree height, risking stack overflow on very deep/unbalanced trees · converting to iterative (explicit `std::stack`) is easy for preorder but genuinely hard for postorder · careless per-call copying of a path structure can turn O(n) into O(n·h) · does not parallelize across "the same level" as naturally as BFS. |
| **Use When** | Root-to-leaf path/sum questions · max depth/height · subtree aggregation (balance, diameter, max path sum) · AST/expression-tree/directory-tree style recursive evaluation. |
| **Avoid When** | Problem says "level," "level order," or "minimum depth" (use Tree BFS) · you need shortest hops in a general graph (Graph BFS) · you are building your own decision tree of choices to enumerate/prune (Backtracking) · recursion depth is a real operational risk on adversarial input (convert to iterative, or bound/reject pathological input first). |
| **Related Patterns** | Tree BFS (level-by-level via a queue, not branch-by-branch via recursion) · Backtracking (same recurse-then-undo shape, but over an implicit decision tree you construct, not input data you were handed). |

### Template Skeleton

```cpp
// Preorder, carry-state-down (path/sum collection)
void dfsPreorder(TreeNode* node, StateType stateSoFar /* by value, or push+pop if shared */) {
    if (!node) return;                       // base case
    stateSoFar = fold(stateSoFar, node->val); // "process the node"

    bool isLeaf = (!node->left && !node->right);
    if (isLeaf) {
        recordAnswerFrom(stateSoFar);
        return;
    }
    dfsPreorder(node->left, stateSoFar);
    dfsPreorder(node->right, stateSoFar);
    // If stateSoFar is a SHARED mutable structure (not passed by value),
    // undo the fold here (pop_back / subtract) before returning.
}

// Postorder, combine-on-the-way-up (aggregation)
ResultType dfsPostorder(TreeNode* node, RunningBest& bestSoFar) {
    if (!node) return baseCaseValue;                 // e.g. 0

    ResultType leftResult = dfsPostorder(node->left, bestSoFar);
    ResultType rightResult = dfsPostorder(node->right, bestSoFar);

    // Only NOW, after both children are fully resolved, combine.
    bestSoFar = combineForAnswer(bestSoFar, node->val, leftResult, rightResult);
    return combineForParent(node->val, leftResult, rightResult); // may differ from bestSoFar's update!
}
```

### Remember In One Sentence
> **Tree DFS recurses into a node's children, using the call stack itself as "the path so far" — carry state DOWN as a parameter when the problem needs the path (preorder), or combine children's results UP via the return value when the problem needs a subtree aggregate (postorder), and never confuse the return value a parent may use with the problem's actual answer.**

### Two Facts People Get Wrong
- A leaf is **any node with a null child**? **No** — a leaf is a node where **both** `left` and `right` are null. A node with exactly one child is an internal node, not a leaf, and treating it as one is the most common bug in this pattern.
- In postorder aggregation, the function's return value **is** the problem's answer? **Not always** — in "the path can bend through this node" problems (max path sum, diameter), the return value is only what a *parent* is allowed to extend through (at most one child's contribution); the actual answer is tracked in a separate running variable updated at every node.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. What is the recognition signal that distinguishes "reach for Tree DFS" from "reach for Tree BFS"?
2. Why does the call stack alone carry "the path from root to the current node," with no extra data structure needed?
3. State the difference between the preorder framing and the postorder framing in one sentence each.
4. In `hasPathSum`, why does the recursive function check `remaining == 0` only at a leaf, and not at every node?
5. Precisely define "leaf" as used in this pattern, and name the specific bug that results from getting the definition wrong.
6. Why does a shared, mutable path vector (as in Path Sum II) require an explicit "un-append" step after the recursive calls, while a path passed by value (as in Binary Tree Paths' string) does not?
7. In Binary Tree Maximum Path Sum, why is the value returned to the parent not the same as the value compared against the running best answer?
8. Why are negative subtree contributions clamped to zero in Binary Tree Maximum Path Sum?
9. Compare Tree DFS's worst-case space to Tree BFS's worst-case space on (a) a balanced tree and (b) a degenerate, linked-list-shaped tree.
10. Name one real production system (not a LeetCode problem) that uses this exact recursive shape, and say which framing (preorder or postorder) it uses.
