# Tree DFS — Flow Diagram

This traces the control flow of the general "descend, then combine on the way back up" shape shared by every Tree DFS problem — the two branches (preorder vs. postorder) diverge only in *when* the current node's value is folded into the answer relative to the two recursive calls. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual node values.

```mermaid
flowchart TD
    Start([Call dfs on a node]) --> Base{node == nullptr?}

    Base -- Yes --> ReturnBase([Return base-case value<br/>e.g. false / 0 / empty path --<br/>this IS the recursion's floor])

    Base -- No --> Shape{Which framing<br/>does this problem need?}

    Shape -- "PREORDER<br/>-- path/sum collection" --> Fold["Fold node->val into the<br/>state carried down<br/>-- append to path, or<br/>update running sum/remainder"]

    Fold --> LeafCheck{Is node a LEAF?<br/>-- BOTH left and right<br/>are null --}

    LeafCheck -- Yes --> Record["Record the answer from<br/>the fully-built state<br/>-- push path into results,<br/>or check remaining == 0"]

    LeafCheck -- No --> RecurseChildrenPre["Recurse into node->left<br/>and node->right, passing<br/>the updated state down"]

    RecurseChildrenPre --> UndoMaybe{Shared mutable state?<br/>-- e.g. one std::vector<br/>reused across calls --}
    UndoMaybe -- Yes --> Undo["Backtrack: undo the fold<br/>-- pop_back / add back --<br/>so the CALLER sees the<br/>state exactly as before"]
    UndoMaybe -- No --> ReturnPre

    Undo --> ReturnPre([Return control to caller])
    Record --> ReturnPre

    Shape -- "POSTORDER<br/>-- subtree aggregation" --> RecurseChildrenPost["Recurse FULLY into<br/>node->left, then node->right<br/>-- both calls must finish<br/>before the next step"]

    RecurseChildrenPost --> Combine["NOW combine: use node->val<br/>PLUS both children's already-<br/>resolved return values to compute<br/>this node's own result"]

    Combine --> UpdateBest["If this problem allows the path<br/>to 'bend' through this node<br/>-- e.g. max path sum, diameter --<br/>update a running best-answer<br/>variable HERE, separately"]

    UpdateBest --> ReturnPost(["Return to parent: ONLY the<br/>one-sided value the parent<br/>is allowed to extend through<br/>-- NOT the same as the running best"])
```

## How to read it

The single most important fork in this diagram is **"which framing does this problem need"** — get that decision right before writing any code, because the two branches genuinely do different things in a different order. In the **preorder** branch (left), the current node's value is folded into the carried-down state *before* recursing, and the payoff happens the moment a leaf is reached — everything needed to record the answer is already sitting in the state parameter. In the **postorder** branch (right), nothing is computed from the current node until *both* recursive calls have fully returned — the node literally cannot know its own answer until its children have told it theirs.

The **"undo maybe" diamond** in the preorder branch is the backtracking discipline this module calls out repeatedly: if the state carried down is a *value copy* (a string built fresh at each call, as in Binary Tree Paths), there is nothing to undo — each call's copy simply goes out of scope. If the state is a *shared, mutable* structure (one vector reused across every call, as in Path Sum II), the fold must be paired with an explicit undo after the recursive calls return, or a sibling branch will silently inherit values left over from an already-finished branch.

The **"update best" box** in the postorder branch is where problems like Maximum Path Sum diverge from something as simple as `maxDepth`: the value returned to the parent (one-sided, extendable) and the value that competes to be the actual answer (possibly bending through both children, not extendable any further) are tracked as two genuinely different things — conflating them is the most common real mistake in this pattern's hardest problems.
