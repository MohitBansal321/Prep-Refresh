# Bit Manipulation — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Bit Manipulation across its main flavors (XOR pair-cancellation, lowest-set-bit removal, dynamic programming over bit counts, and XOR combined with a partitioning trick for two unique values). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-single-number.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Single Number | [136](https://leetcode.com/problems/single-number/) | Easy | XOR-accumulate every element; pairs cancel to 0 (`x ^ x = 0`), leaving the single unpaired value. | O(n) time, O(1) space | [01-single-number.cpp](01-single-number.cpp) |
| Number of 1 Bits | [191](https://leetcode.com/problems/number-of-1-bits/) | Easy | Loop `n &= (n - 1)`; each pass clears the lowest set bit, so the loop runs popcount(n) times. | O(popcount) time, O(1) space | [02-number-of-1-bits.cpp](02-number-of-1-bits.cpp) |
| Counting Bits | [338](https://leetcode.com/problems/counting-bits/) | Easy | DP recurrence `bits[i] = bits[i >> 1] + (i & 1)`: dropping i's lowest bit maps to an already-solved subproblem. | O(n) time, O(n) output space | [03-counting-bits.cpp](03-counting-bits.cpp) |
| Single Number III | [260](https://leetcode.com/problems/single-number-iii/) | Medium | XOR everything to get `a ^ b`, isolate its lowest set bit, and partition elements by that bit so each group's XOR yields one answer. | O(n) time, O(1) space | [04-single-number-iii.cpp](04-single-number-iii.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the textbook XOR cancellation — the pattern in its purest form, and the base case that 04 builds on.
- **02** is the textbook `n & (n-1)` clear-lowest-set-bit loop — also the identity behind power-of-two testing (see [images/trace-diagram.md](../images/trace-diagram.md) for a full bit-level trace of this exact algorithm).
- **03** shows the pattern composed with dynamic programming: when you need bit counts for *every* value in a range, reuse smaller subproblems instead of re-popcounting each from scratch.
- **04** shows XOR's limits being pushed past the plain form — two unknowns at once — solved by deriving a discriminating bit and splitting into two independent single-number problems.

For unguided practice on problems that are *not* worked out step by step — including the case where plain XOR breaks (distractors appearing three times) — see [exercises.md](../exercises.md).
