# Dijkstra's Algorithm — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — single-source weighted shortest path via greedy min-heap expansion. |
| **Recognition Signal** | A graph (explicit, or implicit — a grid with move costs, flights with prices, a state space) whose edges carry **non-negative weights**, and the question asks for the **minimum total cost** to get from a source to a node (or all nodes). If it says "fewest hops" and every move costs 1, that is plain BFS instead; if edges can be negative, that is Bellman-Ford. |
| **Problem** | BFS's "first time you reach a node = cheapest path" guarantee dies the moment edge weights differ: a node discovered in one hop of weight 100 may be reachable later via two hops totaling 5, so discovery order no longer implies optimality. |
| **Solution** | Replace BFS's FIFO queue with a **min-heap keyed on tentative distance**. Repeatedly pop the not-yet-finalized node with the smallest known distance; that distance is final (any alternative path must pass through nodes still in the heap, all of which have larger-or-equal keys). Relax each outgoing edge `u -> v`: if `dist[u] + w < dist[v]`, update `dist[v]` and push the improved entry. Skip stale heap entries (`d > dist[u]`). |
| **Time / Space Complexity** | O((V + E) log V) time with a binary heap (each edge triggers at most one push; each push/pop costs O(log V)). O(V + E) space for adjacency list, distance array, and heap (the lazy-deletion heap can hold up to E entries). |
| **Pros** | Provably optimal for non-negative weights · much faster than Bellman-Ford's O(V·E) on typical sparse graphs · simple template (~20 lines) reusable across many problem shapes (min cost, min effort/max-edge, state-augmented variants) · naturally yields distances to *all* nodes from one run. |
| **Cons** | Breaks outright with negative edge weights (correctness argument collapses) · overkill for unweighted graphs where BFS is O(V + E) with no heap · does not directly handle extra constraints per path (e.g. "at most K stops") without augmenting the state space · all-pairs on dense small graphs is simpler with Floyd-Warshall than V runs of Dijkstra. |
| **Use When** | Single-source shortest/cost path on non-negative weighted graphs · grid problems where moves have unequal costs or the path metric is "minimize the maximum step" · implicit graphs built from states (positions, remaining fuel/stops) · any interview phrasing like "minimum cost / effort / price / time to reach". |
| **Avoid When** | Negative weights possible (Bellman-Ford) · all edges equal weight (plain BFS) · shortest paths between *every pair* needed on a small graph (Floyd-Warshall, O(V³)) · the graph is a DAG (one topological-order pass, O(V + E), no heap). |
| **Related Patterns** | Graph BFS/DFS ([../graph-bfs-dfs/](../graph-bfs-dfs/README.md)) — the unit-weight special case Dijkstra degenerates into · Bellman-Ford ([../bellman-ford/](../bellman-ford/README.md)) — the negative-weight fallback · Floyd-Warshall ([../floyd-warshall/](../floyd-warshall/README.md)) — all-pairs · Top-K Elements ([../../searching-sorting-patterns/](../../searching-sorting-patterns/README.md)) — same min-heap-greedy mechanic on a different problem shape. |

### Template Skeleton

```cpp
// adj[u] = list of {v, weight} pairs for each directed edge u -> v
std::vector<int> dijkstra(int n, int src,
                          const std::vector<std::vector<std::pair<int,int>>>& adj) {
  const int kInf = std::numeric_limits<int>::max();
  std::vector<int> dist(n, kInf);
  dist[src] = 0;

  // Min-heap of {distance, node}; std::greater<> flips the default max-heap.
  std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>,
                      std::greater<>> pq;
  pq.push({0, src});

  while (!pq.empty()) {
    const int d = pq.top().first;   // distance recorded when pushed
    const int u = pq.top().second;  // node it belongs to
    pq.pop();
    if (d > dist[u]) continue;      // stale entry — a better one already won

    for (const std::pair<int,int>& edge : adj[u]) {
      const int v = edge.first;
      const int w = edge.second;
      if (dist[u] != kInf && dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;      // relax: found a cheaper path to v
        pq.push({dist[v], v});      // lazy: old entry stays, gets skipped later
      }
    }
  }
  return dist;
}
```

### Remember In One Sentence

> **Dijkstra replaces BFS's FIFO queue with a min-heap keyed on tentative distance so the closest unfinalized node is always expanded next — which guarantees each node's distance is finalized the first useful time it is popped, provided every edge weight is non-negative.**

### Two Facts People Get Wrong

- Mark visited nodes and skip them? **No** — the standard-safe check is `if (d > dist[u]) continue;` comparing the popped distance against the current best, not a boolean `visited` array set on push; a node can legitimately be improved after being pushed, and the stale-entry comparison handles that cleanly.
- The K-stops constraint (Cheapest Flights Within K Stops) is just Dijkstra with an early exit? **No** — plain Dijkstra finalizes a node on first pop, but with a hop limit the cheapest path may use more hops than allowed while an allowed-but-pricier path exists; you must augment the state to `(node, stopsUsed)` so both candidates coexist.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. What exact property of edge weights makes Dijkstra's correctness argument work, and what precisely breaks when a negative edge appears?
2. Why is the popped node's distance guaranteed final at its first *useful* pop? State the argument in terms of what is still sitting in the heap.
3. What does the `if (d > dist[u]) continue;` line do, why are stale entries created in the first place, and why not just delete them from the heap?
4. Why does BFS give correct shortest paths without a heap when every edge weighs 1? What does Dijkstra degenerate to in that case?
5. What is the time complexity with a binary heap, and where does each factor come from? How many entries can the lazy-deletion heap grow to?
6. In Path With Minimum Effort-style problems, how does the relaxation rule change when the path metric is "minimize the maximum edge along the path" instead of "minimize the sum"?
7. Why does plain Dijkstra fail on Cheapest Flights Within K Stops even though all weights are positive, and what exactly is the augmented state?
8. Name three alternatives to Dijkstra and the specific situation each one wins in.
9. Why is `dist[src] = 0` before pushing essential, and what symptom do you see if you forget it?
10. Give one real production/system context (not a LeetCode problem) where this algorithm — or a state-augmented variant — runs today.
