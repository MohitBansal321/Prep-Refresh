# Dijkstra's Algorithm (Weighted Shortest Path)

> **5-min refresher instead?** [cheatsheet.md](cheatsheet.md) has the one-table summary and recall questions.

## Intent

Find the shortest-cost path from a single source to every other node in a graph whose edges carry **non-negative weights** — the case plain BFS cannot handle, because BFS's "shortest = fewest hops" guarantee only holds when every edge costs exactly 1.

## Recognition Signal

The problem gives you a graph (or an implicit one — a grid with move costs, a set of flights with prices, a state space with transition costs) where edges have **different weights**, and asks for the *minimum total cost* to reach a node (or all nodes) from a source. If someone asks "fewest hops" with no weights, that's plain Graph BFS ([../graph-bfs-dfs/](../graph-bfs-dfs/)), not this.

## Core Idea

BFS explores strictly in ring order because every edge costs 1, so the first time you reach a node is guaranteed to be via the fewest hops. The moment edges have different weights, "discovered first" no longer means "cheapest" — a node reached via one hop of weight 100 might later be reachable via two hops totaling weight 5. Dijkstra fixes this by replacing BFS's FIFO queue with a **min-heap keyed on tentative distance**: always expand the not-yet-finalized node with the smallest known distance so far, and when you pop a node, its distance is now guaranteed final (any other path to it would have to go through a node with an even larger distance already in the heap, so it can never be cheaper). Relaxing a neighbor means: if `dist[u] + weight(u, v) < dist[v]`, update `dist[v]` and push the improved distance.

## Template

```cpp
// adj[u] = list of {v, weight} for each edge u -> v
std::vector<int> dijkstra(int n, int src,
                           const std::vector<std::vector<std::pair<int,int>>>& adj) {
  const int INF = std::numeric_limits<int>::max();
  std::vector<int> dist(n, INF);
  dist[src] = 0;

  // min-heap of {distance, node}; std::priority_queue is a max-heap by
  // default, so `greater<>` flips it to a min-heap.
  std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>,
                       std::greater<>> pq;
  pq.push({0, src});

  while (!pq.empty()) {
    auto [d, u] = pq.top();
    pq.pop();
    if (d > dist[u]) continue;  // stale entry — a better one already won

    for (auto [v, w] : adj[u]) {
      if (dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        pq.push({dist[v], v});
      }
    }
  }
  return dist;
}
```

## Complexity

**Time:** `O((V + E) log V)` with a binary heap — each edge can trigger at most one push, and each push/pop is `O(log V)`.
**Space:** `O(V + E)` for the adjacency list, distance array, and heap.

## Common Mistakes

- **Marking a node "visited" and skipping it on the next pop, instead of comparing distances.** The `if (d > dist[u]) continue;` check above is what makes stale heap entries harmless — without it (or with a separate `visited` array checked before popping), a subtle bug creeps in if you ever push a node more than once, which Dijkstra does routinely.
- **Using Dijkstra with negative edge weights.** The correctness argument depends on "the cheapest unfinalized node can never get cheaper later" — a negative edge breaks that outright (a longer path could later subtract enough to beat a shorter one already finalized). Negative weights require [Bellman-Ford](../bellman-ford/) instead.
- **Forgetting `dist[src] = 0` before pushing the source**, or pushing the source with a distance that isn't 0.

## When To Use

- Single-source shortest path (or cost) in a graph with **non-negative** edge weights — the most common form this takes in interviews is "minimum cost to reach node B from node A" where costs vary per edge (flight prices, road tolls, effort/risk scores).

## When NOT To Use

- **Edge weights can be negative** — use [Bellman-Ford](../bellman-ford/) (`O(V·E)`, also detects negative cycles).
- **All edges cost the same (unweighted)** — plain BFS ([../graph-bfs-dfs/](../graph-bfs-dfs/)) gets the same answer in `O(V + E)`, with no heap needed.
- **You need shortest paths between every pair of nodes on a small graph** — [Floyd-Warshall](../floyd-warshall/) (`O(V³)`) computes all pairs at once, simpler to reason about than running Dijkstra from every source.
- **The graph is a DAG** — a single topological-order pass ([../topological-sort/](../topological-sort/)) computes shortest paths in `O(V + E)`, no heap required, since the ordering already guarantees you process predecessors before successors.

## Similar Patterns

- **Graph BFS/DFS** ([../graph-bfs-dfs/](../graph-bfs-dfs/)): the unweighted special case — Dijkstra degenerates to BFS when every edge weight is 1.
- **Bellman-Ford** ([../bellman-ford/](../bellman-ford/)): the fallback the moment a negative edge is possible — slower (`O(V·E)` vs. `O((V+E) log V)`), but correct unconditionally, and detects a reachable negative cycle as a byproduct.
- **Floyd-Warshall** ([../floyd-warshall/](../floyd-warshall/)): the all-pairs generalization — computes every pair's shortest distance in one `O(V³)` pass instead of running Dijkstra from every source.
- **Minimum Spanning Tree** ([../mst-kruskal-prim/](../mst-kruskal-prim/)): Prim's variant reuses this exact "min-heap driving a greedy frontier" shape, but the heap key means something different — cumulative distance here, a single edge's weight there — and the two algorithms answer genuinely different questions (cheapest path to one node vs. cheapest way to connect every node).
- **Top "K" Elements / Two Heaps** ([../../searching-sorting-patterns/](../../searching-sorting-patterns/)): share the same "min-heap driving a greedy selection" mechanic, applied to a different problem shape.

## Further Reading

- LeetCode — Network Delay Time (743), Cheapest Flights Within K Stops (787), Path With Minimum Effort (1631), Swim in Rising Water (778).
- *Introduction to Algorithms* (CLRS) — single-source shortest paths chapter (Dijkstra's and Bellman-Ford side by side).
