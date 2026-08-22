// ============================================================================
// LeetCode 136 — Single Number
// ============================================================================
//
// PROBLEM
// -------
// Given a non-empty array of integers `nums`, every element appears twice
// except for one. Find that single one. You must implement a solution with
// linear runtime complexity and O(1) extra space.
//
// Example: nums = [4, 1, 2, 1, 2]  ->  4
//
// APPROACH — XOR accumulation (pair cancellation)
// ------------------------------------------------
// The obvious approaches violate the constraints: a hash map counting
// occurrences is O(n) time but O(n) space; sorting first is O(n log n).
// Bit Manipulation solves it with a single accumulator and no auxiliary
// structure, using three algebraic properties of XOR:
//
//   1. x ^ x = 0        — a value XOR-ed with itself vanishes.
//   2. x ^ 0 = x        — zero is the identity; the accumulator starts there.
//   3. ^ is commutative and associative — so the ORDER of elements does not
//      matter at all; equal values "find each other" wherever they sit in
//      the array.
//
// Because of (3), conceptually rearrange the array so every pair sits side
// by side: each pair XORs to 0 by (1), and the running result keeps only
// the unpaired value — which survives because it never meets its partner.
// Formally: result = v1 ^ v2 ^ ... ^ vn = (single) ^ 0 ^ 0 ^ ... = single.
//
// WHY THIS MATTERS CONCEPTUALLY
// -----------------------------
// This is the entire pattern family in miniature: replace an auxiliary data
// structure (the hash map that remembers what it has seen) with an algebraic
// identity that makes remembering unnecessary. The precondition is strict,
// though — every distractor must appear an EVEN number of times. If any
// distractor appeared three times, its leftover copy would pollute the
// accumulator (x ^ x ^ x = x), and a per-bit-position counting technique
// would be required instead (LeetCode 137 — see ../exercises.md).
//
// COMPLEXITY
// ----------
// Time:  O(n) — one pass, one XOR per element.
// Space: O(1) — a single int accumulator, regardless of input size.
// ============================================================================
//
// Compile & run:
//   g++ -std=c++17 -Wall problems/01-single-number.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// Returns the element appearing exactly once; every other element appears
// exactly twice. (Precondition per problem statement — see banner notes for
// why this precondition is not optional.)
int singleNumber(const std::vector<int>& nums) {
  int result = 0;

  // Order-independence means we do not need to sort or partition first:
  // pairs cancel whenever they meet, in any order of encounter.
  for (int n : nums) {
    result ^= n;
  }

  return result;
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
    std::vector<int> nums = {2, 2, 1};
    check(singleNumber(nums) == 1, "[2,2,1] -> 1");
  }

  {
    // The canonical example: the answer (4) appears FIRST, before its
    // would-be partners — order-independence is what makes this work.
    std::vector<int> nums = {4, 1, 2, 1, 2};
    check(singleNumber(nums) == 4, "[4,1,2,1,2] -> 4");
  }

  {
    // Edge case: array of size 1 — no cancellation needed at all.
    std::vector<int> nums = {7};
    check(singleNumber(nums) == 7, "[7] -> 7 (single element)");
  }

  {
    // Edge case: negative numbers. XOR operates on the two's-complement bit
    // pattern, so sign is handled automatically (-3 ^ -3 == 0 just as well).
    std::vector<int> nums = {-3, -1, -3};
    check(singleNumber(nums) == -1, "[-3,-1,-3] -> -1 (negatives)");
  }

  {
    // Edge case: interleaved pairs around a large-magnitude unique value.
    std::vector<int> nums = {1000000, 5, 5, 1000000, 42};
    check(singleNumber(nums) == 42, "interleaved pairs -> 42");
  }

  {
    // Edge case: the unique value is 0 itself. A broken implementation that
    // initializes the accumulator wrong (or skips zeros) fails here.
    std::vector<int> nums = {9, 9, 0};
    check(singleNumber(nums) == 0, "[9,9,0] -> 0 (unique value is zero)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
