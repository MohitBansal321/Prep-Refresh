// ============================================================================
// LeetCode 209 — Minimum Size Subarray Sum
// ============================================================================
// Statement:
//   Given an array of positive integers `nums` and a positive integer
//   `target`, return the length of the shortest contiguous subarray whose sum
//   is >= target. Return 0 if no such subarray exists.
//
// Example:
//   nums = [2,3,1,2,4,3], target = 7 -> answer 2 (the subarray [4,3])
//   nums = [1,4,4],       target = 4 -> answer 1 (the subarray [4])
//   nums = [1,1,1,1,1,1,1,1], target = 11 -> answer 0 (impossible)
//
// Sliding Window shape: VARIABLE-SIZE window, "shortest window satisfying a
// condition" — the condition is "running sum >= target."
//
// Why this works only because all numbers are POSITIVE:
//   Growing the window (moving `right`) can only increase the sum; shrinking
//   it (moving `left`) can only decrease the sum. That monotonic relationship
//   between window size and window sum is exactly what makes the two-pointer
//   sweep correct — once a window is valid, we know shrinking it is the only
//   way to look for something shorter, and we know exactly when to stop
//   shrinking (the moment the sum drops below target). This trick breaks if
//   negative numbers are allowed, because then a bigger window is not
//   guaranteed to have a bigger sum — see Common Mistakes in README.md.
//
// Approach:
//   1. Expand `right`, adding nums[right] to a running sum.
//   2. While the running sum is already >= target, the current window is a
//      valid candidate. Record its length, then shrink from the left
//      (subtracting nums[left] from the running sum) as long as the sum
//      stays >= target, because a shorter window is strictly better.
//   3. Track the minimum length seen across the whole scan.
//
// Complexity:
//   Time:  O(n)   — `right` visits every index once; `left` only moves
//                    forward and visits each index at most once across the
//                    entire run, so total pointer movement is O(n).
//   Space: O(1)   — a running sum and a couple of indices.
//   Contrast: the brute force checks every subarray's sum directly —
//   O(n^2) with a nested loop recomputing sums, or O(n^3) if the sum itself
//   is recomputed from scratch for every (start, end) pair without reusing
//   partial sums. For n = 10^5, O(n^2) is 10^10 operations — computationally
//   infeasible; O(n) is 10^5, trivial.
// ============================================================================

#include <climits>
#include <iostream>
#include <vector>

int minSubArrayLen(int target, const std::vector<int>& nums) {
    int n = static_cast<int>(nums.size());
    long long windowSum = 0;
    int left = 0;
    int best = INT_MAX;

    for (int right = 0; right < n; ++right) {
        windowSum += nums[right];  // grow: bring the new element into the window

        // Shrink while the window already satisfies sum >= target — we want
        // the shortest such window, so keep trying to shrink further.
        while (windowSum >= target) {
            best = std::min(best, right - left + 1);
            windowSum -= nums[left];  // leaving element must update the sum
            ++left;
        }
    }

    return (best == INT_MAX) ? 0 : best;
}

// ----------------------------------------------------------------------------
// Test harness
// ----------------------------------------------------------------------------
static int failures = 0;

static void check(int actual, int expected, const std::string& label) {
    bool pass = (actual == expected);
    std::cout << (pass ? "[PASS] " : "[FAIL] ") << label
              << " -> got " << actual << ", expected " << expected << '\n';
    if (!pass) ++failures;
}

int main() {
    std::cout << "=== Minimum Size Subarray Sum (LC 209) ===\n\n";

    check(minSubArrayLen(7, {2, 3, 1, 2, 4, 3}), 2, "target=7, [2,3,1,2,4,3]");
    check(minSubArrayLen(4, {1, 4, 4}), 1, "target=4, [1,4,4]");
    check(minSubArrayLen(11, {1, 1, 1, 1, 1, 1, 1, 1}), 0, "target=11, all 1s (impossible)");
    check(minSubArrayLen(15, {1, 2, 3, 4, 5}), 5, "target=15, [1,2,3,4,5] (needs whole array)");
    check(minSubArrayLen(100, {1, 2, 3}), 0, "target=100, sum too small (impossible)");
    check(minSubArrayLen(3, {3}), 1, "target=3, single element equals target");

    std::cout << '\n' << (failures == 0 ? "All tests PASSED." : "Some tests FAILED.") << '\n';
    return failures == 0 ? 0 : 1;
}
