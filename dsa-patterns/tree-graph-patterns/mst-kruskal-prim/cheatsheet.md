# Minimum Spanning Tree (Kruskal's / Prim's) — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — cheapest way to connect every node, via greedy edge selection guarded against cycles. |
| **Recognition Signal** | "Minimum total cost to connect everything" / "cheapest network" / "connect all X" — with **no privileged source or destination node**. The moment a specific "from A" or "to B" appears, the question is shortest path, not MST. |
| **Problem** | Brute-forcing every possible spanning tree is infeasible (up to `n^(n-2)` distinct trees on a complete graph); a shortest-path algorithm answers a different question (cheapest to reach one node) and can silently produce the wrong tree if used here by mistake. |
| **Solution** | The cheapest available edge that does not close a cycle can always be safely added to a growing MST (an exchange argument). **Kruskal's**: sort every edge, add each unless [Union Find](../union-find/cheatsheet.md) says its endpoints are already connected. **Prim's**: grow one tree from a start node, always taking the cheapest edge leaving the tree via a min-heap. |
| **Time / Space Complexity** | Kruskal's: `O(E log E)` time (the sort dominates), `O(V + E)` space · Prim's (binary heap): `O(E log V)` time, `O(V + E)` space · both beat brute force's exponential blowup by construction, not by a constant-factor trick. |
| **Pros** | Provably optimal, via a short exchange-argument proof · Kruskal's needs only a flat edge list, no adjacency structure · Prim's needs no separate cycle-detection structure at all — the `inTree` marker suffices · both generalize cleanly (max spanning tree, virtual-node tricks, forced/forbidden edges). |
| **Cons** | Answers a fundamentally different question from shortest path — easy to reach for by mistake when a problem statement is ambiguous · standard form assumes an undirected graph · gives zero redundancy by construction, the opposite of what fault-tolerant network design usually wants. |
| **Use When** | The question has no privileged source/destination and asks for cheapest total connectivity · the graph is undirected · a flat edge list is available (Kruskal's) or an adjacency list plus density favor a heap-based frontier (Prim's). |
| **Avoid When** | A specific source or destination matters (use Dijkstra/Bellman-Ford) · the graph is directed (needs minimum spanning arborescence, a different algorithm) · redundancy/fault-tolerance is required, not minimum connectivity · only pure connectivity (no weights) matters (plain Union Find or BFS/DFS suffices). |
| **Related Patterns** | Union Find ([../union-find/](../union-find/README.md)) — the exact mechanism behind Kruskal's cycle check · Dijkstra ([../dijkstras-algorithm/](../dijkstras-algorithm/README.md)) — superficially similar to Prim's (both grow via a min-heap), but the heap key means something different (cumulative distance vs. a single edge weight) · Graph BFS/DFS ([../graph-bfs-dfs/](../graph-bfs-dfs/README.md)) — pure connectivity with no weights at all. |

### Template Skeleton (Kruskal's)

```cpp
std::sort(edges.begin(), edges.end(),
          [](const Edge& a, const Edge& b) { return a.weight < b.weight; });

DisjointSet dsu(n);
long long totalWeight = 0;
int edgesUsed = 0;
for (const Edge& e : edges) {
  if (dsu.unionSets(e.u, e.v)) {      // true only if this avoids a cycle
    totalWeight += e.weight;
    ++edgesUsed;
    if (edgesUsed == n - 1) break;    // tree complete
  }
}
// edgesUsed < n - 1 here means the graph was disconnected: no spanning tree exists.
```

### Remember In One Sentence

> **Both Kruskal's and Prim's rest on the same exchange argument — the cheapest edge available that does not close a cycle can always be added to a minimum spanning tree without loss — differing only in whether "available" means "anywhere in the sorted edge list" (Kruskal's) or "on the current tree's frontier" (Prim's).**

### Two Facts People Get Wrong

- An MST is the cheapest way to *reach* any one node from any other? **No** — it minimizes the *total* weight of edges used to connect everything, with no guarantee about the resulting path length between any specific pair. That guarantee is what a shortest-path tree (Dijkstra/Bellman-Ford) provides instead, and the two trees can disagree on the same graph.
- Prim's and Dijkstra's are basically the same algorithm since both grow via a min-heap from a start node? **No** — the heap key means something different in each: Dijkstra's key is *cumulative distance from the source so far*; Prim's key is *the single edge weight* connecting the frontier to a new node. Using the wrong key silently implements the other algorithm's logic while looking almost identical.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the exchange argument that makes Kruskal's (or Prim's) provably optimal, in your own words, without looking.
2. Why must Kruskal's cycle check use `find(a) == find(b)` (or `unionSets`'s return value) rather than comparing `parent_[a] == parent_[b]` directly?
3. What does the stale-entry check `if (inTree[to]) continue;` in Prim's prevent, concretely, if it were removed?
4. Give the time complexity of Kruskal's and of Prim's (binary heap), and say which term in each comes from which part of the algorithm.
5. An MST and a shortest-path tree from a fixed source can coincide on some graphs. Under what specific condition does that happen, and why does it break the moment edge weights differ?
6. In [problems/03-optimize-water-distribution-in-a-village.cpp](problems/03-optimize-water-distribution-in-a-village.cpp), what does the virtual node 0 represent, and why does treating "build a well" as an edge from node 0 make the problem an ordinary MST instance?
7. How do you detect that no spanning tree exists (the graph is disconnected), in both Kruskal's and Prim's? What exactly gets checked?
8. In [problems/04-find-critical-and-pseudo-critical-edges.cpp](problems/04-find-critical-and-pseudo-critical-edges.cpp), what two counterfactual re-runs does classifying a single edge require, and what does each one's outcome tell you?
9. Why is an MST not necessarily unique? Under what specific condition on the edge weights can multiple distinct minimum spanning trees exist?
10. Give one real production/system context (not a LeetCode problem) where an MST algorithm — or a close variant — runs today.
