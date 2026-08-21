# Tree BFS — Trace Diagram (Worked Example)

This traces the queue's exact contents, level by level, for `levelOrder` in [code.cpp](../code.cpp) — see also [problems/01-binary-tree-level-order-traversal.cpp](../problems/01-binary-tree-level-order-traversal.cpp) — on the tree used throughout this module (the same tree `buildSampleTree()` constructs):

```
              3
        (left)  (right)
           9         20
                (left) (right)
                  15      7

Expected output: [[3], [9, 20], [15, 7]]
```

Node `9` is a leaf. Node `20` has both children. That asymmetry is deliberate: it is what makes level 2 contain two nodes rather than four, and it is what lets the same tree also demonstrate `minDepth`'s early exit at the bottom of this page.

```mermaid
sequenceDiagram
    autonumber
    participant Q as queue (front on the left)
    participant Loop as outer loop (one iteration = one level)
    participant Out as result

    Note over Q: initial: push root -> [3]

    Loop->>Q: LEVEL 0 -- snapshot level_size = 1
    Q-->>Loop: pop 3
    Note over Loop: record 3 -> level_values = [3]
    Loop->>Q: push 9, push 20
    Note over Q: queue is now [9, 20]<br/>(i = 1 of 1 done, inner loop exits)
    Loop->>Out: commit [3]

    Loop->>Q: LEVEL 1 -- snapshot level_size = 2
    Q-->>Loop: pop 9
    Note over Loop: record 9 -> level_values = [9]
    Note over Loop: 9 has NO children -> nothing pushed
    Q-->>Loop: pop 20
    Note over Loop: record 20 -> level_values = [9, 20]
    Loop->>Q: push 15, push 7
    Note over Q: queue is now [15, 7]<br/>(i = 2 of 2 done, inner loop exits)
    Loop->>Out: commit [9, 20]

    Loop->>Q: LEVEL 2 -- snapshot level_size = 2
    Q-->>Loop: pop 15
    Note over Loop: record 15; 15 is a leaf -> nothing pushed
    Q-->>Loop: pop 7
    Note over Loop: record 7; 7 is a leaf -> nothing pushed
    Note over Q: queue is now [] -- EMPTY<br/>(i = 2 of 2 done, inner loop exits)
    Loop->>Out: commit [15, 7]

    Loop->>Q: outer loop re-checks: queue is empty
    Note over Out: return [[3], [9, 20], [15, 7]]
```

## How to read it

Read the `Note over Q` lines as the ground truth: they are the queue's literal contents at that instant, front on the left. Every `Loop->>Q` arrow is the algorithm reading or writing the queue; every `Q-->>Loop` arrow is one `q.front()` plus `q.pop()`.

**Follow the snapshot at each level boundary and watch what it protects you from.** At level 0 the snapshot is `1`, so the inner loop runs exactly once. During that single iteration, `9` and `20` are pushed — so by the time the iteration ends, the queue's *actual* size is `2`, not the `1` that was snapshotted. This is the exact moment the bug would happen. A loop written as `for (size_t i = 0; i < q.size(); ++i)` would re-read `q.size()`, now see `2`, and keep going: it would pop `9` and `20` in the same iteration batch as `3` and record `level_values = [3, 9, 20]`, producing `[[3, 9, 20], [15, 7]]` — levels 0 and 1 silently fused. Nothing throws; the output is just wrong. Because `level_size` was copied into a named local *before* the pushes, the loop stops at one node and the two children sit untouched in the queue until the outer loop's next iteration re-reads the size.

**Level 1 is where the tree's asymmetry does its teaching work.** The snapshot is `2`, and the two nodes are processed in strict left-to-right order — `9` first, `20` second — because that is the order in which their parent pushed them, and FIFO preserves it. `9` contributes nothing to the queue (it is a leaf, so both null-checks fail and nothing is pushed), while `20` contributes two nodes. This is why level 2 has two members and not four: the queue's contents at any level boundary are exactly "the children that actually exist," never a fixed power of two. It is also why the null-check before each push matters — pushing `9`'s absent children as `nullptr` would put two poison entries in the queue that crash on dereference two lines later.

**The final iteration is the termination proof.** At level 2 both nodes are leaves, so the inner loop pops both and pushes nothing. The queue drains to empty for the first time, and the outer `while (!q.empty())` condition finally fails. Notice that this happens *naturally* — there is no depth counter to compare against, no "am I at the last level" test anywhere in the code. The loop terminates because the deepest level is by definition the one that produces no children, and each node is pushed exactly once and popped exactly once, which is also the whole argument for the O(n) time bound.

**Peak queue size is the space cost, and you can read it straight off the trace.** The largest the queue ever gets here is `2` — the width of the widest level. On a complete binary tree the widest level is the last one, holding roughly `n / 2` nodes, all resident simultaneously; that is where Tree BFS's O(n) worst-case space comes from, in direct contrast to Tree DFS's O(h) call stack (see [../../tree-dfs/](../../tree-dfs/), whose own trace diagram shows the mirror-image picture: a stack of frames as deep as the tree is tall, but never more than one root-to-leaf path wide). Compare the two traces side by side and the tradeoff stops being a memorized table row: BFS holds a level's *breadth* in memory, DFS holds a path's *depth*.

## The same tree, traced for `minDepth` — where BFS stops early

`minDepth` ([code.cpp](../code.cpp), and [problems/03-minimum-depth-of-binary-tree.cpp](../problems/03-minimum-depth-of-binary-tree.cpp)) runs the identical skeleton on the identical tree, but returns on the first leaf instead of collecting values:

| Step | Queue before | Action | Result |
|---|---|---|---|
| Level 1, snapshot = 1 | `[3]` | pop `3`; not a leaf (has both children); push `9`, `20` | continue, `depth` becomes 2 |
| Level 2, snapshot = 2 | `[9, 20]` | pop `9`; **both children null → leaf** | **return 2 immediately** |

Nodes `20`, `15`, and `7` are never dequeued at all — `20` was pushed but is abandoned in the queue, and `15` and `7` are never even pushed. That is the early exit: three of five nodes untouched. The return is safe not because `9` looked special, but because of the queue's ordering property — BFS dequeues in non-decreasing depth order, so no leaf shallower than `9` can still be waiting anywhere. A Tree DFS solution to the same question would have had to walk `20`'s entire subtree before it could be sure `9`'s depth of 2 was the minimum, because nothing in DFS's visiting order favors shallow leaves over deep ones.
