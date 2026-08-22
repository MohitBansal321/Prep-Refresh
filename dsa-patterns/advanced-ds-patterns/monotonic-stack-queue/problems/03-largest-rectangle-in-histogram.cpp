// ============================================================================
// LeetCode 84 — Largest Rectangle in Histogram
// ============================================================================
//
// PROBLEM
// -------
// Given an array of integers `heights` representing the histogram's bar
// heights (each bar has width 1), return the area of the largest rectangle
// that fits entirely inside the histogram.
//
// Example: heights = [2,1,5,6,2,3] -> 10
//   (the rectangle spanning bars 5 and 6: min height 5 x width 2 = 10)
//
// APPROACH — Monotonic stack: each bar's best rectangle is found at its pop
// --------------------------------------------------------------------------
// KEY INSIGHT: the largest rectangle containing bar i has height exactly
// heights[i] (if it were taller it could not include bar i, so every maximal
// rectangle has some bar as its "limiting" height — checking one rectangle
// per bar covers all candidates). That rectangle extends left and right from
// bar i until it hits a STRICTLY SHORTER bar (equal-height bars don't block
// it; they merely duplicate credit). So the answer is:
//
//     max over i of  heights[i] * (right_smaller(i) - left_smaller(i) - 1)
//
// Computing nearest-smaller-on-both-sides naively is O(n^2). The monotonic
// stack gets BOTH neighbors in ONE pass:
//
// Maintain a stack of bar indices with NON-DECREASING heights bottom-to-top.
// When bar i arrives with height h:
//   - While the top's height is >= h, that bar's rectangle cannot extend past
//     i on the right (i blocks or matches it): POP it. At pop time we know
//     BOTH boundaries:
//       right boundary = i          (first bar to the right that is shorter,
//                                    since the pop happened because of i)
//       left boundary  = new stack top after popping
//                                    (the nearest surviving bar to the left
//                                    that is shorter — everything between
//                                    was popped earlier for being taller)
//       width = i - new_top - 1, or i if the stack is empty (bar reaches the
//               left edge).
//   - Push i.
//
// To force every bar to be popped (and thus evaluated), append a virtual bar
// of height 0 at position n: nothing survives a zero-height bar.
//
// WHY THIS IS O(n) DESPITE THE INNER WHILE LOOP (amortized argument):
// each index is pushed once and popped at most once over the whole run, so
// total work <= 2n even though one incoming bar can pop many at once. The
// brute force alternative — expand left/right from each bar until a shorter
// one appears — is O(n^2) on e.g. a strictly increasing histogram.
//
// COMPLEXITY
// ----------
// Time:  O(n) amortized.
// Space: O(n) worst case for the stack (strictly increasing input never pops
//        until the final sentinel bar, which then pops everything).
// ============================================================================
#include <iostream>
#include <string>
#include <vector>

long long largestRectangleArea(const std::vector<int>& heights) {
  int n = static_cast<int>(heights.size());
  std::vector<int> st;  // INDICES; heights non-decreasing bottom-to-top
  long long best = 0;

  // Iterate one step PAST the end: i == n acts as the virtual zero-height
  // sentinel that flushes all remaining bars off the stack.
  for (int i = 0; i <= n; ++i) {
    // Treat position n as height 0; real positions use their own height.
    int h = (i == n) ? 0 : heights[i];

    // Pop every bar whose rectangle is terminated by bar i. Using '>=' means
    // equal-height bars pop each other; the earlier of two equal bars then
    // computes a too-small width, but the later one inherits the full span,
    // so the maximum is still correct.
    while (!st.empty() && heights[st.back()] >= h) {
      int popped = st.back();
      st.pop_back();
      long long height = heights[popped];
      // Left boundary: the bar now on top is the nearest shorter one to the
      // left (everything taller-or-equal between them was already popped).
      // If the stack is empty, the rectangle reaches the left edge.
      long long width =
          st.empty() ? i : static_cast<long long>(i - st.back() - 1);
      long long area = height * width;
      if (area > best) best = area;
    }
    st.push_back(i);  // (pushing the sentinel index is harmless: loop ends)
  }

  return best;
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
    std::vector<int> h = {2, 1, 5, 6, 2, 3};
    check(largestRectangleArea(h) == 10,
          "[2,1,5,6,2,3] -> 10");
  }

  {
    // Strictly increasing: no pops until the sentinel; the widest low bar
    // wins (height 1 across the full width).
    std::vector<int> h = {1, 2, 3, 4, 5};
    check(largestRectangleArea(h) == 9,
          "[1,2,3,4,5] -> 9 (1x9 full-width beats 3x3)");
  }

  {
    // Strictly decreasing: every new bar pops the previous immediately;
    // answer is max single-bar-area products.
    std::vector<int> h = {5, 4, 3, 2, 1};
    check(largestRectangleArea(h) == 9,
          "[5,4,3,2,1] -> 9 (3x3)");
  }

  {
    // All equal heights: '>=' pops let each bar compute a partial width, but
    // the LAST equal bar inherits the full width -> n * height.
    std::vector<int> h = {4, 4, 4};
    check(largestRectangleArea(h) == 12,
          "[4,4,4] -> 12 (duplicates still get full-span credit)");
  }

  {
    std::vector<int> h = {2};
    check(largestRectangleArea(h) == 2, "single bar -> its own area");
  }

  {
    // Zero-height bar splits the histogram; rectangles cannot cross it.
    std::vector<int> h = {2, 0, 2};
    check(largestRectangleArea(h) == 2,
          "[2,0,2] -> 2 (zero bar blocks crossing)");
  }

  {
    // Classic plateau-then-spike shape requiring a multi-pop resolution.
    std::vector<int> h = {2, 1, 2};
    check(largestRectangleArea(h) == 3,
          "[2,1,2] -> 3 (two 1-wide 2-high bars, not one 3-wide)");
  }

  {
    // Overflow-sensitive case: tall bar x wide span exceeds int range.
    std::vector<int> h(10, 100000);
    check(largestRectangleArea(h) == 1000000LL,
          "10 bars of 100000 -> 1000000 (long long guards overflow)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
