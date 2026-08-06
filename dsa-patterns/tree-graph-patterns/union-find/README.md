# Union Find (Disjoint Set)

## Intent

Track which elements belong to the same connected group, supporting near-`O(1)` amortized "are these connected?" and "merge these two groups" operations, without ever needing to traverse the underlying graph's edges.

## Real Life Analogy

Think about **merging friend groups at a party**. Every guest starts out as their own "group of one." As the party goes on, you learn things like "Alice and Bob know each other" — so you merge Alice's group and Bob's group into one. Later you learn "Carol and Dave know each other" — merge their groups too. Later still, "Bob and Carol know each other" — now Alice, Bob, Carol, and Dave's groups all merge into a single group.

At any point, if someone asks "are Alice and Dave in the same friend group?", you don't need to retrace every "X knows Y" fact that led to that merge — you just need to know which group's *representative* (say, whoever's name you decided to label the merged group with) Alice and Dave each currently point to. If they point to the same representative, they're connected; if not, they aren't. That's the entire idea: **track group membership directly, not the path of connections that produced it.**

## Problem

### What engineering problem exists?

Edges (connections) arrive one at a time, and you repeatedly need to answer:

- **"Are these two elements already connected?"** — directly, or transitively through a chain of other connections.
- **"Would adding this edge create a cycle?"** — true exactly when the edge's two endpoints are already connected before the edge is added.
- **"How many separate connected groups exist right now?"**

> **Term: Connected component.** A maximal set of elements where every pair is connected (directly or transitively) through some chain of edges, and no element outside the set is connected to any element inside it.

The naive way to answer "are X and Y connected?" is to run a graph traversal (BFS or DFS) from X every time the question is asked, checking whether Y is reachable — `O(V + E)` per query.

### Why is this problem difficult?

- **Re-running a full traversal for every single query is wasteful when queries and edge-additions are interleaved many times.** If you're processing a stream of thousands of "add this edge" / "are these connected?" operations, paying `O(V + E)` for every query adds up to a very expensive total, even though each individual traversal is "fast" in isolation.
- **Connectivity needs to be tracked incrementally, not recomputed from scratch.** Every new edge can merge two previously-separate components into one — the data structure needs to reflect that merge cheaply, without re-deriving the whole component structure from the full edge list every time.
- **A naive "track a group ID per element, update all of them on every merge" approach is itself expensive** — relabeling every element in a large group every time it merges with another group costs `O(n)` per merge in the worst case.

### What happens if we ignore it?

- **Quadratic-or-worse total cost** when many union/find operations are interleaved — re-traversing the graph (or relabeling entire groups) on every operation, across thousands of operations, adds up far beyond what's necessary.
- **Missed or duplicated cycle detection** in algorithms (like Kruskal's minimum spanning tree) that fundamentally depend on a fast, incremental "would this edge create a cycle?" check.
- **Redundant re-implementation of the same connectivity logic** across problems that all reduce to the same "track groups, merge on demand" shape.

## Why Not Other Approaches?

**"Re-run BFS/DFS from scratch on every 'are these connected?' query."** Correct, but each query costs `O(V + E)` — for `q` queries interleaved with edge additions, that's `O(q * (V + E))` total, which is far worse than necessary when connectivity only *changes* on the (typically much rarer) edge-addition operations, not on every query.

**"Maintain a group ID per element, updating all members' IDs on every merge."** Also correct, but relabeling every element of a group on each merge costs `O(n)` in the worst case (imagine repeatedly merging a huge group into progressively larger ones) — across many merges this degrades toward `O(n^2)` total, which the tree-based approach below avoids entirely.

**Tradeoff summary:** repeated full traversals waste work on every query; full relabeling wastes work on every merge. Union Find's tree-of-parent-pointers representation (below) makes both `find` and `union` cheap by design — neither operation needs to touch every element in a group, only walk up one parent-chain.

## Solution

Represent each group as an implicit tree: every element has a `parent` pointer (initially pointing to itself). `find(x)` walks up `x`'s parent chain until it reaches a node whose parent is itself — the **representative** (root) of `x`'s group. `union(x, y)` finds each element's root and, if they differ, links one root's parent to point at the other, merging the two trees into one.

Two optimizations make both operations run in near-constant amortized time:

- **Path compression.** While `find(x)` walks up to the root, re-point every node visited along the way directly to the root. Future `find` calls on those nodes become instant — the tree flattens toward a bush with depth close to 1.
- **Union by rank/size.** When merging two groups, always attach the *smaller* (by tree height or element count) group's root under the *larger* group's root, rather than picking arbitrarily. This keeps the resulting tree's height growing logarithmically instead of linearly in the worst case.

Used together, these two optimizations give an amortized time per operation of `O(alpha(n))`, where `alpha` is the inverse Ackermann function — a function that grows so slowly it is less than 5 for any `n` that could ever be represented in memory, making the pattern effectively constant time in practice.

## Architecture

The "participants" are:

1. **The `parent` array**, indexed by element, holding each element's current parent. A root element's parent is itself.
2. **The `rank` (or `size`) array**, tracking an upper bound on each tree's height (or its element count), used purely to decide which root to attach under which during a union — never used for anything else.
3. **`find(x)`**, which walks `x`'s parent chain to the root, applying path compression along the way.
4. **`union(x, y)`**, which calls `find` on both elements and, if their roots differ, attaches the smaller-ranked root under the larger-ranked one (updating rank only when the two ranks were equal, since attaching a smaller tree under a larger one doesn't increase the larger tree's height).

## Execution Flow

1. **Initialize:** every element starts as its own group — `parent[i] = i` for all `i`, `rank[i] = 0`.
2. **`find(x)` with path compression:** if `parent[x] == x`, return `x` (it's the root). Otherwise, recursively (or iteratively) find the root of `parent[x]`, then set `parent[x]` to that root before returning it — flattening the path for every future call.
3. **`union(x, y)` with union by rank:** compute `rootX = find(x)` and `rootY = find(y)`. If they're equal, `x` and `y` are already in the same group — nothing to do. Otherwise, attach the root with the smaller rank under the root with the larger rank; if ranks are equal, attach either one under the other and increment the new root's rank by one.
4. **`connected(x, y)`:** simply `find(x) == find(y)`.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the flowchart distinguishing Union Find from Graph BFS/DFS and Topological Sort, based on whether edges arrive incrementally and whether the question is repeatedly about connectivity rather than traversal order.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of `find` with path compression and `union` with union by rank.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of the parent-pointer forest as several unions are performed on a concrete small example.

## Implementation

[code.cpp](code.cpp) provides a generic, reusable `DisjointSet` class with `find`, `unionSets`, and `connected` methods, implementing both path compression and union by rank/size together.

## Code Walkthrough

**`DisjointSet` constructor** (in [code.cpp](code.cpp)). Initializes `parent[i] = i` for every element (each starts as its own root) and `rank[i] = 0`.

**`find(x)`** (in [code.cpp](code.cpp)). Implements path compression: if `x` is already a root, returns it immediately; otherwise recurses to find the true root, then rewrites `parent[x]` to point directly at that root before returning — so any future call on `x` (or anything that later points through `x`) is O(1).

**`unionSets(x, y)`** (in [code.cpp](code.cpp)). Finds both roots; if they're already equal, does nothing (already connected). Otherwise attaches the lower-rank root under the higher-rank root, incrementing the surviving root's rank only when the two input ranks were equal (the one case where the resulting tree's height could actually increase).

**`connected(x, y)`** (in [code.cpp](code.cpp)). A thin wrapper comparing `find(x)` to `find(y)`.

**Files in [problems/](problems/).** Each is a complete, standalone solution to one named LeetCode problem applying `DisjointSet` (or a close variant) to a specific scenario — counting groups, detecting a redundant edge, merging accounts by shared identifiers, and answering incremental connectivity queries as elements are added one at a time. See [problems/README.md](problems/README.md) for the full index.

## Advantages

- **Near-constant amortized time per operation** (`O(alpha(n))`) once both path compression and union by rank are applied together.
- **No graph traversal needed at query time** — `connected(x, y)` is just two `find` calls and a comparison, regardless of how large or sparse the underlying graph is.
- **Naturally incremental.** Handles edges arriving one at a time far more cheaply than re-running a full traversal after each addition.
- **Simple to implement correctly** — the entire structure is two small arrays and two short functions, yet the amortized complexity guarantee is a genuinely deep result.

## Disadvantages

- **Doesn't reveal the actual path or edges between two connected elements** — it only answers "are they connected," not "how" or "via what route."
- **Doesn't support efficient edge removal / "un-union."** Once two groups are merged, splitting them back apart efficiently requires a fundamentally different structure (or storing enough history to roll back, which defeats much of the efficiency gain).
- **Rank/size bookkeeping is easy to get subtly wrong** if path compression and union by rank aren't both applied — using only one of the two optimizations still works correctly, but loses the near-constant-time guarantee and can degrade toward `O(n)` per operation in adversarial input orders.

## Tradeoffs

**What we gain:** near-`O(1)` amortized connectivity queries and merges, without ever needing to traverse the graph's actual edges at query time.

**What we lose:** the ability to answer "what is the path/edges connecting these two elements" (only connectivity itself is tracked) and the ability to efficiently undo a merge once it's happened.

## Complexity

**Time:** `O(alpha(n))` amortized per `find` or `union` operation, with both path compression and union by rank applied — effectively constant for any practically-sized `n` (the inverse Ackermann function is less than 5 for any `n` up to numbers vastly larger than the number of atoms in the observable universe). Without both optimizations, individual operations can degrade to `O(n)` in the worst case (a long, unbalanced parent chain).

**Space:** `O(n)` for the `parent` and `rank` arrays.

**Comparison to the brute force it replaces:**

| Operation | BFS/DFS per query | Union Find |
|---|---|---|
| "Are X and Y connected?" | `O(V + E)` per query | `O(alpha(n))` amortized |
| "Does adding this edge create a cycle?" | `O(V + E)` per check | `O(alpha(n))` amortized |
| Total cost over `q` interleaved queries/unions | `O(q * (V + E))` | `O((n + q) * alpha(n))` |

## Common Mistakes

- **Forgetting path compression, union by rank, or both.** The structure is still *correct* without either optimization — `find` and `union` still return the right answer — but the amortized time guarantee depends on having both; skipping them can silently degrade performance to `O(n)` per operation on adversarially-ordered input, without ever producing a wrong answer (making the bug easy to miss until it shows up as a timeout).
- **Comparing `x == y` instead of `find(x) == find(y)` to check connectivity.** Two elements can be in the same group without being equal to each other — connectivity is a property of the roots, not the elements themselves.
- **Off-by-one or missed initialization** — forgetting to initialize `parent[i] = i` for every element (rather than leaving it at some default like 0) means every element silently starts out "connected" to element 0.
- **Updating rank on every union, rather than only when the two ranks are equal.** Attaching a strictly-smaller-rank tree under a strictly-larger-rank tree never increases the larger tree's height, so incrementing rank in that case is not just unnecessary but actively breaks the invariant that makes union-by-rank's height bound correct.

## When To Use

- **Edges/connections arrive incrementally**, and you repeatedly need to check connectivity or detect cycles as they're added.
- **Counting connected components** in an undirected graph, especially when the graph is built up dynamically rather than given all at once.
- **Kruskal's minimum spanning tree algorithm**, which relies on Union Find's fast cycle detection to decide whether to include each candidate edge.
- **Merging records by a shared, possibly-transitive identifier** (e.g. merging user accounts that share any of several email addresses).

## When NOT To Use

- **You need the actual path or specific edges connecting two elements, not just whether they're connected** — Union Find deliberately discards that information; use Graph BFS/DFS if the path itself matters.
- **You need to remove edges/connections and efficiently query the resulting (weaker) connectivity** — Union Find has no efficient "undo."
- **The graph is static (given once, not built incrementally) and you only need to answer connectivity once** — a single BFS/DFS pass computing all connected components up front is simpler and just as fast for a one-time query.

## Real Interview/Production Examples

- **Kruskal's minimum spanning tree algorithm** uses Union Find directly to decide, for each candidate edge in increasing weight order, whether adding it would create a cycle (skip it) or connect two previously-separate components (include it).
- **Image segmentation** algorithms use Union Find to merge adjacent pixels into connected regions based on similarity, building up segments incrementally.
- **Network connectivity monitoring** — tracking which nodes in a network remain mutually reachable as links come online, using Union Find to answer connectivity queries without re-running a full traversal after every link event.
- **Detecting redundant connections in infrastructure graphs** — identifying which added edge (in a build order) is the one that would introduce a cycle, directly analogous to "Redundant Connection"-style problems.

## Where I Can Use This

Five realistic ideas for your own backend projects:

1. **A user-account deduplication service** — merge accounts that share any identifier (email, phone number) transitively, using Union Find to group all accounts reachable through a chain of shared identifiers.
2. **A dynamic service-mesh connectivity checker** — track which services remain mutually reachable as network links go up and down, answering "can service A currently reach service B" queries in near-constant time.
3. **An incremental build-dependency validator** — as dependencies between build targets are declared one at a time, use Union Find (or a hybrid with Topological Sort) to detect when a newly-declared dependency would introduce a cycle.
4. **A friend-group / community detection feature** — incrementally merge users into groups as "these two users are connected" events arrive, without re-running a full graph traversal on every event.
5. **A distributed cache invalidation grouping tool** — group cache keys that must be invalidated together because they've been observed to be transitively linked (e.g. via shared computed dependencies), merging groups as new links are discovered.

## Similar Patterns

- **Graph BFS/DFS** ([../graph-bfs-dfs/](../graph-bfs-dfs/)): also answers connectivity questions, but by actually traversing the graph's edges — necessary when you need the path itself, or when the graph is static and queried only once. Union Find wins when connectivity must be tracked incrementally across many interleaved queries and edge additions.
- **Topological Sort** ([../topological-sort/](../topological-sort/)): answers a different question entirely (a valid processing *order* respecting directed dependencies) rather than undirected connectivity — the two are sometimes combined (e.g. detecting whether a new dependency edge would create a cycle in an otherwise-DAG build system).

| Pattern | What it answers | Core mechanism |
|---|---|---|
| Union Find | "Are these connected?" / "Would this edge create a cycle?" incrementally | Parent-pointer forest, path compression, union by rank |
| Graph BFS/DFS | "What's the path?" / one-time connectivity or reachability | Explicit traversal with a visited set |
| Topological Sort | "What's a valid dependency order?" | In-degree tracking + queue (Kahn's) or DFS post-order |

## Interview Discussion

Experienced engineers rarely spend time on the basic `parent[x] = x` bootstrapping — they probe whether you understand *why* path compression and union by rank together produce the near-constant amortized bound, and whether you can recognize when a problem's "keep asking about connectivity as edges arrive" shape calls for this pattern instead of repeated traversal.

Common follow-up questions:
- *"What is the actual time complexity, and why is it not simply O(1)?"* — expects naming the inverse Ackermann function and explaining, at a high level, why it grows so slowly that the bound is "effectively constant" in practice without literally being O(1) in the strict theoretical sense.
- *"What happens if you only implement path compression but not union by rank (or vice versa)?"* — expects recognizing the structure remains correct but can degrade toward O(n) per operation in adversarial orderings without either optimization, and toward O(log n) with only one of the two.
- *"How does Union Find relate to Kruskal's algorithm?"* — expects describing Kruskal's as: sort edges by weight, then use Union Find's `connected` check to skip edges that would form a cycle and `union` to include edges that connect new components.
- *"Can Union Find tell you the path between two connected elements?"* — expects a clear "no," and recognizing that Graph BFS/DFS is the right tool if the path itself is needed.

Common misconceptions:
- "Union Find can be undone efficiently." It generally cannot — removing an edge and re-deriving weaker connectivity is not supported by the basic structure.
- "Union Find is only useful for graph cycle detection." It's the right tool for any dynamic connectivity or incremental-grouping problem, well beyond literal graphs — account deduplication and image segmentation are not "graph problems" in the traditional sense but fit the same shape exactly.
- "The rank array tracks the exact tree height at all times." It's only an upper bound used for the union decision, not necessarily updated to reflect the true height after every path compression — this is fine and doesn't affect correctness, but is worth knowing precisely so you don't over-interpret what `rank` means.

## Summary

- Union Find tracks group membership directly (via a parent-pointer forest), answering "are these connected?" without ever traversing the graph's actual edges.
- `find(x)` walks up to the root, applying path compression to flatten future lookups; `union(x, y)` merges two groups' roots, using union by rank to keep the resulting tree shallow.
- Together, path compression and union by rank give amortized `O(alpha(n))` time per operation — effectively constant for any practical input size.
- The structure cannot reveal the actual path between two elements, and does not efficiently support removing a connection once merged.
- Kruskal's minimum spanning tree algorithm is the canonical application: sort edges by weight, use Union Find to skip cycle-forming edges and include connecting ones.

## Key Takeaways

1. `find(x)` walks to the root of `x`'s group; `union(x, y)` merges the two groups' roots if they differ.
2. Path compression flattens the tree during `find`, making future lookups on the same elements effectively instant.
3. Union by rank/size always attaches the smaller tree under the larger one, keeping tree height logarithmic in the worst case.
4. Together, both optimizations give amortized `O(alpha(n))` time per operation — effectively constant in practice.
5. Check connectivity via `find(x) == find(y)`, never via `x == y` directly.
6. The structure only answers "connected or not" — it cannot reveal the actual path or edges.
7. There is no efficient "undo" — removing an edge and recomputing weaker connectivity needs a different approach entirely.
8. Kruskal's minimum spanning tree algorithm is the textbook production use: skip edges that would form a cycle, using Union Find's connectivity check.
9. Union Find beats repeated BFS/DFS specifically when queries and edge-additions are interleaved many times — a one-time static connectivity query is just as well served by a single traversal.
10. Only increment rank during a union when the two input ranks were equal — attaching a smaller-rank tree under a strictly larger one never increases the larger tree's height.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — Chapter 21, "Data Structures for Disjoint Sets," the formal treatment of path compression, union by rank, and the inverse-Ackermann amortized bound.
- *The Algorithm Design Manual* — Steven Skiena — covers Union Find alongside Kruskal's algorithm and general graph connectivity techniques.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — community-maintained classic algorithm implementations, including disjoint-set-union implementations.

**Official Documentation**
- LeetCode — Number of Provinces (problem 547).
- LeetCode — Redundant Connection (problem 684).
- LeetCode — Accounts Merge (problem 721).
- LeetCode — Number of Islands II (problem 305).

**Blog Articles**
- GeeksforGeeks — "Union Find Algorithm" explainer, covering path compression and union by rank with worked examples.
- CP-Algorithms (cp-algorithms.com) — "Disjoint Set Union" — a widely referenced competitive-programming reference for the structure and its optimizations.
- Educative.io — "Grokking the Coding Interview," the Union Find pattern chapter.
