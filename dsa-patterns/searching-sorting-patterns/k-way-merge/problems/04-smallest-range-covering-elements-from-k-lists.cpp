// ============================================================================
// LeetCode 632 — Smallest Range Covering Elements from K Lists (Hard)
// ============================================================================
//
// PROBLEM
// -------
// You have k lists of sorted integers. Find the SMALLEST range that
// includes at least one number from each of the k lists.
//
// A range [a, b] (a <= b) is said to cover the k lists if, for every list,
// there is at least one number in that list that lies within [a, b].
// If there are multiple smallest ranges of the same length, return the one
// with the smallest starting number `a`.
//
// Example:
//   lists = [[4,10,15,24,26], [0,9,12,20], [5,18,22,30]]
//   -> [20, 24]
//   Explanation: [20,24] contains 24 (list 1), 20 (list 2), 22 (list 3).
//
// APPROACH — K-way Merge, tracking a running max alongside the heap
// -----------------------------------------------------------------------------
// This is the hardest variant in this module because it extends the base
// K-way Merge pattern with one extra piece of tracked state.
//
// Seed a min-heap with the FIRST (smallest) element of every list, tagged
// with (list_index, element_index) exactly as in ../code.cpp. At every
// point in the merge, the heap's top (smallest current candidate) and a
// separately tracked `current_max` (the LARGEST current candidate across
// all k lists' current positions) together define a valid candidate range:
// [heap_top_value, current_max] is guaranteed to contain at least one
// element from every list, because every list's current position IS one
// of the values between those two bounds (the heap top is the smallest of
// them, current_max is the largest).
//
// On every pop:
//   1. The popped (smallest) value and the tracked current_max form one
//      candidate range. Compare its width (current_max - popped_value) to
//      the best range found so far; update if this one is smaller (or
//      equally small but starts earlier — though with a min-heap breaking
//      ties by increasing start value naturally, the first range found at
//      a given width is already the earliest-starting one).
//   2. Advance that SAME list's pointer (the base pattern's replenishment
//      step). If that list has a next element, push it and update
//      current_max = max(current_max, that new value) -- pushing a new
//      candidate can only ever raise (or leave unchanged) the maximum,
//      never lower it.
//   3. If that list has NO next element, we must STOP: once any single
//      list runs out of elements, no future range can possibly cover it
//      (there is nothing left in that list to include), so no better
//      answer can exist beyond this point.
//
// COMPLEXITY
// ----------
// Let n = total number of elements across all k lists.
// Time:  O(n log k) — each of the (at most) n pops/pushes is an O(log k)
//        heap operation, since the heap never holds more than k entries
//        (one per still-active list).
// Space: O(k) for the heap.
//
// Contrast with brute force (try every possible pair of "which element from
// each list to include" combinatorially): exponential in k — utterly
// infeasible beyond a handful of lists. The heap-based approach is the only
// practical option once k grows past a small constant.
// ============================================================================

#include <climits>
#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

using HeapEntry = std::tuple<int, int, int>;  // {value, list_index, element_index}
using MinHeap = std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<>>;

std::vector<int> smallestRange(const std::vector<std::vector<int>>& lists) {
  MinHeap heap;
  int current_max = INT_MIN;
  const int k = static_cast<int>(lists.size());

  // Seed: first (smallest) element of every list, and track the largest
  // among those seeds as current_max.
  for (int list_index = 0; list_index < k; ++list_index) {
    if (lists[list_index].empty()) {
      // A list with no elements at all can never be covered by any range;
      // per this problem's constraints every list is guaranteed non-empty,
      // but guarding here avoids silently producing a wrong answer if that
      // guarantee were ever violated.
      return {};
    }
    heap.emplace(lists[list_index][0], list_index, 0);
    current_max = std::max(current_max, lists[list_index][0]);
  }

  int best_start = 0;
  int best_end = 0;
  bool has_answer = false;  // avoids seeding best_width via INT_MAX - INT_MIN,
                             // which overflows a 32-bit int (undefined behavior)

  while (true) {
    auto [smallest_value, list_index, element_index] = heap.top();
    heap.pop();

    // Candidate range: [smallest_value, current_max] is guaranteed to touch
    // every list, because every list's current position lies in between.
    const int candidate_width = current_max - smallest_value;
    if (!has_answer || candidate_width < best_end - best_start) {
      best_start = smallest_value;
      best_end = current_max;
      has_answer = true;
    }
    // Note: with a min-heap, smallest_value only ever increases (or stays
    // the same) across iterations, so the first range recorded at a given
    // width is already the one with the smallest starting value -- no
    // separate tie-break logic is needed for "smallest a on equal width."

    // Advance the source pointer for this SAME list.
    const int next_index = element_index + 1;
    if (next_index >= static_cast<int>(lists[list_index].size())) {
      // This list is now exhausted: no future range can include anything
      // from it, so no better answer can exist beyond what we've already
      // found. Stop here.
      break;
    }

    const int next_value = lists[list_index][next_index];
    heap.emplace(next_value, list_index, next_index);
    current_max = std::max(current_max, next_value);
  }

  return {best_start, best_end};
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

  // ---- Test 1: the classic LeetCode example -----------------------------------
  {
    std::vector<std::vector<int>> lists = {
        {4, 10, 15, 24, 26},
        {0, 9, 12, 20},
        {5, 18, 22, 30},
    };
    auto result = smallestRange(lists);
    std::vector<int> expected = {20, 24};
    check(result == expected, "classic example -> [20, 24]");
  }

  // ---- Test 2: single-element lists (the range must span all of them) --------
  {
    std::vector<std::vector<int>> lists = {{1}, {100}, {50}};
    auto result = smallestRange(lists);
    std::vector<int> expected = {1, 100};
    check(result == expected, "single-element lists -> range spans min to max");
  }

  // ---- Test 3: all lists identical (a range of width 0 should be possible) ---
  {
    std::vector<std::vector<int>> lists = {{5, 10}, {5, 10}, {5, 10}};
    auto result = smallestRange(lists);
    std::vector<int> expected = {5, 5};
    check(result == expected, "identical lists -> range width 0 at the shared value");
  }

  // ---- Test 4: two lists, disjoint ranges of values ----------------------------
  {
    std::vector<std::vector<int>> lists = {{1, 2, 3}, {100, 200, 300}};
    auto result = smallestRange(lists);
    // Best range must include one value from each: options are e.g.
    // [3,100] (width 97) is the narrowest since 3 is closest to 100.
    std::vector<int> expected = {3, 100};
    check(result == expected, "disjoint value ranges -> tightest bridging pair [3,100]");
  }

  // ---- Test 5: k = 1 (a single list; the range is a single point) -------------
  {
    std::vector<std::vector<int>> lists = {{7, 15, 22}};
    auto result = smallestRange(lists);
    // With only one list, the smallest covering range is any single value
    // from it; the algorithm should report the first (smallest) element,
    // width 0, since it's the first candidate the heap pops.
    std::vector<int> expected = {7, 7};
    check(result == expected, "k=1 -> width-0 range at the list's smallest element");
  }

  // ---- Test 6: lists of uneven length, needing several advances ---------------
  {
    std::vector<std::vector<int>> lists = {
        {1, 3, 5, 7, 9},
        {2, 4},
        {6, 8, 10, 12, 14, 16},
    };
    auto result = smallestRange(lists);
    // Sanity-check structural properties rather than hand-deriving the
    // exact expected range by eye: the returned range must be non-empty
    // and its start must not exceed its end.
    check(result.size() == 2 && result[0] <= result[1],
          "uneven-length lists -> produces a valid, well-formed range");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
