# Modified Binary Search — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Modified Binary Search across its three shapes (classic exact match, boundary/first-last occurrence, and rotated-array search). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-binary-search.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Binary Search | [704](https://leetcode.com/problems/binary-search/) | Easy | Classic lo/hi/mid halving; three-way compare `nums[mid]` against `target`. | O(log n) time, O(1) space | [01-binary-search.cpp](01-binary-search.cpp) |
| Find First and Last Position of Element in Sorted Array | [34](https://leetcode.com/problems/find-first-and-last-position-of-element-in-sorted-array/) | Medium | Two independent boundary searches; on a match, keep narrowing left (first) or right (last) instead of stopping. | O(log n) time, O(1) space | [02-find-first-and-last-position-of-element-in-sorted-array.cpp](02-find-first-and-last-position-of-element-in-sorted-array.cpp) |
| Search in Rotated Sorted Array | [33](https://leetcode.com/problems/search-in-rotated-sorted-array/) | Medium | At each mid, determine which half is normally sorted, then test whether `target`'s value range falls inside it. | O(log n) time, O(1) space | [03-search-in-rotated-sorted-array.cpp](03-search-in-rotated-sorted-array.cpp) |
| Find Minimum in Rotated Sorted Array | [153](https://leetcode.com/problems/find-minimum-in-rotated-sorted-array/) | Medium | Compare `nums[mid]` to `nums[hi]` to decide which side of the rotation point `mid` is on; converge `lo == hi` onto the pivot. | O(log n) time, O(1) space | [04-find-minimum-in-rotated-sorted-array.cpp](04-find-minimum-in-rotated-sorted-array.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the textbook classic exact-match search — the pattern in its purest, most foundational form, and the direct ancestor of every other variant's lo/hi/mid skeleton.
- **02** is the textbook boundary-finding search, run twice (once per edge) — shows how "found a match" and "found the *first* match" are different questions requiring a modified halving rule.
- **03** shows the halving *test itself* changing to handle piecewise-sorted (rotated) data — the canonical "which half is guaranteed valid?" problem.
- **04** is a pure boundary search on a rotated array with no target value at all — the boundary being found is the rotation point itself, which is a subtly different (and easier to get backward) two-way comparison than 03's three-way one.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
