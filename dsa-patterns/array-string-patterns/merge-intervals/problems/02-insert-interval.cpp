// ============================================================================
// LeetCode 57 — Insert Interval
// ============================================================================
//
// Problem: You are given an array of non-overlapping intervals, sorted by
// start time, and a new interval. Insert the new interval into the array
// (merging as necessary) and return the array still sorted by start time
// with no overlaps.
//
// Approach (a specialization of the Merge Intervals sweep):
//   Because the input is ALREADY sorted and non-overlapping, we do not need
//   to sort at all — the "sort first" step of the general pattern is already
//   satisfied by the problem's own precondition. We just walk the list once,
//   in three phases:
//     1. Copy every interval that ends strictly before the new interval
//        starts (no overlap possible — they come entirely first).
//     2. While the current interval's start is <= the new interval's
//        (possibly already-grown) end, it overlaps — absorb it by expanding
//        the new interval's start/end to cover it (min of starts, max of
//        ends), and move on. This is the exact same "extend current" idea as
//        Merge Intervals, just applied to one growing interval instead of a
//        list.
//     3. Copy everything left — it all starts strictly after the (now final)
//        new interval ends.
//
// Complexity: O(n) time — a single linear pass, no sort needed because the
// input's sortedness is a given precondition. O(n) space for the output.
//
// Compile:
//   g++ -std=c++17 -Wall 02-insert-interval.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::vector<int>> insert(std::vector<std::vector<int>> intervals,
                                      std::vector<int> newInterval) {
  std::vector<std::vector<int>> result;
  size_t i = 0;
  size_t n = intervals.size();

  // Phase 1: existing intervals entirely before newInterval.
  while (i < n && intervals[i][1] < newInterval[0]) {
    result.push_back(intervals[i]);
    ++i;
  }

  // Phase 2: existing intervals that overlap newInterval — absorb them.
  while (i < n && intervals[i][0] <= newInterval[1]) {
    newInterval[0] = std::min(newInterval[0], intervals[i][0]);
    newInterval[1] = std::max(newInterval[1], intervals[i][1]);
    ++i;
  }
  result.push_back(newInterval);

  // Phase 3: everything left starts strictly after newInterval now ends.
  while (i < n) {
    result.push_back(intervals[i]);
    ++i;
  }

  return result;
}

std::string toString(const std::vector<std::vector<int>>& intervals) {
  std::string out = "[";
  for (size_t i = 0; i < intervals.size(); ++i) {
    out += "[" + std::to_string(intervals[i][0]) + "," +
           std::to_string(intervals[i][1]) + "]";
    if (i + 1 < intervals.size()) out += ",";
  }
  out += "]";
  return out;
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](const std::vector<std::vector<int>>& actual,
                    const std::vector<std::vector<int>>& expected,
                    const std::string& label) {
    bool ok = (actual == expected);
    std::cout << (ok ? "[PASS] " : "[FAIL] ") << label << "\n";
    std::cout << "       got:      " << toString(actual) << "\n";
    std::cout << "       expected: " << toString(expected) << "\n";
    ok ? ++pass_count : ++fail_count;
  };

  check(insert({{1, 3}, {6, 9}}, {2, 5}),
        {{1, 5}, {6, 9}},
        "classic example: new interval merges with one left neighbor");

  check(insert({{1, 2}, {3, 5}, {6, 7}, {8, 10}, {12, 16}}, {4, 8}),
        {{1, 2}, {3, 10}, {12, 16}},
        "new interval absorbs three overlapping neighbors into one");

  check(insert({}, {5, 7}), {{5, 7}}, "insert into an empty list");

  check(insert({{1, 5}}, {2, 3}), {{1, 5}}, "new interval fully inside existing one");

  check(insert({{1, 5}}, {6, 8}), {{1, 5}, {6, 8}}, "new interval starts after everything else, no overlap");

  check(insert({{1, 5}}, {0, 0}), {{0, 0}, {1, 5}}, "new interval starts before everything else, no overlap");

  check(insert({{1, 3}, {6, 9}}, {2, 6}), {{1, 9}}, "new interval bridges both existing intervals via touching endpoints");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
