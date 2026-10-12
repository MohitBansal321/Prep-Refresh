// ============================================================================
// LeetCode 56 — Merge Intervals
// ============================================================================
//
// Problem: Given an array of intervals where intervals[i] = [start_i, end_i],
// merge all overlapping intervals and return an array of the non-overlapping
// intervals that cover all the intervals in the input.
//
// Approach (the pure, textbook Merge Intervals pattern):
//   1. Sort the intervals by start time. This is the pattern's entire
//      "unlock" — once sorted, the only interval that can possibly overlap
//      the one we are building is the very next one in the list, so we never
//      need to look back or compare against every other interval.
//   2. Sweep once, keeping a "current" merged interval. If the next
//      interval's start is <= current's end, they overlap (or touch) — grow
//      current's end to the max of the two. Otherwise current can never grow
//      again, so flush it to the output and start a new current.
//
// This is exactly the mechanism described in ../README.md and implemented
// generically in ../code.cpp; this file reimplements it standalone (no
// #include of code.cpp) so it can be read and compiled in isolation.
//
// Complexity: O(n log n) time (dominated by the sort), O(1) extra space
// beyond the sort's own working space and the output array.
//
// Compile:
//   g++ -std=c++17 -Wall 01-merge-intervals.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


vector<vector<int>> merge(vector<vector<int>> intervals) {
  if (intervals.empty()) return {};

  sort(intervals.begin(), intervals.end(),
            [](const vector<int>& a, const vector<int>& b) {
              return a[0] < b[0];
            });

  vector<vector<int>> result;
  result.push_back(intervals[0]);

  for (size_t i = 1; i < intervals.size(); ++i) {
    vector<int>& current = result.back();
    const vector<int>& next = intervals[i];

    if (next[0] <= current[1]) {
      // Overlap or exact touch: extend current's end.
      current[1] = max(current[1], next[1]);
    } else {
      // No overlap: current is finished, start a new one.
      result.push_back(next);
    }
  }

  return result;
}

string toString(const vector<vector<int>>& intervals) {
  string out = "[";
  for (size_t i = 0; i < intervals.size(); ++i) {
    out += "[" + to_string(intervals[i][0]) + "," +
           to_string(intervals[i][1]) + "]";
    if (i + 1 < intervals.size()) out += ",";
  }
  out += "]";
  return out;
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](const vector<vector<int>>& actual,
                    const vector<vector<int>>& expected,
                    const string& label) {
    bool ok = (actual == expected);
    cout << (ok ? "[PASS] " : "[FAIL] ") << label << "\n";
    cout << "       got:      " << toString(actual) << "\n";
    cout << "       expected: " << toString(expected) << "\n";
    ok ? ++pass_count : ++fail_count;
  };

  check(merge({{1, 3}, {2, 6}, {8, 10}, {15, 18}}),
        {{1, 6}, {8, 10}, {15, 18}},
        "classic example: overlapping middle pair merges");

  check(merge({{1, 4}, {4, 5}}),
        {{1, 5}},
        "touching endpoints [1,4] + [4,5] merge into [1,5]");

  check(merge({{1, 4}, {0, 4}}),
        {{0, 4}},
        "unsorted input (second interval has smaller start) still merges correctly");

  check(merge({{1, 4}, {2, 3}}),
        {{1, 4}},
        "fully-nested interval [2,3] inside [1,4] does not shrink the result");

  check(merge({{1, 2}, {3, 4}, {5, 6}}),
        {{1, 2}, {3, 4}, {5, 6}},
        "already-disjoint intervals pass through unchanged");

  check(merge({}), {}, "empty input produces empty output");

  check(merge({{1, 4}}), {{1, 4}}, "single interval passes through unchanged");

  cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
