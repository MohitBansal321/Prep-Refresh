# Cyclic Sort — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Array pattern — in-place placement using the array itself as an implicit hash table. |
| **Recognition Signal** | Array holds `n` numbers drawn from a bounded range like `[1..n]` or `[0..n-1]`, and the question is about a missing, duplicate, or misplaced value. |
| **Problem** | Sorting costs `O(n log n)`; hashing costs `O(n)` extra space. Neither exploits the fact that each value already knows exactly where it "should" live. |
| **Solution** | Walk a cursor through the array; at each position, if the current value isn't at its home index (`value - 1` for `[1..n]`, or `value` for `[0..n-1]`) and its home doesn't already hold an identical value, swap it there and re-check the same position; otherwise advance. |
| **Participants** | The array (mutated in place) · cursor `i` · the home-index formula (the one thing that changes per problem variant) · the post-sort verification scan. |
| **Flow** | While `i < n`: compute home index for `nums[i]`; if in range and not already home, swap into place and re-check `i`; else advance `i`. After sorting, scan for the first `nums[i] != expected(i)` to reveal missing/duplicate/misplaced values. |
| **Pros** | `O(n)` time, `O(1)` extra space · no separate hash structure needed · the array itself becomes the lookup table. |
| **Cons** | Mutates the input · only applies when values map cleanly to a bounded index range · duplicates need careful "already home" handling to avoid infinite loops. |
| **Use When** | Values are exactly (or almost exactly) `1..n` or `0..n-1` and you need the missing/duplicate/first-missing-positive value, in `O(n)`/`O(1)`. |
| **Avoid When** | Values span a huge or unbounded range · input must not be mutated (use hashing or Fast & Slow Pointers instead) · you need to preserve original array order after the check. |
| **Real Examples** | Data-integrity checks for sequential ID assignment · detecting corrupted/duplicated batch record IDs · classic interview warm-ups (Missing Number, Find the Duplicate). |
| **Related Topics** | Fast & Slow Pointers (an alternative `O(1)`-space, non-mutating way to find a single duplicate) · hashing (simplest but `O(n)` space) · sorting (general but `O(n log n)`). |

### Skeleton
```cpp
void cyclic_sort(std::vector<int>& nums) {
  int n = nums.size(), i = 0;
  while (i < n) {
    int correct_index = nums[i] - 1;  // adjust formula per problem's range convention
    if (nums[i] >= 1 && nums[i] <= n && nums[i] != nums[correct_index]) {
      std::swap(nums[i], nums[correct_index]);   // do NOT advance i here
    } else {
      ++i;
    }
  }
}
// Then scan: for i in [0,n), if nums[i] != i+1, that slot reveals a mismatch.
```

### Remember In One Sentence
> **Cyclic Sort swaps every value directly to its "home" index in one in-place pass, using the array itself as a hash table — turning missing/duplicate/misplaced-value questions into a single O(n)-time, O(1)-space scan instead of a sort or a hash set.**

### Two Facts People Get Wrong
- Does it work on **any** array of numbers? **No** — only when values map to a bounded index range (`[1..n]`, `[0..n-1]`, or similar); unbounded/sparse ranges need hashing instead.
- Is it always the **best** O(1)-space choice? **No** — Fast & Slow Pointers solves "find the duplicate" in O(1) space *without* mutating the input, which cyclic sort cannot avoid.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. What is the home-index formula for a `[1..n]`-ranged array? For a `[0..n-1]`-ranged array?
2. Why does the swap loop NOT advance the cursor after a successful swap?
3. What's the termination argument for why this runs in `O(n)` total time, not `O(n^2)`?
4. How do you handle a value that's out of range (e.g. negative, zero when the range is `[1..n]`, or greater than `n`)?
5. After cyclic-sorting, how do you find a single missing value? A single duplicate? Both simultaneously?
6. Why must the "home already holds an identical value" check exist — what would go wrong without it on an array with duplicates?
7. Name one alternative technique for "find the duplicate number" that doesn't mutate the input, and state its space complexity.
8. Why is this pattern described as "using the array as an implicit hash table"?
9. What happens to the algorithm's correctness if values are NOT confined to a bounded range close to `[1..n]`?
10. Give one real-world scenario where mutating the input array during this check would be unacceptable, and what you'd use instead.
