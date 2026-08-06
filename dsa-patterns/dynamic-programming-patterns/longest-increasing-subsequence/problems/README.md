# Longest Increasing Subsequence — Worked Problems

| # | Name | LeetCode | Difficulty | Approach | Complexity |
|---|------|----------|------------|----------|------------|
| 01 | [Longest Increasing Subsequence](01-longest-increasing-subsequence.cpp) | [300](https://leetcode.com/problems/longest-increasing-subsequence/) | Medium | Patience sorting: `tails` array + binary search | `O(n log n)` time, `O(n)` space |
| 02 | [Number of Longest Increasing Subsequence](02-number-of-longest-increasing-subsequence.cpp) | [673](https://leetcode.com/problems/number-of-longest-increasing-subsequence/) | Medium | `O(n^2)` DP tracking both length AND count per ending index | `O(n^2)` time, `O(n)` space |
| 03 | [Russian Doll Envelopes](03-russian-doll-envelopes.cpp) | [354](https://leetcode.com/problems/russian-doll-envelopes/) | Hard | Sort by width asc / height desc (tie-break trick), then LIS on height | `O(n log n)` time, `O(n)` space |
| 04 | [Maximum Length of Pair Chain](04-maximum-length-of-pair-chain.cpp) | [646](https://leetcode.com/problems/maximum-length-of-pair-chain/) | Medium | Sort by right endpoint, then a simple greedy chain-count (LIS-adjacent) | `O(n log n)` time, `O(1)` extra space |

**Why these four:** 01 is the pure length recurrence in its sharpest (`O(n log n)`) form. 02 shows the `O(n^2)` DP extended to count, not just measure, the longest subsequences — a natural "what if" question once you understand the base DP. 03 is the classic 2D-to-1D reduction (sort away one dimension, LIS the other) and is notorious for a subtle sort-tie-break bug if you don't sort height descending within equal widths. 04 shows a close relative solved more simply by a greedy sort, useful for contrasting "this looks like LIS but doesn't need the full machinery."
