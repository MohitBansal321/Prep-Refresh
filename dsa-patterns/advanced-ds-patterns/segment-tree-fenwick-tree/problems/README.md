# Segment Tree / Fenwick Tree — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating the Fenwick Tree across its two faces: direct range-sum maintenance (LC 307) and the counting family — "how many elements before/after me satisfy a value condition" (LC 315, 493, 327) — where coordinate compression is mandatory. Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers, including brute-force cross-checks on the counting problems.

```bash
g++ -std=c++17 -Wall problems/01-range-sum-query-mutable.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Range Sum Query - Mutable | [307](https://leetcode.com/problems/range-sum-query-mutable/) | Medium | Fenwick over raw positions; point-assignment updates become deltas by keeping the current value array (`delta = val - cur[i]`). | O(log n) per op, O(n) space | [01-range-sum-query-mutable.cpp](01-range-sum-query-mutable.cpp) |
| Count of Smaller Numbers After Self | [315](https://leetcode.com/problems/count-of-smaller-numbers-after-self/) | Hard | Scan right-to-left; at each element ask the tree how many already-inserted values are strictly smaller, then insert its compressed rank. | O(n log n) time, O(n) space | [02-count-of-smaller-numbers-after-self.cpp](02-count-of-smaller-numbers-after-self.cpp) |
| Reverse Pairs | [493](https://leetcode.com/problems/reverse-pairs/) | Hard | Scan left-to-right; count inserted values strictly greater than `2 * nums[j]` via `inserted - query(rank(2*nums[j]))`, all keys in `long long`. | O(n log n) time, O(n) space | [03-reverse-pairs.cpp](03-reverse-pairs.cpp) |
| Count of Range Sum | [327](https://leetcode.com/problems/count-of-range-sum/) | Hard | Prefix sums turn it into counting pairs `S[j] - S[i]` in `[lower, upper]`; per `j`, count earlier prefix sums in a value window with two prefix-count queries. | O(n log n) time, O(n) space | [04-count-of-range-sum.cpp](04-count-of-range-sum.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the textbook range-sums-plus-point-updates problem — the pattern in its purest form, and the source of the delta-vs-assignment subtlety.
- **02** is the canonical counting variant: the tree stores *occurrence counts* over compressed values, not sums of the input.
- **03** pushes the counting idea past "smaller than me" to a threshold comparison (`> 2 * nums[j]`) that forces `long long` keys and derived-rank queries.
- **04** is the hardest composition: a pair-counting problem over *derived* values (prefix sums), where both the indexed values and the query window endpoints must be compressed together.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
