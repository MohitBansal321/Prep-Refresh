# Backtracking — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether Backtracking is the right tool, or whether the problem actually wants plain Subsets-style enumeration or Dynamic Programming instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Does the problem ask you to build up<br/>a solution piece by piece from a set<br/>of choices -- e.g. place items on a<br/>board, fill cells, choose characters/<br/>elements one at a time?}

    Q1 -- No, it just wants a running<br/>calculation over an array/string --> NotThisFamily[["Not this family --<br/>check other pattern families<br/>(Two Pointers, Sliding Window, etc.)"]]

    Q1 -- Yes --> Q2{Is there a CONSTRAINT that can make<br/>a PARTIAL (not yet complete) solution<br/>invalid -- e.g. two queens already<br/>attacking, a Sudoku digit already used<br/>in this row/column/box, a grid cell<br/>already visited on this path?}

    Q2 -- "No -- every combination /<br/>subset / permutation the naive<br/>recursion reaches is valid output" --> Subsets[["Use Subsets<br/>(../../subsets/)<br/>no pruning needed --<br/>enumerate the full tree"]]

    Q2 -- "Yes -- most partial states<br/>are invalid and should be<br/>discarded before going deeper" --> Q3{Do overlapping subproblems repeat --<br/>i.e. would memoizing a (state) key<br/>save real recomputation, and do you<br/>only need an optimal VALUE/count<br/>rather than every actual arrangement?}

    Q3 -- "Yes -- same subproblem state<br/>recurs many times, and only a<br/>best value/count is needed" --> DP[["Use Dynamic Programming<br/>(../../../dynamic-programming-patterns/)<br/>memoize or tabulate instead of<br/>re-deriving arrangements"]]

    Q3 -- "No -- you need every actual<br/>valid ARRANGEMENT (or a single one),<br/>and subproblems do not meaningfully<br/>overlap across branches" --> Backtracking["Use BACKTRACKING<br/>choose -> recurse -> undo,<br/>pruning invalid partial states<br/>as early as possible"]

    Backtracking --> Done([Backtracking applies])
```

## How to read it

The **first fork** is whether the problem is even a decision-tree / choice-by-choice construction problem at all -- if it is really a single linear scan over an array with running state, you are almost certainly looking at a different pattern family entirely (Two Pointers, Sliding Window), and this diagram does not apply.

The **second fork is the one that matters most**: does a **constraint** exist that can invalidate a **partial** (not yet complete) solution? N-Queens has one (two placed queens attacking each other before the board is even full); Sudoku has one (a digit already used in the row/column/box); Word Search has one (revisiting a cell, or the letter not matching); Palindrome Partitioning has one (the candidate substring is not a palindrome). Plain Subsets/Combinations/Permutations problems ("generate every subset of this array") have **no such constraint** -- every leaf of the naive recursion tree is valid output, so there is nothing to prune, and the "pruning" machinery Backtracking is built around would be pure overhead. That is the single sharpest signal distinguishing the two: **if you cannot describe a check that says "this partial state can already never lead to a valid answer," you probably want Subsets, not Backtracking.**

The **third fork** separates Backtracking from Dynamic Programming. Both explore a decision tree, but DP additionally requires **overlapping subproblems** (the same sub-state gets reached via multiple different paths) and typically only needs an optimal **value or count**, not every distinct arrangement. If you find yourself wanting to cache "have I already solved this exact sub-state before?" and reuse the answer -- and you do not need to reconstruct every individual valid arrangement -- that caching opportunity is the signal to reach for DP instead. Backtracking, by contrast, is for when you need the **actual arrangements** (all N-Queens boards, all valid Sudoku fills, all word paths, all palindrome partitions) and the subproblems along different branches do not usefully share cached state. See [../README.md "Similar Patterns"](../README.md#similar-patterns) for the full comparison table.
