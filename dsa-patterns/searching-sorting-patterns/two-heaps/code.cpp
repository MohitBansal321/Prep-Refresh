// ============================================================================
// Two Heaps — generic reusable MedianFinder (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem's input/output shape
// (though it directly implements LeetCode 295's contract). It demonstrates
// the core Two Heaps idea in its purest, most reusable form:
//
//   - A MAX-HEAP holds the "lower half" of everything inserted so far.
//     std::priority_queue<int> is a max-heap by default in C++ — its top()
//     is always the largest element currently stored.
//
//   - A MIN-HEAP holds the "upper half" of everything inserted so far.
//     std::priority_queue<int, std::vector<int>, std::greater<int>> flips
//     the default ordering, so its top() is always the smallest element
//     currently stored.
//
// Both heaps are kept balanced in size (never differing by more than one
// element) after every insertion, which is what makes the median always
// readable in O(1) from just the two heaps' top elements.
//
// The worked, problem-specific solutions (including the harder sliding-
// window variant with lazy deletion) live in problems/*.cpp.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// MedianFinder — maintains the running median of a stream of integers.
//
// Invariants maintained after every call to addNum():
//   1. VALUE invariant: every element in `low_` is <= every element in
//      `high_`. This is what makes "the boundary between the two heaps" the
//      same thing as "the middle of the sorted-order of everything seen."
//   2. SIZE invariant: low_.size() is either equal to high_.size(), or
//      exactly one greater. By convention, when the total count is odd,
//      `low_` is the heap that holds the extra element.
//
// Given both invariants, findMedian() never needs to look at anything other
// than the two heaps' top elements.
// ----------------------------------------------------------------------------
class MedianFinder {
 public:
  MedianFinder() = default;

  // Inserts a new number into the running stream in O(log n).
  void addNum(int num) {
    // --- Step 1: route the new number into the correct half. ---
    //
    // If low_ is empty, there is nothing to compare against yet, so the
    // first element always starts in low_ (an arbitrary but consistent
    // choice — either heap could hold the very first element).
    //
    // Otherwise: if num belongs in the lower half (it is <= the current
    // largest element of the lower half), it goes into low_. Otherwise it
    // belongs in the upper half, so it goes into high_.
    if (low_.empty() || num <= low_.top()) {
      low_.push(num);
    } else {
      high_.push(num);
    }

    // --- Step 2: rebalance sizes. ---
    //
    // Routing alone does not guarantee the size invariant — for example, a
    // long run of small numbers would all land in low_, growing it far
    // larger than high_. This step restores |low_.size() - high_.size()| <= 1
    // after every single insertion, unconditionally.
    if (low_.size() > high_.size() + 1) {
      // low_ grew two elements ahead of high_: move its top (the largest of
      // the lower half) across to high_. This is still value-correct: that
      // element is >= everything remaining in low_ and, by the value
      // invariant that held before this insert, <= everything in high_.
      high_.push(low_.top());
      low_.pop();
    } else if (high_.size() > low_.size()) {
      // high_ grew strictly larger than low_: move its top (the smallest of
      // the upper half) across to low_, by the symmetric argument.
      low_.push(high_.top());
      high_.pop();
    }
  }

  // Returns the median of every number inserted so far in O(1).
  // Throws std::logic_error if no numbers have been inserted yet.
  double findMedian() const {
    if (low_.empty() && high_.empty()) {
      throw std::logic_error("findMedian() called with no numbers inserted.");
    }

    if (low_.size() == high_.size()) {
      // Even total count: the median sits exactly between the two halves,
      // i.e. the average of both boundary values.
      return (static_cast<double>(low_.top()) + static_cast<double>(high_.top())) / 2.0;
    }

    // Odd total count: by our convention, low_ holds the extra element, so
    // its top IS the single middle element.
    return static_cast<double>(low_.top());
  }

  // Returns how many numbers have been inserted so far (useful for tests).
  size_t size() const { return low_.size() + high_.size(); }

 private:
  // Max-heap: top() is always the largest element of the lower half.
  std::priority_queue<int> low_;

  // Min-heap: top() is always the smallest element of the upper half.
  std::priority_queue<int, std::vector<int>, std::greater<int>> high_;
};

// ============================================================================
// main() — demonstrates MedianFinder with printed, verifiable output.
// ============================================================================
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

  std::cout << "--- Odd/even progression: 5, then 15, then 1 ---\n";
  {
    MedianFinder mf;
    mf.addNum(5);
    check(approx_equal(mf.findMedian(), 5.0), "after [5] -> median 5.0");

    mf.addNum(15);
    check(approx_equal(mf.findMedian(), 10.0), "after [5,15] -> median 10.0");

    mf.addNum(1);
    check(approx_equal(mf.findMedian(), 5.0), "after [5,15,1] -> median 5.0");
  }

  std::cout << "\n--- Longer sequence requiring several rebalances ---\n";
  {
    // Stream: 5, 15, 1, 3, 8, 7, 9, 10
    // Sorted progressively: [5] [5,15] [1,5,15] [1,3,5,15] [1,3,5,8,15]
    //                       [1,3,5,7,8,15] [1,3,5,7,8,9,15] [1,3,5,7,8,9,10,15]
    // Medians:                5    10      5       4         5
    //                         6       7        7.5
    MedianFinder mf;
    std::vector<int> stream = {5, 15, 1, 3, 8, 7, 9, 10};
    std::vector<double> expected_medians = {5.0, 10.0, 5.0, 4.0, 5.0, 6.0, 7.0, 7.5};

    for (size_t i = 0; i < stream.size(); ++i) {
      mf.addNum(stream[i]);
      double median = mf.findMedian();
      check(approx_equal(median, expected_medians[i]),
            "insert " + std::to_string(stream[i]) + " -> median " +
                std::to_string(expected_medians[i]));
    }
  }

  std::cout << "\n--- Negative numbers and duplicates ---\n";
  {
    MedianFinder mf;
    mf.addNum(-5);
    mf.addNum(-5);
    mf.addNum(0);
    check(approx_equal(mf.findMedian(), -5.0), "[-5,-5,0] -> median -5.0");

    mf.addNum(10);
    check(approx_equal(mf.findMedian(), -2.5), "[-5,-5,0,10] -> median -2.5");

    mf.addNum(-100);
    check(approx_equal(mf.findMedian(), -5.0), "[-100,-5,-5,0,10] -> median -5.0");
  }

  std::cout << "\n--- size() bookkeeping ---\n";
  {
    MedianFinder mf;
    check(mf.size() == 0, "fresh MedianFinder -> size 0");
    mf.addNum(1);
    mf.addNum(2);
    mf.addNum(3);
    check(mf.size() == 3, "after 3 inserts -> size 3");
  }

  std::cout << "\n--- Calling findMedian() with nothing inserted throws ---\n";
  {
    MedianFinder mf;
    bool threw = false;
    try {
      mf.findMedian();
    } catch (const std::logic_error&) {
      threw = true;
    }
    check(threw, "findMedian() on empty stream -> throws std::logic_error");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
