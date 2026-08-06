# Two Heaps — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Two Heaps and its closest single-heap relative. Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-find-median-from-data-stream.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Complexity | File |
|------|-----------|------------|--------------------|------------|------|
| Kth Largest Element in a Stream | [703](https://leetcode.com/problems/kth-largest-element-in-a-stream/) | Easy | One min-heap capped at size k; if a new value beats the heap's top once full, swap it in. The kth largest is always the heap's top. | O(log k) per add, O(n) space for k elements | [04-kth-largest-element-in-a-stream.cpp](04-kth-largest-element-in-a-stream.cpp) |
| Find Median from Data Stream | [295](https://leetcode.com/problems/find-median-from-data-stream/) | Hard (LeetCode's own rating) | Max-heap for the lower half, min-heap for the upper half, rebalanced in size after every insert. | O(log n) per add, O(1) per query | [01-find-median-from-data-stream.cpp](01-find-median-from-data-stream.cpp) |
| IPO | [502](https://leetcode.com/problems/ipo/) | Hard | Min-heap of not-yet-affordable projects by capital requirement; max-heap of currently-affordable projects by profit; repeatedly unlock and greedily take the most profitable affordable project. | O(n log n) total for n projects, k rounds | [03-ipo.cpp](03-ipo.cpp) |
| Sliding Window Median | [480](https://leetcode.com/problems/sliding-window-median/) | Hard | Two heaps as in problem 01, plus lazy deletion (a hash map of pending removals) to handle values aging out of the window. | O(n log k) for window size k over n elements | [02-sliding-window-median.cpp](02-sliding-window-median.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **04** is a deliberate single-heap warm-up, placed first in study order even though it is numbered last alphabetically in some listings — it shows the "one fixed-size heap tracks one extreme value" shape that Two Heaps generalizes from one boundary to two.
- **01** is the textbook two-heap running median — the pattern in its purest form, and the direct ancestor of `code.cpp`'s `MedianFinder`.
- **03** shows the "two heaps split by a moving boundary" idea applied to a scheduling/greedy problem instead of a literal median — proof that the pattern generalizes beyond "compute the median."
- **02** is the hard extension: the same two-heap core as 01, plus lazy deletion, needed the moment old data must leave the structure (a sliding window) rather than only ever accumulating.

Recommended reading order: **04 -> 01 -> 03 -> 02** (single heap, to plain two heaps, to a structural variant, to the hardest extension).

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
