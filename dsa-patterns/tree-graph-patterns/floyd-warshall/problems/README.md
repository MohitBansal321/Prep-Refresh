# Floyd-Warshall — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Floyd-Warshall's waypoint-relaxation idea across its main problem shapes: the textbook numeric all-pairs distance matrix, boolean transitive closure, multiplicative (rather than additive) relaxation, and asymmetric reachability. Each file is self-contained: compile and run it directly to see printed `[PASS]`/`[FAIL]` output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-find-the-city-with-smallest-number-of-neighbors-at-a-threshold-distance.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Find the City With the Smallest Number of Neighbors at a Threshold Distance | [1334](https://leetcode.com/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/) | Medium | Textbook all-pairs distance matrix, then count each city's within-threshold neighbors. | O(V³) time, O(V²) space | [01-...cpp](01-find-the-city-with-smallest-number-of-neighbors-at-a-threshold-distance.cpp) |
| Course Schedule IV | [1462](https://leetcode.com/problems/course-schedule-iv/) | Medium | The waypoint relaxation specialized to booleans (OR instead of MIN) — full transitive closure. | O(V³) time, O(V²) space | [02-course-schedule-iv.cpp](02-course-schedule-iv.cpp) |
| Evaluate Division | [399](https://leetcode.com/problems/evaluate-division/) | Medium | The waypoint relaxation specialized to products (MULTIPLY instead of ADD) over a quotient graph. | O(V³) time, O(V²) space | [03-evaluate-division.cpp](03-evaluate-division.cpp) |
| Detonate the Maximum Bombs | [2101](https://leetcode.com/problems/detonate-the-maximum-bombs/) | Medium | Transitive closure over an **asymmetric** reachability relation, then take the largest reachable set over every starting choice. | O(V³) time, O(V²) space | [04-detonate-the-maximum-bombs.cpp](04-detonate-the-maximum-bombs.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):

- **01** is the pattern in its purest numeric form — a genuine all-pairs distance question on a graph small enough (`n ≤ 100`) that `O(V³)` is the pragmatic default, not a compromise.
- **02** shows the exact same triple loop generalizes from "cheapest numeric path" to "does any path exist at all" by swapping `MIN` for `OR` — the transitive-closure use case, and the facet where the relaxation payload becomes a boolean instead of a distance.
- **03** generalizes again, this time to `MULTIPLY` instead of `ADD` — proof that the waypoint idea depends only on the relaxation operation being associative and having an identity, not on it specifically being addition.
- **04** is the sharpest departure from the other three: the direct edges are **not** symmetric (a large blast radius can trigger a small one without the reverse being true), which is a real, easy-to-miss trap if you assume "undirected road network" style symmetry out of habit.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
