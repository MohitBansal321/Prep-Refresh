# Longest Increasing Subsequence (LIS) — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Dynamic Programming — one sequence, order-relation state. |
| **Recognition Signal** | Longest subsequence (gaps allowed) of ONE sequence obeying an order relation (strictly increasing, or similar) — not comparing two sequences, not requiring contiguity. |
| **Problem** | Brute-force subsequence enumeration is `O(2^n * n)` — exponential. No safe local greedy rule exists at the element level. |
| **Solution** | `O(n^2)` DP: `dp[i]` = LIS length ending at `i`, built from earlier smaller elements; answer = `max(dp)`. `O(n log n)`: maintain `tails[k]` = smallest tail value for length `k+1`; binary search (`lower_bound`) to update or extend; answer = `tails.size()`. |
| **Participants** | The input array (read-only) · `dp` array (Approach 1) OR `tails` array (Approach 2) · the binary search step (Approach 2 only). |
| **Flow** | O(n^2): for each i, scan all j<i with nums[j]<nums[i], take best dp[j]+1 -> answer = max(dp). O(n log n): for each number, binary search tails for first entry >= number; replace it, or append if none found -> answer = tails.size(). |
| **Pros** | Two complexity tiers to choose from · `O(n log n)` is a genuine algorithmic upgrade, not a constant-factor tweak · generalizes to 2D nesting problems via sort-then-LIS. |
| **Cons** | `tails` is NOT a valid subsequence itself -- reconstructing the actual answer needs extra parent-pointer bookkeeping · patience-sorting's correctness is genuinely non-obvious · easy to confuse with the much simpler contiguous "increasing subarray" problem. |
| **Use When** | Longest run (gaps allowed) obeying an order relation within one sequence · as a subroutine after sorting for 2D nesting/ordering problems. |
| **Avoid When** | Need a CONTIGUOUS run -- simple O(n) scan instead · comparing TWO sequences -- that's Longest Common Subsequence. |
| **Real Examples** | Stock trend analysis (longest improving run) · patience-sorting-based logistics/box-stacking algorithms · version/metric monotonicity checks. |
| **Related Topics** | Longest Common Subsequence (two sequences, 2D state) · Kadane's Algorithm (one sequence, contiguous, sum-based) · Russian Doll Envelopes (sort + LIS combo) · patience sorting (the card game the O(n log n) technique is named after). |

### Skeleton
```cpp
// O(n^2)
template <typename T>
int lengthOfLIS_On2(const std::vector<T>& nums) {
  int n = nums.size();
  std::vector<int> dp(n, 1);
  int best = 1;
  for (int i = 1; i < n; ++i)
    for (int j = 0; j < i; ++j)
      if (nums[j] < nums[i]) { dp[i] = std::max(dp[i], dp[j] + 1); best = std::max(best, dp[i]); }
  return best;
}

// O(n log n) -- patience sorting
template <typename T>
int lengthOfLIS_NLogN(const std::vector<T>& nums) {
  std::vector<T> tails;
  for (const T& x : nums) {
    auto it = std::lower_bound(tails.begin(), tails.end(), x);
    if (it == tails.end()) tails.push_back(x);
    else *it = x;
  }
  return tails.size();
}
```

### Remember In One Sentence
> **LIS finds the longest order-preserving-with-gaps run within one sequence — the O(n^2) DP tracks the best length ending at each position, while the sharper O(n log n) patience-sorting trick tracks the smallest tail value achievable for each length instead, and binary searches into it.**

### Two Facts People Get Wrong
- The `tails` array **is** the longest subsequence? **No** — it only tracks the smallest tail value per length; it isn't itself a valid subsequence of the input.
- LIS is about **contiguous** runs? **No** — gaps are explicitly allowed; contiguous "increasing subarray" is a different, much simpler O(n) problem.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. State what `dp[i]` means in the `O(n^2)` approach. Why is the final answer `max(dp)` and not `dp[n-1]`?
2. What does `tails[k]` mean in the `O(n log n)` approach? Is it a real subsequence of the input?
3. Which binary search function (`lower_bound` or `upper_bound`) do you use for strictly increasing, and which for non-decreasing? Why does that single swap matter?
4. Why is there no safe greedy rule for this problem, unlike Merge Intervals' sort-then-sweep?
5. How would you reconstruct the actual LIS (not just its length) from the `O(n log n)` approach?
6. What's the key difference between LIS and Longest Common Subsequence in terms of how many sequences and what state shape each uses?
7. How does Russian Doll Envelopes reduce a 2D nesting problem to a 1D LIS problem? What's the tie-break trick, and why is it needed?
8. Give one concrete example distinguishing "increasing subsequence" from "increasing subarray."
9. What is patience sorting, and why does the number of piles equal the LIS length?
10. When would you prefer the `O(n^2)` DP over the sharper `O(n log n)` approach, even knowing the latter is asymptotically better?
