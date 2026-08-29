# Minimum Spanning Tree — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Kruskal's algorithm across its main problem shapes: the canonical implicit-complete-graph case, an explicit sparse graph with a feasibility check, the virtual-node trick for mixing cost types, and interrogating a single MST run to classify every edge. Each file is self-contained: compile and run it directly to see printed `[PASS]`/`[FAIL]` output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-min-cost-to-connect-all-points.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Min Cost to Connect All Points | [1584](https://leetcode.com/problems/min-cost-to-connect-all-points/) | Medium | Build every pairwise Manhattan-distance edge (the graph is implicit and complete), then plain Kruskal's. | O(n²log n) time, O(n²) space | [01-min-cost-to-connect-all-points.cpp](01-min-cost-to-connect-all-points.cpp) |
| Connecting Cities With Minimum Cost | [1135](https://leetcode.com/problems/connecting-cities-with-minimum-cost/) | Medium | Plain Kruskal's on an explicit sparse edge list, with a feasibility check (`edgesUsed == n - 1`) reporting `-1` when no spanning tree exists. | O(E log E) time, O(V + E) space | [02-connecting-cities-with-minimum-cost.cpp](02-connecting-cities-with-minimum-cost.cpp) |
| Optimize Water Distribution in a Village | [1168](https://leetcode.com/problems/optimize-water-distribution-in-a-village/) | Hard | The virtual-node trick: model "build a well" as an edge from an invented node 0 to that house, turning two cost types into one graph's MST. | O(E log E) time, O(V + E) space | [03-optimize-water-distribution-in-a-village.cpp](03-optimize-water-distribution-in-a-village.cpp) |
| Find Critical and Pseudo-Critical Edges in Minimum Spanning Tree | [1489](https://leetcode.com/problems/find-critical-and-pseudo-critical-edges-in-minimum-spanning-tree/) | Hard | Classify each edge by re-running Kruskal's with it excluded (critical test) and forced-included (pseudo-critical test), comparing against the baseline MST weight. | O(E²log E) time, O(V + E) space | [04-find-critical-and-pseudo-critical-edges.cpp](04-find-critical-and-pseudo-critical-edges.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):

- **01** is the pattern in its purest form on an implicit graph — the input is points, not edges, and the first real step is recognizing that "every pair is connectable" makes this a complete-graph MST before any algorithm runs at all.
- **02** shows the same algorithm on an explicit, possibly-sparse graph, and adds the feasibility check every real MST implementation needs: not every input can be spanned, and silently returning a partial total instead of `-1` is the most common bug this problem exposes.
- **03** is the module's sharpest conceptual trick: two structurally different cost types (a per-node cost and a per-edge cost) become one ordinary graph MST problem by inventing a single virtual node — a technique that generalizes far beyond this one problem, anywhere "connect to an external resource" and "connect to each other" are both being minimized together.
- **04** is the facet that moves past *running* Kruskal's to *interrogating* it: classifying every edge requires understanding the algorithm well enough to construct the right counterfactual re-runs (force-exclude, force-include) rather than inspecting a single MST's edge list, which is not enough information on its own to distinguish critical from pseudo-critical.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
