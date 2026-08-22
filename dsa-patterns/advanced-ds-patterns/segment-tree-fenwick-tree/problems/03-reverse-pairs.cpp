// ============================================================================
// LeetCode 493 — Reverse Pairs
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums`, return the number of REVERSE PAIRS: index
// pairs (i, j) with i < j and nums[i] > 2 * nums[j].
//
// Example: nums = [1, 3, 2, 3, 1]
//   -> 2   (pairs (0,4): 1 > 2*1? no... actual pairs are (1,4): 3 > 2*1,
//           and (3,4): 3 > 2*1)
//
// APPROACH — Fenwick Tree over compressed keys, scanned left-to-right
// -------------------------------------------------------------------
// Brute force checks all pairs: O(n^2), too slow at n = 5*10^4. The key
// reframing: a pair (i, j) is counted at the moment we PROCESS j — every
// earlier-inserted element is exactly the set of valid i's. So scan j from
// left to right, maintaining a Fenwick Tree of occurrence counts over VALUE
// ranks, and at each step ask:
//
//     how many inserted values v satisfy v > 2 * nums[j]?
//         = inserted_total - count(v <= 2 * nums[j])
//         = inserted_total - query(rank_count(2 * nums[j]))
//
// where rank_count(x) is how many DISTINCT KEYS are <= x — because the tree
// is indexed by compressed rank, "values <= x" corresponds to the first
// rank_count(x) slots, which is exactly what a prefix query returns.
//
// Two traps this problem is famous for:
//   1. OVERFLOW: 2 * nums[j] can reach ~2^32 in magnitude — far outside int.
//      The threshold must be computed and stored as long long. We never put
//      thresholds INTO the tree; we only binary-search them against the key
//      list, so the keys themselves stay within int range but we still widen
//      them to long long for safe comparison.
//   2. THRESHOLD NOT A KEY: 2 * nums[j] generally is not one of the values,
//      so its "rank" comes from upper_bound (count of keys <= threshold),
//      not lower_bound. Using lower_bound here silently miscounts pairs with
//      v == 2 * nums[j], which must NOT count (condition is strict >).
//
// COMPLEXITY
// ----------
// Time:  O(n log n) — sort for compression plus n iterations, each doing an
//               O(log n) binary search and an O(log n) tree operation.
// Space: O(n) — key list, tree, bookkeeping arrays.
//
// Correctness is cross-checked in main() against an O(n^2) brute force on
// batteries including negatives, duplicates, INT_MIN/INT_MAX extremes, and
// values whose doubling overflows int.
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

long long reversePairs(const std::vector<int>& nums) {
  const int n = static_cast<int>(nums.size());
  if (n == 0) return 0;

  // ---- Coordinate compression over the ORIGINAL values ------------------
  // Thresholds (2 * nums[j]) are searched against these keys via
  // upper_bound; they never need to be inserted themselves.
  std::vector<long long> keys(nums.begin(), nums.end());
  std::sort(keys.begin(), keys.end());
  keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
  const int m = static_cast<int>(keys.size());

  FenwickCount ft(m);
  long long inserted = 0;  // total occurrences pushed so far (= j so far)
  long long pairs = 0;

  // ---- Left-to-right scan ----------------------------------------------
  for (int j = 0; j < n; ++j) {
    // Count already-inserted values strictly greater than 2 * nums[j]:
    // everything inserted MINUS everything inserted that is <= threshold.
    long long threshold = 2LL * nums[j];  // long long: avoids int overflow
    int leq =
        static_cast<int>(
            std::upper_bound(keys.begin(), keys.end(), threshold) -
            keys.begin());  // number of distinct keys <= threshold
    if (leq > 0) {
      pairs += inserted - ft.query(leq - 1);
    } else {
      pairs += inserted;  // threshold below every key: all inserted count
    }

    // Insert nums[j] AFTER counting, since the pair needs i < j strictly.
    int rank =
        static_cast<int>(std::lower_bound(keys.begin(), keys.end(),
                                          static_cast<long long>(nums[j])) -
                         keys.begin());
    ft.update(rank, 1);
    ++inserted;
  }

  return pairs;
}

// O(n^2) reference implementation used only to cross-check the Fenwick result.
long long reversePairsBrute(const std::vector<int>& nums) {
  const int n = static_cast<int>(nums.size());
  long long pairs = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      if (static_cast<long long>(nums[i]) > 2LL * nums[j]) ++pairs;
    }
  }
  return pairs;
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
    // Canonical examples from the problem statement.
    check(reversePairs({1, 3, 2, 3, 1}) == 2, "[1,3,2,3,1] -> 2");
    check(reversePairs({2, 4, 3, 5, 1}) == 3, "[2,4,3,5,1] -> 3");
  }

  {
    // Strictness: v == 2 * nums[j] must NOT count (upper_bound, not
    // lower_bound, on the threshold).
    check(reversePairs({2, 1}) == 0, "[2,1]: 2 > 2*1 is false -> 0");
    check(reversePairs({5, 2}) == 1, "[5,2]: 5 > 4 -> 1");
  }

  {
    // Overflow battery: doubling these values escapes int range. Any int
    // arithmetic in the threshold would produce wrong answers here.
    check(reversePairs({2147483647, -2147483648}) == 1,
          "INT_MAX vs INT_MIN: 2*INT_MIN overflows int -> pair counts");
    check(reversePairs({-2147483648, 2147483647}) == 0,
          "INT_MIN vs INT_MAX -> 0");
  }

  {
    // Duplicates and negatives.
    check(reversePairs({1, 1, 1}) == 0, "all equal -> 0");
    check(reversePairs({-5, -5, -5}) == 3,
          "all equal negatives: -5 > -10 holds -> 3 pairs");
    check(reversePairs({-3, -1, -2}) == 2,
          "[-3,-1,-2]: -3>-2 no; -3>-4 yes; -1>-4 yes -> 2");
  }

  {
    // Cross-check battery against brute force: assorted shapes including
    // sorted, reverse-sorted, mixed signs, and overflow-prone extremes.
    std::vector<std::vector<int>> batteries = {
        {1, 3, 2, 3, 1},
        {2, 4, 3, 5, 1},
        {5, 4, 3, 2, 1},
        {1, 2, 3, 4, 5},
        {2147483647, 1000000000, -1000000000, -2147483648},
        {-2147483648, -2147483648, 2147483647, 0},
        {9, 1, 8, 2, 7, 3, 6, 4}};
    bool all_match = true;
    for (size_t t = 0; t < batteries.size(); ++t) {
      if (reversePairs(batteries[t]) != reversePairsBrute(batteries[t])) {
        all_match = false;
      }
    }
    check(all_match, "battery of 7 arrays matches O(n^2) brute force");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
