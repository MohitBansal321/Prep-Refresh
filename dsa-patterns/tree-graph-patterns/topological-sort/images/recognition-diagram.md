# Topological Sort — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether Topological Sort is the right tool, or whether the problem actually wants plain Graph BFS/DFS or Union Find instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Does the problem describe<br/>tasks/courses/items with<br/>directed 'must happen before'<br/>dependency constraints?}

    Q1 -- No --> Q1b{Is the graph undirected,<br/>with edges arriving one at a<br/>time, and you repeatedly ask<br/>'connected?' or 'would this<br/>edge create a cycle?'}
    Q1b -- Yes --> UnionFind[["Use Union Find<br/>(track components incrementally,<br/>no traversal needed,<br/>near O(1) amortized per query)"]]
    Q1b -- No --> Q1c{Do you need reachability,<br/>shortest hops, or connected<br/>components on a graph that<br/>may have cycles?}
    Q1c -- Yes --> GraphBFSDFS[["Use Graph BFS/DFS<br/>(traversal with a visited set,<br/>cycles are fine and expected)"]]
    Q1c -- No --> Rethink[Re-read the problem —<br/>it may not be a graph<br/>pattern at all]

    Q1 -- Yes --> Q2{Do you need a valid<br/>PROCESSING ORDER of all<br/>items, or to detect that<br/>no valid order exists?}

    Q2 -- "No, just 'is it possible?'<br/>(yes/no only)" --> CycleOnly["Use Topological Sort<br/>(Kahn's algorithm),<br/>but you only need the<br/>hasCycle / order-length check —<br/>discard the order itself"]

    Q2 -- "Yes, I need the actual order<br/>(or the order derived from<br/>some other input first)" --> Q3{Is 'any' valid order fine,<br/>or do you need a SPECIFIC one<br/>(e.g. lexicographically smallest)?}

    Q3 -- "Any valid order is fine" --> Kahn["Use Topological Sort<br/>(Kahn's algorithm, plain FIFO queue)<br/>in-degree array + queue + output list"]
    Q3 -- "Must be the lexicographically<br/>smallest valid order" --> KahnHeap["Use Topological Sort<br/>(Kahn's algorithm, min-heap<br/>instead of a plain queue)<br/>O(V log V + E)"]

    CycleOnly --> Done([Topological Sort applies])
    Kahn --> Done
    KahnHeap --> Done
```

## How to read it

The **first fork** is the real recognition signal: does the problem describe **directed** "this must come before that" relationships between discrete items? If the relationships are undirected and you are being asked repeated connectivity or cycle-on-insertion queries as edges stream in one at a time, that is Union Find's territory, not Topological Sort's — Union Find never produces an ordering, it only answers "same component?" If instead you need reachability or shortest hops on a graph that may legitimately contain cycles, that is plain Graph BFS/DFS.

The **second fork**, once you know the relationships are directed "must precede" constraints, separates "do I just need to know if a valid order exists at all" (a pure cycle-detection question, like Course Schedule) from "I actually need the order itself" (like Course Schedule II). Both use exactly the same Kahn's-algorithm machinery — the only difference is whether you keep and return the `order` list or just its length/`hasCycle` flag.

The **third fork** matters only if you do need the actual order: a plain FIFO queue gives you *a* valid order (any one that respects every edge), which is sufficient for the overwhelming majority of problems. Only reach for a min-heap-backed priority queue if the problem explicitly demands a specific canonical order (most commonly, the lexicographically smallest one) — that swap costs an extra `log V` factor per operation but is a small, well-understood extension rather than a different algorithm. Also watch for problems that hide the graph entirely, like Alien Dictionary, where you must first *derive* the directed edges from an unrelated-looking input before any of this diagram applies.
