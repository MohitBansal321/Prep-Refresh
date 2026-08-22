// ============================================================================
// LeetCode 239 — Sliding Window Maximum
// ============================================================================
//
// PROBLEM
// -------
// You are given an array of integers `nums` and a window size k. There is a
// sliding window of size k that moves from the very left to the very right,
// one position at a time. Return the max of each window.
//
// Example: nums = [1,3,-1,-3,5,3,6,7], k = 3
//   Windows: [1 3 -1] -3 5 3 6 7 -> 3
//             1 [3 -1 -3] 5 3 6 7 -> 3
//             1 3 [-1 -3 5] 3 6 7 -> 5
//             1 3 -1 [-3 5 3] 6 7 -> 5
//             1 3 -1 -3 [5 3 6] 7 -> 6
//             1 3 -1 -3 5 [3 6 7] -> 7
//   Answer: [3,3,5,5,6,7]
//
// APPROACH — Monotonic DEQUE of indices, values decreasing front-to-back
// --------------------------------------------------------------------------
// WHY A RUNNING VARIABLE FAILS: a sum can be maintained by adding the entering
// element and subtracting the leaving one, but a MAX cannot — if the leaving
// element WAS the max, there is no way to "un-max" and recover the runner-up.
// A heap gives the max in O(log n) but has the same removal problem (you must
// lazily delete expired elements). The monotonic deque solves both ends at
// once:
//
// The deque holds INDICES whose values are strictly DECREASING front-to-back.
// Two facts make this the right invariant:
//   1. The FRONT is always the current window's maximum (it is the largest,
//      by the invariant).
//   2. If an incoming element is >= some element currently in the deque, that
//      older element can NEVER be a future window's maximum again: every
//      future window containing the old element also contains the new one
//      (the new one is to its RIGHT), and the new one is at least as large.
//      So the old element is dead weight — pop it from the BACK immediately.
//
// Per step i:
//   a. BACK-POP (domination): while the back's value < nums[i], pop it. Each
//      popped index is permanently dominated by i.
//   b. Push i.
//   c. FRONT-EVICT (expiry): if dq.front() <= i - k, it has slid out of the
//      window [i-k+1, i]; pop it. Note an index can only just now have
//      expired, so a single 'if' suffices (not a while).
//   d. RECORD: once the first full window exists (i >= k - 1), the answer for
//      this window is nums[dq.front()].
//
// WHY THIS IS O(n) DESPITE THE INNER WHILE LOOP (amortized argument):
// each index enters the deque exactly once and leaves at most once (from the
// back via domination, or from the front via expiry) — total deque operations
// <= 2n across the whole run. The naive per-window scan is O(n·k).
//
// COMPLEXITY
// ----------
// Time:  O(n) amortized.
// Space: O(k) — the deque never holds indices spanning more than one window,
//        because expired entries are evicted from the front each step.
// ============================================================================
#include <deque>
#include <iostream>
#include <string>
#include <vector>

std::vector<int> maxSlidingWindow(const std::vector<int>& nums, int k) {
  std::deque<int> dq;  // INDICES; values strictly decreasing front-to-back
  std::vector<int> result;
  result.reserve(nums.size() - k + 1);

  for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
    // (a) Domination: anything smaller than the newcomer can never be a
    // future max — every future window holding it also holds nums[i].
    // Strict '<' keeps duplicates in the deque; either comparison yields a
    // correct maximum, but '<' makes expiry handling unambiguous.
    while (!dq.empty() && nums[dq.back()] < nums[i]) {
      dq.pop_back();
    }

    // (b) The newcomer joins at the back as the smallest (or tied-smallest)
    // entry — the invariant is restored.
    dq.push_back(i);

    // (c) Expiry: front index outside window [i-k+1, i]. Condition is
    // dq.front() <= i - k, i.e. front entered k or more steps ago.
    if (dq.front() <= i - k) {
      dq.pop_front();
    }

    // (d) Record once the first complete window [0..k-1] has formed.
    if (i >= k - 1) {
      result.push_back(nums[dq.front()]);
    }
  }

  return result;
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](bool condition, const std::string& label) {
    if (condition) {
      std::cout << "[PASS] " << label << "\n";
      ++pass_count;
    } else {
      std::cout << "[FAIL] " << label << "\n";
      ++fail_count;
    }
  };

  {
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<int> expected = {3, 3, 5, 5, 6, 7};
    check(maxSlidingWindow(nums, 3) == expected,
          "classic example k=3 -> [3,3,5,5,6,7]");
  }

  {
    // k = 1: every element is its own window's max.
    std::vector<int> nums = {1};
    std::vector<int> expected = {1};
    check(maxSlidingWindow(nums, 1) == expected, "single element k=1 -> [1]");
  }

  {
    std::vector<int> nums = {9, 8, 7, 6, 5};
    std::vector<int> expected = {9, 8, 7};
    check(maxSlidingWindow(nums, 3) == expected,
          "strictly decreasing: front expires each step -> [9,8,7]");
  }

  {
    // Strictly increasing: every newcomer dominates the whole deque, so the
    // deque holds exactly one index at all times.
    std::vector<int> nums = {1, 2, 3, 4, 5};
    std::vector<int> expected = {3, 4, 5};
    check(maxSlidingWindow(nums, 3) == expected,
          "strictly increasing: constant back-pops -> [3,4,5]");
  }

  {
    // Duplicates: strict '<' keeps equal values; when the older duplicate
    // expires the newer identical one still represents the max correctly.
    std::vector<int> nums = {4, 4, 4, 4};
    std::vector<int> expected = {4, 4, 4};
    check(maxSlidingWindow(nums, 2) == expected,
          "all duplicates k=2 -> [4,4,4]");
  }

  {
    // The current max sits exactly at the window edge and must be evicted by
    // the front-expiry rule, promoting the next candidate.
    std::vector<int> nums = {5, 1, 2, 3};
    std::vector<int> expected = {5, 2, 3};
    check(maxSlidingWindow(nums, 2) == expected,
          "max at window edge gets evicted -> [5,2,3]");
  }

  {
    // Negative values throughout.
    std::vector<int> nums = {-7, -3, -9, -1};
    std::vector<int> expected = {-3, -1};
    check(maxSlidingWindow(nums, 3) == expected,
          "negatives k=3 -> [-3,-1]");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
