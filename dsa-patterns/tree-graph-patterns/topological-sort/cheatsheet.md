# Topological Sort — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — dependency ordering over a directed graph, via BFS with an in-degree frontier (Kahn's algorithm) or DFS post-order plus a reversal. |
| **Recognition Signal** | Discrete items with **directed "must happen before" constraints** — prerequisites, build targets, package dependencies, pipeline stages · or a question of the form "is this schedule even possible?" · or an input that *hides* such constraints (a sorted word list, a list of subsequences). |
| **Problem** | You cannot place an item until every prerequisite is placed, but you do not know which items are unblocked until you have placed others — and some constraint sets are self-contradictory, so "no valid order exists" is half the problem, not an edge case. |
| **Solution** | Track **in-degree** (count of unmet prerequisites) per node. Seed a queue with every in-degree-0 node. Pop → append to the order → decrement each dependent's in-degree → enqueue any that reach 0. Cycle iff the final order is shorter than the node count. Alternative: DFS post-order, push each node when its call finishes, reverse at the end; cycle iff DFS reaches a node still on the recursion stack (**gray**). |
| **Time / Space Complexity** | `O(V + E)` time — each node enqueued/dequeued at most once, each edge walked exactly once. `O(V + E)` space — adjacency list `O(E)`, in-degree array + queue + output `O(V)`. Both are structural, not data-dependent: best = worst = average. |
| **Pros** | Linear instead of the `O(N!)` a permutation search needs · cycle detection is free, falling out of the same bookkeeping that produces the order · returns the actual execution order, not just a yes/no · models the constraints directly, so correctness is easy to argue · swapping the queue for a min-heap gives the lexicographically smallest order for one extra `log V` factor. |
| **Cons** | Requires a DAG — a cycle means no valid order for the *whole* graph, with no partial-credit version · the length check is easy to omit, and omitting it silently returns a truncated order that looks plausible · a plain FIFO queue gives *a* valid order with no tie-break guarantee · needs `O(V + E)` setup (adjacency list + in-degree) before any work begins · undirected inputs need "in-degree" reframed as plain degree. |
| **Use When** | Build systems (Make, Bazel, Gradle) · package/dependency resolution (npm, pip, Cargo) · course/curriculum planning · spreadsheet and dataflow recalculation order (Airflow-style DAGs) · CI/CD stage ordering · any "must precede" scheduling, including detecting a deadlock-shaped dependency cycle. |
| **Avoid When** | Cycles are legitimate and meaningful (state machines, social graphs) — use Graph BFS/DFS · you need reachability or shortest hops rather than an ordering · you need incremental "connected?" / "would this edge close a cycle?" queries as edges stream in — use Union Find · the relationships are not precedence at all (similarity, distance, weight). |
| **Related Patterns** | Graph BFS/DFS (same queue-driven traversal; frontier gated by a `visited` set rather than by in-degree, cycles allowed) · Union Find (incremental connectivity and cycle queries, produces no ordering) · Tree BFS (level-by-level over a tree — Kahn's layer-at-a-time variant is the same shape applied to a DAG). |

### Template Skeleton

```cpp
// Kahn's algorithm (BFS). Default choice: forward output, one-line cycle check.
// edges are {u, v} meaning "u must come before v".
std::vector<int> topoSort(int numNodes,
                          const std::vector<std::pair<int,int> >& edges) {
    // adj[u] = everything that must come AFTER u. Write this sentence down
    // BEFORE coding — reversing it is the most common bug in the pattern.
    std::vector<std::vector<int> > adj(numNodes);
    std::vector<int> inDegree(numNodes, 0);
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first].push_back(edges[i].second);
        ++inDegree[edges[i].second];             // one increment per edge, exactly
    }

    std::queue<int> ready;                       // min-heap here => lexicographically smallest
    for (int n = 0; n < numNodes; ++n)
        if (inDegree[n] == 0) ready.push(n);     // no prerequisites at all

    std::vector<int> order;
    while (!ready.empty()) {
        // if (ready.size() > 1) -> more than one valid order exists (uniqueness test)
        int cur = ready.front(); ready.pop();
        order.push_back(cur);                    // drop this line => "is it possible?" only
        for (size_t i = 0; i < adj[cur].size(); ++i) {
            int nxt = adj[cur][i];
            if (--inDegree[nxt] == 0) ready.push(nxt);  // last prerequisite satisfied
        }
    }

    // THE check. Without it, a cyclic graph silently returns a partial order.
    if ((int)order.size() != numNodes) return std::vector<int>();  // cycle
    return order;
}

// DFS post-order + reverse. Colours: 0 white (unseen), 1 gray (on the stack),
// 2 black (finished). Gray-on-gray is the cycle signal; black is fine.
bool dfs(int u, const std::vector<std::vector<int> >& adj,
         std::vector<int>& colour, std::vector<int>& post) {
    colour[u] = 1;
    for (size_t i = 0; i < adj[u].size(); ++i) {
        int v = adj[u][i];
        if (colour[v] == 1) return false;                        // back-edge => cycle
        if (colour[v] == 0 && !dfs(v, adj, colour, post)) return false;
    }
    colour[u] = 2;
    post.push_back(u);              // AFTER all descendants...
    return true;                    // ...so std::reverse(post) is the answer
}
```

### Remember In One Sentence
> **Topological Sort orders a directed graph so every edge `u -> v` puts `u` first, and it exists at all only if the graph is a DAG — so track each node's in-degree, repeatedly emit whatever has reached zero and decrement its dependents (Kahn's, where a short final order *is* the cycle proof and a ready queue of size one *is* the uniqueness proof), or run DFS and reverse the finish order (where a back-edge into a gray node *is* the cycle proof).**

### Two Facts People Get Wrong
- A topological sort returns **the** order for a graph? **No** — it returns *an* order. The moment two nodes sit at in-degree 0 simultaneously, either may go first, so at least two valid orders exist. The order is unique exactly when the ready queue holds exactly one node at every single step — which is precisely the test [problems/03-sequence-reconstruction.cpp](problems/03-sequence-reconstruction.cpp) implements.
- If the code runs to completion without crashing, the cycle was handled? **No** — on a cyclic graph Kahn's loop terminates perfectly normally; the queue just runs dry early. What detects the cycle is comparing `order.size()` to `numNodes`, nothing else. Skip that comparison and you return a valid order for a *subset* of the nodes, which is indistinguishable from a correct answer by inspection — see [problems/02-course-schedule-ii.cpp](problems/02-course-schedule-ii.cpp)'s "orderable prefix + downstream cycle" test.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the exact precondition for a topological order to exist, and say why it is an "if and only if" rather than just "if".
2. In `topologicalSort` in [code.cpp](code.cpp), what does `adj[x]` hold — the prerequisites of `x`, or the things that depend on `x`? Which line would you change if you had it backwards?
3. Kahn's algorithm detects a cycle with one comparison. Which comparison, and why does that condition imply a cycle rather than merely an unusual graph?
4. LeetCode gives prerequisites as `{a, b}`. Which direction does the edge run, and why is [problems/01-course-schedule.cpp](problems/01-course-schedule.cpp) a place where getting it backwards can still pass the tests?
5. What is the *only* difference in the algorithm between `problems/01` (Course Schedule) and `problems/02` (Course Schedule II)?
6. Give the exact condition under which a graph's topological order is unique, and explain why DFS post-order cannot check it while Kahn's can.
7. In `problems/03-sequence-reconstruction.cpp` there are three separate conditions that must all hold for the answer to be `true`. Name all three, and say which wrong answer each one alone prevents.
8. In `problems/04-alien-dictionary.cpp`, why does only the *first* differing character of two adjacent words produce an edge — and what breaks if you emit an edge for every differing position?
9. In the same file, name the failure case that has nothing to do with cycles and cannot be detected by any graph algorithm. How is it detected instead?
10. Why does DFS-based topological sort need three colours instead of a single `visited` boolean, and what does each of the three mean?
