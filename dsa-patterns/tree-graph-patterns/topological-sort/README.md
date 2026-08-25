# Topological Sort


> **In one line:** Kahn's algorithm: repeatedly peel off nodes with zero remaining prerequisites, decrementing their neighbours' in-degree as they're removed — if some nodes never reach in-degree 0, the graph has a cycle.

```cpp
std::queue<int> readyQueue;
for (int node = 0; node < numNodes; ++node) {
  if (inDegree[node] == 0) readyQueue.push(node);   // no prerequisites: safe to place first
}

while (!readyQueue.empty()) {
  int current = readyQueue.front(); readyQueue.pop();
  order.push_back(current);
  for (int neighbor : adj[current]) {
    if (--inDegree[neighbor] == 0) readyQueue.push(neighbor);   // one fewer unmet prerequisite
  }
}
bool hasCycle = order.size() != static_cast<size_t>(numNodes);   // some nodes never got placed
```

**O(V + E)** time · **O(V + E)** space. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Produce a linear ordering of the nodes of a directed graph such that every directed edge `u -> v` places `u` before `v` in the output — and detect, rather than silently mis-order, the case where no such ordering exists.

## Real Life Analogy

Think about **getting dressed in the morning**. Socks must go on before shoes. An undershirt must go on before a shirt. A belt goes on after trousers. None of these constraints say "socks must be item #1 of the day" — they only say "this must come before that." Given all the constraints together, there are several perfectly valid orders you could get dressed in: socks, undershirt, shirt, trousers, belt, shoes — or undershirt, shirt, socks, trousers, belt, shoes. Both satisfy every "must come before" rule. What would **not** be valid is putting shoes on before socks, no matter what else you do around it.

Now imagine someone hands you a broken instruction: "put your belt on before your trousers, and your trousers on before your belt." That is not a hard morning — it is an impossible one. No order exists that satisfies both rules simultaneously, because they contradict each other. This is the second half of the problem: recognizing that some constraint sets have **no valid order at all**, and saying so, rather than getting stuck trying to place the same two items forever.

Topological Sort is exactly this: given a set of items and a list of "this must happen before that" constraints, produce **one** valid order that respects every constraint — or determine that the constraints are self-contradictory (a **cycle**) and no valid order exists.

## Problem

### What engineering problem exists?

A huge class of real problems is really "schedule these things so every dependency happens before its dependents":

- **Course prerequisites.** "Data Structures" requires "Intro to Programming" first. Given a full prerequisite list across a curriculum, produce *some* order a student could take all courses in without ever taking a course before its prerequisite.
- **Build systems.** A `Makefile` or a Bazel `BUILD` file says "target C depends on target B, which depends on target A." The build tool must compile A, then B, then C — never the other way around.
- **Package managers.** `npm install` (or `pip`, or `cargo`) must install a package's dependencies before the package itself, across a dependency graph that can be arbitrarily deep.
- **Spreadsheet recalculation.** If cell `C3` contains `=A1+B2`, then `A1` and `B2` must be recomputed before `C3` whenever any of them changes.

> **Term: Directed graph.** A set of **nodes** (also called vertices) connected by **edges**, where each edge has a direction — `u -> v` means "there is a relationship pointing from `u` to `v`," not the reverse. In a prerequisite graph, an edge `u -> v` conventionally means "`u` must come before `v`."

> **Term: DAG (Directed Acyclic Graph).** A directed graph that contains **no cycles** — there is no way to start at some node, follow directed edges, and return to that same node. A valid topological order exists **if and only if** the graph is a DAG. This "if and only if" is the single most important fact in this entire pattern.

The engineering problem, stated precisely: given `N` items and a list of directed "must come before" edges between them, produce a permutation of all `N` items such that for every edge `u -> v`, `u` appears earlier than `v` in the permutation — or report that no such permutation exists.

### Why is this problem difficult?

- **The naive instinct is to try orders and check them.** Given a candidate order, checking whether it respects all edges is easy (one pass over the edges, verifying each `u` appears before its `v`). But *finding* a valid order by generating candidates and checking them is a different problem entirely — the number of possible orderings of `N` items is `N!` (N factorial), and there is no obvious way to guess a good candidate without already understanding the dependency structure.
- **You must process items in an order that depends on the very order you are trying to compute.** This feels circular at first: you cannot safely place an item in the output until every one of its prerequisites is already placed — but you do not know which items have no unplaced prerequisites left until you have already placed some others. The algorithm has to discover a *safe-to-place-next* set incrementally, not decide it all up front.
- **Detecting "no valid order exists" is not optional — it is the whole other half of the problem.** A naive implementation can silently produce a partial, wrong, or truncated order when the input contains a cycle, and unless you explicitly check for that condition, the bug is invisible until something downstream (a build, a course plan, a spreadsheet) breaks in production.

### What happens if we ignore it?

- **Trying every permutation is exponential.** For even a modest 15-item dependency list, `15!` is over a trillion — completely infeasible to enumerate and check, where a correct algorithm solves the same problem in a single linear pass.
- **A cyclic dependency set can hang a naive scheduler.** If your build system or course planner does not detect cycles, it can loop forever trying to find "the next thing with no unmet prerequisites," because a cycle guarantees there will always be at least one item stuck waiting on another item that is itself stuck waiting on it.
- **Silently returning a partial or arbitrary order on a cyclic input is worse than crashing.** A build tool that ships half-built artifacts because it silently gave up mid-cycle, or a course planner that tells a student a schedule is valid when two of their courses secretly require each other, causes damage that surfaces much later and far from the actual bug.

## Solution

The key insight: an item is safe to place in the output the moment **all of its prerequisites have already been placed**. Track that fact directly instead of guessing.

> **Term: In-degree.** For a node `v`, the in-degree is the number of directed edges pointing *into* `v` — i.e., the number of prerequisites `v` still has. A node with in-degree 0 has no unmet prerequisites and is safe to process right now.

**Kahn's algorithm** (the BFS-based approach this module builds around), step by step:

1. Build the adjacency list `adj` from the edge list: for every edge `u -> v`, append `v` to `adj[u]`.
2. Compute the in-degree of every node by scanning all edges once: initialize `inDegree[v] = 0` for every node, then increment `inDegree[v]` for each edge `u -> v`.
3. Seed a queue with every node whose in-degree is 0 — these have no prerequisites at all and can go first.
4. While the queue is not empty: pop a node `u`, append it to the output order (`u` is now finalized), then for every neighbor `v` in `adj[u]`, decrement `inDegree[v]` by 1 — because one of its prerequisites (`u`) is now satisfied — and if `inDegree[v]` just became 0, push `v` onto the queue.
5. When the queue empties, compare the output list's length to the total node count. **Equal** means every node was placed and the list is a valid topological order. **Shorter** means the remaining, never-placed nodes are stuck in a cycle — none of them ever reached in-degree 0, because each was waiting on another node in the same cycle that was, in turn, waiting on it. Report failure rather than returning the partial list as if it were complete.

That stuck state is exactly how "no valid order exists" is detected, and it falls directly out of the algorithm's own bookkeeping — no separate cycle-detection pass is needed.

There is a second, equally standard way to get a topological order: **DFS post-order, reversed.** Run a depth-first search from every unvisited node; whenever a node's DFS call finishes (all its descendants have been fully explored), push it onto a stack. When all nodes have been visited, popping the stack (equivalently, reversing the order nodes finished in) gives a valid topological order. Cycle detection in this version requires tracking nodes currently "on the recursion stack" (the "in-progress" / gray-colored nodes in the classic white/gray/black DFS coloring) — if DFS ever reaches a node that is already gray, that back-edge proves a cycle exists. This module implements Kahn's algorithm because its cycle-detection signal (final order length versus node count) is simpler to state and to get right than DFS's recursion-stack tracking, but both approaches are standard and interviewers expect you to at least know the DFS alternative exists.

## Architecture

Kahn's algorithm has three moving structures, each with one clear job:

1. **In-degree array (`inDegree`).** One counter per node, tracking how many not-yet-placed prerequisites it currently has. This is the single source of truth for "is this node safe to process yet?" — a node is safe exactly when its counter reaches 0. It starts as a direct count of incoming edges and is decremented as prerequisites get placed.

2. **Adjacency list (`adj`).** For each node `u`, the list of nodes `v` such that `u -> v` is an edge — i.e., the nodes that depend on `u`. This is what lets the algorithm answer "which nodes' in-degree should I decrement now that `u` is placed?" in time proportional to `u`'s own out-degree, not by rescanning the whole edge list.

3. **The queue of currently-removable nodes.** Holds every node whose in-degree is currently 0 but has not yet been placed in the output. This is the algorithm's "worklist" — it always contains exactly the set of nodes that are legal to place right now, and nothing else. A plain FIFO queue is enough because *any* node in it is equally valid to place next (there is no preference among them for a "any valid order" answer); a min-heap in its place gives you the *lexicographically smallest* valid order instead, at the cost of `O(log V)` per operation instead of `O(1)`.

4. **The output order list.** The accumulated topological order so far, built by appending nodes as they are popped from the queue. Its final length is also the cycle-detection signal: if it ends up shorter than the total node count, some nodes never reached in-degree 0, which can only happen if they were caught in a cycle.

Responsibilities in one line each:
- **In-degree array:** the ground truth for "how many prerequisites does this node still have?"
- **Adjacency list:** answers "who depends on this node?" so removing it can update the right counters.
- **Queue:** always holds exactly the currently-processable nodes — the algorithm's frontier.
- **Output list:** the accumulating answer, whose final length doubles as the cycle check.

## Why Not Other Approaches?

**"Try every permutation of items and check which ones respect all the constraints."**
Correct, but `O(N!)` time. Even ignoring runtime, this approach gives you no insight into *why* an order works or fails — you are blindly guessing and verifying rather than reasoning about the dependency structure. Utterly impractical past a handful of items.

**"Randomly shuffle the list, then repeatedly swap violating adjacent pairs until no violations remain."**
This resembles bubble sort applied to a partial (not total) order. There is no guarantee of termination in a reasonable number of passes, no guarantee two arbitrary items are even comparable (topological order is a **partial** order — many pairs of unrelated items have no required relationship at all), and — critically — this approach has **no way to detect a cycle**. It will either loop indefinitely or converge on a state that still has a violation, with no signal telling you the input was actually invalid.

**"Sort items by some heuristic proxy, like alphabetically or by how many total dependents they have."**
This ignores the actual constraint structure entirely. Alphabetical order has nothing to do with prerequisite relationships; a "sort by dependency count" heuristic can still place an item before one of its direct prerequisites if the heuristic does not exactly encode the edge relationships (and if it did exactly encode them, you would already have solved the problem).

**Net:** every alternative either pays an exponential cost (brute-force permutation checking), offers no termination or cycle-detection guarantee (repeated shuffle-and-fix), or ignores the actual dependency graph in favor of an unrelated heuristic (proxy sorting). What all of them lack is a way to *directly compute*, from the edges themselves, which items are safe to place next — that direct computation is exactly what Topological Sort provides.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart distinguishing Topological Sort from plain Graph BFS/DFS and from Union Find, based on the signals in a problem statement.
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of Kahn's algorithm (compute in-degrees, seed the queue, pop/emit/decrement/enqueue, check final order length).
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of in-degree values and queue contents on a concrete 5-course prerequisite graph.

## The Code

[code.cpp](code.cpp) provides one generic, reusable function, deliberately separated from any single LeetCode problem so the pattern's shape is visible on its own: `topologicalSort(numNodes, edges)` — takes the total number of nodes (numbered `0` to `numNodes - 1`) and a list of directed edges `{u, v}` meaning "`u` must come before `v`," and returns a small struct containing the computed order and a `hasCycle` boolean. The function is intentionally graph-shape-agnostic: it does not know or care whether the nodes represent courses, build targets, or spreadsheet cells — that mapping is the caller's job (see [problems/](problems/) for four such mappings). Returning `hasCycle` explicitly, rather than an empty order, keeps "no valid order exists" and "the valid order happens to be empty because there are zero nodes" unambiguous to callers.

Internally, it builds `adj` (size `numNodes`, each entry a list of direct dependents) and `inDegree` (size `numNodes`, initialized to 0) in a single pass over `edges`: for every `{u, v}` pair, `v` is appended to `adj[u]` and `inDegree[v]` is incremented. A `std::queue<int>` is then seeded with every node whose `inDegree` is 0 — the initial frontier of "no prerequisites at all" nodes. The main loop pops a node, appends it to the `order` output vector, and walks its adjacency list, decrementing each neighbor's `inDegree` and enqueuing any neighbor that just reached 0 — mechanically implementing the steps described in Solution above. After the loop, `hasCycle` is computed as `order.size() != static_cast<size_t>(numNodes)`, and the function returns `{order, hasCycle}` as a small aggregate struct so callers get both pieces of information without needing an ambiguous sentinel value.

**`main()`** exercises `topologicalSort` against two hand-checked cases: a valid DAG (a small course-prerequisite graph, where the expected order is checked by confirming every edge constraint holds rather than an exact sequence, since Kahn's algorithm on a DAG with multiple in-degree-0 nodes at once does not guarantee one single "the" answer — only "a" valid one), and a graph containing a deliberate cycle, checking that `hasCycle` comes back `true` and that the returned partial order is strictly shorter than `numNodes`. Both cases print `[PASS]`/`[FAIL]` lines, proving the implementation compiles and behaves correctly end to end.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem, reimplementing Kahn's algorithm inline (rather than calling the generic template directly) so every file stays dependency-free and independently readable — mirroring the same choice made in this repo's other full pattern modules. See [problems/README.md](problems/README.md) for the index. Briefly: `01` is pure cycle detection (does *any* valid order exist?); `02` is the same graph but asking for the order itself; `03` is the hardest of the four, requiring you to first *derive* the edges from an unrelated-looking input (a sorted word list) before topological sort even begins; `04` is a variant that peels graph "leaves" layer by layer on an *undirected* tree rather than following directed prerequisite edges, showing how the same in-degree/queue machinery generalizes.

## Tradeoffs

**What Topological Sort buys you**

- **Linear time, not exponential.** `O(V + E)` (V = nodes, E = edges) instead of the `O(N!)` a brute-force permutation search would need — the difference between "instant" and "will never finish" once `N` grows past roughly 15-20 items.
- **Built-in cycle detection, for free.** The exact same bookkeeping that produces the order (in-degree tracking) also tells you, via the final order length, whether a valid order existed at all — no separate pass is required.
- **Directly models the problem's real structure.** Unlike a proxy heuristic (alphabetical, dependency count), the algorithm operates directly on the "must come before" edges themselves, so a node is placed exactly when its prerequisites are actually satisfied — nothing more, nothing less.
- **Produces a legal execution order, not just a yes/no answer.** Build systems, package managers, and course planners need the *actual order*, not merely confirmation that one exists — Kahn's algorithm gives you both from the same run.
- **Extends cleanly to tie-breaking variants.** Swapping the plain queue for a min-heap costs only a factor of `log V` per operation and directly gives the lexicographically smallest valid order.
- **A simpler cycle-detection signal than the DFS alternative.** Comparing final order length to node count is more directly stateable than tracking a "currently on the recursion stack" set during DFS, and it naturally produces the order in forward-build order, with no reversal step needed.

**What it costs you**

- **Only works on a DAG.** If the input graph has a cycle, there is **no valid topological order at all** — not a degraded one, not a "best effort" one. This is a structural precondition of the whole technique, not an edge case to patch around.
- **Must actively detect and report cycles, not silently return a partial order.** A naive implementation that forgets to check `order.size() == numNodes` returns a truncated list as if it were a complete, valid answer — a correctness bug, not a performance one, and often more dangerous than crashing outright.
- **Only one arbitrary valid order is produced by default, not a stable or "preferred" one.** If two nodes both have in-degree 0 at the same moment, which one appears first depends on incidental factors like node numbering and the standard queue's FIFO behavior — a specific (e.g. lexicographically smallest) order needs a min-heap instead of a plain queue.
- **Requires building auxiliary structures up front.** The in-degree array and adjacency list are `O(V + E)` to build — asymptotically free, but still real, non-trivial setup work compared to a pattern that needs no preprocessing at all.
- **Undirected graphs need a different mental model.** Kahn's algorithm as described assumes directed edges with a clear "prerequisite" meaning; applying the same in-degree machinery to an undirected graph (as in Minimum Height Trees) requires reframing "in-degree" as plain degree and reinterpreting what "reaching 0" means (see `problems/04`).
- **No partial credit for a graph with cycles.** Either the whole graph is a DAG and a full valid order exists, or it is not and no valid order exists at all for the *entire* graph — even though acyclic portions of a cyclic graph could, in principle, still be locally orderable, the standard formulation reports failure for the whole input rather than a fragmented "order what I can" result.
- **Loses the "free traversal" benefit the DFS alternative sometimes offers.** DFS-based topological sort is preferable when you are already doing a DFS for another reason in the same pass (e.g. simultaneously detecting strongly connected components), getting the ordering as a side effect of a traversal you needed anyway; Kahn's algorithm needs its own dedicated queue-driven pass.

## Complexity

**Time:** `O(V + E)` — building the adjacency list and in-degree array is a single `O(E)` pass over the edges; the main loop visits each node exactly once (`O(V)`) and, across the whole run, decrements each edge's target in-degree exactly once (`O(E)` total across all iterations, not per iteration). This holds in the best, worst, and average case alike, because the loop bound is structural (every node is enqueued and dequeued at most once), not data-dependent.

**Space:** `O(V + E)` — the adjacency list stores every edge once (`O(E)`), the in-degree array is `O(V)`, and the queue plus output list are each bounded by `O(V)`.

**Comparison to the brute force it replaces:**

| Approach | Time | Space | Detects cycles? |
|---|---|---|---|
| Brute-force: try every permutation, validate against edges | `O(N! * E)` | `O(N)` per candidate | Yes, but only after exhausting (or nearly exhausting) all `N!` candidates |
| DFS post-order + reverse | `O(V + E)` | `O(V + E)` | Yes, via a "currently on recursion stack" (gray) set |
| Kahn's algorithm (this module) | `O(V + E)` | `O(V + E)` | Yes, via final order length vs. `V` |

## Common Mistakes

- **Forgetting to check whether the output order's length equals `V`.** This is *the* tell-tale sign of a cycle: if the final order is shorter than the total node count, a cycle exists and the order is not valid. Skipping this check means silently returning a wrong, partial answer for cyclic input instead of reporting failure. *Avoid:* always compare `order.size()` to `numNodes` before returning, and make the return type/contract communicate "no valid order" unambiguously (a boolean flag, a clearly documented empty-with-sentinel convention, or an exception — but never a silently-truncated list treated as complete).
- **Mixing up which direction an edge's dependency points.** An edge meaning "`a` requires `b` as a prerequisite" is naturally drawn as `b -> a` (b before a), but it is easy to instead build the edge as `a -> b` by reflex, especially when the input format lists `[a, b]` pairs (as LeetCode's Course Schedule problems do) without making the direction visually obvious. *Avoid:* before writing any code, write down in a comment, in words, exactly what `adj[x]` is supposed to contain ("the nodes that depend on x" / "the nodes that must come after x") and re-derive the edge direction from that sentence every time.
- **Off-by-one in in-degree bookkeeping.** Incrementing the wrong node's in-degree, forgetting to initialize `inDegree` to 0 for nodes with no incoming edges (leaving garbage or uninitialized values), or decrementing a node's in-degree more than once per actual incoming edge (e.g. from double-counting duplicate edges in the input) all corrupt the "is this node safe to process yet?" signal. *Avoid:* initialize the in-degree array explicitly to all zeros sized to `numNodes` (not to the number of edges or some other unrelated count), and be careful that each edge contributes exactly one increment to exactly one node's in-degree.
- **Assuming a plain queue gives you a specific (e.g. lexicographically smallest) valid order.** A `std::queue` only guarantees FIFO order among nodes that reached in-degree 0 in that order — it does **not** guarantee any particular tie-breaking among nodes that become eligible at the same moment. *Avoid:* if the problem asks for a specific canonical order (not just "a" valid order), swap the queue for a `std::priority_queue` (min-heap) ordered by whatever tie-break rule is required.
- **Treating "no cycle detected in a small manual test" as proof of correctness.** Cycles can be subtle — a cycle spanning many nodes through several edges is easy to miss by eye, and a graph can be "mostly" a DAG with just one small cyclic pocket that still corrupts the whole result. *Avoid:* always test explicitly with at least one deliberately cyclic input, not only well-behaved DAGs, exactly as this module's `code.cpp` and `problems/01` do.

## When To Use

- **Build systems.** Ordering compilation/link steps so every target's dependencies are built before the target itself (Makefiles, Bazel, Gradle task graphs).
- **Package/dependency managers.** Installing or resolving packages so every package's dependencies are installed before it (npm, pip, Cargo).
- **Course/curriculum scheduling.** Producing a valid sequence of courses given prerequisite constraints, or determining whether a proposed curriculum is even completable.
- **Spreadsheet or build-graph recalculation order.** Recomputing derived values (spreadsheet formulas, CI pipeline stages, data pipeline DAGs like Airflow tasks) so every input is fresh before the value that depends on it is recomputed.
- **Any "must happen before" scheduling problem over discrete tasks** where you need one valid execution order, or need to detect that the task graph is contradictory (a deadlock-shaped dependency cycle).

## When NOT To Use

- **The dependency graph isn't a DAG by nature** — if cycles are expected and meaningful (e.g. a state machine with legitimate cyclic transitions, or a social graph), forcing a topological interpretation onto it is a category error; you likely want plain Graph BFS/DFS or a cycle-aware algorithm instead, not a "valid order."
- **You need more than "a" valid order — specifically, the *unique* lexicographically smallest one.** A plain FIFO queue does not guarantee this; you need a min-heap-backed variant, which changes the complexity from `O(V + E)` to `O(V log V + E)` and is a different (though closely related) algorithm shape.
- **You need shortest-path distances, not an ordering.** Even though topological order is useful for single-source shortest paths in a DAG (a real, legitimate application), if that is your actual goal you want a DAG shortest-path algorithm that *uses* topological order as a subroutine, not topological sort in isolation.
- **The graph is undirected and you just need connectivity or cycle detection as edges are added incrementally.** That is Union Find's job — it answers "connected?" and "would adding this edge create a cycle?" without ever needing a full traversal or ordering.
- **The relationships between items are not really "must happen before" at all**, but something else entirely (similarity, distance, weight) — forcing those into a topological-sort shape produces a meaningless "order."

## Where This Shows Up

Topological Sort appears constantly in both interview settings and production infrastructure:

- **Build system task ordering.** `make`, Bazel, Gradle, and similar tools model every build target and its dependencies as a DAG, and use a topological-sort-equivalent algorithm to decide the compilation/link order — the same shape as an **internal build/task DAG validator** for a monorepo, run in CI to reject a newly introduced dependency cycle between internal packages before it merges, rather than discovering the cycle only when a build mysteriously hangs.
- **Package manager dependency resolution.** `npm`, `pip`, and `cargo` all need to install a package's dependencies before the package itself; the dependency graph they resolve is exactly the DAG this pattern operates on (and a version-conflict "cannot resolve dependencies" error is often, at its core, a cycle or contradiction in that graph).
- **Course/curriculum scheduling.** University degree-planning tools and LeetCode's own "Course Schedule" family of problems (207, 210) model prerequisites directly as this pattern's canonical example.
- **Spreadsheet and dataflow recalculation order.** Spreadsheet engines (and reactive/dataflow systems more generally, like build graphs in Bazel or task graphs in Airflow) recompute derived cells/values in dependency order so a value is never read before it is freshly computed.
- **CI/CD pipeline stage ordering.** Pipeline definitions that declare "stage B depends on stage A" are DAGs whose valid execution order is a topological sort; a pipeline with a dependency cycle is a misconfiguration the CI system should detect and reject, not silently misrun.
- **Database migration ordering.** If migrations declare dependencies on other migrations (rather than relying purely on filename/timestamp ordering), topological sort gives a robust way to compute a valid apply order and to detect a broken, circular migration dependency before it reaches production.
- **Feature-flag or config dependency resolution.** If one feature flag's rollout requires another to be enabled first, modeling this as a DAG and topologically sorting it catches contradictory flag dependencies before a bad rollout config ships.
- **Microservice startup orchestration.** For a local dev environment (e.g. `docker-compose` service startup ordering) where service B must be healthy before service C starts, topological sort over the declared `depends_on` edges computes a safe startup sequence and can flag an accidental circular dependency between services.
- **Data pipeline task scheduling.** A lightweight internal ETL job scheduler that lets teams declare "this transform depends on that one" can compute a safe run order automatically, rejecting the configuration up front if it contains a dependency cycle.

## Similar Patterns

- **Graph BFS/DFS** ([../graph-bfs-dfs/](../graph-bfs-dfs/)): Topological Sort's Kahn's-algorithm variant is literally "BFS with an in-degree-based frontier instead of a `visited`-set-based frontier" — same queue-driven traversal shape, different admission rule for what enters the queue. Plain Graph BFS/DFS answers "what is reachable, and how far/deep is it?" on graphs that may have cycles; Topological Sort answers "what is a valid dependency order?" and requires the graph to be acyclic for an answer to exist at all. If you only need reachability or shortest hops (and the graph may legitimately have cycles), use Graph BFS/DFS directly instead.
- **Union Find** ([../union-find/](../union-find/)): answers a completely different question — "are these two nodes in the same connected component?" and "would adding this edge create a cycle?" — without ever performing a full traversal or producing any ordering. Union Find shines when edges arrive one at a time and you need fast, repeated connectivity/cycle queries; Topological Sort assumes the whole graph is known up front and produces a total ordering, which Union Find never does (it has no concept of "order," only "same component or not").

| Aspect | Topological Sort | Graph BFS/DFS | Union Find |
|---|---|---|---|
| Graph type required | Directed, must be acyclic (DAG) for success | Any (directed or undirected, cycles allowed) | Undirected, edges added incrementally |
| Core question answered | "What is a valid dependency order? Does one exist?" | "What is reachable? What is the shortest/some path?" | "Are these two nodes connected? Would this edge create a cycle?" |
| Output | A full ordering of all nodes (or cycle-detected failure) | A visited set, distances, or a specific path | A yes/no connectivity or cycle answer per query |
| Typical complexity | O(V + E) | O(V + E) | ~O(alpha(N)) per operation, near-constant amortized |
| Needs full graph up front? | Yes | Usually yes (though can run on streaming exploration) | No — edges can be added incrementally |

## Interview Discussion

Experienced engineers rarely dwell on the mechanics of writing the Kahn's-algorithm loop itself — that is mechanical. What they actually probe is whether you understand the **precondition and the failure mode**: do you know that a valid order requires a DAG, and do you handle the cyclic case *explicitly* rather than assuming well-behaved input?

Follow-up questions worth rehearsing:
- *"Can you get the lexicographically smallest valid order instead of just any valid one?"* — expects recognizing that swapping the plain queue for a min-heap (`priority_queue`) achieves this, at the cost of an added `log V` factor per operation.
- *"How would you do this with DFS instead of BFS?"* — expects the post-order-plus-reversal description from Solution above, and knowing that its cycle detection needs a recursion-stack ("gray set") check rather than a simple length comparison.
- *"Give me a real system that needs this."* — expects a genuine production example (build systems, package managers, CI pipelines), not just "Course Schedule on LeetCode."

Misconceptions worth killing early:
- **"Topological sort always produces a unique answer."** False — any graph with more than one node having in-degree 0 at some point admits multiple valid orders; "a" valid order, not "the" valid order, is the default contract.
- **"Topological sort only applies to explicitly-graph-shaped problems like course scheduling."** False — the pattern applies the moment you can identify discrete items and directed "must precede" relationships between them, even when the input looks nothing like a graph on the surface (see `problems/03`, where the graph has to be derived from a sorted word list first).
- **"In-degree and out-degree are interchangeable for this algorithm."** False — Kahn's algorithm specifically tracks in-degree (unmet prerequisites); confusing the two produces an algorithm that processes nodes in the wrong order or never terminates correctly.

## Key Takeaways

1. A valid topological order exists **if and only if** the graph is a DAG — cycles make the problem unsolvable for the whole graph, not just harder.
2. Kahn's algorithm: track in-degree per node, seed a queue with in-degree-0 nodes, repeatedly emit-and-decrement-neighbors, requeue any neighbor that reaches in-degree 0.
3. The cycle-detection signal is simple and easy to forget: compare the final output order's length to the total node count.
4. Edge direction matters — `u -> v` must mean "`u` is a prerequisite of `v`"; write down what `adj[x]` contains before coding to avoid reversing it by accident.
5. A plain queue gives *a* valid order; swap it for a min-heap if the problem demands the lexicographically smallest one.
6. DFS post-order + reverse is the standard alternative, trading a length check for a recursion-stack ("gray set") cycle check — know both for interviews.
7. Complexity is `O(V + E)` time and space — versus the `O(N!)` a brute-force permutation search would require.
8. Real systems that rely on this: build tools (Make, Bazel), package managers (npm, pip, Cargo), course/curriculum planners, spreadsheet engines, and CI/CD pipelines.
9. Union Find is a *different* tool for a *different* question (incremental connectivity/cycle checks, no ordering); do not reach for it when you actually need a full valid order.
10. Some problems (like Alien Dictionary) hide the graph entirely — recognizing "these are really directed 'must precede' constraints in disguise" is often the harder half of solving them.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — Section 22.4, the canonical DFS-based topological sort treatment and its correctness proof.
- *Algorithms, 4th Edition* — Robert Sedgewick & Kevin Wayne — the Directed Graphs chapter covers topological sort and precedence-constrained scheduling with a companion visualized implementation.
- *The Algorithm Design Manual* — Steven Skiena — covers topological sort within its graph algorithms chapter, with a practical, engineering-oriented framing.
- *Competitive Programmer's Handbook* — Antti Laaksonen — a concise, practical treatment of topological sorting alongside other graph algorithms.

**Open Source Projects / GitHub Repositories**
- CPython's `graphlib` module (`TopologicalSorter`) — https://github.com/python/cpython/blob/main/Lib/graphlib.py — a production-quality, standard-library implementation of Kahn's-style topological sorting with cycle detection built into the API contract.
- NetworkX — https://github.com/networkx/networkx — a widely used Python graph library whose `topological_sort` and related DAG algorithms are a good reference for a general-purpose, well-tested implementation.
- `TheAlgorithms/C++` — https://github.com/TheAlgorithms/C++ — a community-maintained collection of classic algorithms in C++, including Kahn's-algorithm and DFS-based topological sort implementations under its graph algorithms section.

**Official Documentation**
- Python docs — `graphlib.TopologicalSorter` — https://docs.python.org/3/library/graphlib.html — the standard library's own topological-sort utility, including its explicit `CycleError` for invalid (cyclic) input.
- NetworkX documentation — DAG algorithms (`topological_sort`) — https://networkx.org/documentation/stable/reference/algorithms/dag.html
- LeetCode — Course Schedule (problem 207), Course Schedule II (problem 210), Minimum Height Trees (problem 310) — the canonical worked problems referenced throughout this module.

**Blog Articles**
- GeeksforGeeks — "Topological Sorting" — https://www.geeksforgeeks.org/topological-sorting/ — a widely used explainer covering both the DFS and Kahn's-algorithm approaches with worked examples.
- NeetCode — Course Schedule / Topological Sort pattern videos — walks through the Course Schedule problem family and Kahn's algorithm with visual traces.
- Educative.io — "Grokking the Coding Interview" Topological Sort pattern chapter — one of the most widely referenced pattern-based framings of this exact technique.
