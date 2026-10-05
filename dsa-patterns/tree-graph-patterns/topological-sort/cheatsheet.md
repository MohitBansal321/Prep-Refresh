# Topological Sort — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — linearise a **directed acyclic graph** so that every edge `u → v` places `u` before `v`. |
| **Recognition Signal** | The words **prerequisite, dependency, "must come before", build order, course schedule**. A directed graph where you need a valid *sequence*, not a path — and, crucially, where the answer might be "impossible" because the constraints are circular. |
| **Problem** | Constraints are given pairwise ("u before v") but you need one global order satisfying all of them at once. Repeatedly scanning for something with no unmet prerequisite is O(V²); worse, a **cycle** makes the task impossible and you must detect that rather than loop forever or emit a plausible but invalid order. |
| **Solution** | **Kahn's algorithm** (BFS-flavoured): compute `inDegree[v]` = number of unmet prerequisites. Seed a queue with every node of in-degree **0** — these depend on nothing. Pop one, append it to the order, and decrement the in-degree of each dependent; any that reaches 0 has all prerequisites satisfied, so enqueue it. **Cycle detection is free**: if the emitted order is shorter than `numNodes`, the leftover nodes are precisely those trapped in a cycle. (The DFS alternative: post-order finish times, reversed.) |
| **Time / Space Complexity** | **O(V + E)** time — building the adjacency list and in-degrees is O(E), and across the whole run each node is enqueued once and each edge decremented exactly once. **O(V + E)** space. |
| **Pros** | Linear time, single pass · **cycle detection comes free** from a length comparison — no separate colouring pass · Kahn's is iterative, so no stack-overflow risk on deep dependency chains · swapping the queue for a min-heap yields the lexicographically smallest valid order at O((V+E) log V) · directly models real build systems and schedulers. |
| **Cons** | The order is **not unique** — any node with in-degree 0 may go next, so tests that assert one exact sequence are brittle and should verify the *constraints* instead · only defined on a DAG; on a cyclic graph there is no answer and the caller must handle that case · the edge direction convention ("u → v means u before v") is easy to invert, producing a perfectly reversed order that passes casual inspection · decrementing in-degree more than once per edge silently corrupts the result. |
| **Use When** | Build systems and task schedulers (make, Bazel, CI DAGs) · course prerequisite / "can I finish all courses" problems · package/module dependency resolution · spreadsheet formula recalculation order · deadlock and circular-import detection · linear-time DAG shortest paths (relax in topological order — works with negative weights). |
| **Avoid When** | The graph is undirected (the notion does not apply) · the graph has cycles and you actually need to handle them (condense strongly connected components first — Tarjan/Kosaraju) · you need a *shortest path*, not an ordering · there are no ordering constraints to satisfy. |
| **Related Patterns** | Graph BFS (Kahn's is BFS with an in-degree gate) · Graph DFS (the post-order alternative, and the three-colour cycle detector this replaces) · Dijkstra (shortest paths when the graph is not a DAG or weights demand a heap). |

### Template Skeleton

```cpp
struct TopoResult {
    std::vector<int> order;   // a valid ordering; INCOMPLETE (short) if hasCycle
    bool hasCycle;
};

// edges: {u, v} means "u must come before v" — v depends on u.
TopoResult topologicalSort(int numNodes,
                           const std::vector<std::pair<int,int>>& edges) {
    std::vector<std::vector<int>> adj(numNodes);   // adj[u] = everything depending on u
    std::vector<int> inDegree(numNodes, 0);        // unmet prerequisites per node

    for (const auto& [u, v] : edges) {
        adj[u].push_back(v);
        ++inDegree[v];                             // v gains one prerequisite
    }

    std::queue<int> q;
    for (int i = 0; i < numNodes; ++i)
        if (inDegree[i] == 0) q.push(i);           // seed: depends on nothing
                                                   // (ALL of them, not just node 0)
    std::vector<int> order;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);                        // every prerequisite of u is placed

        for (int v : adj[u])
            if (--inDegree[v] == 0)                // decrement ONCE per edge
                q.push(v);                         // v is now unblocked
    }

    // Cycle detection, free of charge: nodes inside a cycle never reach in-degree 0,
    // so they are never enqueued and the order comes up short.
    return {order, (int)order.size() != numNodes};
}
```

### Remember In One Sentence
> **Topological sort emits nodes whose prerequisites are all satisfied — seed a queue with every in-degree-0 node, and after placing one, decrement its dependents' in-degrees and enqueue any that hit zero — and if the final order is shorter than the node count, the missing nodes form a cycle.**

### Two Facts People Get Wrong
- The topological order is unique, so a test can assert one exact sequence? **No** — whenever two or more nodes have in-degree 0 simultaneously, either may come next, so a DAG generally has many valid orders. Verify the *property* ("for every edge u→v, u appears before v") rather than a specific permutation; that is exactly what a `respectsAllEdges`-style checker is for.
- Cycle detection needs a separate DFS colouring pass? **No** — it falls out of Kahn's for free. A node inside a cycle always has at least one prerequisite still unplaced, so its in-degree never reaches 0 and it is never enqueued. Comparing `order.size()` to `numNodes` is the entire check.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the defining property of a valid topological order in one sentence.
2. What does `inDegree[v]` count, physically, in dependency terms?
3. Which nodes seed the queue, and why is it *all* of them rather than just one?
4. Walk through what happens when a node is popped: what two things does the algorithm do?
5. How is a cycle detected, and why does that check work? What is true of a node inside a cycle?
6. Why is the topological order not unique? Give the exact condition under which a choice exists.
7. Given that, how should a test verify the output correctly?
8. Justify the O(V + E) bound — where does each term come from?
9. What single change produces the lexicographically smallest valid order, and what does it cost?
10. Name two real systems that run this algorithm, and say what the nodes and edges represent in each.
