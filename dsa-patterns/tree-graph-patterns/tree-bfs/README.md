# Tree BFS

> **In one line:** snapshot `q.size()` *before* consuming the queue — that count is exactly the current level, because children pushed during the loop are invisible to it.

```cpp
std::vector<std::vector<int>> levelOrder(TreeNode* root) {
  std::vector<std::vector<int>> result;
  if (root == nullptr) return result;

  std::queue<TreeNode*> q;
  q.push(root);                          // the queue now holds exactly level 0

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
    result.push_back(std::move(level_values));   // one commit per level
  }
  return result;
}
```

**O(n)** time · **O(w)** space (w = widest level). Full runnable version, with `minDepth` and the edge cases: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → the [worked trace](#watch-it-run) below it → run [code.cpp](code.cpp) yourself → [problems/](problems/). Start here, not with the prose. |
| **40 min** — want the *why* | read on ↓ for the reasoning, tradeoffs and interview framing, then [exercises.md](exercises.md) |

---

## Intent

Visit every node of a tree **level by level** — all nodes at depth 0, then all at depth 1, then all at depth 2 — using a queue, whenever a problem's answer depends on grouping nodes by their distance from the root rather than on the paths between them.

## Real Life Analogy

Picture an announcement spreading through a company's **org chart**, one reporting level at a time. The CEO tells their direct reports on Monday morning. Each of those tells *their* direct reports on Monday afternoon. Each of those tells *their* reports on Tuesday morning. Nobody two levels below the CEO hears it before everybody one level below does — the announcement moves outward in synchronized "rounds," and everyone in the same round hears it at the same time, regardless of which branch they sit in.

Now suppose you want to answer "how many people are in the third round?" or "what is the fewest rounds before someone in Sales hears it?" Those are trivial if you tracked the announcement round by round. They are awkward if you only tracked "who told whom" as a tangle of individual conversations — you would have to reconstruct the rounds afterward by computing everyone's distance from the CEO.

Tree BFS is that round-by-round announcement in code: a queue holds "everyone in the current round," and the algorithm empties that exact set before moving to the next.

> **Term: BFS (Breadth-First Search).** A traversal that visits all nodes at the current distance from the start before visiting any node farther away. "Breadth" refers to spreading sideways across a level before going deeper — the opposite emphasis of depth-first search, which dives down one path as far as it can before backtracking.

## Problem

A large class of tree problems is naturally phrased in terms of **levels** (depth from the root), not **paths** (root-to-node sequences):

- **"Return the tree's values grouped by level."** (`[[3], [9, 20], [15, 7]]`, not a flat list.)
- **"What is the minimum depth to reach a leaf?"** — inherently level-by-level, because "minimum" means "the first level at which this becomes true."
- **"What is the maximum width of the tree?"** — the most nodes that ever exist in one level.
- **"Connect each node to its right-hand neighbour at the same level."** — impossible without knowing precisely which nodes share a level.
- **"Return the average / sum of each level's values."** — level aggregation by definition.

The recursive traversal every engineer learns first — Tree DFS — visits nodes in **path order**: root, then root's *entire* left subtree however deep, then its *entire* right subtree. A DFS call at depth 5 on the left runs long before a DFS call at depth 2 on the right. It does not group nodes by level at all.

Three things make that hard to fix by hand:

- **Recursion models "go deep," not "go wide."** The call stack tracks *how you got here* (the path), not *how far you have spread out* (the level). There is no equally natural recursive phrasing of "finish this depth before touching the next."
- **You need an explicit, mutable frontier.** Processing "the current level" as a genuine batch requires a structure holding exactly those nodes, separate from the nodes that will form the next level. A recursive call has only "the current node" and "the path so far."
- **Level boundaries are easy to lose.** Once a queue holds a mix of nodes, it is easy to consume a few next-level nodes as if they belonged to the current one — silently corrupting the grouping rather than crashing.

## Solution

Hold a **queue** containing exactly the nodes of the level being processed. Before enqueuing any children — who belong to the *next* level — count and process every node that was in the queue for *this* one.

1. If the root is `nullptr`, there are no levels. Return empty.
2. Push the root. The queue now holds exactly level 0.
3. While the queue is not empty:
   - Snapshot `level_size = q.size()`. Every node of the current level was pushed during the *previous* iteration, and nothing has been added since — so this count *is* the level's size, and it will not change as children get pushed.
   - Repeat exactly `level_size` times: dequeue the front node, do the problem's per-node work, then push its non-null children.
   - Everything now left in the queue is the next level in full, with nothing from the level after it (grandchildren have not been pushed yet).
4. When the queue empties, every level has been processed. Assemble the result.

The trick is not the queue — a queue is a queue. It is reading the queue's size **before** consuming it, rather than checking `q.empty()` node-by-node with no regard for which level a node belongs to. That is what turns *a queue of nodes* into *a queue of levels*.

### Watch it run

On the tree `3` at the root, `9` and `20` as its children, `15` and `7` under `20`:

| Round | `level_size` | Dequeued | Children pushed | Queue after | `result` |
|---|---|---|---|---|---|
| init | — | — | `3` | `[3]` | `[]` |
| 1 | **1** | `3` | `9`, `20` | `[9, 20]` | `[[3]]` |
| 2 | **2** | `9`, `20` | `15`, `7` | `[15, 7]` | `[[3], [9, 20]]` |
| 3 | **2** | `15`, `7` | none | `[]` | `[[3], [9, 20], [15, 7]]` |

Round 2 is the one to stare at. The loop dequeues `9`, then `20` — and pushing `20`'s children puts `15` and `7` into the queue *while that same loop is still running*. But `level_size` was snapshotted at **2** before any of that, so the loop stops after two iterations and leaves the grandchildren for the next round.

Break the snapshot and the levels shift silently — no crash, no exception, just a wrong answer. These are the actual outputs on this same tree:

| What you wrote for the inner loop | Output |
|---|---|
| `for (i = 0; i < level_size; ++i)` — snapshotted (correct) | `[[3], [9, 20], [15, 7]]` |
| `for (i = 0; i < q.size(); ++i)` — re-reads the growing size | `[[3, 9], [20, 15], [7]]` |
| `while (!q.empty())` — no boundary at all | `[[3, 9, 20, 15, 7]]` |

Note that the middle one does not merge everything into one level — it *shifts* the boundaries, which is harder to spot than the third and just as wrong. That is why this bug survives a casual eyeball test.

For the full trace, including `minDepth`, see [images/trace-diagram.md](images/trace-diagram.md).

## Architecture

- **The queue.** Holds nodes awaiting a visit, always in strict level order. Its FIFO discipline is what preserves "shallower before deeper, left-to-right within a level" — a stack (LIFO) would not.
- **The level-size snapshot.** Recorded once per level. It enforces the level boundary by telling the inner loop "process exactly this many," even as the queue's actual size grows underneath it.
- **The current node.** One unit of work within a level: read its value, test whether it is a leaf, connect it to a sibling — then push its children for the next level's snapshot.
- **The result accumulator.** Updated once per **level** (after the inner loop), not once per node — because one level's worth of work is this pattern's natural unit of progress.

## Why Not Other Approaches?

**"Use Tree DFS with a `depth` parameter, then bucket by depth."** This works, and for pure depth questions ("return the deepest leaves") it can be the more natural choice: carry `depth + 1` downward and push each value into `buckets[depth]`. The output is equivalent to `levelOrder`'s. But it is strictly more indirect. You need an accumulator that persists *across* recursive calls, and you compute "which level is this node in" by counting stack depth rather than reading it off the traversal. Critically, **the natural unit of work — a level — never exists as a concrete collection during a DFS walk.** You synthesize it afterward from scattered calls. For problems where you want to *act* on a level's nodes together (aggregate them, connect them, reverse them), that reconstruction is the whole cost.

**"Flatten to a list of `(node, depth)` pairs, then group."** The same approach wearing different clothes, plus an explicit grouping pass (a `map<int, vector<int>>`, or a sort by depth) after the traversal.

**"Plain DFS, tracking the minimum depth seen at any leaf."** Correct, but wasteful for *minimum* depth specifically. DFS must in the worst case visit every node before it can be sure, because nothing in its order favours shallow leaves — it might fully explore a 50-node-deep path before finding the 2-node-deep one that was the answer. BFS returns the moment it sees the first leaf.

**Net:** DFS-with-depth is not *wrong* — same O(n) time, and preferable for path and subtree questions (see [../tree-dfs/](../tree-dfs/)). But when the natural unit of work is "a level, processed together," Tree BFS hands you that unit directly with no reconstruction step, plus a genuine early exit that DFS structurally cannot offer.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart distinguishing Tree BFS from Tree DFS and Graph BFS/DFS, based on signals in the problem statement.
- [images/flow-diagram.md](images/flow-diagram.md) — control flow of the level-by-level loop, showing exactly where the snapshot is taken relative to the inner loop.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of the queue's contents at every level, on the same tree [code.cpp](code.cpp) uses (`3` at the root, `9` and `20` as children, `15` and `7` under `20`).

## The Code

[code.cpp](code.cpp) is a **generic, problem-agnostic template** rather than one LeetCode answer — the goal is to see the mechanic clearly before the problem-specific solutions in [problems/](problems/). It provides two functions on the same `TreeNode` shape LeetCode itself uses, so they transfer onto real problem statements with no adaptation:

- **`levelOrder`** — the canonical routine, shown in full at the top of this file. Collects each level into its own `vector<int>`. Children are pushed *after* the snapshot is taken, so they are invisible to the current level's inner loop and only appear when the outer loop re-reads `q.size()`.
- **`minDepth`** — the identical skeleton. Only the line inside the inner loop changes:

  ```cpp
  for (size_t i = 0; i < level_size; ++i) {
    TreeNode* node = q.front();
    q.pop();
    if (node->left == nullptr && node->right == nullptr) return depth;  // BOTH null = leaf
    if (node->left  != nullptr) q.push(node->left);
    if (node->right != nullptr) q.push(node->right);
  }
  ++depth;                                   // a whole level survived with no leaf
  ```

  Since BFS dequeues in non-decreasing depth order, the first leaf found is provably at the minimum depth — so the `return` is safe the instant it fires, with most of the tree never visited. Note the `&&`: a node missing only *one* child is not a leaf, and writing `||` here is the bug that makes minimum-depth answers come out too small.

That both functions share one skeleton is the point: the level-size snapshot is the pattern, not the per-node logic.

Supporting pieces: `buildSampleTree` constructs the one small tree used by every demo and by this module's diagrams, so you can follow a single example from prose to diagram to running code. `freeTree` is a post-order walk that `delete`s every node — C++ has no garbage collector, so every `new` here is paired with a teardown. `main()` exercises both functions against the sample tree plus edge cases (empty tree, single node, a one-sided chain to specifically test the "one missing child is not a leaf" rule), printing `[PASS]`/`[FAIL]`.

Each file in [problems/](problems/) defines its own `TreeNode` rather than including `code.cpp`, so it compiles and reads independently — see [problems/README.md](problems/README.md). Briefly: `01` is pure level-order collection; `02` adds direction-alternation to the same skeleton; `03` is the early-exit minimum depth; `04` uses the snapshot to link siblings via a `next` pointer instead of collecting values.

## Tradeoffs

**What the queue buys you**

- **Levels are a first-class unit.** No reconstruction step to learn which nodes belong together — the snapshot hands you that set every iteration.
- **A real early exit on "minimum" questions.** Non-decreasing depth order means you can return the instant the first qualifying node is dequeued, never touching deeper parts of the tree that could not hold a shallower answer.
- **Composability.** Zigzag order, next-right pointers, per-level sums/averages/max-width all slot into the same skeleton by changing only what happens *inside* the inner loop.
- **Iterative control flow.** No recursion, no call stack to reason about, no stack-depth limit on a very deep narrow tree — the queue lives on the heap.

**What it costs you**

- **O(n) space, regardless of shape.** A wide shallow tree (a nearly-complete binary tree) can hold roughly `n / 2` nodes in its last level, all in the queue at once. DFS's stack is O(h) and *varies* with shape — for a balanced tree that is O(log n).
- **Path questions get awkward.** "Every root-to-leaf path," "does any path sum to a target," "lowest common ancestor" — all are naturally a single path built up and unwound as recursion descends and returns, which is exactly what DFS's call stack gives free. A queue holds isolated nodes, not paths, so BFS means carrying path state alongside each queued node.
- **No output until a level completes.** DFS can emit a result the moment it hits a base case; a level-order result is only complete once every node in that level has been processed.
- **One more piece of state.** DFS piggybacks on the language's own call stack; BFS makes you manage an explicit queue and get its termination right.

Against DFS-with-a-depth-parameter specifically — the alternative that *also* produces level-grouped output — the gain is narrower but real: the level exists as a concrete, iterable collection at every step, instead of being an emergent property you bucket after the fact.

## Complexity

**Time: O(n).** Every node is enqueued once and dequeued once; each dequeue does O(1) work. True for every function in this module, early exit or not.

**Space: O(n) worst case** — the widest level can hold ~`n / 2` nodes simultaneously. This is the sharpest contrast with Tree DFS, whose stack costs O(h): for a balanced tree that is O(log n), dramatically less; for a chain-like tree `h = O(n)` and the two converge. The distinction that matters is that **BFS pays O(n) regardless of shape, while DFS's O(h) genuinely varies with it.**

| Operation | Time | Space |
|---|---|---|
| `levelOrder` | O(n) | O(n) worst case (widest level) |
| `minDepth` (BFS) | O(n) worst case; best case returns after level 1 | O(n) worst case |
| `minDepth` (DFS, for contrast) | O(n) always — no early exit | O(h) — height, not node count |

That best case is worth naming: if a leaf sits directly under the root, BFS finds it on the first level and returns without touching most of the tree. The DFS solution has no equivalent — it must explore every root-to-leaf path, always.

## Common Mistakes

- **Not snapshotting the size, and silently merging levels.** Writing `for (int i = 0; i < q.size(); ++i)` re-evaluates `q.size()` every iteration — and since children are pushed *during* that loop, the bound keeps growing and the loop consumes next-level nodes as if they were current. *Fix:* capture the size into a named local once, before the inner loop, and loop by that fixed count. This does not crash; it produces a wrong grouping, which is why it survives casual testing.
- **Not null-checking children before enqueuing.** Pushing `node->left` / `node->right` unconditionally puts `nullptr` in the queue, which crashes when later dereferenced. *Fix:* guard each push, exactly as [code.cpp](code.cpp) does.
- **Off-by-one on depth.** Starting `depth` at `0` instead of `1` shifts every depth answer by one — reporting a single-node tree's minimum depth as `0` rather than the expected `1`. *Fix:* pin the convention up front ("a single node has depth 1," matching LeetCode) and trace a one-node and a two-level example by hand.
- **Treating "one missing child" as a leaf.** A leaf has **zero** children. Returning on `node->left == nullptr || node->right == nullptr` (should be `&&`) gives a too-small minimum depth whenever some node has exactly one child. *Fix:* both children absent, and test it with a one-sided chain.
- **Using a stack instead of a queue.** LIFO still visits every node, but not in level order — silently invalidating any level-dependent output. If you are reaching for a stack, you have drifted into Tree DFS, which is a different pattern with a different signal.

## When To Use

- The problem asks for **level-order output** — values grouped by depth.
- It asks for **minimum depth**, shortest path in an unweighted structure, or any "first level at which X becomes true" question, where the early exit is a genuine win.
- It needs **per-level aggregation** — sums, averages, maximums, counts, computed independently per level.
- It needs to **connect or compare nodes sharing a level** — next-right pointers, zigzag order, maximum width, "is this level a palindrome."
- You need an **iterative** traversal, to avoid stack depth concerns on a very deep tree or because the surrounding code is already iterative.

## When NOT To Use

- **Root-to-leaf path questions** — all paths, path sums, longest/shortest path between two nodes, lowest common ancestor. That is [Tree DFS](../tree-dfs/).
- **Bottom-up subtree properties** — "is this balanced," "what is the diameter." These want post-order recursion, where a parent's answer depends on its children's already-computed answers. BFS's top-down order fits the wrong direction.
- **The input is a general graph**, not a tree — cycles, disconnected components, no single root. That needs a `visited` set, which is [Graph BFS/DFS](../graph-bfs-dfs/)'s job.
- **Memory is tight and the tree is wide** — the O(n) queue is a real cost; DFS's O(h) stack may be preferable if the tree is also reasonably balanced.

## Where This Shows Up

Level-order traversal is one of the most frequently asked tree patterns in interviews — Binary Tree Level Order Traversal, Minimum Depth, Zigzag Level Order, and Populating Next Right Pointers all recur at major companies, precisely because they test whether you reach for an explicit queue (and get the snapshot right) instead of defaulting to recursion out of habit.

In production systems:

- **Hierarchical rollouts and notifications.** Broadcasting an announcement, a permission change, or a config flag down an org chart or tenant hierarchy one level at a time — the analogy this README opened with, implemented literally. Doing it level by level is what lets you pause between batches and roll back before a change reaches more customers.
- **Serialization by level.** Some tree serialization formats and hierarchical database exports (nested-set, adjacency-list) emit level by level so a deserializer can rebuild top-down, allocating each level before it needs to know anything about the next.
- **Lazily-loaded tree UIs.** A file explorer, a nested category tree, or a comment thread that renders top-level items first and loads replies only as the user expands them — mirroring BFS's frontier rather than eagerly recursing to full depth.
- **Blast-radius analysis.** Which services are affected by a change, grouped by how many hops away they are from the changed one — a direct level-order collection over a dependency tree.
- **Nearest-match search in a hierarchy.** The nearest ancestor category carrying a discount rule, searched outward from a leaf — `minDepth`'s early exit pointed at "nearest match" instead of "nearest leaf."
- **Per-level monitoring statistics.** Fan-out counts per level of an org chart, or average queue depth per level of a job-dependency tree — level-order aggregation.

## Similar Patterns

| Pattern | Structure | Traversal order | Needs `visited`? | Primary question |
|---|---|---|---|---|
| Tree BFS | Tree (no cycles, one root) | Level by level (queue, FIFO) | No — a tree has no cycles | "Level-order output? Minimum depth? Per-level aggregate?" |
| [Tree DFS](../tree-dfs/) | Tree (no cycles, one root) | Path by path (recursion / stack, LIFO) | No — a tree has no cycles | "Root-to-leaf paths? Path sums? Subtree properties?" |
| [Graph BFS/DFS](../graph-bfs-dfs/) | General graph (cycles, possibly disconnected) | Either shape, over reachable nodes | **Yes** — required to avoid infinite loops | "Shortest path (BFS)? Connectivity or cycles (DFS)?" |

The row that matters most is the third. Graph BFS/DFS is the *same two traversal shapes* generalized to structures that may have cycles, multiple components, and no obvious root. Tree BFS never needs a `visited` set because a tree has exactly one path from root to any node — no node can be reached, and re-enqueued, twice. Both traversals are O(n) time; the space costs differ structurally, as above.

## Interview Discussion

Experienced interviewers do not spend time on "can you write a queue loop" — that is mechanical. What they probe is whether you know **why** the snapshot is necessary and can state the DFS tradeoff precisely rather than reflexively. "I snapshot `q.size()` before the inner loop because children get pushed during that loop, and I need to know how many nodes belong to *this* level before any of them arrive" demonstrates understanding; reciting the code does not.

Follow-ups worth rehearsing:

- *"Why not just `while (!q.empty())` for the whole traversal, with no per-level snapshot?"* — It visits every node in the right order, but destroys any knowledge of where one level ends and the next begins. Fine for a flat list; useless the moment the problem wants level-grouped output.
- *"Can you solve minimum depth with DFS? What changes?"* — You can, and you lose the early exit: DFS must explore every root-to-leaf path and track a running minimum.
- *"How would you adapt this for zigzag order?"* — The queue-and-snapshot mechanic is unchanged; only the direction values are appended (or a per-odd-level reversal) changes. This is the question that reveals whether you see the skeleton as reusable.
- *"What if the input might have cycles?"* — Name the `visited` set immediately and pivot to Graph BFS, and be able to say why a genuine tree never needs one.

Two misconceptions worth killing early:

- **"BFS on a tree needs a `visited` set, like graph BFS."** It does not. Reaching for one out of habit signals you have not internalized *why* graphs need it (cycles) and trees structurally cannot have that problem.
- **"BFS is generally better than DFS for tree problems."** Neither dominates. BFS wins on level and minimum-depth questions; DFS wins on path and subtree questions, and usually costs less space.

## Key Takeaways

1. Tree BFS processes a tree level by level via a queue — the opposite emphasis of Tree DFS's path-by-path recursion.
2. The **level-size snapshot** (`level_size = q.size()`, taken before the inner loop) is the entire mechanism. It is what stops next-level children being mistaken for the current level.
3. Recognition signal: "level order," "level by level," "minimum depth," per-level aggregation, or connecting same-level siblings.
4. BFS's non-decreasing-depth order gives a genuine early exit on minimum-depth questions that DFS cannot structurally match.
5. Time is O(n). Space is O(n) worst case, *regardless of tree shape* — contrast DFS's O(h), which varies with shape.
6. The most common bug is omitting the snapshot: it does not crash, it silently merges two levels. A correctness bug, not a performance one.
7. A leaf has **zero** children — not "at least one missing child." Get this backwards and minimum-depth answers come out too small.
8. Always null-check before enqueuing children, and use a queue (FIFO) — a stack silently destroys level ordering without crashing.
9. A tree never needs a `visited` set; a general graph always does. That distinction is exactly what separates Tree BFS from Graph BFS.
10. Not the tool for root-to-leaf paths or bottom-up subtree properties — that is Tree DFS's shape.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — the standard formal treatment of BFS, including the proof that it visits nodes in non-decreasing distance order.
- *Elements of Programming Interviews in C++* — worked binary tree traversal problems with C++-specific implementation notes.
- *Competitive Programmer's Handbook* (Antti Laaksonen) — BFS's role in shortest-path problems on unweighted structures, the same early-exit idea generalized beyond trees.

**Reference**
- cppreference.com — `std::queue`, including complexity guarantees for `push` / `pop` / `front` / `size`.
- LeetCode — Level Order Traversal (102), Zigzag Level Order (103), Minimum Depth (111), Populating Next Right Pointers (116, 117).

**Explainers**
- NeetCode — tree BFS pattern videos, walking through Level Order, Zigzag, and Minimum Depth visually.
- Educative — "Grokking the Coding Interview" Tree BFS chapter, the most widely referenced framing of this technique, and the inspiration for organizing study by pattern rather than by individual problem.
