// ============================================================================
// Merge Intervals — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// two operations that almost every "list of [start, end] ranges" problem
// reduces to:
//
//   1. mergeIntervals — given an unordered list of intervals, sort by start
//      time and sweep once, merging any that overlap. This is the mechanical
//      core of the whole pattern.
//
//   2. insertInterval — given an ALREADY-sorted, non-overlapping list plus one
//      new interval to add, splice the new interval in and merge whatever it
//      now touches, all in a single O(n) pass (no full re-sort needed, since
//      the list was already sorted before the insert).
//
// Both functions are expressed generically over std::pair<int,int> so you can
// see the *shape* of the pattern independent of any one problem. The worked,
// problem-specific solutions live in problems/*.cpp.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// mergeIntervals
//
// Takes intervals in ANY order (each a std::pair<int,int> meaning [start,
// end], inclusive on both ends) and returns a new list where every interval
// is disjoint from every other, sorted by start time.
//
// Why sort first: the merge decision at each step ("does the next interval
// overlap the one I am currently building?") is only a single O(1) comparison
// if we can guarantee the next interval we look at has the smallest remaining
// start time. Sorting buys us that guarantee once, up front, for the price of
// O(n log n) — after which the sweep itself is O(n).
//
// The sweep keeps exactly one "current merged interval" alive at a time:
//   - If the next interval's start is <= current.end (they touch or overlap,
//     since intervals are treated as closed/inclusive), we EXTEND current by
//     taking the max of the two ends.
//   - Otherwise the next interval starts strictly after current ends, so
//     current can never be touched again (everything after it, thanks to the
//     sort, only has even larger start values) — we FLUSH current to the
//     output and make the next interval the new current.
// ----------------------------------------------------------------------------
std::vector<std::pair<int, int>> mergeIntervals(
    std::vector<std::pair<int, int>> intervals) {
  if (intervals.empty()) {
    return {};
  }

  // Sort by start time. This single sort is what turns an O(n^2) pairwise
  // overlap check into an O(n) linear sweep.
  std::sort(intervals.begin(), intervals.end(),
            [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
              return a.first < b.first;
            });

  std::vector<std::pair<int, int>> merged;
  merged.reserve(intervals.size());

  // "current" is the one interval we are actively growing. It starts as the
  // first (smallest-start) interval after sorting.
  std::pair<int, int> current = intervals[0];

  for (size_t i = 1; i < intervals.size(); ++i) {
    const std::pair<int, int>& next = intervals[i];

    if (next.first <= current.second) {
      // Overlap (or exact touch, e.g. [1,3] and [3,5] -> [1,5]): extend
      // current's end. We never shrink current.second, hence the max().
      current.second = std::max(current.second, next.second);
    } else {
      // No overlap: current can never grow again (every interval after
      // `next`, thanks to sorting, has an even larger start). Flush it and
      // start a new "current" at `next`.
      merged.push_back(current);
      current = next;
    }
  }

  // The loop above never flushes the final "current" — flush it now.
  merged.push_back(current);

  return merged;
}

// ----------------------------------------------------------------------------
// insertInterval
//
// Given a list that is ALREADY sorted by start and already non-overlapping
// (the postcondition of mergeIntervals), plus one new interval to insert,
// produce the new sorted, non-overlapping list in a single O(n) pass.
//
// Why this does not need mergeIntervals' O(n log n) sort: the existing list
// is already ordered, so we can walk it once and classify every existing
// interval into exactly one of three buckets, in order:
//   1. Ends strictly before newInterval starts -> no overlap, copy as-is.
//   2. Overlaps newInterval (touches or crosses) -> absorb it by expanding
//      newInterval's own bounds (min of starts, max of ends).
//   3. Starts strictly after newInterval ends -> no overlap, copy as-is.
// Because the input is sorted, once we leave bucket 1 we are in bucket 2
// until we leave it into bucket 3 — no interval can jump back a bucket.
// ----------------------------------------------------------------------------
std::vector<std::pair<int, int>> insertInterval(
    const std::vector<std::pair<int, int>>& sortedNonOverlapping,
    std::pair<int, int> newInterval) {
  std::vector<std::pair<int, int>> result;
  result.reserve(sortedNonOverlapping.size() + 1);

  size_t i = 0;
  size_t n = sortedNonOverlapping.size();

  // Bucket 1: existing intervals that end before newInterval even starts.
  while (i < n && sortedNonOverlapping[i].second < newInterval.first) {
    result.push_back(sortedNonOverlapping[i]);
    ++i;
  }

  // Bucket 2: existing intervals that overlap newInterval. Absorb each one
  // into newInterval's bounds instead of emitting it directly.
  while (i < n && sortedNonOverlapping[i].first <= newInterval.second) {
    newInterval.first = std::min(newInterval.first, sortedNonOverlapping[i].first);
    newInterval.second = std::max(newInterval.second, sortedNonOverlapping[i].second);
    ++i;
  }
  result.push_back(newInterval);

  // Bucket 3: everything left starts strictly after newInterval now ends.
  while (i < n) {
    result.push_back(sortedNonOverlapping[i]);
    ++i;
  }

  return result;
}

// ----------------------------------------------------------------------------
// Small helper for printing intervals during the demo below.
// ----------------------------------------------------------------------------
std::string toString(const std::vector<std::pair<int, int>>& intervals) {
  std::string out = "[";
  for (size_t i = 0; i < intervals.size(); ++i) {
    out += "[" + std::to_string(intervals[i].first) + "," +
           std::to_string(intervals[i].second) + "]";
    if (i + 1 < intervals.size()) out += ", ";
  }
  out += "]";
  return out;
}

// ============================================================================
// main() — demonstrates both functions with printed, verifiable output.
// ============================================================================
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

  std::cout << "--- mergeIntervals ---\n";
  {
    std::vector<std::pair<int, int>> input = {{1, 3}, {8, 10}, {2, 6}, {15, 18}};
    auto result = mergeIntervals(input);
    std::vector<std::pair<int, int>> expected = {{1, 6}, {8, 10}, {15, 18}};
    std::cout << "  input:    " << toString(input) << "\n";
    std::cout << "  merged:   " << toString(result) << "\n";
    check(result == expected, "unsorted overlapping intervals merge correctly");
  }
  {
    // Fully non-overlapping input should come back untouched (aside from the
    // sort, which is already a no-op here).
    std::vector<std::pair<int, int>> input = {{1, 2}, {3, 4}, {5, 6}};
    auto result = mergeIntervals(input);
    check(result == input, "already-disjoint intervals are unchanged");
  }
  {
    // Touching endpoints ([1,4] and [4,5]) count as overlapping because
    // intervals are treated as closed (inclusive) on both ends.
    std::vector<std::pair<int, int>> input = {{1, 4}, {4, 5}};
    auto result = mergeIntervals(input);
    std::vector<std::pair<int, int>> expected = {{1, 5}};
    check(result == expected, "touching endpoints [1,4] + [4,5] merge into [1,5]");
  }
  {
    // A fully-nested interval must not stretch the outer one's end backward.
    std::vector<std::pair<int, int>> input = {{1, 10}, {2, 3}};
    auto result = mergeIntervals(input);
    std::vector<std::pair<int, int>> expected = {{1, 10}};
    check(result == expected, "fully-nested interval collapses into the outer one");
  }
  {
    std::vector<std::pair<int, int>> empty_input = {};
    auto result = mergeIntervals(empty_input);
    check(result.empty(), "empty input -> empty output");
  }

  std::cout << "\n--- insertInterval ---\n";
  {
    std::vector<std::pair<int, int>> existing = {{1, 3}, {6, 9}};
    auto result = insertInterval(existing, {2, 5});
    std::vector<std::pair<int, int>> expected = {{1, 5}, {6, 9}};
    std::cout << "  existing: " << toString(existing) << ", insert [2,5]\n";
    std::cout << "  result:   " << toString(result) << "\n";
    check(result == expected, "insert overlapping interval merges with left neighbor");
  }
  {
    std::vector<std::pair<int, int>> existing = {
        {1, 2}, {3, 5}, {6, 7}, {8, 10}, {12, 16}};
    auto result = insertInterval(existing, {4, 8});
    std::vector<std::pair<int, int>> expected = {{1, 2}, {3, 10}, {12, 16}};
    check(result == expected, "insert absorbs three overlapping neighbors into one");
  }
  {
    // New interval fits entirely in a gap: no merging needed at all.
    std::vector<std::pair<int, int>> existing = {{1, 2}, {8, 10}};
    auto result = insertInterval(existing, {4, 6});
    std::vector<std::pair<int, int>> expected = {{1, 2}, {4, 6}, {8, 10}};
    check(result == expected, "insert into a gap with no overlap stays separate");
  }
  {
    // New interval starts after everything else ends.
    std::vector<std::pair<int, int>> existing = {{1, 2}, {3, 4}};
    auto result = insertInterval(existing, {5, 7});
    std::vector<std::pair<int, int>> expected = {{1, 2}, {3, 4}, {5, 7}};
    check(result == expected, "insert appended after the last interval");
  }
  {
    std::vector<std::pair<int, int>> empty_existing = {};
    auto result = insertInterval(empty_existing, {5, 7});
    std::vector<std::pair<int, int>> expected = {{5, 7}};
    check(result == expected, "insert into an empty list produces just the new interval");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
