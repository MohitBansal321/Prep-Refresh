# Tree DFS — Trace Diagram (Worked Example)

This traces the recursive call stack descending and returning for **Path Sum** (LeetCode 112) — see [problems/01-path-sum.cpp](../problems/01-path-sum.cpp) — on the tree used throughout this module:

```
              5
        (left)  (right)
           4         8
      (left)      (left) (right)
       11           13      4
   (left)(right)          (right)
     7     2                 1

targetSum = 22
```

```mermaid
sequenceDiagram
    autonumber
    participant Caller
    participant N5 as hasPathSum(5, 22)
    participant N4 as hasPathSum(4, 17)
    participant N11 as hasPathSum(11, 13)
    participant N7 as hasPathSum(7, 2)
    participant N2 as hasPathSum(2, 2)

    Caller->>N5: call, remaining = 22 - 5 = 17
    Note over N5: not a leaf -> recurse left first

    N5->>N4: call, remaining = 17 - 4 = 13
    Note over N4: not a leaf -> recurse left (only child)

    N4->>N11: call, remaining = 13 - 11 = 2
    Note over N11: not a leaf -> recurse left, then right

    N11->>N7: call, remaining = 2 - 7 = -5
    Note over N7: node 7 IS a leaf (both children null)<br/>remaining == -5, NOT 0 -> return false
    N7-->>N11: return false

    Note over N11: left returned false -> try right<br/>(short-circuit did NOT trigger yet)

    N11->>N2: call, remaining = 2 - 2 = 0
    Note over N2: node 2 IS a leaf<br/>remaining == 0 -> MATCH -> return true
    N2-->>N11: return true

    Note over N11: right returned true -> return true<br/>(this is the "||" short-circuit point)
    N11-->>N4: return true
    Note over N4: only child returned true -> return true<br/>(right child does not exist, never called)
    N4-->>N5: return true
    Note over N5: left returned true -> return true<br/>(right subtree, rooted at 8, is NEVER explored)
    N5-->>Caller: return true
```

## How to read it

Each arrow going **down** (`->>`) is one recursive call being made — a new stack frame pushed, carrying the reduced `remaining` value as its parameter, exactly as described in the Execution Flow section of the [README](../README.md). Each arrow going **up** (`-->>`) is that call **returning**, popping its frame off the stack. Notice that the `remaining` value shown at each level is computed once, on the way *down*, and never needs to be recomputed on the way back up — it was carried, not recomputed.

The single most important detail in this trace is what happens **after** node `7` returns `false`: node `11`'s call does **not** give up. It still has an unexplored right child (`2`), so it calls into that branch too — this is the `||` in `hasPathSum(node->left, remaining) || hasPathSum(node->right, remaining)`. Only once `2` returns `true` does the short-circuit actually fire, at which point node `11`, then `4`, then `5` all return `true` in sequence **without ever calling into the right subtree rooted at `8` at all** — the calls to `hasPathSum(8, ...)`, `hasPathSum(13, ...)`, and `hasPathSum(4, ...)` (the second node valued `4`, on the right side) are never made, because C++'s `||` operator short-circuits the moment the left operand is `true`. This is why the diagram only ever shows five participants instead of all nine nodes in the tree — four nodes are never visited at all once the answer is already known.
