// ============================================================================
// LeetCode 295 — Find Median from Data Stream
// ============================================================================
//
// PROBLEM
// -------
// Design a data structure that supports:
//   - addNum(int num): adds an integer to the data structure.
//   - findMedian(): returns the median of all elements added so far.
// The median of a sorted list of n numbers is the middle element if n is
// odd, or the average of the two middle elements if n is even.
//
// Example: addNum(1), addNum(2) -> findMedian() = 1.5
//          addNum(3)             -> findMedian() = 2
//
// APPROACH — Two Heaps (this module's namesake pattern)
// -------------------------------------------------------
// This is the canonical Two Heaps problem — see ../README.md for the full
// conceptual treatment. In short:
//
//   - `low` is a MAX-HEAP holding the smaller half of every number inserted
//     so far. Its top() is always the largest of that lower half.
//   - `high` is a MIN-HEAP holding the larger half of every number inserted
//     so far. Its top() is always the smallest of that upper half.
//
// Two invariants are maintained after EVERY addNum() call:
//   1. VALUE invariant: everything in `low` is <= everything in `high`.
//   2. SIZE invariant: |low.size() - high.size()| <= 1, with `low` holding
//      the extra element when the total count is odd (a fixed convention
//      applied consistently in both addNum and findMedian).
//
// Given both invariants, findMedian() never touches a heap operation: if
// sizes are equal, the median is the average of both tops; otherwise it is
// simply low.top().
//
// This file implements the exact same logic as ../code.cpp's MedianFinder
// class, but standalone (no #include of the module's code.cpp) so it reads
// as a complete, independent solution matching LeetCode's expected class
// shape (`MedianFinder`, `addNum`, `findMedian`).
//
// COMPLEXITY
// ----------
// Time:  addNum()     -> O(log n) (one heap push, at most one rebalance
//                         pop-and-push).
//        findMedian()  -> O(1) (reads at most two heap tops).
// Space: O(n) — every inserted number lives in exactly one heap.
//
// Contrast with re-sorting on every query (O(n log n) per query) or a
// sorted array with shift-insert (O(n) per insert) -- see ../README.md's
// Complexity section for the full comparison table.
// ============================================================================

#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

class MedianFinder {
 public:
  MedianFinder() = default;

  void addNum(int num) {
    // Route the new number into the correct half.
    if (low_.empty() || num <= low_.top()) {
      low_.push(num);
    } else {
      high_.push(num);
    }

    // Rebalance sizes, unconditionally, every call.
    if (low_.size() > high_.size() + 1) {
      high_.push(low_.top());
      low_.pop();
    } else if (high_.size() > low_.size()) {
      low_.push(high_.top());
      high_.pop();
    }
  }

  double findMedian() const {
    if (low_.empty() && high_.empty()) {
      throw std::logic_error("findMedian() called with no numbers inserted.");
    }
    if (low_.size() == high_.size()) {
      return (static_cast<double>(low_.top()) + static_cast<double>(high_.top())) / 2.0;
    }
    return static_cast<double>(low_.top());
  }

 private:
  std::priority_queue<int> low_;                                          // max-heap
  std::priority_queue<int, std::vector<int>, std::greater<int>> high_;    // min-heap
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

  auto approx_equal = [](double a, double b) {
    return (a > b ? a - b : b - a) < 1e-9;
  };

  {
    // Classic LeetCode example.
    MedianFinder mf;
    mf.addNum(1);
    mf.addNum(2);
    check(approx_equal(mf.findMedian(), 1.5), "addNum(1), addNum(2) -> median 1.5");
    mf.addNum(3);
    check(approx_equal(mf.findMedian(), 2.0), "addNum(3) -> median 2.0");
  }

  {
    // Strictly increasing sequence: catches a swapped min/max-heap bug
    // immediately, since the median must track the middle of 1..n exactly.
    MedianFinder mf;
    for (int i = 1; i <= 7; ++i) {
      mf.addNum(i);
    }
    // 1..7 -> sorted is itself, median is 4.
    check(approx_equal(mf.findMedian(), 4.0), "addNum 1..7 in order -> median 4.0");
  }

  {
    // Decreasing sequence (opposite direction from above).
    MedianFinder mf;
    for (int i = 7; i >= 1; --i) {
      mf.addNum(i);
    }
    check(approx_equal(mf.findMedian(), 4.0), "addNum 7..1 in order -> median 4.0");
  }

  {
    // All identical values.
    MedianFinder mf;
    for (int i = 0; i < 5; ++i) {
      mf.addNum(42);
    }
    check(approx_equal(mf.findMedian(), 42.0), "five copies of 42 -> median 42.0");
  }

  {
    // Negative numbers mixed with positive, even final count.
    MedianFinder mf;
    for (int v : {-10, 5, -3, 8}) {
      mf.addNum(v);
    }
    // sorted: [-10, -3, 5, 8] -> median = (-3 + 5) / 2 = 1.0
    check(approx_equal(mf.findMedian(), 1.0), "[-10,5,-3,8] -> median 1.0");
  }

  {
    // Empty stream should throw rather than silently return garbage.
    MedianFinder mf;
    bool threw = false;
    try {
      mf.findMedian();
    } catch (const std::logic_error&) {
      threw = true;
    }
    check(threw, "findMedian() with no inserts -> throws std::logic_error");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
