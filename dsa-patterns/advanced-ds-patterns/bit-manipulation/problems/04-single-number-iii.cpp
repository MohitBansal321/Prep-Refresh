// ============================================================================
// LeetCode 260 — Single Number III
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums`, in which exactly two elements appear once
// and all other elements appear exactly twice, find the two elements that
// appear only once. Return them in any order. Linear runtime, O(1) extra
// space.
//
// Example: nums = [1, 2, 1, 3, 2, 5]  ->  {3, 5}
//
// APPROACH — XOR everything, then partition on the lowest differing bit
// ----------------------------------------------------------------------
// Plain XOR cancellation (problems/01) cannot finish the job here: XOR-ing
// the whole array cancels every duplicated pair but leaves TWO survivors
// merged into one accumulator:
//
//   xor_all = a ^ b        (the two unique values, still entangled)
//
// The entanglement is not useless, though. Because a != b, xor_all is NOT
// zero, so it has at least one set bit — and any set bit of (a ^ b) marks a
// position where a and b DIFFER. Pick one such bit (the lowest is
// conventional; `diff & -diff` isolates it). That bit is now a perfect
// discriminator:
//
//   - a has it set, b does not (or vice versa).
//   - Every DUPLICATED value falls wholly on one side or the other — both
//     copies land in the same group.
//
// So walk the array once more, splitting elements into two groups by that
// bit, and XOR within each group. Within each group, duplicates cancel to
// zero and the lone survivor is revealed — reducing ONE two-unknowns problem
// into TWO independent instances of Single Number I.
//
// WHY UNSIGNED ARITHMETIC FOR THE LOW BIT
// ---------------------------------------
// `x & (-x)` is the textbook lowest-set-bit idiom, but negating a signed int
// is undefined behavior for INT_MIN. Casting to unsigned first makes the
// arithmetic well-defined modulo 2^32 (~x + 1 == -x there), so the idiom is
// safe for every input including extreme values.
//
// COMPLEXITY
// ----------
// Time:  O(n) — two passes over the array, O(1) work per element.
// Space: O(1) — two accumulators and a mask; nothing proportional to n.
// ============================================================================
//
// Compile & run:
//   g++ -std=c++17 -Wall problems/04-single-number-iii.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// Returns {a, b} — the two values appearing exactly once, in arbitrary order.
std::pair<int, int> singleNumber(const std::vector<int>& nums) {
  // Pass 1: cancel every duplicated pair. What remains is a ^ b.
  long long acc = 0;
  for (int n : nums) {
    acc ^= n;
  }

  // Isolate the lowest set bit of (a ^ b), in unsigned space so negation is
  // well-defined even when the top bit participates. Any set bit of the diff
  // would work as the discriminator; the lowest is just easy to compute.
  unsigned int diff = static_cast<unsigned int>(acc);
  unsigned int discriminator = diff & (~diff + 1u);  // == diff & -diff

  // Pass 2: split by the discriminating bit and XOR each side separately.
  // Both copies of every duplicated value take the same branch, so they
  // cancel within their group; each group keeps exactly one unique value.
  int first = 0;
  int second = 0;
  for (int n : nums) {
    if ((static_cast<unsigned int>(n) & discriminator) != 0u) {
      first ^= n;
    } else {
      second ^= n;
    }
  }

  return std::make_pair(first, second);
}

// Order-independent comparison helper for the tests below.
bool samePair(std::pair<int, int> p, int x, int y) {
  return (p.first == x && p.second == y) || (p.first == y && p.second == x);
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
    std::vector<int> nums = {1, 2, 1, 3, 2, 5};
    check(samePair(singleNumber(nums), 3, 5),
          "[1,2,1,3,2,5] -> {3,5} (any order)");
  }

  {
    // Edge case: array of size 2 — both elements unique, no pairs at all.
    std::vector<int> nums = {1, 2};
    check(samePair(singleNumber(nums), 1, 2), "[1,2] -> {1,2}");
  }

  {
    // Edge case: negative numbers mixed with zero as one of the answers.
    std::vector<int> nums = {-1, 0};
    check(samePair(singleNumber(nums), -1, 0), "[-1,0] -> {-1,0}");
  }

  {
    // Duplicates interleaved around negatives; also exercises a diff whose
    // set bits live in high/sign-bit positions.
    std::vector<int> nums = {-4, 7, -4, 9, 7, 13};
    check(samePair(singleNumber(nums), 9, 13),
          "[-4,7,-4,9,7,13] -> {9,13} (interleaved)");
  }

  {
    // Edge case where the two uniques differ ONLY in the top bit: the
    // discriminator must be 0x80000000, which the signed-negation idiom
    // would mishandle (UB for INT_MIN); the unsigned form stays correct.
    std::vector<int> nums = {2147483647, -2147483648};  // INT_MAX and INT_MIN
    check(samePair(singleNumber(nums), 2147483647, -2147483648),
          "{INT_MAX, INT_MIN} -> top-bit discriminator");
  }

  {
    // Larger case: many pairs surrounding both uniques.
    std::vector<int> nums = {6, 6, 15, 15, 11, 4};
    check(samePair(singleNumber(nums), 11, 4),
          "[6,6,15,15,11,4] -> {11,4} (any order)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
