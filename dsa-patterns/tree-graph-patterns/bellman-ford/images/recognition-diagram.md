# Bellman-Ford — Recognition Diagram

Use this flowchart when you are staring at a new shortest-path problem and trying to decide whether Bellman-Ford is the right tool, or whether the problem actually wants Dijkstra, Floyd-Warshall, or a topological-order relaxation instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Is it a shortest-path /<br/>minimum-cost-to-reach<br/>problem?}
    Q1 -- No --> NotSP[["Not a shortest-path problem<br/>— look at other patterns"]]

    Q1 -- Yes --> Q2{Can any edge weight<br/>be NEGATIVE?}
    Q2 -- No --> Dijkstra[["Use Dijkstra<br/>O((V+E) log V), faster —<br/>no reason to pay Bellman-Ford's cost"]]

    Q2 -- "Yes, or you need to detect<br/>a negative cycle" --> Q3{Is the graph a DAG<br/>(no cycles at all)?}
    Q3 -- Yes --> Topo[["One topological-order<br/>relaxation pass, O(V+E) —<br/>no repeated passes needed"]]

    Q3 -- "No — cycles are possible" --> Q4{Which pairs do you<br/>need distances between?}
    Q4 -- "One source to all nodes" --> BellmanFord[["Use BELLMAN-FORD<br/>relax every edge, V-1 times<br/>O(V*E)"]]
    Q4 -- "Every pair" --> FW[["Use Floyd-Warshall<br/>O(V^3) all-pairs,<br/>also tolerates negative weights"]]

    BellmanFord --> Q5{Does the problem give an<br/>explicit resource limit —<br/>at most K edges, a time<br/>budget, a discount count?}
    Q5 -- "Yes" --> Bounded["Bound the pass count to that<br/>resource instead of V-1 —<br/>e.g. K+1 passes for 'at most K stops'"]
    Q5 -- "No" --> Plain([Run the full V-1 passes,<br/>plus one more for cycle detection])
```

## How to read it

Start at the top and answer each diamond honestly. The **first fork** is whether this is a shortest-path question at all — a surprising number of graph problems that mention "cost" are actually MST or connectivity questions in disguise, which is a different family entirely (see the sibling recognition diagrams for [Dijkstra](../../dijkstras-algorithm/images/recognition-diagram.md) and [MST](../../mst-kruskal-prim/images/recognition-diagram.md)).

The **second fork** is the one that actually decides whether you need this module at all: if every edge is verifiably non-negative, Dijkstra is strictly faster with no correctness tradeoff, and there is no reason to reach for Bellman-Ford. Only a genuine possibility of a negative edge — or a question that is explicitly about detecting a negative cycle — routes you here.

The **third fork** is worth checking before committing to the repeated-passes approach: if the graph is guaranteed acyclic (a DAG), a single topological-order relaxation pass is both correct and asymptotically better than Bellman-Ford's `V - 1` passes, precisely because a DAG's structure already guarantees you process every predecessor before its successors — no repetition needed to account for "what if a later pass changes an earlier answer," because there is no way to loop back and revisit a node.

The **fourth fork** — single-source vs. all-pairs — is the same choice Dijkstra's own recognition diagram makes, except here both branches tolerate negative weights, unlike Dijkstra which requires the all-pairs branch to still avoid them.

Finally, watch the fifth diamond: this is Bellman-Ford's most commonly tested real-world shape on LeetCode. The moment a problem hands you an explicit constraint — "at most K stops," "within T minutes," "using at most D discounts" — that constraint almost always *is* the correct number of relaxation passes, tighter than the general-purpose `V - 1` bound. Missing this fork is what leads to correct-looking code that quietly ignores a constraint the problem actually requires you to respect.
