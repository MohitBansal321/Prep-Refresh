// ============================================================================
// LeetCode 338 — Counting Bits
// ============================================================================
//
// PROBLEM
// -------
// Given an integer `n`, return an array `bits` of length n + 1 such that
// bits[i] is the number of '1' bits in the binary representation of i
// (for every i in the range [0, n]).
//
// Example: n = 5 -> [0, 1, 1, 2, 1, 2]
//           (0b0, 0b1, 0b10, 0b11, 0b100, 0b101)
//
// APPROACH — Dynamic programming on "drop the lowest set bit"
// -----------------------------------------------------------
// Calling Kernighan's loop (see problems/02) independently for each of the
// n + 1 values costs O(n * popcount) total and, worse, re-derives answers
// that neighboring values already contain. The insight is that i's bit
// pattern is a tiny modification of a SMALLER number's already-computed
// pattern, so we can reuse subproblems — classic dynamic programming:
//
//   bits[i] = bits[i >> 1] + (i & 1)
//
// WHY THIS RECURRENCE IS CORRECT
// ------------------------------
// Right-shifting i by one (`i >> 1`) discards the lowest bit; everything
// else is identical. So:
//   - the set bits of i are exactly the set bits of (i >> 1), PLUS
//   - one more if i's lowest bit is 1 (`i & 1`), zero otherwise.
//
// Equivalently: doubling a number appends a 0 bit (same popcount), and
// doubling-plus-one appends a 1 bit (popcount + 1). Since i >> 1 < i for
// i >= 1, the table entry we need is always computed before we need it.
// The base case bits[0] = 0 falls out of initializing the vector to zeros.
//
// This is the same "look up the smaller subproblem" move as Fibonacci-style
// DP — the only novelty is that the decomposition operator is a shift
// instead of a subtraction.
//
// COMPLEXITY
// ----------
// Time:  O(n) — one addition and two cheap bitwise ops per entry; no
//               per-entry popcount loop.
// Space: O(1) extra beyond the required output vector itself.
// ============================================================================
//
// Compile & run:
//   g++ -std=c++17 -Wall problems/03-counting-bits.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// Returns a vector v of size n + 1 where v[i] == popcount(i).
std::vector<int> countBits(int n) {
  // bits[0] stays 0 — zero has no set bits. Every later index reads an
  // earlier entry, so plain left-to-right filling is valid.
  std::vector<int> bits(n + 1, 0);

  for (int i = 1; i <= n; ++i) {
    // i >> 1 : i with its lowest bit discarded  -> smaller, already solved.
    // i & 1  : the lowest bit itself            -> 0 or 1.
    bits[i] = bits[i >> 1] + (i & 1);
  }

  return bits;
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
    std::vector<int> expected = {0};
    check(countBits(0) == expected, "n=0 -> [0]");
  }

  {
    std::vector<int> result = countBits(2);
    std::vector<int> expected = {0, 1, 1};  // 0b0, 0b1, 0b10
    check(result == expected, "n=2 -> [0,1,1]");
  }

  {
    // The canonical example — includes both parities at each shift level.
    std::vector<int> result = countBits(5);
    std::vector<int> expected = {0, 1, 1, 2, 1, 2};
    check(result == expected, "n=5 -> [0,1,1,2,1,2]");
  }

  {
    // A power-of-two boundary: 8 starts a fresh binary digit. Its popcount
    // must be 1, while 7 (0b111) peaks at 3 just before it.
    std::vector<int> result = countBits(8);
    std::vector<int> expected = {0, 1, 1, 2, 1, 2, 2, 3, 1};
    check(result == expected, "n=8 -> power-of-two boundary handled");
  }

  {
    // Cross-check against an independent definition (Kernighan's loop from
    // problems/02) over a longer range — catches subtle recurrence bugs.
    bool all_match = true;
    std::vector<int> result = countBits(64);
    for (int i = 0; i <= 64 && all_match; ++i) {
      int reference = 0;
      unsigned int u = static_cast<unsigned int>(i);
      while (u != 0) {
        u &= (u - 1);
        ++reference;
      }
      if (result[i] != reference) {
        all_match = false;
      }
    }
    check(all_match, "n=64 matches independent Kernighan reference");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
