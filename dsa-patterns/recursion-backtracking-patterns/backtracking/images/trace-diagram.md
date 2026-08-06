# Backtracking — Trace Diagram (Worked Example: N-Queens, n=4)

This traces the actual decision tree explored by [code.cpp](../code.cpp)'s `solveNQueens(4)`, showing one branch that gets **pruned and fully backtracked** (row 0's queen at column 0 leads nowhere) and one branch that **succeeds** (row 0's queen at column 1 leads to a complete solution). Recall `n=4` has exactly 2 solutions total; this trace walks through finding the first one and shows why the column-0 start fails completely.

```mermaid
flowchart TD
    Root(["Row 0: choose a column for the queen<br/>(loop over col = 0, 1, 2, 3)"])

    Root -->|"CHOOSE col=0"| R0C0["Row 0: Q at col 0"]
    Root -->|"CHOOSE col=1"| R0C1["Row 0: Q at col 1"]
    Root -.->|"col=2, col=3 omitted here --<br/>col=2 mirrors col=1's success,<br/>col=3 mirrors col=0's failure"| Dots(("..."))

    R0C0 --> R1Prune["Row 1: col=0 PRUNED same column<br/>Row 1: col=1 PRUNED same diagonal"]
    R1Prune -->|"CHOOSE col=2"| R1C2["Row 1: Q at col 2"]
    R1C2 --> R2DeadA["Row 2: col=1 PRUNED diagonal vs row1<br/>Row 2: col=3 PRUNED diagonal vs row1<br/>NO CANDIDATES LEFT -- dead end"]
    R2DeadA -.->|"UNDO row1=col2, backtrack"| R1After2["Row 1: col 2 retracted"]
    R1After2 -->|"CHOOSE col=3"| R1C3["Row 1: Q at col 3"]
    R1C3 -->|"CHOOSE col=1"| R2C1["Row 2: Q at col 1"]
    R2C1 --> R3Dead["Row 3: only col=2 remains,<br/>PRUNED diagonal vs row2<br/>NO CANDIDATES LEFT -- dead end"]
    R3Dead -.->|"UNDO row2=col1, backtrack"| R2After1["Row 2: col 1 retracted"]
    R2After1 --> R2Dead["Row 2: no candidates left -- dead end"]
    R2Dead -.->|"UNDO row1=col3, backtrack"| R1After3["Row 1: col 3 retracted"]
    R1After3 --> R1Dead["Row 1: no candidates left -- dead end"]
    R1Dead -.->|"UNDO row0=col0, backtrack"| R0Done["Row 0, col=0 branch EXHAUSTED:<br/>zero solutions found under it"]

    R0C1 -->|"CHOOSE col=3"| R1C3b["Row 1: Q at col 3<br/>(col=0,1,2 all pruned)"]
    R1C3b -->|"CHOOSE col=0"| R2C0b["Row 2: Q at col 0<br/>(col=1,2,3 pruned or used)"]
    R2C0b -->|"CHOOSE col=2"| R3C2b["Row 3: Q at col 2<br/>(the only safe remaining column)"]
    R3C2b --> Solution(["row==n: COMPLETE VALID SOLUTION<br/>.Q.. / ...Q / Q... / ..Q.<br/>recorded, then the stack unwinds<br/>with UNDO all the way back to Root"])
```

## How to read it

Follow the **left branch** (row 0's queen at column 0) first: it descends two more levels before hitting a row with **zero safe candidates** (`R2DeadA`), at which point the dotted `UNDO ... backtrack` edges take over -- each one retracts exactly the single choice made at that level and returns control to the loop one level up, which then tries its *next* candidate column rather than giving up entirely. Notice this happens **twice** in sequence (row 1's `col=2` is undone and replaced by `col=3`; then, after that also dead-ends two levels deeper, row 1's `col=3` is undone too) before the algorithm finally concludes the *entire* column-0 start is fruitless and backtracks all the way to row 0. This is the mechanical reality behind "explore, hit a wall, retreat to the last decision point, try the next option" from the maze analogy in the README.

Follow the **right branch** (row 0's queen at column 1) to see the mirror image: every choice happens to be forced (only one safe column remains at each subsequent row), and the recursion reaches `row == n` with a fully valid board -- the base case that records a solution rather than triggering a prune. Even after a successful solution is recorded, the call stack still unwinds through the same `UNDO` steps on the way back to `Root`, because the search must continue to check whether *other* columns in row 0 (`col=2`, `col=3`, both omitted here for space) also lead to valid boards -- backtracking does not stop at the first solution unless the caller explicitly tells it to.

The key visual takeaway: **dashed arrows are always `UNDO`, and they always point back up to a shallower row than the solid arrows that led down into the dead end** -- the tree is walked depth-first, and every dead end is a wasted *sub*-tree, not the whole remaining search, precisely because pruning happens at each row rather than only at the leaves.
