# Minimum Spanning Tree (Kruskal's / Prim's)

> **In one line:** sort every edge cheapest-first and add each one unless it would close a cycle (Kruskal's) — or grow one tree outward, always adding the cheapest edge leaving it (Prim's) — either way, the cheapest edge available can always be added safely.

```cpp
std::sort(edges.begin(), edges.end(),
          [](const Edge& a, const Edge& b) { return a.weight < b.weight; });

DisjointSet dsu(n);
for (const Edge& e : edges) {
  if (dsu.unionSets(e.u, e.v)) {   // <-- true only if this edge avoids a cycle
    totalWeight += e.weight;
    edgesUsed.push_back(e);
  }
}
```

**O(E log E)** time (Kruskal's, dominated by the sort) · **O(V + E)** space. Full runnable version, with both Kruskal's and Prim's: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---

## Intent

Given a connected, undirected, weighted graph, find a **spanning tree** — a subset of edges connecting every node with no cycles — whose total edge weight is as small as possible. This is a fundamentally different question from shortest path: an MST minimizes the total cost of *connecting everything*, not the cost of *reaching any particular node* from any particular other node.

## Real Life Analogy

Picture a telecom company that needs to run fiber between `n` towns, and already knows the cost of laying cable directly between every pair of towns that could be connected. The company does not need every town connected to every other town directly — it needs the towns connected *at all*, as cheaply as possible, so that a signal can reach any town from any other, possibly through several intermediate towns. Laying a direct, expensive cable between two towns that are already connected through cheaper existing links would be pure waste — money spent that adds zero new reachability. A minimum spanning tree is exactly the cheapest set of cable segments that connects every town with no such waste and no redundant loops.

The analogy breaks down in one place: a real fiber network often wants *redundancy* — a second cable so that one cut line does not partition the network — which is deliberately the opposite of what an MST provides. An MST is the cheapest network with **zero** redundancy (removing any single edge disconnects something); real infrastructure planning frequently pays extra, on purpose, for the redundancy an MST specifically avoids.

## Problem

A large class of problems needs the cheapest way to connect a set of things, not the cheapest way to reach one thing from another:

- **Physical network design** — cable, pipeline, or road-network planning where every location must be reachable, but no assumption is made about which two locations will ever need to talk to each other most.
- **Clustering** — treating "cheapest connections first, skip ones that would just merge two already-connected clusters" as a way to build a hierarchy of groupings (this is literally what single-linkage hierarchical clustering does).
- **Approximation algorithms for harder problems** — an MST is a well-known building block in approximate solutions to the Traveling Salesman Problem and Steiner Tree problems, because a real TSP tour must be at least as long as an MST connecting the same points.

> **Term: spanning tree.** A subset of a graph's edges that connects every node, contains no cycles, and therefore uses exactly `n - 1` edges for `n` nodes — the minimum possible number of edges that can still connect everything.

Confusing this with shortest path is the single most common mistake made when recognizing this pattern: "minimize total cost to connect everything" is an MST question; "minimize cost to get from A to B" is a shortest-path question ([Dijkstra](../dijkstras-algorithm/) or [Bellman-Ford](../bellman-ford/)), and the two produce genuinely different trees on the same graph — the cheapest way to connect every node is not generally the same tree as the cheapest way to reach any one specific node from another.

## Solution

Both algorithms rest on the same underlying guarantee, applied differently: **the cheapest edge available, at any point, can always be safely added to a growing minimum spanning tree**, as long as adding it does not create a cycle. This is provable by an exchange argument: if the globally cheapest edge in the graph were excluded from every possible MST, some MST would contain a cycle if that edge were added to it — and removing any other edge from that cycle, then adding the cheap edge instead, would produce a spanning tree that is no more expensive, contradicting that the original was already minimum. The same argument applies inductively to the next-cheapest edge that does not close a cycle with what has already been chosen, and so on.

**Kruskal's** applies this globally: sort every edge in the entire graph by weight once, then walk them cheapest-to-priciest, adding each one unless its two endpoints are already connected (which would form a cycle) — checked in near-`O(1)` via [Union Find](../union-find/).

**Prim's** applies the identical guarantee locally instead: grow one tree outward from a single starting node, and at every step add the cheapest edge that connects a node already in the tree to a node not yet in it — found via a min-heap of the tree's current "frontier" edges. Kruskal's considers cheap edges anywhere in the graph in any order; Prim's only ever considers edges touching what has already been grown.

## Architecture

**Kruskal's** has two participants: **the sorted edge list** (the single most important structural decision — sorting is what turns "find the globally cheapest safe edge" into "the next unprocessed edge in the list") and **the Union-Find structure**, whose sole job is answering "would adding this edge create a cycle" in near-constant time by checking whether the edge's two endpoints already share a root.

**Prim's** has three participants: **the `inTree` marker** (which nodes have already been absorbed into the growing tree), **the min-heap of frontier edges** (every edge leaving the current tree toward a node not yet in it, ordered by weight), and **the stale-entry check** (`if (inTree[to]) continue;`) — because a node can be pushed onto the heap multiple times, once for each edge reaching it from the growing tree, and only the cheapest such entry should ever actually be used; the rest are lazily discarded when popped.

Running Kruskal's: sort edges by weight. For each edge in order, call `unionSets` on its two endpoints; if it returns true (they were in different components), keep the edge and add its weight to the total. Stop once `n - 1` edges have been kept.

Running Prim's: mark node 0 as in the tree, push all of its edges onto the heap. Repeatedly pop the cheapest heap entry; if its destination is already in the tree, discard it as stale and continue. Otherwise, add the destination to the tree, record the edge, and push every edge from the newly-added node to a node not yet in the tree. Stop once `n - 1` edges have been added.

## Why Not Other Approaches?

**"Just take every edge below some cost threshold."** This does not guarantee a tree at all — it can leave nodes disconnected (too strict a threshold) or include cycles (too loose a threshold), and there is no single threshold that works correctly across different graphs, because the right answer depends on the graph's specific structure, not on an absolute cost cutoff.

**"Run Dijkstra or Bellman-Ford from an arbitrary source and call that the MST."** A shortest-path tree and a minimum spanning tree are genuinely different objects that can disagree on the same graph — a shortest-path tree minimizes the distance from one fixed source to every other node, while an MST minimizes the total weight of the edges used to connect everything, with no notion of a privileged source at all. Confusing the two produces a tree that is cheap to reach things *from one particular node*, not cheap *overall*.

**"Brute-force every possible spanning tree and pick the cheapest."** Correct by construction, but there can be an enormous number of distinct spanning trees on even a modestly-sized graph (up to `n^(n-2)` for a complete graph, by Cayley's formula) — utterly infeasible beyond tiny inputs, where Kruskal's or Prim's find the same answer in polynomial time via the exchange-argument shortcut.

**Tradeoff summary:** Kruskal's and Prim's are not competing solutions to different problems — they are two different traversal strategies (global-sorted vs. local-frontier) that provably reach the identical minimum total weight (though possibly a different specific tree, when ties exist). The practical choice between them is about the graph's shape, not correctness: Kruskal's edge-list-and-sort approach fits naturally when edges are handed to you as a flat list and the graph is sparse; Prim's node-growing approach fits naturally when you already have an adjacency-list representation and the graph is dense.

## Diagrams

- **Recognition** — [images/recognition-diagram.md](images/recognition-diagram.md), the flowchart distinguishing an MST question from a shortest-path question, and Kruskal's from Prim's based on how the graph is represented and how dense it is.
- **Flow** — [images/flow-diagram.md](images/flow-diagram.md), the control flow of both algorithms side by side — Kruskal's sort-then-filter loop and Prim's grow-the-frontier loop.
- **Trace** — [images/trace-diagram.md](images/trace-diagram.md), a step-by-step trace of both algorithms building the identical MST on the 5-node graph asserted in [code.cpp](code.cpp), showing that they arrive at the same total weight via genuinely different edge orders.

## The Code

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see both algorithms' mechanics clearly, side by side, before the problem-specific solutions in [problems/](problems/).

**`DisjointSet`** (in [code.cpp](code.cpp)). A minimal Union-Find implementation with path compression and union by rank, included so this module's `code.cpp` stays standalone — see [../union-find/README.md](../union-find/README.md) for the full explanation of why those two optimizations matter and how they interact.

**`kruskalMST`** (in [code.cpp](code.cpp)). Sorts a copy of the edge list by weight, then walks it once: `dsu.unionSets(e.u, e.v)` returns `true` only if the edge's two endpoints were in different components — exactly the "does not create a cycle" check — and the edge is kept only in that case. Stops as soon as `n - 1` edges have been kept, since a spanning tree can never need more.

**`primMST`** (in [code.cpp](code.cpp)). Starts with node 0 marked `inTree` and its edges seeded into a min-heap keyed on weight. Each iteration pops the cheapest heap entry; a destination already in the tree means this entry is stale (a cheaper path to that same node was already used) and is discarded via `continue`. Otherwise the destination joins the tree, and every edge from it to a node not yet in the tree is pushed onto the heap for future consideration.

**`main()`** (in [code.cpp](code.cpp)). Runs both algorithms against the same 5-node graph and confirms they produce the identical total weight (a strong cross-check that both implementations are correct), then separately verifies a triangle graph where the MST must skip an expensive redundant edge, a disconnected graph where no spanning tree exists, a 4-node cycle where exactly one of four equal-weight edges must be dropped, and the trivial single-node case.

**Files in [problems/](problems/).** Each file defines its own graph representation rather than including `code.cpp`, so it compiles and reads independently — see [problems/README.md](problems/README.md) for the index.

## Tradeoffs

**What Kruskal's buys you**

- **Simple to reason about**: sort once, then a single linear pass with an O(1)-ish cycle check.
- **Naturally suited to a flat edge list** — no adjacency structure needs to be built first.
- **Works well on sparse graphs**, where `E` is much smaller than `V²` and the sort's `O(E log E)` cost stays modest.

**What Prim's buys you**

- **Naturally suited to an adjacency-list representation** already available from other graph processing.
- **Never needs Union-Find at all** — the `inTree` boolean array is a strictly simpler cycle-avoidance mechanism, specific to the fact that Prim's only ever grows one connected tree rather than merging multiple components.
- **Can be more efficient on dense graphs** with certain heap implementations, since it processes edges incrementally from the frontier rather than sorting the entire edge list upfront.

**What both cost you, compared to shortest-path algorithms**

- **Neither produces a shortest-path tree.** The tree that minimizes total connection cost is generally *not* the tree that minimizes distance from any one fixed source — reaching for an MST when the actual question is "cheapest path from A to B" produces a plausible-looking but wrong-purpose answer.
- **Both assume the graph is undirected** in their standard form. Neither Kruskal's nor Prim's as shown here handles directed edges meaningfully — the "minimum spanning arborescence" problem for directed graphs needs a different algorithm (Chu-Liu/Edmonds').

**Versus each other:** Kruskal's and Prim's provably reach the same minimum total weight on the same graph (though the specific tree can differ when ties exist), so the choice between them is an engineering one about the graph's representation and density, not a correctness tradeoff.

## Complexity

**Kruskal's time:** `O(E log E)` — dominated by sorting the edges; the subsequent Union-Find operations are near-`O(1)` each (amortized, with path compression and union by rank), for `O(E)` total across all edges.

**Prim's time:** `O(E log V)` with a binary heap — each edge can trigger at most one heap push, and each push/pop costs `O(log V)`.

**Space:** `O(V + E)` for both — the edge list or adjacency list, plus the Union-Find arrays (Kruskal's) or the heap and `inTree` array (Prim's).

| Approach | Time | Best fit |
|---|---|---|
| Kruskal's | `O(E log E)` | Sparse graphs, edges already in a flat list |
| Prim's (binary heap) | `O(E log V)` | Dense graphs, adjacency list already available |
| Prim's (Fibonacci heap) | `O(E + V log V)` | Very dense graphs where the asymptotic improvement matters in practice |
| Brute force (enumerate all spanning trees) | Exponential (`n^(n-2)` trees on a complete graph) | Never — included only as the baseline the exchange argument beats |

## Common Mistakes

- **Confusing an MST with a shortest-path tree.** They are different objects that can disagree on the same graph — an MST minimizes total edge weight to connect everything; a shortest-path tree minimizes distance from one specific source. Using Dijkstra's output where an MST was actually needed silently produces a tree that is cheap to reach things from one node, not cheap overall. *Avoid:* before coding, ask "does this question have a privileged starting node?" — if not, it's an MST question.
- **Comparing `parent_[a] == parent_[b]` directly in Kruskal's cycle check instead of `find(a) == find(b)`.** Neither `a` nor `b` may currently point straight at their component's root, so a direct comparison can wrongly conclude two connected nodes are in different components. *Avoid:* always call `find()` on both sides (or use `unionSets`'s own return value, as [code.cpp](code.cpp) does) rather than reading `parent_[]` directly.
- **Forgetting the stale-entry check in Prim's.** A node can be pushed onto the heap multiple times as different edges reach it from the growing tree; failing to skip an already-`inTree` destination when popped will double-count that node's edge into the total weight and can corrupt the tree structure. *Avoid:* always check `if (inTree[to]) continue;` before processing a popped heap entry, exactly as [code.cpp](code.cpp) does.
- **Running Prim's on a disconnected graph and not detecting it.** If the heap empties before `n - 1` edges have been added, the graph was disconnected and no spanning tree exists — silently returning whatever partial tree was built, without checking `edgesUsed.size() == n - 1`, produces a plausible-looking but incomplete and misleadingly-labeled "MST." *Avoid:* always check the final edge count against `n - 1` before trusting the result.
- **Assuming Kruskal's and Prim's always pick the exact same edges.** They always reach the same *total weight*, but when multiple edges share the same weight, the two algorithms' different traversal orders can select different — equally valid — sets of edges. *Avoid:* if a specific tree structure (not just the total weight) matters downstream, do not assume the two algorithms are interchangeable.

## When To Use

- **The question is "minimum total cost to connect everything," not "cheapest path from A to B."**
- **The graph is undirected** and every edge can be traversed either way at the same cost.
- **A flat edge list is already available and the graph is sparse** — reach for Kruskal's.
- **An adjacency-list representation is already available, or the graph is dense** — reach for Prim's.
- **The problem is a known MST-adjacent approximation** (Steiner Tree, metric TSP lower bounds, single-linkage clustering).

## When NOT To Use

- **The actual question is shortest path between specific nodes** — use [Dijkstra](../dijkstras-algorithm/) or [Bellman-Ford](../bellman-ford/) instead; an MST does not answer this question even though both involve minimizing a sum of edge weights.
- **The graph is directed and the directionality matters** — standard Kruskal's/Prim's assume undirected edges; a directed version needs a fundamentally different algorithm (minimum spanning arborescence).
- **You need the graph to remain connected after removing any single edge (redundancy)** — an MST is deliberately the *minimum* connected structure, with zero redundancy by construction; if fault tolerance matters, a different design (e.g., a 2-edge-connected subgraph) is needed.
- **You only need to know whether the graph is connected at all, with no weight consideration** — plain [Union Find](../union-find/) or a traversal ([Graph BFS/DFS](../graph-bfs-dfs/)) answers that more directly, without sorting or a heap.

## Where This Shows Up

MST algorithms are the standard technique behind **network design and infrastructure planning** — telecom fiber layout, power grid distribution planning, and pipeline routing all use MST or MST-derived algorithms to find the cheapest way to connect a fixed set of locations. In **computational biology**, single-linkage hierarchical clustering is literally Kruskal's algorithm run partway: repeatedly merge the two closest clusters (the cheapest available edge) until the desired number of clusters remains, stopping before the full spanning tree completes. In **circuit design and VLSI layout**, MST-based algorithms provide a starting point for Steiner tree approximations used to minimize the total wire length connecting a set of components on a chip. In **approximation algorithms**, MST is the standard lower-bound and construction tool for the metric Traveling Salesman Problem — the classic `2`-approximation algorithm for metric TSP builds an MST first, then converts it into a tour, exploiting the fact that any valid TSP tour is at least as expensive as the graph's MST.

Five realistic ideas for your own backend/systems work:

1. **Designing the cheapest internal network topology** connecting a fixed set of data centers or edge nodes, given the known cost of laying a direct link between any two of them.
2. **Building a service-consolidation recommendation**: given the cost of migrating data between any two of several legacy databases, find the cheapest sequence of migrations that eventually consolidates everything into one connected system, without doing redundant migrations.
3. **Clustering similar records or entities** (deduplication, fraud-ring detection) by treating pairwise similarity as an inverted edge weight and running Kruskal's partway, stopping once a target number of clusters remains — literal single-linkage clustering.
4. **Computing a baseline lower bound for a routing/delivery cost estimate**, using an MST over delivery stops as a quick, cheap-to-compute floor on the true optimal tour length, before running a more expensive exact or heuristic routing algorithm.
5. **Designing a minimal-cost internal dependency-sharing layer** for a monorepo's build graph, where "connecting" two modules through a shared library has a cost (build time, coupling risk) and the goal is the cheapest set of shared dependencies that still lets every module reach any shared utility it needs.

## Similar Patterns

- **Union Find** ([../union-find/](../union-find/)): the exact mechanism Kruskal's uses for its cycle check — connectivity without traversal, in near-`O(1)` per query. Every Kruskal's implementation is, structurally, a Union Find application with one added rule (process edges cheapest-first).
- **Dijkstra's Algorithm** ([../dijkstras-algorithm/](../dijkstras-algorithm/)): superficially similar to Prim's (both use a min-heap growing outward from a start node), but answers a genuinely different question — Dijkstra's heap key is *total distance from the source so far*; Prim's heap key is *the single edge weight* connecting the frontier to a new node. Confusing the two heap keys is the most common way to accidentally implement one algorithm while believing you wrote the other.
- **Graph BFS/DFS** ([../graph-bfs-dfs/](../graph-bfs-dfs/)): answers pure connectivity/traversal questions with no notion of edge weight at all — the right tool when the question is "can these be connected," not "what is the cheapest way to connect them."
- **Bellman-Ford** ([../bellman-ford/](../bellman-ford/)) **and Floyd-Warshall** ([../floyd-warshall/](../floyd-warshall/)): both solve shortest-path questions, which remain a different problem from MST even when the same graph and the same edge weights are involved.

| Pattern | Question answered | Mechanism | Time |
|---|---|---|---|
| Kruskal's MST | Cheapest way to connect everything | Sort edges + Union Find | `O(E log E)` |
| Prim's MST | Cheapest way to connect everything | Grow one tree + min-heap | `O(E log V)` |
| Dijkstra | Cheapest path from one source to everywhere | Grow finalized distances + min-heap | `O((V+E) log V)` |
| Union Find alone | Are these two nodes connected at all | Disjoint-set roots | `~O(1)` per query |

## Interview Discussion

Experienced engineers rarely need convincing that sorting-then-filtering or growing-a-frontier both work — what they actually probe is whether you can **state the exchange argument that makes either algorithm provably optimal**, and whether you can immediately distinguish an MST question from a shortest-path question when a problem statement is ambiguous about which one is being asked.

Common follow-up questions:
- *"Prove that Kruskal's is correct — why does taking the globally cheapest edge first always work?"* — expects the exchange argument: if the cheapest edge were excluded from every MST, some MST would form a cycle upon adding it, and swapping out any other cycle edge for this cheaper one would produce an equally-or-more optimal tree, a contradiction.
- *"When would you choose Prim's over Kruskal's, or vice versa?"* — expects a real answer about graph density and available representation (adjacency list vs. flat edge list), not "they're basically the same" — expects naming the actual complexity difference (`E log E` vs. `E log V`) and when it matters.
- *"Is the minimum spanning tree unique?"* — expects recognizing that it is unique only if all edge weights are distinct; ties allow multiple equally-valid MSTs with the same total weight but different specific edges.
- *"How would you find the MAXIMUM spanning tree instead?"* — expects recognizing that negating every edge weight (or simply sorting descending / using a max-heap) reduces the maximum-spanning-tree problem directly to the minimum one, since the exchange argument does not care about the sign of the comparison.
- *"What if the graph is disconnected? What does your algorithm return?"* — expects naming that no spanning tree exists, and describing exactly how the implementation should detect and report that (fewer than `n - 1` edges used) rather than silently returning a partial forest labeled as if it were complete.

Common misconceptions:
- **"An MST minimizes the distance between the two farthest nodes."** It does not — that is closer to a different, harder problem. An MST minimizes the *sum* of edge weights used, with no guarantee about any specific pair's resulting path length within the tree.
- **"Prim's and Dijkstra's are basically the same algorithm."** They share the "min-heap growing outward" shape but differ in exactly what the heap key represents — Dijkstra's key is cumulative distance from the source; Prim's key is a single edge's weight. Swapping one implementation's heap key logic into the other silently produces the wrong algorithm's answer.
- **"Kruskal's needs an adjacency list."** It does not — it needs only a flat list of edges, which is precisely what makes it convenient when edges arrive from an external source (a CSV of connections, a list of API-reported links) with no adjacency structure built yet.
- **"The MST is the cheapest way to visit every node" (confusing it with TSP).** An MST connects every node with a tree (branching structure allowed, no requirement to visit nodes in any particular order or return to the start); a TSP tour visits every node exactly once in a single cycle. They are related (MST gives a lower bound for metric TSP) but are not the same object.

## Key Takeaways

1. An MST minimizes the **total** cost to connect every node — a fundamentally different question from shortest path, which minimizes the cost to reach one node from another.
2. Both Kruskal's and Prim's rest on the same exchange argument: the cheapest available edge that does not create a cycle can always be added safely to a growing minimum spanning tree.
3. Kruskal's applies this globally (sort all edges once, filter via Union Find); Prim's applies it locally (grow one tree, always taking the cheapest frontier edge via a min-heap).
4. Recognition signal: "minimum cost to connect everything," "cheapest network," or a phrasing with no privileged starting node — as soon as a specific source/destination pair matters, this is the wrong pattern.
5. Kruskal's is `O(E log E)`, dominated by the sort; Prim's is `O(E log V)` with a binary heap — the choice between them is about graph representation and density, not correctness.
6. The most common conceptual bug is confusing an MST with a shortest-path tree — they can disagree on the exact same graph.
7. Kruskal's cycle check must use `find(a) == find(b)`, never a direct `parent_[a] == parent_[b]` comparison, since neither may point straight at its root.
8. Prim's must discard stale heap entries (`if (inTree[to]) continue;`) — a node can be pushed multiple times as different frontier edges reach it.
9. If the graph is disconnected, no spanning tree exists — check the final edge count against `n - 1` rather than silently returning an incomplete forest.
10. An MST is not unique when edge weights tie — Kruskal's and Prim's always agree on the total weight, but may select different, equally valid sets of edges.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — the standard formal treatment of both Kruskal's and Prim's, including the full matroid-theoretic generalization of the exchange argument.
- *Algorithms* (Sedgewick & Wayne) — covers both algorithms with a strong emphasis on the Union-Find implementation details Kruskal's depends on.

**Reference**
- Wikipedia — "Minimum spanning tree," including the Chu-Liu/Edmonds' algorithm for the directed-graph generalization (minimum spanning arborescence) this module does not cover.
- Wikipedia — "Borůvka's algorithm," a third, less commonly taught MST algorithm that runs multiple rounds of "every component picks its cheapest outgoing edge simultaneously" — worth knowing exists, since it parallelizes more naturally than either Kruskal's or Prim's.

**Explainers**
- NeetCode — minimum spanning tree videos covering Kruskal's and Prim's side by side with the same worked example.
