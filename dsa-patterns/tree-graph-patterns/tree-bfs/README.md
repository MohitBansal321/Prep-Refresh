# Tree BFS

> **In one line:** snapshot `q.size()` *before* consuming the queue — that count is exactly the current level, because children pushed during the loop are invisible to it.

```cpp
while (!q.empty()) {
  size_t level_size = q.size();        // <-- the whole pattern is this line
  std::vector<int> level_values;

  for (size_t i = 0; i < level_size; ++i) {
    TreeNode* node = q.front();
    q.pop();
    level_values.push_back(node->val);

    if (node->left  != nullptr) q.push(node->left);   // next level
    if (node->right != nullptr) q.push(node->right);
  }
  result.push_back(std::move(level_values));
}
```

**O(n)** time · **O(w)** space (w = widest level). Full runnable version, with `minDepth` and the edge cases: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — learning it | box above → [images/trace-diagram.md](images/trace-diagram.md) → [code.cpp](code.cpp) → [problems/](problems/) |
| **40 min** — first exposure | read on ↓ then [exercises.md](exercises.md) |
| **hands on keyboard** | [exercises/](exercises/) — 5 stub files with assertions and hints, no solutions. Compile and you get `[PASS]`/`[FAIL]` straight away. |

---

## Intent

Visit every node of a tree **level by level** — all nodes at depth 0, then all nodes at depth 1, then all at depth 2, and so on — using a queue, whenever a problem's answer depends on grouping nodes by their distance from the root rather than on the paths between them.

## Real Life Analogy

Picture an announcement spreading through a company's **org chart**, one reporting level at a time. The CEO tells their direct reports on Monday morning. Each of those direct reports tells *their* direct reports on Monday afternoon. Each of those tells *their* reports on Tuesday morning. Nobody two levels below the CEO hears the announcement before everybody one level below does — the announcement moves outward in synchronized "rounds," and everyone in the same round hears it at the same time, regardless of which branch of the org chart they sit in.

Now suppose you want to answer questions like "how many people are in the third round?" or "what is the fewest number of rounds before someone in Sales hears it?" Those questions are trivial if you tracked the announcement round by round. They are awkward if you only tracked "who told whom" as a tangle of individual conversations (a path-based view of the same organization) — you would have to reconstruct the rounds after the fact by computing everyone's distance from the CEO.

Tree BFS is that round-by-round announcement in code: a queue holds "everyone in the current round," and the algorithm empties that exact set of people before moving to the next round.

> **Term: BFS (Breadth-First Search).** A traversal strategy that visits all nodes at the current distance from the start before visiting any node farther away. "Breadth" refers to spreading sideways across a level before going deeper — the opposite emphasis of depth-first search, which dives down one path as far as it can before backtracking.

## Problem

### What engineering problem exists?

A large class of tree problems is naturally phrased in terms of **levels** (depth from the root), not **paths** (root-to-node or root-to-leaf sequences):

- **"Return the tree's values grouped by level."** (`[[3], [9, 20], [15, 7]]` rather than a single flat list.)
- **"What is the minimum depth to reach a leaf?"** — an inherently level-by-level question, because "minimum" means "first level at which this becomes true."
- **"What is the maximum width of the tree?"** — the largest number of nodes that ever exist in a single level.
- **"Connect each node to its right-hand neighbor at the same level."** — impossible to answer without first knowing, precisely, which nodes belong to the same level.
- **"Return the average / sum of each level's values."** — a level-aggregation question by definition.

The recursive traversal every engineer learns first — Tree DFS, visiting a node, then recursing into its left subtree, then its right subtree — visits nodes in **path order**: root, then root's *entire* left subtree (however deep), then root's *entire* right subtree. It does not naturally group nodes by level at all; a DFS call at depth 5 in the left subtree runs long before the DFS call at depth 2 in the right subtree, even though depth 2 is "earlier" in level order. Level information exists in a DFS traversal only if you thread a `depth` parameter through every call and manually bucket results by that depth afterward — the traversal order itself does not do the grouping for you.

### Why is this problem difficult?

- **Recursion mirrors "go deep," not "go wide."** A recursive function call naturally models "solve the left subtree completely, then the right subtree" — which is depth-first by construction. There is no equally natural recursive expression of "process everyone at this depth before touching anyone at the next depth," because recursion's call stack tracks *how you got here* (the path), not *how far you've spread out* (the level).
- **You need an explicit, mutable frontier.** To process "the current level" as a genuine batch, you need some structure holding exactly the nodes at that level, separate from the nodes that will make up the *next* level (their children). A plain recursive call has no such structure — it has only "the current node" and "the accumulated path so far."
- **Level boundaries are easy to lose track of.** Once you introduce a queue holding a mix of nodes, it is easy to accidentally process a few nodes from the next level as if they were still part of the current one, silently corrupting the grouping (see Common Mistakes below).

### What happens if we ignore it?

- **You reconstruct level information after the fact, at extra cost.** A DFS-plus-depth-parameter approach works, but it means bucketing results by depth in a separate data structure (e.g. a `vector<vector<int>>` indexed by depth) after or during the recursive walk — extra bookkeeping that a level-native traversal does not need, and an extra place to introduce an off-by-one bug (see Why Not Other Approaches below).
- **Minimum-depth questions become slower than necessary.** A DFS solution to "minimum depth" must explore **every** root-to-leaf path and keep a running minimum, because it has no way to know in advance which path is shortest — it might fully explore a 50-node-deep path before finding the single 2-node-deep path that was the real answer. A BFS solution finds the first leaf and stops immediately, because BFS's visiting order guarantees the first leaf found is at the minimum depth.
- **Level-dependent connections (next-right pointers, zigzag order) become error-prone.** Without an explicit notion of "this batch of nodes is one level," code that tries to connect siblings or reverse alternate rows ends up guessing at level boundaries instead of having them handed to it directly by the traversal.

## Why Not Other Approaches?

**"Use Tree DFS with a `depth` parameter, and post-process by depth."**
This absolutely works, and for some problems (e.g. "return the deepest leaves") it can even be the more natural choice. The recursive call carries `depth + 1` downward, and at each node you push its value into `buckets[depth]` (growing a `vector<vector<int>>` on demand, or pre-sizing it once you know the tree's height). The result is functionally equivalent to Tree BFS's `levelOrder` output. But it is doing extra, avoidable work: you need a place to accumulate per-depth buckets that persists *across* recursive calls (typically threaded through as a reference parameter or a member/closure variable), and you are computing "which level is this node in" indirectly (by counting stack depth) instead of it being a direct, structural property of the traversal itself, the way it is with a BFS queue. For a problem framed explicitly as "process level by level, as you go" — where you want to *act* on a level's nodes together (aggregate them, connect them, reverse them) rather than just label them and sort it out afterward — DFS-plus-depth is strictly more indirect: the natural unit of iteration (a level) never actually exists as a concrete collection during a DFS walk; you have to synthesize it from scattered recursive calls.

**"Do a plain DFS and just track the minimum depth seen at any leaf."**
Correct, but wasteful for the *minimum* depth question specifically: DFS must in the worst case visit every node in the tree (all root-to-leaf paths) before it can be sure it has seen the true minimum, because nothing about DFS's visiting order favors shallow leaves over deep ones. BFS, by contrast, visits nodes in strictly non-decreasing depth order, so it can return the moment it sees the *first* leaf — no need to look at the rest of the tree at all in the best case (e.g. a leaf directly under the root, on either side).

**"Flatten the tree into a list of (node, depth) pairs, then group by depth."**
This is DFS-plus-depth wearing different clothes — same asymptotic cost, plus an explicit grouping pass (e.g. a `std::map<int, vector<int>>` or a sort by depth) after the traversal finishes. It adds a data-structuring step that a level-native BFS traversal gets for free, as a direct consequence of the order nodes are dequeued in.

**Tradeoff summary:** DFS-with-depth-tracking is not *wrong* — it is asymptotically the same O(n) time and can even be preferred for pure path/depth questions (see Tree DFS's own README). But for any problem whose natural unit of work is "a level, processed together, before moving to the next one" — level-order output, minimum depth with early exit, level aggregation, connecting same-level siblings — Tree BFS's queue gives you that unit directly, with no reconstruction step, and (for minimum depth specifically) a genuine early-exit speed advantage DFS structurally cannot offer.

## Solution

The core idea is a single data structure and a single discipline around it: hold a **queue** containing exactly the nodes of "the level currently being processed," and before enqueuing any of their children (who belong to the *next* level), make sure you have counted and processed every node that was in the queue for *this* level.

Concretely: start with just the root in the queue. Repeatedly, take a "snapshot" of how many nodes are currently in the queue — that count *is* the size of the current level, because every node belonging to the current level was pushed onto the queue during the *previous* iteration, and nothing else has been added since. Process exactly that many nodes (dequeuing each one, recording it, and pushing its non-null children onto the back of the queue for later). Once you have processed exactly that many, every node still left in the queue is a child that was just pushed — i.e., the *next* level, in full, with nothing from the level after that mixed in (because grandchildren have not been pushed yet). Repeat until the queue is empty.

This is the entire idea. No code yet, on purpose: the trick that makes this work is not the queue itself (a queue is a queue) — it is the discipline of reading the queue's size **before** you start consuming it for the current level, rather than checking `queue.empty()` node-by-node without regard to which level a node belongs to.

## Architecture

- **The queue.** Holds nodes waiting to be visited, always in strict level order: the root first, then (after the root is dequeued) both its children in left-to-right order, then (after those are dequeued) all four grandchildren in left-to-right order, and so on. The queue's FIFO (first-in-first-out) discipline is exactly what preserves "visit shallower nodes before deeper ones, and left-to-right within a level" — a stack (LIFO) would not preserve this order.

- **The "level size snapshot" trick.** Before processing a level, you record `size = queue.size()` once. This number is the participant that actually enforces the level boundary: it tells the inner loop "process exactly this many nodes, no more," even though the queue's *actual* size keeps growing (as children get pushed) while that inner loop runs. Without this snapshot, a loop like `while (!queue.empty())` run naively for "one level" would keep consuming the children you just pushed, silently sliding into the next level and merging two levels' worth of nodes into one. The snapshot is what turns "a queue of nodes" into "a queue of *levels*."

- **The current node being dequeued.** Represents one unit of work within a level: read its value (for level-order output), test whether it is a leaf (for minimum depth), or connect it to its sibling (for next-right pointers) — then push its children (if any) so they are available for the *next* level's snapshot.

- **The result accumulator.** Whatever the problem needs built up across levels — a `vector<vector<int>>` of per-level values, a running depth counter, a set of next-pointers — is updated once per level (after that level's snapshot-bounded inner loop finishes), not once per individual node globally, because "one level's worth of work" is the natural unit of progress in this pattern.

## Execution Flow

1. If the tree is empty (root is `nullptr`), there are no levels to process — return immediately with an empty/zero result.
2. Create a queue and push the root node onto it. The queue now holds exactly level 0 (one node).
3. While the queue is not empty:
   a. Snapshot the current queue size into `level_size` — this is the exact number of nodes belonging to the level about to be processed, and it will not change even as children are pushed during this level's processing.
   b. Repeat exactly `level_size` times:
      i. Dequeue the front node.
      ii. Do whatever work the problem requires with that node's value (append to a level list, check "is this a leaf," link it to the previously-dequeued node in this same level, etc.).
      iii. Push the node's non-null left child, then its non-null right child, onto the back of the queue — these belong to the *next* level and must not be touched until this level's `level_size` iterations are done.
   c. Now that exactly `level_size` nodes have been processed, everything currently in the queue is the next level, in full, and nothing from farther levels — return to step 3.
4. When the queue is finally empty (every level has been processed, including the last one, which pushed no further children), assemble and return whatever result the problem asked for.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the flowchart distinguishing Tree BFS from Tree DFS and from Graph BFS/DFS, based on the signals in a problem statement (is the input a tree? does the wording mention "level order," "level by level," or "minimum depth"?).

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the queue-based level-by-level loop, including exactly where the level-size snapshot is taken relative to the inner processing loop.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a concrete, step-by-step trace of the queue's contents at every level on a small binary tree (the same tree used in [code.cpp](code.cpp): `3` at the root, `9` and `20` as its children, `15` and `7` as `20`'s children).

## Implementation

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the queue-and-snapshot mechanic clearly, separated from any one problem's details, before looking at the worked, problem-specific solutions in [problems/](problems/).

It provides two small functions built on the same `TreeNode` shape LeetCode itself uses:

- `levelOrder` — the canonical routine: collect every level into its own `vector<int>`, returning `vector<vector<int>>`.
- `minDepth` — the routine that demonstrates BFS's early-exit advantage: return the moment the first leaf is found, since BFS guarantees that leaf is at the minimum depth.

Both share the identical queue-and-snapshot skeleton; only what happens to each dequeued node differs. That repetition is deliberate — it is the clearest way to see that the "level size snapshot" discipline, not the per-node logic, is the actual pattern.

## Code Walkthrough

**`TreeNode`** (in [code.cpp](code.cpp)). The plain binary tree node every function in this module operates on: an `int val` and two raw `TreeNode*` children, defaulted to `nullptr` in the constructor. This is deliberately the same shape LeetCode itself uses for tree problems, so the functions here transfer directly onto real problem statements with no adaptation needed.

**`buildSampleTree`** (in [code.cpp](code.cpp)). Constructs one small, fixed tree (`3` at the root, `9` and `20` as children, `15` and `7` as `20`'s children) used by every demo in `main()`, and reused in this README's diagrams so a reader can follow one concrete example from prose, to diagram, to running code.

**`freeTree`** (in [code.cpp](code.cpp)). A small recursive post-order walk (left, then right, then the node itself) that `delete`s every node. C++ has no garbage collector — every `new TreeNode(...)` in this module is paired with exactly one call to `freeTree` (or an equivalent teardown) so `main()` runs cleanly under a leak checker.

**`levelOrder`** (in [code.cpp](code.cpp)). Pushes the root, then loops while the queue is non-empty. Each iteration snapshots `level_size = q.size()` before its inner `for` loop, builds a fresh `level_values` vector, and for exactly `level_size` iterations: pops the front node, records its value, and pushes its non-null children. Because children are pushed *after* the snapshot was taken, they are invisible to this level's inner loop and only become visible once the outer `while` loop re-reads `q.size()` on the next pass. This function exists to demonstrate the level-size-snapshot mechanic in its most direct form — it is the ancestor of [problems/01-binary-tree-level-order-traversal.cpp](problems/01-binary-tree-level-order-traversal.cpp).

**`minDepth`** (in [code.cpp](code.cpp)). Identical queue-and-snapshot skeleton, but instead of unconditionally recording every node's value, it checks `node->left == nullptr && node->right == nullptr` (the precise definition of a leaf: *zero* children, not "missing one child") and returns the current `depth` immediately on the first such node found. Because BFS guarantees nodes are dequeued in non-decreasing depth order, the first leaf found is provably at the minimum depth — no need to keep searching. This function exists to demonstrate BFS's early-exit advantage over DFS for minimum-depth questions, and is the ancestor of [problems/03-minimum-depth-of-binary-tree.cpp](problems/03-minimum-depth-of-binary-tree.cpp).

**`main()`** (in [code.cpp](code.cpp)). Exercises both functions against the sample tree plus edge cases (empty tree, single node, a one-sided chain to specifically exercise the "single child is not a leaf" rule), printing `[PASS]`/`[FAIL]` for each assertion and freeing every allocated node.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem, defining its own `TreeNode` (or `Node`, for problem 04) rather than including `code.cpp`, so every file compiles and reads independently. See [problems/README.md](problems/README.md) for the index. Briefly: `01` is the pure level-order collection; `02` adds direction-alternation on top of the same skeleton; `03` is the pure early-exit minimum-depth routine; `04` uses the level-size snapshot to link siblings within a level via a `next` pointer instead of collecting their values.

## Advantages

- **Levels are a first-class, directly available unit.** No reconstruction step is needed to know "which nodes belong together" — the level-size snapshot hands you exactly that set on every iteration.
- **Natural early exit for "minimum" questions.** Because BFS visits nodes in non-decreasing depth order, "minimum depth to reach X" can return the instant the first qualifying node is dequeued, without exploring deeper parts of the tree that could not possibly contain a shallower answer.
- **Straightforward, iterative control flow.** No recursion, no call stack to reason about, no risk of a stack-depth limit on a very deep (though narrow) tree — the queue lives on the heap and can grow as large as available memory allows.
- **Composability with level-dependent transformations.** Reversing alternate levels (zigzag order), connecting same-level siblings (next-right pointers), or computing per-level aggregates (sums, averages, max width) all slot into the same skeleton by changing only what happens *within* a level's inner loop.
- **Deterministic, easy-to-reason-about ordering.** Left-to-right within a level, shallow-to-deep across levels — a fixed, predictable visiting order that is simple to state and simple to prove correct.

## Disadvantages

- **O(n) space for the queue in the worst case.** A wide, shallow tree (e.g. a nearly-complete binary tree) can have up to roughly `n / 2` nodes in its last (widest) level, all sitting in the queue simultaneously — proportional to the total node count `n`, not to the tree's height.
- **Less natural for path-based questions.** "Return every root-to-leaf path," "does any path sum to a target," "find the lowest common ancestor" — all of these are naturally expressed in terms of a single path being built up and unwound as recursion goes deeper and returns, which is exactly what Tree DFS's call stack gives you for free. Tree BFS's queue holds isolated nodes, not paths, so answering a path question with BFS means manually carrying path information alongside each queued node — extra bookkeeping DFS does not need.
- **No output until a full level completes (for level-order style output).** Unlike DFS, which can emit a result as soon as it hits a base case deep in the recursion, level-order BFS's per-level result is only complete once every node in that level has been dequeued and processed.
- **Requires an explicit queue.** Unlike DFS, which piggybacks on the language's own call stack, BFS needs you to manage an explicit `std::queue` (or equivalent) — one more piece of state to get right, and one more thing to check for correct emptiness/termination.

## Tradeoffs

**What we gain versus Tree DFS:** direct, level-native grouping with zero reconstruction cost, plus a genuine early-exit speed advantage on minimum-depth-style questions, because BFS's visiting order is structurally aligned with "shallowest first."

**What we lose versus Tree DFS:** natural expression of path-based questions (root-to-leaf paths, path sums, ancestor relationships), and O(1) auxiliary space — DFS's recursive call stack costs only O(h) (tree height) in the worst case, while BFS's queue costs O(n) in the worst case for a wide tree, because an entire level's worth of nodes can be in flight at once.

**What we gain versus DFS-with-a-depth-parameter (the alternative that also produces level-grouped output):** the level itself exists as a concrete, iterable collection at every step of the traversal, not as an emergent property you have to bucket after the fact by reading a `depth` value carried through recursive calls.

## Complexity

**Time:** **O(n)** — every node is enqueued exactly once and dequeued exactly once, and each dequeue does O(1) work (record a value, check leaf-ness, push up to two children) before the constant-time per-node cost of building the result. This holds for every function in this module, whether or not an early exit is possible.

**Space:** **O(n)** worst case for the queue — a wide, shallow tree can have up to roughly `n / 2` nodes (the entire last level of a complete binary tree) sitting in the queue at the same time. This is the single most important complexity contrast with Tree DFS: DFS's recursive call stack costs **O(h)**, where `h` is the tree's height — for a balanced tree, `h = O(log n)`, dramatically less than BFS's O(n) worst case; for a completely unbalanced (chain-like) tree, `h = O(n)` and DFS's stack cost converges to the same O(n) BFS pays, but BFS's queue is O(n) *regardless* of shape, while DFS's O(h) genuinely varies with the tree's shape.

**minDepth specifically:** the worst case is still O(n) (a tree where the only leaf is the very last node visited, or the tree has no early-exit opportunity), but the **best case** is O(1) relative to the tree's total size — if a leaf exists directly under the root, BFS finds it on the very first level and returns without touching most of the tree. Tree DFS's minimum-depth solution has no equivalent best case: it must explore every root-to-leaf path (true O(n) always) because nothing in its traversal order favors finding a shallow leaf sooner.

| Operation | Time | Space |
|---|---|---|
| `levelOrder` | O(n) | O(n) worst case (widest level) |
| `minDepth` (BFS) | O(n) worst case, O(1)-relative-to-n best case | O(n) worst case |
| `minDepth` (DFS, for contrast) | O(n) always (no early exit) | O(h) — height, not node count |

## Common Mistakes

- **Forgetting to snapshot the queue size before the inner loop, and accidentally mixing levels.** Writing `for (int i = 0; i < q.size(); ++i)` directly, instead of first storing `level_size = q.size()` into a separate variable, re-evaluates `q.size()` on every iteration of the `for` loop — and since children get pushed onto the queue *during* that same loop, `q.size()` keeps growing, so the loop condition never converges the way you expect and ends up silently consuming next-level nodes as if they belonged to the current level. *Avoid:* always capture the size into a named local variable once, before the inner loop begins, and loop by that fixed count, not by re-checking `q.size()`.
- **Not checking for null children before enqueueing.** Pushing `node->left` and `node->right` onto the queue unconditionally — without first checking they are non-null — enqueues `nullptr` values that then crash (or require awkward null-guards) when later dequeued and dereferenced. *Avoid:* always guard each push with `if (node->left != nullptr)` / `if (node->right != nullptr)`, exactly as shown in [code.cpp](code.cpp).
- **Off-by-one on depth counting.** Starting `depth` at `0` instead of `1` (or vice versa) silently shifts every depth-dependent answer by one — e.g. reporting a single-node tree's minimum depth as `0` instead of the problem's expected `1`. *Avoid:* fix the convention explicitly up front ("a single node has depth 1," matching LeetCode's convention) and initialize the counter to match, then trace through a one-node and a two-level example by hand before trusting the code.
- **Treating "has one missing child" as "is a leaf."** For minimum-depth questions specifically, a node with exactly one child is *not* a leaf — depth is measured to the nearest node with **zero** children. Returning as soon as `node->left == nullptr || node->right == nullptr` (a bug: should be `&&`) produces a wrong, too-small answer whenever the tree has a node with only one child. *Avoid:* the leaf check must be `node->left == nullptr && node->right == nullptr` — both children absent, not just one — and test this explicitly with a one-sided chain, as [code.cpp](code.cpp)'s `main()` does.
- **Using a stack instead of a queue.** Swapping `std::queue` for `std::stack` (or otherwise processing nodes LIFO instead of FIFO) breaks the level ordering entirely — you would still visit every node, but not in level order, silently invalidating any level-dependent output. *Avoid:* Tree BFS specifically requires FIFO order; if you find yourself reaching for a stack, you have drifted into Tree DFS territory (which is fine, but is a different pattern with a different recognition signal).

## When To Use

- The problem explicitly asks for **level-order output** — values grouped by depth from the root.
- The problem asks for **minimum depth**, **shortest path in an unweighted structure to some target node**, or any "first level at which X becomes true" question, where BFS's early-exit property gives a genuine speed advantage over exploring the whole tree.
- The problem needs **per-level aggregation** — sums, averages, maximums, or counts computed independently for each level.
- The problem needs to **connect or compare nodes that share the same level** — next-right pointers, zigzag traversal, checking whether a level is a palindrome, finding the level with the maximum width.
- You need an **iterative** traversal (no recursion), for instance to avoid stack-depth concerns on a very deep tree, or because the surrounding code is already iterative.

## When NOT To Use

- **Root-to-leaf path questions** — "return all root-to-leaf paths," "does any path sum to a target," "find the longest/shortest path between two specific nodes," "find the lowest common ancestor." These are naturally expressed with a single path built up and unwound as recursion proceeds — that is **Tree DFS**'s territory, not Tree BFS's (see [../tree-dfs/](../tree-dfs/)).
- **Subtree-property questions** that need a bottom-up answer (e.g. "is this tree balanced," "what is the diameter of this tree") — these are naturally computed via post-order recursion, where a parent's answer depends on its children's already-computed answers. Tree BFS's top-down, level-by-level order does not fit this dependency direction.
- **The input is not a tree but a general graph** (possibly with cycles, possibly disconnected, possibly with multiple valid "roots") — you need an explicit `visited` set to avoid infinite loops and re-visiting nodes, which is **Graph BFS/DFS**'s job, not Tree BFS's (see [../graph-bfs-dfs/](../graph-bfs-dfs/)).
- **Memory is tightly constrained and the tree is very wide** — Tree BFS's O(n) worst-case queue can be a real problem for a wide, shallow tree in a memory-constrained environment; Tree DFS's O(h) stack usage may be preferable if the tree is also reasonably balanced.

## Real Interview/Production Examples

Tree BFS (specifically "level order traversal") is one of the most frequently asked tree-traversal patterns in coding interviews — Binary Tree Level Order Traversal, Minimum Depth of Binary Tree, Binary Tree Zigzag Level Order Traversal, and Populating Next Right Pointers are all recurring questions at major tech companies, precisely because they test whether a candidate reaches for an explicit queue (and gets the level-size snapshot right) rather than defaulting to recursion out of habit.

Beyond interviews:

- **Org-chart-style hierarchical notifications.** Broadcasting an announcement or a permission change down a management hierarchy or a folder-sharing hierarchy, one reporting/nesting level at a time — the exact real-life analogy this README opened with, implemented literally in systems that model hierarchical org structures or nested resource permissions.
- **Serialization by level.** Some tree/graph serialization formats (and some database representations of hierarchical data, such as nested-set or adjacency-list exports) serialize a tree level by level so a deserializer can reconstruct it top-down, allocating each level's nodes before needing to know about the next level down.
- **UI tree-rendering, level by level.** Rendering a deeply nested UI component tree, a file-system explorer, or a comment-thread tree where the UI wants to render (or lazily load) one depth level at a time — e.g. showing top-level comments first, then loading replies-to-replies only as the user expands them, mirroring BFS's level-by-level frontier rather than eagerly recursing to full depth.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **Rolling out a feature flag or config change through an org/tenant hierarchy** (parent account, then its direct sub-accounts, then their sub-accounts) in controlled batches, so you can pause or roll back between levels if a level's rollout misbehaves.
2. **Computing the "blast radius" of a change in a dependency tree** (e.g. which services would be affected, grouped by how many hops away they are from a changed service) — a direct level-order collection over a service-dependency tree.
3. **Building a "load one level of a lazily-fetched tree at a time" API** for a large hierarchical resource (an org chart, a deeply nested category tree in a catalog) — matching a paginated or on-demand UI that only wants the next level's children, not the whole subtree.
4. **Finding the minimum number of hops to the nearest node matching a condition** in a hierarchical structure (e.g. the nearest ancestor category with a discount rule, searched outward from a leaf category) — the same early-exit BFS idea as `minDepth`, applied to "nearest match" instead of "nearest leaf."
5. **Computing per-level statistics for monitoring/reporting** on a hierarchical data structure — e.g. average queue depth per level of a job-dependency tree, or fan-out counts per level of an org chart, both direct applications of level-order aggregation.

## Similar Patterns

- **Tree DFS** ([../tree-dfs/](../tree-dfs/)): also a full traversal of every node in a tree, but in **path order** (root, then entire left subtree, then entire right subtree) via recursion (or an explicit stack), rather than level order via a queue. Tree DFS is the natural fit for root-to-leaf paths, path sums, and subtree properties that need a bottom-up (post-order) computation; Tree BFS is the natural fit for level-order output, minimum depth, and same-level connections. Both are O(n) time; the space cost differs structurally (BFS: O(n) worst case for a wide tree; DFS: O(h) worst case for a deep tree).
- **Graph BFS/DFS** ([../graph-bfs-dfs/](../graph-bfs-dfs/)): the same two traversal *shapes* (level-by-level via a queue, or path-by-path via recursion/a stack), generalized to structures that are not trees — meaning they may have cycles, multiple disconnected components, and no single obvious "root." Graph BFS/DFS must track an explicit `visited` set to avoid infinite loops re-processing the same node through a cycle; Tree BFS never needs this, because a tree has no cycles and exactly one path from the root to any node, so no node can ever be reached, and re-enqueued, twice.

| Pattern | Structure | Traversal order | Needs `visited` tracking? | Primary question answered |
|---|---|---|---|---|
| Tree BFS | Tree (no cycles, one root) | Level by level (queue, FIFO) | No — a tree has no cycles | "What's the level-order output? The minimum depth? The per-level aggregate?" |
| Tree DFS | Tree (no cycles, one root) | Path by path (recursion / stack, LIFO) | No — a tree has no cycles | "What are the root-to-leaf paths? Path sums? Subtree properties?" |
| Graph BFS/DFS | General graph (cycles allowed, possibly disconnected) | Level by level, or path by path, over reachable nodes | **Yes** — required to avoid infinite loops on cycles | "What's the shortest path (BFS)? Is it connected, or is there a cycle (DFS)?" |

## Interview Discussion

Experienced engineers do not spend interview time on "can you write a queue-based loop" — that is mechanical. What they actually probe is whether you understand **why** the level-size snapshot is necessary, and whether you can articulate the tradeoff against DFS precisely rather than reflexively. A candidate who says "I snapshot `queue.size()` before the inner loop because children get pushed during that loop and I need to know exactly how many nodes belong to *this* level before any of them arrive" is demonstrating real understanding, not memorized code.

Common follow-up questions:
- *"Why not just use `while (!queue.empty())` for the whole traversal without snapshotting per level?"* — expects recognizing that this correctly visits every node (in the right FIFO order) but destroys the ability to know *where one level ends and the next begins*, which matters the moment the problem needs level-grouped output rather than a flat list.
- *"Can you solve minimum depth with DFS instead? What changes?"* — expects naming the real cost: DFS must explore every root-to-leaf path and track a running minimum, losing the early-exit advantage BFS gets for free from its visiting order.
- *"What is the worst-case space complexity, and when is it worst?"* — expects "O(n), when the tree is wide and shallow — the last level of a complete binary tree can hold up to roughly n/2 nodes, all in the queue simultaneously."
- *"How would you adapt this for zigzag order?"* — expects recognizing that the queue-and-snapshot mechanic is unchanged; only the direction values are appended to the result (or a final reversal per odd level) changes.
- *"What if the tree isn't guaranteed to be a tree — could it have cycles?"* — expects immediately naming the need for a `visited` set and pivoting to Graph BFS, and explaining why a genuine tree never needs one.

Common misconceptions:
- "BFS on a tree needs a `visited` set, just like graph BFS." It does not — a tree has exactly one path from the root to any node, so no node can ever be reached (and therefore enqueued) more than once. Adding a `visited` set for a tree is harmless but unnecessary overhead, and reaching for it out of habit is a sign of not having internalized *why* graphs need it (cycles) and trees structurally cannot have that problem.
- "The level-size snapshot is just a minor implementation detail." It is not — it is the entire mechanism that turns "a queue of nodes" into "a queue of levels." Skipping it does not crash; it silently merges levels, which is a correctness bug, not a performance one.
- "BFS is always better than DFS for tree problems." Neither dominates the other — BFS wins for level/minimum-depth questions via early exit and direct level grouping; DFS wins for path/subtree questions via the call stack's natural path-tracking and its typically smaller O(h) space cost.

## Summary

- Tree BFS visits nodes level by level using a queue, in contrast to Tree DFS's path-by-path recursive order.
- The entire mechanism rests on the **level-size snapshot**: record `queue.size()` before the inner loop, so children pushed during that loop are never mistaken for the current level.
- Recognition signal: the problem mentions "level order," "level by level," "minimum depth," or asks for per-level aggregation or same-level connections.
- BFS's visiting order (non-decreasing depth) gives a genuine early-exit advantage for "minimum depth" style questions that DFS structurally cannot match.
- Time is O(n) for every function in this module; space is O(n) worst case for the queue (a wide tree's widest level), contrasted with DFS's O(h) worst-case stack cost.
- The single most common bug is forgetting the level-size snapshot and silently merging two levels together.
- Not the right tool for root-to-leaf path questions or bottom-up subtree properties — that is Tree DFS's job.
- Once the input can have cycles or is not a single-rooted tree, the same queue-based idea needs an explicit `visited` set — that is Graph BFS's job, not Tree BFS's.

## Key Takeaways

1. Tree BFS processes a tree level by level via a queue — the direct opposite emphasis of Tree DFS's path-by-path recursive order.
2. The level-size snapshot (`level_size = queue.size()`, taken before the inner loop) is the entire mechanism — it is what prevents next-level children from being mistaken for the current level.
3. Recognition signal: "level order," "level by level," "minimum depth," per-level aggregation, or connecting same-level siblings.
4. BFS's non-decreasing-depth visiting order gives a genuine early-exit speed advantage for minimum-depth questions that DFS cannot structurally match.
5. Time is O(n); space is O(n) worst case (a wide tree's widest level can hold up to ~n/2 nodes at once) — contrast with DFS's O(h) worst-case stack cost.
6. Always null-check before enqueueing children — pushing `nullptr` crashes (or forces awkward guards) later.
7. A leaf has **zero** children, not "at least one missing child" — get this backward and minimum-depth answers come out too small.
8. A tree never needs a `visited` set (no cycles, one path from root to any node); a general graph always does — that distinction is exactly what separates Tree BFS from Graph BFS.
9. Don't reach for Tree BFS on root-to-leaf path questions or bottom-up subtree properties — that's Tree DFS's shape, not this one.
10. Use a queue (FIFO), never a stack (LIFO) — swapping the two silently destroys level ordering without crashing.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — the standard formal treatment of breadth-first search, including the proof that BFS visits nodes in non-decreasing distance order.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — includes level-order traversal and minimum-depth-style tree problems with interview framing.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes worked binary tree traversal problems, including BFS/level-order variants, with C++-specific implementation notes.
- *Competitive Programmer's Handbook* — Antti Laaksonen — covers BFS's role in shortest-path-style problems on unweighted structures, the same early-exit idea generalized beyond trees.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including binary tree traversal implementations useful for seeing varied implementation styles.
- The C++ Standard Library's `<queue>` header (`std::queue`) — the container this entire module is built on; reading libstdc++'s or libc++'s implementation is a direct look at how a FIFO queue is actually laid out underneath `push`/`pop`/`front`.

**Official Documentation**
- LeetCode — Binary Tree Level Order Traversal (problem 102).
- LeetCode — Binary Tree Zigzag Level Order Traversal (problem 103).
- LeetCode — Minimum Depth of Binary Tree (problem 111).
- LeetCode — Populating Next Right Pointers in Each Node (problem 116).
- cppreference.com — `std::queue` — container adaptor reference, including complexity guarantees for `push`/`pop`/`front`/`size`.

**Blog Articles**
- GeeksforGeeks — "Level Order Traversal" and "Breadth First Search or BFS for a Graph" — widely used explainers covering the general technique and common problem shapes.
- NeetCode — Tree traversal / BFS pattern videos — walks through Level Order Traversal, Zigzag Level Order, and Minimum Depth with visual explanations.
- Educative.io — "Grokking the Coding Interview" Tree BFS pattern chapter — one of the most widely referenced pattern-based framings of this exact technique (the inspiration for organizing DSA study by pattern rather than by individual problem).
