# Bitmask DP — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether Bitmask DP is the right tool, or whether the problem actually wants plain DP, backtracking, or a greedy instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{How many items / nodes /<br/>choices are there in total?}

    Q1 -- "n > ~24" --> TooBig[["Bitmask DP is hopeless:<br/>2^n states explode.<br/>Use greedy heuristic, graph-specific<br/>algorithm, or meet-in-the-middle"]]

    Q1 -- "n <= ~20-24" --> Q2{Does the future depend on<br/>WHICH items were used so far —<br/>or only on HOW MANY / running total?}

    Q2 -- "Only count/total matters" --> PlainDP[["Use plain DP<br/>(0/1 Knapsack style: dp[i][capacity],<br/>O(n * capacity), no mask needed)"]]

    Q2 -- "Exact identity of used set matters" --> Q3{What shape is the problem?}

    Q3 -- "Assign items to groups / partition<br/>into k subsets with a property<br/>e.g. equal sums, min max load" --> Assign["Bitmask DP fits:<br/>dp over masks of already-placed items"]

    Q3 -- "Permutation / ordering / visiting<br/>every node exactly once<br/>e.g. TSP, Hamiltonian paths" --> Perm["Bitmask DP with dp[mask][last]:<br/>mask = visited set, last = current node"]

    Q3 -- "Count arrangements satisfying<br/>per-position divisibility or<br/>compatibility rules" --> Count["Bitmask DP counting:<br/>position = popcount(mask),<br/>try each unused item at that position"]

    Q3 -- "Two-player turn game over a<br/>finite pool of numbered choices,<br/>players remove without replacement" --> Game["Bitmask DP game theory:<br/>mask = remaining pool,<br/>memoize win/loss per mask"]

    Q3 -- "Just enumerate ALL subsets/<br/>permutations, no optimization or<br/>counting to memoize" --> Backtrack[["Plain backtracking is enough —<br/>a memo table would add memory<br/>for states you never revisit twice"]]

    Assign --> Done([Bitmask DP applies])
    Perm --> Done
    Count --> Done
    Game --> Done
```

## How to read it

Start at the top and answer each diamond honestly before moving on — the most common mistake is jumping to "small n, use bitmask" before checking whether subset identity is genuinely needed. The **first fork** is size: `2^n` states means `n = 20` is already a million-entry table per extra dimension, and `n = 30` is a billion — if `n` exceeds roughly 24, no amount of clever bit manipulation saves you, and you need a different technique entirely.

The **second fork** is the one that separates this pattern from its closest sibling: if two different choices that have consumed the same *number* of items always have interchangeable futures, then a mask adds exponential blowup for zero information — plain 0/1 Knapsack-style DP (see the [0-1-knapsack module](../0-1-knapsack/)) is smaller and simpler. Bitmask DP earns its cost only when the exact identity of the chosen set changes what transitions remain possible. Ask yourself concretely: "could I shuffle which specific items were picked earlier and still get the same optimal continuation?" If yes, no mask needed.

The **third fork** picks the state shape among the four common flavors (assignment/partition, permutation with `last`, position-counting, game pools). Note the backtracking exit: if the task is literally to enumerate all solutions rather than compute one optimal value or a count, memoization buys nothing — each state's subtree is explored exactly once anyway, so a memo table only wastes memory. For problems where several signals overlap (BFS over `(node, mask)` states, greedy pruning inside a masked search), re-check the Similar Patterns section of the [README](../README.md) and the worked examples under [problems/](problems/) before committing to a shape.
