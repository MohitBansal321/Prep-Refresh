// ============================================================================
// LeetCode 2305 — Fair Distribution of Cookies
// ============================================================================
//
// PROBLEM
// -------
// You are given an integer array `cookies` (bags of cookies) and an integer
// `k` (children, k <= number of bags). Distribute ALL bags to the children,
// where each child may receive any number of bags including zero. The
// UNFAIRNESS of a distribution is the maximum total any single child
// receives. Return the minimum possible unfairness over all distributions.
//
// Example: cookies = [8,15,10,20,8], k = 2 -> 31
//   ({20,10} sums to 30 vs {8,15,8} sums to 31; no split beats max 31)
//
// APPROACH — Bitmask DP with subset-enumeration transitions
// ---------------------------------------------------------
// Recognition signal: n <= 8 bags and k <= n children — tiny — but the
// state genuinely needs subset identity: which specific bags are already
// handed out determines what remains for the next child. A count-based DP
// cannot distinguish leaving {1,4} versus {2,3} for later children.
//
// State: solve(child, mask) = the minimum achievable "max bag sum" for
// children `child..k-1`, given exactly the bags in `mask` are already
// assigned. Answer = solve(0, 0).
//
// Transition: choose a SUBSET of the still-unassigned bags (a submask of
// ~mask) to give entirely to the current child, then recurse on the next
// child. Unlike most bitmask problems — where one transition adds ONE item
// — here a single move can commit many items at once. That is why the
// inner loop enumerates submasks with the classic lossless trick:
//
//     sub = free;                       // start from the full remainder
//     ... use sub ...
//     sub = (sub - 1) & free;           // next-lower submask of `free`
//     ... repeat while sub != 0 ...
//
// This visits every submask of `free` exactly once in decreasing numeric
// order, including `free` itself; the empty submask is handled separately
// (a child receiving nothing is legal).
//
// Value: giving subset S to this child costs max(sum(S), solve(child+1,
// mask|S)) — unfairness is the MAXIMUM across children, so the current
// child's own total enters as a lower bound on the answer, and we minimize.
//
// COMPLEXITY
// ----------
// Time:  O(k * 3^n) — for each of k children boundaries, the total work
//               enumerating submasks over all masks is 3^n (each element is
//               in one of three states: not yet in mask, in mask but not in
//               the chosen submask, or in both). With n <= 8 that is at
//               most 6561 * 8 steps per memo level — trivially fast.
// Space: O(k * 2^n) for the memo table (plus recursion stack).
// ============================================================================
#include <iostream>
#include <string>
#include <vector>

class Solution {
 public:
  int distributeCookies(const std::vector<int>& cookies, int k) {
    cookies_ = &cookies;
    n_ = static_cast<int>(cookies.size());
    k_ = k;

    const int fullMask = (1 << n_) - 1;

    // Precompute subset sums once so every submask lookup is O(1) later.
    // dp[0] = 0 comes straight from assign(); start the recurrence at
    // mask = 1 because extracting a lowest bit from 0 is undefined.
    subsetSum_.assign(1 << n_, 0);
    for (int mask = 1; mask <= fullMask; ++mask) {
      int low = mask & (-mask);  // lowest set bit of this subset
      int idx = __builtin_ctz(static_cast<unsigned>(low));
      subsetSum_[mask] = subsetSum_[mask ^ low] + cookies[idx];
    }

    // -1 marks "not computed"; memo dimensions are (child, mask).
    memo_.assign(static_cast<size_t>(k) * (fullMask + 1), -1);
    return solve(0, 0);
  }

 private:
  int solve(int child, int mask) {
    const int fullMask = (1 << n_) - 1;

    // Base case: every bag handed out -> no unfairness added downstream.
    if (mask == fullMask) return 0;
    // Guard: out of children while bags remain should never happen when
    // k <= n and every bag is eventually given, but fail loudly rather
    // than silently returning garbage if the invariant ever breaks.
    if (child >= k_) return kLarge;

    int& cached = memo_[static_cast<size_t>(child) * ((1 << n_)) + mask];
    if (cached != -1) return cached;

    int freeBags = fullMask & ~mask;  // bags still unassigned
    int best = kLarge;

    // Give the current child every non-empty submask of the remaining bags.
    for (int sub = freeBags; sub != 0; sub = (sub - 1) & freeBags) {
      int costHere = subsetSum_[sub];
      if (costHere >= best) continue;  // cannot beat the incumbent optimum
      int rest = solve(child + 1, mask | sub);
      if (rest >= kLarge) continue;
      best = std::min(best, std::max(costHere, rest));
    }

    return cached = best;
  }

  static const int kLarge =
      1000000000;  // sentinel larger than any possible unfairness

  const std::vector<int>* cookies_ = nullptr;
  std::vector<int> subsetSum_;
  std::vector<int> memo_;
  int n_ = 0;
  int k_ = 0;
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
    // Classic example: {20,10} vs {8,15,8} gives max 31.
    check(sol.distributeCookies({8, 15, 10, 20, 8}, 2) == 31,
          "[8,15,10,20,8], k=2 -> 31");
  }

  {
    // Perfect balance achievable: {6,1},{4,2,1},{3,2,2} all sum to 7.
    check(sol.distributeCookies({6, 1, 3, 2, 2, 4, 1, 2}, 3) == 7,
          "[6,1,3,2,2,4,1,2], k=3 -> 7");
  }

  {
    // Edge case: k equals the bag count -> every child gets exactly one
    // bag, so the answer is simply the largest single bag.
    check(sol.distributeCookies({3, 9, 5}, 3) == 9,
          "[3,9,5], k=3 -> 9 (one bag per child)");
  }

  {
    // Edge case: single bag, single child takes everything.
    check(sol.distributeCookies({5}, 1) == 5, "[5], k=1 -> 5");
  }

  {
    // More children than strictly needed: extra children can take zero
    // bags, so the optimal split ignores them (same as k=2 here).
    check(sol.distributeCookies({4, 6}, 4) == 6,
          "[4,6], k=4 -> 6 (spare children take nothing)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
