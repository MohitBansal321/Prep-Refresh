# Top K Elements — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating the Top K Elements pattern across its variants (largest-K via min-heap, smallest-K via max-heap, frequency-keyed with and without custom tie-breaking). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-kth-largest-element-in-an-array.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Kth Largest Element in an Array | [215](https://leetcode.com/problems/kth-largest-element-in-an-array/) | Medium | Min-heap of size k; the top of the heap after one full pass IS the k-th largest. | O(n log k) time, O(k) space | [01-kth-largest-element-in-an-array.cpp](01-kth-largest-element-in-an-array.cpp) |
| Top K Frequent Elements | [347](https://leetcode.com/problems/top-k-frequent-elements/) | Medium | Hash map to count occurrences, then min-heap of size k keyed by `(frequency, value)`. | O(n + d log k) time, O(d + k) space | [02-top-k-frequent-elements.cpp](02-top-k-frequent-elements.cpp) |
| K Closest Points to Origin | [973](https://leetcode.com/problems/k-closest-points-to-origin/) | Medium | Max-heap of size k keyed by squared distance from origin (inverted heap type: "smallest K" wants a max-heap). | O(n log k) time, O(k) space | [03-k-closest-points-to-origin.cpp](03-k-closest-points-to-origin.cpp) |
| Top K Frequent Words | [692](https://leetcode.com/problems/top-k-frequent-words/) | Medium | Hash map to count occurrences, then min-heap of size k with a custom comparator: higher frequency wins, lexicographically smaller word wins ties. | O(n + d log k) time, O(d + k) space | [04-top-k-frequent-words.cpp](04-top-k-frequent-words.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the textbook min-heap-of-size-K search for "K largest" in its purest form — no frequency counting, no derived key, just the raw push/evict rule.
- **02** introduces the frequency-keyed variant: a preliminary hash-map counting pass, then the identical push/evict rule applied to `(frequency, value)` pairs.
- **03** is the mirror-image case: "K smallest" (closest points), which inverts the heap type to a max-heap — the single most commonly mixed-up detail in this whole pattern.
- **04** composes the frequency-keyed variant with a custom comparator, showing how the pattern handles tie-breaking rules beyond a plain numeric comparison.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
