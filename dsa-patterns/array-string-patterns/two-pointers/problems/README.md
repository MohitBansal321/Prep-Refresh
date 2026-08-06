# Two Pointers — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Two Pointers across its two flavors (converging pair/triplet search, converging optimization, and same-direction in-place compaction). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-pair-with-target-sum.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Two Sum II - Input Array Is Sorted | [167](https://leetcode.com/problems/two-sum-ii-input-array-is-sorted/) | Easy | Converging pointers on sorted array; move `left` up if sum too small, `right` down if too large. | O(n) time, O(1) space | [01-pair-with-target-sum.cpp](01-pair-with-target-sum.cpp) |
| Remove Duplicates from Sorted Array | [26](https://leetcode.com/problems/remove-duplicates-from-sorted-array/) | Easy | Same-direction slow `write` / fast `read` pointers; copy forward only when the value differs from the last written one. | O(n) time, O(1) space | [02-remove-duplicates-from-sorted-array.cpp](02-remove-duplicates-from-sorted-array.cpp) |
| 3Sum | [15](https://leetcode.com/problems/3sum/) | Medium | Sort, fix one element per outer loop, run converging two pointers on the rest for the remaining pair; skip duplicate values at every level. | O(n²) time, O(1) extra space | [03-3sum.cpp](03-3sum.cpp) |
| Container With Most Water | [11](https://leetcode.com/problems/container-with-most-water/) | Medium | Converging pointers from the widest span inward; always discard the shorter wall since it strictly caps the area. | O(n) time, O(1) space | [04-container-with-most-water.cpp](04-container-with-most-water.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the textbook converging-pointer exact-match search — the pattern in its purest form.
- **02** is the textbook same-direction in-place compaction — the pattern's other main flavor.
- **03** shows the pattern composed with sorting and an outer loop to solve a harder (triplet, not pair) problem, and is the canonical example of the "skip duplicates" subtlety.
- **04** shows the pattern used for optimization (maximize a value) rather than exact matching, with a non-obvious but provable greedy elimination rule.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
