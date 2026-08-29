# Bellman-Ford — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing when a shortest-path problem's edges cannot be trusted to be non-negative, or when the question is really "does a negative cycle exist," and (2) correctly bounding the relaxation loop to whatever resource the problem actually constrains — every simple path (`V - 1`), a hop count, a time budget, or something else entirely.

> Rule of thumb for every exercise: before writing a single line, ask "can any edge weight be negative?" and "am I bounding the relaxation by `V - 1` passes, or does the problem give me a tighter, more specific budget?" If you cannot answer both, you are not ready to write the relax loop yet.

---

## Easy — Path With Maximum Probability (Bellman-Ford framing)

**LeetCode 1514 — Path With Maximum Probability.**

Given `n` nodes, undirected weighted edges where `edges[i] = [a, b]` carries success probability `succProb[i]` between 0 and 1, plus a start and an end node, return the maximum probability of successfully getting from start to end. Return 0 if there is no path.

**Constraints to notice:** this problem is usually solved with a max-heap Dijkstra variant (see the Dijkstra module's own exercises), but it is worth solving with plain Bellman-Ford first, because the "maximize a product" framing is a genuinely different relaxation rule than "minimize a sum" — and Bellman-Ford's relax-every-edge loop does not care whether the comparison is `<` or `>`, or whether combining two edges means adding or multiplying.

**Task:** adapt the relax loop from [code.cpp](code.cpp). Instead of `dist[u] + weight < dist[v]`, the relaxation becomes `prob[u] * succProb < prob[v]` flipped to `>` (you are maximizing, not minimizing), starting every node's probability at 0 except the source at 1. Run it for `V - 1` passes exactly as the template does.

**Think about:** why does the `V - 1` pass bound still apply here, even though you are maximizing a product instead of minimizing a sum? What property of the graph — not of the comparison operator — is actually responsible for that bound?

---

## Medium — Number of Ways to Arrive at Destination (Bellman-Ford framing)

**LeetCode 1976 — Number of Ways to Arrive at Destination.**

Given an undirected weighted graph with `n` intersections and bidirectional roads with travel times, count the number of distinct shortest paths from intersection 0 to intersection n-1. Since the count can be huge, return it modulo 10^9 + 7.

**Constraints to notice:** you still need the shortest distances, but the *output* is a count of paths achieving that distance, not the distance itself. Every time you relax an edge, there are now three possible outcomes: strictly better distance found (reset the count), exactly equal distance found (accumulate the count), or worse distance (ignore) — and because Bellman-Ford relaxes edges in an unordered pass rather than Dijkstra's strictly-increasing-distance pop order, you must be careful about *when* within a pass a count is considered final.

**Task:** extend the relax loop with a parallel `ways[]` array alongside `dist[]`. Run enough full passes (`V - 1`) that every edge has had the chance to contribute to both `dist[]` and `ways[]`, and think carefully about whether ties discovered on different passes need to be treated any differently from ties discovered within the same pass.

**Think about:** [code.cpp](code.cpp)'s `bellmanFord` has an early-exit `changed` flag that stops as soon as a pass relaxes nothing. Is that early exit still safe once you are also tracking path counts, or could stopping early undercount ties that would have been found on a later pass? Justify your answer with a concrete small example.

---

## Hard — Detecting an Exploitable Currency Loop

Given a list of currency pairs and their exchange rates (`rates[i] = [from, to, rate]`, meaning 1 unit of `from` converts to `rate` units of `to`), determine whether there exists a sequence of conversions starting and ending at the same currency that results in **more** currency than you started with — an arbitrage opportunity.

**Constraints to notice:** exchange rates multiply along a path, but Bellman-Ford's relaxation is built around *summing* edge weights. The standard trick: transform each edge's weight to `-log(rate)` before running Bellman-Ford. Multiplying rates along a path becomes *summing* the transformed weights, and "final value greater than 1" (a profitable loop) becomes "the summed weight is negative" — exactly the condition [code.cpp](code.cpp) already detects.

**Task:** build the `-log(rate)` graph, run [code.cpp](code.cpp)'s `bellmanFord` from every currency as a potential source (since an arbitrage loop might not be reachable from any single fixed starting point you pick arbitrarily), and report `true` the moment any run's `hasNegativeCycle` comes back true.

**Then answer:** why is `-log(rate)` the correct transformation rather than, say, `1 - rate` or `1 / rate`? Work through what "sum of transformed weights around a loop" needs to equal for the untransformed product around that same loop to equal exactly 1, and confirm `-log` is the transformation that achieves it.

---

## Real-World Challenge — A Discount-Chain Abuse Detector

You run a promotions engine for an e-commerce platform. Each promotion, when applied, can *unlock* other promotions as a side effect (e.g., "Spend $50, get 10% off your next order" unlocks a promotion worth applying next). Product has asked you to detect whether any sequence of promotion applications lets a customer end up with **more** account credit than they started with — a closed loop of promotions that is net-positive for the customer and therefore exploitable at scale.

**Task:**

1. Model each promotion as a node, and each "applying promotion A unlocks promotion B, with a net credit change of `c`" relationship as a directed edge `A -> B` with weight `-c` (so that a net-*positive* chain of promotions becomes a *negative*-weight cycle, exactly mirroring the currency-arbitrage transformation above). Explain, in your own words, why the sign has to flip here the same way it did for exchange rates.
2. Implement the detector using [code.cpp](code.cpp)'s `bellmanFord`, running it from every promotion as a potential source, since a real promotions graph is unlikely to have one single obvious "entry point" the way a fixed currency example does.
3. **Discuss:** in a real production system, this graph changes every time a new promotion is added or an existing one's terms change. Re-running full Bellman-Ford from every node on every promotion-catalog update is correct but potentially expensive at scale. Sketch two strategies for cutting that cost (e.g., only re-checking cycles that pass through the newly added/changed promotion, or maintaining an incremental "known negative-cycle-free" invariant that a single new edge can only violate locally) and name one correctness trap each strategy introduces.

---

## Bonus Challenge — Distance-Vector Routing, By Hand

Distance-vector routing protocols like RIP run a *distributed* version of Bellman-Ford: each router only knows the costs of its own direct links, and periodically tells its neighbors "here is my current best cost to every destination I know about." A neighbor updates its own table by relaxing against whatever its neighbors just told it — the exact same relaxation rule as centralized Bellman-Ford, just run independently on every machine instead of in one loop over one edge list.

**Task:** using the same 5-node graph asserted in [code.cpp](code.cpp)'s first test case, simulate this by hand (or in a small script) as a **distributed** process: give each node its own local table, and on each synchronized "round," have every node send its current table to its direct neighbors and relax against what it receives, rather than one central loop iterating over a shared edge list. Confirm your distributed simulation converges to the same `dist[]` values [code.cpp](code.cpp) computes centrally, and count how many rounds it actually takes.

**Then, generalize in writing (no code required):** distance-vector routing has a well-known failure mode called "count to infinity," where a link failure can cause routers to slowly increment a cost back and forth between each other instead of quickly discovering a route is gone. Explain, using what you now know about *why* Bellman-Ford needs exactly `V - 1` passes to converge, why a distributed version with no global pass counter and no global view of `V` is vulnerable to this in a way the centralized algorithm in [code.cpp](code.cpp) is not.

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
