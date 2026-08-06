// ============================================================================
// Longest Increasing Subsequence (LIS) — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates
// TWO different algorithms for the same question — "what is the length of
// the longest strictly increasing subsequence of this array?" — so you can
// see both the intuitive DP shape and the sharper, non-obvious optimization
// side by side, and convince yourself they agree on every input.
//
//   1. lengthOfLIS_On2   — O(n^2) DP. dp[i] = length of the longest
//                          increasing subsequence ENDING at index i, built
//                          by looking back at every earlier smaller element.
//                          This is the "obvious once you see it" DP.
//
//   2. lengthOfLIS_NLogN — O(n log n) "patience sorting". Maintains a
//                          `tails` array (the smallest possible tail value
//                          for each achievable subsequence length) and uses
//                          binary search (std::lower_bound) to place each
//                          new element in O(log n). This is the classical,
//                          much less obvious trick — see the README for the
//                          full correctness argument.
//
// Both are generic function templates so they work over vector<int>,
// vector<long long>, vector<double>, etc. — the algorithm only needs the
// element type to be totally ordered (operator< defined).
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_lis_code && /tmp/out_lis_code
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Variant 1: O(n^2) DP.
//
// dp[i] means: the length of the longest strictly increasing subsequence
// that ENDS exactly at index i (i.e. nums[i] is definitely the last element
// chosen). Every element is itself a valid subsequence of length 1, so
// dp[i] starts at 1. To extend a subsequence so it ends at i, we need some
// earlier index j < i with nums[j] < nums[i] — then dp[i] can become
// dp[j] + 1. We try every earlier j and take the best one.
//
// The final answer is NOT dp[n-1] — it is max(dp[0..n-1]), because the
// longest subsequence overall might end anywhere, not necessarily at the
// last element.
// ----------------------------------------------------------------------------
template <typename T>
int lengthOfLIS_On2(const std::vector<T>& nums) {
  const size_t n = nums.size();
  if (n == 0) return 0;

  std::vector<int> dp(n, 1);  // Every single element is a subsequence of length 1.
  int best = 1;

  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (nums[j] < nums[i]) {
        // We could place nums[i] right after whatever subsequence ends at j.
        dp[i] = std::max(dp[i], dp[j] + 1);
      }
    }
    best = std::max(best, dp[i]);
  }

  return best;
}

// ----------------------------------------------------------------------------
// Variant 2: O(n log n) patience sorting.
//
// `tails[k]` holds the SMALLEST possible tail value among all increasing
// subsequences of length k+1 built from the prefix of `nums` processed so
// far. It is NOT a real subsequence you could point to in `nums` — it is a
// summary of "best possible tail per length" (see README's Disadvantages
// section for why this matters when you need to reconstruct the actual
// subsequence, not just its length).
//
// For each new number x:
//   - std::lower_bound finds the first element in `tails` that is >= x
//     (the leftmost position x could occupy while keeping `tails` sorted
//     and respecting strict increase).
//   - If no such element exists (x is bigger than every tail so far), x
//     extends the longest subsequence found so far by one: append it.
//   - Otherwise, x can replace that tail with a smaller-or-equal value
//     without invalidating anything: replace it in place. This never
//     shrinks `tails.size()` and can only help future elements, because a
//     smaller tail for the same length is strictly at least as easy to
//     extend later.
//
// `tails.size()` at the end is the answer: the length of the LIS.
//
// NOTE on strict vs non-strict increase: this uses std::lower_bound, which
// finds the first element NOT LESS THAN x (i.e. >= x). That is correct for
// STRICT increase (no repeated values allowed in the subsequence). If the
// problem instead wants a NON-DECREASING subsequence (repeats allowed),
// swap in std::upper_bound (first element STRICTLY GREATER than x) instead
// — see the README's Common Mistakes section for why mixing these up is a
// classic off-by-one bug.
// ----------------------------------------------------------------------------
template <typename T>
int lengthOfLIS_NLogN(const std::vector<T>& nums) {
  std::vector<T> tails;
  tails.reserve(nums.size());

  for (const auto& x : nums) {
    auto it = std::lower_bound(tails.begin(), tails.end(), x);
    if (it == tails.end()) {
      tails.push_back(x);  // x is larger than every current tail: extend.
    } else {
      *it = x;  // x gives a strictly-better (smaller-or-equal) tail for this length.
    }
  }

  return static_cast<int>(tails.size());
}

// ============================================================================
// main() — runs both algorithms against the same inputs and asserts they
// always agree, proving the O(n log n) trick is equivalent to the O(n^2) DP.
// ============================================================================
int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check_both_agree = [&](const std::vector<int>& nums, int expected,
                               const std::string& label) {
    int result_on2 = lengthOfLIS_On2(nums);
    int result_nlogn = lengthOfLIS_NLogN(nums);

    bool ok = (result_on2 == expected) && (result_nlogn == expected) &&
              (result_on2 == result_nlogn);

    if (ok) {
      std::cout << "[PASS] " << label << " -> both report " << result_on2 << "\n";
      ++pass_count;
    } else {
      std::cout << "[FAIL] " << label << " -> On2=" << result_on2
                << " NLogN=" << result_nlogn << " expected=" << expected << "\n";
      ++fail_count;
    }
  };

  std::cout << "--- lengthOfLIS_On2 vs lengthOfLIS_NLogN equivalence checks ---\n";

  check_both_agree({10, 9, 2, 5, 3, 7, 101, 18}, 4,
                    "classic example [10,9,2,5,3,7,101,18]");

  check_both_agree({0, 1, 0, 3, 2, 3}, 4, "[0,1,0,3,2,3]");

  check_both_agree({7, 7, 7, 7, 7, 7, 7}, 1,
                    "all equal values (strict increase -> length 1)");

  check_both_agree({1, 2, 3, 4, 5}, 5, "already strictly increasing");

  check_both_agree({5, 4, 3, 2, 1}, 1, "strictly decreasing -> length 1");

  check_both_agree({}, 0, "empty input -> length 0");

  check_both_agree({42}, 1, "single element -> length 1");

  check_both_agree({4, 10, 4, 3, 8, 9}, 3, "[4,10,4,3,8,9]");

  check_both_agree({-2, -1, -3, 5, 0, 4}, 4, "with negative numbers [-2,-1,-3,5,0,4]");

  // A larger, less hand-checkable input, still verified independently via
  // the O(n^2) DP acting as the "obviously correct" oracle for the O(n log n)
  // version — this is the whole point of running both side by side.
  check_both_agree({3, 4, -1, 0, 6, 2, 3}, 4, "[3,4,-1,0,6,2,3]");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
