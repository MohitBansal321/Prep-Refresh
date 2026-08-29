# Minimum Spanning Tree — Exercises

Work through these in order. The goal is to build two reflexes: (1) instantly distinguishing an MST question ("cheapest way to connect everything") from a shortest-path question ("cheapest way to reach one specific node"), and (2) recognizing when a problem's "cost" is not literally an edge weight yet, and needs a transformation (a virtual node, a sign flip, a different comparator) before Kruskal's or Prim's applies directly.

> Rule of thumb for every exercise: before writing a single line, ask "is there a privileged source or destination node in this question?" If yes, this is shortest path, not MST — go re-read [Dijkstra](../dijkstras-algorithm/exercises.md) or [Bellman-Ford](../bellman-ford/exercises.md) instead. If no single node is privileged and the question is about connecting a whole set as cheaply as possible, you are in the right module.

---

## Easy — Maximum Spanning Tree

Given the same kind of input as [code.cpp](code.cpp)'s `kruskalMST` (a node count and a weighted edge list), find the **maximum** spanning tree instead — the spanning tree whose total edge weight is as large as possible.

**Constraints to notice:** the exchange argument behind Kruskal's never depended on the *direction* of the comparison — it only depended on "the most extreme available edge that doesn't close a cycle can always be safely added." Flipping "cheapest first" to "priciest first" (or, equivalently, negating every weight and running the unmodified algorithm) should be the entire change needed.

**Task:** adapt `kruskalMST` from [code.cpp](code.cpp), changing only the sort comparator (or negating weights before sorting, then negating the total back at the end — try both and compare).

**Think about:** is a maximum spanning tree unique whenever a minimum one would be, under the same tie-breaking conditions? Construct a small graph where you can predict the answer by hand and confirm your adapted code matches it.

---

## Medium — Second-Best Minimum Spanning Tree

Given a weighted graph, find the total weight of the **second-best** spanning tree — the minimum-weight spanning tree among all spanning trees that are *not* equal in edge-set to the true MST (its total weight may tie the true MST's weight if multiple distinct minimum spanning trees exist, or may be strictly higher if the MST is unique).

**Constraints to notice:** the second-best spanning tree, if the MST is unique, is always exactly one edge-swap away from the true MST — replace exactly one MST edge with exactly one non-MST edge, chosen to minimize the resulting increase in total weight.

**Task:** first compute the true MST with `kruskalMST` from [code.cpp](code.cpp), recording exactly which edges were used. Then, for every non-MST edge `e`, find the maximum-weight MST edge on the tree path between `e`'s two endpoints (this is the edge `e` would have to replace to remain a valid tree), and compute the total weight if `e` replaced that specific edge. The second-best total is the minimum such replacement total across every non-MST edge.

**Then answer:** why must the replaced edge specifically be the *maximum*-weight edge on the path between `e`'s endpoints, rather than any edge on that path? What would go wrong (either producing an invalid tree, or a total that is not actually the *second-best*) if you replaced a different, cheaper edge on that same path instead?

---

## Hard — Minimum Spanning Tree With Required and Forbidden Edges

Given a weighted graph, plus a list of edges that **must** be included in the final tree and a separate list of edges that **must not** be included, find the minimum spanning tree respecting both constraints — or determine that no valid spanning tree exists under these constraints.

**Constraints to notice:** this is directly related to [problems/04-find-critical-and-pseudo-critical-edges.cpp](problems/04-find-critical-and-pseudo-critical-edges.cpp)'s "force-include" technique, generalized to *multiple* forced edges and combined with exclusion at the same time.

**Task:** before running the main Kruskal's loop, first union every endpoint pair from the **required** edges list and add their weights to the running total (checking as you go that no two required edges create a cycle with each other — if they do, no valid tree exists). Then run the ordinary sorted-edge Kruskal's loop over every remaining edge *except* the forbidden ones, continuing from the Union-Find state the required edges already built.

**Then answer:** two required edges are given that, together, already form a cycle before any optional edge is even considered. What should your function report, and where exactly in the algorithm does this get detected? Compare this to how [code.cpp](code.cpp)'s ordinary `kruskalMST` detects a disconnected (unspannable) graph, and explain why both failure modes are detected by the identical Union-Find mechanism.

---

## Real-World Challenge — A Multi-Region Network Cost Optimizer

You are designing the backbone network for a company with data centers in several regions. Connecting any two regions directly costs a known amount (private link setup + ongoing bandwidth cost, amortized to a single number per region-pair). Leadership additionally requires that two specific, business-critical regions (say, the primary and disaster-recovery sites) have a **direct** link between them regardless of cost, for latency reasons that override pure cost optimization.

**Task:**

1. Model the regions as nodes and the pairwise connection costs as a complete or near-complete weighted graph, and compute the ordinary minimum spanning tree using [code.cpp](code.cpp)'s `kruskalMST` as a baseline "if there were no special requirements" cost.
2. Now incorporate the required-direct-link constraint using the technique from the Hard exercise above: force that one specific edge into the tree before running the rest of Kruskal's, and report both the new total cost and the cost *increase* over the unconstrained baseline — a number leadership will want to see when approving the requirement.
3. **Discuss:** the network's link costs change periodically (new pricing contracts, new regions added). Recomputing the full MST from scratch on every change is correct but potentially wasteful if only one edge's cost actually changed. Sketch, in prose, how you would determine whether a single edge's cost change can possibly affect the existing MST at all, using what you know about the exchange argument — specifically, under what condition can a *single* edge cost change be proven to leave the previous MST optimal without recomputing anything?

---

## Bonus Challenge — Distinguishing MST From Shortest Path Under Pressure

You are given a single weighted, undirected graph and two different questions to answer against it:

1. "What is the minimum total cost to lay cable so every office can communicate with every other office (possibly through intermediate offices)?"
2. "What is the minimum cost to send a single message from the headquarters office to every other office as fast/cheaply as possible?"

**Task:** implement both answers against the same input graph — question 1 using [code.cpp](code.cpp)'s `kruskalMST`, question 2 using Dijkstra's algorithm (see [../dijkstras-algorithm/code.cpp](../dijkstras-algorithm/code.cpp)) — and construct a concrete example graph where the two produce **different trees** with different total weights, to prove to yourself they are not interchangeable.

**Then, generalize in writing (no code required):** using the Architecture and Similar Patterns sections of the [README](README.md), state precisely, in one sentence each, what quantity Kruskal's/Prim's minimizes versus what quantity Dijkstra's minimizes. Then explain why a graph with all-equal edge weights is the one case where an MST and a shortest-path tree from any single source can happen to coincide, and why that coincidence disappears the moment edge weights differ even slightly.

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
