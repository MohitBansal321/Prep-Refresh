// ============================================================================
// LeetCode 435 — Non-overlapping Intervals
// ============================================================================
//
// Problem: Given an array of intervals, find the minimum number of intervals
// you need to remove to make the rest of the intervals non-overlapping.
//
// Approach (a sort-and-sweep sibling of Merge Intervals, sorted by END
// instead of start): the general Merge Intervals pattern sorts by start time
// because it wants to grow the biggest possible merged block. Here the goal
// is different — we want to KEEP as many intervals as possible (equivalently,
// remove as few as possible) — so we sort by END time instead. Sorting by end
// time means that whichever interval we look at first always frees up the
// earliest possible "room" for whatever comes next, which is the greedy
// insight this problem needs (this is the same "sort by the right key, commit
// to one irrevocable choice per step" idea as the Greedy pattern; see the
// README's Similar Patterns section).
//
// The sweep itself still uses the same "current interval + does the next one
// overlap it" comparison from Merge Intervals:
//   - Keep a running `lastEnd`, initialized to the end of the first
//     (earliest-ending) interval.
//   - For every subsequent interval (in sorted-by-end order):
//       - If its start is >= lastEnd, it does not overlap what we have kept
//         so far — keep it, and update lastEnd to its end.
//       - If its start is < lastEnd, it overlaps the interval we already
//         committed to keeping. Since that kept interval ends earlier (we
//         sorted by end), it is always at least as good to discard the
//         CURRENT (later-ending) interval instead of the one we already
//         kept — removing it costs one removal and leaves lastEnd unchanged
//         (still as small as possible for future comparisons).
//
// Complexity: O(n log n) time (dominated by the sort by end time), O(1)
// extra space beyond the sort's own working space.
//
// Compile:
//   g++ -std=c++17 -Wall 03-non-overlapping-intervals.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int eraseOverlapIntervals(std::vector<std::vector<int>> intervals) {
  if (intervals.empty()) return 0;

  // Sort by END time — the key insight that separates this problem from
  // plain Merge Intervals (which sorts by start).
  std::sort(intervals.begin(), intervals.end(),
            [](const std::vector<int>& a, const std::vector<int>& b) {
              return a[1] < b[1];
            });

  int removals = 0;
  int lastEnd = intervals[0][1];

  for (size_t i = 1; i < intervals.size(); ++i) {
    int start = intervals[i][0];
    int end = intervals[i][1];

    if (start < lastEnd) {
      // Overlaps the interval we already committed to keeping. Discard the
      // current one (it ends later, so keeping the earlier-ending interval
      // we already have leaves more room for whatever comes next).
      ++removals;
      // lastEnd deliberately stays unchanged — we keep the smaller end.
    } else {
      // No overlap: keep this interval, and it becomes the new baseline.
      lastEnd = end;
    }
  }

  return removals;
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

  check(eraseOverlapIntervals({{1, 2}, {2, 3}, {3, 4}, {1, 3}}), 1,
        "classic example: remove [1,3] to leave a non-overlapping chain");

  check(eraseOverlapIntervals({{1, 2}, {1, 2}, {1, 2}}), 2,
        "three identical intervals -> keep one, remove two");

  check(eraseOverlapIntervals({{1, 2}, {2, 3}}), 0,
        "touching-but-not-overlapping intervals need no removals");

  check(eraseOverlapIntervals({}), 0, "empty input needs no removals");

  check(eraseOverlapIntervals({{1, 100}, {11, 22}, {1, 11}, {2, 12}}), 2,
        "one long interval overlapping two smaller ones -> remove the long one and one more");

  check(eraseOverlapIntervals({{-52, 31}, {-73, -26}, {82, 97}, {-65, -11},
                                {-62, -49}}), 3,
        "unsorted mixed-sign intervals, four of five overlap once sorted by end");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
