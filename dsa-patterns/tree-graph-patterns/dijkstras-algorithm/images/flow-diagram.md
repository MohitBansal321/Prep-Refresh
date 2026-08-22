# Dijkstra's Algorithm — Flow Diagram (Lazy-Deletion Min-Heap)

This traces the control flow of the standard lazy-deletion Dijkstra loop — the shape behind Network Delay Time, Path With Minimum Effort (over grid cells), and every state-augmented variant. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual numbers.

```mermaid
flowchart TD
    Start([Start: weighted graph, all weights >= 0,<br/>a source node]) --> Init["Initialize dist[] = INF for all nodes<br/>dist[src] = 0<br/>min-heap pq, push {0, src}"]

    Init --> Loop{pq non-empty?}

    Loop -- No --> Done([Return dist[]<br/>unreachable nodes remain INF])

    Loop -- Yes --> Pop["Pop top = {d, u}<br/>d = distance recorded when pushed<br/>u = the node it belongs to"]

    Pop --> Stale{d > dist[u]?}
    Stale -- "Yes: a cheaper entry for u<br/>was pushed later and already processed" --> Loop

    Stale -- "No: this is the final distance for u" --> Relax["For each edge (u -> v, w):<br/>if dist[u] + w < dist[v]:<br/>  dist[v] = dist[u] + w<br/>  push {dist[v], v}"]

    Relax --> EarlyExit{"Looking for one specific target<br/>and it just popped?"}
    EarlyExit -- "Yes" -> Target([Return dist[target] now —<br/>it is provably final])
    EarlyExit -- "No / all distances needed" --> Relax2[Continue through remaining edges]
    Relax2 --> Loop
```

## How to read it

The loop's lifetime is bounded by heap size, not by V — because we use **lazy deletion**, an improved distance pushes a *new* entry and leaves the old stale one in place rather than running an expensive decrease-key operation. Each successful relaxation costs at most one push, so the heap holds at most E entries over the whole run; each push/pop is O(log E) = O(log V) on simple graphs, giving **O((V + E) log V)** total.

The `d > dist[u]` check is the entire correctness mechanism for lazy deletion: when a node's distance improves after an older entry is queued, the *improved* (smaller-d) entry necessarily pops first, does its relaxation work with the final distance, and by the time the stale larger-d copy later surfaces, `dist[u]` has already been lowered to or below it — so the O(1) comparison discards it without touching any edges. Skipping this check does not always produce wrong answers, but it turns every stale pop into a full edge-relaxation pass — wasted work at best, subtle re-relaxation bugs at worst.

The early-exit branch is worth internalizing: when you only need source-to-target distance, the moment the target survives the staleness check its distance is final — any other path to it would have to enter through a heap key that is already ≥ it. That single fact is what makes "point-to-point query" variants (and bidirectional-Dijkstra extensions) correct without processing the whole graph.

One more reading note: the diagram shows relaxation reading `dist[u]` after the staleness check passed — that ordering matters. If you relax edges using a stale `d` instead of the finalized `dist[u]`, you can propagate an outdated distance into neighbors' tentative values.
