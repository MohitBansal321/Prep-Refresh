// ============================================================================
// LeetCode 315 — Count of Smaller Numbers After Self
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums`, return an array `counts` where counts[i] is
// the number of elements to the RIGHT of i (indices j > i) that are STRICTLY
// SMALLER than nums[i].
//
// Example: nums = [5, 2, 6, 1]
//   -> [2, 1, 1, 0]
//   (right of 5: {2, 1} smaller -> 2; right of 2: {1} -> 1;
//    right of 6: {1} -> 1; right of 1: nothing -> 0)
//
// APPROACH — Fenwick Tree over compressed ranks, scanned right-to-left
// -------------------------------------------------------------------
// Brute force compares every pair: O(n^2) — too slow at n = 10^5. The
// Fenwick reformulation turns "how many smaller to my right" into a prefix
// COUNT question:
//
//   Walk positions from RIGHT to LEFT, maintaining a Fenwick Tree indexed by
//   VALUE-RANK whose slots store occurrence counts. When we arrive at
//   nums[i], every element already inserted is exactly the set of elements
//   to its right. So:
//     counts[i] = query(rank(nums[i]) - 1)   // how many inserted values are
//                                            // strictly below nums[i]
//     insert(rank(nums[i]))                  // then join them
//
// Why coordinate compression is mandatory: values can be any 32-bit int
// (negative, huge), but a Fenwick Tree needs small non-negative indices.
// We map each value to its rank in the sorted distinct values. Ranks preserve
// order, so "strictly smaller value" becomes exactly "strictly smaller rank"
// and all comparisons survive compression intact.
//
// Why right-to-left rather than left-to-right: either direction works with a
// mirrored question ("how many greater to my left"), but this direction lets
// us fill `counts` in its natural index order without reversing at the end.
//
// COMPLEXITY
// ----------
// Time:  O(n log n) — one sort for compression plus n tree operations of
//               O(log n) each.
// Space: O(n) — compressed key list, rank map, tree, output.
//
// Correctness is cross-checked in main() against an O(n^2) brute force on
// several arrays, including duplicates, negatives, and sorted extremes.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// Standard Fenwick Tree storing occurrence counts. Positions are RANKS in
// [0, m-1] externally (0-indexed), 1-indexed internally.
class FenwickCount {
 public:
  explicit FenwickCount(int size) : tree_(size + 1, 0), n_(size) {}

  void update(int i, int delta) {
    for (++i; i <= n_; i += i & (-i)) tree_[i] += delta;
  }

  // Number of occurrences at ranks [0..i].
  long long query(int i) const {
    long long sum = 0;
    for (++i; i > 0; i -= i & (-i)) sum += tree_[i];
    return sum;
  }

 private:
  std::vector<int> tree_;
  int n_;
};

std::vector<int> countSmaller(const std::vector<int>& nums) {
  const int n = static_cast<int>(nums.size());
  if (n == 0) return {};

  // ---- Coordinate compression ------------------------------------------
  // sorted distinct values; rank(v) = index in this vector preserves order.
  std::vector<long long> keys(nums.begin(), nums.end());
  std::sort(keys.begin(), keys.end());
  keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
  const int m = static_cast<int>(keys.size());

  std::vector<int> counts(n);
  FenwickCount ft(m);

  // ---- Right-to-left scan ----------------------------------------------
  for (int i = n - 1; i >= 0; --i) {
    // Rank of nums[i]: first key >= nums[i]. Since nums[i] IS a key, this is
    // exact (not merely a lower bound).
    int rank =
        static_cast<int>(std::lower_bound(keys.begin(), keys.end(),
                                          static_cast<long long>(nums[i])) -
                         keys.begin());
    // Strictly smaller values = occurrences at ranks strictly below mine.
    counts[i] = static_cast<int>(rank > 0 ? ft.query(rank - 1) : 0);
    ft.update(rank, 1);  // register my own occurrence for elements to my left
  }

  return counts;
}

// O(n^2) reference implementation used only to cross-check the Fenwick result.
std::vector<int> countSmallerBrute(const std::vector<int>& nums) {
  const int n = static_cast<int>(nums.size());
  std::vector<int> counts(n, 0);
  for (int i = 0; i < n; ++i) {
    int c = 0;
    for (int j = i + 1; j < n; ++j) {
      if (nums[j] < nums[i]) ++c;
    }
    counts[i] = c;
  }
  return counts;
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

  auto expect = [&](const std::vector<int>& got,
                    const std::vector<int>& want, const std::string& label) {
    check(got == want, label);
  };

  {
    // Canonical example from the problem statement.
    expect(countSmaller({5, 2, 6, 1}), {2, 1, 1, 0}, "[5,2,6,1] -> [2,1,1,0]");
  }

  {
    // Duplicates must count as neither smaller nor greater (STRICTLY smaller).
    expect(countSmaller({2, 2, 2}), {0, 0, 0},
           "all equal -> all zeros (strictness)");
    expect(countSmaller({1, 2, 2, 1}), {0, 1, 1, 0},
           "duplicates both sides -> [0,1,1,0]");
  }

  {
    // Negative values exercise the compression path (raw values are not
    // usable as Fenwick indices).
    expect(countSmaller({-1, -5, 0, -3}), {2, 0, 1, 0},
           "negatives mixed -> [2,0,1,0]");
  }

  {
    // Monotone arrays: worst cases for naive structures, trivially linear
    // answers here.
    expect(countSmaller({1, 2, 3, 4, 5}), {0, 0, 0, 0, 0},
           "ascending -> all zeros");
    expect(countSmaller({5, 4, 3, 2, 1}), {4, 3, 2, 1, 0},
           "descending -> full inversion counts");
  }

  {
    // Single element boundary.
    expect(countSmaller({42}), {0}, "single element -> [0]");
  }

  {
    // Randomized-style fixed battery: cross-check against brute force on
    // assorted arrays (values include negatives and duplicates).
    std::vector<std::vector<int>> batteries = {
        {9, 1, 8, 2, 7, 3},
        {-100000, 100000, -50000, 0, 99999},
        {3, 3, 1, 1, 2, 2},
        {10},
        {7, 7},
        {4, 99, -2, 4, 0, 99, -50}};
    bool all_match = true;
    for (size_t t = 0; t < batteries.size(); ++t) {
      if (countSmaller(batteries[t]) != countSmallerBrute(batteries[t])) {
        all_match = false;
      }
    }
    check(all_match, "battery of 6 arrays matches O(n^2) brute force");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
