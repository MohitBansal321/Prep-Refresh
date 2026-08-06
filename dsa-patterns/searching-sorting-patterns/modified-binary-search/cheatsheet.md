# Modified Binary Search — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Searching & Sorting pattern — index-halving technique. |
| **Recognition Signal** | Input is **sorted**, **piecewise-sorted** (rotated at an unknown pivot), or has any other **monotonic property** (a boundary where a predicate flips from false to true), and you need better than O(n). |
| **Problem** | A linear scan checks every element — O(n) — even though the data's order already tells you, from a single comparison, which entire half of the remaining range cannot contain the answer. |
| **Solution** | `lo`/`hi`/`mid` window that starts as the whole array. At each step, a problem-specific predicate decides which half is provably irrelevant and discards it. Three shapes: **classic** (exact match), **boundary** (first/last occurrence — keep narrowing after a match), **rotated** (figure out which half is normally ordered first). |
| **Time / Space Complexity** | O(log n) time (the range halves every iteration), O(1) extra space (iterative — no recursion stack). |
| **Pros** | O(log n) instead of O(n) · no extra memory · same lo/hi/mid skeleton reused across many problem shapes · provably correct once the halving argument is stated. |
| **Cons** | Requires sorted/monotonic data · easy to get `mid` overflow, infinite loops, or off-by-one wrong · rotated-array halving test is easy to get backward (comparing `target` to `nums[mid]` directly, instead of first checking which half is sorted) · does not help at all on genuinely unordered data. |
| **Use When** | Sorted or rotated-sorted array + need O(log n) · finding the first/last index of a repeated value · finding a boundary between a false-region and a true-region (`std::lower_bound`/`upper_bound` territory) · finding an extremum in a rotated array. |
| **Avoid When** | Data has no monotonic property at all (no way to prove a half is irrelevant) · need a pair of elements from two ends, not one index (use Two Pointers) · array is unsorted and cannot be sorted without losing needed information. |
| **Related Patterns** | Two Pointers (pair/triplet search on sorted data, not a single-index search) · Top K Elements (heap-based, not halving-based) · `std::lower_bound`/`std::upper_bound` (the C++ standard library's own boundary search). |

### Template Skeleton

```cpp
// Classic exact-match search
int lo = 0, hi = n - 1;
while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;   // never (lo + hi) / 2 -> overflow risk
    if (nums[mid] == target) return mid;
    else if (nums[mid] < target) lo = mid + 1;  // discard left half
    else hi = mid - 1;                          // discard right half
}
return -1;  // not found

// Boundary search (e.g. first occurrence)
int lo = 0, hi = n - 1, result = -1;
while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;
    if (nums[mid] == target) {
        result = mid;
        hi = mid - 1;        // record, then keep narrowing left
    } else if (nums[mid] < target) lo = mid + 1;
    else hi = mid - 1;
}
return result;

// Rotated-array search
int lo = 0, hi = n - 1;
while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;
    if (nums[mid] == target) return mid;
    if (nums[lo] <= nums[mid]) {               // left half is sorted
        if (nums[lo] <= target && target < nums[mid]) hi = mid - 1;
        else lo = mid + 1;
    } else {                                   // right half is sorted
        if (nums[mid] < target && target <= nums[hi]) lo = mid + 1;
        else hi = mid - 1;
    }
}
return -1;
```

### Remember In One Sentence
> **Modified Binary Search generalizes "find an exact value" to "which half of the search space is provably still valid, and which can be discarded" — the same lo/hi/mid halving loop answers exact-match, boundary, and rotated-array questions by swapping only the predicate that decides which half survives.**

### Two Facts People Get Wrong
- `(lo + hi) / 2` is just a **style preference** over `lo + (hi - lo) / 2`? **No** — for large enough `lo` and `hi`, `lo + hi` can overflow a signed 32-bit int before the division happens, silently producing a negative or wrong `mid`. This is a real, documented bug class (it shipped in the JDK's binary search for years), not a nitpick.
- Binary search on a rotated array means **comparing `target` to `nums[mid]` directly, like normal**? **No** — the array is not globally sorted, so that comparison is meaningless on its own. You must first determine which *half* is internally sorted, then test whether `target`'s value falls inside that half's known range.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. Why is `mid = lo + (hi - lo) / 2` preferred over `mid = (lo + hi) / 2`, concretely?
2. In a first-occurrence search, what happens on the iteration where `nums[mid] == target`, and why do we not just return `mid` immediately?
3. In `searchRotated`, how do you determine which half — `[lo..mid]` or `[mid..hi]` — is the normally-sorted one?
4. Why does `findMin` (LeetCode 153) compare `nums[mid]` to `nums[hi]` instead of to `nums[lo]`?
5. What loop condition difference exists between the classic/boundary searches (`lo <= hi`) and `findMin` (`lo < hi`), and why does that difference matter?
6. Name one concrete way an infinite loop can happen in a binary search implementation.
7. Why does classic binary search alone fail on a rotated sorted array?
8. What is the time complexity win versus a linear scan, and why does it hold in the worst case too?
9. Give one real production/system context (not a LeetCode problem) where boundary-style binary search shows up.
10. What is the key structural difference between Modified Binary Search and Two Pointers, given that both exploit sorted data?
