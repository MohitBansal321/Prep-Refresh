// ============================================================================
// Problem: Maximum Sum Subarray of Size K
// ============================================================================
// Source: This is not an official LeetCode-numbered problem — it is the
// canonical introductory Sliding Window problem used by Grokking the Coding
// Interview and widely mirrored on GeeksforGeeks as "Maximum Sum Subarray of
// Size K". It is the simplest possible instance of the pattern and the usual
// first problem taught, which is why it opens this problems/ folder.
//
// Statement:
//   Given an array of positive integers `arr` and a positive integer `k`,
//   find the maximum sum of any contiguous subarray of size exactly `k`.
//
// Example:
//   arr = [2, 1, 5, 1, 3, 2], k = 3
//   Windows of size 3: [2,1,5]=8, [1,5,1]=7, [5,1,3]=9, [1,3,2]=6
//   Answer: 9
//
// Sliding Window shape: FIXED-SIZE window (k is given and constant).
//
// Approach:
//   1. Compute the sum of the first window [0, k-1] directly: O(k).
//   2. Slide the window one position at a time. Each slide adds exactly one
//      new element (entering on the right) and removes exactly one old
//      element (leaving on the left) — both O(1) updates to a running sum.
//   3. Track the maximum sum seen across all windows.
//
//   The key realization: two adjacent windows of size k share k-1 elements.
//   Recomputing the sum of every window from scratch (brute force) redoes
//   that shared work every single time. The sliding window reuses it.
//
// Complexity:
//   Time:  O(n)   — every element is added to the running sum once and
//                    removed once, n total slides.
//   Space: O(1)   — only a running sum and a few indices/variables.
//   Contrast: brute force recomputing each window's sum from scratch is
//   O(n * k) time, O(1) space. For n = 10^5 and k = 10^3, that is 10^8
//   operations against 10^5 — a ~1000x difference in real terms.
// ============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>

int maxSumSubarrayOfSizeK(const std::vector<int>& arr, int k) {
    int n = static_cast<int>(arr.size());
    if (k <= 0 || k > n) {
        // Not a valid window size for this array; caller error.
        return INT_MIN;
    }

    // Step 1: sum of the first window [0, k-1].
    long long windowSum = 0;
    for (int i = 0; i < k; ++i) {
        windowSum += arr[i];
    }

    long long maxSum = windowSum;

    // Step 2: slide the window across the rest of the array.
    // `right` is the index of the element entering the window; the element
    // leaving is always k positions behind it, i.e. arr[right - k].
    for (int right = k; right < n; ++right) {
        windowSum += arr[right];       // add the incoming element
        windowSum -= arr[right - k];   // remove the outgoing element
        maxSum = std::max(maxSum, windowSum);
    }

    return static_cast<int>(maxSum);
}

// ----------------------------------------------------------------------------
// Brute-force reference implementation (for comparison / sanity checking).
// Recomputes each window's sum from scratch: O(n * k) time.
// ----------------------------------------------------------------------------
int maxSumSubarrayBruteForce(const std::vector<int>& arr, int k) {
    int n = static_cast<int>(arr.size());
    if (k <= 0 || k > n) return INT_MIN;

    long long maxSum = LLONG_MIN;
    for (int start = 0; start + k <= n; ++start) {
        long long sum = 0;
        for (int i = start; i < start + k; ++i) {
            sum += arr[i];
        }
        maxSum = std::max(maxSum, sum);
    }
    return static_cast<int>(maxSum);
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
    std::cout << "=== Maximum Sum Subarray of Size K ===\n\n";

    {
        std::vector<int> arr{2, 1, 5, 1, 3, 2};
        check(maxSumSubarrayOfSizeK(arr, 3), 9, "sliding window, k=3");
        check(maxSumSubarrayBruteForce(arr, 3), 9, "brute force,    k=3");
    }
    {
        std::vector<int> arr{2, 3, 4, 1, 5};
        check(maxSumSubarrayOfSizeK(arr, 2), 7, "sliding window, k=2");
        check(maxSumSubarrayBruteForce(arr, 2), 7, "brute force,    k=2");
    }
    {
        // k equals array length: only one window, its sum is the answer.
        std::vector<int> arr{4, 2, 1, 7};
        check(maxSumSubarrayOfSizeK(arr, 4), 14, "k == n (single window)");
    }
    {
        // k == 1: the answer is just the maximum single element.
        std::vector<int> arr{5, 1, 9, 3};
        check(maxSumSubarrayOfSizeK(arr, 1), 9, "k == 1 (max element)");
    }

    std::cout << '\n' << (failures == 0 ? "All tests PASSED." : "Some tests FAILED.") << '\n';
    return failures == 0 ? 0 : 1;
}
