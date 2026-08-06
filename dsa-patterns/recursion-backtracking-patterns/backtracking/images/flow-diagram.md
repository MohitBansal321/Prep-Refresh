# Backtracking — Flow Diagram

This traces the control flow of the general backtracking loop -- the shape behind N-Queens, Sudoku, Word Search, and Palindrome Partitioning alike. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual queen placements.

```mermaid
flowchart TD
    Start([Enter decision slot: e.g. "this row",<br/>"this empty cell", "this grid position",<br/>"the next partition piece"]) --> Base{Is the partial solution<br/>already COMPLETE?<br/>e.g. all rows filled,<br/>whole string consumed}

    Base -- Yes --> Record["Record this partial state<br/>as one full valid solution<br/>(e.g. push a copy onto the results list)"]
    Record --> Return1([Return to caller])

    Base -- No --> Candidates{More candidates left<br/>to try in this slot?<br/>e.g. next column,<br/>next digit, next neighbor,<br/>next substring end}

    Candidates -- No, all candidates<br/>exhausted --> DeadEnd([Return to caller:<br/>this branch is a dead end])

    Candidates -- Yes --> Constraint{Does this candidate satisfy<br/>the constraint given the<br/>CURRENT partial state?<br/>e.g. no shared column/diagonal,<br/>no row/col/box conflict,<br/>cell not yet used, substring<br/>is a palindrome}

    Constraint -- "No -- PRUNE" --> NextCandidate["Move to the next candidate<br/>WITHOUT recursing --<br/>this is the entire complexity<br/>win over brute force"]
    NextCandidate --> Candidates

    Constraint -- Yes --> Choose["1) CHOOSE:<br/>commit to this candidate,<br/>mutate the shared partial state"]
    Choose --> Recurse["2) RECURSE:<br/>enter the next decision slot<br/>with the updated partial state"]
    Recurse --> Undo["3) UNDO:<br/>revert the mutation made in CHOOSE,<br/>regardless of what the recursive<br/>call returned"]
    Undo --> Candidates
```

## How to read it

The two diamonds after `Base` are where all the leverage lives. `Candidates` simply asks "is there anything left to try here," but `Constraint` is the diamond that makes Backtracking different from brute-force generate-then-filter: it is evaluated **before** recursing, using only the partial state built so far, and a "no" answer skips the recursive call entirely (`NextCandidate`, straight back to `Candidates`). Brute force would instead recurse anyway, build the candidate out to full depth, and only discover the same conflict once it reached a complete (invalid) leaf -- wasting every bit of work below the point where the constraint was already violated.

The `Choose -> Recurse -> Undo` chain is the three-step skeleton named throughout this module. `Choose` mutates state that is **shared** across the whole search (a board array, a "used" grid, a running partition list) -- and precisely because it is shared, `Undo` must run **unconditionally** after `Recurse` returns, whether that recursive call found a solution or hit a dead end. Skipping `Undo` does not crash the program; it silently leaves the shared state mutated for every sibling candidate still to be tried in the current `Candidates` loop, which is the single most common bug in backtracking code (see the README's "Common Mistakes"). Note also that `Undo` runs on the way back UP the call stack -- the diagram's single loop back to `Candidates` after `Undo` is what turns a straight-line recursive descent into an actual **search over the whole tree** rather than a single fixed path.
