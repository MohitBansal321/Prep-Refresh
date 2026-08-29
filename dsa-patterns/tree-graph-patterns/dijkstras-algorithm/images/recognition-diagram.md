# Dijkstra's Algorithm — Recognition Diagram

Use this flowchart when you are staring at a new shortest-path problem and trying to decide whether Dijkstra is the right tool, or whether the problem actually wants BFS, [Bellman-Ford](../../bellman-ford/), [Floyd-Warshall](../../floyd-warshall/), or an [MST algorithm](../../mst-kruskal-prim/) instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Is it a graph or<br/>implicit graph problem asking for<br/>a minimum-cost path?}
    Q1 -- No --> NotSP[["This is not a shortest-path<br/>problem — look at other patterns"]]

    Q1 -- Yes --> Q2{Are ALL edge weights<br/>equal (or every move<br/>costs exactly 1)?}
    Q2 -- "Yes — 'fewest hops/moves/steps'" --> BFS[["Use plain Graph BFS<br/>O(V + E), no heap needed"]]
    Q2 -- "No — weights differ" --> Q3{Can any edge weight<br/>be NEGATIVE?}

    Q3 -- "Yes (prices as refunds,<br/>differences, credits)" --> BellmanFord[["Use Bellman-Ford<br/>see ../../bellman-ford/<br/>O(V * E); also detects<br/>negative cycles"]]
    Q3 -- "No — all non-negative" --> Q4{Which pairs do you<br/>need distances between?}

    Q4 -- "One source to all nodes<br/>(or one target: early-exit on pop)" --> Dijkstra[["Use DIJKSTRA<br/>min-heap keyed on tentative distance<br/>O((V + E) log V)"]]
    Q4 -- "Every pair, and the graph<br/>is small/dense (n <= ~500)" --> FW[["Use Floyd-Warshall<br/>see ../../floyd-warshall/<br/>O(V^3) all-pairs in one pass,<br/>simpler than V runs of Dijkstra"]]
    Q4 -- "Graph is a DAG" --> DAG[["Use one topological-order pass<br/>relaxing edges in order<br/>O(V + E), no heap"]]

    Dijkstra --> Q5{Any extra per-path constraint,<br/>e.g. 'at most K stops/edges'?}
    Q5 -- "Yes" --> Augmented["Augment the state:<br/>run Dijkstra over (node, budgetUsed)<br/>NOT over bare nodes"]
    Q5 -- "No" --> Done([Plain Dijkstra applies])

    Start -.-> SideQ{Is it really about connecting<br/>all nodes cheaply, not path costs?}
    SideQ -- "Yes — 'minimize total wiring/<br/>connect everything'" --> MST[["Use MST (Kruskal / Prim)<br/>see ../../mst-kruskal-prim/<br/>a tree of edges, not a path —<br/>different question entirely"]]
```

## How to read it

Start at the top and answer each diamond honestly before moving on — the most common mistake is jumping straight to "shortest path means Dijkstra" without checking the weight structure. The **first real fork** is uniformity of weights: if every edge costs 1, BFS's FIFO queue already explores in cheapest-first order for free, and adding a heap only buys you an extra `log` factor. The **second fork** is sign: Dijkstra's correctness argument ("the closest unfinalized node can never get cheaper later") collapses the moment a negative edge exists, so negative weights route to [Bellman-Ford](../../bellman-ford/) — see that module's own recognition diagram for how it further distinguishes itself from Floyd-Warshall and a DAG's topological-order shortcut.

The **third fork** is which pairs you need. Single source → Dijkstra; all pairs on a small dense graph → [Floyd-Warshall](../../floyd-warshall/), because running Dijkstra V times is both more code and worse asymptotics there. And if you reached the Dijkstra exit but the problem attaches a *budget* (at most K stops, at most K edges), drop down to the augmented-state branch before writing any code — finalizing a node on first pop is wrong under a hop limit, which is exactly why [problems/03-cheapest-flights-within-k-stops.cpp](../problems/03-cheapest-flights-within-k-stops.cpp) runs Dijkstra over `(node, stopsUsed)` states rather than bare nodes.

Finally, watch the dotted side-question: "minimum total cost to connect everything" is an [**MST**](../../mst-kruskal-prim/) question (Prim's/Kruskal's produce a tree spanning all nodes), not a shortest-path question — confusing the two is one of the most common pattern mix-ups in interviews. For the sibling relationship with unweighted traversal, see this module's [README](../README.md) and the family overview [../../README.md](../../README.md).
