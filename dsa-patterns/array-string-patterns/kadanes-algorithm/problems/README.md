# Kadane's Algorithm — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Kadane's Algorithm in its vanilla form and its three most common adaptations (product instead of sum, a disguised delta-array reformulation, and circular wraparound). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-maximum-subarray.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Maximum Subarray | [53](https://leetcode.com/problems/maximum-subarray/) | Medium | Vanilla Kadane's: extend the running sum or restart at the current element, tracking the best seen. | O(n) time, O(1) space | [01-maximum-subarray.cpp](01-maximum-subarray.cpp) |
| Maximum Product Subarray | [152](https://leetcode.com/problems/maximum-product-subarray/) | Medium | Track running max **and** min product (a negative number can flip the sign), swapping them when the current element is negative. | O(n) time, O(1) space | [02-maximum-product-subarray.cpp](02-maximum-product-subarray.cpp) |
| Best Time to Buy and Sell Stock | [121](https://leetcode.com/problems/best-time-to-buy-and-sell-stock/) | Easy | Kadane's on day-over-day price deltas, reformulated as "track the running minimum price seen so far, maximize profit if selling today." | O(n) time, O(1) space | [03-best-time-to-buy-and-sell-stock.cpp](03-best-time-to-buy-and-sell-stock.cpp) |
| Maximum Sum Circular Subarray | [918](https://leetcode.com/problems/maximum-sum-circular-subarray/) | Medium | `max(vanilla Kadane's max, total_sum - Kadane's min)`, with an all-negative guard so the wraparound trick never returns an invalid 0. | O(n) time, O(1) space | [04-maximum-sum-circular-subarray.cpp](04-maximum-sum-circular-subarray.cpp) |

## Why these four

They cover every adaptation called out in the [README](../README.md)'s Disadvantages and Interview Discussion sections:

- **01** is the textbook vanilla Kadane's — the pattern in its purest form, and the direct reference implementation for the extend-or-restart core in [code.cpp](../code.cpp).
- **02** shows what breaks when "sum" becomes "product" — a single running maximum is no longer enough, because a negative number can flip a large-magnitude negative running product into the new best positive one. This is the sharpest illustration of "looks like Kadane's, needs real adaptation."
- **03** shows the pattern in disguise — the problem statement never mentions "contiguous subarray sum," but reformulating "profit" as a day-over-day delta reveals the exact same extend-or-restart shape underneath.
- **04** shows the pattern composed with an algebraic trick (total sum minus the minimum subarray) to handle a structural change (circular wraparound) that vanilla Kadane's cannot handle directly, plus the one degenerate edge case (all-negative input) where the trick must be discarded in favor of the vanilla result.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
