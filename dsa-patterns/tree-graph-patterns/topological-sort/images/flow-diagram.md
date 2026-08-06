# Topological Sort — Flow Diagram (Kahn's Algorithm)

This traces the control flow of Kahn's algorithm — the BFS-based approach this module builds around. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual in-degree values and queue contents.

```mermaid
flowchart TD
    Start([Start: numNodes, directed edges u -> v<br/>meaning 'u must come before v']) --> BuildAdj["Build adjacency list adj[u]<br/>= every v such that u -> v exists"]

    BuildAdj --> BuildDegree["Compute inDegree[v] for every node:<br/>count incoming edges<br/>(one pass over all edges)"]

    BuildDegree --> Seed["Seed queue with every node<br/>whose inDegree == 0<br/>(no prerequisites at all)"]

    Seed --> Init["Initialize empty output order list"]

    Init --> Loop{Queue empty?}

    Loop -- No --> Pop["Pop node 'current' from<br/>the front of the queue"]
    Pop --> Emit["Append 'current' to the output order"]
    Emit --> ForEach["For each neighbor v in adj[current]:<br/>decrement inDegree[v] by 1"]
    ForEach --> CheckZero{"inDegree[v] just<br/>became 0?"}
    CheckZero -- Yes --> Enqueue["Push v onto the queue<br/>(v has no unmet prerequisites left)"]
    CheckZero -- No --> Loop
    Enqueue --> Loop

    Loop -- Yes, queue is empty --> LengthCheck{"order.size()<br/>== numNodes?"}

    LengthCheck -- Yes --> Valid([Return: order is a valid<br/>topological order])
    LengthCheck -- No --> Cycle([Return: hasCycle = true —<br/>a cycle exists, no valid<br/>order for the whole graph])
```

## How to read it

The two setup boxes (`BuildAdj`, `BuildDegree`) are a single `O(E)` pass each over the edge list — they exist purely to answer, in `O(1)` time per lookup during the main loop, "who depends on this node?" and "does this node currently have zero unmet prerequisites?" Without them you would have to rescan the entire edge list on every step, turning a linear algorithm into a quadratic one.

The main loop (`Loop` through `Enqueue`) is where the actual ordering happens: every iteration pops exactly one node, emits it, and walks its (already-built) adjacency list to decrement neighbors' in-degrees, pushing any neighbor that just reached zero. This is precisely BFS, except the "has this node been visited" check that ordinary Graph BFS uses is replaced here by "has this node's in-degree reached zero" — the frontier is defined by satisfied dependencies, not by proximity to a start node.

The final diamond, `LengthCheck`, is the single most important box in the whole diagram and the one most often forgotten in a hastily-written implementation: comparing the final `order.size()` to `numNodes` is the *entire* cycle-detection mechanism. If every node was eventually emitted, the graph was a DAG and `order` is a valid answer. If the queue ran dry while nodes remain un-emitted, those remaining nodes were stuck — each waiting on a prerequisite that was itself (directly or transitively) waiting on it — which is only possible if a cycle exists among them.
