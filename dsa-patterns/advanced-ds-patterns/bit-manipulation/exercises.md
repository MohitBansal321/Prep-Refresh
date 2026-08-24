# Bit Manipulation — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "this answer lives in the bits" signal — an unpaired element among even-count duplicates, a power-of-two/set-bit property, or a small-set mask enumeration — and (2) checking, *before writing code*, that the bitwise identity you plan to use actually matches the problem's multiplicity/structure constraints.

> Rule of thumb for every exercise: before writing a single line, ask "which identity am I relying on — XOR cancellation, lowest-set-bit removal, or mask iteration?" and "does the input's structure satisfy that identity's preconditions exactly?" If the distractors appear an odd number of times, or `n` for masks exceeds ~20, plain XOR / mask enumeration is the wrong tool and you should say so out loud before coding anything.

---

## Easy — Power of Two

**LeetCode 231 — Power of Two.**

Given an integer `n`, return `true` if it is a power of two. Otherwise, return `false`. An integer is a power of two if there exists an integer `x` such that `n == 2^x`.

**Constraints to notice:** `-2^31 <= n <= 2^31 - 1` — the input can be **negative and zero**, and neither can ever be a power of two. The classic one-liner `(n & (n-1)) == 0` returns `true` for `n = 0`, which would be wrong.

**Task:** solve it in O(1) with the bit trick, and write down explicitly why your guard clause handles every non-power case (negative numbers, zero, and positive non-powers).

**Think about:** what is it about the binary representation of a power of two that makes `n & (n-1)` collapse to zero? What survives in `n & (n-1)` when `n` has two or more set bits?

---

## Medium — Single Number II

**LeetCode 137 — Single Number II.**

Given an integer array where every element appears **three times** except one, which appears **exactly once**, find the single one. Follow-up requirement: linear runtime and O(1) extra space.

**Constraints to notice:** this is the exact case where plain XOR **fails** — each triplicated element XORs to itself once (`x ^ x ^ x = x`), so the accumulated result is polluted by leftovers of every distractor. The array can contain negative numbers, so any per-bit counting must handle all 32 bit positions including the sign bit.

**Task:** use per-bit-position counting: for each of the 32 bit positions, sum how many elements have that bit set; each sum modulo 3 reconstructs the corresponding bit of the unique value. Then reason about whether the modulo-3 trick still works if the unique element appeared, say, five times while others appeared three times.

**Think about:** why does taking each bit-position count `% 3` recover the answer's bits rather than garbage? What invariant does the count-per-position maintain about the distractors' contribution?

---

## Hard — Maximum XOR of Two Numbers in an Array

**LeetCode 421 — Maximum XOR of Two Numbers in an Array.**

Given an integer array `nums`, return the maximum result of `nums[i] XOR nums[j]`, where `0 <= i <= j < n`.

**Constraints to notice:** brute force over all pairs is O(n²) — too slow at n = 2·10^5. The key structural fact is that XOR compares **bit by bit independently**, and high bits dominate the value, so a greedy bit-by-bit construction from the top bit down is possible.

**Task:** build the answer greedily from bit 31 down to bit 0: tentatively assume each next bit of the answer is 1, then check whether some pair of prefixes realizes that assumption using a hash set of prefixes (`candidate = prefix | (1 << b)` has a partner iff `prefix ^ candidate` was seen). Achieve O(n · 32) time.

**Think about:** why does checking prefix pairs rather than full values make the greedy step valid? At each bit, what exactly does "the assumed prefix has a partner" prove about the final maximum?

---

## Real-World Challenge — Feature-Flag Audit with Parity Detection

You maintain a service whose configuration is a set of boolean feature flags packed into a single 32-bit integer bitmask (one bit per flag), shipped to clients as part of every response. Two teams independently patch flag defaults, and occasionally their edits cancel out incorrectly: a flag that should be enabled ends up toggled twice (off) or not at all, and you need cheap tooling to catch this without shipping a config database alongside the binary.

**Task:**
1. Implement (a) enable/disable/toggle/query helpers for a named flag using only `|`, `&`, `^`, and shifts; (b) a function that counts how many flags are currently enabled using the `n & (n-1)` loop; (c) a function that lists enabled flag names by iterating the mask.
2. Design a parity check: compute an 8-bit checksum of a known-good baseline mask via repeated `n & (n-1)` popcount, and detect whether a deployed mask differs from the baseline in an **odd** number of positions using a single XOR plus popcount — no per-flag comparison loop needed.
3. Discuss: a colleague proposes packing flags into a `bool flags[32]` array instead because "it is easier to read." Compare both representations on memory footprint, copy cost when passing configs around, and debuggability. Under what circumstances is the readability cost of the bitmask genuinely worth paying, and when is it not?

---

## Bonus Challenge — Bitwise AND of Numbers Range

**LeetCode 201 — Bitwise AND of Numbers Range.**

Given two integers `left` and `right` representing the range `[left, right]`, return the bitwise AND of all numbers in this range, inclusive. Constraints allow `right - left` up to ~2^31, so looping AND-ing every number will not finish.

**Task:** solve it in O(log(max)) time. Observe that AND-ing a contiguous range zeroes out every bit position where the range *spans* a boundary at that bit — so repeatedly right-shift both `left` and `right` until they are equal (recording how many shifts happened), then shift the common prefix back left.

**Then answer:** why does the final result always equal the common binary *prefix* of `left` and `right`, followed by zeros? Give one concrete range (in binary) where `left != right` but the answer equals `left & right` anyway, and explain why the prefix argument covers that case too.

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
