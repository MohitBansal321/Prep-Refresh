// ============================================================================
// LeetCode 327 — Count of Range Sum
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums` and two integers `lower` and `upper`, return
// the number of range sums that lie in [lower, upper], inclusive. A range sum
// S(i, j) is the sum of the elements at indices i..j inclusive, with i <= j.
//
// Example: nums = [-2, 5, -1], lower = -2, upper = 2
//   -> 3   (ranges [0,0] = -2, [2,2] = -1, [0,2] = 2)
//
// APPROACH — Prefix Sums + Fenwick Tree over compressed keys
// -----------------------------------------------------------
// Define prefix sums S[0] = 0 and S[k] = nums[0] + ... + nums[k-1]. Then
// every range sum is a DIFFERENCE of two prefixes:
//
//     S(i, j) = S[j+1] - S[i]
//
// So counting range sums in [lower, upper] is counting PAIRS (i, j') with
// i < j' and
//
//     lower <= S[j'] - S[i] <= upper
//   <=>      S[j'] - upper <= S[i] <= S[j'] - lower
//
// Scan j' left to right; when we reach j', all earlier prefix sums are
// already inserted in a Fenwick Tree (over VALUE ranks, storing occurrence
// counts). The count of valid i's for this j' is:
//
//     query(rank_count(S[j'] - lower)) - query(rank_count(S[j'] - upper) - 1)
//       = #inserted values <= S[j']-lower  minus  #inserted values <  S[j']-upper
//
// i.e. an inclusive-window count assembled from two prefix-count queries —
// the same "derive a range from two prefixes" trick as rangeSum itself.
//
// Three traps this problem concentrates:
//   1. OVERFLOW EVERYWHERE: n up to ~10^5 elements of magnitude up to 2^31
//      makes prefix sums reach ~2^47 — long long is not optional, for the
//      prefix array AND both window endpoints.
//   2. THE WINDOW ENDPOINTS ARE NOT KEYS: S[j']-lower and S[j']-upper are
//      arbitrary values, so their positions come from upper_bound /
//      lower_bound counts against the sorted key list, never from inserting
//      them.
//   3. S[0] = 0 MUST BE INSERTED FIRST: it represents the empty prefix and
//      is what lets single-element ranges (i == j) be counted.
//
// COMPLEXITY
// ----------
// Time:  O(n log n) — one sort plus n iterations with O(log n) searches and
//               tree operations each.
// Space: O(n) — prefix array, key list, tree.
//
// Correctness is cross-checked in main() against an O(n^2) brute force over
// all (i, j) ranges on batteries including negatives, duplicates, and
// overflow-scale values.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// Fenwick Tree storing occurrence counts over compressed value-ranks.
class FenwickCount {
 public:
  explicit FenwickCount(int size) : tree_(size + 1, 0), n_(size) {}

  void update(int i, int delta) {
    for (++i; i <= n_; i += i & (-i)) tree_[i] += delta;
  }

  // Total occurrences at ranks [0..i].
  long long query(int i) const {
    long long sum = 0;
    for (++i; i > 0; i -= i & (-i)) sum += tree_[i];
    return sum;
  }

 private:
  std::vector<int> tree_;
  int n_;
};

int countRangeSum(const std::vector<int>& nums, int lower, int upper) {
  const int n = static_cast<int>(nums.size());
  if (n == 0) return 0;

  // ---- Prefix sums (long long: n * INT_MAX overflows int badly) ---------
  std::vector<long long> pre(n + 1, 0);
  for (int k = 0; k < n; ++k) pre[k + 1] = pre[k] + nums[k];

  // ---- Coordinate compression over ALL indexed values -------------------
  // Only the prefix sums themselves ever get inserted; the window endpoints
  // (S[j]-lower / S[j]-upper) are located by binary search against these
  // keys, so compressing just the prefixes suffices.
  std::vector<long long> keys(pre.begin(), pre.end());
  std::sort(keys.begin(), keys.end());
  keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
  const int m = static_cast<int>(keys.size());

  // Number of distinct keys strictly less than x (via lower_bound).
  auto countLess = [&](long long x) -> int {
    return static_cast<int>(
        std::lower_bound(keys.begin(), keys.end(), x) - keys.begin());
  };
  // Number of distinct keys <= x (via upper_bound).
  auto countLeq = [&](long long x) -> int {
    return static_cast<int>(
        std::upper_bound(keys.begin(), keys.end(), x) - keys.begin());
  };

  FenwickCount ft(m);

  // Insert S[0] = 0 first: it stands for the empty prefix, enabling
  // single-element ranges to be found by later iterations.
  int r0 = static_cast<int>(std::lower_bound(keys.begin(), keys.end(), 0LL) -
                            keys.begin());
  ft.update(r0, 1);

  long long total = 0;
  for (int j = 1; j <= n; ++j) {
    // Valid partners: inserted S[i] in [S[j]-upper, S[j]-lower].
    // #in-window = (#<= hi) - (#< lo).
    int hi_leq = countLeq(pre[j] - static_cast<long long>(lower));
    int lo_less = countLess(pre[j] - static_cast<long long>(upper));
    if (hi_leq > 0) total += ft.query(hi_leq - 1);
    if (lo_less > 0) total -= ft.query(lo_less - 1);

    // Insert S[j] AFTER counting so that i < j strictly.
    int rj =
        static_cast<int>(std::lower_bound(keys.begin(), keys.end(), pre[j]) -
                         keys.begin());
    ft.update(rj, 1);
  }

  return static_cast<int>(total);
}

// O(n^2) reference implementation used only to cross-check the Fenwick result.
int countRangeSumBrute(const std::vector<int>& nums, int lower, int upper) {
  const int n = static_cast<int>(nums.size());
  int count = 0;
  for (int i = 0; i < n; ++i) {
    long long sum = 0;
    for (int j = i; j < n; ++j) {
      sum += nums[j];  // long long: range sums can overflow int
      if (sum >= lower && sum <= upper) ++count;
    }
  }
  return count;
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
    // Canonical example from the problem statement.
    check(countRangeSum({-2, 5, -1}, -2, 2) == 3,
          "[-2,5,-1], [-2,2] -> 3");
  }

  {
    // Single element boundaries: window exactly matching / missing the value.
    check(countRangeSum({0}, 0, 0) == 1, "[0], [0,0] -> 1");
    check(countRangeSum({5}, 6, 9) == 0, "[5], window above value -> 0");
    check(countRangeSum({-7}, -10, -8) == 0, "[-7], window below value -> 0");
  }

  {
    // Inclusive endpoints matter: sums equal to lower or upper must count.
    check(countRangeSum({1, 1, 1}, 2, 2) == 2,
          "[1,1,1], [2,2]: ranges [0,1],[1,2] hit boundary -> 2");
    check(countRangeSum({2, -2}, 0, 0) == 1,
          "[2,-2], [0,0]: only [0,1]=0 lands in window -> 1");
  }

  {
    // Overflow battery: prefix sums here exceed 32-bit range; any int
    // arithmetic in the prefix/window computation would misfire.
    check(countRangeSum({2147483647, 2147483647, -2147483648}, -4, 4) ==
              countRangeSumBrute({2147483647, 2147483647, -2147483648}, -4, 4),
          "INT_MAX/INT_MIN prefix overflow matches brute force");
  }

  {
    // Cross-check battery against brute force: assorted shapes, negative
    // windows, duplicates, mixed signs.
    struct Case {
      std::vector<int> nums;
      int lower, upper;
    };
    std::vector<Case> batteries = {
        {{-2, 5, -1}, -2, 2},
        {{1, 2, 3}, 0, 100},
        {{1, 2, 3}, 100, 200},
        {{0, 0, 0, 0}, 0, 0},
        {{-1, -1, -1}, -3, -1},
        {{5, -12, 7, -3, 9, -8}, -10, 5},
        {{2147483647, -2147483647, 1}, -1, 1}};
    bool all_match = true;
    for (size_t t = 0; t < batteries.size(); ++t) {
      if (countRangeSum(batteries[t].nums, batteries[t].lower,
                        batteries[t].upper) !=
          countRangeSumBrute(batteries[t].nums, batteries[t].lower,
                             batteries[t].upper)) {
        all_match = false;
      }
    }
    check(all_match, "battery of 7 cases matches O(n^2) brute force");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
