# Segment Tree / Fenwick Tree — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Advanced DS pattern — range-query data structure. |
| **Recognition Signal** | **Repeated range queries (sum/min/max/count) interleaved with point updates** to individual elements. Plain Prefix Sum is O(1) query but O(n) per update; this pattern makes both O(log n). Counting problems of the form "how many earlier/later elements satisfy a value condition" are a hidden second signal. |
| **Problem** | A prefix-sum array answers range sums in O(1) but any update forces an O(n) rebuild of everything after it; rescanning the array per query costs O(n) per query, which is O(n·q) overall. |
| **Solution** | Store partial aggregates at tree-chosen indices so any prefix/range decomposes into O(log n) stored blocks. **Fenwick:** index `i` owns a run of length `i & (-i)`; update walks `i += i & (-i)`, query walks `i -= i & (-i)`. **Segment Tree:** implicit binary tree over ranges, node `i`'s children at `2i`/`2i+1`; both operations descend/merge along O(log n) levels. |
| **Time / Space Complexity** | O(log n) per update or query for either structure, O(n) space (Fenwick: exactly `n+1` slots; segment tree: `4n` safe bound). |
| **Pros** | Both operations logarithmic · Fenwick is ~10 lines and cache-friendly · segment tree generalizes to min/max/any associative op and lazy range updates · counting variants solve inversion/pair-counting problems that look nothing like "range sum" at first glance. |
| **Cons** | More code and constant factor than prefix sum · Fenwick does not naturally do min/max (needs invertibility or extra tricks) · 0-indexed input vs 1-indexed internal indexing is a classic off-by-one trap · large/negative values require coordinate compression before indexing · lazy propagation adds real complexity. |
| **Use When** | Range sums + point updates (Fenwick) · range min/max + updates (segment tree) · count inversions / smaller-numbers-after-self / reverse pairs / range-sum counting (Fenwick over compressed values) · streaming/mutable data where rebuilding per query is too slow. |
| **Avoid When** | Data is static — plain Prefix Sum gives O(1) queries with less code · only adjacent/sliding-window aggregates needed — Monotonic Queue is simpler · single build then many queries with no updates — sort once or precompute · need "next greater element" style scans — Monotonic Stack. |
| **Related Patterns** | Prefix Sum (the static special case this generalizes) · Merge Sort-based counting (LC 493/327 solvable either way — same O(n log n), different mechanics) · Sqrt Decomposition (O(√n) per op but far simpler for rare updates) · Union Find (another incremental-tree structure, for connectivity not aggregates). |

### Template Skeleton

```cpp
// Fenwick Tree (1-indexed internally; pass/receive 0-indexed positions)
class FenwickTree {
    std::vector<long long> tree;
    int n;
 public:
    explicit FenwickTree(int size) : tree(size + 1, 0), n(size) {}

    void update(int i, long long delta) {        // add delta at position i
        for (++i; i <= n; i += i & (-i)) tree[i] += delta;
    }
    long long query(int i) const {               // prefix aggregate [0..i]
        long long s = 0;
        for (++i; i > 0; i -= i & (-i)) s += tree[i];
        return s;
    }
    long long rangeSum(int l, int r) const {     // aggregate over [l..r]
        return query(r) - (l > 0 ? query(l - 1) : 0);
    }
};

// Counting idiom (inversions / after-self counts):
//   compress values -> walk elements in scan order ->
//   query how many already-inserted values satisfy the condition ->
//   insert current value's rank.
```

### Remember In One Sentence
> **A Fenwick/Segment Tree buys O(log n) point updates by storing partial aggregates at bit-trick-chosen (or tree-node) indices, trading the prefix sum's O(1)-query/O(n)-update imbalance for a balanced O(log n) on both operations.**

### Two Facts People Get Wrong
- A Fenwick Tree is just a slower prefix sum? **No** — it is a different decomposition: each slot holds a *partial* block sum, not a full prefix, which is exactly why one update touches O(log n) slots instead of invalidating every later prefix.
- You can do range-min queries with a Fenwick Tree as easily as range sums? **No** — min is not invertible (`query(r) - query(l-1)` has no analogue), so arbitrary range-min needs a Segment Tree; a Fenwick can only do prefix-min or special-cased tricks.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. What does `i & (-i)` compute, and how does the update loop use it differently from the query loop?
2. Why must a Fenwick Tree be indexed from 1 internally? What breaks with index 0?
3. How do you convert a point-*assignment* update ("set position i to v") into the delta update a Fenwick expects?
4. State the complexity tradeoff between Prefix Sum and Fenwick Tree, and when each side wins.
5. Why can't `rangeSum(l, r)` be answered by a single Fenwick walk? What identity replaces it?
6. When must you coordinate-compress before building a Fenwick Tree, and what do you index by after compressing?
7. In "count of smaller numbers after self," which direction do you scan and what exact question do you ask the tree at each step?
8. What structural property lets a Segment Tree support min/max while a Fenwick Tree is limited to invertible operations?
9. What is lazy propagation for, in one sentence?
10. Give two LeetCode-style problems where the answer is a *count* but the right tool is still a Fenwick Tree.
