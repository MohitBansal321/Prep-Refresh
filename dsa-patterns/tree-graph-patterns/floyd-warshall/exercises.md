# Floyd-Warshall — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing when a problem is genuinely asking about *every pair* of nodes rather than one fixed source, and (2) seeing that the waypoint-relaxation loop generalizes far beyond numeric shortest paths — to boolean reachability, to products, and to recovering the actual path, not just its cost.

> Rule of thumb for every exercise: before writing a single line, ask "do I actually need every pair's answer, or would a single-source algorithm from the one node I care about be simpler and faster?" Floyd-Warshall is frequently reached for out of habit on problems that only ever needed one source.

---

## Easy — Sum of Distances Between All Pairs

Given the adjacency matrix of a small, unweighted, undirected graph (`n ≤ 50` nodes), compute the sum of shortest-path distances between every pair of distinct nodes, treating unreachable pairs as contributing `0` to the sum rather than infinity.

**Constraints to notice:** the graph is unweighted, so every edge that exists has weight 1 — this is Floyd-Warshall's simplest possible numeric case, useful for building the reflex of translating an adjacency matrix into an initial `dist[][]` (0 on the diagonal, 1 for a direct edge, infinity otherwise) before running the triple loop.

**Task:** adapt `floydWarshall` from [code.cpp](code.cpp) directly. After computing the full distance matrix, sum every `dist[i][j]` for `i < j` (each unordered pair counted once), skipping any pair still at infinity.

**Think about:** why is skipping infinite-distance pairs in the final sum a different decision from how the algorithm itself treats infinity internally during relaxation? Could you accidentally include a pair that only *looks* finite because of an unguarded addition against the infinity sentinel?

---

## Medium — Reconstructing the Actual Shortest Path

[code.cpp](code.cpp)'s `floydWarshall` returns only the distance matrix — it never tells you *which* nodes a shortest path from `i` to `j` actually passes through. Extend it to answer: given a source and destination, return the sequence of nodes on one shortest path between them (any valid one, if there are ties).

**Task:** maintain a parallel `next[i][j]` table alongside `dist[i][j]`, initialized so `next[i][j] = j` wherever a direct edge `i -> j` exists (and left as "none" otherwise). Every time the main relaxation updates `dist[i][j] = dist[i][k] + dist[k][j]`, also update `next[i][j] = next[i][k]` — meaning "the first step from `i` toward `j` is now the same as the first step from `i` toward `k`." To reconstruct a path from `src` to `dst`, repeatedly follow `next[current] = next[current][dst]` until `current == dst`.

**Think about:** why does `next[i][j] = next[i][k]` correctly capture "the first step," rather than needing `next[k][j]` at all? Trace through one relaxation by hand on a 4-node graph and confirm the reconstructed path actually matches a path whose total weight equals `dist[src][dst]`.

---

## Hard — All-Pairs Widest Path (Maximum Bottleneck)

Given a weighted, undirected graph, define the "width" of a path as the **minimum** edge weight along it (the bottleneck), not the sum. For every pair of nodes, find the path between them that **maximizes** this bottleneck value — the "widest path" or "maximum capacity path" problem, which shows up in network-bandwidth and load-capacity questions ("what is the most bandwidth guaranteed available between every pair of routers, given each link's individual capacity").

**Constraints to notice:** this is Floyd-Warshall with yet another relaxation operation. Where the standard version relaxes with `dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])`, and Course Schedule IV relaxes with `OR`, this problem relaxes with `width[i][j] = max(width[i][j], min(width[i][k], width[k][j]))` — combining two path segments takes the `min` of their widths (a path is only as wide as its narrowest link), and comparing candidate paths takes the `max` (you want the widest option available).

**Task:** initialize `width[i][i] = infinity` (a path from a node to itself has no bottleneck) and `width[i][j]` to the direct edge weight where one exists, `-infinity` (or a sentinel meaning "no path") otherwise. Run the same `k`-outermost triple loop from [code.cpp](code.cpp), substituting the relaxation rule above.

**Then answer:** why must `width[i][i]` start at infinity rather than `0`, given that the combining operation is `min`? What would break in the very first relaxation pass if it started at `0` instead?

---

## Real-World Challenge — A Service-Mesh Latency and Reachability Dashboard

You run the infrastructure team for a company with roughly 40 internal microservices, each capable of calling several others directly (a service dependency graph, with each edge's weight being the median call latency in milliseconds). Product wants a dashboard answering two questions for any pair of services picked by an on-call engineer during an incident: "what is the fastest known call path between these two services" and "is service A even transitively reachable from service B at all."

**Task:**

1. Model the service graph as an adjacency structure and compute the full all-pairs latency matrix with Floyd-Warshall, refreshed as a nightly batch job (the graph changes slowly — new services are added, old ones deprecated — so recomputing once a day is acceptable, unlike a live per-request computation).
2. Reuse the `next[i][j]` technique from the Medium exercise above to let the dashboard show the actual call chain, not just the aggregate latency number, when an engineer investigates a specific pair during an incident.
3. Extend the reachability question specifically: some service pairs will show `dist[i][j] = infinity` (Course Schedule IV's boolean closure, computed as a side effect of the same matrix, answers "reachable at all" directly from whether `dist[i][j]` is finite).
4. **Discuss:** with roughly 40 services, `O(V³)` is a few tens of thousands of operations — trivially fast. Argue for the point at which you would switch strategies (running Dijkstra from each service instead) as the company grows to, say, 5,000 microservices, and name the specific complexity comparison (`O(V³)` vs. `O(V·(V+E) log V)`) that justifies switching at that scale, not merely "it feels slow."

---

## Bonus Challenge — Detecting *Every* Negative Cycle, Not Just Whether One Exists

[code.cpp](code.cpp)'s `floydWarshall` reports a single boolean: does any negative cycle exist anywhere in the graph. In a real system modeling currency exchange rates or promotion-credit chains (see the [Bellman-Ford exercises](../bellman-ford/exercises.md) for the arbitrage framing), knowing merely "yes, somewhere" is not actionable — you need to know **which** nodes are actually corrupted by a negative cycle, so you can flag exactly the exploitable subset.

**Task:** after running the standard relaxation, a node `i` is corrupted if `dist[i][i] < 0` **or** if `i` is reachable from, or can reach, some other node `j` where `dist[j][j] < 0` (since a negative cycle anywhere upstream or downstream of `i` can still make `i`'s own distances meaningless). Implement this two-stage check: first collect every node with a negative self-distance, then propagate "corrupted" status outward using the already-computed `dist[][]` matrix's finiteness as a reachability signal.

**Then, generalize in writing (no code required):** explain why checking only `dist[i][i] < 0` for the specific node `i` you care about is not sufficient in general, using a concrete 4-node example where node `i` itself has `dist[i][i] = 0` (no self-loop through a cycle) but is still downstream of a negative cycle elsewhere in the graph, making every distance *through* `i` unreliable even though `i`'s own diagonal entry looks clean.

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
