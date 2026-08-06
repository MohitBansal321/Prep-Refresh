// ============================================================================
// Segment Tree / Fenwick Tree — generic reusable template (C++17)
// ============================================================================
//
// FenwickTree: point update + prefix/range sum query, both O(log n).
// SegmentTree: point update + range MIN query, both O(log n) -- included to
// show the more general structure that Fenwick Tree specializes away from.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <climits>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Fenwick Tree (Binary Indexed Tree), 0-indexed externally, 1-indexed
// internally. update adds a delta at a position; query returns the prefix
// sum [0, i]; rangeSum derives [l, r] from two prefix queries.
// ----------------------------------------------------------------------------
class FenwickTree {
 public:
  explicit FenwickTree(int size) : tree_(size + 1, 0), n_(size) {}

  void update(int i, long long delta) {
    for (++i; i <= n_; i += i & (-i)) tree_[i] += delta;
  }

  long long query(int i) const {
    long long sum = 0;
    for (++i; i > 0; i -= i & (-i)) sum += tree_[i];
    return sum;
  }

  long long rangeSum(int l, int r) const {
    return query(r) - (l > 0 ? query(l - 1) : 0);
  }

 private:
  std::vector<long long> tree_;
  int n_;
};

// ----------------------------------------------------------------------------
// Segment Tree for range MINIMUM query with point updates. Stored as an
// implicit binary tree in an array sized 4*n (a safe upper bound for any n).
// ----------------------------------------------------------------------------
class SegmentTreeMin {
 public:
  explicit SegmentTreeMin(const std::vector<int>& nums)
      : n_(static_cast<int>(nums.size())), tree_(4 * n_, INT_MAX) {
    if (n_ > 0) build(nums, 1, 0, n_ - 1);
  }

  void update(int index, int value) { updateHelper(1, 0, n_ - 1, index, value); }

  int queryMin(int l, int r) const { return queryHelper(1, 0, n_ - 1, l, r); }

 private:
  int n_;
  std::vector<int> tree_;

  void build(const std::vector<int>& nums, int node, int lo, int hi) {
    if (lo == hi) {
      tree_[node] = nums[lo];
      return;
    }
    int mid = lo + (hi - lo) / 2;
    build(nums, 2 * node, lo, mid);
    build(nums, 2 * node + 1, mid + 1, hi);
    tree_[node] = std::min(tree_[2 * node], tree_[2 * node + 1]);
  }

  void updateHelper(int node, int lo, int hi, int index, int value) {
    if (lo == hi) {
      tree_[node] = value;
      return;
    }
    int mid = lo + (hi - lo) / 2;
    if (index <= mid) updateHelper(2 * node, lo, mid, index, value);
    else updateHelper(2 * node + 1, mid + 1, hi, index, value);
    tree_[node] = std::min(tree_[2 * node], tree_[2 * node + 1]);
  }

  int queryHelper(int node, int lo, int hi, int l, int r) const {
    if (r < lo || hi < l) return INT_MAX;          // no overlap
    if (l <= lo && hi <= r) return tree_[node];     // fully covered
    int mid = lo + (hi - lo) / 2;
    return std::min(queryHelper(2 * node, lo, mid, l, r),
                     queryHelper(2 * node + 1, mid + 1, hi, l, r));
  }
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
    FenwickTree ft(8);
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8};
    for (int i = 0; i < 8; ++i) ft.update(i, nums[i]);

    check(ft.rangeSum(0, 7) == 36, "Fenwick: full range sum -> 36");
    check(ft.rangeSum(2, 4) == 12, "Fenwick: rangeSum(2,4) -> 3+4+5=12");

    ft.update(2, 10);  // add 10 more at index 2 (value there becomes 3+10=13)
    check(ft.rangeSum(2, 4) == 22, "Fenwick: after update, rangeSum(2,4) -> 13+4+5=22");
    check(ft.rangeSum(0, 7) == 46, "Fenwick: full range sum after update -> 46");
  }

  {
    SegmentTreeMin st({5, 2, 8, 1, 9, 3});
    check(st.queryMin(0, 5) == 1, "SegmentTree: full range min -> 1");
    check(st.queryMin(0, 2) == 2, "SegmentTree: queryMin(0,2) -> 2");
    check(st.queryMin(4, 5) == 3, "SegmentTree: queryMin(4,5) -> 3");

    st.update(3, 100);  // the '1' at index 3 is no longer the minimum
    check(st.queryMin(0, 5) == 2, "SegmentTree: after update, full range min -> 2");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
