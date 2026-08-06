# Two Pointers — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Array/String pattern — index-manipulation technique. |
| **Recognition Signal** | Input is (or can be) **sorted**, and you need a **pair/triplet summing to / comparing against a target**, an **optimum derived from two endpoints** (area, capacity), or **in-place compaction** (remove duplicates/values, partition, move zeroes). |
| **Problem** | Brute-force nested loops over all pairs/triplets cost O(n²) or O(n³); repeatedly scanning for duplicates or filtering while shifting elements also costs O(n²). |
| **Solution** | Two index variables walking the same array instead of nested loops. **Converging:** `left=0`, `right=n-1`, move one inward per comparison. **Same-direction:** slow `write` + fast `read`, both starting at 0, moving at different rates. |
| **Time / Space Complexity** | O(n) time (single pass, pointers move at most n steps combined), O(1) extra space — no auxiliary array or hash structure needed. |
| **Pros** | O(n) time in O(1) space · no hash structure to size or maintain · exploits sort order directly · simple to reason about and prove correct (monotonic narrowing) · in-place, cache-friendly. |
| **Cons** | Requires sorted input for the converging variant (sorting unsorted input costs O(n log n) and can destroy original indices) · easy to get pointer-movement direction wrong · 3Sum-style problems need careful duplicate-skipping · does not directly generalize past pairs/triplets without an outer loop. |
| **Use When** | Sorted array + pair/triplet search · area/capacity maximization from two endpoints · in-place removal/compaction under an O(1)-space constraint · merging two sorted sequences. |
| **Avoid When** | Data is unsorted and original indices matter (use hashing) · you need a variable-size *contiguous* window with a running property (use Sliding Window) · you need single-index search only (use Binary Search) · the relationship being searched is not monotonic as pointers move. |
| **Related Patterns** | Sliding Window (contiguous range, not two independent endpoints) · Fast & Slow Pointers (cycle detection on linked lists, different rates not different starting ends) · Merge step of Merge Sort (same-direction pointers over two separate sorted arrays). |

### Template Skeleton

```cpp
// Converging variant (sorted array, pair search or optimization)
int left = 0, right = n - 1;
while (left < right) {
    auto current = combine(a[left], a[right]);   // e.g. sum, or min(h[l],h[r])*width
    if (current == target) { /* found */ break; }
    else if (current < target) ++left;   // too small -> only left++ can increase it
    else --right;                        // too large -> only right-- can decrease it
}

// Same-direction variant (in-place compaction)
int write = 0;
for (int read = 0; read < n; ++read) {
    if (keep(a[read])) {
        a[write] = a[read];
        ++write;
    }
}
// a[0..write-1] is the compacted result.
```

### Remember In One Sentence
> **Two Pointers replaces an O(n²) nested scan with a single O(n) pass by using two indices whose movement is provably monotonic — either converging from opposite ends of sorted data, or advancing together at different rates through an unsorted-order-preserving compaction.**

### Two Facts People Get Wrong
- Two Pointers works on **any** array? **No** — the converging variant strictly requires sorted (or sortable-without-information-loss) input; on unsorted data the pointer-movement argument breaks entirely.
- 3Sum's duplicate-skip is a **minor detail**? **No** — it is required for correctness of the *output* (unique triplets), not just an optimization; forgetting it produces wrong answers, not just slower ones.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the two recognition signals that mean "reach for Two Pointers" — one for the converging variant, one for the same-direction variant.
2. Why must the input be sorted for the converging variant? What breaks if it is not?
3. In a pair-sum search, if the current sum is too small, which pointer moves and why is moving the other pointer never useful?
4. In Container With Most Water, why is it always safe to discard the shorter wall's pointer?
5. What invariant does the `write` pointer maintain in the same-direction (compaction) variant?
6. Name the brute-force complexity that Two Pointers replaces for: (a) pair-sum search, (b) 3Sum, (c) in-place deduplication.
7. Why does 3Sum need explicit duplicate-skipping, but Two Sum II does not?
8. How does Two Pointers' time/space tradeoff compare to a hash-based pair-sum solution?
9. Give one real production/system context (not a LeetCode problem) where the same-direction two-pointer shape shows up.
10. What is the key structural difference between Two Pointers and Sliding Window, given that both use two indices over an array?
