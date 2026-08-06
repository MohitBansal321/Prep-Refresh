# Prefix Sum — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Prefix Sum across its main shapes (pure range-sum query, prefix-sum-plus-hash-map for subarray existence, a remapped-alphabet variant of the same trick, and the prefix/suffix-product sibling). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-range-sum-query-immutable.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Range Sum Query - Immutable | [303](https://leetcode.com/problems/range-sum-query-immutable/) | Easy | Build a prefix array once in the constructor; each `sumRange(l, r)` returns `prefix[r+1] - prefix[l]`. | O(n) build, O(1) query, O(n) space | [01-range-sum-query-immutable.cpp](01-range-sum-query-immutable.cpp) |
| Subarray Sum Equals K | [560](https://leetcode.com/problems/subarray-sum-equals-k/) | Medium | Running prefix sum plus a hash map of how many times each prefix-sum value has been seen; count `seen[curr - k]` at every step. | O(n) time, O(n) space | [02-subarray-sum-equals-k.cpp](02-subarray-sum-equals-k.cpp) |
| Contiguous Array | [525](https://leetcode.com/problems/contiguous-array/) | Medium | Remap 0 to -1, take a running prefix sum, and track the FIRST index each running total was seen; the widest repeat gap is the answer. | O(n) time, O(n) space | [03-contiguous-array.cpp](03-contiguous-array.cpp) |
| Product of Array Except Self | [238](https://leetcode.com/problems/product-of-array-except-self/) | Medium | Prefix-product pass left to right, then a suffix-product pass right to left using a single running accumulator (no division). | O(n) time, O(1) extra space (excluding output) | [04-product-of-array-except-self.cpp](04-product-of-array-except-self.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the textbook "build once, query many times" use case — the pattern in its purest form, and the direct ancestor of the generic `PrefixSum` class in [code.cpp](../code.cpp).
- **02** is the prefix-sum-plus-hash-map-of-frequencies technique that turns a subarray-sum-equals-K existence/counting question into a single O(n) pass, instead of an O(n²) brute-force check of every subarray.
- **03** shows the same prefix-sum-plus-hash-map idea applied to a cleverly remapped alphabet (0 -> -1, 1 -> +1), and the subtlety of storing only the *first* occurrence of each running total to guarantee the longest, not just any, qualifying subarray.
- **04** shows the pattern's product sibling: precomputing running aggregates from BOTH directions (prefix and suffix) so that each output only needs the two precomputed values that surround it, without ever dividing.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
