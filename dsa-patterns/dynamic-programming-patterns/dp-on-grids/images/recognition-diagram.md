# DP on Grids — Recognition Diagram

Use this flowchart when you are staring at a new grid problem and trying to decide whether grid DP is the right tool, or whether the problem actually wants BFS/DFS, Dijkstra, or a greedy pass instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Is the input a 2D grid<br/>and are you asked to COUNT paths<br/>or find a MIN/MAX-cost path<br/>through it?}

    Q1 -- "No, it's a different question<br/>(subregions, islands, matching...)" --> NotThis[["Not grid DP.<br/>Consider other patterns:<br/>BFS/DFS for connectivity,<br/>prefix sums for range queries"]]

    Q1 -- Yes --> Q2{Is movement restricted to<br/>MONOTONE directions only —<br/>e.g. right/down, so you can never<br/>step back up or left?}

    Q2 -- "No: all 4 directions allowed,<br/>or cells can be revisited" --> Q3{What does 'best path' mean here?}
    Q3 -- "Fewest steps, uniform cost" --> BFS[["Use BFS<br/>(queue + visited set;<br/>first time you reach a cell<br/>is via a shortest path)"]]
    Q3 -- "Weighted cells/edges" --> Dijkstra[["Use Dijkstra<br/>(priority queue instead of queue)"]]
    Q3 -- "Reachability / exploration only" --> DFS[["Use DFS<br/>(stack or recursion)"]]

    Q2 -- Yes, monotone --> Q4{Counting paths,<br/>or optimizing cost/value?}
    Q4 -- "Counting distinct paths" --> Count["Grid DP (COUNTING)<br/>dp[r][c] = dp[r-1][c] + dp[r][c-1]<br/>obstacle cell -> 0"]
    Q4 -- "Min/max cost or value along a path" --> Cost["Grid DP (COST)<br/>dp[r][c] = best(dp[r-1][c], dp[r][c-1])<br/>+ value[r][c]<br/>obstacle cell -> INF"]

    Q4 -- "One locally-best choice is provably<br/>globally best (e.g. all costs equal,<br/>any monotone path has same length)" --> GreedyCheck{Really no<br/>tradeoff between<br/>early and late choices?}
    GreedyCheck -- No, early cheap choice<br/>can force expensive late ones --> Cost
    GreedyCheck -- Yes, provably independent --> Greedy[["A greedy pass suffices<br/>(rare on grids — be suspicious)"]]

    Count --> Done([Grid DP applies])
    Cost --> Done
```

## How to read it

Start at the top and answer each diamond honestly before moving on. The **first fork** is whether the question is even about paths through the grid at all — many grid problems (largest island, number of islands, flood fill) are connectivity questions and belong to BFS/DFS, not this pattern. The **second fork is the decisive one**: monotone movement. Grid DP's entire correctness argument rests on being able to order the cells so that every dependency is computed before its consumer; right/down-only movement gives you that order for free (row by row, left to right). The moment upward or leftward steps are legal, a cell's answer may depend on cells *below or to its right* — the table becomes unfillable in one pass, and you have left DP's jurisdiction. That is the exit toward BFS (unweighted shortest path), Dijkstra (weighted), or plain DFS (reachability).

The **third fork** picks the recurrence flavor: counting problems sum the two predecessors (and zero out obstacles); cost problems take the min/max of the two predecessors plus the current cell's own weight (and poison obstacles with infinity). The greedy exit exists because interviewees occasionally reach for greedy on grids ("always step onto the cheaper neighbor") — on real grids an early cheap step routinely forces an expensive corridor later, which is precisely the tradeoff dynamic programming exists to handle, so treat that exit as rare and requiring proof.

For the boundary between this pattern and LCS-style sequence tables — same neighbor-reading mechanics, different underlying shape — see the Similar Patterns section of this module's [README](../README.md).
