# Bellman-Ford — Trace Diagram (Worked Example)

This traces `dist[]` across every pass for the graph used in [code.cpp](../code.cpp)'s first test case:

```
Graph (directed, weights on arrows):

    0 --6--> 1        1 --5--> 2        2 --3--> 4
    0 --7--> 2        1 -(-4)-> 3       3 --7--> 1
                       1 --8--> 4        4 --2--> 0
                       2 -(-3)-> 3       4 --7--> 3

Source = 0.  Expected final distances: dist = [0, 6, 7, 2, 14]
(The negative edge 1->3 (weight -4) is what makes dist[3] = 2, not some
larger value reached without it.)
```

Edges are relaxed in this fixed order every pass: `0→1, 0→2, 1→2, 1→3, 1→4, 2→3, 2→4, 3→1, 4→0, 4→3`.

```mermaid
sequenceDiagram
    autonumber
    participant Dist as dist[]
    participant Loop as pass loop
    participant Check as V-th pass (cycle check)

    Note over Dist: init: [0, INF, INF, INF, INF]

    Loop->>Dist: PASS 1 -- relax every edge once
    Note over Dist: 0->1 sets dist[1]=6<br/>0->2 sets dist[2]=7<br/>1->3 sets dist[3]=6+(-4)=2<br/>1->4 sets dist[4]=6+8=14
    Note over Dist: [0, 6, 7, 2, 14] -- changed=true

    Loop->>Dist: PASS 2 -- relax every edge again
    Note over Dist: every edge checked, none improves<br/>(e.g. 2->3: 7+(-3)=4, not < 2)
    Note over Dist: [0, 6, 7, 2, 14] -- changed=false
    Loop->>Loop: changed=false -> EARLY EXIT after pass 2

    Loop->>Check: run ONE more pass over every edge
    Note over Check: nothing relaxes -- same result as pass 2
    Check-->>Loop: hasNegativeCycle = false
```

## Table form (the same trace, pass by pass)

| Pass | `dist` before | Relaxations that fire | `dist` after | `changed`? |
|---|---|---|---|---|
| init | — | `dist[0] = 0` | `[0, INF, INF, INF, INF]` | — |
| 1 | `[0, INF, INF, INF, INF]` | `0→1`: dist[1]=6 · `0→2`: dist[2]=7 · `1→3`: dist[3] = 6+(-4) = **2** · `1→4`: dist[4] = 6+8 = 14 | `[0, 6, 7, 2, 14]` | **yes** |
| 2 | `[0, 6, 7, 2, 14]` | none — every edge is checked and none improves (e.g. `2→3`: 7+(-3)=4, not < 2) | `[0, 6, 7, 2, 14]` | **no** |

`changed` was `false` on pass 2, so [code.cpp](../code.cpp)'s early-exit `break` fires — the loop stops after 2 passes instead of running the full `n - 1 = 4` passes, because a pass that relaxes nothing proves no later pass can relax anything either.

**Negative-cycle check (the extra pass):** relax every edge one more time against the converged `[0, 6, 7, 2, 14]`. Nothing improves (same result as pass 2), so `hasNegativeCycle = false` — confirmed correct, since this graph's only negative edges (`1→3` weight -4, `2→3` weight -3) do not form a cycle among themselves.

## How to read it

Pass 1 is where the real work happens: watch `1→3` relax `dist[3]` directly from `INF` to `2` using a **negative** edge weight — this is the exact scenario Dijkstra cannot handle, because Dijkstra would have already "finalized" some other node's distance under the assumption that popped-node distances never improve, an assumption a negative edge like this one violates. Bellman-Ford never finalizes anything mid-run, so it has no such assumption to violate.

Pass 2 is the one worth sitting with even though nothing changes: it is not wasted work in the sense of being unnecessary to *run* (the algorithm cannot know in advance that pass 2 will find nothing), but it is exactly the pass whose "nothing changed" result is what makes the early-exit optimization safe — the algorithm does not merely stop because it *feels* converged, it stops because a full pass provably found zero improvements, which is a stronger and checkable guarantee.

The negative-cycle check pass is structurally identical to pass 2 in this trace — both examine every edge and find nothing to relax — but they answer different questions. Pass 2's "nothing changed" tells the algorithm it's safe to stop iterating. The final pass's "nothing changed" tells the algorithm the answer it already has is trustworthy. Contrast this with [code.cpp](../code.cpp)'s second test case (`0→1→2→1` at weights `1, -1, -1`), where the final pass would instead find an improvement forever — that is the signal that separates "converged" from "diverging through a negative cycle."
