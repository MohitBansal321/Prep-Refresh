# Segment Tree / Fenwick Tree (BIT)

## Intent

Support both range queries (sum/min/max over `[i, j]`) AND point updates in `O(log n)` each — for cases where a plain prefix-sum array can't handle updates efficiently, since any update to a prefix-sum array forces an `O(n)` rebuild of everything after it.

## Recognition Signal

The problem needs repeated range queries interleaved with updates to individual elements. Plain Prefix Sum ([../../array-string-patterns/prefix-sum/](../../array-string-patterns/prefix-sum/)) breaks down here — it's excellent for static data, but every update would force an `O(n)` rebuild.

## Core Idea

**Fenwick Tree (Binary Indexed Tree)** — the leaner of the two, for prefix-sum-style queries. It stores partial sums at cleverly chosen indices, exploiting the binary representation of the index itself: each index `i` is responsible for a range of size equal to the lowest set bit of `i`. Both `update` (add a delta to one position) and `query` (prefix sum up to a position) walk `O(log n)` steps, using `i += i & (-i)` to move to the next responsible index during update, and `i -= i & (-i)` to accumulate during query. A range sum `[l, r]` is `query(r) - query(l-1)`.

**Segment Tree** — more general, supports range min/max/sum (anything associative) and even range updates with lazy propagation. Represented as a binary tree over the array (often stored implicitly in an array, node `i`'s children at `2i` and `2i+1`), where each node holds the aggregate of its range. Both query and update walk `O(log n)` tree levels.

**When to reach for which:** Fenwick Tree for sum-based range queries with point updates — it's simpler to implement and has a smaller constant factor. Segment Tree when you need min/max (not just sum), or range updates, or any non-sum associative operation.

## Template

```cpp
// Fenwick Tree (1-indexed internally).
class FenwickTree {
  std::vector<long long> tree;
  int n;
 public:
  explicit FenwickTree(int size) : tree(size + 1, 0), n(size) {}

  void update(int i, long long delta) {       // add delta at position i (0-indexed)
    for (++i; i <= n; i += i & (-i)) tree[i] += delta;
  }

  long long query(int i) const {              // prefix sum [0, i] inclusive (0-indexed)
    long long sum = 0;
    for (++i; i > 0; i -= i & (-i)) sum += tree[i];
    return sum;
  }

  long long rangeSum(int l, int r) const {     // sum over [l, r] inclusive
    return query(r) - (l > 0 ? query(l - 1) : 0);
  }
};
```

## Complexity

**Time:** `O(log n)` per update or query, for either structure.
**Space:** `O(n)`.

**Comparison to plain Prefix Sum:** Prefix Sum is `O(1)` query but `O(n)` per update (a full rebuild); Fenwick/Segment Tree trade a small query cost (`O(log n)` instead of `O(1)`) for a massive update improvement (`O(log n)` instead of `O(n)`) — the right tradeoff whenever updates happen at all.

## Common Mistakes

- **Off-by-one between 0-indexed problem input and the Fenwick Tree's internal 1-indexing** — the `+1`/`-1` conversions in `update`/`query` are easy to get backward.
- **Using `i & (-i)` incorrectly** — this isolates the lowest set bit via two's-complement negation; on unsigned types this needs care.
- **Reaching for a Fenwick Tree when you need min/max, not sum** — Fenwick Trees are naturally suited to sum (and other invertible operations); min/max generally need a full Segment Tree instead.

## When To Use

- Repeated range-sum queries interleaved with point updates — Fenwick Tree.
- Repeated range min/max/sum queries interleaved with point OR range updates — Segment Tree (with lazy propagation for range updates).

## When NOT To Use

- **No updates at all, ever** — plain Prefix Sum ([../../array-string-patterns/prefix-sum/](../../array-string-patterns/prefix-sum/)) is simpler and gives `O(1)` queries.
- **Need "next greater/smaller" or sliding-window max, not arbitrary range queries** — Monotonic Stack/Queue ([../monotonic-stack-queue/](../monotonic-stack-queue/)) is the simpler, more direct tool.

## Similar Patterns

- **Prefix Sum** ([../../array-string-patterns/prefix-sum/](../../array-string-patterns/prefix-sum/)): the static-data special case this pattern generalizes — no updates needed means no reason to pay the `O(log n)` query cost.
- **Union Find** ([../../tree-graph-patterns/union-find/](../../tree-graph-patterns/union-find/)): a different `O(log*n)`-ish incremental structure, but for connectivity rather than range aggregates.

## Further Reading

- LeetCode — Range Sum Query - Mutable (307), Range Sum Query 2D - Mutable (308).
- CP-Algorithms (cp-algorithms.com) — "Fenwick Tree" and "Segment Tree" — the standard competitive-programming references for both structures.
- *Competitive Programmer's Handbook* — Antti Laaksonen — covers both structures with worked examples.
