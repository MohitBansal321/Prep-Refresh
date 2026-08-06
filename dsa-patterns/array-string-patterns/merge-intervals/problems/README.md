# Merge Intervals — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Merge Intervals and its two closest sort-and-sweep siblings (sort-by-start merging, sort-by-start insertion, and sort-by-end greedy grouping). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-merge-intervals.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Merge Intervals | [56](https://leetcode.com/problems/merge-intervals/) | Medium | Sort by start, sweep once, extend or flush a running "current merged interval." | O(n log n) time, O(1) extra space | [01-merge-intervals.cpp](01-merge-intervals.cpp) |
| Insert Interval | [57](https://leetcode.com/problems/insert-interval/) | Medium | Input is already sorted/disjoint; walk once in three phases (before / overlapping / after) absorbing overlaps into the new interval. | O(n) time, O(n) space (output) | [02-insert-interval.cpp](02-insert-interval.cpp) |
| Non-overlapping Intervals | [435](https://leetcode.com/problems/non-overlapping-intervals/) | Medium | Sort by **end** time, greedily keep the earliest-ending interval whenever the next one overlaps it. | O(n log n) time, O(1) extra space | [03-non-overlapping-intervals.cpp](03-non-overlapping-intervals.cpp) |
| Minimum Number of Arrows to Burst Balloons | [452](https://leetcode.com/problems/minimum-number-of-arrows-to-burst-balloons/) | Medium | Sort by **end** time, count new "groups" (arrows) each time a balloon starts after the current arrow's position. | O(n log n) time, O(1) extra space | [04-minimum-arrows-to-burst-balloons.cpp](04-minimum-arrows-to-burst-balloons.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):

- **01** is the textbook sort-by-start-and-sweep merge — the pattern in its purest form, and the direct ancestor of `mergeIntervals` in [../code.cpp](../code.cpp).
- **02** shows the pattern's other half: when the input is *already* sorted and disjoint (the postcondition of 01), a single O(n) pass suffices — no re-sort needed, which matters when insertion happens repeatedly against a maintained schedule.
- **03** and **04** both show the "sort by end, not start" variant that shows up whenever the question is "how many groups of overlap are there" (removals, or arrows) rather than "what do the merged blocks look like" — a subtle but important fork explained in the README's Common Mistakes section.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
