// ============================================================================
// LeetCode 11 — Container With Most Water
// ============================================================================
//
// PROBLEM
// -------
// You are given an integer array `height` of length n. There are n vertical
// lines drawn such that the two endpoints of the i-th line are (i, 0) and
// (i, height[i]). Find two lines that, together with the x-axis, form a
// container that holds the most water. Return the maximum amount of water
// the container can store.
//
// The container's capacity between lines at indices i < j is:
//     width  = (j - i)
//     height = min(height[i], height[j])   <- water spills over the shorter wall
//     area   = width * height
//
// Example: height = [1,8,6,2,5,4,8,3,7]  ->  49  (lines at index 1 and 8:
//          width = 7, wall height = min(8,7) = 7, area = 49)
//
// APPROACH — Two Pointers (converging, optimization instead of exact match)
// -----------------------------------------------------------------------
// This is the "area maximization" flavor of converging Two Pointers: instead
// of stopping the moment we hit an exact target sum (as in problem 01), we
// keep a running best answer while the pointers close in, and we use a
// greedy elimination argument to decide which pointer moves.
//
// Start with `left = 0` and `right = n - 1` — this is the WIDEST possible
// container, since width = right - left is maximized at the very start.
// At each step:
//   1. Compute the current area and update `best` if it's larger.
//   2. Move the pointer at the SHORTER wall inward.
//
// Why moving the shorter wall is always correct (this is the crux of the
// problem, and worth being able to state precisely in an interview):
// suppose height[left] < height[right]. The current area is capped by
// height[left] (the shorter wall). If we moved `right` inward instead, the
// new width is strictly smaller than the current width, AND the new area is
// still capped by min(height[left], height[new_right]) <= height[left]
// (since height[left] hasn't changed and is the smaller of the two original
// walls). So every container we could form by moving `right` next is
// PROVABLY no better than the one we already measured. The only way to
// possibly find a taller limiting wall is to abandon the current shorter
// wall and try a new left. That is why we always discard the shorter side.
//
// COMPLEXITY
// ----------
// Time:  O(n) — left and right move toward each other, at most n steps
//               total, O(1) work per step.
// Space: O(1) — two indices and a running best, no auxiliary storage.
//
// Contrast with brute force (try every pair (i, j) and compute the area):
// O(n^2) time, since there are ~n^2/2 pairs. Two Pointers uses the greedy
// "always drop the shorter wall" argument above to eliminate huge swaths of
// pairs without ever examining them individually, collapsing O(n^2) to O(n).
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int maxArea(const std::vector<int>& height) {
  int left = 0;
  int right = static_cast<int>(height.size()) - 1;
  long long best = 0;

  while (left < right) {
    long long width = static_cast<long long>(right - left);
    long long limiting_wall = std::min(height[left], height[right]);
    best = std::max(best, width * limiting_wall);

    // Always advance the pointer at the shorter wall — see the proof above.
    if (height[left] < height[right]) {
      ++left;
    } else {
      --right;
    }
  }

  return static_cast<int>(best);
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
    std::vector<int> height = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    check(maxArea(height) == 49, "[1,8,6,2,5,4,8,3,7] -> 49");
  }

  {
    std::vector<int> height = {1, 1};
    check(maxArea(height) == 1, "[1,1] -> 1 (only one possible container)");
  }

  {
    std::vector<int> height = {4, 3, 2, 1, 4};
    check(maxArea(height) == 16, "[4,3,2,1,4] -> 16 (outer two walls win)");
  }

  {
    std::vector<int> height = {1, 2, 1};
    check(maxArea(height) == 2, "[1,2,1] -> 2");
  }

  {
    std::vector<int> height = {2, 3, 4, 5, 18, 17, 6};
    // Best pair: indices 3 (height 5) and 4 (height 18) -> width 1, area 5
    // vs indices 1 (3) and 4 (18) -> width 3, height min(3,18)=3, area 9
    // vs indices 0 (2) and 4 (18) -> width 4, height 2, area 8
    // vs indices 4 (18) and 5 (17) -> width 1, height 17, area 17 <- best so far
    // vs indices 3 (5) and 6 (6)   -> width 3, height 5, area 15
    // vs indices 1 (3) and 6 (6)   -> width 5, height 3, area 15
    // vs indices 0 (2) and 6 (6)   -> width 6, height 2, area 12
    check(maxArea(height) == 17, "[2,3,4,5,18,17,6] -> 17");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
