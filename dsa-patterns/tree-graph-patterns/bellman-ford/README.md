# Bellman-Ford

> **In one line:** relax every edge, `V - 1` times, in any order — after that many passes, every shortest path (which uses at most `V - 1` edges) has been fully computed, no heap and no "process the closest node first" required.

```cpp
for (int pass = 0; pass < n - 1; ++pass) {
  bool changed = false;
  for (const Edge& e : edges) {
    if (dist[e.from] == kInf) continue;
    if (dist[e.from] + e.weight < dist[e.to]) {   // <-- the whole algorithm is this relax step
      dist[e.to] = dist[e.from] + e.weight;
      changed = true;
    }
  }
  if (!changed) break;   // converged early — nothing left to relax
}
```

**O(V·E)** time · **O(V)** space. Full runnable version, with negative-cycle detection: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---

## Intent

Find the shortest-cost path from a single source to every other node in a graph whose edges may carry **negative weights** — the case [Dijkstra's Algorithm](../dijkstras-algorithm/) cannot handle, because Dijkstra's correctness argument depends on "the closest unfinalized node can never get cheaper later," and a negative edge breaks that outright. Bellman-Ford also detects whether a **negative-weight cycle** is reachable from the source, in which case "shortest path" has no answer at all.

## Real Life Analogy

Picture a currency-arbitrage desk: each currency is a node, and each conversion (USD → EUR, EUR → JPY, …) is an edge whose weight is `-log(exchange rate)`, so that multiplying exchange rates along a path becomes *summing* edge weights along that path — a standard trick that turns "which sequence of trades maximizes value" into a shortest-path question. Some edges are genuinely negative: a trade can be profitable, which is a negative cost.

Now suppose three currencies form a loop — USD → EUR → GBP → USD — where converting all the way around actually gains you money instead of losing a small spread to fees. That loop is a **negative cycle**, and if you could take it for free and looped it a thousand times, you would have infinite money. That is exactly why Bellman-Ford refuses to report a "shortest path" through a reachable negative cycle: the real-world answer is "there is no shortest path, because you can always find a cheaper one by going around the loop one more time." The analogy breaks down in one place: a real trading desk cannot loop instantly and infinitely (fees, slippage, and execution time bound how many loops are actually profitable), but the graph-theoretic version has no such limit, which is precisely why the algorithm must detect the cycle rather than silently return an ever-decreasing number.

## Problem

A large class of shortest-path problems has edges that are not uniformly positive:

- **Financial and arbitrage graphs**, as above, where an edge can represent a gain (negative cost) as naturally as a loss.
- **Graphs built from differences or refunds** — "this transition costs `−5`" is a legitimate edge weight when the domain is a balance, a credit, or a correction, not a physical distance.
- **Detecting whether a system of constraints is even satisfiable.** A system of difference constraints (`x_j − x_i ≤ w`) maps directly onto a graph with an edge `i → j` of weight `w`; the constraints are satisfiable if and only if that graph has no negative cycle — Bellman-Ford answers both "is it satisfiable" and, if so, "here is a satisfying assignment" in one pass.

> **Term: negative cycle.** A cycle whose edge weights sum to a negative total. If such a cycle is reachable from the source, the notion of "shortest path" to every node the cycle can reach is undefined — you can always produce a cheaper path by looping the cycle one more time before continuing.

Dijkstra's greedy strategy — always finalize the closest not-yet-finalized node — assumes that once a node is popped with its current-best distance, no future relaxation can ever beat it. That assumption is exactly what a negative edge violates: a node popped early with a seemingly-final distance can later be beaten by a path that first takes a longer, more expensive route through a node with a large negative edge waiting on the other side. Bellman-Ford's fix is not a smarter selection rule; it is to **stop trying to select** and instead relax every edge, unconditionally, enough times that the order does not matter.

## Solution

Relax every edge in the graph, `V − 1` times, where `V` is the number of nodes. "Relax edge `u → v`" means: if `dist[u] + weight(u, v) < dist[v]`, then `dist[v]` improves to `dist[u] + weight(u, v)`. No edge is ever selected specially, no heap is involved, and edges can be processed in any fixed order, every pass.

The `V − 1` bound comes directly from what a shortest path can look like: a **simple path** (one that never revisits a node) between any two of the `V` nodes uses at most `V − 1` edges, because there are only `V − 1` "steps" available between `V` distinct nodes. Each full pass over all edges is guaranteed to extend every already-correct distance by at least one more edge's worth of progress — so after `V − 1` passes, every shortest simple path, regardless of which specific edges it uses, has been fully relaxed into `dist[]`.

A `V`-th pass then answers the negative-cycle question for free: if any edge can still relax after `V − 1` passes have already accounted for every simple path, the improvement it found *must* have come from a path using `V` or more edges — which is only possible if that path loops through a cycle, and the only reason looping through a cycle would ever help is if that cycle's total weight is negative.

## Architecture

Three participants, all simpler than Dijkstra's:

1. **The edge list.** Unlike Dijkstra's adjacency list (which needs fast "neighbors of this node" lookup because the algorithm visits nodes one at a time), Bellman-Ford only ever needs to iterate over *every edge*, in any fixed order, so a flat `vector<Edge>` is both sufficient and simplest.
2. **The distance array.** Identical role to Dijkstra's `dist[]` — the current best known distance to each node, initialized to infinity except the source (`0`). The difference is how it gets updated: Dijkstra updates it only for neighbors of the node currently being finalized; Bellman-Ford updates it for *every* edge, *every* pass, with no notion of a node being "finalized" until all passes complete.
3. **The pass counter and the `changed` flag.** The outer loop runs at most `V − 1` times, but [code.cpp](code.cpp)'s `bellmanFord` tracks whether any edge relaxed during a pass and exits early if not — a graph that converges in fewer passes (the chain example in `main()` converges in exactly 4, not `V − 1 = 4`, but a shorter chain would exit even earlier) does not pay for passes that can no longer change anything.

Running it: initialize `dist[src] = 0` and everything else to infinity. Repeat `V − 1` times: for every edge `(u, v, w)`, if `dist[u] + w < dist[v]`, update `dist[v]` and remember that this pass changed something; stop early if a pass changes nothing. Afterward, run one more pass over every edge — if anything still relaxes, a negative cycle is reachable from the source and every downstream distance is unreliable.

## Why Not Other Approaches?

**"Just run Dijkstra, but keep relaxing even after a node is popped."** This is the instinct every engineer has the first time they see a negative edge, and it does not work as a patch — it works only by becoming Bellman-Ford in disguise. Dijkstra's speed comes entirely from *trusting* that a popped node's distance is final so it never needs to be revisited; the moment you remove that trust to handle negative edges, the heap buys you nothing, because you are now potentially re-relaxing every node many times regardless of "distance so far" order. You end up doing strictly more bookkeeping (heap operations) to arrive at the same `O(V·E)` result Bellman-Ford gets with a flat array and no heap at all.

**"Detect negative cycles by checking for revisits during a DFS."** A DFS that notices "I have returned to a node already on my current path" does find *some* cycle, but it tells you nothing about whether that cycle's total weight is negative, and it does not compute any shortest distances along the way. You would need Bellman-Ford's relaxation logic anyway to know whether the cycle you found actually makes distances unbounded — the DFS cycle check answers a different, weaker question.

**"Floyd-Warshall, since it also handles negative edges."** [Floyd-Warshall](../floyd-warshall/) handles negative edges too and additionally detects negative cycles, but it computes **all-pairs** shortest paths in `O(V³)` — solving a strictly harder problem than "single source" at a strictly worse complexity when you only needed one source. Reach for Bellman-Ford when you have one source; reach for Floyd-Warshall only when you genuinely need every pair.

**Tradeoff summary:** Dijkstra is faster (`O((V+E) log V)`) but requires non-negative weights, full stop — there is no partial-credit version of Dijkstra that tolerates "a few" negative edges. Bellman-Ford is slower (`O(V·E)`) but works unconditionally on any edge weights, and its negative-cycle detection is not an add-on — it is a one-line consequence of the same relaxation loop already being run for the shortest-path computation itself.

## Diagrams

- **Recognition** — [images/recognition-diagram.md](images/recognition-diagram.md), the flowchart distinguishing Bellman-Ford from Dijkstra, Floyd-Warshall, and a plain topological-order relaxation, based on whether negative weights are possible and how many source nodes are needed.
- **Flow** — [images/flow-diagram.md](images/flow-diagram.md), the control flow of the `V − 1`-pass relax loop plus the `V`-th negative-cycle-detection pass.
- **Trace** — [images/trace-diagram.md](images/trace-diagram.md), a step-by-step trace of `dist[]` across every pass on the 5-node graph asserted in [code.cpp](code.cpp), including the pass where the negative edge `1 → 3` (weight `-4`) improves `dist[3]` from infinity to `2`.

## The Code

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the relax-loop mechanic clearly before the problem-specific solutions in [problems/](problems/).

**`bellmanFord`** (in [code.cpp](code.cpp)). Takes the node count, a source index, and a flat `vector<Edge>`. Initializes `dist[src] = 0` and everything else to `kInf`. The outer loop runs up to `n - 1` times; each pass walks every edge and applies the relax check `dist[e.from] + e.weight < dist[e.to]`, skipping edges whose source side is still unreached (`dist[e.from] == kInf`) to avoid computing `kInf + weight`, which would silently wrap or misbehave. A `changed` flag lets the loop exit as soon as a pass relaxes nothing, since no later pass can relax anything either once that happens. After the main loop, one extra pass over every edge checks whether anything *still* relaxes — if so, `hasNegativeCycle` is set and the caller is told every returned distance may be unreliable.

**`main()`** (in [code.cpp](code.cpp)). Exercises `bellmanFord` against a graph with a negative edge but no negative cycle (verifying exact distances, including one path that only becomes cheapest *because* of the negative edge), a graph with a negative cycle reachable from the source (verifying detection), a graph with a negative cycle that is *not* reachable from the source (verifying no false positive — the single most important edge case for this function), a disconnected node, a single-node graph with no edges, and a chain that converges before `V − 1` passes complete (verifying the early-exit `changed` flag does not produce a wrong, half-relaxed answer). Prints `[PASS]`/`[FAIL]` per assertion.

**Files in [problems/](problems/).** Each file defines its own `Edge`/graph representation rather than including `code.cpp`, so it compiles and reads independently — see [problems/README.md](problems/README.md) for the index.

## Tradeoffs

**What the relax-everything approach buys you**

- **Works unconditionally on any edge weights**, including negative ones, with no precondition to verify before running it.
- **Detects negative cycles as a byproduct** of the same loop already being run — no separate algorithm needed.
- **No heap, no adjacency-list-with-fast-neighbor-lookup requirement.** A flat edge list is enough, which makes it trivial to feed edges from any source (a stream, a difference-constraint system, a list of currency conversions) without building a full adjacency structure first.
- **Simple to parallelize per pass.** Because every edge in a single pass reads distances computed by the *previous* pass, all edges within one pass can, in principle, be relaxed independently before the next pass begins — a property Dijkstra's inherently sequential "pop the next closest node" loop does not share.

**What it costs you**

- **`O(V·E)` time**, strictly worse than Dijkstra's `O((V+E) log V)` on graphs where all weights happen to be non-negative — you pay for generality you may not need.
- **No early exit toward a single target.** Dijkstra can stop the moment its target node is popped; Bellman-Ford's guarantee only holds after a full pass completes, so there is no meaningful way to stop early when you only care about one destination (short of the `changed`-flag convergence check, which still requires completing whole passes).
- **Negative cycle detection tells you a cycle exists, not which distances it corrupted.** [code.cpp](code.cpp) returns one boolean; identifying exactly *which* nodes have an unreliable distance requires an additional pass propagating "unreliable" outward from every edge that still relaxes on the `V`-th pass (see Common Mistakes).

**Versus Dijkstra directly:** Dijkstra is asymptotically faster and should always be preferred when every edge weight is verified non-negative; Bellman-Ford is the fallback the moment that verification fails, and there is no middle ground — a single negative edge anywhere in the graph is enough to make Dijkstra's output untrustworthy.

## Complexity

**Time:** `O(V·E)` — `V - 1` passes, each visiting every one of the `E` edges once, plus one final pass for cycle detection.

**Space:** `O(V)` for the distance array, plus `O(E)` for the edge list itself (which the caller typically already has).

| Approach | Handles negative weights? | Detects negative cycles? | Time |
|---|---|---|---|
| Bellman-Ford | Yes | Yes | `O(V·E)` |
| Dijkstra | No | No (undefined behavior if present) | `O((V+E) log V)` |
| Floyd-Warshall (all-pairs) | Yes | Yes | `O(V³)` |
| Plain BFS (unweighted) | N/A (assumes weight 1) | N/A | `O(V + E)` |

## Common Mistakes

- **Stopping after `V − 1` passes without checking for a negative cycle.** If the graph might contain one, skipping the `V`-th pass does not crash — it silently returns distances that look like plausible numbers but are actually meaningless, because the "shortest path" they claim to represent does not exist. *Avoid:* always run the extra pass and check its return value before trusting any distance.
- **Marking nodes "visited" and skipping them, Dijkstra-style.** Bellman-Ford has no notion of a node being "finalized" until all `V − 1` passes complete — a node's distance can legitimately improve on pass 3 after having already improved on pass 1. Adding a Dijkstra-style visited guard silently produces wrong, too-large distances by refusing valid later improvements. *Avoid:* every pass must re-examine every edge unconditionally; there is no early "this node is done" state.
- **Forgetting to guard against relaxing from an unreached node.** Computing `dist[e.from] + e.weight` when `dist[e.from]` is still the sentinel "infinity" value can silently wrap around (integer overflow) or produce a nonsensical negative "improvement," corrupting `dist[e.to]` with garbage. *Avoid:* skip any edge whose source side is still at the infinity sentinel, exactly as [code.cpp](code.cpp)'s `if (dist[e.from] == kInf) continue;` does.
- **Processing edges in a different order each pass and assuming it changes correctness.** It does not — the algorithm's correctness proof never assumes a particular edge order, only that every edge is examined every pass. Assuming a specific order is *required* is a common but harmless misconception that sometimes leads to writing needless, complicated edge-sorting logic.
- **Confusing "no negative cycle reachable from the source" with "no negative cycle in the graph at all."** A negative cycle that the source cannot reach is invisible to (and correctly ignored by) this algorithm — see [code.cpp](code.cpp)'s third test case. Reporting a global "does this graph contain any negative cycle anywhere" answer requires running the check from every node, or reasoning about connectivity separately.

## When To Use

- **Edge weights can be negative** — refunds, gains, differences, or corrections modeled as costs.
- **You need to detect whether a negative cycle is reachable from a source** — arbitrage detection, feasibility of a system of difference constraints, or validating that a graph of dependencies with "credit" edges cannot be exploited infinitely.
- **The graph is small-to-medium and only one source matters** — `O(V·E)` is perfectly fine when `V` and `E` are not enormous and you do not need all-pairs results.
- **Edges arrive as a flat list rather than a pre-built adjacency structure** — Bellman-Ford's edge-list-only requirement fits naturally.

## When NOT To Use

- **All edge weights are verified non-negative** — use [Dijkstra's Algorithm](../dijkstras-algorithm/) instead; it is asymptotically faster and there is no correctness reason to pay Bellman-Ford's `O(V·E)` cost.
- **You need shortest paths between every pair of nodes** — use [Floyd-Warshall](../floyd-warshall/) instead; running Bellman-Ford from every node costs `O(V²·E)`, worse than Floyd-Warshall's `O(V³)` on any reasonably dense graph.
- **The graph is a DAG** — a single topological-order relaxation pass computes shortest paths in `O(V + E)` even with negative weights (as long as there are no cycles at all, negative or otherwise), with no need for multiple passes.
- **All edges cost exactly 1 (unweighted)** — plain BFS gets the same answer in `O(V + E)`, with no relaxation loop needed.

## Where This Shows Up

Bellman-Ford's headline production use is arbitrage detection in currency and cryptocurrency trading systems: build a graph where each edge weight is `-log(exchange rate)`, and a negative cycle reachable from any starting currency is a literal, exploitable arbitrage loop — this is a real, checkable technique used in quantitative trading infrastructure, not a textbook curiosity. It is also the standard algorithm behind **distance-vector routing protocols** like RIP (Routing Information Protocol), where each router only knows its direct neighbors' costs and iteratively relaxes its routing table pass by pass, exactly mirroring Bellman-Ford's edge-relaxation loop distributed across machines instead of run centrally. In constraint satisfaction, a **system of difference constraints** (`x_j - x_i <= w` for each constraint) maps directly onto a graph, and Bellman-Ford both tests satisfiability and, when satisfiable, produces a valid assignment via its output distances — a technique used in scheduling and timing-verification tools. In interviews, Cheapest Flights Within K Stops (LeetCode 787) is frequently solved with a **K-bounded variant** of Bellman-Ford (running only `K + 1` relaxation passes instead of `V - 1`) rather than augmented-state Dijkstra, precisely because bounding the number of passes maps naturally onto bounding the number of edges (stops) allowed.

Five realistic ideas for your own backend/systems work:

1. **Detecting circular, self-reinforcing discount or credit chains** in a promotions engine — if applying promotion A unlocks promotion B which unlocks A again at a net gain, that is a negative cycle in a graph of promotion transitions, and Bellman-Ford finds it before it is exploited.
2. **Validating a distributed scheduling system's timing constraints** ("service B must start at least 5 minutes after service A, service C at most 10 minutes after B, …") by modeling them as difference constraints and checking for a negative cycle, which would mean the schedule is infeasible.
3. **Building a simple distance-vector-style route recalculation** for an internal service mesh where each node only knows its direct neighbors' latencies, propagating updates pass by pass rather than requiring global topology knowledge (as Dijkstra effectively does).
4. **Fraud detection over a graph of financial transfers with fee rebates**, where some edges represent net-positive transfers (rebates larger than fees) — a reachable negative cycle signals a wash-trading or self-dealing loop worth flagging.
5. **Feasibility-checking a chain of currency or loyalty-point conversions** in a rewards platform, to ensure no sequence of conversions lets a user manufacture points or currency out of nothing.

## Similar Patterns

- **Dijkstra's Algorithm** ([../dijkstras-algorithm/](../dijkstras-algorithm/)): solves the same single-source shortest-path problem, faster, but only when every edge weight is non-negative. Bellman-Ford is the unconditional fallback the moment that assumption cannot be guaranteed.
- **Floyd-Warshall** ([../floyd-warshall/](../floyd-warshall/)): also tolerates negative weights and also detects negative cycles, but computes all-pairs shortest paths in `O(V³)` rather than single-source in `O(V·E)` — reach for it only when every pair of distances is actually needed.
- **Topological Sort** ([../topological-sort/](../topological-sort/)): on a DAG specifically (no cycles at all, negative or positive), a single topological-order relaxation pass computes shortest paths in `O(V + E)`, faster than Bellman-Ford's repeated passes, because the ordering already guarantees predecessors are processed before successors.
- **Graph BFS/DFS** ([../graph-bfs-dfs/](../graph-bfs-dfs/)): the unweighted special case of shortest-path — BFS finds fewest-hops paths in `O(V + E)` with no relaxation loop needed when every edge costs exactly 1.

| Pattern | Handles negative weights | Detects negative cycles | Source count | Time |
|---|---|---|---|---|
| Bellman-Ford | Yes | Yes | Single | `O(V·E)` |
| Dijkstra | No | No | Single | `O((V+E) log V)` |
| Floyd-Warshall | Yes | Yes | All-pairs | `O(V³)` |
| Topological-order relax (DAG only) | Yes (no cycles at all) | N/A (DAG has none) | Single | `O(V + E)` |

## Interview Discussion

Experienced engineers rarely need to be walked through the relax loop itself — that is four lines. What they probe is whether you understand **why `V − 1` passes is the exact right bound**, not a heuristic, and whether you can explain the negative-cycle detection as a direct consequence of that bound rather than as a separately-memorized add-on step.

Common follow-up questions:
- *"Why exactly `V − 1` passes, not `V` or `log V`?"* — expects the simple-path argument: a shortest simple path between any two of `V` nodes uses at most `V − 1` edges, and each pass guarantees one more edge's worth of correctness is folded in.
- *"Can you bound the number of stops/edges in the answer? How does that change the algorithm?"* — expects recognizing that running only `K` (rather than `V − 1`) passes computes shortest paths using at most `K` edges, which is exactly the technique behind Cheapest Flights Within K Stops (LeetCode 787).
- *"Why can't you just use a `visited` array like Dijkstra to skip finalized nodes?"* — expects naming that no node is ever "finalized" mid-run in Bellman-Ford; any node's distance can improve on any pass until the algorithm fully completes.
- *"How would you find which specific nodes are affected by a negative cycle, not just whether one exists?"* — expects proposing an additional propagation pass: mark every node whose edge still relaxes on the `V`-th pass, then propagate that "unreliable" marker outward through the graph (typically via one more BFS/DFS from all such marked nodes).
- *"What breaks if you run this on a graph with a negative cycle and just report `dist[]` after `V − 1` passes without checking?"* — expects naming that the returned distances would be plausible-looking numbers that are not actually shortest paths, silently wrong rather than crashing.

Common misconceptions:
- **"Bellman-Ford is just a slower Dijkstra."** It solves a strictly more general problem (negative weights, negative-cycle detection) that Dijkstra's algorithm is not correct for at all — it is not merely a slower version of the same guarantee, it is a different guarantee.
- **"A negative cycle anywhere in the graph makes the whole result invalid."** Only a negative cycle *reachable from the source* corrupts the result; a negative cycle in an unreachable part of the graph is correctly, silently ignored (see [code.cpp](code.cpp)'s third test case).
- **"You need a heap to make this efficient."** A heap would help Dijkstra select the next node to finalize; Bellman-Ford has no such selection step to accelerate — every edge in every pass must be examined regardless of any distance ordering, so a heap buys nothing here.
- **"Edges must be processed in a specific order (e.g., by weight, or in graph traversal order) for correctness."** The algorithm's proof holds for *any* fixed order of edges within each pass — order only affects how many passes are needed before convergence, never correctness.

## Key Takeaways

1. Bellman-Ford relaxes every edge, `V − 1` times, with no node-selection step — the opposite strategy from Dijkstra's "always finalize the closest node next."
2. The `V − 1` bound comes from the fact that a shortest simple path between any two of `V` nodes uses at most `V − 1` edges.
3. A `V`-th pass detects negative cycles for free: if anything still relaxes after `V − 1` passes have accounted for every simple path, the improvement must have come from looping a negative-weight cycle.
4. Recognition signal: negative edge weights are possible, or you need to detect whether a reachable negative cycle exists (arbitrage, feasibility of difference constraints).
5. Time is `O(V·E)`; space is `O(V)` for distances plus `O(E)` for the edge list — no heap, no adjacency-list-with-fast-lookup requirement.
6. The most common bug is skipping the `V`-th detection pass — this does not crash, it silently returns distances for a "shortest path" that does not actually exist.
7. The second most common bug is adding a Dijkstra-style `visited` guard — Bellman-Ford has no notion of a finalized node until every pass completes.
8. A negative cycle only corrupts results if it is reachable from the source; an unreachable negative cycle is correctly, silently ignored.
9. Use Dijkstra instead the moment all weights are verified non-negative; use Floyd-Warshall instead the moment you need all-pairs, not single-source, results.
10. Bounding the pass count to `K` instead of `V − 1` computes shortest paths using at most `K` edges — the technique behind LeetCode 787, Cheapest Flights Within K Stops.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — the standard formal treatment of Bellman-Ford, including the full proof of the `V − 1`-pass bound and negative-cycle detection.
- *Algorithms* (Sedgewick & Wayne) — covers Bellman-Ford alongside its use in arbitrage detection with a worked currency-exchange example.

**Reference**
- LeetCode — Cheapest Flights Within K Stops (787), Network Delay Time (743, also solvable with Bellman-Ford though Dijkstra is the intended approach).
- Wikipedia — "Bellman–Ford algorithm," including the distance-vector routing connection (RIP).

**Explainers**
- NeetCode — shortest path algorithm comparison videos, contrasting Bellman-Ford, Dijkstra, and Floyd-Warshall side by side.
