// ============================================================================
// LeetCode 307 — Range Sum Query - Mutable
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums`, handle the following two kinds of calls on a
// class:
//   - update(i, val): set nums[i] to val.
//   - sumRange(l, r): return the sum of nums[l..r] inclusive.
// Both operations may be called many times, interleaved arbitrarily.
//
// Example: nums = [1, 3, 5]
//   sumRange(0, 2) -> 9
//   update(1, 2)            (nums becomes [1, 2, 5])
//   sumRange(0, 2) -> 8
//
// APPROACH — Fenwick Tree (Binary Indexed Tree)
// ---------------------------------------------
// A plain prefix-sum array answers sumRange in O(1), but one update invalidates
// every prefix after it — O(n) per update. With updates interleaved into the
// query stream, that degenerates to O(n * q). The Fenwick Tree fixes exactly
// this asymmetry: it stores PARTIAL block sums at bit-trick-chosen indices so
// that both operations touch only O(log n) slots.
//
// Why it works: internal slot i owns a run of lowbit(i) = i & (-i) elements
// ending at i. A prefix [0..i] decomposes into disjoint owned runs (peeled off
// by repeatedly clearing the lowest set bit during query), and an update at
// position p is absorbed by precisely the O(log n) slots whose run contains p
// (found by repeatedly ADDING the lowest set bit during update).
//
// The one subtlety here: LeetCode's update is an ASSIGNMENT ("set to val"),
// while a Fenwick Tree natively supports DELTA updates ("add d"). We bridge
// the gap by keeping the current value of each position and sending
// delta = val - cur[i] into the tree. Forgetting this and pushing `val`
// directly is the classic wrong answer for this problem.
//
// COMPLEXITY
// ----------
// Time:  O(log n) per update and per sumRange.
// Space: O(n) for the tree plus O(n) for the current-value mirror array.
//
// Contrast with brute force (recompute the range sum per query): O(n) per
// query, O(1) per update — right trade only when queries vastly outnumber
// updates. Contrast with a full segment tree: same asymptotics but ~3x the
// code; Fenwick wins whenever the aggregate is invertible (sum is).
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// Fenwick Tree over positions 0..n-1 externally, 1-indexed internally.
// Stores long long throughout: with n up to 3*10^4 and values up to 10^9 in
// magnitude, any prefix sum can reach ~3*10^13 — far beyond int range.
class FenwickTree {
 public:
  explicit FenwickTree(int size) : tree_(size + 1, 0), n_(size) {}

  // Add `delta` at 0-indexed position i.
  void update(int i, long long delta) {
    // Convert to 1-indexed: slot 0 does not exist, and lowbit(0) == 0 would
    // make the climb loop forever.
    for (++i; i <= n_; i += i & (-i)) tree_[i] += delta;
  }

  // Prefix sum of 0-indexed positions [0..i].
  long long query(int i) const {
    long long sum = 0;
    for (++i; i > 0; i -= i & (-i)) sum += tree_[i];
    return sum;
  }

  // Sum over 0-indexed positions [l..r]. Valid because sum is invertible:
  // prefix(r) - prefix(l-1) cancels everything left of l.
  long long rangeSum(int l, int r) const {
    return query(r) - (l > 0 ? query(l - 1) : 0);
  }

 private:
  std::vector<long long> tree_;
  int n_;
};

class NumArray {
 public:
  explicit NumArray(const std::vector<int>& nums)
      : cur_(nums.begin(), nums.end()), ft_(static_cast<int>(nums.size())) {
    // Build the tree incrementally: each build-update is O(log n), so total
    // construction is O(n log n). (An O(n) linear build exists but obscures
    // the pattern being taught.)
    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
      ft_.update(i, nums[i]);
    }
  }

  void update(int index, int val) {
    // Assignment -> delta conversion: push only the CHANGE into the tree.
    long long delta = static_cast<long long>(val) - cur_[index];
    cur_[index] = val;
    if (delta != 0) ft_.update(index, delta);
  }

  int sumRange(int left, int right) const {
    return static_cast<int>(ft_.rangeSum(left, right));
  }

 private:
  std::vector<long long> cur_;  // mirror of current values, for deltas
  FenwickTree ft_;
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

  {
    // Canonical example from the problem statement.
    NumArray na({1, 3, 5});
    check(na.sumRange(0, 2) == 9, "[1,3,5]: sumRange(0,2) -> 9");
    na.update(1, 2);  // [1,3,5] -> [1,2,5]
    check(na.sumRange(0, 2) == 8, "after update(1,2): sumRange(0,2) -> 8");
    check(na.sumRange(1, 2) == 7, "after update(1,2): sumRange(1,2) -> 7");
  }

  {
    // Single-element array: exercises the smallest possible tree (one slot).
    NumArray na({7});
    check(na.sumRange(0, 0) == 7, "single element: sumRange(0,0) -> 7");
    na.update(0, -3);  // assignment crossing zero: delta must be -10
    check(na.sumRange(0, 0) == -3,
          "single element after assignment across zero -> -3");
  }

  {
    // Negative values and repeated assignments to the SAME value (delta 0
    // must not corrupt anything even though we skip the tree write).
    NumArray na({-1, -2, -3, -4});
    check(na.sumRange(0, 3) == -10, "all negatives: full sum -> -10");
    na.update(2, -3);  // assign the value it already has: delta == 0
    check(na.sumRange(0, 3) == -10, "no-op assignment leaves sum at -10");
    na.update(0, 100);
    check(na.sumRange(0, 1) == 98 && na.sumRange(2, 3) == -7,
          "update splits prefix correctly: 98 and -7");
  }

  {
    // Interleaved stress against a naive recompute oracle: every Fenwick
    // answer must match the brute-force sum of the mirrored array.
    std::vector<int> nums = {5, -9, 12, 0, 33, -7, 8, 8, -20, 4};
    NumArray na(nums);
    bool all_match = true;
    na.update(3, 50);   // 0 -> 50
    na.update(8, 6);    // -20 -> 6
    na.update(0, -5);   // 5 -> -5
    nums[3] = 50; nums[8] = 6; nums[0] = -5;

    for (int l = 0; l < static_cast<int>(nums.size()); ++l) {
      long long brute = 0;
      for (int r = l; r < static_cast<int>(nums.size()); ++r) {
        brute += nums[r];
        if (na.sumRange(l, r) != static_cast<int>(brute)) all_match = false;
      }
    }
    check(all_match, "interleaved updates: all 55 ranges match brute force");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
