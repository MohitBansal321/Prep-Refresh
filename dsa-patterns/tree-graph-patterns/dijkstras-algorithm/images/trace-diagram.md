# Dijkstra's Algorithm — Trace Diagram (Worked Example)

This traces the exact heap state and distance array for the graph used in [code.cpp](../code.cpp) and [problems/01-network-delay-time.cpp](../problems/01-network-delay-time.cpp)'s core routine:

```
Graph (directed, weights on arrows):

    0 --4--> 1
    0 --1--> 2
    2 --2--> 1
    1 --1--> 3
    2 --5--> 3

Source = 0.  Expected final distances: dist = [0, 3, 1, 4]
(The direct edge 0->1 of weight 4 LOSES to the two-hop path 0->2->1 costing 1+2=3.)
```

Heap entries are written `{d, u}` = tentative distance, node. `INF` means not yet reached.

| Iter | Pop `{d, u}` | Stale? | Relaxations performed | Heap after iteration | `dist` array after |
|------|--------------|--------|------------------------|----------------------|--------------------|
| — (init) | — | — | `dist[0] = 0`, push `{0,0}` | `{0,0}` | `[0, INF, INF, INF]` |
| 1 | `{0, 0}` | no | `0->1 w4`: dist[1]=4, push `{4,1}` · `0->2 w1`: dist[2]=1, push `{1,2}` | `{1,2} {4,1}` | `[0, 4, 1, INF]` |
| 2 | `{1, 2}` | no | `2->1 w2`: 1+2=3 < 4 → dist[1]=3, push `{3,1}` · `2->3 w5`: 1+5=6 → dist[3]=6, push `{6,3}` | `{3,1} {4,1} {6,3}` | `[0, 3, 1, 6]` |
| 3 | `{3, 1}` | no | `1->3 w1`: 3+1=4 < 6 → dist[3]=4, push `{4,3}` | `{4,1} {4,3} {6,3}` | `[0, 3, 1, 4]` |
| 4 | `{4, 1}` | **yes** (4 > dist[1]=3) | skipped — stale entry from iteration 1 | `{4,3} {6,3}` | `[0, 3, 1, 4]` |
| 5 | `{4, 3}` | no | node 3 has no outgoing edges | `{6,3}` | `[0, 3, 1, 4]` |
| 6 | `{6, 3}` | **yes** (6 > dist[3]=4) | skipped — stale entry from iteration 2 | *(empty)* | `[0, 3, 1, 4]` |

Heap empties → done. Final: `dist = [0, 3, 1, 4]`.

## How to read it

Read row by row and watch three things.

**First, the key insight of iteration 2:** popping node 2 with distance 1 lets the *indirect* path to node 1 (`0->2->1` = 1+2 = 3) beat the *direct* edge's 4 that was already sitting in both `dist[1]` and the heap. This is exactly why BFS-style "first discovery wins" is wrong on weighted graphs, and why Dijkstra pushes an improved entry instead of refusing to revisit a discovered node. The old `{4,1}` entry is left in place — lazy deletion — and becomes the stale pop you see in iteration 4.

**Second, the two staleness checks (iterations 4 and 6):** each pops an entry whose recorded distance is strictly greater than the node's current best, so the O(1) comparison `d > dist[u]` discards it with zero edge work. Without that check, iteration 4 would re-relax node 1's edges using the outdated distance 4 and could push garbage back into the heap.

**Third, the ordering guarantee in action:** every non-stale pop comes out in non-decreasing distance order (0, then 1, then 3, then 4), which is precisely why each such pop finalizes its node — nothing still in the heap can ever offer a cheaper route. Iteration 3 also shows a tie (`{4,1}` vs `{4,3}`) being broken arbitrarily by the pair comparison; ties are harmless because both distances are already correct.

Also note this trace needed **six pops for four nodes**, with two wasted on stale entries — typical for lazy deletion, and exactly the cost profile behind the "heap holds up to E entries" bound in [flow-diagram.md](flow-diagram.md).
