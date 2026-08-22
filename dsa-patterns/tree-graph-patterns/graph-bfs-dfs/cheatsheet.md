# Graph BFS/DFS — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — traversal technique for general graphs (possibly cyclic, no root). |
| **Recognition Signal** | Input is a **graph or grid** (not guaranteed tree-shaped), and you need the **minimum number of hops** (BFS), or **reachability / connected components / cycle detection** (DFS). A 2D grid is almost always an implicit graph: cells are nodes, up/down/left/right neighbors are edges. |
| **Problem** | Tree BFS/DFS ported unchanged to a graph never terminates on a cycle (queue never empties / stack overflows); DFS finds *a* path but gives no shortest-path guarantee; single-start traversals silently miss disconnected pieces. |
| **Solution** | Same two traversals plus an explicit `visited` marker checked before exploring and set **the instant a node is discovered** (enqueued/pushed), not when processed. BFS uses a FIFO queue for ring-by-ring order; DFS recurses (or uses an explicit stack); directed-cycle detection upgrades `visited` to a three-state (`unvisited`/`in-progress`/`done`) marker. |
| **Time / Space Complexity** | O(V + E) time for both traversals (each node entered at most once thanks to the guard; each edge examined once or twice). O(V) space — `visited` always; BFS adds O(V) worst-case queue, DFS adds O(V) worst-case recursion stack. |
| **Pros** | Exact shortest-hop guarantee from BFS for free in unweighted graphs · DFS naturally answers structural questions (reachability, components, cycles) · O(V + E) is near-optimal · composable — components and cycle detection are the same skeleton plus one counter or one extra state · grid problems map directly onto it. |
| **Cons** | BFS needs O(V) queue space on wide graphs · recursive DFS can overflow the call stack at depth ~V on chain-like graphs · neither handles weighted edges (BFS silently returns wrong "shortest" paths) · graph-wide questions require remembering the outer loop over all nodes · easy to mark visited on dequeue instead of enqueue. |
| **Use When** | Minimum hops/steps/minutes in an **unweighted** graph (BFS) · reachability, connected-component count, flood fill (either traversal) · directed-cycle detection / deadlock detection (DFS + three states) · grid problems (islands, mazes, rotting-over-time). |
| **Avoid When** | Edges have weights and hop-count ≠ shortest distance (use Dijkstra, or Bellman-Ford for negative weights) · you need a valid ordering under dependencies (Topological Sort) · edges arrive incrementally with repeated connectivity queries (Union Find) · the structure really is a tree (drop the `visited` set — use Tree BFS/DFS). |
| **Related Patterns** | Tree BFS ([../tree-bfs/](../tree-bfs/)) and Tree DFS ([../tree-dfs/](../tree-dfs/)) — same traversals minus the `visited` guard · Topological Sort ([../topological-sort/](../topological-sort/)) — ordering under dependencies, built on this machinery · Union Find ([../union-find/](../union-find/)) — incremental connectivity without traversal · Dijkstra — BFS generalized to weighted edges via a min-priority queue. |

### Template Skeleton

```cpp
// BFS shortest hops (unweighted) — mark on ENQUEUE, never on pop
std::vector<int> dist(n, -1);
std::queue<int> frontier;
dist[src] = 0;
frontier.push(src);
while (!frontier.empty()) {
    int node = frontier.front();
    frontier.pop();
    for (int neighbor : adj[node]) {
        if (dist[neighbor] == -1) {          // not yet discovered
            dist[neighbor] = dist[node] + 1; // set BEFORE push
            frontier.push(neighbor);
        }
    }
}

// DFS connected components — outer loop over ALL nodes (disconnected graphs!)
std::vector<bool> visited(n, false);
std::function<void(int)> dfs = [&](int node) {
    visited[node] = true;                    // mark on entry
    for (int neighbor : adj[node])
        if (!visited[neighbor]) dfs(neighbor);
};
int components = 0;
for (int start = 0; start < n; ++start)
    if (!visited[start]) { ++components; dfs(start); }

// Directed-cycle detection — THREE states, not a boolean:
// 0 = unvisited, 1 = in progress (on current path), 2 = done
// state[neighbor] == 1 -> back edge -> cycle; state == 2 -> safe, skip.
```

### Remember In One Sentence
> **Graph BFS/DFS is exactly the tree traversal you already know plus one non-negotiable piece of state — a `visited` marker applied at discovery time — which turns cycles from an infinite loop into ordinary work, lets BFS hand you shortest hop-counts for free via its ring-by-ring order, and lets DFS answer reachability, components, and (with a third state) directed cycles.**

### Two Facts People Get Wrong
- The `visited` set is a performance optimization? **No** — it is a *correctness* requirement; without it a cyclic graph makes BFS hang forever and recursive DFS overflow the stack, not merely run slower.
- Seeing an already-visited node again in a directed graph means you found a cycle? **No** — it only means a cycle if that node is still on the *current* recursion path (`in-progress`); a diamond DAG revisits nodes via two parents with no cycle anywhere.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. What structural guarantee does a tree have that a general graph lacks, and what single piece of state compensates for its absence?
2. Why does BFS guarantee the first time you reach any node is via a shortest path? State the ring-by-ring argument precisely.
3. At what exact moment must a node be marked visited in BFS, and what concrete bug happens if you mark it one step later?
4. Why does a function claiming a graph-wide answer (component count, "any cycle anywhere") need an outer loop over every node? What does forgetting it look like in production?
5. Walk through why a plain boolean `visited` array fails for directed-cycle detection, using the diamond DAG example (0->1, 0->2, 1->3, 2->3).
6. Name the three states in directed-cycle detection and what each means; which state transition signals a back edge?
7. What are the time and space complexities of both traversals, and why is the edge term E absent from tree-traversal complexity reasoning?
8. Edges now carry weights. Why does BFS silently return a wrong answer rather than erroring, and which algorithm replaces it — with what complexity?
9. When is recursive DFS a production risk even though the algorithm is textbook-correct, and what is the fix?
10. How does a 2D grid become a graph without ever building an adjacency list, and which worked problem in [problems/](problems/README.md) demonstrates each of: implicit-grid components, adjacency-matrix components, two-colouring, and generated-on-the-fly neighbors?
