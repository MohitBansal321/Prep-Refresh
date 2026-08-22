# Dijkstra's Algorithm — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Dijkstra across its main problem shapes: the textbook single-source run, the grid-as-implicit-graph with a "minimize the maximum step" twist, the state-augmented variant under an edge-count constraint, and all-pairs-by-repeated-runs on a small graph. Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-network-delay-time.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Network Delay Time | [743](https://leetcode.com/problems/network-delay-time/) | Medium | Textbook lazy-deletion min-heap Dijkstra from the source; answer is the maximum finalized distance (or -1 if any node stays at INF). | O((V + E) log V) time, O(V + E) space | [01-network-delay-time.cpp](01-network-delay-time.cpp) |
| Path With Minimum Effort | [1631](https://leetcode.com/problems/path-with-minimum-effort/) | Medium | Grid as an implicit graph; relax with `max(effort[u], weight)` instead of a sum — minimize the largest single step along the path. | O(V log V) time, O(V) space (V = cells) | [02-path-with-minimum-effort.cpp](02-path-with-minimum-effort.cpp) |
| Cheapest Flights Within K Stops | [787](https://leetcode.com/problems/cheapest-flights-within-k-stops/) | Medium | Plain Dijkstra fails under a hop limit; augment the state to `(node, stopsUsed)` so a pricier-but-fewer-stops path is never thrown away. | O((K+1)·E·log(K·V)) time, O(K·V + E) space | [03-cheapest-flights-within-k-stops.cpp](03-cheapest-flights-within-k-stops.cpp) |
| Find the City With the Smallest Number of Neighbors at a Threshold Distance | [1334](https://leetcode.com/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/) | Medium | Run Dijkstra from every node on a small graph; count reachable cities within the threshold; break ties toward the larger city id. | O(V · E log V) time, O(V + E) space | [04-find-the-city-with-smallest-number-of-neighbors-at-a-threshold-distance.cpp](04-find-the-city-with-smallest-number-of-neighbors-at-a-threshold-distance.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is Dijkstra in its purest form — explicit weighted graph, single source, sum of weights — plus the "any unreachable node?" edge case that turns the distance array into an answer.
- **02** shows the graph is often *implicit* (a grid), and that the relaxation rule generalizes beyond sums to path metrics like "minimize the maximum edge".
- **03** demonstrates the pattern's most important limitation in practice: constraints that make bare-node states insufficient, fixed by running Dijkstra over `(node, budget)` states. Understanding *why* plain Dijkstra breaks here is the whole lesson.
- **04** shows the standard trick for small-graph all-pairs questions: V independent Dijkstra runs instead of Floyd-Warshall, with tie-breaking and disconnected-component handling.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
