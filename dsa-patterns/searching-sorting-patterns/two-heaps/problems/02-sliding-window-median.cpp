// ============================================================================
// LeetCode 480 — Sliding Window Median
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums` and an integer `k`, there is a sliding
// window of size k moving from the very left to the very right of the
// array. For each window position, return the median of the k elements
// currently inside it.
//
// Example: nums = [1,3,-1,-3,5,3,6,7], k = 3
//   window [1,3,-1]  -> sorted [-1,1,3]  -> median 1
//   window [3,-1,-3] -> sorted [-3,-1,3] -> median -1
//   window [-1,-3,5] -> sorted [-3,-1,5] -> median -1
//   window [-3,5,3]  -> sorted [-3,3,5]  -> median 3
//   window [5,3,6]   -> sorted [3,5,6]   -> median 5
//   window [3,6,7]   -> sorted [3,6,7]   -> median 6
//
// APPROACH — Two Heaps + LAZY DELETION (the hard extension)
// -------------------------------------------------------------
// This is the same two-heap core as problem 01 (max-heap for the lower
// half, min-heap for the upper half, balanced in size), but with one
// fundamental new requirement: as the window slides, the OLDEST element
// must leave the structure, not just new elements arrive. Plain
// std::priority_queue cannot remove an arbitrary element cheaply -- it can
// only pop its root. That mismatch is exactly why this problem is rated
// hard, and exactly why it belongs last in this module's reading order
// (see problems/README.md).
//
// The fix is LAZY DELETION:
//   - Keep a hash map `pending_removals` counting values that have
//     conceptually left the window but have not yet been physically
//     popped from whichever heap they happen to sit in.
//   - Before every rebalance step and every median read, "clean" the top
//     of each heap: while a heap's top value has a pending removal count
//     > 0, pop it and decrement its pending-removal count. This lazily
//     discards stale values only when they would otherwise be read or
//     interfere with the size/value invariants -- not immediately when
//     they expire, which would require an O(n) heap scan to find them.
//   - Sizes are tracked with separate integer counters (`low_size`,
//     `high_size`) representing the LOGICAL size of each half (excluding
//     values that are pending removal but not yet popped), because the
//     heaps' own .size() would otherwise overcount stale entries still
//     sitting inside them.
//
// Step by step, for each new window position:
//   1. If the window already has k elements (i.e. this is not the very
//      first window), the element sliding OUT of the window is marked in
//      `pending_removals`, its logical heap's size counter is decremented,
//      and (if needed) sizes are rebalanced to restore the invariant.
//   2. The new incoming element is routed into low_ or high_ exactly as in
//      problem 01, and the logical size counters are updated, followed by
//      the same size-rebalancing rule as problem 01.
//   3. Before reading the median, both heaps are "cleaned" (pending-removed
//      stale tops popped) so their .top() calls return live data.
//
// Why lazy deletion is safe: a value marked pending-removal is guaranteed
// to eventually surface at the top of whichever heap it sits in (a heap's
// root is always its extreme value, and once every "smaller"/"larger"
// value that could sit above it in heap order has itself been popped, the
// stale value becomes the new top and gets cleaned away then). Because we
// always clean before rebalancing or reading, the invariants problem 01
// relies on (value split + size balance) are restored using the LOGICAL
// sizes before they are ever trusted.
//
// COMPLEXITY
// ----------
// Time:  O(n log k) for n elements and window size k -- each element is
//        pushed once (O(log k)) and popped at most once, either during a
//        normal rebalance or during lazy cleanup (O(log k) each).
// Space: O(k) for the two heaps' live contents, plus O(k) worst case for
//        pending_removals (bounded by how many stale entries can exist
//        before being cleaned).
// ============================================================================

#include <cmath>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

class SlidingWindowMedian {
 public:
  explicit SlidingWindowMedian(int k) : k_(k) {}

  // Processes the full array and returns the median of every window of
  // size k_, in order.
  std::vector<double> medianSlidingWindow(const std::vector<int>& nums) {
    std::vector<double> result;
    result.reserve(nums.size() >= static_cast<size_t>(k_) ? nums.size() - k_ + 1 : 0);

    for (size_t i = 0; i < nums.size(); ++i) {
      addNum(nums[i]);

      if (i >= static_cast<size_t>(k_)) {
        removeNum(nums[i - k_]);
      }

      if (i >= static_cast<size_t>(k_) - 1) {
        result.push_back(currentMedian());
      }
    }

    return result;
  }

 private:
  int k_;
  std::priority_queue<long long> low_;                                                   // max-heap
  std::priority_queue<long long, std::vector<long long>, std::greater<long long>> high_;  // min-heap
  int low_size_ = 0;   // logical size of low_, excluding pending removals
  int high_size_ = 0;  // logical size of high_, excluding pending removals
  std::unordered_map<long long, int> pending_removals_;

  // Pops the top of low_ (or high_) while it is a value pending removal.
  void cleanTop(std::priority_queue<long long>& heap) {
    while (!heap.empty()) {
      auto it = pending_removals_.find(heap.top());
      if (it == pending_removals_.end() || it->second == 0) break;
      --it->second;
      heap.pop();
    }
  }

  void cleanTop(std::priority_queue<long long, std::vector<long long>, std::greater<long long>>& heap) {
    while (!heap.empty()) {
      auto it = pending_removals_.find(heap.top());
      if (it == pending_removals_.end() || it->second == 0) break;
      --it->second;
      heap.pop();
    }
  }

  void cleanBoth() {
    cleanTop(low_);
    cleanTop(high_);
  }

  void rebalance() {
    cleanBoth();
    if (low_size_ > high_size_ + 1) {
      high_.push(low_.top());
      low_.pop();
      --low_size_;
      ++high_size_;
      cleanBoth();
    } else if (high_size_ > low_size_) {
      low_.push(high_.top());
      high_.pop();
      --high_size_;
      ++low_size_;
      cleanBoth();
    }
  }

  void addNum(long long num) {
    cleanBoth();
    if (low_size_ == 0 || num <= low_.top()) {
      low_.push(num);
      ++low_size_;
    } else {
      high_.push(num);
      ++high_size_;
    }
    rebalance();
  }

  void removeNum(long long num) {
    // Decide which half this value logically belongs to FIRST, before any
    // popping happens. This must be captured up front: if we cleaned the
    // heaps (or popped anything) before making this decision, and `num`
    // itself happened to be sitting on top of low_, cleaning would pop it
    // and shift low_.top() to a different value -- so the routing check
    // below would compare against the wrong reference point and could
    // decrement the wrong side's size counter, silently corrupting the
    // low_size_/high_size_ invariant until a later top() call hits an
    // empty heap.
    bool from_low = !low_.empty() && num <= low_.top();

    // Mark this value as pending removal so it gets popped the next time
    // it surfaces at the top of whichever physical heap it sits in.
    ++pending_removals_[num];

    if (from_low) {
      --low_size_;
    } else {
      --high_size_;
    }

    rebalance();
  }

  double currentMedian() {
    cleanBoth();
    if (low_size_ == high_size_) {
      return (static_cast<double>(low_.top()) + static_cast<double>(high_.top())) / 2.0;
    }
    return static_cast<double>(low_.top());
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

  auto approx_equal_vec = [](const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
      if (std::fabs(a[i] - b[i]) > 1e-9) return false;
    }
    return true;
  };

  {
    // Classic LeetCode example.
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    SlidingWindowMedian swm(3);
    std::vector<double> expected = {1.0, -1.0, -1.0, 3.0, 5.0, 6.0};
    auto result = swm.medianSlidingWindow(nums);
    check(approx_equal_vec(result, expected),
          "nums=[1,3,-1,-3,5,3,6,7], k=3 -> [1,-1,-1,3,5,6]");
  }

  {
    // Even window size -> every median is an average of two values.
    std::vector<int> nums = {1, 2, 3, 4, 2, 3, 1, 4, 2};
    SlidingWindowMedian swm(2);
    // Windows: [1,2]->1.5 [2,3]->2.5 [3,4]->3.5 [4,2]->3.0 [2,3]->2.5
    //          [3,1]->2.0 [1,4]->2.5 [4,2]->3.0
    std::vector<double> expected = {1.5, 2.5, 3.5, 3.0, 2.5, 2.0, 2.5, 3.0};
    auto result = swm.medianSlidingWindow(nums);
    check(approx_equal_vec(result, expected), "k=2 (even window) -> averaged medians");
  }

  {
    // Window size 1 -> median is just the element itself.
    std::vector<int> nums = {5, -2, 8, 0};
    SlidingWindowMedian swm(1);
    std::vector<double> expected = {5.0, -2.0, 8.0, 0.0};
    auto result = swm.medianSlidingWindow(nums);
    check(approx_equal_vec(result, expected), "k=1 -> median equals each element");
  }

  {
    // Window size equal to the full array -> a single median value.
    std::vector<int> nums = {4, 2, 7, 1};
    SlidingWindowMedian swm(4);
    // sorted [1,2,4,7] -> median (2+4)/2 = 3.0
    std::vector<double> expected = {3.0};
    auto result = swm.medianSlidingWindow(nums);
    check(approx_equal_vec(result, expected), "k == n -> single median over whole array");
  }

  {
    // Duplicate values sliding in and out repeatedly -- stresses lazy
    // deletion when the value being removed has copies still present.
    std::vector<int> nums = {2, 2, 2, 2, 2};
    SlidingWindowMedian swm(3);
    std::vector<double> expected = {2.0, 2.0, 2.0};
    auto result = swm.medianSlidingWindow(nums);
    check(approx_equal_vec(result, expected), "all-duplicate stream -> median stays 2.0");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
