# Bitmask DP — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Bitmask DP across its four common flavors (position-counting arrangements, equal-sum partitioning, game-theory win/loss over a shrinking pool, and min-max assignment). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-beautiful-arrangement.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Beautiful Arrangement | [526](https://leetcode.com/problems/beautiful-arrangement/) | Medium | Count arrangements position-by-position: `solve(mask)` fills position `popcount(mask)+1` with each unused number passing the divisibility rule; memoize on the mask. | O(2^n * n) time, O(2^n) space | [01-beautiful-arrangement.cpp](01-beautiful-arrangement.cpp) |
| Partition to K Equal Sum Subsets | [698](https://leetcode.com/problems/partition-to-k-equal-sum-subsets/) | Medium | Bottom-up `dp[mask]` = current group's running sum mod target; extend by any unset bit that keeps the sum within target; succeed iff `dp[full] == 0`. | O(2^n * n) time, O(2^n) space | [02-partition-to-k-equal-sum-subsets.cpp](02-partition-to-k-equal-sum-subsets.cpp) |
| Can I Win | [464](https://leetcode.com/problems/can-i-win/) | Medium | Game-theory bitmask: `canWin(mask, remaining)` wins if some unused number reaches the total or leaves the opponent losing; mask alone determines remaining, so one memo dimension suffices. | O(2^n * n) time, O(2^n) space | [03-can-i-win.cpp](03-can-i-win.cpp) |
| Fair Distribution of Cookies | [2305](https://leetcode.com/problems/fair-distribution-of-cookies/) | Medium | Assign whole subsets of bags to children one at a time: enumerate submasks of unassigned bags per child via `(sub - 1) & free`; value = min over splits of max(child sum, rest). | O(k * 3^n) time, O(k * 2^n) space | [04-fair-distribution-of-cookies.cpp](04-fair-distribution-of-cookies.cpp) |

## Why these four

They cover every state shape called out in the [README](../README.md) and the [recognition diagram](../images/recognition-diagram.md):
- **01** is the counting flavor — the position being filled is derived from `__builtin_popcount(mask)`, so no extra DP dimension is needed.
- **02** is the flat bottom-up flavor — a single `dp[mask]` array filled in increasing numeric order, showing why "setting a bit strictly increases the integer" makes iteration order trivial.
- **03** adds adversarial game theory on top of the mask: the same memo table now stores win/loss instead of a numeric optimum, and the transition inverts ("I win if I can move you to a loss").
- **04** shows subset-enumeration transitions (`3^n` submask iteration), the most general — and most expensive — form of the pattern.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
