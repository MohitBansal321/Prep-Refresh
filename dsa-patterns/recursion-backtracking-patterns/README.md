# Recursion & Backtracking Patterns

Both patterns explore a decision tree of choices, but they answer different questions: **Subsets** wants to *enumerate everything* (there's no way to prune, every leaf is valid output); **Backtracking** wants to *find valid arrangements under constraints* (most of the tree is invalid and should be pruned as early as possible).

| Pattern | Core idea | Used for |
|---------|-----------|----------|
| [Subsets](subsets/README.md) | Double the set of built-so-far results for each new element (or recurse include/exclude) | All subsets, combinations, permutations |
| [Backtracking](backtracking/README.md) | Choose → recurse → **undo the choice** the moment it can't lead anywhere valid | N-Queens, Sudoku, word search, constrained combinations |

## How to tell them apart

- **"Return all possible subsets/combinations/permutations" with no extra constraint to check?** → Subsets.
- **There's a constraint that can invalidate a partial solution early (a queen attacking another, a Sudoku conflict, a used cell in a grid)?** → Backtracking, so you can prune before wasting time on doomed branches.

In practice Subsets is often the *base case* — Backtracking adds a validity check and an explicit "undo" step on top of the same recursive skeleton.

## Recommended study order

1. **Subsets** — learn the include/exclude recursion tree (or iterative doubling) with no pruning first.
2. **Backtracking** — add the constraint check and the undo step to the same tree shape.

Both are built: **Backtracking** is a Full-tier module; **Subsets** is Compact-tier (README + code.cpp only). See [`../INDEX.md`](../INDEX.md) for details.
