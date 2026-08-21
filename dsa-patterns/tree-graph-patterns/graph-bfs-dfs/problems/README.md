# Graph BFS/DFS — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Graph BFS/DFS across its distinct recognition signals — grid-as-graph flood fill, connected-component counting, undirected cycle detection via two-colouring, and shortest-hops BFS. Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers. Every file's header comment states, explicitly, **which traversal it uses and why the other one was rejected** — that decision is the whole lesson of this module.

```bash
g++ -std=c++17 -Wall problems/01-number-of-islands.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Number of Islands | [200](https://leetcode.com/problems/number-of-islands/) | Medium | DFS flood fill over the grid-as-implicit-graph; each unvisited land cell found by the outer scan starts a new island. | O(m·n) time, O(m·n) space | [01-number-of-islands.cpp](01-number-of-islands.cpp) |
| Number of Provinces | [547](https://leetcode.com/problems/number-of-provinces/) | Medium | DFS component counting over an adjacency **matrix**; loop every city as a potential unvisited start. | O(n²) time, O(n) space | [02-number-of-provinces.cpp](02-number-of-provinces.cpp) |
| Is Graph Bipartite? | [785](https://leetcode.com/problems/is-graph-bipartite/) | Medium | BFS two-colouring; a neighbour already painted the *same* colour proves an odd cycle, so no split exists. | O(V + E) time, O(V) space | [03-is-graph-bipartite.cpp](03-is-graph-bipartite.cpp) |
| Word Ladder | [127](https://leetcode.com/problems/word-ladder/) | Hard | BFS shortest hops where neighbours are generated on the fly (every one-letter mutation), counting words = hops + 1. | O(N·L²) time, O(N·L) space | [04-word-ladder.cpp](04-word-ladder.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md), and — deliberately — they disagree with each other about which traversal to use:

- **01 — grid as an implicit graph, flood fill.** The input never says "graph": it is a rectangle of characters. Seeing that each cell is a node with (at most) four edges is the recognition leap, and once made, "count the islands" is literally "count the connected components." **DFS, because either works** — the question has no distance in it, so BFS's ring-by-ring guarantee buys nothing and recursion is simply shorter code. The file names the one real reason to switch to BFS here (recursion depth on an all-land grid), which is a stack-safety concern, not a correctness one.
- **02 — connected-component counting on an explicit graph.** The same algorithm as 01 with the disguise removed: a real graph, handed over as an adjacency **matrix** rather than a list, which changes how you enumerate neighbours (scan a row) and pushes the bound to O(n²) rather than O(V + E). Pairing it with 01 is the point: recognising that two problems that look nothing alike are the same traversal. **DFS again, and the file argues explicitly that BFS is equally correct** — plus where Union Find would be the genuinely different third option.
- **03 — cycle detection, undirected flavour, via two-colouring.** "Bipartite" is a two-colouring question, and a graph is two-colourable exactly when it has no **odd** cycle — so this is the undirected counterpart to `hasCycleDirected` in [../code.cpp](../code.cpp), and the file contrasts the two directly: directed detection needs the three-state (`unvisited`/`in-progress`/`done`) marker, while here the `color` array *is* the visited array and the conflict (not the revisit) is the failure signal. **BFS, chosen over an equally-correct DFS purely to keep the traversal off the call stack.**
- **04 — shortest hops, where BFS is mandatory and DFS is wrong.** The hardest of the four on both counts: the graph is implicit *and* not obviously a graph at all (nodes are words, edges are one-letter mutations, and nothing is ever materialised), and the question says "shortest." **DFS here does not return a slower correct answer — it returns a valid transformation sequence that simply is not the minimum**, with no crash to warn you; making DFS correct requires un-marking on backtrack, which turns O(V + E) into an exponential enumeration of every simple path. This is the sharpest BFS-vs-DFS contrast in the module, and the file also carries its two classic bugs: counting words instead of hops (off by one), and erasing a candidate at dequeue rather than at discovery.

Read them in order — 01 and 02 establish that traversal choice is often a *style* decision, so that 04's "BFS or wrong answer" lands as the exception it actually is.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
