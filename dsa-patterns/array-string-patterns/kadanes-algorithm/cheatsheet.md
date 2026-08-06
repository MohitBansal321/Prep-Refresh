# Kadane's Algorithm — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Array pattern — running-state single pass; a gateway into 1D dynamic programming. |
| **Recognition Signal** | Problem asks for the **maximum (or minimum) sum of a contiguous subarray**, with **no fixed/bounded window size** given. |
| **Problem** | Brute-force nested loops over all contiguous subarrays cost O(n²) (running inner sum) or O(n³) (re-summing from scratch each time). |
| **Solution** | Track `current_sum` (best sum ending exactly here) and `best_sum` (best sum anywhere so far). At each index, either **extend** the previous run (`current_sum + nums[i]`) or **restart** at `nums[i]` alone — take whichever is larger. `best_sum` updates every step and never resets. |
| **Time / Space Complexity** | O(n) time (single pass, O(1) work per element), O(1) extra space — no auxiliary array, no prefix-sum table. |
| **Pros** | O(n) time in O(1) space · single forward pass, streaming-friendly · provably correct (inductive, not heuristic) · natural gateway into 1D DP thinking. |
| **Cons** | Answers exactly one question (max/min contiguous sum) · does not count/enumerate all optimal subarrays without extra bookkeeping · product, circular, and 2D-matrix variants need real adaptation, not verbatim reuse. |
| **Use When** | Best contiguous sum, no window-size constraint · a thin variant (product, circular, buy/sell-delta) of the same shape · you spot the recurrence `dp[i] = max(nums[i], dp[i-1] + nums[i])` inside a larger DP problem. |
| **Avoid When** | The problem allows **non-contiguous** subsequences (that's DP territory, e.g. Longest Increasing Subsequence) · a fixed/bounded window size is given (that's Sliding Window) · you need to count or enumerate every optimal subarray, not just find one. |
| **Related Patterns** | Sliding Window (explicit window-size/property constraint, not just "best contiguous sum") · DP on Grids (2D generalization: `dp[i][j]` from neighboring cells, the same "define state from a smaller subproblem" idea in two dimensions). |

### Template Skeleton

```cpp
// Vanilla Kadane's core (sum-based, no fixed window size)
long long current_sum = nums[0];   // NEVER initialize from 0 -- breaks all-negative arrays
long long best_sum = nums[0];

for (size_t i = 1; i < nums.size(); ++i) {
    // Extend the previous run, or abandon it and restart at nums[i] alone.
    current_sum = std::max(static_cast<long long>(nums[i]), current_sum + nums[i]);

    // best_sum is checked on EVERY iteration, not just at a restart.
    best_sum = std::max(best_sum, current_sum);
}
// best_sum now holds the maximum-sum contiguous subarray's total.
```

### Remember In One Sentence
> **Kadane's Algorithm replaces an O(n²)/O(n³) all-subarrays scan with a single O(n) pass by asking, at every index, "is it better to extend the current run or abandon it and restart here?" — and separately recording the best answer seen anywhere, which never resets.**

### Two Facts People Get Wrong
- "Reset to 0 when the running sum goes negative" is a **literal instruction**? **No** — the correct operation is "restart at the current element," which only coincides with "reset to 0, then add the element" as a two-step mental shortcut; writing code that sets the variable to `0` and moves on without incorporating the current element skips a value and produces wrong sums.
- An all-negative array should return **0** (as if choosing no subarray were valid)? **No** — the problem requires a non-empty subarray; the correct answer is the single least-negative element, which only falls out correctly if both running trackers initialize from `nums[0]`, never from `0`.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the recognition signal that means "reach for Kadane's Algorithm," and contrast it with the signal that means "this is actually Sliding Window instead."
2. What are the two candidates compared at every index to compute `current_sum`, and why is "start the new run somewhere further back" never a third candidate you need to consider?
3. Why must `best_sum` and `current_sum` both initialize from `nums[0]` rather than from `0`? Give a concrete array where initializing from `0` produces a wrong answer.
4. Explain precisely why "reset the running sum to 0" is a dangerous shorthand if taken literally as code, rather than as "restart at the current element."
5. Does `best_sum` ever decrease during the scan? Why or why not?
6. Name the brute-force complexity Kadane's replaces, both for the "re-sum from scratch" version and the "running sum per start index" version.
7. What extra running value must Maximum Product Subarray track, beyond a running maximum, and why does the sum-only version's argument break for products?
8. State the "total sum minus minimum subarray sum" trick used for Maximum Sum Circular Subarray, and the one edge case where it fails.
9. Give one real production context (not a LeetCode problem) where finding the best contiguous run in a time series is directly useful.
10. What is the key structural difference between Kadane's Algorithm and Sliding Window, given that both scan a contiguous range with running state?
