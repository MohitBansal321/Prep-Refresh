# Prefix Sum — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Array/String pattern — precomputation technique. |
| **Recognition Signal** | The same (static, unchanging) array is queried **repeatedly** for the sum (or count) over arbitrary ranges `[i, j]`, or you need to check whether a subarray with some sum-derived property exists. |
| **Problem** | Naive re-scanning of each range costs O(range length) per query; over `q` queries that is O(n·q) total — wasteful when the array never changes between queries. |
| **Solution** | Build a running-total array `P` once: `P[0] = 0`, `P[i] = P[i-1] + arr[i-1]`. Answer any range sum with `rangeSum(i, j) = P[j+1] - P[i]`. |
| **Time / Space Complexity** | O(n) one-time build, O(1) per query thereafter — O(n + q) total for `q` queries, vs. O(n·q) naive. O(n) extra space for `P` (O(rows·cols) in 2D). |
| **Pros** | O(1) query time regardless of range width · trivial one-sentence correctness argument · very low constant factor (no pointers/recursion) · generalizes cleanly to 2D (submatrix sums) and to products/XOR (any invertible operation) · composes with a hash map of prefix-sum frequencies for subarray-existence problems. |
| **Cons** | Requires a static (or rarely-changing) array — a single point update forces an O(n) rebuild · O(n) extra space · silent, not loud, failure if you forget to rebuild after a mutation · only works for **invertible** aggregates (sum, product, XOR) — not min/max. |
| **Use When** | Many range-sum queries over a fixed array · subarray-sum-equals-K style existence checks · 2D submatrix sums (integral images) · any prefix/suffix aggregate over an invertible operation. |
| **Avoid When** | Updates are frequent and interleaved with queries (use Segment Tree/Fenwick Tree) · you only expect one or two queries total (the build cost does not pay for itself) · you need range minimum/maximum (not invertible — use Sparse Table or Segment Tree). |
| **Related Patterns** | Sliding Window (a moving window during one pass, not arbitrary after-the-fact queries) · Segment Tree / Fenwick Tree (O(log n) query, but supports O(log n) updates) · Difference Array (the "reverse" trick: cheap range *updates* instead of cheap range *queries*). |

### Template Skeleton

```cpp
// Build phase — O(n), done once.
std::vector<long long> prefix(n + 1, 0);
for (int i = 1; i <= n; ++i) {
    prefix[i] = prefix[i - 1] + arr[i - 1];
}

// Query phase — O(1), done as many times as needed.
long long rangeSum(int i, int j) {           // inclusive, 0-indexed
    return prefix[j + 1] - prefix[i];
}

// Subarray-sum-equals-K family (prefix sum + hash map of frequencies):
std::unordered_map<long long, int> seen{{0, 1}};   // prefixSum -> count seen so far
long long running = 0;
int count = 0;
for (int x : arr) {
    running += x;
    count += seen[running - k];   // how many earlier prefixes make this window == k
    ++seen[running];
}
```

### Remember In One Sentence
> **Prefix Sum pays an O(n) precomputation cost once so that any range-sum query afterward is a single O(1) subtraction — and it only pays for itself when the array is static and there is more than one query.**

### Two Facts People Get Wrong
- Prefix Sum is a **net win even for a single query**? **No** — building `P` costs the same O(n) as just scanning the range directly; the win only appears once you amortize the build across multiple queries.
- A single element update means an **O(1) patch** to the prefix array? **No** — because every `P[k]` for `k` at or after the changed index depends on it, an update invalidates a whole suffix of `P`, forcing an O(n) rebuild (or worst case O(n) suffix recompute) — this is exactly why Segment Tree/Fenwick Tree exist.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. Why is the prefix array built with length `n + 1` instead of `n`, and what bug does that extra slot specifically prevent?
2. Write the range-sum query formula from memory, and explain in one sentence why the subtraction gives exactly the elements in `[i, j]`.
3. What is the total time cost of answering `q` range-sum queries with Prefix Sum, versus the naive re-scan approach?
4. Why does updating a single element force an O(n) rebuild rather than an O(1) patch?
5. Name the two data structures you would reach for instead, once updates become frequent and interleaved with queries — and state their query/update complexity.
6. Explain how the prefix-sum-plus-hash-map technique turns "does a subarray summing to K exist" into an O(n) algorithm — what specific rearrangement of the range-sum formula makes this possible?
7. Why does the subtraction trick fail for range *minimum*/*maximum* queries, but work for sum, product, and XOR?
8. Write the four-corner inclusion-exclusion formula for a 2D submatrix sum from memory.
9. Give one real production/system context (not a LeetCode problem) where 2D prefix sums are used directly.
10. What is the key structural difference between Prefix Sum and Sliding Window, given that both are commonly used on "sum over a subarray" problems?
