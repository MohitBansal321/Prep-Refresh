# Graph BFS/DFS — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide (a) whether it is even a graph problem, and (b) if it is, whether it wants BFS, DFS, or one of the neighbouring patterns — Topological Sort, Union Find, or Dijkstra — instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q0{Is there a set of THINGS<br/>with pairwise connections<br/>between them?<br/>-- cells and their neighbours,<br/>users and friendships,<br/>words differing by one letter,<br/>tasks and prerequisites}

    Q0 -- "No relation structure at all" --> NotGraph[["Not a graph problem<br/>-- look elsewhere<br/>(array / string / DP patterns)"]]

    Q0 -- "Yes -- explicit adjacency list/matrix,<br/>OR an IMPLICIT one:<br/>a grid, a word list, a<br/>dependency list" --> Q1{Is the structure a TREE?<br/>-- one root, exactly one path<br/>between any two nodes,<br/>cycles impossible}

    Q1 -- Yes --> TreeTraversal[["Use Tree BFS / Tree DFS<br/>-- no visited set needed,<br/>revisiting is structurally<br/>impossible"]]

    Q1 -- "No -- cycles are possible,<br/>no designated root" --> Q2{Do the edges carry<br/>WEIGHTS / costs / distances,<br/>where 1 hop is not the<br/>same as 1 unit of cost?}

    Q2 -- "Yes, non-negative weights" --> Dijkstra[["Use Dijkstra's algorithm<br/>-- BFS with a min-heap<br/>instead of a FIFO queue,<br/>O((V+E) log V)"]]
    Q2 -- "Yes, some weights negative" --> BellmanFord[["Use Bellman-Ford<br/>-- Dijkstra's greedy<br/>assumption breaks on<br/>negative edges"]]

    Q2 -- "No -- every edge is one hop" --> Q3{What is actually<br/>being asked?}

    Q3 -- "'Shortest' / 'minimum' /<br/>'fewest steps' / 'nearest'<br/>number of hops" --> BFS["Use Graph BFS<br/>-- queue, mark on ENQUEUE.<br/>Ring-by-ring order IS the<br/>shortest-path proof.<br/>DFS here is WRONG, not slow"]

    Q3 -- "'In what ORDER can these<br/>dependent tasks run?'" --> TopoSort[["Use Topological Sort<br/>-- this pattern's machinery,<br/>different question<br/>(needs a DAG)"]]

    Q3 -- "Repeated 'are these two<br/>connected?' as edges<br/>arrive one at a time" --> UnionFind[["Use Union Find<br/>-- answers connectivity<br/>without traversing,<br/>~O(alpha(V)) per query"]]

    Q3 -- "'How many groups / islands /<br/>provinces?' 'Can A reach B?'<br/>'Is it all connected?'" --> Components["Use Graph DFS or BFS<br/>-- EITHER works; no distance<br/>in the question, so pick DFS<br/>for brevity or BFS for<br/>stack safety.<br/>Loop over EVERY node as a<br/>potential unvisited start"]

    Q3 -- "'Is there a cycle?'" --> Q4{Is the graph<br/>DIRECTED or UNDIRECTED?}

    Q4 -- Directed --> ThreeState["Use Graph DFS with a<br/>THREE-state marker<br/>unvisited / in-progress / done.<br/>Only a back edge to an<br/>IN-PROGRESS node is a cycle"]

    Q4 -- Undirected --> TwoColor["Use Graph DFS or BFS with<br/>a parent check, or -- for the<br/>odd-cycle question -- two-colouring.<br/>The colour array doubles as<br/>the visited array"]

    BFS --> Done([Graph BFS/DFS applies])
    Components --> Done
    ThreeState --> Done
    TwoColor --> Done
```

## How to read it

Start at the top and answer each diamond honestly. The **first fork is not about traversal at all** — it is about whether you have noticed there is a graph in front of you. A large share of real graph problems never use the word: a 2D grid is a graph whose edges are geometric ([problems/01-number-of-islands.cpp](../problems/01-number-of-islands.cpp)), a word list is a graph whose edges are "differs by one letter" ([problems/04-word-ladder.cpp](../problems/04-word-ladder.cpp)), an `isConnected` matrix is a graph in the least disguised possible way ([problems/02-number-of-provinces.cpp](../problems/02-number-of-provinces.cpp)). If there is a set of things and a rule saying which pairs are directly related, you have nodes and edges, whatever the input type says.

The **second fork** is the one that decides whether you need this module at all. If the structure is genuinely a tree — one root, exactly one path between any two nodes — then revisiting a node is structurally impossible and the `visited` set is dead weight; use [../../tree-bfs/](../../tree-bfs/) or [../../tree-dfs/](../../tree-dfs/). Everything below this fork exists because that guarantee is gone.

The **third fork is a trap door, and it is worth checking before anything else in the "what is being asked" group.** If edges carry weights — road distances, request latencies, dollar costs — then BFS's ring-by-ring order stops corresponding to "cheapest," and BFS will confidently return a wrong answer rather than an error. One 100-cost edge is not better than three 3-cost edges. That escalation is Dijkstra's algorithm (non-negative weights) or Bellman-Ford (negative weights allowed). Graph BFS is exactly Dijkstra with every weight pinned to 1, which is why it gets away with a plain FIFO queue and the cheaper O(V + E) bound.

The **fourth fork — "what is actually being asked" — is where the module's real decision lives**, and it splits five ways:

- **"Shortest / minimum / fewest / nearest," counting hops.** BFS, and this is the one branch where the choice is not stylistic. DFS commits to one direction and can reach the target via a long detour, marking the short route's nodes visited on the way; it returns a real path that is not the shortest one, silently. Making DFS correct means un-marking on backtrack and enumerating every simple path — exponential, not O(V + E).
- **"How many groups / can A reach B / is it connected."** Either traversal, and saying so out loud is the stronger answer. There is no distance anywhere in the question, so BFS's guarantee is unused. Pick DFS for brevity (`problems/01`, `problems/02`) or BFS when recursion depth is a genuine risk. The non-negotiable part is not the traversal — it is the **outer loop over every node as a potential unvisited start**, without which a disconnected component is silently never counted.
- **"Is there a cycle."** Here the traversal choice is forced by the *edge direction*, not by the question. A directed graph needs the three-state (`unvisited`/`in-progress`/`done`) marker of `hasCycleDirected` in [../code.cpp](../code.cpp), because reaching an already-finished node through a second parent is normal in a DAG and proves nothing. An undirected graph needs no third state: any edge to an already-visited non-parent node closes a cycle, and if the question is specifically about **odd** cycles ("is it bipartite"), the two-colouring in [problems/03-is-graph-bipartite.cpp](../problems/03-is-graph-bipartite.cpp) reuses the colour array as the visited array and treats a colour *match* — not the revisit — as the failure.
- **"In what order can these tasks run."** Topological Sort ([../../topological-sort/](../../topological-sort/)) — built on this exact machinery, answering a different question, and requiring a DAG (which is why a cycle check is usually its first step).
- **"Are these two connected, asked repeatedly as edges arrive."** Union Find ([../../union-find/](../../union-find/)), which maintains component membership incrementally and never traverses an edge to answer a query.
