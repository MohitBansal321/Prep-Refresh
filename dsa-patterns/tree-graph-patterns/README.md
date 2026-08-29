# Tree & Graph Patterns

A tree is just a graph with no cycles and exactly one path between any two nodes — so every tree pattern here is really a graph pattern with that extra guarantee removed. The nine patterns cover the two traversal orders (BFS/DFS), the three most common things you do once you can traverse (order nodes by dependency, track connectivity without traversing at all, find the cheapest single path), and the two questions that follow naturally once "cheapest" is on the table: cheapest path when weights can be negative or you need every pair, and cheapest way to connect everything rather than reach any one thing.

| Pattern | Core idea | Used for |
|---------|-----------|----------|
| [Tree BFS](tree-bfs/README.md) | Queue-based level-by-level traversal | Level-order output, minimum depth, level aggregation |
| [Tree DFS](tree-dfs/README.md) | Recursive (or stack-based) depth-first traversal | Root-to-leaf paths, path sums, subtree properties |
| [Graph BFS/DFS](graph-bfs-dfs/README.md) | Same two traversals + an explicit `visited` set | Shortest path (BFS), connectivity/cycle detection (DFS), in a graph that may have cycles |
| [Topological Sort](topological-sort/README.md) | BFS (Kahn's) or DFS-based ordering of a DAG | Scheduling with prerequisites/dependencies |
| [Union Find (Disjoint Set)](union-find/README.md) | Track components without traversing edges at all | Dynamic connectivity, cycle detection as edges are added incrementally |
| [Dijkstra's Algorithm](dijkstras-algorithm/README.md) | Min-heap driven single-source shortest path on weighted edges | Cheapest route, minimum-cost/effort path with non-negative weights |
| [Bellman-Ford](bellman-ford/README.md) | Relax every edge, `V-1` times, tolerating negative weights | Cheapest route when edges can be negative; detecting a reachable negative cycle |
| [Floyd-Warshall](floyd-warshall/README.md) | Triple-loop waypoint relaxation over every pair | All-pairs shortest distances; transitive closure; negative cycle anywhere |
| [Minimum Spanning Tree (Kruskal's/Prim's)](mst-kruskal-prim/README.md) | Cheapest edge that avoids a cycle, added greedily | Cheapest total cost to connect every node (not to reach any one node) |

## How to tell them apart

- **Input is explicitly a tree (one root, no cycles)?** → Tree BFS or Tree DFS, chosen by whether the question is about *levels* (BFS) or *paths/depth* (DFS).
- **Input is a general graph (possibly cyclic) and you need shortest hops or to check reachability, with every edge costing the same?** → Graph BFS/DFS.
- **Same, but edges carry different, non-negative weights/costs, single source?** → Dijkstra's Algorithm.
- **Same, but edges can be negative, single source?** → Bellman-Ford — it also tells you if a negative cycle makes "shortest path" undefined.
- **You need distances between every pair, not just from one source?** → Floyd-Warshall — also tolerates negative weights and finds a negative cycle anywhere in the graph.
- **The graph has directed "must come before" edges and you need a valid order?** → Topological Sort.
- **Edges arrive one at a time and you keep asking "connected?" or "would this create a cycle?"** → Union Find (no traversal needed at all).
- **The question is "cheapest way to connect every node," with no privileged source or destination?** → Minimum Spanning Tree (Kruskal's/Prim's) — this is the one question in this family that shortest-path algorithms do not answer, even though it also involves minimizing a sum of edge weights.

## Recommended study order

1. **Tree BFS** and **Tree DFS** together — master both traversal shapes on the simplest structure first.
2. **Graph BFS/DFS** — the same two traversals, generalized with a `visited` set.
3. **Dijkstra's Algorithm** — swap BFS's FIFO queue for a min-heap the moment edges stop being uniform cost.
4. **Bellman-Ford** — drop Dijkstra's non-negative-weight requirement by relaxing every edge instead of selecting one.
5. **Floyd-Warshall** — generalize Bellman-Ford's single-source relaxation to every pair at once.
6. **Topological Sort** — builds directly on graph BFS (Kahn's algorithm) or DFS (post-order + reverse).
7. **Union Find** — a genuinely different tool (no traversal), best appreciated once you've felt DFS-based cycle detection's cost on repeated queries.
8. **Minimum Spanning Tree (Kruskal's/Prim's)** — reuses Union Find (Kruskal's) and the min-heap frontier shape from Dijkstra (Prim's) directly, applied to a genuinely different question.

All nine are Full-tier modules. See [`../INDEX.md`](../INDEX.md) for exactly what each one has.
