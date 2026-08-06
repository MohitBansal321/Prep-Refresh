// ============================================================================
// Prefix Sum — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// core "build once, query many times" idea in two forms:
//
//   1. PrefixSum (1D)  — a class wrapping a running-total array over a
//      std::vector<int>, answering any inclusive range-sum query in O(1)
//      after an O(n) build. Includes rebuild() to make the "mutation forces
//      an O(n) rebuild" cost explicit rather than hidden.
//
//   2. 2D prefix sum   — free functions building a 2D running-total grid over
//      a matrix and answering any axis-aligned submatrix-sum query in O(1),
//      using the four-corner inclusion-exclusion formula.
//
// Both are expressed as small, generic, well-commented building blocks so you
// can see the *shape* of the pattern independent of any one problem. The
// worked, problem-specific solutions live in problems/*.cpp.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// PrefixSum — 1D running-total array with O(1) range-sum queries.
//
// Convention used throughout: prefix_ has length n + 1.
//   prefix_[0]     = 0                          (sum of zero elements)
//   prefix_[k]     = arr[0] + arr[1] + ... + arr[k-1]   for k = 1..n
//
// This "one longer, leading zero" convention means rangeSum(i, j) never needs
// a special case for a range that starts at index 0 — see the README's
// Common Mistakes section for why this specific off-by-one is the most
// frequent bug in this pattern.
//
// A wide accumulator (long long) is used for the running totals even though
// the input elements are int, because a running total over many elements can
// exceed the range of a 32-bit int well before any individual element does.
// ----------------------------------------------------------------------------
class PrefixSum {
 public:
  explicit PrefixSum(const std::vector<int>& arr) { build(arr); }

  // Sum of arr[i..j] inclusive, 0-indexed. O(1).
  // Throws std::out_of_range for an invalid range so a bug surfaces loudly
  // instead of silently returning a wrong number.
  long long rangeSum(size_t i, size_t j) const {
    const size_t n = prefix_.size() - 1;  // original array length
    if (n == 0 || i > j || j >= n) {
      throw std::out_of_range("PrefixSum::rangeSum: invalid range [" +
                               std::to_string(i) + ", " + std::to_string(j) +
                               "] for array of length " + std::to_string(n));
    }
    return prefix_[j + 1] - prefix_[i];
  }

  // Rebuilds the entire prefix array from a new snapshot of the underlying
  // data. This is the ONLY way to keep queries correct after a mutation —
  // named explicitly (rather than hidden behind an "update" that looks cheap)
  // to make the O(n) mutability cost visible in code, not just in prose.
  void rebuild(const std::vector<int>& arr) { build(arr); }

  size_t size() const { return prefix_.size() - 1; }

 private:
  void build(const std::vector<int>& arr) {
    prefix_.assign(arr.size() + 1, 0LL);
    for (size_t i = 1; i <= arr.size(); ++i) {
      prefix_[i] = prefix_[i - 1] + arr[i - 1];
    }
  }

  std::vector<long long> prefix_;  // length arr.size() + 1
};

// ----------------------------------------------------------------------------
// buildPrefixSum2D — builds a (rows+1) x (cols+1) 2D running-total grid.
//
// P[r][c] = sum of the rectangle from (0,0) to (r-1, c-1) inclusive, in the
// original matrix. Recurrence (inclusion-exclusion):
//
//   P[r][c] = matrix[r-1][c-1]   // the new cell itself
//           + P[r-1][c]          // everything above this row
//           + P[r][c-1]          // everything to the left of this column
//           - P[r-1][c-1]        // subtract: counted in BOTH terms above
//
// The subtraction undoes double-counting of the top-left rectangle, which
// both "everything above" and "everything to the left" include once each.
// ----------------------------------------------------------------------------
std::vector<std::vector<long long>> buildPrefixSum2D(
    const std::vector<std::vector<int>>& matrix) {
  const size_t rows = matrix.size();
  const size_t cols = rows == 0 ? 0 : matrix[0].size();

  std::vector<std::vector<long long>> prefix(rows + 1,
                                              std::vector<long long>(cols + 1, 0LL));

  for (size_t r = 1; r <= rows; ++r) {
    for (size_t c = 1; c <= cols; ++c) {
      prefix[r][c] = matrix[r - 1][c - 1] + prefix[r - 1][c] + prefix[r][c - 1] -
                     prefix[r - 1][c - 1];
    }
  }
  return prefix;
}

// ----------------------------------------------------------------------------
// rangeSum2D — sum of the submatrix from (r1, c1) to (r2, c2) inclusive
// (0-indexed, in the ORIGINAL matrix's coordinates), given a prefix grid
// already built by buildPrefixSum2D. O(1).
//
// Four-corner inclusion-exclusion formula (the 2D analog of P[j+1] - P[i]):
//
//   answer = P[r2+1][c2+1]   // everything from the top-left corner through
//                            // (r2, c2)
//          - P[r1][c2+1]     // remove: everything above row r1 (too far up)
//          - P[r2+1][c1]     // remove: everything left of column c1 (too far left)
//          + P[r1][c1]       // add back: the top-left region subtracted twice above
// ----------------------------------------------------------------------------
long long rangeSum2D(const std::vector<std::vector<long long>>& prefix, size_t r1,
                      size_t c1, size_t r2, size_t c2) {
  const size_t rows = prefix.size() - 1;
  const size_t cols = rows == 0 ? 0 : prefix[0].size() - 1;

  if (r1 > r2 || c1 > c2 || r2 >= rows || c2 >= cols) {
    throw std::out_of_range("rangeSum2D: invalid rectangle");
  }

  return prefix[r2 + 1][c2 + 1] - prefix[r1][c2 + 1] - prefix[r2 + 1][c1] +
         prefix[r1][c1];
}

// ============================================================================
// main() — demonstrates both the 1D and 2D variants with printed, verifiable
// output.
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

  std::cout << "--- PrefixSum (1D) ---\n";
  {
    std::vector<int> arr = {2, 4, 5, 3, 6, 7, 1, 9};
    PrefixSum ps(arr);

    check(ps.rangeSum(0, 7) == 37, "sum of entire array -> 37");
    check(ps.rangeSum(2, 5) == 21, "sum of arr[2..5] (5+3+6+7) -> 21");
    check(ps.rangeSum(0, 0) == 2, "single-element range at index 0 -> 2");
    check(ps.rangeSum(7, 7) == 9, "single-element range at last index -> 9");

    bool threw = false;
    try {
      ps.rangeSum(5, 2);  // i > j: invalid
    } catch (const std::out_of_range&) {
      threw = true;
    }
    check(threw, "invalid range (i > j) throws std::out_of_range");

    // Simulate a mutation: arr[3] changes from 3 to 100. Queries against the
    // OLD prefix array would now be silently wrong -- rebuild() is required.
    std::vector<int> mutated = arr;
    mutated[3] = 100;
    ps.rebuild(mutated);
    check(ps.rangeSum(2, 5) == 118, "after rebuild, sum of arr[2..5] (5+100+6+7) -> 118");
  }

  std::cout << "\n--- 2D Prefix Sum (submatrix sums) ---\n";
  {
    std::vector<std::vector<int>> matrix = {
        {3, 0, 1, 4, 2},
        {5, 6, 3, 2, 1},
        {1, 2, 0, 1, 5},
        {4, 1, 0, 1, 7},
        {1, 0, 3, 0, 5},
    };
    auto prefix2d = buildPrefixSum2D(matrix);

    // Classic example from LeetCode 304's problem statement.
    check(rangeSum2D(prefix2d, 2, 1, 4, 3) == 8,
          "submatrix (2,1)-(4,3) -> 8");
    check(rangeSum2D(prefix2d, 1, 1, 2, 2) == 11,
          "submatrix (1,1)-(2,2) -> 11");
    check(rangeSum2D(prefix2d, 1, 2, 2, 4) == 12,
          "submatrix (1,2)-(2,4) -> 12");

    // Single cell and whole-matrix sanity checks.
    check(rangeSum2D(prefix2d, 0, 0, 0, 0) == 3, "single cell (0,0) -> 3");

    long long total = 0;
    for (const auto& row : matrix) {
      for (int v : row) total += v;
    }
    check(rangeSum2D(prefix2d, 0, 0, 4, 4) == total,
          "whole-matrix submatrix sum matches manual total");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
