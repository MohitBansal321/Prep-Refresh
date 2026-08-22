// ============================================================================
// LeetCode 191 — Number of 1 Bits
// ============================================================================
//
// PROBLEM
// -------
// Given a positive integer `n` (treated as a 32-bit unsigned value), return
// the number of '1' bits it has (also known as the Hamming weight /
// population count).
//
// Example: n = 11 = 0b1011  ->  3
//
// APPROACH — Brian Kernighan's algorithm (n & (n - 1) clears lowest set bit)
// ---------------------------------------------------------------------------
// The naive approach tests all 32 bits one at a time (`n & (1 << i)` for each
// i) — O(32) regardless of content. Kernighan's trick does strictly better by
// exploiting one identity:
//
//   Subtracting 1 from n flips the LOWEST SET bit from 1 to 0 and turns
//   every bit BELOW it to 1. AND-ing that against the original therefore
//   preserves everything above the lowest set bit and erases it (plus the
//   now-irrelevant bits below).
//
// So each loop iteration removes exactly one set bit, and the loop runs
// popcount(n) times — not word-width times. countSetBits(11):
//   1011 & 1010 = 1010   (count 1)
//   1010 & 1001 = 1000   (count 2)
//   1000 & 0111 = 0000   (count 3, then n == 0 exits)
// See ../images/trace-diagram.md for this trace annotated bit by bit.
//
// The SAME identity answers power-of-two questions without looping: a power
// of two has exactly one set bit, so one iteration collapses it to zero —
// isPowerOfTwo(n) == (n != 0 && (n & (n-1)) == 0). The `n != 0` guard is NOT
// optional: n = 0 also satisfies (n & (n-1)) == 0 but is not a power of two.
//
// WHY UNSIGNED MATTERS
// --------------------
// We take the parameter as `unsigned int` deliberately: right-shifting a
// NEGATIVE signed int is implementation-defined in C++, and `1 << 31` on a
// signed int is outright undefined behavior. Bit-twiddling code should use
// unsigned types so shift semantics are well-defined.
//
// COMPLEXITY
// ----------
// Time:  O(popcount(n)) — one iteration per SET bit; best case O(1)
//               (e.g. any power of two takes exactly one pass), worst case
//               O(32) when every bit is set.
// Space: O(1) — one counter and the mutated copy of n.
// ============================================================================

#include <iostream>
#include <string>

int countSetBits(unsigned int n) {
  int count = 0;

  // Loop while set bits remain. Each pass deletes exactly one — the lowest.
  while (n != 0) {
    // n - 1 flips the lowest set bit to 0 and fills everything below with 1s;
    // AND-ing keeps the untouched high part and drops the rest. Example:
    //   n     = 0b1010
    //   n - 1 = 0b1001
    //   n &=  -> 0b1000   (lowest set bit at position 1 removed)
    n &= (n - 1);
    ++count;
  }

  return count;
}

bool isPowerOfTwo(unsigned int n) {
  // Guard against n == 0: it passes the bit test but is not a power of two.
  // For any n > 0 with a single set bit, clearing it leaves 0 immediately.
  return n != 0 && (n & (n - 1)) == 0;
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
    unsigned int n = 11;  // 0b1011
    check(countSetBits(n) == 3, "countSetBits(11 = 0b1011) -> 3");
  }

  {
    unsigned int n = 128;  // 0b10000000 — single high set bit
    check(countSetBits(n) == 1, "countSetBits(128 = 0b10000000) -> 1");
  }

  {
    // Edge case: n = 0 has no set bits; the loop body must never execute.
    check(countSetBits(0u) == 0, "countSetBits(0) -> 0");
  }

  {
    // Edge case: ALL 32 bits set — the loop's worst case, exercising every
    // position including the top bit (which would be UB territory if we had
    // shifted a signed 1 left by 31 anywhere along the way).
    check(countSetBits(0xFFFFFFFFu) == 32,
          "countSetBits(0xFFFFFFFF) -> 32 (all bits set)");
  }

  {
    // Edge case: top bit set only — verifies unsigned handling of bit 31.
    check(countSetBits(0x80000000u) == 1,
          "countSetBits(0x80000000) -> 1 (top bit only)");
  }

  {
    // Sparse value: few set bits spread across the word; loop count should
    // equal the popcount, not the distance to the highest set bit.
    check(countSetBits(0x80000001u) == 2,
          "countSetBits(0x80000001) -> 2 (sparse)");
  }

  // Power-of-two checks — same identity, one-shot form.
  check(isPowerOfTwo(1u) == true, "isPowerOfTwo(1) -> true (2^0)");
  check(isPowerOfTwo(16u) == true, "isPowerOfTwo(16) -> true");
  check(isPowerOfTwo(0u) == false, "isPowerOfTwo(0) -> false (the classic trap)");
  check(isPowerOfTwo(18u) == false, "isPowerOfTwo(18 = 0b10010) -> false");
  check(isPowerOfTwo(0x80000000u) == true,
        "isPowerOfTwo(0x80000000) -> true (2^31)");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
