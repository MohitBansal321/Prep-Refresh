# Bellman-Ford — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — single-source shortest path via unconditional edge relaxation, tolerant of negative weights. |
| **Recognition Signal** | A graph whose edges can be **negative** (refunds, gains, differences, credits), or a question asking whether a **negative cycle** is reachable from a source (arbitrage detection, feasibility of a system of difference constraints). If every edge is guaranteed non-negative, that recognition signal belongs to Dijkstra instead. |
| **Problem** | Dijkstra's "the closest unfinalized node can never get cheaper later" guarantee collapses the instant a negative edge exists — a node popped early with a seemingly-final distance can later be beaten by a path through a large negative edge waiting further along. |
| **Solution** | Relax **every** edge, unconditionally, `V - 1` times: if `dist[u] + weight(u, v) < dist[v]`, update `dist[v]`. The `V - 1` bound comes from a shortest simple path using at most `V - 1` edges. A `V`-th pass that still finds an improvement proves a negative cycle is reachable from the source. |
| **Time / Space Complexity** | `O(V·E)` time — `V - 1` passes over `E` edges, plus one extra pass for cycle detection · `O(V)` space for the distance array plus `O(E)` for the edge list · a bounded variant (K stops, a time budget, a discount count) replaces `V - 1` with that resource's own limit, giving `O(K·E)` instead. |
| **Pros** | Works unconditionally on any edge weights, negative included · detects negative cycles as a direct byproduct of the same loop, not a separate algorithm · no heap and no adjacency-list-with-fast-lookup requirement — a flat edge list is enough · naturally bounds to a resource budget (stops, time, discounts) by simply changing the pass count. |
| **Cons** | `O(V·E)` is strictly worse than Dijkstra's `O((V+E) log V)` whenever weights happen to be non-negative — you pay for generality you may not need · no meaningful early exit toward a single target, unlike Dijkstra popping its target and stopping · detecting *that* a negative cycle exists is one boolean, not which specific distances it corrupted. |
| **Use When** | Edge weights can be negative · you need to detect a reachable negative cycle · the problem hands you an explicit resource bound (at most K edges/stops, a time limit, a discount budget) that is tighter than `V - 1` · edges arrive as a flat list rather than a pre-built adjacency structure. |
| **Avoid When** | All weights are verified non-negative (use Dijkstra — faster, no tradeoff) · you need all-pairs shortest paths (use Floyd-Warshall) · the graph is a DAG (one topological-order relaxation pass, `O(V+E)`, no repeated passes needed) · every edge costs exactly 1 (plain BFS, `O(V+E)`). |
| **Related Patterns** | Dijkstra ([../dijkstras-algorithm/](../dijkstras-algorithm/README.md)) — faster, but only correct for non-negative weights · Floyd-Warshall ([../floyd-warshall/](../floyd-warshall/README.md)) — all-pairs, `O(V^3)`, also tolerant of negative weights · Topological Sort ([../topological-sort/](../topological-sort/README.md)) — the DAG-only shortcut that needs no repeated passes at all. |

### Template Skeleton

```cpp
struct Edge { int from, to, weight; };

std::vector<long long> dist(n, kInf);
dist[src] = 0;

for (int pass = 0; pass < n - 1; ++pass) {
  bool changed = false;
  for (const Edge& e : edges) {
    if (dist[e.from] == kInf) continue;         // source side unreached
    if (dist[e.from] + e.weight < dist[e.to]) {
      dist[e.to] = dist[e.from] + e.weight;
      changed = true;
    }
  }
  if (!changed) break;                          // converged early
}

// One more pass: anything that STILL relaxes means a negative cycle
// is reachable from src, and every distance below is unreliable.
bool hasNegativeCycle = false;
for (const Edge& e : edges) {
  if (dist[e.from] != kInf && dist[e.from] + e.weight < dist[e.to]) {
    hasNegativeCycle = true;
    break;
  }
}
```

### Remember In One Sentence

> **Bellman-Ford trades Dijkstra's speed for unconditional correctness by refusing to trust any node's distance as final until every edge has been relaxed `V - 1` times — enough passes to account for every simple path — and one more pass beyond that turns "did anything still improve" into a free negative-cycle detector.**

### Two Facts People Get Wrong

- A `visited` array, like Dijkstra's, would make this faster? **No** — Bellman-Ford has no notion of a "finalized" node until every one of the `V - 1` passes completes; a node's distance can legitimately improve on pass 3 after already improving on pass 1, and a visited guard would silently block that valid later improvement, producing wrong, too-large distances.
- Skipping the extra `V`-th pass just saves time when you don't care about negative cycles? **No** — if a negative cycle reachable from the source exists and you skip the check, every distance you return is a plausible-looking number that does not actually represent a shortest path, because no shortest path exists. The failure is silent, not a crash.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. Why exactly `V - 1` passes, not `V` or `log V`? State the argument in terms of what a shortest *simple* path can look like.
2. What does the `V`-th pass detect, and why does an improvement found on that pass necessarily mean a cycle was involved?
3. Why does adding a Dijkstra-style `visited` array break Bellman-Ford instead of merely being redundant? Describe the exact scenario where it produces a wrong answer.
4. What does `if (dist[e.from] == kInf) continue;` prevent, concretely, if it were removed?
5. In [problems/02-cheapest-flights-within-k-stops.cpp](problems/02-cheapest-flights-within-k-stops.cpp), why must each pass relax against a *snapshot* of the previous pass's distances rather than mutating in place? What specific wrong answer results if you mutate in place?
6. A negative cycle exists somewhere in the graph, but `hasNegativeCycle` comes back `false`. Is that necessarily a bug? Explain using [code.cpp](code.cpp)'s third test case.
7. How does bounding the pass count to `K + 1` instead of `V - 1` change what the algorithm computes? Why is that the correct way to encode "at most K stops"?
8. Name two problem shapes where Bellman-Ford is the wrong tool even though the graph has weighted edges, and say which algorithm you would reach for instead in each case.
9. Why can Dijkstra not simply be patched to "keep relaxing after a node is popped" to handle negative edges cheaply? What specifically is lost by doing that?
10. Give one real production/system context (not a LeetCode problem) where this algorithm — or a resource-bounded variant of it — runs today.
