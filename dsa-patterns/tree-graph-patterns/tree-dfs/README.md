# Tree DFS

## Intent

Explore a binary tree one branch at a time, all the way to a leaf, before backtracking to try the next branch — carrying state (a running sum, a path so far) down the recursive call stack rather than tracking it in an explicit queue.

## Real Life Analogy

Think of **exploring a maze by always going as deep as possible down one corridor before backtracking to try another**. You do not send out a wave of scouts that fan out level by level, checking every branch point at the same distance from the entrance before moving further in (that is what Tree BFS does). Instead you pick a direction, commit to it, and walk until you either hit a dead end or the exit. If you hit a dead end, you retrace your steps back to the most recent fork you have not fully explored, and try the other direction from there.

Crucially, you remember the corridors you walked through **to get here** — if the maze is actually a puzzle where you need to report "the sequence of turns that led from the entrance to this dead end," that sequence is naturally sitting in your memory of the walk you just took, because you built it up one turn at a time as you went deeper. You do not need a separate notebook tracking "which corridors were open at each distance from the start" the way a level-by-level search would.

A second everyday version: **filing a company's org chart to compute a total headcount per department.** To know department X's total headcount, you need the headcount of every team nested inside it, and every sub-team nested inside those. You cannot compute X's number by looking only at people who report *directly* to X — you have to walk all the way down into the leaves of that branch, add everything up, and carry the total back up to X. That "go all the way down, then combine coming back up" motion is the second half of Tree DFS, and it looks nothing like scanning level by level either.

Tree DFS is that maze-walking, org-chart-summing instinct, formalized: recurse into a child, let the recursion go as deep as it needs to, and either carry information down as you go (the path so far) or combine information on the way back up (a subtree's total).

## Problem

### What engineering problem exists?

A large class of binary tree problems asks one of these questions:

- **"Find every root-to-leaf path that satisfies some property."** Return all root-to-leaf paths as strings; find all root-to-leaf paths where the digits sum to a target; find all paths whose node values sum to a given number.
- **"Compute a value that depends on the sum, count, or shape of one full branch, from root down to leaf."** Does any root-to-leaf path sum to a target value? What is the tree's maximum depth? What is the maximum sum achievable along any path (not necessarily through the root)?
- **"Aggregate a per-subtree property up to the root."** Is this tree height-balanced (each subtree's left/right heights differ by at most 1)? Is this a valid binary search tree? What is the diameter (longest path between any two nodes)?

All three of these share a structural need: the answer for a given node depends on **the full path from the root down to that node**, or on **the fully-resolved answer for both of that node's subtrees**. Either way, you need "the state accumulated so far along one branch" to be available exactly where you are standing in the tree, and you need it to *disappear* the moment you back out of that branch (so it does not leak into a sibling branch that shares nothing with it except a common ancestor).

> **Term: Root-to-leaf path.** A path in the tree that begins at the root node and ends at a **leaf** — a node with no children at all (both `left` and `right` are `nullptr`). A path that stops at an internal node (one that still has a child) is not a root-to-leaf path, even if it looks complete at first glance; this distinction is a common source of bugs (see Common Mistakes).

### Why is this problem difficult?

- **Naive iterative "level tracking" does not naturally carry a full path.** If you approach this the way you would approach a level-order problem — a queue holding nodes, processed one level at a time — the queue only ever knows "which nodes are at distance k from the root." It has no built-in concept of "which specific sequence of turns got me to this particular node," because at any given level there can be many nodes, each reached by a completely different sequence of left/right choices. You would have to bolt a separate "path so far" onto every single queue entry, and that path is a different, growing list for every entry — which is possible, but wasteful (see Why Not Other Approaches below).
- **The state needed lives naturally on the call stack, not in a data structure you manage yourself.** Recursion gives you, for free, a private copy of "how did I get here" for every active call: the local variables and parameters of every still-running call form an implicit stack of everything above the current node. Recognizing that this implicit stack **is** the path-so-far — and does not need to be built by hand — is the conceptual leap that makes Tree DFS click.
- **Knowing when to combine on the way down versus on the way back up is not obvious the first time.** Some problems (collecting root-to-leaf paths) want you to build up state as you descend and only *use* it once you reach a leaf (a "preorder" flavor). Other problems (max path sum, tree diameter, balanced-tree checks) need the fully-resolved answer from both children *before* they can compute the current node's answer, which means you must finish recursing into both subtrees first and combine their results only on the way back up (a "postorder" flavor). Picking the wrong shape for a given problem produces code that either cannot compile cleanly (you need a value you have not computed yet) or silently computes the wrong thing.

### What happens if we ignore it?

- **Reinventing recursion badly, by hand, with an explicit stack and a parallel "path" structure per stack entry.** This is possible (see Architecture below for when it is actually the right call) but for the common case it adds bookkeeping — pushing and popping a path vector in lockstep with a node stack — for no benefit over just letting the language's call stack do it.
- **Re-deriving the same subtree answer multiple times.** If you do not recognize that a problem wants postorder aggregation, you might try to compute "the max path sum through this node" by re-walking each subtree fresh every time it is queried from an ancestor — turning an O(n) tree walk into something closer to O(n²) on an unbalanced tree.
- **Silently wrong answers at the leaf/internal-node boundary.** Treating "a node with one null child" as a leaf (or forgetting to check for a leaf at all) produces paths that end too early or too late — a specific, easy-to-miss correctness bug covered in Common Mistakes.

## Why Not Other Approaches?

**"Use BFS with manual path-tracking attached to every queue entry."**
This actually works — you can push `(node, path_so_far)` pairs onto a queue instead of just `node`, and every time you dequeue and want to go to a child, you `push(child, path_so_far + child.val)`. The problem is not correctness, it is waste: at every level, *every single node in the queue carries its own copy of the path from the root*, and those copies overlap heavily (siblings deep in the tree share almost their entire path with their parent, yet each queue entry stores that shared prefix independently). For a tree with `n` nodes and height `h`, this can mean O(n · h) total memory across all the path copies sitting in the queue at once, versus DFS's O(h) — because DFS's "path" is just the current recursive call stack, and there is only ever *one* active path in memory at a time (the one currently being explored), not one copy per node waiting in a queue.

**"Do a full BFS to collect all nodes first, then a second pass to reconstruct paths from a parent-pointer map."**
This works and uses O(n) space for the parent map, but it is two full passes over the tree plus an auxiliary hash map, when the problem can be solved in a single top-down recursive pass with no auxiliary structure beyond the call stack itself. It is strictly more code and more memory for the same answer.

**"Convert the tree to an explicit graph adjacency list and run generic graph DFS with a `visited` set."**
A tree has no cycles and exactly one path from the root to any node, so a `visited` set (needed in general graph DFS to avoid infinite loops on cycles) is pure overhead here — you would be paying for a guarantee (cycle safety) the input already gives you for free. It also throws away the natural `left`/`right` child structure that makes tree recursion so direct to write.

**"Iterative preorder/postorder with an explicit `std::stack<TreeNode*>`, from the start."**
This is a legitimate alternative and is exactly what you reach for when recursion depth is a real concern (see Disadvantages) — but for learning the pattern and for the overwhelming majority of interview-sized trees, it adds stack-management bookkeeping (manually pushing/popping, and for postorder, tracking whether a node's children have already been processed) that recursion gets for free from the language runtime. Reach for the explicit-stack version once you have a concrete reason (extremely deep/unbalanced trees, or a language/runtime with a small default stack), not as the default.

**Tradeoff summary:** every alternative either duplicates the path at every branch point (BFS with manual tracking), does the walk twice with an auxiliary structure (BFS + parent map), pays for a safety guarantee the tree does not need (generic graph DFS with `visited`), or manually re-implements what the call stack already gives for free (naive iterative version, absent a real depth concern). Tree DFS wins specifically because the recursive call stack **is** the "path so far" and the "state to combine on the way back up," with zero extra bookkeeping — that is its entire value proposition, and it costs nothing extra until the tree gets deep enough that stack depth itself becomes the risk (again, see Disadvantages).

## Solution

The core idea is: **write a function that takes a node, and recurses into `node->left` and `node->right`.** Everything else is a decision about *what you carry into the recursive call* and *what you do with what comes back out of it*. There are three framings, distinguished by when you "visit" (process) the current node relative to recursing into its children:

**Preorder framing — process the node, then recurse.** You look at the current node's value first, incorporate it into whatever running state you are carrying (append it to a path, add it to a running sum), and only then recurse into left and then right, passing that updated state down as a parameter. This is the natural shape for **root-to-leaf path collection**: you build the path as you go down, and the moment you reach a leaf, the path parameter already contains the complete root-to-leaf sequence — you just record it (or check its sum) right there.

**Inorder framing — recurse left, process the node, recurse right.** Less central to this module (inorder's headline use is "visit nodes of a binary *search* tree in sorted order"), but it is worth naming because it is the third leg of the preorder/inorder/postorder trio and interviewers expect you to know all three exist and differ only in *when* the current node is processed relative to its children.

**Postorder framing — recurse into both children fully, then combine their results with the current node.** You call yourself on `node->left`, call yourself on `node->right`, and only after **both** calls have returned do you compute something using the current node's value plus whatever each recursive call handed back. This is the natural shape for **aggregation problems**: max path sum, tree height/balance checks, tree diameter — anything where the correct answer for a node genuinely cannot be computed until you know the fully-resolved answer for both of its subtrees.

The unifying thought, in one sentence: **recursion into `left`/`right` handles "go deeper"; a parameter carried down handles "what do I know so far, on the way in"; a return value handles "what did my subtree decide, on the way back out."** Every Tree DFS problem is some combination of those three ingredients — no explicit stack, no queue, no code yet, just the shape of the recursive call.

## Architecture

Tree DFS has one real "participant" beyond the tree itself:

1. **The recursive function.** Takes a `TreeNode*` (possibly `nullptr`) and whatever accumulated state the problem needs (a running sum, a path vector, a target). Its job, every single call, is the same three-part shape: (a) handle the base case (`nullptr`, or a leaf, depending on the problem), (b) recurse into `left` and/or `right`, (c) combine what it knows with what came back from the recursive calls (postorder) or with what it is passing down (preorder), and return or record the result.

2. **The call stack (implicit).** Every active (not-yet-returned) call to the recursive function is one frame on the language's call stack. The *sequence* of frames currently active, from the outermost call down to the current one, **is** the root-to-leaf path you are standing on right now — that is why no explicit path-tracking data structure is needed for many problems. This is the single most important architectural fact about Tree DFS: **the mechanism (the call stack) and the data you need (the path/ancestry) are the same thing.**

3. **Accumulated state, carried as a parameter (preorder) or built from return values (postorder).** For path problems, this is typically a `std::vector<int>` (or a running sum) passed by value or reference into each recursive call, representing "everything on the path from the root to (and including) the current node." For aggregation problems, this is typically the recursive function's **return value** — e.g. "the height of this subtree," which the parent then uses to compute its own height, and so on up to the root.

4. **The explicit `std::stack`, only if converting to an iterative version.** If recursion depth is a genuine concern (see Disadvantages), the same algorithm can be rewritten to manage its own stack of `(TreeNode*, state)` pairs on the heap instead of the language call stack. This is a mechanical transformation of the same logic, not a different algorithm — every problem below is described first as recursive, because that is the natural, default shape.

Responsibilities in one line each:
- **Recursive function:** handles one node, recurses into children, combines results.
- **Call stack:** *is* the path/ancestry information, for free, with no bookkeeping.
- **Accumulated state:** the running sum/path (preorder) or the subtree's resolved answer (postorder) — the actual payload being threaded through the recursion.
- **Explicit stack (optional):** a heap-allocated stand-in for the call stack, used only when recursion depth itself is the risk being managed.

## Execution Flow

**Preorder — root-to-leaf path collection**, step by step:

1. Define a recursive function `collect(node, path_so_far)`.
2. If `node` is `nullptr`, return immediately — there is nothing to add and nothing to recurse into (the base case).
3. Append `node->val` to `path_so_far` (or, in a language without free copying, push it and remember to pop it later — see Common Mistakes for what goes wrong if you forget).
4. Check whether `node` is a **leaf** (`node->left == nullptr && node->right == nullptr`). If it is, `path_so_far` now holds a complete root-to-leaf path — record it (append it to the results list, check its sum against a target, etc.).
5. If `node` is not a leaf, recurse: call `collect(node->left, path_so_far)`, then call `collect(node->right, path_so_far)`.
6. After both recursive calls return (or immediately, if using a shared mutable path vector instead of passing a new copy each time), remove `node->val` from `path_so_far` — this is the "backtrack" step, undoing step 3 so that when control returns to this node's *parent*, the path no longer includes a value from a branch that has been fully explored and left behind.
7. The top-level call is `collect(root, {})` (an empty path). When it returns, every root-to-leaf path has been recorded.

**Postorder — aggregation, e.g. maximum path sum**, step by step:

1. Define a recursive function `bestDownward(node)` that returns "the best sum achievable on a path that starts at `node` and goes downward into at most one child" — and, along the way, updates a running "best path sum seen anywhere so far" (often a variable captured by reference or a class member, since the true best path can bend through a node using *both* children, which is not a valid "downward" value to return to a parent).
2. If `node` is `nullptr`, return `0` (a path that does not extend through a missing child contributes nothing) — the base case.
3. Recurse: `leftBest = bestDownward(node->left)`; `rightBest = bestDownward(node->right)`. Both calls must fully complete (all the way down to their leaves and back) before step 4 can happen — this is what makes it postorder.
4. Clamp negative contributions to zero (`leftBest = max(leftBest, 0)`, same for `rightBest`) — a negative-sum subtree should simply not be included, since including it can only hurt the total.
5. Update the global/running best: `bestSoFar = max(bestSoFar, node->val + leftBest + rightBest)` — this is the one place where the path is allowed to "bend" through `node` using both children at once, because at this exact node we are allowed to consider it as the path's highest point.
6. Return `node->val + max(leftBest, rightBest)` to the caller — the value the *parent* is allowed to use, which may only extend through **one** child (a real path cannot branch), so the parent gets the better of the two downward options, not both.
7. The top-level call is `bestDownward(root)`; the answer to the problem is whatever `bestSoFar` ended up holding after that call returns, not `bestDownward(root)`'s own return value.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full flowchart distinguishing Tree DFS from Tree BFS and from Backtracking based on the signals in a problem statement.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the general recurse-down-then-combine-on-the-way-back-up shape shared by preorder path collection and postorder aggregation.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of the recursive call stack descending and returning on a small concrete tree, using Path Sum (LeetCode 112) as the worked example.

## Implementation

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the *shape* of the pattern (preorder path-building, postorder aggregation) clearly, before looking at the worked, problem-specific solutions in [problems/](problems/).

It provides three small, reusable functions, all operating on a plain `TreeNode`:

- `binaryTreePaths` — the preorder, path-building flavor: walks every root-to-leaf path and returns all of them formatted as `"root->child->leaf"`-style strings.
- `hasPathSum` — a second preorder flavor, carrying a running sum instead of a path, to check whether *any* root-to-leaf path sums to a target.
- `maxDepth` — the simplest possible postorder aggregation: each call returns "the height of this subtree," computed from the (already-resolved) heights of its two children.

## Code Walkthrough

**`TreeNode`** (in [code.cpp](code.cpp)). The standard binary tree node: an `int val` and two raw pointers, `left` and `right`, defaulted to `nullptr` in the constructor. This is the exact shape LeetCode uses for every binary tree problem, which is why every file in this module (including all four `problems/*.cpp`) redefines this same struct rather than sharing a header — each file is meant to be copy-pasteable and standalone.

**`buildSampleTree()`** (in [code.cpp](code.cpp)). A small helper that heap-allocates a fixed, hand-checkable tree (the same shape used in the Trace Diagram) so `main()` has something concrete to run the three functions against without repeating tree-construction boilerplate at every call site.

**`binaryTreePaths`** (in [code.cpp](code.cpp)). Preorder path-building. An inner recursive helper takes the current node and a `std::string` representing the path built so far (passed *by value*, so each recursive call automatically gets its own independent copy — no explicit backtracking/un-append step is needed here, unlike the shared-mutable-vector version used in `problems/03-path-sum-ii.cpp`, which exists specifically to show that alternative, more memory-efficient style). At a leaf, the completed path string is pushed onto the results vector; otherwise the function recurses into whichever children exist, extending the path string with `"->"` plus the child's value before each call.

**`hasPathSum`** (in [code.cpp](code.cpp)). Preorder, but carrying a running `int` sum instead of a string. At each call, the current node's value is added to the running sum ("carried in as a parameter" is the preorder discipline described in Solution above); at a leaf, the function checks whether the accumulated sum equals `targetSum` and returns that boolean up through the recursion (short-circuiting via `||` so a `true` from the left subtree skips evaluating the right subtree at all, exactly like ordinary boolean short-circuit evaluation).

**`maxDepth`** (in [code.cpp](code.cpp)). Pure postorder aggregation, and the simplest possible one: `nullptr` returns `0` (base case), otherwise the function calls itself on `left` and `right`, and only after **both** have returned does it compute `1 + max(leftDepth, rightDepth)` — one more than the taller of its two subtrees. This is deliberately the smallest example of "you cannot answer for this node until both children have fully answered for themselves," to make the postorder shape as clear as possible before `problems/04-binary-tree-maximum-path-sum.cpp` builds a much more involved postorder aggregation on top of the same shape.

**`main()`** (in [code.cpp](code.cpp)). Builds the sample tree once, runs all three functions against it, prints `[PASS]`/`[FAIL]` for each assertion against a hand-computed expected answer, and frees every heap-allocated `TreeNode` before exiting (via a small recursive `deleteTree` helper) — there is no garbage collector in C++, so every `new TreeNode(...)` in `buildSampleTree()` must be matched by a `delete` somewhere, and a postorder traversal (delete children before the node itself) is the only safe order to do it in.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem, defining its own `TreeNode` and helpers rather than including `code.cpp` (so every file stays independently readable and copy-pasteable), but implementing the *same* recursive shape established above. See [problems/README.md](problems/README.md) for the index. Briefly: `01` is the pure preorder-with-running-sum check (does *any* path match); `02` is the pure preorder path-building-and-collecting flavor; `03` extends `01`/`02` with explicit backtracking on a **shared, mutable** path vector, showing the un-append discipline that `binaryTreePaths` in `code.cpp` avoids by copying instead; `04` is the hardest postorder aggregation in the set, where the path is allowed to "bend" through any node, not just run root-to-leaf.

## Advantages

- **No auxiliary data structure needed for the common case.** The path/ancestry information you need is already sitting in the call stack's local variables and parameters — you do not allocate or manage a queue, a stack, or a parent-pointer map yourself.
- **Direct, close match between code shape and problem shape.** "Find all root-to-leaf paths" reads almost like the problem statement once written as `if (isLeaf) record(path); else { recurse(left); recurse(right); }` — the code is a near-literal transcription of the recursive definition of "root-to-leaf path."
- **Naturally handles both "carry state down" and "combine on the way up" problems** with the same basic recursive skeleton, just changing whether the important work happens before or after the two recursive calls (preorder vs. postorder).
- **Composability.** Postorder aggregation composes cleanly: `maxDepth` on a node is `1 + max(maxDepth(left), maxDepth(right))`, and this exact "combine children's answers" shape reappears, with small variations, in balanced-tree checking, diameter, and max path sum — learn the shape once, reuse it repeatedly.
- **Low constant-factor overhead versus an explicit stack.** Recursive calls compiled by any mainstream C++ compiler are cheap; there is no heap allocation for a manual stack structure and no manual push/pop bookkeeping to get wrong.

## Disadvantages

- **Recursion depth equals tree height, which risks stack overflow on very unbalanced or very deep trees.** A perfectly balanced tree of `n` nodes has height `O(log n)`, which is nothing to worry about even for millions of nodes. But a **degenerate** tree — one that is really a linked list in disguise, e.g. built by inserting already-sorted data into an unbalanced BST — has height `O(n)`, meaning `n` nested recursive calls. For `n` in the hundreds of thousands, that can exceed a thread's default stack size (commonly a few MB) and crash the program with a stack overflow, not a clean exception.
- **Converting to an iterative version with an explicit `std::stack` adds real complexity.** Preorder is fairly mechanical to make iterative (push right child, then left child, so left is popped first). Postorder is the hardest of the three to do iteratively without recursion, because you need to know, for a node already on the stack, whether both of its children have already been fully processed before you are allowed to "visit" it — this typically needs either a second stack, a "last visited node" marker, or reversing a modified preorder traversal, none of which is as immediately readable as the recursive version.
- **Passing large state by value down every recursive call can waste memory/time if done carelessly.** Copying an entire path vector at every single node (rather than passing by reference and explicitly backtracking, or building strings incrementally) turns what should be O(h) auxiliary space per active path into something closer to O(h²) total space/time across the whole traversal, because each of the `h` levels re-copies an ever-growing vector.
- **Harder to parallelize than level-order traversal.** Because each recursive call depends on its parent's call frame and, in postorder, must wait for children to fully finish, naive DFS does not parallelize across "the same level" the way BFS's queue-per-level structure more naturally invites (though DFS subtrees *can* be parallelized independently — it is just less of a free/obvious win than BFS's level-by-level shape).

## Tradeoffs

**What we gain versus BFS with manual path-tracking:** the same asymptotic correctness, but O(h) space for the "current path" state (living in the call stack) instead of O(n·h) worst case for duplicating a growing path into every queue entry, plus no need to hand-roll path-copying logic into a queue-based traversal.

**What we gain versus a two-pass BFS-plus-parent-map approach:** a single pass, no auxiliary hash map, and the path is available directly as you go, rather than reconstructed afterward by walking parent pointers back to the root.

**What we lose versus BFS:** BFS gives you level-order output and shortest-path-in-edges (unweighted) guarantees directly; Tree DFS does not naturally answer "what is at depth 3" without extra bookkeeping, and does not naturally give you "shortest path to the nearest matching node" the way BFS's level-by-level expansion does.

**What we lose versus an explicit-stack iterative version:** the explicit-stack version trades away recursion's simplicity for a hard ceiling on stack usage (you control the heap-allocated stack's growth, or at least it is not bound by the *thread's* stack size), which matters once tree height is a genuine risk.

## Complexity

**Time:** **O(n)** for all three implementations in [code.cpp](code.cpp) and for `problems/01`–`03` — every node is visited exactly once, and each visit does O(1) work (or O(1) amortized work; string/vector append is typically O(1) amortized). `problems/04` (max path sum) is also **O(n)**: each node's `bestDownward` call does O(1) work beyond its two recursive calls, and there are exactly `n` calls total.

**Space:** **O(h)** where `h` is the tree's height, for the recursion's call stack — this is the *auxiliary* space cost of the traversal mechanism itself, separate from whatever output you are building (a list of paths, which is itself O(n · h) in the worst case just to *store* every path, not because of the traversal). For a balanced tree, `h = O(log n)`; for a completely degenerate (linked-list-shaped) tree, `h = O(n)`.

**Contrast with Tree BFS:** BFS's queue can hold up to the widest level of the tree at once, which for a **complete/balanced** tree is `O(n)` (the last level alone can hold roughly half the nodes) — so BFS's worst-case space is O(n) even though its time complexity is also O(n). DFS's worst-case *space* (O(h)) is actually **better** than BFS's worst-case space (O(n)) on a wide, shallow, balanced tree; the situation flips on a narrow, deep, degenerate tree, where DFS's O(h) becomes O(n) too (and risks stack overflow, per Disadvantages) while BFS's queue stays small (O(1)-ish, since each level has only one node).

| Shape of tree | Tree DFS space (call stack) | Tree BFS space (queue) |
|---|---|---|
| Balanced (height ~log n) | O(log n) | O(n) (wide last level) |
| Degenerate / linked-list-shaped (height ~n) | O(n) (stack overflow risk) | O(1)-ish (narrow every level) |

## Common Mistakes

- **Forgetting to "un-append" a path element when backtracking out of a branch, if using a shared mutable path vector.** If you `path.push_back(node->val)` at the start of a call and recurse into both children using that *same* vector object (rather than a fresh copy per call, as `code.cpp`'s `binaryTreePaths` does with strings), you must `path.pop_back()` **after** both recursive calls return, before the function itself returns. Skip this and every sibling branch silently inherits leftover values from a branch that has already been fully explored and should have nothing to do with it — producing paths that are too long and contain values from the wrong branch entirely. *Avoid:* treat push/pop (or append/un-append) as a strict pair bracketing the two recursive calls, the same discipline as acquiring and releasing a lock.
- **Off-by-one when checking for a leaf node (both children null) versus just a null node.** Confusing "`node` is `nullptr`" (there is no node here at all — the base case for recursion) with "`node` is a **leaf**" (`node->left == nullptr && node->right == nullptr` — there is a real node here, it just has no children) leads to two different bugs: recording a path one node too early (if you stop at the first null child instead of confirming *both* are null) and double-counting or mis-triggering the "found a path" logic on nodes that have exactly one child. *Avoid:* write the leaf check as its own named condition (`bool isLeaf = !node->left && !node->right;`) rather than inlining it, and reason about the null-check (base case) and the leaf-check (recording condition) as two entirely separate questions.
- **Mixing up preorder, inorder, and postorder semantics.** Writing code that processes the current node's value *before* recursing when the problem actually needs the fully-resolved values of both children first (or vice versa) produces code that either uses a value that has not been computed yet, or computes the right value in the wrong place. *Avoid:* before writing a line of code, decide explicitly which of the three framings (Solution section above) the problem needs, and say out loud (or in a comment) whether you need "what I know coming in" (preorder) or "what my children resolved to" (postorder) — inorder is rare outside BST-specific problems, so if you find yourself reaching for it elsewhere, double-check that is really what is needed.
- **Treating the running "best" in a postorder aggregation as the function's return value, when it needs to be tracked separately.** In max path sum, the value a node *returns to its parent* (best downward extension through one child) is different from the value that could win as the *overall* answer (which is allowed to bend through both children at this node). Returning the "bends through both children" value up to the parent produces an invalid path in the parent's own computation, because a real path cannot branch. *Avoid:* keep a separate running "best answer so far" (a reference parameter or class/lambda-captured variable) distinct from the value returned up the call chain.
- **Not handling `nullptr` as the very first check in every recursive call.** Forgetting the base case (or putting it after code that dereferences `node`) causes a null-pointer dereference the moment recursion reaches a missing child. *Avoid:* make "if (!node) return <base case value>;" the literal first line of every Tree DFS function, no exceptions.

## When To Use

- The problem explicitly mentions **root-to-leaf paths** — enumerating them, summing them, or checking whether one exists matching a condition.
- The problem needs a value that depends on **the full depth of one branch**, like maximum depth/height, or "does a path exist with property X."
- The problem needs to **aggregate a property from children up to a parent** — tree height, whether the tree is height-balanced, diameter, maximum path sum (where the path can start/end anywhere, not just root-to-leaf).
- You need to **serialize or reconstruct a tree**, or evaluate an **expression tree** (compute the value represented by a tree of operators and operands) — both require visiting every node in a strict parent/child dependency order that only DFS (not level order) naturally provides.
- You are validating structural properties that depend on subtree contents, like **validating a binary search tree** (each subtree must fall within a value range derived from its ancestors) or checking whether two trees are structurally identical.

## When NOT To Use

- **Level-order output or level-aggregated questions** — "return the tree level by level," "find the minimum depth" (specifically minimum, where BFS can stop at the *first* leaf it reaches, often faster in practice than a full DFS traversal that must potentially explore deep, irrelevant branches first), "find the rightmost node at each level." That is **Tree BFS**'s territory (see [../tree-bfs/](../tree-bfs/)) — reach for a queue instead of recursion whenever "level" appears in the problem statement.
- **Shortest path in an unweighted general graph.** Even though Tree DFS can visit every node, it does not give you the shortest-hop-count guarantee that BFS's level-by-level expansion gives for free; that is Graph BFS's job.
- **The tree is extremely deep/unbalanced and stack overflow is a real operational risk** (e.g. processing untrusted, adversarially-shaped input trees in a production service) — either convert to the iterative explicit-stack form, or reject/rebalance pathologically deep inputs before traversing.
- **You need to explore many candidate *sequences of choices* and prune invalid branches early, generating combinatorially many results (permutations, subsets, combinations, N-Queens-style placements)** — that is **Backtracking**'s territory (see [../../recursion-backtracking-patterns/backtracking/](../../recursion-backtracking-patterns/backtracking/)), a close cousin that shares the "recurse, then undo on the way back" shape but is typically applied to a decision tree you build yourself (choices at each step), not a tree that already exists as input data.

## Real Interview/Production Examples

Tree DFS (root-to-leaf paths, path sums, max depth, diameter, validate-BST) is one of the most frequently asked tree-traversal families across essentially every major tech company's interview loop, precisely because it tests whether a candidate can correctly distinguish "carry state down" from "combine on the way back up" — Path Sum, Binary Tree Paths, Maximum Depth of Binary Tree, and Binary Tree Maximum Path Sum (this module's four worked problems) are among the most commonly cited tree-DFS questions in interview-prep material.

Beyond interviews, the same recursive shape shows up directly in real systems:

- **Compiler / interpreter Abstract Syntax Tree (AST) traversal.** An AST — the tree representation of parsed source code — is walked with exactly this recursion to type-check expressions, generate bytecode, or evaluate an interpreter's expression nodes: you cannot know an expression node's type or value until you know its children's types/values first (a postorder aggregation), and reporting "which function called which" for a stack trace is fundamentally a root-to-leaf-path problem over the call tree.
- **Filesystem directory tree walks.** Computing a directory's total size (`du -sh`-style), or finding every file matching a pattern by full path, is Tree DFS: total size is postorder aggregation (a directory's size is the sum of its children's, computed bottom-up), and "list every file's full path" is preorder path-building (the path so far is literally the directory path being extended into each subdirectory).
- **Expression tree evaluation.** A tree representing an arithmetic expression (operators as internal nodes, operands as leaves) is evaluated with postorder aggregation: you cannot compute an operator node's value until both of its operand subtrees have been fully evaluated — precisely the max-depth/max-path-sum shape applied to a different value being aggregated.
- **JSON/XML/HTML DOM tree processing.** Rendering, validating, or computing derived properties (total text length, whether all required fields are present) over a nested document tree uses the identical recursive walk, often needing both "path so far" (for error messages: "error at root.users[2].address") and "aggregate children's results" (for validation: "this node is valid only if all its children are").

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **Computing storage usage per directory in a file-management or backup service**, recursively summing each subdirectory's size (postorder aggregation) to produce a `du`-style report without shelling out to an external tool.
2. **Validating a deeply nested configuration or permissions tree** (e.g. an organization's team/sub-team hierarchy with inherited permissions), where a leaf team's effective permissions depend on every ancestor's settings — a preorder "carry the accumulated permission set down" traversal.
3. **Generating full breadcrumb paths for a nested category tree in an e-commerce or CMS backend** (`Electronics -> Computers -> Laptops -> Gaming Laptops`), by building up the path string as you recurse toward each leaf category, directly mirroring `binaryTreePaths`.
4. **Detecting the "riskiest" cost path through a nested decision/approval workflow tree** (e.g. total approval cost or risk score along any path from an initiating request down to a terminal approval/rejection), the same shape as Path Sum or Maximum Path Sum applied to a domain-specific tree instead of integers.
5. **Type-checking or evaluating a small expression language or rule engine** you have embedded in your service (e.g. a JSON-based "if this condition tree evaluates to true, apply this discount"), using postorder evaluation exactly as a compiler evaluates an AST.

## Similar Patterns

- **Tree BFS** ([../tree-bfs/](../tree-bfs/)): also a full traversal of every node in a tree, but processes nodes **level by level** using an explicit queue, rather than one branch at a time using recursion. Tree BFS answers "what is true at a given depth, or what is the level-order layout" and can short-circuit on **minimum** depth/distance questions because it reaches shallower nodes first; Tree DFS answers "what is true about one full root-to-leaf branch, or about a subtree's aggregate" and cannot naturally short-circuit on "shallowest" questions the way BFS does. Recognition signal: "level," "level order," "minimum depth" → BFS; "path," "path sum," "depth of this branch," "subtree property" → DFS.
- **Backtracking** ([../../recursion-backtracking-patterns/backtracking/](../../recursion-backtracking-patterns/backtracking/)): shares the exact "recurse deeper, then undo the choice on the way back up" mechanical shape (and, when a shared mutable path/state vector is used, the identical push-then-pop discipline described in Common Mistakes). The difference is *what* is being traversed: Tree DFS walks a tree that already exists as **input data** (the binary tree you were handed), visiting every node exactly once; Backtracking walks an **implicit decision tree that you construct as you go** (all the ways to place N queens, all subsets of a set, all permutations of an array), often visiting many more "nodes" than any fixed input structure, and frequently **pruning** a branch early once it is provably invalid — something Tree DFS problems in this module do not need, since every node in a given input tree is relevant and must be visited.

| Pattern | What it traverses | Movement style | Primary question answered |
|---|---|---|---|
| Tree DFS | A given binary tree (input data) | Recurse into `left`/`right`, one branch at a time to a leaf, backtrack | "What is true about this root-to-leaf path / this subtree's aggregate?" |
| Tree BFS | A given binary tree (input data) | Queue, one level at a time | "What is true at this depth? What is the level-order layout? What is the minimum depth?" |
| Backtracking | An implicit decision tree (built during the search) | Recurse into each choice, undo (backtrack) if invalid or after exploring, prune early | "Enumerate every valid combination/arrangement of choices" |

## Interview Discussion

Experienced engineers rarely spend interview time on "how do you write a recursive tree function" — the skeleton (`if (!node) return base; recurse(left); recurse(right);`) is mechanical. What they actually probe is whether you can **correctly identify which of the two main shapes (preorder carry-down vs. postorder combine-up) a given problem needs**, and whether you can precisely state what your recursive function's return value *means* — is it "the answer for this whole subtree," or "the best downward-only extension a parent is allowed to use"? Confusing those two in a max-path-sum-style problem is the single most common real mistake candidates make live.

Common follow-up questions:
- *"Can you do this iteratively instead of recursively?"* — expects you to name the explicit-`std::stack` transformation, and to acknowledge that postorder is the hardest of the three to do iteratively (needing a second stack or a "last visited" marker), not just say "sure, use a stack" without engaging with why postorder is different.
- *"What happens on a very deep, skewed tree?"* — expects recognition that recursion depth equals tree height, that a degenerate (linked-list-shaped) tree has height O(n), and that this risks a stack overflow — a real production concern, not just a theoretical one, when input trees are not guaranteed to be balanced (e.g. built from untrusted or adversarial input).
- *"In Binary Tree Maximum Path Sum, why do you return one value up to the parent but track a different value as the answer?"* — expects the precise distinction: a real path cannot branch, so what a parent may extend through is at most one child's contribution, while the globally best path is allowed to bend through both children at exactly one node (see Execution Flow above).
- *"Why not just use BFS for everything, since it also visits every node?"* — expects recognition that BFS does not naturally carry "the path so far" without duplicating it per queue entry, and that some problems (subtree aggregation, where a parent needs both children's *fully resolved* answers) have no natural level-order analogue at all.
- *"How would you validate that a binary tree is a valid BST?"* — expects a preorder-with-range-carried-down solution (each recursive call is handed a valid `(min, max)` range from its parent, and must both satisfy it and narrow it for its own children) — a slightly different but very common preorder variant worth being able to produce on the spot.

Common misconceptions:
- "DFS and recursion are the same thing." Recursion is the *usual implementation vehicle* for DFS on a tree, but DFS is the traversal *strategy* (go deep before wide); it can equally be implemented iteratively with an explicit stack, and recursion is also used for plenty of non-DFS things (divide and conquer, dynamic programming with memoization).
- "Preorder/inorder/postorder are only relevant for printing/serializing a tree." They describe a fundamental choice about *when* a node's own value is used relative to its children's — which directly determines whether you can solve a "carry state down" problem or a "combine children's answers" problem with a given traversal order.
- "You always need to pass the whole path as a vector." Many problems only need a running scalar (a sum, a count, a boolean) carried down, not the literal sequence of values — reach for the full path vector only when the problem asks for the path itself (as in Binary Tree Paths / Path Sum II), not for path-derived aggregates (as in Path Sum, which only needs a running sum).
- "The return value of a postorder function is always 'the final answer.'" Frequently it is an intermediate value the *parent* needs (a subtree height, a best downward-only extension), while the actual problem answer is tracked separately, as in Maximum Path Sum.

## Summary

- Tree DFS recurses into a node's children, going as deep as possible down one branch before backtracking — the recursive call stack itself carries the "path so far."
- Two main framings: **preorder** (process the node, then recurse — carry state *down* as a parameter) for path-collection/path-sum problems; **postorder** (recurse into both children fully, then combine their results with the current node) for aggregation problems (max depth, balance, diameter, max path sum).
- The call stack **is** the mechanism *and* the data — no explicit path-tracking structure is needed for the common case, unlike BFS with manually-attached per-node paths.
- Time is O(n) for a single full traversal; space is O(h) for the call stack, where `h` is tree height — O(log n) for balanced trees, O(n) worst case for degenerate ones.
- The single biggest correctness risk is confusing a node's **return value up to its parent** with the problem's actual answer, especially in "the path can bend through this node" aggregation problems.
- A shared mutable path vector needs strict push-then-pop (append/un-append) discipline bracketing the two recursive calls; forgetting the pop leaks state into sibling branches.
- Reach for Tree BFS instead the moment the problem says "level"; reach for Backtracking instead when you are building your own decision tree of choices rather than traversing a tree handed to you as input.
- Converting to an explicit `std::stack` iterative version is a real option once recursion depth is a genuine risk, but postorder is meaningfully harder to make iterative than preorder.

## Key Takeaways

1. Tree DFS recurses into `left`/`right`, going all the way to a leaf before backtracking; the call stack itself is the "path so far" — no extra data structure needed for the common case.
2. Preorder ("process, then recurse") carries state *down* as a parameter — the shape for root-to-leaf path/sum problems.
3. Postorder ("recurse fully, then combine") uses the recursive *return value* to aggregate children's results into the parent's answer — the shape for depth, balance, diameter, and max path sum.
4. Distinguish a function's return value (what the parent is allowed to use) from the problem's actual answer (often tracked in a separate running "best so far" variable) — the single most common real interview mistake.
5. Time is O(n); space is O(h) for the call stack, which is O(log n) on a balanced tree but O(n) on a degenerate one — a real stack-overflow risk on adversarial or pathological input.
6. A leaf is a node with **both** children null — not just any node with a null child; conflating "null node" (base case) with "leaf node" (recording condition) is a common off-by-one bug.
7. A shared mutable path vector needs push-then-pop bracketing the recursive calls; forgetting the pop leaks state into sibling branches that should share nothing but a common ancestor.
8. Tree DFS's O(h) worst-case-typical space actually beats Tree BFS's O(n) worst-case space on a wide, balanced tree — but the situation flips on a narrow, deep tree.
9. Reach for Tree BFS instead whenever "level" appears in the problem; reach for Backtracking instead when the tree being explored is a decision tree you build, not input data you were handed.
10. Real systems use this exact recursive shape for compiler AST traversal, filesystem directory-size computation, and expression-tree evaluation — it is not just an interview construct.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — the standard reference for tree/graph traversal fundamentals, recurrence-based complexity analysis, and recursion tree reasoning.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — has a dedicated Trees and Graphs chapter covering DFS variants (preorder/inorder/postorder) with interview framing.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes worked binary tree recursion problems with C++-specific implementation notes (pointer management, recursion depth considerations).
- *Algorithms* (4th edition) — Sedgewick & Wayne — covers binary tree traversal and recursive tree algorithms with a strong emphasis on invariants and correctness arguments.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including multiple binary tree traversal implementations (recursive and iterative) useful for comparing styles.
- LLVM / Clang's AST classes (`clang::Stmt`, `clang::Expr` traversal via `RecursiveASTVisitor`) — a real, production-grade example of postorder-style recursive traversal over a compiler's abstract syntax tree.
- CPython's `ast` module (the `NodeVisitor`/`NodeTransformer` classes) — a widely-read, approachable example of recursively walking a parsed program's syntax tree, conceptually identical to Tree DFS over a binary tree.

**Official Documentation**
- LeetCode — Path Sum (problem 112).
- LeetCode — Binary Tree Paths (problem 257).
- LeetCode — Path Sum II (problem 113).
- LeetCode — Binary Tree Maximum Path Sum (problem 124).
- cppreference.com — recursion and the call stack; `std::stack` (for the iterative/explicit-stack variant discussed in Disadvantages).

**Blog Articles**
- GeeksforGeeks — "Tree Traversals (Inorder, Preorder and Postorder)" — a widely used explainer covering all three traversal orders with diagrams.
- NeetCode — Trees pattern videos/playlist — walks through Maximum Depth of Binary Tree, Path Sum, and Binary Tree Maximum Path Sum with visual call-stack explanations.
- Educative.io — "Grokking the Coding Interview" Tree DFS pattern chapter — one of the most widely referenced pattern-based framings of this exact technique.
