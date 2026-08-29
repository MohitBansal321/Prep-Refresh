# Floyd-Warshall — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — all-pairs shortest path via triple-nested waypoint relaxation. |
| **Recognition Signal** | The question needs distances (or reachability) between **every pair** of nodes, not just from one fixed source — "distance/fare/latency matrix," "how many cities can each city reach," "transitive closure," or "detect a negative cycle anywhere in the graph." The graph is usually small (`n` in the low hundreds), which is what makes `O(V³)` the pragmatic choice. |
| **Problem** | Running a single-source algorithm (Dijkstra or Bellman-Ford) from every one of the `n` nodes answers the same question, but at real, avoidable cost — `n` separate runs, `n` separate result sets to manage, and `O(V·E)` per Bellman-Ford run if negative weights are possible, giving `O(V²·E)` overall. |
| **Solution** | Maintain an `n × n` matrix. For each node `k` from `0` to `n-1`, treat `k` as a candidate waypoint: for every pair `(i, j)`, check whether `dist[i][k] + dist[k][j] < dist[i][j]`, and update if so. `k` must be the **outermost** loop — the induction proof depends on `dist[i][k]` and `dist[k][j]` already reflecting every earlier waypoint when read. |
| **Time / Space Complexity** | `O(V³)` time — three nested loops over all `V` nodes · `O(V²)` space for the full distance matrix · both are fixed costs depending only on node count, never on edge count, unlike edge-list-based algorithms. |
| **Pros** | One matrix answers any future "distance between X and Y" query in O(1) · tolerates negative weights, matching Bellman-Ford · detects a negative cycle **anywhere** in the graph, not just from one chosen source · extremely simple to implement correctly — three loops, one comparison, no heap. |
| **Cons** | `O(V³)` regardless of how sparse the graph is — no way to exploit sparsity the way an edge-list algorithm does · `O(V²)` space becomes the binding constraint well before `O(V³)` time does, on graphs with tens of thousands of nodes · wasteful if only one source's distances are actually needed. |
| **Use When** | Genuinely all-pairs distances or reachability needed · graph is small-to-medium · negative weights are possible and all-pairs results (not just single-source) are required · need to detect a negative cycle anywhere, not from one specific source. |
| **Avoid When** | Only one source matters (use Dijkstra or Bellman-Ford instead — strictly less work for the actual question) · the graph is large and sparse (`V` runs of a single-source algorithm can beat the fixed `O(V³)`) · `O(V²)` memory is not available · the graph is a DAG (a per-source topological pass is often cheaper). |
| **Related Patterns** | Dijkstra ([../dijkstras-algorithm/](../dijkstras-algorithm/README.md)) — single-source, faster per run, no negative weights · Bellman-Ford ([../bellman-ford/](../bellman-ford/README.md)) — single-source, tolerates negative weights, the direct ancestor this pattern generalizes to all-pairs · Union Find ([../union-find/](../union-find/README.md)) — answers connectivity alone, near-O(1) per query, no actual distance. |

### The three relaxation flavors

| Payload | Relaxation rule | Example |
|---|---|---|
| Numeric distance | `dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])` | the classic all-pairs shortest path |
| Boolean reachability | `reach[i][j] = reach[i][j] OR (reach[i][k] AND reach[k][j])` | transitive closure (Course Schedule IV) |
| Product (ratio) | `quot[i][j] = quot[i][k] * quot[k][j]` when `quot[i][j]` is still unknown | Evaluate Division |

### Template Skeleton

```cpp
std::vector<std::vector<long long>> dist(n, std::vector<long long>(n, kInf));
for (int i = 0; i < n; ++i) dist[i][i] = 0;
// ... fill dist[edge.from][edge.to] with direct edge weights ...

for (int k = 0; k < n; ++k) {           // k MUST be outermost
  for (int i = 0; i < n; ++i) {
    if (dist[i][k] == kInf) continue;
    for (int j = 0; j < n; ++j) {
      if (dist[k][j] == kInf) continue;
      if (dist[i][k] + dist[k][j] < dist[i][j]) {   // <-- the whole algorithm
        dist[i][j] = dist[i][k] + dist[k][j];
      }
    }
  }
}

// Negative cycle anywhere in the graph: any dist[i][i] that dropped below 0.
```

### Remember In One Sentence

> **Floyd-Warshall treats every node in turn as a candidate waypoint for every pair, so that after all `n` nodes have had their turn, `dist[i][j]` reflects the best path using any subset of the graph as intermediate stops — the entire correctness argument, and the entire reason `k` must be the outermost loop.**

### Two Facts People Get Wrong

- The loop order (`k`, `i`, `j`) doesn't matter, as long as all three run? **No** — `k` must be outermost. Putting `i` or `j` outermost does not crash; it silently produces a partially-relaxed matrix, because some pairs get checked against a waypoint value that has not itself been fully computed yet.
- You need a separate "previous iteration" copy of the matrix to update it safely? **No** — updating `dist[][]` in place during the same `k` iteration is safe, because relaxing `dist[i][j]` through waypoint `k` only ever reads `dist[i][k]` and `dist[k][j]`, and neither of those two values could improve any further during this same `k` without routing through `k` itself, which never happens.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. Why must the `k` loop be the outermost of the three nested loops? What specifically goes wrong if it is not?
2. Why is the in-place update (no separate "previous `k`" matrix copy) safe? State the argument in terms of what `dist[i][k]` and `dist[k][j]` could or could not still change during the current `k` iteration.
3. How does a negative cycle show up in the final matrix, and why does checking only one node's diagonal entry potentially miss cycles elsewhere in the graph?
4. What is the time and space complexity, and what do both depend on (node count or edge count)?
5. Give the relaxation rule for boolean transitive closure (Course Schedule IV) and explain why `OR`/`AND` replaces `MIN`/`+`.
6. Give the relaxation rule for the multiplicative case (Evaluate Division) and explain why the direction of an edge and its reciprocal both need to be stored.
7. When would `V` runs of Dijkstra or Bellman-Ford beat Floyd-Warshall's flat `O(V³)`? Name the specific property of the graph that decides it.
8. In [problems/04-detonate-the-maximum-bombs.cpp](problems/04-detonate-the-maximum-bombs.cpp), why can `reach[i][j]` be true while `reach[j][i]` is false? What does that say about how the direct edges must be built?
9. What extra data structure would you add to recover the actual shortest path, not just its distance? What update accompanies every successful relaxation?
10. Give one real production/system context (not a LeetCode problem) where this algorithm runs today.
