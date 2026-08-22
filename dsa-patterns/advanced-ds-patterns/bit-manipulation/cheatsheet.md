# Bit Manipulation — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Advanced DS pattern — integer-bit-level technique. |
| **Recognition Signal** | The problem hints at **binary representation** (set bits, powers of two), a **single unpaired element among duplicates that appear an even number of times**, a request for **O(1) extra space**, or **enumerating all subsets/masks of a small set** (`n` up to ~20). |
| **Problem** | Brute force reaches for a hash set/count map (O(n) extra space) to track what has been seen, checks all 32/64 bits one at a time for bit-counting, or recurses include/exclude over `2^n` subsets with call-stack overhead. |
| **Solution** | A handful of bitwise identities do the work: **XOR cancels pairs** (`x ^ x = 0`, `x ^ 0 = x`, commutative/associative); **`n & (n-1)` clears the lowest set bit** (loop to count set bits in O(popcount); test `== 0` for power-of-two); **iterate `mask` from `0` to `2^n - 1` and read its bits** to enumerate every subset non-recursively. |
| **Time / Space Complexity** | O(n) time for XOR-based single-number problems · O(popcount) for `n & (n-1)` bit counting · O(2^n · n) for full subset-mask enumeration. O(1) extra space for XOR and bit-counting tricks — the entire point of the pattern; subset enumeration needs O(n) working space per mask (output excluded). |
| **Pros** | O(1) extra space for cancellation/counting tricks · fewer moving parts than a hash structure (nothing to size, resize, or collide) · `n & (n-1)` loop runs proportional to *set* bits, not word width · bitmask enumeration replaces recursion with a plain integer loop · constant-time set membership per bit. |
| **Cons** | Only applies when the algebra actually works — plain XOR breaks the moment any distractor appears an odd number of times · unreadable to reviewers unfamiliar with the identities (comment generously) · signed-shift and sign-extension pitfalls near the top bit (`1 << 31` on signed `int` is UB) · `2^n` mask enumeration hits the same exponential wall as Subsets past `n ≈ 20-25`. |
| **Use When** | Exactly one element is unpaired and everything else appears an even number of times · counting set bits or testing power-of-two / lowest-set-bit properties · enumerating all subsets of a small set as a non-recursive alternative to Subsets · any problem explicitly graded on O(1) space. |
| **Avoid When** | Duplicates can appear an odd number of times other than exactly once for the answer (needs per-bit-position counting instead) · you need key→value lookup semantics, not cancellation (use hashing) · the set exceeds ~20-25 elements so `2^n` masks are infeasible (use DP/backtracking formulations) · the input is floats/strings — the identities only make sense on integers. |
| **Related Patterns** | Subsets ([../../recursion-backtracking-patterns/subsets/](../../recursion-backtracking-patterns/subsets/)) — bitmask enumeration generates the identical `2^n` subsets non-recursively · Cyclic Sort ([../../array-string-patterns/cyclic-sort/](../../array-string-patterns/cyclic-sort/)) — a different O(1)-space trick for the same "find the odd one out" family when values map to a bounded index range · Dynamic Programming — Counting Bits' recurrence `bits[i] = bits[i >> 1] + (i & 1)` is pure DP. |

### Template Skeleton

```cpp
// XOR cancels pairs: the single value appearing an odd number of times.
int singleNumber(const std::vector<int>& nums) {
  int result = 0;
  for (int n : nums) result ^= n;
  return result;
}

// Count set bits: each pass clears the LOWEST set bit, so the loop runs
// popcount(n) times, not 32 times.
int countSetBits(unsigned int n) {
  int count = 0;
  while (n) { n &= (n - 1); ++count; }
  return count;
}

// Power-of-two test: exactly one set bit, and never zero itself.
bool isPowerOfTwo(unsigned int n) {
  return n != 0 && (n & (n - 1)) == 0;
}

// Enumerate all subset masks of an n-element set (n <= ~20).
for (int mask = 0; mask < (1 << n); ++mask) {
  for (int i = 0; i < n; ++i) {
    if (mask & (1 << i)) { /* element i is in this subset */ }
  }
}
```

### Remember In One Sentence
> **Bit Manipulation exploits a few integer identities — XOR's pair-cancellation, `n & (n-1)`'s lowest-set-bit removal, and integers-as-subset-masks — to replace auxiliary data structures with arithmetic, achieving O(1) extra space wherever the problem's structure mirrors the algebra of bits.**

### Two Facts People Get Wrong
- "XOR solves any 'find the unique element' problem"? **No** — XOR only isolates a value that appears an **odd** number of times while *every other* value appears an **even** number of times. If distractors can appear three times, XOR accumulates their leftover copies and the result is garbage; you need per-bit-position counting (LeetCode 137-style) instead.
- "`n & (n-1) == 0` proves `n` is a power of two"? **Not quite** — it is also true for `n = 0`, which is not a power of two; the correct test is `n != 0 && (n & (n-1)) == 0`. Forgetting the guard is one of the most common off-by-zero bugs in this pattern.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the three recognition signals that mean "reach for Bit Manipulation."
2. Why does XOR-ing an entire array isolate the single unpaired value? Name the three algebraic properties you are relying on.
3. What does `n & (n-1)` do to `n` in one sentence, and why does that make bit-counting run in O(popcount) rather than O(32)?
4. Why must the power-of-two test include an explicit `n != 0` check?
5. In subset-mask enumeration, what does bit `i` being set in `mask` mean, and which element index does it correspond to?
6. Give the exact condition under which plain XOR fails for "find the unique element," and name the technique you would fall back to.
7. Why is `1 << 31` on a signed 32-bit `int` undefined behavior in C++, and what are the two standard fixes?
8. How does the `bits[i] = bits[i >> 1] + (i & 1)` recurrence work for Counting Bits, and why is it faster than calling popcount independently on each `i`?
9. In Single Number III (two unique values), how do you split the array into two groups so each group's XOR yields exactly one of the answers?
10. What structural property does bitmask enumeration share with recursive Subsets, and what complexity do both hit when `n` grows past ~20-25?

(End of file - total 61 lines)
