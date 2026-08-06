// ============================================================================
// LeetCode 703 — Kth Largest Element in a Stream
// ============================================================================
//
// PROBLEM
// -------
// Design a class KthLargest that, given an integer k and an initial array of
// integers `nums`, supports adding new integers to the stream one at a time
// via `add(int val)`, returning the k-th largest element in the stream after
// each addition. There are always at least k elements in the stream when
// `add` is called (after any needed initial elements have been added).
//
// Example: k = 3, nums = [4, 5, 8, 2]
//   add(3)  -> stream [4,5,8,2,3], 3rd largest is 4
//   add(5)  -> stream [4,5,8,2,3,5], 3rd largest is 5
//   add(10) -> stream [4,5,8,2,3,5,10], 3rd largest is 5
//   add(9)  -> stream [4,5,8,2,3,5,10,9], 3rd largest is 8
//   add(4)  -> stream [4,5,8,2,3,5,10,9,4], 3rd largest is 8
//
// APPROACH — single min-heap of size k (the one-heap warm-up before Two Heaps)
// ------------------------------------------------------------------------
// This problem is included in the Two Heaps module as the simplest possible
// heap application, deliberately studied BEFORE the two-heap problems (see
// the recommended reading order in problems/README.md), because it shows the
// "one heap tracks one boundary value" idea in isolation before Two Heaps
// generalizes it to TWO simultaneous boundaries (the median's lower/upper
// split).
//
// The key insight: to know the k-th LARGEST element at all times, you do not
// need to remember every element ever added, sorted. You only need to keep
// the k LARGEST elements seen so far, and among those k, the SMALLEST one is
// exactly the k-th largest overall (everything smaller than it has already
// been correctly excluded; everything in the heap is one of the top k).
//
// A min-heap capped at size k does exactly this:
//   - If the heap has fewer than k elements, always push the new value (we
//     have not seen enough elements yet to exclude anything).
//   - Once the heap has k elements, a new value only matters if it is larger
//     than the heap's current top (the smallest of the current top k) — if
//     so, it replaces that top (pop the old top, push the new value); a
//     value <= the current top cannot be among the top k, so it is ignored.
//   - The heap's top is always the answer: the smallest of the k largest
//     elements seen so far is, by definition, the k-th largest overall.
//
// Contrast with Two Heaps (problems/01): here `k` is a FIXED, small count
// that does not grow with the stream, so one bounded heap suffices. The
// median instead needs a boundary that grows proportionally with the whole
// stream (always "half of everything so far"), which is why the median
// needs two heaps balanced against each other rather than one heap capped
// at a constant size. See the README's Similar Patterns comparison table.
//
// COMPLEXITY
// ----------
// Time:  O(log k) per add() call — one heap push, and at most one heap pop.
//        Constructor: O(n log k) for n initial elements (n calls to add()).
// Space: O(k) — the heap never holds more than k elements at a time.
// ============================================================================

#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

class KthLargest {
 public:
  KthLargest(int k, const std::vector<int>& nums) : k_(k) {
    for (int num : nums) {
      add(num);
    }
  }

  // Adds val to the stream and returns the k-th largest element so far.
  int add(int val) {
    if (static_cast<int>(min_heap_.size()) < k_) {
      // Haven't seen k elements yet: always keep this one.
      min_heap_.push(val);
    } else if (val > min_heap_.top()) {
      // val beats the current smallest of the top k -> it belongs in the
      // top k, and the old smallest no longer does.
      min_heap_.pop();
      min_heap_.push(val);
    }
    // Else: val is <= the current k-th largest, so it cannot be among the
    // top k; do nothing.

    if (min_heap_.empty()) {
      throw std::logic_error("add() called with k <= 0 or no elements retained.");
    }
    return min_heap_.top();
  }

 private:
  int k_;
  // Min-heap holding (at most) the k largest elements seen so far. Its top
  // is always the smallest of those k, i.e. the k-th largest overall.
  std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap_;
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
    // Classic LeetCode example.
    std::vector<int> nums = {4, 5, 8, 2};
    KthLargest kth(3, nums);

    check(kth.add(3) == 4, "add(3) -> 4");
    check(kth.add(5) == 5, "add(5) -> 5");
    check(kth.add(10) == 5, "add(10) -> 5");
    check(kth.add(9) == 8, "add(9) -> 8");
    check(kth.add(4) == 8, "add(4) -> 8");
  }

  {
    // k = 1: the "k-th largest" is just the running maximum.
    std::vector<int> nums = {};
    KthLargest kth(1, nums);
    check(kth.add(-3) == -3, "k=1, add(-3) -> -3");
    check(kth.add(-2) == -2, "k=1, add(-2) -> -2");
    check(kth.add(-4) == -2, "k=1, add(-4) -> -2 (unchanged, -4 not larger)");
    check(kth.add(0) == 0, "k=1, add(0) -> 0");
    check(kth.add(4) == 4, "k=1, add(4) -> 4");
  }

  {
    // Starting with exactly k initial elements already.
    std::vector<int> nums = {3, 1, 5};
    KthLargest kth(2, nums);
    // Top 2 so far: {3, 5}, 2nd largest = 3.
    check(kth.add(2) == 3, "k=2, initial [3,1,5], add(2) -> 3 (top2={3,5})");
    check(kth.add(10) == 5, "k=2, add(10) -> 5 (top2={5,10})");
    check(kth.add(9) == 9, "k=2, add(9) -> 9 (top2={9,10})");
  }

  {
    // Duplicate values should be handled like any other value.
    std::vector<int> nums = {8, 8, 8};
    KthLargest kth(2, nums);
    // Top 2 so far: {8, 8}, 2nd largest = 8.
    check(kth.add(8) == 8, "k=2, all duplicates -> 8");
    check(kth.add(1) == 8, "k=2, add smaller duplicate-ish value -> still 8");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
