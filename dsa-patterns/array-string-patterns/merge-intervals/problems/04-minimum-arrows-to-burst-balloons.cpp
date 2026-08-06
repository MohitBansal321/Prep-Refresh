// ============================================================================
// LeetCode 452 — Minimum Number of Arrows to Burst Balloons
// ============================================================================
//
// Problem: Balloons are represented as intervals [x_start, x_end] along a
// horizontal line. An arrow shot straight up at position x bursts every
// balloon whose interval contains x. Given the balloons, find the minimum
// number of arrows needed to burst them all.
//
// Approach (sort-and-sweep, same family as Merge Intervals, sorted by END):
// a group of balloons that all pairwise overlap can always be burst with a
// SINGLE arrow, fired at the point where they all still overlap. So the
// question "how many arrows" is really "how many disjoint overlapping groups
// are there" — which is exactly the group-boundary detection Merge Intervals
// already does, except here we count groups (arrows) instead of merging them
// into one big interval per group.
//
// We sort by end time (not start) for the same reason as
// 03-non-overlapping-intervals.cpp: the earliest-ending balloon in the
// current group tells us the tightest possible firing point (its own end) —
// firing there is guaranteed to also hit every other balloon in the group,
// because by definition every balloon in the group started at or before that
// point and ends at or after it.
//
//   - Sort balloons by end coordinate.
//   - Track `arrowPosition`, initialized to the first balloon's end — that is
//     where we "fire" the first arrow.
//   - For every subsequent balloon: if its start is <= arrowPosition, the
//     current arrow already bursts it (it overlaps the group) — no new arrow
//     needed. Otherwise it starts strictly after the current arrow's
//     position, so it needs a NEW arrow, fired at ITS end coordinate (the new
//     tightest point for the next group).
//
// Complexity: O(n log n) time (dominated by the sort by end), O(1) extra
// space beyond the sort's own working space.
//
// Compile:
//   g++ -std=c++17 -Wall 04-minimum-arrows-to-burst-balloons.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int findMinArrowShots(std::vector<std::vector<int>> points) {
  if (points.empty()) return 0;

  // Sort by end coordinate — the same "sort by end" idea as Non-overlapping
  // Intervals, for the same reason: it gives the tightest possible shared
  // firing point for the current group of overlapping balloons.
  std::sort(points.begin(), points.end(),
            [](const std::vector<int>& a, const std::vector<int>& b) {
              return a[1] < b[1];
            });

  int arrows = 1;
  long long arrowPosition = points[0][1];

  for (size_t i = 1; i < points.size(); ++i) {
    long long start = points[i][0];

    if (start > arrowPosition) {
      // This balloon starts after the current arrow's position: it is not
      // burst by the current arrow, so a new group (and a new arrow) begins.
      ++arrows;
      arrowPosition = points[i][1];
    }
    // Else: start <= arrowPosition means this balloon overlaps the current
    // group and is already burst by the arrow already placed. No new arrow,
    // and arrowPosition does NOT change — it must stay at the tightest
    // (smallest) end seen so far in this group to keep bursting everything
    // still to come that also belongs to this group.
  }

  return arrows;
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](int actual, int expected, const std::string& label) {
    bool ok = (actual == expected);
    std::cout << (ok ? "[PASS] " : "[FAIL] ") << label << " (got " << actual
               << ", expected " << expected << ")\n";
    ok ? ++pass_count : ++fail_count;
  };

  check(findMinArrowShots({{10, 16}, {2, 8}, {1, 6}, {7, 12}}), 2,
        "classic example: two overlapping groups need two arrows");

  check(findMinArrowShots({{1, 2}, {3, 4}, {5, 6}, {7, 8}}), 4,
        "no balloons overlap at all -> one arrow per balloon");

  check(findMinArrowShots({{1, 2}, {2, 3}, {3, 4}, {4, 5}}), 2,
        "touching endpoints share an arrow (closed intervals overlap at the boundary)");

  check(findMinArrowShots({{1, 10}, {2, 9}, {3, 8}, {4, 7}}), 1,
        "fully nested balloons all burst with a single arrow");

  check(findMinArrowShots({}), 0, "empty input needs zero arrows");

  check(findMinArrowShots({{1, 100000000}}), 1,
        "single wide balloon needs exactly one arrow, no overflow on large coordinates");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
