# Graph BFS/DFS — Trace Diagram (Worked Example)

This traces `bfsShortestPath(adj, 0)` from [../code.cpp](../code.cpp) — the same function and the same fixture graph its `main()` asserts against — on a graph that deliberately contains **a cycle**, **a second disconnected component**, and **an isolated node**, so all three of this pattern's characteristic situations show up in one trace.

```
       0 ------- 1                4 ------- 5              6
       |         |
       3 ------- 2                (second component)   (isolated)

    Adjacency list, built with each undirected edge added BOTH ways:
       adj[0] = {1, 3}      adj[3] = {2, 0}
       adj[1] = {0, 2}      adj[4] = {5}
       adj[2] = {1, 3}      adj[5] = {4}
                            adj[6] = {}          (no edges at all)

    Source = 0.  Expected result: dist = [0, 1, 2, 1, -1, -1, -1]
```

```mermaid
sequenceDiagram
    autonumber
    participant Q as Queue (FIFO)
    participant D as dist[] (doubles as visited)
    participant S as Neighbour scan

    Note over D: init: dist = [-1,-1,-1,-1,-1,-1,-1]<br/>every node "unreached"

    D->>D: dist[0] = 0 -- mark the SOURCE visited
    D->>Q: push 0
    Note over Q: queue = [0]

    Q->>S: pop 0  (dist 0)
    S->>D: neighbour 1: dist[1] == -1 -> set dist[1] = 0+1 = 1, push
    S->>D: neighbour 3: dist[3] == -1 -> set dist[3] = 0+1 = 1, push
    Note over Q: queue = [1, 3] -- ring 1 is now fully discovered

    Q->>S: pop 1  (dist 1)
    S->>S: neighbour 0: dist[0] == 0, already set -> SKIP
    S->>D: neighbour 2: dist[2] == -1 -> set dist[2] = 1+1 = 2, push
    Note over Q: queue = [3, 2]

    Q->>S: pop 3  (dist 1)
    S->>S: neighbour 2: dist[2] == 2, already set -> SKIP<br/>*** this is the cycle closing ***
    S->>S: neighbour 0: dist[0] == 0, already set -> SKIP
    Note over Q: queue = [2] -- nothing new; the cycle added no work

    Q->>S: pop 2  (dist 2)
    S->>S: neighbour 1: already set -> SKIP
    S->>S: neighbour 3: already set -> SKIP
    Note over Q: queue = [] -- empty

    Note over D: RESULT: dist = [0, 1, 2, 1, -1, -1, -1]<br/>nodes 4, 5, 6 were NEVER touched --<br/>they are unreachable from source 0
```

## How to read it

Every `->>` into `dist[]` is a **discovery**: a node being seen for the first time, having its distance written, and being pushed onto the queue — all three in one step, never split apart. Every `S->>S` is a neighbour being **skipped** because its distance was already set. Read those two arrow types as the only two things BFS ever does to a neighbour, and the algorithm has no remaining mystery.

**Notice that `dist[]` is doing two jobs.** It holds the answer being computed, and it *is* the visited set: "`dist[v] == -1`" and "`v` has not been visited" are the same test. That is why there is no separate `visited` array in `bfsShortestPath` — a sentinel value in the output array covers both. (Compare [../problems/03-is-graph-bipartite.cpp](../problems/03-is-graph-bipartite.cpp), where the `color` array pulls exactly the same double duty with `-1` meaning "uncoloured, therefore unvisited.")

**The single most important step in this trace is step 11 — node 3 looking at neighbour 2.** This is the cycle `0 → 1 → 2 → 3 → 0` closing. Node 3 sits at distance 1, so it would like to offer node 2 a distance of 2 — which happens to be exactly what node 1 already gave it, so nothing is lost either way. But look at what the guard prevents: without the `dist[2] == -1` check, node 2 would be **pushed a second time**, and when that duplicate was popped it would re-scan its neighbours and re-push node 1 and node 3, which would re-push node 2, and the queue would never empty. The graph is finite; the traversal would not be. That is the precise, mechanical answer to the interview question "walk me through what goes wrong without a visited set on a cyclic graph."

**Notice also what BFS did *not* do: it never revisited a node to improve its distance.** Node 2 was reached at distance 2 via node 1 and never reconsidered, even though node 3 also offered a route. That is safe only because every edge costs exactly 1 hop: the FIFO queue pops nodes in non-decreasing distance order, so the first arrival is always the cheapest and no later arrival can beat it. The instant edges carry different weights, that reasoning collapses — a later, longer-in-hops route can be cheaper in total cost — and the algorithm needs a min-heap that *can* revise a distance downward. That is Dijkstra, and this trace is the exact place where the difference lives.

**Finally, count what the trace never touched: nodes 4, 5, and 6.** BFS from source 0 visited its own component and stopped, which is correct behaviour for a single-source distance query — `dist[4] = dist[5] = dist[6] = -1` truthfully reports "unreachable from 0." But if the question had been "how many components does this graph have," this one traversal would have answered 1 instead of 3. Getting 3 requires the outer loop over every node as a potential unvisited start, which is exactly what `dfsConnectedComponents` in [../code.cpp](../code.cpp) wraps around its traversal and what [../problems/02-number-of-provinces.cpp](../problems/02-number-of-provinces.cpp) is built around. The traversal is the same; the loop around it is what changes the question being answered.
