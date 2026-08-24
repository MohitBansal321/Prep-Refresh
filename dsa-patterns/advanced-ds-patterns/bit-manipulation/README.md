# Bit Manipulation

> **5-min refresher instead?** [cheatsheet.md](cheatsheet.md) has the one-table summary and recall questions.

## Intent

Use XOR/AND/OR/shift tricks to solve problems (single unpaired number, subset/mask generation, counting set bits) in `O(1)` extra space, without any auxiliary data structure.

## Recognition Signal

The problem hints at binary representation, a single unpaired element among duplicates, or generating all subsets/masks of a small set.

## Core Idea

A handful of bitwise identities do most of the work in this pattern:

- **XOR cancels pairs.** `x ^ x = 0` and `x ^ 0 = x`, and XOR is commutative/associative — so XOR-ing an entire array where every value appears twice except one leaves exactly that one unpaired value.
- **`n & (n-1)` clears the lowest set bit.** Repeating this until `n` becomes 0 counts the set bits in `O(popcount)` time instead of checking all 32/64 bits.
- **`n & (n-1) == 0` tests "is this a power of two."** A power of two has exactly one set bit; clearing the lowest set bit of a power of two always yields 0.
- **Iterating `0` to `2^n - 1` and reading each number's bits enumerates every subset mask** of an n-element set — bit `i` set means "element `i` is included." This is the same `2^n` enumeration as the Subsets pattern, expressed via integers instead of explicit recursion.

## Template

```cpp
// XOR cancels pairs: find the single number where every other appears twice.
int singleNumber(const std::vector<int>& nums) {
  int result = 0;
  for (int n : nums) result ^= n;
  return result;
}

// Count set bits using n & (n-1) to clear the lowest set bit each iteration.
int countSetBits(unsigned int n) {
  int count = 0;
  while (n) { n &= (n - 1); ++count; }
  return count;
}

// Enumerate all subset masks of an n-element set.
std::vector<std::vector<int>> allSubsetsViaBitmask(const std::vector<int>& nums) {
  int n = nums.size();
  std::vector<std::vector<int>> result;
  for (int mask = 0; mask < (1 << n); ++mask) {
    std::vector<int> subset;
    for (int i = 0; i < n; ++i)
      if (mask & (1 << i)) subset.push_back(nums[i]);
    result.push_back(subset);
  }
  return result;
}
```

## Complexity

**Time:** `O(n)` for XOR-based single-number problems; `O(popcount)` for bit-counting via `n & (n-1)`; `O(2^n * n)` for full subset-mask enumeration.
**Space:** `O(1)` extra for the XOR and bit-counting tricks — the entire point of this pattern.

## Common Mistakes

- **Forgetting XOR only works cleanly when every OTHER value appears an EVEN number of times.** If duplicates can appear 3+ times, plain XOR doesn't isolate the answer — a different bit-counting-per-position technique is needed instead.
- **Sign-extension surprises with signed integers and shifts** — `1 << 31` on a signed 32-bit int is undefined behavior in C++; use unsigned types or `1u << 31` when working near the top bit.
- **Off-by-one when converting between a bitmask and a 0-indexed element list** — bit `i` corresponds to `nums[i]`, not `nums[i-1]` or `nums[i+1]`.

## When To Use

- Finding a single unpaired/unique element among duplicates.
- Counting set bits, checking power-of-two, or other binary-representation-specific properties.
- Enumerating all subsets/masks of a SMALL set (`n` up to ~20), as an integer-based alternative to recursive Subsets.

## When NOT To Use

- **Duplicates can appear an odd number of times other than exactly once for the answer** — plain XOR cancellation breaks down; needs a different per-bit-position counting technique.
- **The set is too large for `2^n` mask enumeration** (`n` beyond ~20-25) — same exponential wall as the Subsets pattern.

## Similar Patterns

- **Subsets** ([../../recursion-backtracking-patterns/subsets/](../../recursion-backtracking-patterns/subsets/)): bitmask enumeration is a non-recursive way to generate the exact same `2^n` subsets — same complexity, different mechanism (integer bits vs. recursive include/exclude).
- **Cyclic Sort** ([../../array-string-patterns/cyclic-sort/](../../array-string-patterns/cyclic-sort/)): a different `O(1)`-space technique for a similar "find the odd one out" family of problems, when values map to a bounded index range instead of needing XOR cancellation.

## Further Reading

- LeetCode — Single Number (136), Number of 1 Bits (191), Counting Bits (338), Power of Two (231).
- *Hacker's Delight* — Henry S. Warren Jr. — the definitive reference for bit-manipulation tricks.
