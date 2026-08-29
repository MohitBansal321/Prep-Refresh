# Floyd-Warshall — Recognition Diagram

Use this flowchart when you are staring at a new shortest-path or reachability problem and trying to decide whether Floyd-Warshall is the right tool, or whether the problem actually wants Dijkstra, Bellman-Ford, or Union Find instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Do you need distances<br/>or reachability between<br/>EVERY pair of nodes?}
    Q1 -- "No — one fixed source" --> Q1b{Can edge weights<br/>be negative?}
    Q1b -- No --> Dijkstra[["Use Dijkstra —<br/>single source, O((V+E) log V)"]]
    Q1b -- Yes --> BellmanFord[["Use Bellman-Ford —<br/>single source, O(V*E)"]]

    Q1 -- "Yes — genuinely all-pairs" --> Q2{Is the question ONLY<br/>'are these connected at all',<br/>with no actual distance needed?}
    Q2 -- Yes --> UF[["Use Union Find —<br/>connectivity only, ~O(1) per query"]]

    Q2 -- "No — an actual distance,<br/>ratio, or ordered chain matters" --> Q3{Is the graph small<br/>enough that V^3 and V^2<br/>space are comfortable?<br/>(roughly n up to a<br/>few thousand)}
    Q3 -- No --> QV["Reconsider: V runs of Dijkstra<br/>(O(V*(V+E) log V)) or Bellman-Ford<br/>(O(V^2*E)) may be cheaper<br/>on a large, sparse graph"]
    Q3 -- Yes --> FW[["Use FLOYD-WARSHALL<br/>triple loop, k outermost,<br/>O(V^3) time, O(V^2) space"]]

    FW --> Q4{What does the relaxation<br/>need to combine along a path?}
    Q4 -- "Sum of weights" --> Numeric["Standard MIN / + relaxation —<br/>the classic all-pairs distance case"]
    Q4 -- "Does any path exist" --> Bool["OR / AND relaxation —<br/>boolean transitive closure"]
    Q4 -- "Product of ratios" --> Prod["Relaxation via MULTIPLY —<br/>e.g. currency/unit conversion chains"]
```

## How to read it

The **first fork** is the one that decides whether this module even applies: if the question only ever cares about one fixed starting node, running Floyd-Warshall computes far more than was asked and pays `O(V³)` for it — Dijkstra or Bellman-Ford answers the actual question at a lower cost. Only a genuinely all-pairs question (a full distance matrix, "for every node, count how many others satisfy X," a complete reachability table) belongs on this side of the fork.

The **second fork** catches a subtler mismatch: sometimes "are these connected" is asked without any actual distance, cost, or ratio being relevant at all — that is a pure connectivity question, and [Union Find](../../union-find/) answers it far more cheaply than building a full distance matrix ever would, because it never needs to compute an actual shortest path, only whether one exists.

The **third fork** is a genuine engineering judgment call, not a hard rule: `O(V³)` time and `O(V²)` space are both fixed costs depending only on node count. On a graph with a few hundred nodes, this is trivially fast regardless of how many edges exist. On a graph with tens of thousands of nodes or more, the flat `O(V²)` space requirement alone can become impractical well before the `O(V³)` runtime does — at that scale, running a single-source algorithm from every node you actually care about, rather than computing a full matrix nobody will fully use, is usually the better engineering tradeoff.

The **fourth branch**, once you have committed to Floyd-Warshall, is about what the relaxation actually combines — this is where the same triple loop generalizes far beyond "shortest numeric distance": swap `+`/`min` for `AND`/`OR` and you get transitive closure; swap for multiplication and you get ratio-chain resolution (see [problems/03-evaluate-division.cpp](../problems/03-evaluate-division.cpp)). The loop structure and the `k`-outermost requirement never change; only the payload and the combining operation do.
