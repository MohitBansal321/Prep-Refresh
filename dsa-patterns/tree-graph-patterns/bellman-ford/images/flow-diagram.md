# Bellman-Ford — Flow Diagram

See this diagram for the control flow of the relax-every-edge loop and the negative-cycle-detection pass that follows it, exactly as implemented in [code.cpp](../code.cpp)'s `bellmanFord`.

```mermaid
flowchart TD
    A([Initialize dist: 0 at source,<br/>infinity everywhere else]) --> B["pass = 0"]
    B --> C{pass &lt; V - 1?}
    C -- No --> G[Run ONE more pass<br/>over every edge]
    C -- Yes --> D["changed = false"]
    D --> E["For every edge (u, v, w):"]
    E --> F{"dist[u] != INF AND<br/>dist[u] + w &lt; dist[v]?"}
    F -- Yes --> F1["dist[v] = dist[u] + w<br/>changed = true"]
    F -- No --> F2[skip this edge]
    F1 --> E
    F2 --> E
    E -- "all edges checked" --> H{changed?}
    H -- No --> G
    H -- Yes --> I["pass = pass + 1"]
    I --> C

    G --> J{"Did ANY edge still<br/>relax during this pass?"}
    J -- Yes --> K([hasNegativeCycle = true<br/>— every dist[] value below<br/>may be unreliable])
    J -- No --> L([hasNegativeCycle = false<br/>— dist[] is final and correct])
```

## How to read it

The outer loop (`pass < V - 1`) is the part that looks like Dijkstra's main loop but is not: there is no priority queue, no "pick the next node," and every single edge is examined on every pass regardless of any distance ordering. The inner relax check (`dist[u] + w < dist[v]`) is the *entire* algorithm's logic — everything else is bookkeeping around calling that check enough times.

Watch the `changed` flag closely: it is what lets [code.cpp](../code.cpp) exit the outer loop early once a pass finds nothing left to improve, without weakening correctness — if a pass changes nothing, every subsequent pass over the exact same graph and the exact same `dist[]` values would also change nothing, so continuing to run more passes would only waste time, never fix a missed case.

The diagram's bottom half is the part that has no Dijkstra equivalent at all: **one extra pass**, run unconditionally after the main loop finishes (whether it finished by exhausting `V - 1` passes or by exiting early via `changed`). That extra pass asks a single question — "did anything relax that shouldn't have been possible if every simple path had already been accounted for?" — and a "yes" answer is only possible if some path used more edges than a simple path ever could, which can only happen by looping through a cycle whose total weight is negative.
