# Floyd-Warshall — Flow Diagram

See this diagram for the control flow of the triple-nested waypoint loop, exactly as implemented in [code.cpp](../code.cpp)'s `floydWarshall`, with the outer-`k` ordering requirement made explicit.

```mermaid
flowchart TD
    A(["Initialize dist[i][i]=0,<br/>dist[i][j]=direct edge weight,<br/>infinity everywhere else"]) --> B["k = 0"]
    B --> C{k &lt; n?}
    C -- No --> Z([Done — dist now holds<br/>every pair's shortest distance])
    C -- Yes --> D["i = 0"]
    D --> E{i &lt; n?}
    E -- No --> KNext["k = k + 1"]
    KNext --> C
    E -- Yes --> F{"dist[i][k] == INF?"}
    F -- Yes --> INext1["i = i + 1<br/>(no path to waypoint k yet)"]
    INext1 --> E
    F -- No --> G["j = 0"]
    G --> H{j &lt; n?}
    H -- No --> INext2["i = i + 1"]
    INext2 --> E
    H -- Yes --> I{"dist[k][j] == INF?"}
    I -- Yes --> JNext1["j = j + 1"]
    JNext1 --> H
    I -- No --> J{"dist[i][k] + dist[k][j]<br/>&lt; dist[i][j]?"}
    J -- Yes --> K["dist[i][j] = dist[i][k] + dist[k][j]"]
    J -- No --> JNext2["j = j + 1"]
    K --> JNext2
    JNext2 --> H

    Z -.-> CycleCheck{"Any dist[i][i] &lt; 0?"}
    CycleCheck -- Yes --> Neg([Negative cycle exists<br/>somewhere in the graph])
    CycleCheck -- No --> Clean([No negative cycle])
```

## How to read it

The outermost loop variable is `k`, not `i` or `j` — follow the diagram's own nesting order (`k` contains `i`, which contains `j`) and notice this is the *only* correct nesting. If `i` were made outermost instead, the diagram's `F` and `I` checks (`dist[i][k]`/`dist[k][j]` against infinity) would sometimes read values for a waypoint `k` that has not yet had its own turn in the outer position — meaning some relaxations would silently use a not-yet-fully-improved value, producing a partially correct matrix with no error or crash to signal it.

The two "== INF, skip" guards (`F` and `I`) exist purely to avoid computing `dist[i][k] + dist[k][j]` when one side is still the infinity sentinel — without them, that addition could silently overflow or produce a nonsensical small number that then corrupts `dist[i][j]` with a wrong "improvement." [code.cpp](../code.cpp) divides its infinity sentinel by 4 specifically so that even two infinities added together cannot overflow a `long long`, but the skip guards remain the correct, explicit way to avoid the addition entirely rather than relying on the sentinel's size alone.

The dotted line at the bottom shows the negative-cycle check running *after* the full triple loop completes, as a separate, final pass over just the diagonal — this is not part of the relaxation loop itself, but a one-line consequence of it: a diagonal entry that has dropped below zero is only possible if some path looped back through a cycle whose total weight is negative.
