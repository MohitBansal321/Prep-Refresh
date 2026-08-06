# Sliding Window — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Array & String pattern (contiguous-range scanning). |
| **Recognition Signal** | Problem asks about a **contiguous** subarray/substring. Either a fixed size `K` is given, or you need the **longest/shortest** contiguous range satisfying some condition. |
| **Problem** | Brute force re-scans every window from scratch: O(n·k) for fixed-size windows, O(n²) or worse for variable-size "find the best window" problems. |
| **Solution** | Maintain a window `[left, right]` and a **running aggregate** (sum, frequency map, distinct count). Move `right` to grow, move `left` to shrink — never reset and rescan. |
| **Participants** | **`left` pointer** (window start, only moves forward) · **`right` pointer** (window end, only moves forward) · **running aggregate** (sum/frequency map — updated incrementally on every expand and shrink) · **best answer tracker**. |
| **Flow (fixed-size)** | Build first window's aggregate once (O(k)) → slide one step at a time: add entering element, remove leaving element (O(1) each) → track best. |
| **Flow (variable-size)** | Expand `right`, update aggregate → while window invalid (or, for "shortest", while window valid): shrink `left`, update aggregate → record best after each valid state. |
| **Pros** | O(n) instead of O(n·k)/O(n²) · O(1) or O(alphabet) extra space · no re-scanning · elegant, short code once the template is internalized. |
| **Cons** | Only works for **contiguous** ranges · variable-size version requires the constraint to be **monotonic** with window size · easy to get shrink/update order wrong · doesn't directly handle negative numbers for sum-threshold problems. |
| **Use When** | Contiguous subarray/substring + (fixed size K) or (longest/shortest satisfying a monotonic condition) · rate limiting over a rolling time window · streaming/network buffer management. |
| **Avoid When** | The array isn't sorted and you need pairs/triplets (→ Two Pointers) · you need many range-sum queries on a static array with no monotonicity (→ Prefix Sum) · negative numbers break the shrink monotonicity (→ Prefix Sum + hash map) · you just want best contiguous sum with no size constraint (→ Kadane's). |
| **Related Topics** | Two Pointers (generalizes to: two indices, one window) · Prefix Sum (precompute vs. scan) · Kadane's Algorithm (running-state DP cousin) · Monotonic Deque (Sliding Window Maximum). |

### Template Skeleton

```cpp
// Fixed-size window
long long windowSum = 0;
for (int i = 0; i < k; ++i) windowSum += arr[i];
long long best = windowSum;
for (int right = k; right < n; ++right) {
    windowSum += arr[right];          // entering element
    windowSum -= arr[right - k];      // leaving element
    best = std::max(best, windowSum);
}

// Variable-size window (grow, shrink while invalid, record when valid)
int left = 0;
/* aggregate init */
for (int right = 0; right < n; ++right) {
    /* expand: fold arr[right] into aggregate */
    while (/* aggregate violates constraint */) {
        /* shrink: remove arr[left] from aggregate */
        ++left;
    }
    best = std::max(best, right - left + 1); // or track shortest, per problem
}
```

### Time / Space Complexity

| Variant | Time | Space | vs. Brute Force |
|---------|------|-------|-------------------|
| Fixed-size window | O(n) | O(1) | Brute force: O(n·k) |
| Variable-size window | O(n) amortized (each index visited by `left` at most once) | O(1) or O(distinct elements) for a frequency map | Brute force: O(n²) or O(n³) |

### Pros
One linear pass · reuses prior work instead of rescanning · small constant-space aggregate · directly maps to real systems (rate limiting, TCP flow control).

### Cons
Contiguous-only · variable-size needs monotonicity · shrink-loop bugs are easy to introduce silently · frequency-map erase-on-zero is a common forgotten step.

### Use When
Contiguous subarray/substring problems with a size constraint or an optimization target (longest/shortest) that is monotonic in window size.

### Avoid When
Non-contiguous subsets, unsorted-pair search, static-array range-sum queries, or sum thresholds over arrays containing negative numbers.

### Related Patterns
Two Pointers (`../two-pointers/`), Prefix Sum (`../prefix-sum/`), Kadane's Algorithm, Monotonic Deque.

### Remember In One Sentence
> **A window that only grows or shrinks — never resets and rescans — turns an O(n·k) or O(n²) brute-force scan into a single O(n) pass by keeping a running aggregate updated incrementally on both ends.**

### Two Facts People Get Wrong
- **"Shrinking once is always enough." No** — use a `while`, not an `if`; some violations (like a duplicate character several positions back) need multiple shrink steps in one outer iteration.
- **"This works for any sum threshold." No** — the shrink logic relies on the window's aggregate changing **monotonically** with size; negative numbers break that for sum thresholds and require Prefix Sum + a hash map instead.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. State the intent of Sliding Window in one sentence. What single idea does every variant share?
2. What is the brute-force complexity this pattern replaces, for (a) a fixed-size window problem and (b) a variable-size "longest substring" problem?
3. Name the three participants in the pattern and one responsibility each.
4. Walk through the fixed-size window's slide step: what exactly happens to the running aggregate on each slide?
5. Walk through the variable-size window's control flow: when does `right` move, and when does `left` move?
6. Why must the shrink step be a `while` loop rather than an `if`? Give a concrete example where a single shrink step is not enough.
7. Why does "Minimum Size Subarray Sum" require all-non-negative numbers? What breaks if negatives are allowed?
8. Give one concrete example of Sliding Window showing up in a production system outside of interview problems.
9. Contrast Sliding Window with Two Pointers in one sentence — what is the same, what is different?
10. What is the single most common bug people introduce when shrinking a frequency-map-based window, and why does it cause a wrong answer?
