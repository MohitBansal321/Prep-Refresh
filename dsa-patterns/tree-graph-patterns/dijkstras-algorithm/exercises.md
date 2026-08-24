# Dijkstra's Algorithm — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "weighted graph + minimum cost to reach" signal that means "reach for Dijkstra" — and equally, recognizing when the answer is actually BFS, Bellman-Ford, or Floyd-Warshall; and (2) writing the lazy-deletion min-heap loop correctly from memory, including the stale-entry check.

> Rule of thumb for every exercise: before writing a single line, ask "can any edge weight be negative?" and "is the metric a sum of edge weights, or something else (a max along the path, a count of paths, a constrained state)?" If you cannot answer both questions, you are not ready to write the relaxation rule yet.

---

## Easy — Path With Maximum Probability

**LeetCode 1514 — Path With Maximum Probability.**

Given `n` nodes, a list of undirected weighted edges where `edges[i] = [a, b]` carries success probability `succProb[i]` between 0 and 1, plus a start and an end node, return the maximum probability of successfully getting from start to end. Return 0 if there is no path.

**Constraints to notice:** the edge values are probabilities, so you are *maximizing a product* of numbers in [0, 1] — not minimizing a sum. Multiplying probabilities in [0, 1] shrinks them, so the greedy direction flips: you want the path whose product is largest. Two standard ways to map this back onto the standard template: work with negated log-probabilities (products become sums), or keep the raw products and flip every comparison so the heap is a max-heap keyed on best-so-far probability.

**Task:** adapt the template from [code.cpp](code.cpp). Decide explicitly which mapping you will use before coding, and write down your new relaxation rule as one line of pseudocode first.

**Think about:** in the product formulation, why is it still true that the first useful pop finalizes a node? What plays the role that "non-negative weights" played in the sum version?

---

## Medium — Number of Ways to Arrive at Destination

**LeetCode 1976 — Number of Ways to Arrive at Destination.**

Given an undirected weighted graph with `n` intersections and bidirectional roads with travel times, count the number of distinct shortest paths from intersection 0 to intersection n-1. Since the count can be huge, return it modulo 10^9 + 7.

**Constraints to notice:** you still need Dijkstra for the distances, but the *output* is not a distance — it is a count. Every time you relax an edge, there are now three possible outcomes per neighbor: strictly better distance found (reset the count), exactly equal distance found (accumulate the count), worse distance (ignore).

**Task:** extend the template with a parallel `ways[]` array alongside `dist[]`, and make sure the tie case (`dist[u] + w == dist[v]`) is handled — it is easy to write only `<` and silently drop every tied path.

**Think about:** when you pop a node whose entry turns out stale, must you do anything special to avoid double-counting paths? Why does processing nodes strictly in finalized-distance order guarantee each shortest path is counted exactly once?

---

## Hard — Swim in Rising Water

**LeetCode 778 — Swim in Rising Water.**

An `n x n` grid where `grid[i][j]` is the elevation of that cell. It rains constantly; at time `t` the water level is `t`, so you can swim to a cell at time `t` only if `t >= grid[i][j]`. Starting at the top-left at time 0, moving orthogonally cell to cell instantly, find the least time until you reach the bottom-right.

**Constraints to notice:** this is *not* a sum-of-weights problem. The cost of a path is the **maximum elevation on it**, and the answer is the minimum such max over all paths. The relaxation rule becomes: `newTime = max(time[u], grid[v])`, i.e. the moment you can step onto the next cell is dictated by the single worst cell you have had to wait for, not by accumulated travel.

**Task:** solve it with a modified Dijkstra over cells (a priority queue of `(time, row, col)`), and also convince yourself why plain BFS would be wrong here and why binary-searching the answer over time is a valid alternative approach worth knowing about.

**Think about:** Path With Minimum Effort ([problems/02-path-with-minimum-effort.cpp](problems/02-path-with-minimum-effort.cpp)) has the same "minimize the maximum step" shape. What is identical between the two solutions, and what differs? Could one shared helper solve both?

---

## Real-World Challenge — A Delivery Router That Respects Time Windows

You are building routing for a delivery fleet. The road network is a weighted graph (intersections as nodes, road segments as directed edges carrying drive-time minutes). Each delivery stop additionally opens at some minute `open[t]`: arriving earlier means waiting idle until it opens. Given a depot and a sequence of stops, compute the earliest feasible arrival time at each stop in order.

**Task:**
1. Model waiting-at-a-stop as part of the graph: describe how to encode "arrive before opening, wait" using either extra edges or a modification to the relaxation rule, and argue which encoding keeps all weights non-negative.
2. Implement point-to-point queries with Dijkstra (early exit once the target pops), reusing the structure from [code.cpp](code.cpp) but storing `std::vector<std::vector<std::pair<int,int>>>` built from an edge list.
3. Discuss: traffic makes edge weights change every few minutes. Re-running full Dijkstra per query is correct but wasteful. Sketch (in prose, no code required) two strategies for amortizing — e.g. caching distances from popular hubs, or bidirectional search meeting in the middle — and name one correctness trap each strategy introduces.

---

## Bonus Challenge — Grid With Cost-Zero Teleports (0-1 BFS)

**LeetCode 1368 — Minimum Cost to Make at Least One Valid Path in a Grid.**

An `n x m` grid where each cell contains an arrow (1=right, 2=left, 3=down, 4=up). Moving in the direction of a cell's arrow is free; modifying a cell's arrow to move any other direction costs 1. Find the minimum total modifications needed to walk from top-left to bottom-right.

**Task:** notice the weights are only 0 or 1. Solve it twice: once with plain Dijkstra treating costs as ordinary weights, and once with the specialized **0-1 BFS** trick (a deque instead of a priority queue: cost-0 moves push front, cost-1 moves push back) — then explain why the deque preserves the same "closest unfinalized node next" guarantee that the heap provides, without any O(log V) factor.

**Then, generalize in writing (no code required):** the template in [code.cpp](code.cpp) relaxes over `(neighbor, weight)` pairs. State precisely what the "state" of a Dijkstra run is in general — it was just `(node)` here, but became `(node, stopsUsed)` in K-constrained problems and `(cell, arrow-direction)` is conceivable elsewhere — and give the one-line criterion for deciding whether a problem needs an augmented state space or the plain node-as-state form.

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
