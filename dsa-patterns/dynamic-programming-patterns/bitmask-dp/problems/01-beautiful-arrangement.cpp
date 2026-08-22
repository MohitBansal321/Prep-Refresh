// ============================================================================
// LeetCode 526 — Beautiful Arrangement
// ============================================================================
//
// PROBLEM
// -------
// Suppose you have n integers labeled 1 through n. A permutation of those n
// integers `perm` (1-indexed) is considered a BEAUTIFUL ARRANGEMENT if for
// every i (1 <= i <= n), at least ONE of these holds:
//   - perm[i] is divisible by i, OR
//   - i is divisible by perm[i].
// Given an integer n, return the NUMBER of beautiful arrangements that exist.
//
// Example: n = 2 -> answer 2, because:
//   [1, 2]  (perm[1]=1 divisible by 1; perm[2]=2 divisible by 2)
//   [2, 1]  (perm[1]=2 divisible by 1; 1 divides perm[2]=... 1 | 2 holds)
//
// APPROACH — Bitmask DP over "which numbers are already placed"
// -------------------------------------------------------------
// Brute force enumerates all n! permutations — hopeless past n = 10 or so.
// But notice the structure: the validity rule couples a POSITION with the
// VALUE placed there, and positions get filled left to right. If we always
// fill positions in order 1, 2, 3, ..., then when we are about to fill
// position p, the ONLY thing that matters about the past is WHICH numbers
// were used — not the order they were placed in. That is exactly the
// recognition signal for bitmask DP (see ../README.md): small n (<= 15 here)
// and subproblems keyed by subset identity.
//
// State: mask = set of numbers already placed.
// Key trick: position to fill = __builtin_popcount(mask) + 1. Because we
// fill positions strictly in increasing order, the number of filled slots
// equals the number of used numbers — so the mask alone determines the
// entire state and NO extra dimension is needed.
//
// Transition: try every unused number x; if x is compatible with position p
// (x % p == 0 || p % x == 0), recurse on mask | (1 << x). Memoize counts.
//
// Why memoization helps even though every permutation path differs: many
// different placement ORDERS lead to the same final SET of used numbers,
// and from any given mask the count of completions is identical. The memo
// collapses that combinatorial overlap from O(n!) paths down to O(2^n)
// distinct states, each doing O(n) transition work.
//
// COMPLEXITY
// ----------
// Time:  O(2^n * n) — each of the 2^n masks computes once, scanning n bits.
// Space: O(2^n) for the memo table (plus O(n) recursion stack).
// ============================================================================
#include <iostream>
#include <string>
#include <vector>

class Solution {
 public:
  int countArrangement(int n) {
    // memo[mask] = number of ways to complete the arrangement given that
    // exactly the numbers in `mask` have been placed. -1 = not computed yet.
    // Bit (v - 1) of the mask stands for value v, so values map to bits
    // without wasting bit 0.
    memo_.assign(1 << n, -1);
    n_ = n;
    return solve(0);
  }

 private:
  int solve(int mask) {
    int pos = __builtin_popcount(static_cast<unsigned>(mask)) +
              1;  // next 1-indexed position to fill

    // Base case: all n numbers placed -> this is exactly one full arrangement.
    if (pos > n_) return 1;

    int& cached = memo_[mask];
    if (cached != -1) return cached;

    int total = 0;
    for (int v = 1; v <= n_; ++v) {
      int bit = 1 << (v - 1);          // bit (v-1) represents value v
      if (mask & bit) continue;        // value already placed elsewhere
      if (v % pos != 0 && pos % v != 0) continue;  // violates the beauty rule
      total += solve(mask | bit);
    }
    return cached = total;
  }

  std::vector<int> memo_;
  int n_ = 0;
};

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

  Solution sol;

  {
    // Smallest possible input: only [1].
    check(sol.countArrangement(1) == 1, "n=1 -> 1 arrangement");
  }

  {
    // Classic example: [1,2] and [2,1] both work.
    check(sol.countArrangement(2) == 2, "n=2 -> 2 arrangements");
  }

  {
    // Known values from LeetCode examples / problem discussion.
    check(sol.countArrangement(3) == 3, "n=3 -> 3 arrangements");
    check(sol.countArrangement(4) == 8, "n=4 -> 8 arrangements");
  }

  {
    // Larger input exercising the exponential speedup: brute force would be
    // 7! = 5040 permutations; memoized DP visits far fewer states.
    check(sol.countArrangement(7) == 41, "n=7 -> 41 arrangements");
    check(sol.countArrangement(15) == 24679,
          "n=15 -> 24679 arrangements (max constraint)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
