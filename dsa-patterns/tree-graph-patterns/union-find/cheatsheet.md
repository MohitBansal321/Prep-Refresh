# Union Find (Disjoint Set) — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Tree/Graph pattern — connectivity tracking without traversal. |
| **Recognition Signal** | Edges/connections arrive **incrementally** (or you must check many pairs against a graph that keeps changing), and the question is repeatedly **"are these two connected?"** or **"would adding this edge create a cycle?"** — never "what is the actual path." |
| **Problem** | Re-running BFS/DFS from scratch on every connectivity query costs O(V+E) *per query*; with many interleaved queries/unions that becomes O(query_count · (V+E)) overall. |
| **Solution** | Each element points to a `parent`; `find(x)` walks up to the group's root, flattening the chain along the way (**path compression**); `unionSets(x, y)` finds both roots and attaches the shorter/shallower tree under the taller one (**union by rank/size**). Two elements are connected iff `find(a) == find(b)`. |
| **Time / Space Complexity** | Amortized **O(alpha(n))** per `find`/`unionSets` with both optimizations — alpha is the inverse Ackermann function, under 5 for any realistic n, hence "effectively O(1)." O(n) space for the `parent`/`rank`/`size` arrays. Without both optimizations, a single operation degrades to O(n) worst case. |
| **Pros** | Near-O(1) amortized per operation · trivial cycle detection (`unionSets` returning false) · trivial component counting · no re-traversal ever needed as edges keep arriving · simple, small, easy-to-verify implementation. |
| **Cons** | Cannot report the actual path/edges between two connected nodes · no efficient "un-union" / edge-removal support · needs elements mapped to small dense integer indices (extra step for strings/records) · undirected only — no notion of edge direction. |
| **Use When** | Dynamic/incremental connectivity queries · cycle detection while building a graph edge by edge · counting connected components from a full edge list · merging records that share a common key (accounts by email, stones by row/column) · Kruskal's MST (skip edges that would close a cycle). |
| **Avoid When** | You need the actual path or shortest hop count (use Graph BFS/DFS) · the graph is directed with a "must come before" ordering (use Topological Sort) · edges need to be *removed* efficiently and re-queried (Union Find has no cheap un-union). |
| **Related Patterns** | Graph BFS/DFS (path/shortest-path/one-shot cycle detection, not repeated incremental queries) · Topological Sort (directed dependency ordering, not undirected connectivity) · Kruskal's MST (uses Union Find directly as its cycle-skip mechanism). |

### Template Skeleton

```cpp
class DisjointSet {
 public:
  explicit DisjointSet(int n) : parent(n), rank_(n, 0) {
    for (int i = 0; i < n; ++i) parent[i] = i;   // every element starts as its own root
  }

  int find(int x) {
    if (parent[x] != x) {
      parent[x] = find(parent[x]);   // PATH COMPRESSION: flatten on the way back
    }
    return parent[x];
  }

  // Returns false if x and y were ALREADY connected (redundant edge / cycle).
  bool unionSets(int x, int y) {
    int rootX = find(x), rootY = find(y);
    if (rootX == rootY) return false;

    if (rank_[rootX] < rank_[rootY]) std::swap(rootX, rootY);
    parent[rootY] = rootX;                        // UNION BY RANK: shorter under taller
    if (rank_[rootX] == rank_[rootY]) ++rank_[rootX];
    return true;
  }

  bool connected(int x, int y) { return find(x) == find(y); }

 private:
  std::vector<int> parent;
  std::vector<int> rank_;
};
```

### Remember In One Sentence
> **Union Find answers "are these connected?" and "did this edge just close a cycle?" in near-constant amortized time per operation by maintaining a forest of parent pointers that path compression keeps flat and union by rank keeps balanced — trading away the ability to report the actual path for the ability to never re-traverse the graph on a repeated query.**

### Two Facts People Get Wrong
- Union Find can tell you the **path** between two connected nodes? **No** — it only ever answers yes/no connectivity (or a component count/size); if you need the actual sequence of edges, you need Graph BFS/DFS, possibly layered on top of Union Find.
- Skipping path compression or union by rank is just a **minor** performance tweak? **No** — without *both*, a deliberately adversarial sequence of unions can degrade a single `find()` to O(n) (a straight-line chain), turning what looks like an O(n · alpha(n)) algorithm into O(n²) in the worst case.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the recognition signal that means "reach for Union Find" instead of a fresh BFS/DFS on every query.
2. What does `parent[i] = i` mean for a freshly initialized element, and why is that the correct starting state (not `-1` or some sentinel)?
3. Why is `find(a) == find(b)` the correct connectivity check, but `parent[a] == parent[b]` is not?
4. What does path compression actually change inside `find(x)`, and why does it make future calls faster?
5. In `unionSets`, why do we compare `rank[rootX]` and `rank[rootY]` before attaching one root under the other, instead of always attaching `y`'s root under `x`'s root?
6. When, exactly, does `rank[rootX]` get incremented during a union, and why only in that one case?
7. What does it mean, precisely, when `unionSets(x, y)` returns `false`? Name two different worked problems in this module where that return value IS the entire answer.
8. What is `alpha(n)` (the inverse Ackermann function), and why is "amortized O(alpha(n))" described as "effectively O(1)" rather than "exactly O(1)"?
9. Give one real production/system context (not a LeetCode problem) where incremental connectivity tracking (not a one-shot BFS/DFS) is the right tool.
10. What is the key structural difference between Union Find and Graph BFS/DFS, given that both can answer "are these two nodes connected"?
