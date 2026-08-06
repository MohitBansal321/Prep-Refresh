// ============================================================================
// LeetCode 202 — Happy Number
// ============================================================================
//
// PROBLEM (summary):
//   Write an algorithm to determine if a number `n` is "happy":
//     - Starting with `n`, replace it with the sum of the squares of its
//       decimal digits.
//     - Repeat this process.
//     - If it eventually reaches 1, `n` is happy.
//     - If it loops endlessly in a cycle that never includes 1, `n` is
//       NOT happy.
//   Return true if `n` is happy, false otherwise.
//
// WHY THIS IS A FAST & SLOW POINTERS PROBLEM (no linked list in sight!):
//   This is the key generalization the whole pattern is built on: Fast &
//   Slow Pointers is not really about `ListNode* next` — it is about ANY
//   sequence defined by repeatedly applying a function to a value, where
//   you need to detect whether that sequence enters a cycle. A linked
//   list's `next` pointer is one such function (node -> node->next). Here
//   the "next" function is `next(x) = sum of squares of x's digits`, and
//   the "nodes" are plain integers instead of heap-allocated structs.
//
//   Because any deterministic function applied repeatedly to a finite
//   input space MUST eventually repeat a value (pigeonhole principle —
//   there are only finitely many possible sums-of-squared-digits for
//   numbers below a bound), the sequence is guaranteed to either hit 1
//   or fall into some other cycle. Floyd's cycle detection applies
//   exactly as it does on a linked list: advance a `slow` value by
//   calling `next()` once, advance a `fast` value by calling `next()`
//   twice, and check if they collide.
//
//   If they collide AT 1, the number is happy (the "cycle" is the
//   trivial fixed point 1 -> 1 -> 1 -> ...). If they collide anywhere
//   else, it is not happy.
//
// COMPLEXITY:
//   Time:  O(log n) per `next()` call to sum digit squares, and the
//          cycle (if any) among small numbers is short and bounded by a
//          small constant, so overall this runs in effectively O(log n)
//          amortized per step until a collision — for interview
//          purposes this is treated as O(1) per step / fast overall.
//   Space: O(1) — two integer variables, no hash set needed. Contrast
//          with the alternative solution that stores every seen value in
//          a std::unordered_set<int>, which is O(k) space for k = number
//          of distinct values visited before a repeat.
//
// Compile:
//   g++ -std=c++17 -Wall 03-happy-number.cpp -o /tmp/out_p3 && /tmp/out_p3
// ============================================================================

#include <iostream>

// ----------------------------------------------------------------------------
// The "next" function: sum of squares of decimal digits. This plays the
// exact role that `node->next` plays in a linked list — it is the single
// step that advances the sequence.
// ----------------------------------------------------------------------------
int sum_of_squared_digits(int n) {
  int sum = 0;
  while (n > 0) {
    int digit = n % 10;
    sum += digit * digit;
    n /= 10;
  }
  return sum;
}

// ----------------------------------------------------------------------------
// The solution itself — Floyd's cycle detection over the integer sequence.
// ----------------------------------------------------------------------------
bool isHappy(int n) {
  int slow = n;
  int fast = n;

  do {
    slow = sum_of_squared_digits(slow);          // one step
    fast = sum_of_squared_digits(sum_of_squared_digits(fast));  // two steps
  } while (slow != fast);

  // slow == fast: we are at the cycle's single point of collision.
  // It is 1 if and only if the number is happy (1 -> 1 -> 1 -> ... is a
  // one-node "cycle" at the fixed point 1).
  return slow == 1;
}

// ----------------------------------------------------------------------------
// Test harness.
// ----------------------------------------------------------------------------
void check(const std::string& name, bool actual, bool expected) {
  std::cout << (actual == expected ? "PASS" : "FAIL") << " — " << name
             << " (got " << std::boolalpha << actual << ", expected "
             << expected << ")\n";
}

int main() {
  std::cout << "=== LeetCode 202: Happy Number ===\n\n";

  // 19 -> 1^2+9^2=82 -> 8^2+2^2=68 -> 36+64=100 -> 1+0+0=1. Happy.
  check("19 is happy", isHappy(19), true);

  // 2 -> 4 -> 16 -> 37 -> 58 -> 89 -> 145 -> 42 -> 20 -> 4 (repeats). Not happy.
  check("2 is not happy", isHappy(2), false);

  // 1 is trivially happy (already at the fixed point).
  check("1 is happy", isHappy(1), true);

  // 7 -> 49 -> 97 -> 130 -> 10 -> 1. Happy.
  check("7 is happy", isHappy(7), true);

  // 4 is a well-known member of the "unhappy cycle"
  // (4 -> 16 -> 37 -> 58 -> 89 -> 145 -> 42 -> 20 -> 4 -> ...).
  check("4 is not happy", isHappy(4), false);

  return 0;
}
