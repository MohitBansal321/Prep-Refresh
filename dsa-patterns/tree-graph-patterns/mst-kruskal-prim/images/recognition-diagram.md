# Minimum Spanning Tree — Recognition Diagram

Use this flowchart when you are staring at a new graph problem and trying to decide whether it is an MST question at all, or whether it is secretly a shortest-path question, a pure-connectivity question, or something else entirely.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Does the question mention<br/>a specific source and/or<br/>destination node?}
    Q1 -- "Yes — 'from A to B',<br/>'reach node X'" --> SP[["This is shortest path, not MST —<br/>see Dijkstra / Bellman-Ford instead"]]

    Q1 -- "No — no privileged node,<br/>the question is about<br/>connecting a whole set" --> Q2{Does an actual edge<br/>weight/cost matter,<br/>or only 'connected or not'?}
    Q2 -- "Only connectivity,<br/>no cost to minimize" --> UF[["Use plain Union Find or<br/>Graph BFS/DFS — no MST needed"]]

    Q2 -- "Yes — minimize total<br/>cost to connect everything" --> Q3{Is the graph handed to<br/>you as edges, or is it<br/>implicit (e.g. points<br/>with a distance formula)?}
    Q3 -- "Implicit — build the<br/>edge list yourself first" --> Build["Generate every relevant<br/>pairwise edge, THEN proceed"]
    Q3 -- "Explicit edge list<br/>or adjacency list" --> Q4

    Build --> Q4{Sparse graph with a flat<br/>edge list, or dense graph<br/>with an adjacency list<br/>already built?}
    Q4 -- "Sparse / flat edge list" --> Kruskal[["Use KRUSKAL'S —<br/>sort edges + Union Find,<br/>O(E log E)"]]
    Q4 -- "Dense / adjacency list" --> Prim[["Use PRIM'S —<br/>grow one tree + min-heap,<br/>O(E log V)"]]

    Kruskal -.-> Q5{Does a cost come from TWO<br/>different sources — e.g. a<br/>per-node cost AND a<br/>per-edge cost?}
    Prim -.-> Q5
    Q5 -- Yes --> Virtual["Add a VIRTUAL node representing<br/>the per-node cost's source,<br/>turning it into an ordinary edge"]
    Q5 -- No --> Done([Run the algorithm directly])
```

## How to read it

The **first fork** is the single most important discriminator in this whole family of patterns, and the one most often gotten wrong under time pressure: any mention of a specific starting or ending node means the question is about *reaching* something, which is shortest path's job, not MST's. An MST has no notion of "from" or "to" at all — it minimizes the cost to connect the *entire* set, symmetrically.

The **second fork** catches the case where cost was never actually part of the question — "can these all be connected" or "how many separate groups are there" is pure connectivity, answered directly and more cheaply by [Union Find](../../union-find/) or a plain traversal, without ever needing to sort edges or grow a weighted tree.

The **third fork** is a practical implementation step, not a conceptual one: many real MST problems (LeetCode 1584 among them) hand you points or objects, not edges — recognizing that "every pair is implicitly connectable, weighted by some formula" is itself the graph-construction step that must happen before any MST algorithm can run.

The **fourth fork** — Kruskal's vs. Prim's — is an engineering choice about representation and density, not a correctness question; both provably reach the same minimum total weight.

Finally, the dotted side-question is worth checking on any MST problem that feels like it has "two kinds of cost" mixed together (see [problems/03-optimize-water-distribution-in-a-village.cpp](../problems/03-optimize-water-distribution-in-a-village.cpp)): a per-node cost (like a well) can almost always be folded into an ordinary edge by inventing one virtual node representing whatever that per-node cost connects to.
