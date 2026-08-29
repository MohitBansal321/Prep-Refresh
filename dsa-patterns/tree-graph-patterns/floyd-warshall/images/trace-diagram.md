# Floyd-Warshall — Trace Diagram (Worked Example)

This traces the full `dist[][]` matrix after each waypoint `k`, for the graph used in [code.cpp](../code.cpp)'s first test case:

```
Graph (directed, weights on arrows):

    0 --3--> 1        1 --2--> 2        2 --1--> 3
    0 --7--> 3        1 --8--> 0        2 --5--> 0
                                          3 --2--> 0

4 nodes: 0, 1, 2, 3.  No negative cycle exists in this graph.
```

```mermaid
sequenceDiagram
    autonumber
    participant Dist as dist[][]
    participant Loop as k loop (waypoint)

    Note over Dist: init: direct edges only<br/>dist[0][3]=7 (direct), dist[2][0]=5 (direct)

    Loop->>Dist: k=0 -- route everything through node 0
    Note over Dist: dist[1][3]: INF -> 15 (1->0->3)<br/>dist[2][1]: INF -> 8 (2->0->1)<br/>dist[3][1]: INF -> 5 (3->0->1)

    Loop->>Dist: k=1 -- route everything through node 1
    Note over Dist: dist[0][2]: INF -> 5 (0->1->2)<br/>dist[3][2]: INF -> 7 (3->1->2, using dist[3][1]=5 from k=0)

    Loop->>Dist: k=2 -- route everything through node 2
    Note over Dist: dist[0][3]: 7 -> 6 (0->2->3, BEATS the direct edge)<br/>dist[1][3]: 15 -> 3 (1->2->3)<br/>dist[1][0]: 8 -> 7 (1->2->0)

    Loop->>Dist: k=3 -- route everything through node 3
    Note over Dist: dist[1][0]: 7 -> 5 (1->3->0)<br/>dist[2][0]: 5 -> 3 (2->3->0, BEATS the direct edge)<br/>dist[2][1]: 8 -> 6 (2->0->1, chaining THIS pass's own dist[2][0] update)

    Loop-->>Loop: check every dist[i][i]: none < 0 -> no negative cycle
```

## Table form (the full matrix after each waypoint)

### Initial matrix (direct edges only)

|     | →0 | →1 | →2 | →3 |
|---|---|---|---|---|
| **0** | 0 | 3 | INF | 7 |
| **1** | 8 | 0 | 2 | INF |
| **2** | 5 | INF | 0 | 1 |
| **3** | 2 | INF | INF | 0 |

### After `k = 0` (routing through node 0)

|     | →0 | →1 | →2 | →3 |
|---|---|---|---|---|
| **0** | 0 | 3 | INF | 7 |
| **1** | 8 | 0 | 2 | **15** |
| **2** | 5 | **8** | 0 | 1 |
| **3** | 2 | **5** | INF | 0 |

Three cells improve, all via routing through node 0: `dist[1][3]` goes from `INF` to `15` (`1 → 0 → 3` = `8 + 7`), `dist[2][1]` goes from `INF` to `8` (`2 → 0 → 1` = `5 + 3`), and `dist[3][1]` goes from `INF` to `5` (`3 → 0 → 1` = `2 + 3`).

### After `k = 1` (routing through node 1)

|     | →0 | →1 | →2 | →3 |
|---|---|---|---|---|
| **0** | 0 | 3 | **5** | 7 |
| **1** | 8 | 0 | 2 | 15 |
| **2** | 5 | 8 | 0 | 1 |
| **3** | 2 | 5 | **7** | 0 |

`dist[0][2]` improves from `INF` to `5` (`0 → 1 → 2` = `3 + 2`), and `dist[3][2]` improves from `INF` to `7` (`3 → 1 → 2` = `5 + 2`, using the value `dist[3][1] = 5` that `k = 0`'s pass just computed).

### After `k = 2` (routing through node 2)

|     | →0 | →1 | →2 | →3 |
|---|---|---|---|---|
| **0** | 0 | 3 | 5 | **6** |
| **1** | **7** | 0 | 2 | **3** |
| **2** | 5 | 8 | 0 | 1 |
| **3** | 2 | 5 | 7 | 0 |

This is the pass to stare at. `dist[0][3]` drops from `7` (the direct edge) to `6` (`0 → 2 → 3` = `5 + 1`), beating the direct edge for the first time. `dist[1][3]` drops dramatically from `15` to `3` (`1 → 2 → 3` = `2 + 1`) — the huge, clearly-wrong-looking `15` from `k = 0`'s pass was never a final answer, just the best *known so far* using only node 0 as a waypoint; node 2 provides a far shorter route. And `dist[1][0]` improves from `8` to `7` (`1 → 2 → 0` = `2 + 5`).

### After `k = 3` (routing through node 3) — final

|     | →0 | →1 | →2 | →3 |
|---|---|---|---|---|
| **0** | 0 | 3 | 5 | 6 |
| **1** | **5** | 0 | 2 | 3 |
| **2** | **3** | **6** | 0 | 1 |
| **3** | 2 | 5 | 7 | 0 |

`dist[1][0]` improves once more, from `7` to `5` (`1 → 3 → 0` = `3 + 2`, using `dist[1][3] = 3` from the previous pass). `dist[2][0]` improves from `5` (the direct edge) to `3` (`2 → 3 → 0` = `1 + 2`), and `dist[2][1]` improves from `8` to `6` (`2 → 3 → 0 → 1`, chaining two already-computed improvements: `dist[2][0] = 3` from this same pass, plus `dist[0][1] = 3`).

No `dist[i][i]` ever dropped below `0` across any pass, confirming `hasNegativeCycle = false`.

## How to read it

Watch `dist[1][3]` across all four passes: `INF → 15 → 15 → 3 → 3`. The value `15` set during `k = 0` was never wrong, exactly, but it was never *final* either — it was simply the best answer achievable using only node `0` as an intermediate stop. This is the entire meaning of the induction argument behind the algorithm: after `k` waypoints have had their turn, `dist[i][j]` is correct **for paths using at most those `k` nodes as intermediate stops**, not correct in any absolute sense until every node has had its turn.

Also watch the very last pass (`k = 3`) improve `dist[2][1]` using `dist[2][0]`, a value that was *itself* just improved earlier in that same pass. `dist[2][0]` drops from `5` to `3` via `2 → 3 → 0` (`1 + 2`), and that freshly-improved `3` is then used to compute `dist[2][1] = dist[2][0] + dist[0][1] = 3 + 3 = 6` — a path (`2 → 3 → 0 → 1`) that routes through node 3 exactly once, not twice. This is exactly why the in-place update (no separate "previous `k`" copy) is safe rather than a bug: chaining same-pass improvements like this is how one pass over waypoint `k` correctly accounts for paths that use `k` together with earlier waypoints already folded into `dist[][]`, without needing a separate pass for every possible combination.
