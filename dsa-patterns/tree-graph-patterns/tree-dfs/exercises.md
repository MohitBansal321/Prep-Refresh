# Tree DFS — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing whether a problem wants **preorder carry-state-down** (root-to-leaf paths/sums) or **postorder combine-on-the-way-up** (subtree aggregation), and (2) correctly distinguishing, in postorder problems, between what a recursive call *returns to its parent* and what the *problem's actual answer* is.

> Rule of thumb for every exercise: before writing a single line, ask "does the answer for this node depend on **the path from the root down to it** (preorder), or on **the fully-resolved answer of both its children** (postorder)?" If you cannot answer that, you are not ready to write the recursive function yet.

---

## Easy — Diameter of Binary Tree

**LeetCode 543 — Diameter of Binary Tree.**

Given the root of a binary tree, return the length (in number of edges) of the diameter of the tree — the length of the longest path between any two nodes, which may or may not pass through the root.

**Task:** solve it with a single postorder pass. Each recursive call should return the node's **height** (as in `maxDepth`), while separately tracking a running "best diameter seen so far" that considers `leftHeight + rightHeight` at every node.

**Think about:** why is this structurally almost identical to `maxDepth` in [code.cpp](code.cpp), plus one extra line? What is the one new piece of state you need that `maxDepth` did not track, and why does it need to live outside the function's return value (the same question `problems/04-binary-tree-maximum-path-sum.cpp` had to answer)?

---

## Medium — Sum Root to Leaf Numbers

**LeetCode 129 — Sum Root to Leaf Numbers.**

Each root-to-leaf path in a binary tree represents a number formed by concatenating the digits along the path (e.g. the path `1 -> 2 -> 3` represents the number 123). Return the total sum of all root-to-leaf numbers.

**Task:** adapt the preorder, carry-state-down shape from `problems/01-path-sum.cpp` — but instead of carrying a subtracted remainder, carry `currentNumber = currentNumber * 10 + node->val` down through the recursion, and add it to a running total whenever you reach a leaf.

**Think about:** this problem carries a running **number**, `binaryTreePaths` carries a running **string**, and `hasPathSum` carries a running **remainder** — three different payload types riding on the exact same preorder skeleton. Write out, in one sentence each, what "carried state" and "leaf action" mean for all three, and confirm they are structurally the same shape.

---

## Hard — Binary Tree Cameras

**LeetCode 968 — Binary Tree Cameras.**

You want to install the minimum number of cameras on a binary tree's nodes so that every node is covered. A camera at a node covers that node itself, its parent, and its direct children. Return the minimum number of cameras needed.

**Task:** solve it with a postorder pass where each recursive call returns one of three states for its subtree: "this node has a camera," "this node is covered but has no camera," or "this node is NOT covered." A node's own state — and whether it needs to place a camera on itself — depends entirely on the (already-resolved) states of both its children, making this a pure postorder aggregation, just with a three-way enum instead of a single integer.

**Then answer:** why must a leaf always report "not covered" (never "has a camera," never "covered") rather than being handled as a trivial base case the way `nullptr` is? What goes wrong if you treat a leaf as automatically "covered"?

---

## Real-World Challenge — Directory Storage Report

You are building a small internal tool for a backup service. Each directory is represented as a tree node with a `sizeBytes` (its own file's size, 0 for a pure directory node) and a list of child directory/file nodes. You need to produce a report with two numbers:

1. The **total storage used** by the entire tree (every file's size, summed).
2. The **single most expensive full path** from the root to any leaf file, reported as both its total byte count and the actual `/`-joined path string (e.g. `"backups/2024/db-dump.tar.gz"`).

**Task:**
1. Implement `(1)` as a postorder aggregation: a directory's total size is the sum of its children's already-resolved totals plus its own `sizeBytes`.
2. Implement `(2)` as a preorder, carry-state-down traversal: carry both the running byte total and the running path string down through the recursion, and update a "best path so far" (tracked outside the recursive function, the same discipline as `bestPathSum` in `problems/04-binary-tree-maximum-path-sum.cpp`) every time you reach a leaf.
3. Discuss: could you compute both `(1)` and `(2)` in a **single** traversal instead of two separate ones? What would the recursive function need to return, and what would it need to carry down, to do both at once? Is there any reason you might *prefer* to keep them as two separate traversals in a production codebase (hint: think about readability, single-responsibility, and how each would need to change independently if requirements evolved)?

---

## Bonus Challenge — Validate Binary Search Tree

**LeetCode 98 — Validate Binary Search Tree.**

Given the root of a binary tree, determine if it is a valid binary search tree (BST): for every node, all values in its left subtree must be strictly less than the node's value, and all values in its right subtree must be strictly greater — and this must hold not just against the node's *immediate* children, but against **every** descendant.

**Task:** implement this as a preorder, carry-state-down traversal where each recursive call is handed a valid `(lowerBound, upperBound)` range from its parent. At each node: check the node's value falls strictly within the current range, then recurse into `left` with the range narrowed to `(lowerBound, node->val)` and into `right` with `(node->val, upperBound)`.

**Then, generalize in writing (no code required):** every problem so far in this module either carries state down (preorder) or combines children's results up (postorder), never both in the same function. Does Validate BST fit cleanly into just one of those two categories, or does it need a piece of each? Justify your answer using the Architecture and Execution Flow sections of the [README](README.md), and be precise about exactly which part of the algorithm is "preorder-shaped" and which (if any) is "postorder-shaped."

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
