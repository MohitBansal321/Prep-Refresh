# Union Find — Recognition Diagram

Use this flowchart when you are staring at a graph/grid problem and trying to decide whether Union Find is the right tool, or whether the problem actually wants Graph BFS/DFS or Topological Sort instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Do edges/connections<br/>arrive one at a time,<br/>or is the graph directed<br/>with a "must come before"<br/>ordering requirement?}

    Q1 -- "Directed edges express<br/>prerequisites / dependencies,<br/>need a valid ORDER" --> TopoSort[["Use Topological Sort<br/>(Kahn's BFS or DFS post-order)<br/>see ../topological-sort/"]]

    Q1 -- "Undirected connections,<br/>no ordering requirement" --> Q2{Do you need the<br/>ACTUAL PATH/edges between<br/>two nodes, or just whether<br/>they're connected?}

    Q2 -- "Need the actual path,<br/>shortest hop count, or to<br/>visit every node/edge" --> GraphTraversal[["Use Graph BFS/DFS<br/>see ../graph-bfs-dfs/"]]

    Q2 -- "Only need yes/no<br/>connectivity, or a<br/>component count" --> Q3{Is this a ONE-SHOT query<br/>on a graph that is fully<br/>built up front, or do queries<br/>and edge-additions REPEAT<br/>and INTERLEAVE many times?}

    Q3 -- "One-shot: build the graph<br/>once, answer once" --> Q3b{Is a single BFS/DFS<br/>pass simpler to write<br/>and equally fast here?}
    Q3b -- Yes --> GraphTraversal
    Q3b -- "No, still cleaner as<br/>component counting" --> UnionFind

    Q3 -- "Repeated/interleaved:<br/>'is X connected to Y?' or<br/>'would adding this edge<br/>create a cycle?' asked<br/>again and again as edges<br/>keep arriving" --> UnionFind["Use Union Find<br/>(Disjoint Set)<br/>find() + unionSets(),<br/>no traversal needed at all"]

    UnionFind --> Done([Union Find applies])
```

## How to read it

Start at the top and answer each diamond honestly. The **first fork** separates directed "must come before" problems (Topological Sort's territory) from plain undirected connectivity problems — Union Find has no concept of edge direction, so if the problem talks about prerequisites, scheduling, or a valid build order, it is never the right tool.

The **second fork** is the one people skip too quickly: Union Find can tell you *whether* two nodes are connected and *how many* components exist, but it can never hand you the actual sequence of edges between them, nor a shortest-hop count. If the question asks "what is the path from A to B" or "what is the minimum number of steps," that is Graph BFS/DFS's job, not Union Find's — see this module's Disadvantages section for exactly why.

The **third fork** is the real recognition signal: Union Find earns its keep specifically when connectivity queries and edge-additions **repeat and interleave** — you keep asking "connected?" or "would this edge close a cycle?" as more edges keep arriving, and re-running BFS/DFS from scratch on every single query would cost O(V+E) *per query*. If instead the graph is fully known up front and you only need to answer once (a single "count the components" pass, as in Number of Provinces), a single BFS/DFS sweep works just as well — Union Find is still a clean way to express "count components," but it is not solving a problem BFS/DFS could not equally solve in that one-shot case. The moment queries and unions are interleaved many times (Redundant Connection's edge-by-edge cycle check, Number of Islands II's cell-by-cell incremental connectivity), Union Find's near-O(1) amortized `find`/`union` stops being a nice-to-have and becomes the only approach that avoids paying O(V+E) again and again.
