// ============================================================================
// Sliding Window — Generic Reusable Templates (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// two shapes the Sliding Window pattern always takes:
//
//   1. FIXED-SIZE WINDOW   — the window width k is given up front and never
//                             changes. We slide it one step at a time.
//
//   2. VARIABLE-SIZE WINDOW — the window grows by moving `right` until some
//                              condition is satisfied/violated, then shrinks
//                              by moving `left` until it is valid again. The
//                              window width is a *side effect* of the data,
//                              not an input.
//
// Both templates keep a running aggregate (a sum, or a frequency map) instead
// of re-scanning the window from scratch on every step. That single idea —
// "update the aggregate incrementally as the window moves" — is the entire
// pattern. Everything else in this file is that idea applied to different
// aggregates and different stopping conditions.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_sw_code && /tmp/out_sw_code
// ============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// ----------------------------------------------------------------------------
// Template 1: FIXED-SIZE WINDOW
// ----------------------------------------------------------------------------
// Signature shape: given an array and a fixed width k, compute some aggregate
// (here: maximum sum) over every contiguous window of width k.
//
// Mechanism:
//   - Build the running sum for the first window [0, k-1] once: O(k).
//   - Slide one step at a time: add the entering element (arr[right]),
//     subtract the leaving element (arr[right - k]). O(1) per slide.
//   - Total: O(k) + O(n-k) * O(1) = O(n), instead of recomputing every
//     window's sum from scratch, which would be O(n * k).
//
// This shape generalizes to: max/min sum of size k, average of every window
// of size k, first negative number in every window of size k, etc. Only the
// "what does the aggregate track" part changes.
long long maxSumSubarrayFixedWindow(const std::vector<int>& arr, int k) {
    int n = static_cast<int>(arr.size());
    if (n == 0 || k <= 0 || k > n) {
        return 0;  // invalid input for this template; caller should validate
    }

    long long windowSum = 0;
    for (int i = 0; i < k; ++i) {
        windowSum += arr[i];
    }

    long long best = windowSum;

    // right is the index of the element entering the window.
    // The window is always [right - k + 1, right].
    for (int right = k; right < n; ++right) {
        windowSum += arr[right];           // element entering the window
        windowSum -= arr[right - k];       // element leaving the window
        best = std::max(best, windowSum);
    }

    return best;
}

// ----------------------------------------------------------------------------
// Template 2: VARIABLE-SIZE WINDOW — "shortest window that satisfies a
// condition" (grow until valid, then shrink while still valid, tracking the
// best/shortest length seen).
// ----------------------------------------------------------------------------
// Concrete instance: smallest contiguous subarray of non-negative ints whose
// sum is >= target. (LeetCode 209 is the well-known version of this shape;
// see problems/04-minimum-size-subarray-sum.cpp for the fully worked file.)
//
// Mechanism:
//   - right expands the window one step at a time, adding arr[right] to a
//     running sum (the aggregate).
//   - Whenever the window's aggregate satisfies the condition (sum >= target),
//     we have a *candidate* answer. We then shrink from the left as long as
//     the condition still holds, because shrinking can only produce an equal
//     or shorter window — and shorter is what we want.
//   - Every element is added to the running sum exactly once (by right) and
//     removed exactly once (by left), so the total work across the whole
//     scan is O(n), not O(n) per window.
int shortestSubarrayWithSumAtLeast(const std::vector<int>& arr, int target) {
    int n = static_cast<int>(arr.size());
    long long windowSum = 0;
    int left = 0;
    int best = INT_MAX;

    for (int right = 0; right < n; ++right) {
        windowSum += arr[right];  // grow: bring the new element into the window

        // Shrink while the window already satisfies the condition — we are
        // looking for the *shortest* valid window, so keep trying to shrink.
        while (windowSum >= target) {
            best = std::min(best, right - left + 1);
            windowSum -= arr[left];  // leaving element must update the aggregate
            ++left;
        }
    }

    return (best == INT_MAX) ? 0 : best;  // 0 conventionally means "no such window"
}

// ----------------------------------------------------------------------------
// Template 3: VARIABLE-SIZE WINDOW — "longest window that satisfies a
// condition" (grow until invalid, then shrink until valid again, tracking the
// best/longest length seen).
// ----------------------------------------------------------------------------
// Concrete instance: longest substring with at most k distinct characters.
// This is the same control-flow skeleton as Template 2, but the aggregate is
// a frequency map instead of a running sum, and we track the longest valid
// window instead of the shortest.
//
// Mechanism:
//   - right expands the window, incrementing freq[s[right]].
//   - While the window VIOLATES the constraint (freq.size() > k), shrink from
//     the left: decrement freq[s[left]], and erase the key entirely once its
//     count hits zero (this is the step people most often forget — see
//     Common Mistakes in README.md).
//   - After the shrink loop, the window is guaranteed valid again, so record
//     its length as a candidate for the best answer.
int longestSubstringWithAtMostKDistinct(const std::string& s, int k) {
    if (k <= 0) return 0;

    std::unordered_map<char, int> freq;
    int left = 0;
    int best = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        freq[s[right]]++;  // grow: bring the new character into the window

        // Shrink while the window VIOLATES the constraint (too many distinct
        // characters). Note the `while`, not `if` — a single left-shift is
        // not always enough to restore validity.
        while (static_cast<int>(freq.size()) > k) {
            char leaving = s[left];
            freq[leaving]--;
            if (freq[leaving] == 0) {
                freq.erase(leaving);  // must erase, not just leave a 0 entry,
                                      // otherwise freq.size() overcounts
                                      // distinct characters
            }
            ++left;
        }

        best = std::max(best, right - left + 1);
    }

    return best;
}

// ----------------------------------------------------------------------------
// Small self-checks so this file proves itself end-to-end when run.
// ----------------------------------------------------------------------------
static void expectEqual(long long actual, long long expected, const std::string& label) {
    std::cout << label << ": " << actual
              << (actual == expected ? "  [PASS]" : "  [FAIL, expected " + std::to_string(expected) + "]")
              << '\n';
}

int main() {
    std::cout << "=== Sliding Window generic templates ===\n\n";

    // --- Template 1: fixed-size window ---
    {
        std::vector<int> arr{2, 1, 5, 1, 3, 2};
        int k = 3;
        long long result = maxSumSubarrayFixedWindow(arr, k);
        // Windows of size 3: (2,1,5)=8 (1,5,1)=7 (5,1,3)=9 (1,3,2)=6 -> max 9
        expectEqual(result, 9, "maxSumSubarrayFixedWindow([2,1,5,1,3,2], k=3)");
    }
    {
        std::vector<int> arr{2, 3, 4, 1, 5};
        int k = 2;
        long long result = maxSumSubarrayFixedWindow(arr, k);
        // Windows of size 2: 5,7,5,6 -> max 7
        expectEqual(result, 7, "maxSumSubarrayFixedWindow([2,3,4,1,5], k=2)");
    }

    std::cout << '\n';

    // --- Template 2: variable-size window, shortest valid window ---
    {
        std::vector<int> arr{2, 3, 1, 2, 4, 3};
        int target = 7;
        int result = shortestSubarrayWithSumAtLeast(arr, target);
        // [4,3] sums to 7, length 2 is the shortest
        expectEqual(result, 2, "shortestSubarrayWithSumAtLeast([2,3,1,2,4,3], target=7)");
    }
    {
        std::vector<int> arr{1, 4, 4};
        int target = 4;
        int result = shortestSubarrayWithSumAtLeast(arr, target);
        expectEqual(result, 1, "shortestSubarrayWithSumAtLeast([1,4,4], target=4)");
    }
    {
        std::vector<int> arr{1, 1, 1, 1};
        int target = 11;
        int result = shortestSubarrayWithSumAtLeast(arr, target);
        expectEqual(result, 0, "shortestSubarrayWithSumAtLeast([1,1,1,1], target=11) - no valid window");
    }

    std::cout << '\n';

    // --- Template 3: variable-size window, longest valid window ---
    {
        std::string s = "eceba";
        int k = 2;
        int result = longestSubstringWithAtMostKDistinct(s, k);
        // "ece" has 2 distinct chars, length 3
        expectEqual(result, 3, "longestSubstringWithAtMostKDistinct(\"eceba\", k=2)");
    }
    {
        std::string s = "aa";
        int k = 1;
        int result = longestSubstringWithAtMostKDistinct(s, k);
        expectEqual(result, 2, "longestSubstringWithAtMostKDistinct(\"aa\", k=1)");
    }
    {
        std::string s = "abcabcbb";
        int k = 1;
        int result = longestSubstringWithAtMostKDistinct(s, k);
        // With only 1 distinct char allowed, the longest run of one repeated
        // character is "bb" (the last two characters), length 2.
        expectEqual(result, 2, "longestSubstringWithAtMostKDistinct(\"abcabcbb\", k=1)");
    }

    std::cout << "\nAll templates exercised. See problems/ for four fully worked,\n"
                 "standalone LeetCode-style solutions built on these same ideas.\n";

    return 0;
}
