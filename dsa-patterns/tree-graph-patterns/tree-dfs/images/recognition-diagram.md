# Tree DFS — Recognition Diagram

Use this flowchart when you are staring at a new tree problem and trying to decide whether Tree DFS is the right tool, or whether the problem actually wants Tree BFS or Backtracking instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Is the input a tree<br/>you were GIVEN<br/>-- one root, no cycles --<br/>or a decision tree YOU<br/>construct as you search?}

    Q1 -- "I construct the tree myself<br/>-- e.g. all subsets, all<br/>permutations, N-Queens<br/>placements" --> Backtracking[["Use Backtracking<br/>(recurse into each choice,<br/>undo/backtrack, prune early<br/>when a branch is invalid)"]]

    Q1 -- "I was given a tree<br/>as input data" --> Q2{Does the question mention<br/>'level', 'level order', or<br/>'minimum depth'?}

    Q2 -- Yes --> TreeBFS[["Use Tree BFS<br/>(queue, level by level;<br/>can short-circuit on<br/>MINIMUM depth/distance)"]]

    Q2 -- No --> Q3{Does the question need<br/>root-to-leaf PATHS or a<br/>running PATH SUM?}

    Q3 -- "Yes -- collect/check paths,<br/>path sums, path existence" --> Preorder["Use Tree DFS<br/>(PREORDER, carry-state-down)<br/>process node, then recurse,<br/>passing path/sum as a parameter"]

    Q3 -- "No" --> Q4{Does the answer for a node<br/>depend on the FULLY-RESOLVED<br/>answer of BOTH its children?<br/>-- height, balance, diameter,<br/>max path sum, subtree validity}

    Q4 -- Yes --> Postorder["Use Tree DFS<br/>(POSTORDER, combine-on-the-way-up)<br/>recurse into both children fully,<br/>THEN combine via return value"]

    Q4 -- "No -- I just need sorted-order<br/>output from a BST" --> Inorder["Use Tree DFS<br/>(INORDER)<br/>recurse left, process node,<br/>recurse right"]

    Preorder --> Done([Tree DFS applies])
    Postorder --> Done
    Inorder --> Done
```

## How to read it

Start at the top and answer each diamond honestly. The **first real fork** is not about the tree traversal at all — it is about **what is being traversed**. If you are enumerating all subsets, permutations, or valid placements (N-Queens, Sudoku), you are not walking a tree that exists as input; you are *building* a decision tree of choices as you go, and pruning invalid branches early is central to the technique — that is Backtracking's territory, not this module's, even though the recursive shape (recurse, then undo) looks nearly identical.

If you were genuinely handed a tree as input, the **second fork** is "level" versus "path/depth." Any mention of level-order output, per-level aggregation, or **minimum** depth/distance is Tree BFS's signal — BFS reaches shallower nodes first, so it can stop the moment it finds the first (shallowest) match, something Tree DFS cannot do without exploring a potentially deep, irrelevant branch first.

The **third and fourth forks** separate the two Tree DFS flavors from each other: a question about root-to-leaf paths or a running sum wants **preorder** (carry state down as you descend, use it once you reach a leaf); a question about a value that depends on both children's fully-resolved answers (height, balance, diameter, "does the path bend through this node") wants **postorder** (recurse fully first, combine on the way back up via the return value). Inorder is called out separately because its headline use — visiting a binary search tree's nodes in sorted order — is narrower and less common outside BST-specific problems; if you find yourself reaching for it elsewhere, double check that sorted-order visitation is really what is needed.
