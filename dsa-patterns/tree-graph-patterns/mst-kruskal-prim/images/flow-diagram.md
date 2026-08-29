# Minimum Spanning Tree — Flow Diagram

See this diagram for the control flow of both Kruskal's and Prim's, side by side, exactly as implemented in [code.cpp](../code.cpp).

```mermaid
flowchart TD
    subgraph Kruskal["Kruskal's — global, sorted"]
        direction TB
        K1(["Sort ALL edges by weight"]) --> K2["edgesUsed = 0"]
        K2 --> K3{More edges<br/>in sorted order?}
        K3 -- No --> K7([Done])
        K3 -- Yes --> K4{"find(u) == find(v)?<br/>(same component already)"}
        K4 -- Yes --> K5["Skip — would close a cycle"]
        K4 -- No --> K6["unionSets(u, v)<br/>totalWeight += edge.weight<br/>edgesUsed += 1"]
        K5 --> K3
        K6 --> K3b{"edgesUsed == n - 1?"}
        K3b -- Yes --> K7
        K3b -- No --> K3
    end

    subgraph Prim["Prim's — local, frontier-growing"]
        direction TB
        P1(["inTree[0] = true<br/>push all of node 0's edges"]) --> P2{Heap empty, or<br/>edgesUsed == n-1?}
        P2 -- Yes --> P7([Done])
        P2 -- No --> P3["Pop cheapest {weight, from, to}"]
        P3 --> P4{"inTree[to]<br/>already?"}
        P4 -- Yes --> P5["Discard — stale entry"]
        P4 -- No --> P6["inTree[to] = true<br/>totalWeight += weight<br/>push every edge from `to`<br/>to a node NOT yet in tree"]
        P5 --> P2
        P6 --> P2
    end
```

## How to read it

Both halves reach the same guarantee through structurally different bookkeeping. Kruskal's `K4` diamond (`find(u) == find(v)`) is checking a property of the **whole graph considered so far** — has *any* previously-processed edge already connected `u` and `v`, regardless of how far apart they are in the edge list. Prim's `P4` diamond (`inTree[to]`) is checking a property of **one specific tree growing outward from one specific starting node** — has `to` already been absorbed into *this* tree.

The loop termination conditions also differ in a way worth noticing: Kruskal's `K3b` checks `edgesUsed == n - 1` after every successful union, stopping the moment the tree is complete regardless of how many sorted edges remain unexamined. Prim's `P2` checks the same condition, but combines it with "is the heap empty" — a Prim's run on a disconnected graph empties its heap before `edgesUsed` ever reaches `n - 1`, which is exactly how disconnection is detected without any separate check.

Follow one edge through each side to see the contrast directly: in Kruskal's, an edge's position in the sorted list is fixed before the algorithm starts, and it is examined exactly once, regardless of where its endpoints are in the graph. In Prim's, an edge is only ever considered once at least one of its endpoints has already joined the growing tree — an edge between two nodes neither of which is `inTree` yet is never pushed onto the heap at all, until the tree's growth happens to reach one of them.
