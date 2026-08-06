# Graph BFS/DFS — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing when a problem is secretly a graph (an implicit adjacency relationship hiding inside a grid, a word list, or a dependency list), and (2) correctly choosing BFS when the question is about shortest hops, versus DFS when it is about reachability, structure, or connectivity.

> Rule of thumb for every exercise: before writing a single line, ask "does this problem contain the word 'minimum,' 'shortest,' 'fewest,' or 'nearest'?" If yes, you almost certainly want BFS. If instead it asks "is this connected," "can I reach X," or "how many separate groups are there," DFS (or BFS — either works for pure reachability) is the natural fit. Then ask "is the `visited` guard actually wired in, and does it cover every disconnected piece?" If you cannot answer both, you are not ready to write the traversal yet.

---

## Easy — Flood Fill

**LeetCode 733 — Flood Fill.**

Given an `m x n` integer grid `image`, a starting pixel `(sr, sc)`, and a new color, perform a flood fill: change the starting pixel's color and every pixel connected to it (4-directionally: up/down/left/right) that shares the *starting pixel's original color* to the new color.

**Constraints to notice:** the grid is an implicit graph — every cell is a node, and its up/down/left/right neighbors (that share the same original color) are its edges. This is the same "grid as graph" idea behind [problems/01-number-of-islands.cpp](problems/01-number-of-islands.cpp), except here you are filling one connected region rather than counting how many exist.

**Task:** solve it with DFS (or BFS — either works, since you only need to visit every connected same-colored cell exactly once, not the shortest distance to any of them) in O(m·n) time.

**Think about:** what happens if the new color is the *same* as the starting pixel's original color? Walk through why a naive DFS without an early check could infinite-loop in that specific case, even though the same-colored cells form no true cycle in the grid's connectivity.

---

## Medium — Course Schedule

**LeetCode 207 — Course Schedule.**

There are `numCourses` courses labeled `0` to `numCourses - 1`. You are given `prerequisites[i] = [a, b]`, meaning you must take course `b` before course `a`. Return `true` if you can finish all courses (i.e., there is no circular dependency).

**Task:** model the prerequisites as a **directed graph** (an edge `b -> a` for each `[a, b]` pair) and detect whether it contains a cycle, using the same three-state (`unvisited` / `in-progress` / `done`) DFS approach as `hasCycleDirected` in [code.cpp](code.cpp). If a cycle exists, it is impossible to complete all courses.

**Think about:** why is a plain two-state `visited` boolean insufficient here — construct a small example (3-4 courses) where a course is legitimately reachable via two different prerequisite chains without any circular dependency existing, and confirm your three-state approach does not falsely report a cycle.

---

## Hard — Shortest Path in Binary Matrix

**LeetCode 1091 — Shortest Path in Binary Matrix.**

Given an `n x n` binary matrix `grid`, return the length of the shortest clear path from top-left to bottom-right, moving in any of the 8 directions (including diagonals), where you may only travel through cells with value `0`. Return `-1` if no such path exists.

**Task:** solve it with BFS over the grid-as-implicit-graph, where each cell's "neighbors" are up to 8 surrounding cells (not just 4) that are within bounds and have value `0`. Track distance the same way `bfsShortestPath` does in [code.cpp](code.cpp): mark a cell visited (and record its distance) the moment it is discovered/enqueued, not when it is popped.

**Then answer:** the path length LeetCode expects counts the number of *cells* visited (including both start and end), not the number of edges/hops. Given that `bfsShortestPath` in this module returns hop-count (edges), what is the precise relationship between "number of cells in the path" and "number of hops," and where exactly in your BFS loop do you need to adjust for it?

---

## Real-World Challenge — Service Dependency Impact Analysis

You maintain a microservices platform where each service declares which other services it calls (a directed graph: an edge `A -> B` means "service A calls service B"). Two operational questions come up constantly during incidents and change-management reviews:

1. **"If service X goes down right now, which services become unreachable from the public API gateway?"** — because they, directly or transitively, depend on a path through X.
2. **"Does deploying this new dependency (`A` now also calls `C`) introduce a circular dependency anywhere in the graph?"** — a circular wait between services can deadlock a coordinated startup/shutdown sequence.

**Task:**
1. Build the service graph as `std::vector<std::vector<int>> adj` from a list of `(caller, callee)` pairs.
2. For question 1: given the gateway's node and the down service `X`, compute the full set of services reachable from the gateway **before** removing `X`, then again **after** temporarily removing all edges into and out of `X`, and report the set difference — the services that lost reachability.
3. For question 2: before accepting the new edge `A -> C`, temporarily add it to the graph and run `hasCycleDirected` (as implemented in [code.cpp](code.cpp)) to check whether it would introduce a cycle; reject the change if so.
4. **Discuss:** your reachability computation in step 2 re-runs a full BFS/DFS from scratch every time a service goes down. In a platform with thousands of services and frequent incidents, is recomputing reachability from scratch each time acceptable, or would you reach for an incrementally-maintained structure instead (hint: revisit the Similar Patterns section of the [README](README.md) and consider what Union Find can and cannot answer here, given that Union Find has no notion of "directed" edges or edge removal).

---

## Bonus Challenge — Multi-Source BFS on a Weighted-Looking Grid (Walls and Gates)

**LeetCode 286 — Walls and Gates.**

You are given an `m x n` grid where each cell is one of: `-1` (a wall), `0` (a gate), or `INF` (an empty room, represented by `2147483647`). Fill each empty room with the distance to its **nearest** gate. If a room cannot reach any gate, it should remain `INF`.

**Task:** implement this as **multi-source BFS**, starting the queue with *every* gate cell simultaneously (distance 0) rather than running a separate single-source BFS from each gate one at a time — the same core idea as [problems/03-rotting-oranges.cpp](problems/03-rotting-oranges.cpp), generalized from "rot spreads to all neighbors" to "distance-to-nearest-gate spreads to all neighbors."

**Then, generalize in writing (no code required):** explain precisely why starting BFS from *all* gates at once and letting them expand together gives the correct "nearest gate" distance for every room, whereas running `n` separate single-source BFS passes (one per gate) and taking the minimum at each cell would produce the *same* answer but cost strictly more work. Quantify the difference using the O(V + E) bound from the [README](README.md)'s Complexity section: what is each approach's total time complexity in terms of `V`, `E`, and the number of gates `k`?

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
