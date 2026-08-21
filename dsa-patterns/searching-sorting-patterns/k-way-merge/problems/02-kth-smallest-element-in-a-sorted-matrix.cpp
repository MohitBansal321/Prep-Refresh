// ============================================================================
// LeetCode 378 — Kth Smallest Element in a Sorted Matrix (Medium)
// ============================================================================
//
// PROBLEM
// -------
// Given an n x n `matrix` where each of the rows and columns is sorted in
// ascending order, return the kth smallest element in the matrix.
//
// Example:
//   matrix = [[1, 5, 9],
//             [10,11,13],
//             [12,13,15]]
//   k = 8  ->  13
//
// APPROACH — K-way Merge in disguise (rows are the "K sorted lists")
// ---------------------------------------------------------------------------
// The key recognition: a matrix whose ROWS are each individually sorted
// ascending is EXACTLY "K already-sorted lists" with K = number of rows.
// (Column-sortedness is not even required for this particular approach —
// only row-sortedness is used — though the problem guarantees both.)
//
// This is the K-th-smallest variant of K-way Merge described in the
// README's Execution Flow step 5 and implemented generically in
// ../code.cpp's kthSmallestInKSortedLists: seed a min-heap with the first
// (leftmost) element of every row, tagged with (row, col), then pop the
// smallest k times, pushing each popped entry's row-neighbor back in. The
// k-th pop is the answer — no need to merge the remaining elements.
//
// Heap entries: {value, row, col}. Popping (value, row, col) means "row
// `row`'s current smallest unconsumed element is now accounted for"; its
// replacement, if one exists, is matrix[row][col+1] — the next element in
// THAT SAME ROW (moving right stays within the row's own sorted order;
// there is no need to look at any other row's columns).
//
// COMPLEXITY
// ----------
// Let n = number of rows = number of columns (matrix is n x n), so the
// matrix holds n*n total elements, and K (number of "lists") = n (number
// of rows).
// Time:  O(k log n) — at most k pops/pushes (early exit at the k-th pop),
//        each an O(log n) heap operation since the heap never holds more
//        than n entries (one per row) at a time. In the worst case k = n^2,
//        giving O(n^2 log n) to fully drain the matrix.
// Space: O(n) for the heap — bounded by the number of rows, not by k or by
//        the total element count n^2.
//
// Contrast with brute force (flatten the whole matrix into one array, sort
// it, index into position k-1): O(n^2 log(n^2)) time and O(n^2) space —
// this discards the fact that every row already arrives sorted, exactly
// the wasted-information problem described in the README's Problem section.
// ============================================================================

#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

using HeapEntry = std::tuple<int, int, int>;  // {value, row, col}
using MinHeap = std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<>>;

int kthSmallest(const std::vector<std::vector<int>>& matrix, int k) {
  const int n = static_cast<int>(matrix.size());
  MinHeap heap;

  // Seed: the leftmost (smallest) element of every row. Each row is one
  // "sorted list" in the K-way Merge sense; K = n (the number of rows).
  for (int row = 0; row < n && row < static_cast<int>(matrix.size()); ++row) {
    if (!matrix[row].empty()) {
      heap.emplace(matrix[row][0], row, 0);
    }
  }

  int popped_count = 0;
  while (!heap.empty()) {
    // Unpack the tuple field by field with std::get<> rather than a C++17
    // structured binding, so this file compiles on older toolchains too.
    const HeapEntry top = heap.top();
    const int value = std::get<0>(top);
    const int row = std::get<1>(top);
    const int col = std::get<2>(top);
    heap.pop();
    ++popped_count;

    if (popped_count == k) {
      return value;  // Early exit: the k-th pop IS the k-th smallest.
    }

    // Advance within the SAME row: the next candidate from this row is
    // one column to the right, still guaranteed >= everything already
    // popped from this row (row-sortedness is what makes this safe).
    const int next_col = col + 1;
    if (next_col < static_cast<int>(matrix[row].size())) {
      heap.emplace(matrix[row][next_col], row, next_col);
    }
  }

  // Per problem constraints, 1 <= k <= n^2, so this should be unreachable
  // for valid input; return a sentinel to make the "impossible" path explicit
  // rather than falling through to undefined behavior.
  return -1;
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

  // ---- Test 1: the classic LeetCode example -----------------------------------
  {
    std::vector<std::vector<int>> matrix = {
        {1, 5, 9},
        {10, 11, 13},
        {12, 13, 15},
    };
    check(kthSmallest(matrix, 8) == 13, "3x3 matrix, k=8 -> 13");
  }

  // ---- Test 2: k = 1 (smallest element overall, top-left in a sorted matrix) -
  {
    std::vector<std::vector<int>> matrix = {
        {1, 5, 9},
        {10, 11, 13},
        {12, 13, 15},
    };
    check(kthSmallest(matrix, 1) == 1, "3x3 matrix, k=1 -> 1 (smallest overall)");
  }

  // ---- Test 3: k = n*n (largest element overall) ------------------------------
  {
    std::vector<std::vector<int>> matrix = {
        {1, 5, 9},
        {10, 11, 13},
        {12, 13, 15},
    };
    check(kthSmallest(matrix, 9) == 15, "3x3 matrix, k=9 -> 15 (largest overall)");
  }

  // ---- Test 4: single-element matrix -------------------------------------------
  {
    std::vector<std::vector<int>> matrix = {{5}};
    check(kthSmallest(matrix, 1) == 5, "1x1 matrix, k=1 -> 5");
  }

  // ---- Test 5: matrix with duplicate values ------------------------------------
  {
    std::vector<std::vector<int>> matrix = {
        {1, 2},
        {1, 3},
    };
    check(kthSmallest(matrix, 2) == 1, "2x2 matrix with duplicates, k=2 -> 1");
    check(kthSmallest(matrix, 3) == 2, "2x2 matrix with duplicates, k=3 -> 2");
  }

  // ---- Test 6: larger 4x4 matrix -------------------------------------------------
  {
    std::vector<std::vector<int>> matrix = {
        {1, 3, 5, 7},
        {2, 4, 6, 8},
        {3, 5, 7, 9},
        {4, 6, 8, 10},
    };
    // Sorted flatten: 1,2,3,3,4,4,5,5,6,6,7,7,8,8,9,10 -> 6th smallest is 4.
    check(kthSmallest(matrix, 6) == 4, "4x4 matrix, k=6 -> 4");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
