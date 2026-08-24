# Graph BFS/DFS


> **In one line:** Tree BFS's exact queue-and-frontier mechanic, now guarding against revisiting a node — mark a node visited the moment it's *enqueued*, not when it's popped, since a graph (unlike a tree) can offer more than one path to the same node.

```cpp
std::vector<int> dist(n, -1);           // -1 means "not yet reached"
std::queue<int> frontier;
dist[src] = 0;                          // mark visited THE MOMENT we enqueue
frontier.push(src);

while (!frontier.empty()) {
  int node = frontier.front(); frontier.pop();
  for (int neighbor : adj[node]) {
    if (dist[neighbor] == -1) {               // not yet discovered
      dist[neighbor] = dist[node] + 1;
      frontier.push(neighbor);                // mark-on-enqueue, not on-pop
    }
  }
}
```

**O(V + E)** time · **O(V)** space. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Traverse a general graph — one that may contain cycles and have no single root — using the same two traversal shapes as tree BFS/DFS, but guarded by an explicit `visited` marker so the traversal terminates and either the shortest hop-count (BFS) or reachability/structure (DFS) comes out correct.

## Real Life Analogy

Picture a city's road network as a graph: intersections are nodes, roads are edges, and — unlike a tree — you can drive in a loop and end up back where you started.

**BFS is a search-and-rescue team fanning out ring by ring from a starting point.** To find the *nearest* exit from a building, you would not send one scout down a single corridor as far as it goes. You would flood the building with searchers who all take one step at a time: first everyone checks every room one door away from the start, then everyone checks every room two doors away, and so on. The team never doubles back into a room another scout already checked (that room gets a chalk mark — "checked" — the instant a scout is assigned to it, not after they finish searching it). The first ring in which an exit is found is, by construction, the *nearest* exit — you cannot possibly find a closer one two rings later, because every closer ring was already fully searched first.

**DFS is a single explorer following one road as far as it goes before turning back.** Drop one person at an intersection with a piece of chalk. They pick a road, follow it to its end (or until it loops back to a chalk mark they already made), then backtrack to the last intersection with an unexplored road and try that one instead. They eventually visit every reachable intersection exactly once, but they have no idea, mid-exploration, whether the intersection they are standing at is 2 blocks from home or 200 — depth, not distance, is what DFS naturally tracks.

Both explorers carry the same essential tool: a piece of chalk to mark "already been here." Without it, a road that loops back on itself (a cycle) sends either of them around in circles forever.

## Problem

### What engineering problem exists?

Once your data stops being a tree — no single root, and edges can form cycles — two very common questions still need answering:

- **"What is the shortest number of hops from A to B?"** — e.g. the fewest number of intermediate friends connecting two users in a social network, or the fewest number of transformations to turn one word into another.
- **"Can I even reach B from A? Is the whole structure connected? Does it contain a cycle?"** — e.g. is this service reachable from the entry point of a dependency graph, or does this set of task dependencies contain a circular wait that would deadlock a scheduler?

> **Term: Graph.** A collection of **nodes** (also called vertices) connected by **edges**. A graph is **directed** if an edge `u -> v` only lets you travel from `u` to `v` (like a one-way street or a "depends on" relationship), and **undirected** if the edge can be traversed either way (like a two-way road or an "is friends with" relationship). Unlike a tree, a graph can have multiple paths between two nodes, no designated root, and **cycles** — a path that leads back to a node already on that same path.

### Why is this problem difficult?

- **The tree guarantee is gone.** Every tree traversal you have written up to this point silently relied on one fact: you can never revisit a node, because a tree has exactly one path from the root to anywhere, and recursing into a child never leads back to an ancestor. In a graph, that guarantee simply does not exist. A naive port of tree BFS/DFS — same code, no changes — will walk around a cycle indefinitely, because nothing in the traversal logic knows "I have already processed this node."
- **"Shortest path" is not the same as "some path."** DFS happily finds *a* path from A to B, but it can wander down a 40-hop detour before ever trying the 3-hop direct route, because DFS commits to one direction and only backtracks when that direction is fully exhausted. If the question is specifically about the *minimum* number of hops, DFS's traversal order gives you no such guarantee.
- **Disconnected pieces are easy to forget.** A graph does not have to be one connected blob. A social network has users with zero friends yet; a microservice dependency graph has services nobody currently calls. A traversal that starts at one node and stops when it runs out of reachable nodes will silently miss everything in a different, disconnected piece of the graph — and the code will not error, it will just quietly under-report.

### What happens if we ignore it?

- **Infinite loop / stack overflow / process hang.** BFS around an unguarded cycle never empties its queue; DFS around an unguarded cycle recurses forever until the call stack overflows and the process crashes. This is not a "slightly wrong answer" bug — it is a full hang or crash in production, often triggered only once the underlying data (a social graph, a service map) grows a cycle it did not have during testing.
- **Wrong "shortest path" answers.** Using DFS (or an unmarked traversal that revisits nodes) where BFS's ring-by-ring guarantee is required silently returns a path that exists but is not the shortest one — a subtly wrong answer that passes casual testing but fails whenever a genuinely shorter alternate route exists.
- **Undercounted reachability/components.** Forgetting to loop over every node as a potential unvisited start point undercounts connected components or misses that half your graph is unreachable — the kind of bug that only shows up once real data includes a disconnected fragment, which test fixtures built by hand often do not.

## Why Not Other Approaches?

**"Just reuse the tree BFS/DFS code unchanged — a graph traversal is basically the same thing."**
This is the single most common mistake going into this pattern, and it is worth naming directly: tree BFS/DFS never needed a `visited` set because a tree's structure makes revisiting a node impossible by construction (exactly one path from the root to any node, no cycles). The moment you traverse a structure where a node can be reached via more than one path, or where following edges can lead back to a node already on the current path, that same code either **hangs** (BFS's queue never empties, because a cyclic path keeps re-adding the same nodes) or **crashes with a stack overflow** (DFS's recursion never bottoms out, because a cyclic path never runs out of "next" nodes to descend into). The fix is not a new algorithm — it is the same two traversals, plus one added piece of state: an explicit `visited` marker.

**"Use DFS to find the shortest path, since DFS is simpler / more familiar / recursion is elegant."**
DFS finds *a* path, not the *shortest* path, unless you additionally track and compare every path's length — which throws away DFS's main appeal (simplicity) and still costs more work than BFS, which gets the shortest-path guarantee **for free** from its traversal order alone. If the question is explicitly about minimum hops, reaching for DFS is reaching for the wrong tool, not a stylistic choice.

**"Skip the outer loop over all nodes — just start the traversal at node 0 (or whatever the 'obvious' start is)."**
This silently assumes the graph is fully connected. The moment the graph has an isolated node, or a disconnected second cluster, a single traversal from one start point never even looks at it. Any function that claims to answer a graph-wide question ("how many components," "is there a cycle anywhere") must loop over every node as a potential start point and only skip nodes that a previous traversal has already marked visited.

**Tradeoff summary:** none of these are complexity tradeoffs in the way Two Pointers vs. hashing is — they are **correctness** failures. Skipping the `visited` set does not make the algorithm faster or use less memory; it makes it wrong (hang, crash, or silently incomplete). The `visited` set is not an optimization layered on top of tree BFS/DFS — it is the missing piece that makes the traversal *well-defined* on a structure with cycles at all.

## Solution

The mechanism is deliberately unglamorous: it is the exact same two traversals you already know from tree BFS and tree DFS, with one addition — an explicit `visited` set (or boolean array) that every node is checked against before being explored, and marked in the instant it is discovered.

**BFS, guarded.** Start a queue with the source node, mark it visited immediately, and process the queue: pop a node, look at its neighbors, and for each neighbor that is **not yet visited**, mark it visited and push it. Because BFS processes the queue in the order nodes were discovered, it explores the graph in strict "rings" outward from the source — everything at distance 1 is fully processed before anything at distance 2 is even looked at. That ordering is exactly what guarantees the first time you reach any node, you have reached it via a shortest possible path, in an unweighted graph where every edge counts as one hop. (No code yet — this is the shape; the implementation is in [code.cpp](code.cpp).)

**DFS, guarded.** Recurse (or use an explicit stack) into a node's neighbors, marking each node visited the instant you enter it, and skipping any neighbor already marked. This gives you reachability (can this node reach that node — walk the whole subgraph and check), connected-component counting (loop over every node; each unvisited node found starting a fresh traversal is a new component), and — with one extra piece of state, a marker for "on the current path" — cycle detection in a directed graph.

The thinking behind it: **the `visited` set is not a performance optimization — it is a correctness requirement.** Every node must be explored *at most once* for two reasons: first, so the traversal actually terminates on a graph with cycles; second, so the O(V+E) complexity bound holds (each node processed once, each edge examined at most once or twice depending on direction). Skip it, and you do not get a slower correct algorithm — you get an algorithm that does not terminate at all.

## Architecture

The participants in a guarded graph traversal:

1. **The adjacency list.** The graph itself, represented as `adj[u]` = the list of nodes directly reachable from `u` in one edge. This is the standard, memory-efficient representation for graphs that are not densely connected (which is most real graphs — a social network with a million users does not have anywhere close to a million-squared friendships). Its responsibility: answer "who are `u`'s neighbors?" in O(1) to look up the list, then O(degree of u) to iterate it.

2. **The `visited` set/array.** A boolean array (or hash set, if node identities are not small contiguous integers) recording which nodes have already been discovered. Its responsibility: guarantee every node is explored at most once, which is what makes the traversal terminate on a cyclic graph and what keeps the total work bounded by O(V + E) rather than unbounded.

3. **The queue (BFS only).** A FIFO structure holding nodes that have been discovered but not yet had their neighbors examined. Its responsibility: enforce the ring-by-ring processing order — because it is FIFO, every node at distance `d` is dequeued (and its neighbors examined) before any node at distance `d+1` is dequeued, which is the entire mechanism behind the shortest-path guarantee.

4. **Recursion (or an explicit stack), DFS only.** Either the language's call stack (recursive DFS) or a manually managed `std::stack` (iterative DFS) holding the path currently being explored. Its responsibility: remember where to backtrack to once the current branch is exhausted. For directed-cycle detection, this same structure doubles as the "on the current path" marker (see `hasCycleDirected` in [code.cpp](code.cpp)).

Responsibilities in one line each:
- **Adjacency list:** answers "who is reachable from here in one step?"
- **`visited`:** guarantees termination and the "at most once per node" work bound.
- **Queue (BFS):** enforces strict ring-by-ring order, which is what makes "first reached = shortest path" true.
- **Recursion/stack (DFS):** remembers how to backtrack, and — for directed cycle detection — remembers what is still "in progress."

## Execution Flow

**BFS shortest-path / level count**, step by step:

1. Initialize a `dist` array of size `V` (number of nodes), every entry set to a sentinel meaning "unreached" (e.g. `-1`).
2. Set `dist[source] = 0` and push `source` into an empty queue. This is the mark-on-discovery step — `source` is now considered visited.
3. While the queue is not empty:
   a. Pop the front node, call it `current`.
   b. For each neighbor of `current`:
      - If `dist[neighbor]` is still the "unreached" sentinel, set `dist[neighbor] = dist[current] + 1` and push `neighbor` onto the queue.
      - If `dist[neighbor]` is already set, skip it — it was already reached via an equal-or-shorter path, because BFS processes nodes in non-decreasing distance order.
4. When the queue empties, `dist[v]` holds the minimum hop count from `source` to every reachable `v`; unreached nodes keep the sentinel value.

**DFS reachability and connected components**, step by step:

1. Initialize a `visited` array of size `V`, all `false`.
2. To check reachability (can `source` reach `target`): start a recursive DFS at `source`. On entering any node, mark it visited, then recurse into every neighbor that is not yet visited. If the recursion ever visits `target`, reachability is confirmed; if the DFS fully unwinds without visiting `target`, it is unreachable.
3. To count connected components: initialize a component counter to 0. Loop over every node `i` from `0` to `V-1`. If `i` is not yet visited, increment the counter (a new component has been found) and run a full DFS starting at `i`, marking every node it reaches. Continue the loop; nodes already marked visited by a previous component's DFS are skipped.
4. After the loop finishes, the counter holds the total number of connected components, and every node has been visited exactly once regardless of which component it belonged to.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full flowchart deciding between Graph BFS/DFS, Topological Sort, and Union Find based on the signals in a problem statement.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the visited-set-guarded BFS queue loop (with DFS's equivalent recursion flow described alongside it).

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of BFS expanding ring-by-ring from a source node on a small concrete graph with a cycle.

## Implementation

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the *shape* of the pattern clearly, separated from any one problem's details, before looking at the worked, problem-specific solutions in [problems/](problems/).

It provides three small, reusable functions, all operating on a graph represented as `std::vector<std::vector<int>> adj` (an adjacency list):

- `bfsShortestPath(adj, src)` — returns a `std::vector<int>` of minimum hop-distances from `src` to every node, with `-1` for unreachable nodes.
- `dfsConnectedComponents(adj)` — returns the number of connected components, treating the graph as undirected (it symmetrizes the adjacency internally, so it is correct even if the caller only supplied one-directional edges).
- `hasCycleDirected(adj)` — returns whether a **directed** graph contains a cycle, using DFS with a three-state (`unvisited` / `in-progress` / `done`) marker instead of a plain boolean, because in a directed graph revisiting an already-fully-explored node is normal and does not by itself indicate a cycle.

No templates over element type here (unlike Two Pointers' container-generic functions) — a graph node is always represented as a small integer index into `adj`, so the functions are already maximally reusable across any problem that can be reduced to that representation.

## Code Walkthrough

**`bfsShortestPath`** (in [code.cpp](code.cpp)). Takes the adjacency list and a source index. Initializes every distance to `-1` ("unreached"), sets `dist[src] = 0`, and pushes `src` onto a `std::queue<int>`. The main loop pops a node and, for every neighbor still at `-1`, sets its distance to one more than the current node's distance and pushes it. The critical line is `dist[neighbor] = dist[node] + 1` happening **before** the push, and the `-1` check happening **before** that — this is "mark on discovery," which is what prevents the same neighbor from being pushed twice by two different in-progress nodes that both point to it. This function exists to demonstrate the exact-shortest-hops BFS in its purest form, and is the direct ancestor of the multi-source BFS shape (seeding the queue with *every* source at distance 0 at once — the classic example is LeetCode 994, Rotting Oranges) and [problems/04-word-ladder.cpp](problems/04-word-ladder.cpp) (which generalizes the "neighbor" relationship to "differs by one letter").

**`dfsConnectedComponents`** (in [code.cpp](code.cpp)). First builds a symmetric adjacency list (`undirected`) by adding both `u -> v` and `v -> u` for every edge found in the input `adj`, so the function gives a correct undirected-component count regardless of how the caller built the original adjacency list. Then it runs a straightforward recursive DFS (marking each node visited the instant it is entered) inside an outer `for` loop over every node from `0` to `V-1` — the outer loop is what correctly handles disconnected pieces: any node not yet visited by a previous component's DFS starts a brand-new component and increments the counter. This function exists to demonstrate both the plain "mark-visited" DFS shape and the "loop over all nodes as potential starts" discipline that connected-component and cycle-detection problems both require, and is the direct ancestor of [problems/01-number-of-islands.cpp](problems/01-number-of-islands.cpp) (where each "island" is a connected component in a grid-as-implicit-graph).

**`hasCycleDirected`** (in [code.cpp](code.cpp)). Uses a `state` array with three values instead of a boolean: `0` (unvisited), `1` (in progress — on the current DFS path), `2` (done — fully explored, and *not* on the current path anymore). The recursive `dfs` lambda sets `state[node] = 1` on entry, and for each neighbor: if the neighbor's state is `1`, that is a **back edge** to a node still on the current path — a cycle, so it returns `true` immediately. If the neighbor's state is `0`, it recurses. If the neighbor's state is `2`, it is safely skipped (already fully explored via a different path, and not part of any cycle involving the current path). After all neighbors are processed, `state[node]` is set to `2`. This function exists to demonstrate why directed-graph cycle detection needs a third state beyond plain `visited`/`unvisited` — the same outer "loop over every unvisited node" discipline as `dfsConnectedComponents` ensures a cycle hidden in a disconnected second component is still found.

**`main()`** (in [code.cpp](code.cpp)). Exercises all three functions against small, hand-checkable graphs — including a 4-cycle for BFS (proving it does not loop forever and still returns correct hop counts), a disconnected 3-component graph for `dfsConnectedComponents` (built with one-directional edges, proving the internal symmetrization works), and both a cyclic and an acyclic (diamond-shaped) directed graph for `hasCycleDirected`, plus a case where the cycle is hidden in a second, otherwise-unreached component — and prints `[PASS]`/`[FAIL]` for each assertion.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem, implementing the same visited-set-guarded traversal logic inline (not calling the generic functions above directly, so each file stays dependency-free and independently readable). See [problems/README.md](problems/README.md) for the index. Briefly: `01` treats a grid as an implicit graph (each cell's up/down/left/right neighbors) and counts connected components via DFS flood-fill; `02` clones a graph via BFS while using a hash map to avoid infinite recursion on cycles and to avoid cloning the same node twice; `03` is multi-source BFS, where every initially-rotten orange starts the queue simultaneously, and the number of BFS "rings" processed is the answer; `04` is BFS shortest-path where the "neighbor" relationship is generated on the fly (every one-letter mutation of the current word) rather than read from a pre-built adjacency list.

## Advantages

- **Same mental model as tree BFS/DFS.** If you already understand level-order and depth-first traversal on trees, this pattern is "that, plus one guard" — not a new algorithm to learn from scratch.
- **BFS gives an exact, provable shortest-path guarantee for free** in any unweighted graph, with no extra bookkeeping beyond the distance array itself.
- **DFS is naturally suited to structural questions** — reachability, connected components, cycle detection — because "fully explore one branch before trying the next" maps directly onto "has everything reachable from here been accounted for?"
- **O(V + E) time and space**, for both traversals — linear in the size of the graph, which is close to the theoretical minimum for any algorithm that must look at every node and edge at least once.
- **Composable.** Connected-component counting and cycle detection are both "DFS plus a small piece of extra state" (a counter, or a three-state marker) layered on the exact same traversal skeleton — you are not learning three unrelated algorithms.

## Disadvantages

- **BFS needs O(V) space for the queue and the visited set**, in the worst case (a "star" graph where the source connects directly to almost every other node puts almost all of them in the queue at once). This is unavoidable — it is not a tuning problem, it is the cost of remembering an entire ring of nodes before moving to the next ring.
- **DFS's recursion depth can reach V in a long, thin path** (a graph that is basically a long chain), risking a **stack overflow** on graphs with tens of thousands of nodes or more — a real production failure mode, not a theoretical one, especially on default thread stack sizes. An iterative DFS with an explicit `std::stack` avoids this at the cost of slightly more code.
- **Neither traversal alone handles weighted edges correctly.** BFS's "first reached = shortest" guarantee is only true when every edge counts as exactly one hop; the moment edges have different costs, BFS's ring-by-ring order no longer corresponds to "closest in total cost" (see Complexity, below).
- **Disconnected graphs require remembering the outer loop.** Every function that claims a graph-wide answer (all components, any cycle anywhere) must loop over every node as a potential unvisited start point — easy to forget, and the resulting bug (under-counting) does not crash or throw, it just quietly returns a wrong number.

## Tradeoffs

**What we gain over the naive (unguarded) traversal:** correctness on any graph, cyclic or not — termination is no longer an accident of the input happening to be tree-shaped.

**What we gain from choosing BFS over DFS (when shortest hops matter):** an exact, provable minimum-hop-count guarantee, at the cost of needing an explicit queue and a full distance array rather than DFS's simpler "just recurse" structure.

**What we gain from choosing DFS over BFS (when structure/reachability matters):** simpler code (often just a recursive function plus a `visited` array), and a natural fit for "fully explore this branch, then backtrack" questions — at the cost of no shortest-path guarantee at all.

**What we lose versus a tree traversal:** the ability to skip the `visited` set entirely. Every graph traversal pays a small, constant bookkeeping cost (one array, one queue or one recursion-stack marker) that a tree traversal never needed — a fixed tax for giving up the "no revisits possible" guarantee.

## Complexity

**Time:** **O(V + E)** for both BFS and DFS, where `V` is the number of nodes and `E` is the number of edges. Every node is enqueued/recursed into at most once (thanks to the `visited` guard), and every edge is examined at most once (undirected: twice, once from each endpoint, but that is still a constant factor, not a growth-rate change).

**Space:** **O(V)** for both — the `visited` array/set is always O(V); BFS additionally needs O(V) for the queue in the worst case (a wide, shallow graph); DFS additionally needs O(V) in the worst case for the recursion stack (a long, narrow graph, i.e. close to a straight-line path).

**Contrast with Dijkstra's algorithm:** the instant edges have different weights (a road network with distances, a service call graph with latencies), BFS's ring-by-ring order stops corresponding to "cheapest total cost" — a 1-hop edge with weight 100 is not better than a 3-hop path whose edges total 10. Answering "shortest weighted path" then requires **Dijkstra's algorithm**, which replaces the plain queue with a **min-priority-queue** (typically a binary heap) ordered by running total cost, at a complexity cost of **O((V + E) log V)** — the `log V` factor is the price of always extracting the currently-cheapest frontier node instead of simply processing discovery order. Graph BFS is a special case of Dijkstra where every edge weight is implicitly 1, which is exactly why BFS can use a plain FIFO queue instead of a heap and gets away with the cheaper O(V + E) bound.

## Common Mistakes

- **Marking a node visited when it is dequeued/popped, instead of when it is enqueued/pushed.** If two different in-progress nodes both point to the same unvisited neighbor, and that neighbor is only marked visited once it is popped, both nodes will push it onto the queue before either processing catches up — the same node ends up enqueued multiple times, wasting work and, in a distance/level-counting context, potentially recording an incorrect distance. *Avoid:* mark visited (and, for BFS, set its distance) in the same step where you decide to enqueue it — the instant it is *discovered*, not when it is later processed.
- **Not handling disconnected components** — running a single traversal from one assumed start node and treating "queue empty" or "recursion returned" as "done with the whole graph." *Avoid:* any function claiming a graph-wide answer (component count, "does any cycle exist," "is the whole graph reachable from anywhere") must loop over every node as a potential unvisited start point.
- **Using DFS when the question actually needs a shortest-path guarantee.** DFS will return *a* path — often not the shortest one — and the bug will not throw an error; it will just quietly return a longer answer than the correct one whenever a shorter alternate route exists in the test data. *Avoid:* the moment the words "shortest," "minimum," "fewest steps," or "nearest" appear in the problem, default to BFS, not DFS.
- **Using a plain `visited` boolean for directed-cycle detection.** A directed graph can (and often does) have a node reachable via two different parents without that implying a cycle — reaching an already-visited node is not automatically a cycle unless that node is still on the *current* recursion path. *Avoid:* use the three-state (`unvisited` / `in-progress` / `done`) marker shown in `hasCycleDirected`, not a two-state boolean.
- **Forgetting recursion depth limits on DFS for large, deeply-chained graphs.** A DFS written recursively over a graph that happens to be a long chain of tens of thousands of nodes can blow the call stack in production even though the algorithm is textbook-correct. *Avoid:* for graphs where depth could realistically be large, use an iterative DFS with an explicit `std::stack<int>` instead of language recursion.

## When To Use

- You need the **minimum number of hops/edges** between two nodes in an **unweighted** graph — BFS.
- You need to know whether one node can **reach** another, whether the graph (or a subgraph) is **fully connected**, or how many **connected components** it has — DFS (or BFS; either traversal order works fine for pure reachability/component-counting, since neither needs the shortest-path guarantee).
- You need to detect a **cycle** — in a directed graph (dependency graphs, task schedulers, deadlock detection) via DFS with a three-state marker, or in an undirected graph via DFS/BFS with a simple "is this neighbor visited and not my immediate parent" check.
- The graph is given as (or can be built into) an **adjacency list**, and the number of edges is not so large that even O(V + E) work is prohibitive.
- You are asked to solve a grid problem (islands, flood fill, maze shortest path) — a 2D grid is almost always an **implicit graph** where each cell is a node and its up/down/left/right neighbors are edges (see [problems/01-number-of-islands.cpp](problems/01-number-of-islands.cpp)).

## When NOT To Use

- **Edges have weights and hop-count is not the same as shortest total distance.** BFS's "first reached = shortest" guarantee only holds when every edge costs exactly 1. The moment edges carry different weights (distances, latencies, costs), you need **Dijkstra's algorithm** (non-negative weights) or **Bellman-Ford** (weights can be negative, and it can also detect negative cycles) instead — plain BFS will confidently return a wrong "shortest path."
- **You need the shortest path in a graph with negative edge weights.** Neither BFS nor Dijkstra handles negative weights correctly (Dijkstra's greedy "extract cheapest, never revisit" assumption breaks); that calls for Bellman-Ford.
- **The question is about ordering under dependency constraints** ("in what order must these tasks run given their prerequisites?") rather than distance or reachability — that is Topological Sort's job, built on top of this same traversal machinery but answering a different question.
- **Edges arrive incrementally and you repeatedly need "are these two nodes connected?" or "would adding this edge create a cycle?"** without wanting to re-run a full traversal every time — that is Union Find's job, which answers connectivity queries without traversing edges at all.
- **The graph is so large that even O(V + E) is too slow for the required latency** (e.g. a real-time query over a graph with billions of edges) — that typically calls for precomputed indexes, bidirectional search, or specialized graph databases, not a from-scratch BFS/DFS per query.

## Real Interview/Production Examples

- **Social network "degrees of connection"** — LinkedIn's "2nd/3rd degree connection" labels and "how are you connected to this person" features are BFS shortest-path queries over the friendship/follow graph, where each ring outward from you is one additional degree.
- **Network routing and broadcast storms.** Network topology discovery and basic routing protocols reason about a graph of routers/switches; BFS-style flooding is literally how a **broadcast storm** happens on a network with a physical cycle and no loop-prevention protocol (which is exactly why Ethernet networks run the **Spanning Tree Protocol** — to detect and break cycles in the physical topology before a broadcast packet can circulate forever, the network-hardware analogue of forgetting a `visited` set).
- **Dependency / service-mesh reachability analysis.** In a microservice architecture, "if service A goes down, which services become unreachable from the public API gateway" is a reachability question answered by DFS/BFS over the service-call graph; "is there a circular dependency between these services that could deadlock a startup sequence" is directed-cycle detection over the same graph.
- **Build systems and package managers.** Determining "does installing/building this set of packages have a circular dependency" is exactly `hasCycleDirected` run over the package dependency graph — the same check that underlies why `npm install` or a Makefile can report a dependency cycle error instead of hanging forever.
- **Maze/pathfinding in games and robotics.** Finding the shortest route through a grid-based maze (a game level, a warehouse robot's floor plan) is BFS over the grid-as-implicit-graph, identical in shape to [problems/01-number-of-islands.cpp](problems/01-number-of-islands.cpp)'s traversal but answering "shortest route" instead of "how many separate blobs."

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **"Degrees of separation" between two entities** in any graph-shaped domain model you own (users and their connections, organizations and their reporting chains) — a straightforward BFS shortest-path query over an adjacency list built from a relational "edges" table.
2. **Circular-dependency detection in a service registry or config graph** — run `hasCycleDirected` over a "service X depends on service Y" graph before allowing a new dependency to be registered, catching a deploy-time deadlock before it happens instead of after.
3. **Reachability impact analysis for an incident**: given "node X is down," BFS/DFS outward over the dependency graph to compute the full set of downstream services that are now unreachable, to page the right on-call owners immediately.
4. **Flood-fill style batch jobs over a grid or spatial index** — e.g. computing contiguous regions in a geospatial dataset, or connected "blobs" of flagged records in a 2D sensor grid, using the same island-counting DFS as [problems/01](problems/01-number-of-islands.cpp).
5. **Multi-source BFS for "time until every node is affected"** style batch jobs — e.g. simulating how many processing cycles it takes for a status (a cache invalidation, a config rollout) to propagate outward from several seed nodes simultaneously through a dependency graph, directly analogous to the multi-source BFS shape (LeetCode 994, Rotting Oranges).

## Similar Patterns

- **Tree BFS** ([../tree-bfs/](../tree-bfs/)) and **Tree DFS** ([../tree-dfs/](../tree-dfs/)): the exact same two traversal shapes, minus the `visited` set, because a tree's "exactly one path from the root" structure makes revisiting a node structurally impossible. Graph BFS/DFS is these two patterns generalized to structures where that guarantee no longer holds.
- **Topological Sort** ([../topological-sort/](../topological-sort/)): builds directly on top of this pattern's traversal machinery (Kahn's algorithm is BFS with an in-degree counter instead of a plain `visited` array; the DFS-based approach is this pattern's DFS plus a post-order reversal), but answers a different question — not "how far / is it reachable," but "in what valid order can these dependent tasks run." A directed graph with a cycle has *no* valid topological order at all, which is exactly why `hasCycleDirected` (this module) is often the first check a topological sort implementation runs.
- **Union Find (Disjoint Set)** ([../union-find/](../union-find/)): answers connectivity questions ("are these two nodes in the same component," "would adding this edge create a cycle") **without traversing edges at all** — it maintains component membership incrementally as edges are added, at roughly O(α(V)) (near-constant) per query, versus this pattern's O(V + E) per full traversal. Reach for Union Find when edges arrive one at a time and you need repeated connectivity queries; reach for Graph BFS/DFS when you have the whole graph up front and need to traverse it (for shortest path, or a one-time structural analysis).

| Pattern | Structure guarantee | Core question answered | Typical complexity |
|---|---|---|---|
| Tree BFS / Tree DFS | Tree (one root, no cycles) | Level order / path order / subtree properties | O(N) |
| Graph BFS/DFS | General graph, possibly cyclic | Shortest hops (BFS) / reachability, components, cycles (DFS) | O(V + E) |
| Topological Sort | Directed **acyclic** graph (DAG) | Valid ordering under dependency constraints | O(V + E) |
| Union Find | Any graph, edges added incrementally | "Same component?" / "would this edge create a cycle?" — no traversal | ~O(α(V)) per query (near-constant) |

## Interview Discussion

Experienced engineers do not spend interview time on "how do you write a BFS loop" — that is mechanical. What they actually probe is whether you can correctly identify **which** traversal the question needs, and whether you remember the guard that a tree traversal never needed. A candidate who says "I'll DFS to find the shortest path" without justifying why that gives the minimum is signaling they have memorized code shapes rather than understood what each traversal order guarantees.

Common follow-up questions:
- *"Why BFS and not DFS for shortest path?"* — expects the ring-by-ring argument: BFS exhausts every node at distance `d` before looking at distance `d+1`, so the first time a node is reached is provably via a shortest path; DFS has no such ordering guarantee.
- *"What if the graph has a cycle — walk me through what would go wrong without a `visited` set."* — expects a concrete failure mode: BFS's queue keeps re-adding nodes already on the cycle and never empties; DFS's recursion keeps re-entering nodes already on the cycle and eventually overflows the stack.
- *"How would you detect a cycle in a directed graph, and why can't you just use a boolean `visited` array like you did for the undirected case?"* — expects the three-state (`unvisited`/`in-progress`/`done`) explanation and a concrete example of a DAG where a node is legitimately revisited via two parents without any cycle existing.
- *"The graph is disconnected — does your solution still work?"* — expects recognition that any graph-wide question requires looping over every node as a potential unvisited start point, not assuming one traversal from an arbitrary start covers everything.
- *"What changes if the edges have weights?"* — expects naming Dijkstra's algorithm (and the priority-queue-driven O((V+E) log V) bound) as the correct escalation, and recognizing that BFS is really just Dijkstra specialized to all-edge-weights-equal-1.

Common misconceptions:
- "BFS and DFS always give the same answer, just in different order." False for shortest-path questions — DFS gives no shortest-path guarantee at all, only *a* path.
- "A `visited` array is an optimization to avoid extra work." It is a **correctness** requirement on any graph that might contain a cycle — without it, the algorithm may never terminate, not just run slower.
- "If I've visited a node once in a directed graph, seeing it again always means a cycle." False — it only means a cycle if that node is still on the *current* DFS path (state "in progress"), not merely visited at some point in the past (state "done").
- "Graph traversal complexity is the same as tree traversal complexity, O(N)." It is O(V + E) — on a dense graph, `E` can be far larger than `V` (up to O(V²)), so the edge term is not always negligible the way it is in a tree (where E = V - 1 always).

## Summary

- Graph BFS/DFS is the same two traversal shapes as tree BFS/DFS, plus an explicit `visited` set/array that a tree never needed because it cannot have cycles.
- BFS explores in strict rings outward from a source; the first time any node is reached is guaranteed to be via a shortest path, in an unweighted graph.
- DFS explores one branch fully before backtracking; it is the natural fit for reachability, connected-component counting, and (with a three-state marker) directed-cycle detection.
- Both run in **O(V + E)** time and O(V) space — linear in the size of the graph.
- The moment edges carry different weights, BFS's shortest-hop guarantee stops meaning "shortest distance" — that escalation is **Dijkstra's algorithm**, at O((V + E) log V).
- The single most common bug: marking a node visited on dequeue/pop instead of on enqueue/push, which allows duplicate enqueues of the same node.
- The single most common omission: forgetting to loop over every node as a potential start point, which silently under-counts on a disconnected graph.
- Directed-cycle detection needs a three-state marker (`unvisited`/`in-progress`/`done`), not a plain boolean, because revisiting an already-fully-explored node in a directed graph is normal and not itself proof of a cycle.

## Key Takeaways

1. Graph BFS/DFS = tree BFS/DFS + an explicit `visited` set, because a general graph can have cycles a tree structurally cannot.
2. BFS explores ring-by-ring outward from a source; that ordering is exactly why it guarantees the shortest hop-count in an unweighted graph.
3. DFS explores one branch fully before backtracking; use it for reachability, connected components, and directed-cycle detection — not for shortest-path guarantees.
4. Mark a node visited the instant it is discovered (enqueued/pushed), never when it is later processed (dequeued/popped) — the single most common correctness bug in this pattern.
5. Any function claiming a graph-wide answer (component count, "any cycle anywhere") must loop over every node as a potential unvisited start point, to correctly handle disconnected pieces.
6. Directed-cycle detection needs a three-state (`unvisited`/`in-progress`/`done`) marker, not a plain boolean — a revisited-but-fully-explored node is not automatically a cycle.
7. Both traversals are O(V + E) time, O(V) space — linear in graph size, and near-optimal for anything that must examine every node and edge.
8. The moment edges have weights and hop-count no longer equals shortest distance, escalate to Dijkstra's algorithm (O((V + E) log V)) — plain BFS silently gives a wrong answer, it does not error out.
9. DFS's recursion depth can reach V on a long, chain-like graph — a real stack-overflow risk in production; switch to an explicit iterative stack for graphs where depth could be large.
10. A 2D grid (islands, flood fill, maze pathfinding) is almost always an implicit graph — each cell is a node, its up/down/left/right neighbors are edges — and this whole pattern applies directly.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — the canonical treatment of BFS, DFS, and their O(V + E) complexity proofs, including the classification of tree/back/forward/cross edges that underlies directed-cycle detection.
- *Algorithms* (4th Edition) — Robert Sedgewick & Kevin Wayne — clear, practical coverage of graph representations, BFS/DFS, and connected components, with Java code that maps directly onto the same ideas in C++.
- *Competitive Programmer's Handbook* — Antti Laaksonen — concise, implementation-focused coverage of BFS/DFS, including common competitive-programming variants like multi-source BFS.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — includes graph BFS/DFS problems (including "route between two nodes") with interview-style framing.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including BFS/DFS graph traversal implementations useful for seeing varied implementation styles.
- Boost Graph Library (BGL) — a production-grade C++ graph library implementing BFS, DFS, and many other graph algorithms with a generic, iterator-based interface; worth reading to see how a real, heavily-used library structures graph algorithms at scale.

**Official Documentation**
- LeetCode — Number of Islands (problem 200).
- LeetCode — Clone Graph (problem 133).
- LeetCode — Rotting Oranges (problem 994).
- LeetCode — Word Ladder (problem 127).
- cppreference.com — `std::queue`, `std::stack` — the standard library containers used to implement BFS's frontier and iterative DFS's explicit stack.

**Blog Articles**
- GeeksforGeeks — "Breadth First Search or BFS for a Graph" and "Depth First Search or DFS for a Graph" — widely used explainers covering the general traversal mechanics and complexity.
- NeetCode — Graph traversal pattern videos/playlist — walks through Number of Islands, Clone Graph, Rotting Oranges, and Word Ladder with visual explanations.
- Educative.io — "Grokking the Coding Interview" Graphs chapter — a widely referenced pattern-based framing of BFS/DFS as a reusable template rather than a one-off algorithm.
