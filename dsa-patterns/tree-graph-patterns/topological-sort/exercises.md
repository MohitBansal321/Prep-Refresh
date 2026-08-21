# Topological Sort — Exercises

Work through these in order. The goal is to build three reflexes: (1) spotting "directed must-happen-before constraints" even when the input does not look like a graph, (2) reaching for the right variant — plain in-degree counting, Kahn's with a FIFO queue, Kahn's with a min-heap, layer-at-a-time Kahn's, or DFS post-order + reverse — and being able to say *why*, and (3) never shipping a topological sort that does not explicitly answer "what if there is a cycle?"

> Rule of thumb for every exercise: before writing a single line, write down in words what `adj[x]` will contain ("the things that must come after `x`") and what your cycle-detection signal is. If you cannot state both, you are not ready to write the loop yet.

---

## Easy — Minimum Number of Vertices to Reach All Nodes

**LeetCode 1557 — Minimum Number of Vertices to Reach All Nodes.** (LeetCode rates it Medium; it is the easiest problem in this set by a wide margin.)

Given a DAG with `n` nodes and a list of directed edges, find the smallest set of nodes from which every node in the graph is reachable.

**Task:** solve it *without* a queue and *without* a traversal. Build only the `inDegree` array from [code.cpp](code.cpp)'s `topologicalSort` — the first loop of that function, nothing after it — and return every node whose in-degree is 0.

**Think about:** why is "in-degree 0" both necessary and sufficient here? Necessary is the easy half (no edge points into such a node, so nothing else can reach it). For sufficient, use the fact that the input is guaranteed acyclic: walk backwards from any node along incoming edges and argue why that walk must terminate, and where it must terminate. **Then answer:** what breaks in your argument if the graph is allowed to contain a cycle, and what would the correct answer even mean then?

---

## Medium — Minimum Height Trees

**LeetCode 310 — Minimum Height Trees.**

Given an **undirected** tree with `n` nodes, find every node that, when used as the root, minimises the tree's height. (There are always either one or two such nodes.)

**Task:** adapt the queue machinery from [problems/02-course-schedule-ii.cpp](problems/02-course-schedule-ii.cpp), but with two changes. First, since edges are undirected, replace "in-degree" with plain **degree** (each edge increments *both* endpoints). Second, instead of emitting nodes one at a time, peel an entire **layer** of degree-1 nodes (leaves) per round, decrement their neighbours' degrees, and repeat until 2 or fewer nodes remain — those are the answer. Capture `ready.size()` at the top of each round and pop exactly that many nodes, so a round never consumes nodes that the same round just enqueued.

**Think about:** the README's Disadvantages section warns that "undirected graphs need a different mental model." Be precise about what changes: what does "degree reaches 1" mean here, and why 1 rather than the 0 that Kahn's algorithm waits for? **Then answer:** why can this process never leave 3 or more nodes, and why is a cycle check unnecessary for this problem in a way it never is for `problems/01`–`04`?

---

## Hard — Sort Items by Groups Respecting Dependencies

**LeetCode 1203 — Sort Items by Groups Respecting Dependencies.**

You have `n` items, each optionally belonging to a group, plus a list of "item `i` must come after these items" constraints. Return an ordering of all items such that every item constraint is respected **and** items of the same group are contiguous in the output. Return an empty list if no such ordering exists.

**Task:** this is two topological sorts stacked. Assign every ungrouped item its own private group first (so "same group must be contiguous" becomes vacuously true for it). Then run the Kahn's loop from [problems/02-course-schedule-ii.cpp](problems/02-course-schedule-ii.cpp) twice: once over **groups** (a group edge exists whenever an item in group A must precede an item in group B) to order the groups, and once **inside each group** to order that group's own items. Concatenate the per-group orders in group order. Report failure if *either* sort's final length check fails.

**Then answer:** the "same group must be contiguous" requirement is what forces two levels instead of one. Explain exactly why a single flat topological sort over all `n` items — even one that happens to produce a contiguous grouping on your test input — cannot be trusted to produce one in general. Then explain why a cycle *among groups* is possible even when there is no cycle among items, and give a concrete three-item example.

---

## Real-World Challenge — Migration Ordering Service

You maintain a Postgres-backed NestJS service whose schema migrations have outgrown filename-timestamp ordering: a migration may now declare `dependsOn: ["add-users-table", "add-orders-index"]`, and CI must compute a safe apply order before anything touches the database.

**Task:**

1. Model it. Each migration is a node; each `dependsOn` entry is a directed edge from the dependency to the dependent. Migration names are strings, so add a `std::unordered_map<std::string, int>` assigning each name a dense integer id on first sight, exactly as [problems/04-alien-dictionary.cpp](problems/04-alien-dictionary.cpp) maps letters onto `0..25` — Kahn's algorithm wants array indices, not strings.
2. Compute the apply order with Kahn's algorithm, and on failure produce a **useful** error, not just "cycle detected." The nodes never emitted are exactly the ones still holding a non-zero in-degree when the queue runs dry; list their names in the CI failure message.
3. Make the output **deterministic**: two engineers running CI on the same branch must get the identical order, and a re-run must not reshuffle unrelated migrations. Swap the FIFO queue for a `std::priority_queue` keyed on migration name so ties break lexicographically, and state the new complexity.
4. Add a validation pass that rejects a `dependsOn` entry naming a migration that does not exist — currently that name would silently become a brand-new node with no incoming edges and get "applied" first.
5. **Discuss:** a migration is also a *destructive* operation, so unlike a build you cannot simply re-run the whole DAG after a partial failure. If migration 7 of 12 fails at runtime, what does the topological order buy you that a flat list does not — and what does it *not* buy you? Specifically: does knowing a valid total order tell you which of the remaining migrations are still safe to apply? Sketch what extra information (hint: reachability from the failed node, not ordering) you would need to answer that, and which pattern computes it.

---

## Bonus Challenge — Parallel Courses

**LeetCode 1136 — Parallel Courses.**

Given `n` courses and prerequisite relations, you may take **any number** of courses in a single semester provided all their prerequisites were completed in earlier semesters. Return the minimum number of semesters needed to take every course, or `-1` if impossible.

**Task:** use the *layered* Kahn's variant from the Minimum Height Trees exercise above, applied to a directed graph: at the top of each round, record `ready.size()` and process exactly that many nodes — one round is one semester. Count the rounds. Keep the length check from [problems/02-course-schedule-ii.cpp](problems/02-course-schedule-ii.cpp) to return `-1` on a cycle.

**Then, generalise in writing (no code required):** you have now used the same in-degree queue to answer four different questions — "is it possible" (`problems/01`), "give me an order" (`problems/02`), "is the order unique" (`problems/03`), and "how many parallel rounds" (here). For each, name the *single observable* of the algorithm that produced the answer (the final count, the emitted sequence, the queue's size per iteration, the number of iterations of the outer loop). Then argue, using the README's Architecture section, why the number of rounds equals the **longest path** in the DAG — and why that makes this problem's answer independent of every tie-breaking choice, unlike `problems/02`'s.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
