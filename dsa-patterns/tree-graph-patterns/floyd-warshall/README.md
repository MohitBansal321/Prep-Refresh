# Floyd-Warshall

> **In one line:** for every candidate "waypoint" node `k`, check whether routing every pair `(i, j)` through `k` beats the current best `i → j` — after `k` has ranged over all `n` nodes, every shortest path between every pair has been found.

```cpp
for (int k = 0; k < n; ++k) {
  for (int i = 0; i < n; ++i) {
    if (dist[i][k] == kInf) continue;
    for (int j = 0; j < n; ++j) {
      if (dist[k][j] == kInf) continue;
      if (dist[i][k] + dist[k][j] < dist[i][j]) {   // <-- the whole algorithm is this line
        dist[i][j] = dist[i][k] + dist[k][j];
      }
    }
  }
}
```

**O(V³)** time · **O(V²)** space. Full runnable version, with negative-cycle detection: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---

## Intent

Compute the shortest distance between **every pair** of nodes in a graph — not just from one fixed source — including graphs with negative edge weights, in a single `O(V³)` pass that also detects whether any negative cycle exists anywhere in the graph.

## Real Life Analogy

Picture a small regional airline building its complete fare table: for every one of its `n` airports, it wants the cheapest total fare to fly to every *other* airport, including routes that connect through one or more layover cities. A naive approach would run a full single-source shortest-path search separately from each of the `n` airports — `n` independent trips through the whole route network. Floyd-Warshall instead asks a completely different, more economical question: one airport at a time, "if flights are now allowed to route through **this** airport as a layover, does any pair of cities get a cheaper combined fare?" After considering every airport once as a potential layover, every pair's cheapest fare — through any combination of layovers — has been found, without ever having explicitly searched from any single airport as a "starting point."

The analogy breaks down in one place: a real airline's layover options are constrained by scheduling (a 6am flight cannot lay over at an airport whose connecting flight already departed), while Floyd-Warshall's graph has no such timing constraint — an edge is available whenever the algorithm wants to use it. Real flight-fare aggregators must additionally solve a time-respecting version of this problem, which this algorithm does not model.

## Problem

A large class of problems needs distances between **every** pair of nodes, not one fixed source:

- **Building a complete distance/fare/latency matrix** — a routing table, a fare chart, an inter-service latency matrix — where any node might later be queried as either the source or the destination.
- **Transitive closure and reachability questions** phrased as "can node A eventually reach node B, possibly through several intermediate steps," which is the `0`/`1`-weight special case of all-pairs shortest paths.
- **Detecting a negative cycle anywhere in the graph**, not just one reachable from a single chosen source — a strictly more thorough check than [Bellman-Ford](../bellman-ford/)'s single-source version.

> **Term: all-pairs shortest path.** The `dist[i][j]` value for every ordered pair `(i, j)`, not just the distances from one fixed source — an `n × n` table instead of a length-`n` array.

Running a single-source algorithm (Dijkstra or Bellman-Ford) from every one of the `n` nodes would answer the same question, but at a real, avoidable cost in code complexity: `n` separate runs, `n` separate distance arrays to manage, and — if any edge is negative — `n` separate `O(V·E)` Bellman-Ford calls, giving `O(V²·E)` overall. Floyd-Warshall answers the identical question with one triple-nested loop, no per-source bookkeeping, and a complexity bound (`O(V³)`) that depends only on the number of nodes, never on the number of edges.

## Solution

Maintain an `n × n` distance matrix, initialized so that `dist[i][i] = 0`, `dist[i][j]` is the direct edge weight if one exists, and infinity otherwise. Then, for each node `k` from `0` to `n - 1` in turn, treat `k` as a candidate intermediate waypoint: for every pair `(i, j)`, check whether routing `i → k → j` beats the currently-known best `i → j`, and update `dist[i][j]` if so.

The correctness argument is an induction on which nodes have been considered as waypoints so far. After `k = 0` has been processed, `dist[i][j]` is correct for every path from `i` to `j` that uses *at most* node `0` as an intermediate stop. After `k = 0` and `k = 1` have both been processed, `dist[i][j]` is correct for every path using *at most* `{0, 1}` as intermediate stops. By induction, once `k` has ranged over every node `0..n-1`, `dist[i][j]` is correct for a path using *any* subset of the graph's nodes as intermediate stops — which describes every path that could possibly exist between `i` and `j`.

## Architecture

Two participants, deliberately simpler than either single-source algorithm's:

1. **The distance matrix.** An `n × n` table, not a length-`n` array — this is the structural difference from Dijkstra and Bellman-Ford, both of which track distance *from one source*. Every cell `dist[i][j]` is independently meaningful and independently updated.
2. **The waypoint loop order.** `k` must be the **outermost** of the three nested loops, never `i` or `j`. This is not a style choice: the correctness induction above depends on `dist[i][k]` and `dist[k][j]`, read inside the innermost check, already reflecting every waypoint considered in *earlier* iterations of the `k` loop — if `k` were not outermost, some `dist[i][k]` reads would happen before waypoint `k`'s own turn to be considered, silently using a stale, unimproved value.

Running it: initialize the matrix as above. For `k` from `0` to `n - 1`: for every `i`, skip if `dist[i][k]` is still infinity (no path to the waypoint yet); otherwise for every `j`, skip if `dist[k][j]` is infinity, and otherwise relax `dist[i][j]` against `dist[i][k] + dist[k][j]`. After the full triple loop, check every `dist[i][i]` — if any has dropped below zero, a negative cycle exists somewhere in the graph, reachable from and back to node `i`.

## Why Not Other Approaches?

**"Run Dijkstra from every node."** This works and is asymptotically faster on sparse graphs when all weights are non-negative: `V` runs of `O((V+E) log V)` gives `O(V·(V+E) log V)` overall, which beats `O(V³)` when `E` is small relative to `V²`. The tradeoff is code complexity (managing `V` separate heap-based runs and their results) and a hard requirement that survives from Dijkstra itself: no negative edges, anywhere, on any run.

**"Run Bellman-Ford from every node."** This tolerates negative weights, matching Floyd-Warshall's capability, but costs `O(V²·E)` overall — worse than Floyd-Warshall's `O(V³)` on any reasonably dense graph (once `E` approaches `V²`, `V²·E` approaches `V⁴`). It is also strictly more code: managing `V` separate relaxation loops and merging their negative-cycle-detection results is more bookkeeping than Floyd-Warshall's single triple loop, which detects a negative cycle anywhere in the graph as a byproduct of computing every distance, not as a separate per-source check.

**"BFS from every node, if the graph happens to be unweighted."** Correct and faster (`O(V·(V+E))`) for the special case where every edge costs exactly 1, but it does not generalize the moment any edge carries a real weight — reach for it only when the unweighted assumption is guaranteed to hold.

**Tradeoff summary:** Floyd-Warshall's `O(V³)` is a **fixed** cost depending only on the node count — it does not improve on a sparse graph the way `V` runs of Dijkstra or Bellman-Ford would, but it also never gets *worse* than that bound regardless of how dense the graph becomes, and it is the simplest of the three to implement correctly (three nested loops, one comparison, no heap, no per-source setup). Choose it when the graph is small-to-medium and dense enough that its fixed `O(V³)` cost is competitive with — or simply easier to reason about than — running a single-source algorithm `V` times.

## Diagrams

- **Recognition** — [images/recognition-diagram.md](images/recognition-diagram.md), the flowchart distinguishing Floyd-Warshall from Dijkstra, Bellman-Ford, and BFS, based on how many source nodes are needed and whether negative weights are possible.
- **Flow** — [images/flow-diagram.md](images/flow-diagram.md), the control flow of the triple-nested waypoint loop, with the outer-`k` ordering requirement called out explicitly.
- **Trace** — [images/trace-diagram.md](images/trace-diagram.md), a step-by-step trace of the distance matrix after each value of `k`, on the 4-node graph asserted in [code.cpp](code.cpp), showing exactly which cells improve at which waypoint.

## The Code

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the waypoint-relaxation mechanic clearly before the problem-specific solutions in [problems/](problems/).

**`floydWarshall`** (in [code.cpp](code.cpp)). Takes the node count and a flat edge list. Initializes an `n × n` matrix with `0` on the diagonal, the cheapest direct edge weight where one exists (handling parallel edges by keeping the minimum), and infinity elsewhere. The triple loop places `k` outermost, exactly as the correctness argument requires; the two `if (... == kInf) continue;` guards inside skip any `i`/`j` pair that cannot yet reach the waypoint, avoiding an addition against the infinity sentinel that could otherwise silently overflow or misbehave. After the loop, a single pass checks every `dist[i][i]` — any value that has dropped below zero proves a negative cycle passes through node `i`.

**`main()`** (in [code.cpp](code.cpp)). Exercises `floydWarshall` against a classic 4-node graph (verifying several multi-hop shortest distances, including one — `dist[2][0]` — where the two-hop route `2 → 3 → 0` beats the direct edge, which only the waypoint relaxation discovers), a graph with a negative edge but no cycle, a graph with a genuine negative cycle, disconnected node pairs, a single-node graph, and parallel edges between the same pair of nodes. Prints `[PASS]`/`[FAIL]` per assertion.

**Files in [problems/](problems/).** Each file defines its own graph representation rather than including `code.cpp`, so it compiles and reads independently — see [problems/README.md](problems/README.md) for the index.

## Tradeoffs

**What the waypoint-relaxation approach buys you**

- **One `dist[][]` table answers every "shortest path between X and Y" query afterward in O(1)**, no matter which two nodes are asked about — a genuine advantage over re-running a single-source algorithm every time a new source is queried.
- **Tolerates negative weights**, matching Bellman-Ford's generality, without needing a separate per-source relaxation loop.
- **Detects a negative cycle anywhere in the graph**, not just one reachable from a single chosen source — strictly more thorough than Bellman-Ford's single-source check.
- **Extremely simple to implement correctly.** Three nested loops and one comparison; no heap, no edge-list-vs-adjacency-list tradeoff to reason about, no per-source initialization to repeat.

**What it costs you**

- **`O(V³)` time regardless of how sparse the graph is.** A graph with very few edges pays the same cost as a nearly-complete graph — there is no way to exploit sparsity the way an edge-list-based algorithm like Bellman-Ford naturally does.
- **`O(V²)` space for the full distance matrix**, which becomes a real constraint well before `O(V³)` time does on genuinely large graphs — a graph with 100,000 nodes needs 10 billion matrix cells, long before `O(V³)` time becomes the binding constraint.
- **No way to answer "just one source" more cheaply.** If only one source's distances are ever needed, running Floyd-Warshall computes and discards `n - 1` sources' worth of unneeded results.

**Versus running Dijkstra or Bellman-Ford `V` times:** those approaches can be faster on genuinely sparse graphs (`E` much smaller than `V²`), because their cost scales with the number of edges actually present. Floyd-Warshall's `O(V³)` is a **ceiling**, not a target — it neither improves on sparsity nor degrades further on density, which makes it the simpler, safer default exactly when the graph is small enough that the distinction does not matter in practice.

## Complexity

**Time:** `O(V³)` — three nested loops, each ranging over all `V` nodes, with `O(1)` work in the innermost check.

**Space:** `O(V²)` for the distance matrix. The classic in-place formulation (used here) updates `dist[][]` directly rather than keeping a separate "previous iteration" copy, which is safe specifically because relaxing `dist[i][j]` using `dist[i][k]` and `dist[k][j]` during the *same* value of `k` can only ever use *already-correct* values for those two cells (neither can improve any further during this same `k`, since improving them would itself require routing through `k` — a self-reference the algorithm structurally avoids).

| Approach | Handles negative weights | Detects negative cycles | Source count | Time |
|---|---|---|---|---|
| Floyd-Warshall | Yes | Yes (anywhere in the graph) | All-pairs | `O(V³)` |
| Bellman-Ford ×V | Yes | Yes (per source) | All-pairs (via V runs) | `O(V²·E)` |
| Dijkstra ×V | No | No | All-pairs (via V runs) | `O(V·(V+E) log V)` |
| BFS ×V (unweighted only) | N/A | N/A | All-pairs (via V runs) | `O(V·(V+E))` |

## Common Mistakes

- **Putting `k` anywhere but the outermost loop.** Swapping the loop order (e.g., `i` outermost) does not crash — it silently produces a distance matrix that is only partially correct, because some pairs get relaxed using waypoint values that have not yet themselves been fully computed. *Avoid:* `k` must be the outermost of the three loops, always — this is the single detail that makes the algorithm correct.
- **Forgetting to initialize `dist[i][i] = 0`.** Without this, the diagonal stays at infinity, which breaks both ordinary path computation (any path that legitimately passes back through its own starting node) and the negative-cycle check entirely, since that check specifically looks for a diagonal value that has dropped *below* zero. *Avoid:* always seed the diagonal to `0` before the first relaxation pass, exactly as [code.cpp](code.cpp) does.
- **Not guarding against relaxing through an unreached waypoint.** Computing `dist[i][k] + dist[k][j]` when either is still the infinity sentinel can silently overflow or wrap, corrupting `dist[i][j]` with a nonsensical value instead of correctly leaving it unrelaxed. *Avoid:* skip any `i`/`k` or `k`/`j` pair still at the sentinel, exactly as [code.cpp](code.cpp)'s two `continue` guards do.
- **Reporting a negative cycle using `dist[src][src] < 0` for one specific `src`, then treating that as proof no other negative cycle exists.** A graph can have several disjoint negative cycles; checking only one node's self-distance tells you nothing about the others. *Avoid:* check every `dist[i][i]` across the whole loop, not just one.
- **Using Floyd-Warshall when only a single source's distances are needed.** This does not produce a wrong answer, but it silently wastes `O(V³)` time and `O(V²)` space computing `n - 1` sources' worth of results nobody asked for — a single Dijkstra or Bellman-Ford run would answer the actual question asked, faster.

## When To Use

- **You need shortest paths between every pair of nodes**, not just from one fixed source.
- **The graph is small-to-medium** — `O(V³)` and `O(V²)` space are comfortable up to roughly a few hundred to a couple thousand nodes, depending on the time/memory budget available.
- **Negative weights are possible**, and you need all-pairs results (if only single-source is needed, [Bellman-Ford](../bellman-ford/) alone suffices at a lower cost).
- **You need to detect whether a negative cycle exists anywhere in the graph**, not just one reachable from a specific chosen source.

## When NOT To Use

- **Only one source's distances are needed** — use [Dijkstra](../dijkstras-algorithm/) (non-negative weights) or [Bellman-Ford](../bellman-ford/) (possible negative weights) instead; both solve the strictly smaller problem you actually have, faster.
- **The graph is large and sparse** — `V` runs of Dijkstra (`O(V·(V+E) log V)`) or Bellman-Ford (`O(V²·E)`) can beat Floyd-Warshall's fixed `O(V³)` when `E` is meaningfully smaller than `V²`.
- **`O(V²)` memory for the full distance matrix is not available** — a graph with tens of thousands of nodes or more makes the matrix itself the binding constraint, well before the `O(V³)` runtime does.
- **The graph is a DAG with no negative weights of concern** — a topological-order pass from each source, or a specialized DAG all-pairs technique, avoids paying for waypoint consideration that a DAG's acyclic structure makes unnecessary.

## Where This Shows Up

Floyd-Warshall is the standard textbook algorithm for computing a **complete transitive closure** or **all-pairs reachability matrix** — the `0`/`1`-weight special case shows up directly in database query planners reasoning about join reachability, and in build-dependency systems checking whether any two targets are transitively connected. It is also the natural fit for **network diameter and centrality calculations**: many graph-analysis libraries (used in social-network analysis, transportation-network research, and biological pathway analysis) compute the full all-pairs distance matrix once with Floyd-Warshall specifically because so many downstream metrics (closeness centrality, graph diameter, eccentricity) need distances between arbitrary pairs, not just from one node. In interviews, LeetCode's "Find the City With the Smallest Number of Neighbors at a Threshold Distance" (1334) is one of the clearest signals for this pattern: the question needs, for every city, a count of how many *other* cities are reachable within a threshold — an inherently all-pairs question on a graph small enough (`n ≤ 100`) that `O(V³)` is the pragmatic, simplest-to-implement choice over `V` runs of Dijkstra.

Five realistic ideas for your own backend/systems work:

1. **Precomputing a complete inter-datacenter or inter-region latency matrix** for a routing/load-balancing service, so any two regions' round-trip cost is an O(1) lookup rather than a live probe or a per-query graph search.
2. **Building a "who can reach whom" access-control audit** over a permission-delegation graph (role A can grant role B, which can grant role C, …), computing full transitive reachability once rather than checking each pair on demand.
3. **Computing a complete fare or shipping-cost matrix** for a logistics platform with a fixed, small set of hubs, refreshed periodically as a batch job rather than queried live per request.
4. **Detecting circular, mutually-reinforcing configuration dependencies** across an entire service mesh's config-override graph in one pass, rather than checking reachability from one service at a time.
5. **Computing graph-wide centrality or diameter metrics** for a dependency-analysis dashboard, where the underlying algorithm (closeness centrality, eccentricity) genuinely needs the full all-pairs distance table, not distances from any one fixed node.

## Similar Patterns

- **Dijkstra's Algorithm** ([../dijkstras-algorithm/](../dijkstras-algorithm/)): single-source, faster per run (`O((V+E) log V)`), but requires non-negative weights and needs `V` separate runs to match Floyd-Warshall's all-pairs coverage.
- **Bellman-Ford** ([../bellman-ford/](../bellman-ford/)): single-source, tolerates negative weights and detects a negative cycle reachable from its one source — Floyd-Warshall is the natural generalization to "detect a negative cycle anywhere, and give me every pair's distance while you're at it."
- **Topological Sort** ([../topological-sort/](../topological-sort/)): on a DAG specifically, a single topological-order pass from each source computes single-source shortest paths in `O(V + E)`, and running it from every source is often still cheaper than Floyd-Warshall's fixed `O(V³)` on a sparse DAG.
- **Union Find** ([../union-find/](../union-find/)): answers a related but weaker question — "are these two nodes connected at all" — in near-`O(1)` per query, without computing an actual distance. Reach for Union Find when connectivity alone is the question; reach for Floyd-Warshall when the actual shortest distance matters.

| Pattern | Handles negative weights | Detects negative cycles | Source count | Time |
|---|---|---|---|---|
| Floyd-Warshall | Yes | Yes (anywhere) | All-pairs | `O(V³)` |
| Dijkstra | No | No | Single (×V for all-pairs) | `O((V+E) log V)` |
| Bellman-Ford | Yes | Yes (from one source) | Single (×V for all-pairs) | `O(V·E)` |
| Union Find | N/A (unweighted connectivity) | N/A | All-pairs (connectivity only) | `~O(1)` per query |

## Interview Discussion

Experienced engineers rarely need convincing that the triple loop is correct once they have seen the "waypoint" framing — what they actually probe is whether you understand **why `k` must be the outermost loop**, and whether you can reason precisely about when `O(V³)` beats running a single-source algorithm `V` times instead of reciting "it's for all-pairs" as a slogan.

Common follow-up questions:
- *"Why does `k` have to be the outermost loop? What breaks if it isn't?"* — expects the induction argument: `dist[i][k]` and `dist[k][j]`, read inside the innermost check, must already reflect every waypoint considered *before* `k`'s own turn — putting `k` innermost lets a pair get relaxed using a not-yet-finalized value for the current waypoint.
- *"When would you choose this over running Dijkstra V times?"* — expects a real complexity comparison (`O(V³)` vs. `O(V·(V+E) log V)`), not just "when you need all pairs" — the graph's density (how close `E` is to `V²`) is what actually decides which is faster.
- *"How do you detect a negative cycle, and does it matter which node you check?"* — expects naming that a negative cycle shows up as `dist[i][i] < 0` for the specific nodes it passes through, so checking only one node's self-distance misses cycles elsewhere in the graph.
- *"Can this be adapted to also recover the actual path, not just the distance?"* — expects proposing a parallel `next[i][j]` table recording which node to go to next from `i` when heading toward `j`, updated alongside `dist[i][j]` on every relaxation.
- *"What is the space cost, and when does it become the real bottleneck?"* — expects recognizing `O(V²)` space becomes binding well before `O(V³)` time does, on graphs with tens of thousands of nodes or more.

Common misconceptions:
- **"Floyd-Warshall is just Bellman-Ford run from every node."** The mechanism is genuinely different — Bellman-Ford relaxes *edges*; Floyd-Warshall relaxes *pairs through a waypoint*. They arrive at the same all-pairs answer, at different complexities (`O(V³)` vs. `O(V²·E)`), via structurally different loops.
- **"You need `V` separate calls to get all-pairs results, one per source."** That is exactly what Floyd-Warshall avoids — it is a single algorithm producing the full matrix in one triple loop, not a wrapper that repeats a single-source algorithm.
- **"`O(V³)` is always worse than running a faster single-source algorithm multiple times."** Only true on sparse graphs. On dense graphs (`E` approaching `V²`), `V` runs of Dijkstra approach `O(V³ log V)` and `V` runs of Bellman-Ford approach `O(V⁴)` — both worse than Floyd-Warshall's flat `O(V³)`.
- **"The in-place update (no separate 'previous k' copy) is a bug waiting to happen."** It is not — relaxing `dist[i][j]` using `dist[i][k]` and `dist[k][j]` during the same `k` iteration is safe precisely because neither of those two values could improve any further during this same `k` without routing through `k` itself, which the algorithm structurally never does.

## Key Takeaways

1. Floyd-Warshall computes shortest distances between **every pair** of nodes in one `O(V³)` pass, by treating each node in turn as a candidate waypoint for every pair.
2. The `k` loop (the waypoint) **must** be outermost — the correctness induction depends on `dist[i][k]` and `dist[k][j]` already reflecting every earlier waypoint when they are read.
3. Recognition signal: "all pairs," "distance/fare/latency matrix," "transitive closure," or "detect a negative cycle anywhere in the graph" (not just from one source).
4. Time is `O(V³)`; space is `O(V²)` for the full distance matrix — both are fixed costs depending only on node count, never on edge count.
5. A negative cycle shows up as `dist[i][i] < 0` for the specific nodes it passes through — check every diagonal entry, not just one.
6. The most common bug is putting `k` in the wrong loop position — this does not crash, it silently produces a partially-relaxed, wrong distance matrix.
7. The in-place update (no separate "previous iteration" copy) is intentional and safe, not an oversight — relaxing through waypoint `k` can never need a stale value of `dist[i][k]` or `dist[k][j]` from before `k`'s own turn.
8. Use Dijkstra or Bellman-Ford instead the moment only a single source's distances are actually needed — Floyd-Warshall computes `V` times more information than that question requires.
9. `V` runs of Dijkstra or Bellman-Ford can beat Floyd-Warshall's fixed `O(V³)` on genuinely sparse graphs; Floyd-Warshall wins as the graph gets denser or when simplicity of implementation matters more than squeezing out the last constant factor.
10. LeetCode's clearest signal for this pattern is a small graph (`n` in the low hundreds) with a genuinely all-pairs question — "how many cities are reachable from every city within a threshold" is the canonical shape.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — the standard formal treatment of Floyd-Warshall, including the full dynamic-programming correctness proof and the path-reconstruction extension.
- *Algorithms* (Sedgewick & Wayne) — covers Floyd-Warshall alongside transitive closure and its relationship to matrix multiplication-style algorithms.

**Reference**
- LeetCode — Find the City With the Smallest Number of Neighbors at a Threshold Distance (1334).
- Wikipedia — "Floyd–Warshall algorithm," including the path-reconstruction variant and its relationship to the Kleene algorithm for regular expressions (the same triple-loop shape appears in an entirely different domain).

**Explainers**
- NeetCode — shortest path algorithm comparison videos, contrasting Floyd-Warshall, Dijkstra, and Bellman-Ford side by side.
