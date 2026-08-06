# Tree & Graph Patterns

A tree is just a graph with no cycles and exactly one path between any two nodes — so every tree pattern here is really a graph pattern with that extra guarantee removed. The five patterns cover the two traversal orders (BFS/DFS) and the three most common things you do once you can traverse: order nodes by dependency, or track connectivity without traversing at all.

| Pattern | Core idea | Used for |
|---------|-----------|----------|
| [Tree BFS](tree-bfs/README.md) | Queue-based level-by-level traversal | Level-order output, minimum depth, level aggregation |
| [Tree DFS](tree-dfs/README.md) | Recursive (or stack-based) depth-first traversal | Root-to-leaf paths, path sums, subtree properties |
| [Graph BFS/DFS](graph-bfs-dfs/README.md) | Same two traversals + an explicit `visited` set | Shortest path (BFS), connectivity/cycle detection (DFS), in a graph that may have cycles |
| [Topological Sort](topological-sort/README.md) | BFS (Kahn's) or DFS-based ordering of a DAG | Scheduling with prerequisites/dependencies |
| [Union Find (Disjoint Set)](union-find/README.md) | Track components without traversing edges at all | Dynamic connectivity, cycle detection as edges are added incrementally |
| [Dijkstra's Algorithm](dijkstras-algorithm/README.md) | Min-heap driven single-source shortest path on weighted edges | Cheapest route, minimum-cost/effort path with non-negative weights |

## How to tell them apart

- **Input is explicitly a tree (one root, no cycles)?** → Tree BFS or Tree DFS, chosen by whether the question is about *levels* (BFS) or *paths/depth* (DFS).
- **Input is a general graph (possibly cyclic) and you need shortest hops or to check reachability, with every edge costing the same?** → Graph BFS/DFS.
- **Same, but edges carry different (non-negative) weights/costs?** → Dijkstra's Algorithm — BFS's ring-order guarantee breaks the moment edges aren't uniform cost.
- **The graph has directed "must come before" edges and you need a valid order?** → Topological Sort.
- **Edges arrive one at a time and you keep asking "connected?" or "would this create a cycle?"** → Union Find (no traversal needed at all).

## Recommended study order

1. **Tree BFS** and **Tree DFS** together — master both traversal shapes on the simplest structure first.
2. **Graph BFS/DFS** — the same two traversals, generalized with a `visited` set.
3. **Dijkstra's Algorithm** — swap BFS's FIFO queue for a min-heap the moment edges stop being uniform cost.
4. **Topological Sort** — builds directly on graph BFS (Kahn's algorithm) or DFS (post-order + reverse).
5. **Union Find** — a genuinely different tool (no traversal), best appreciated once you've felt DFS-based cycle detection's cost on repeated queries.

Tree DFS and Union Find are Full-tier modules; Tree BFS, Graph BFS/DFS, and Topological Sort are Partial-tier (README + code + some but not all of exercises/cheatsheet/diagrams/problems); Dijkstra's Algorithm is Compact-tier (README + code.cpp only). See [`../INDEX.md`](../INDEX.md) for exactly what each one has.
