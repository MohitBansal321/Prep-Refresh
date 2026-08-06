# Sliding Window — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions. Each file compiles and runs on its own and prints `[PASS]`/`[FAIL]` for a set of test cases so you can verify correctness without a separate test framework.

Compile any of them with:

```bash
g++ -std=c++17 -Wall path/to/file.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Maximum Sum Subarray of Size K | N/A (Grokking/GfG canonical) | Easy | Fixed-size window; slide by adding the entering element and subtracting the leaving one | O(n) / O(1) | [01-max-sum-subarray-of-size-k.cpp](01-max-sum-subarray-of-size-k.cpp) |
| Longest Substring Without Repeating Characters | 3 | Medium | Variable-size window; jump `left` directly using a last-seen-index map when a duplicate enters | O(n) / O(min(n, alphabet)) | [02-longest-substring-without-repeating-characters.cpp](02-longest-substring-without-repeating-characters.cpp) |
| Minimum Window Substring | 76 | Hard | Variable-size window; grow until all of `t`'s characters are satisfied, then shrink while still satisfied, tracking the shortest valid window | O(\|s\| + \|t\|) / O(alphabet) | [03-minimum-window-substring.cpp](03-minimum-window-substring.cpp) |
| Minimum Size Subarray Sum | 209 | Medium | Variable-size window; grow until sum >= target, shrink while still >= target, tracking the shortest valid window | O(n) / O(1) | [04-minimum-size-subarray-sum.cpp](04-minimum-size-subarray-sum.cpp) |

## Why these four

- **01** is the simplest possible fixed-size window — the "hello world" of the pattern. Study it first to internalize the add-on-enter / subtract-on-exit mechanism before anything gets a shrink loop.
- **02** introduces the variable-size window and the "jump left directly" optimization (a valid variant of shrinking — see the README's Common Mistakes section for why a naive one-step-at-a-time shrink would still be correct but the jump is what keeps this specific problem clean).
- **03** is the hardest common shape: a window valid against a *multiset* requirement (character counts, not just distinct characters), with a `satisfied`/`required` counter to make the O(1) validity check possible.
- **04** is the "shortest window with a numeric threshold" shape, and doubles as the concrete worked example for why non-negative inputs matter (see the README's Common Mistakes: "at most K" vs. numeric sum monotonicity).

Between them, these four cover fixed vs. variable window, sum-based vs. frequency-based aggregates, and longest vs. shortest optimization targets — the full space of Sliding Window problem shapes you will meet in interviews.
