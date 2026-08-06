// ============================================================================
// K-way Merge — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// two things you will re-derive on almost every problem that fits this
// pattern:
//
//   1. MERGE EVERYTHING — seed a min-heap with the first element of each of
//      K already-sorted lists, then repeatedly pop the smallest and push the
//      next element from that same source list, until the heap is empty.
//      The output comes out fully sorted, in O(n log k) time.
//
//   2. FIND THE K-TH SMALLEST — identical seeding and replenishment, but
//      stop as soon as the k-th value has been popped instead of building
//      the entire merged output. Useful when k is much smaller than the
//      total element count n.
//
// Both variants are expressed as small, generic, reusable functions so you
// can see the *shape* of the pattern independent of any one problem. The
// worked, problem-specific solutions live in problems/*.cpp.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_kwm_code && /tmp/out_kwm_code
// ============================================================================

#include <iostream>
#include <optional>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <vector>

// ----------------------------------------------------------------------------
// The heap entry shared by both functions below: {value, list_index,
// element_index}. std::tuple's comparison operators compare element-by-
// element in declaration order, so ordering these tuples orders primarily by
// `value` (exactly what merging needs) and only falls back to list_index /
// element_index to break ties between equal values — a tie-break that never
// changes correctness, since any order among equal values is a valid sorted
// output.
// ----------------------------------------------------------------------------
using HeapEntry = std::tuple<int, int, int>;  // {value, list_index, element_index}

// A min-heap: std::priority_queue is a MAX-heap by default, so we must
// explicitly supply std::greater<> as the comparator to invert it. Forgetting
// this is the single most common bug when building a min-heap in C++ (see
// the README's Common Mistakes section).
using MinHeap = std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<>>;

// ----------------------------------------------------------------------------
// Generic template #1: merge K already-sorted lists into one sorted vector.
//
// Seeds the heap with the first element of every non-empty list, then
// repeatedly pops the smallest current candidate, appends its value to the
// result, and pushes the next element from that SAME source list (if one
// remains). This is the "advance the source pointer" step described in the
// README's Architecture section — forgetting it silently drops elements.
// ----------------------------------------------------------------------------
std::vector<int> mergeKSortedLists(const std::vector<std::vector<int>>& lists) {
  MinHeap heap;
  const int k = static_cast<int>(lists.size());

  // Seed: push the first element of every non-empty list, tagged with its
  // own list index and starting position 0.
  for (int list_index = 0; list_index < k; ++list_index) {
    if (!lists[list_index].empty()) {
      heap.emplace(lists[list_index][0], list_index, 0);
    }
  }

  std::vector<int> result;
  result.reserve(64);  // Small non-zero hint; exact total size isn't known
                        // up front without a separate pass over all lists.

  while (!heap.empty()) {
    auto [value, list_index, element_index] = heap.top();
    heap.pop();
    result.push_back(value);

    // Replenish: if this source list has a next element, push it. This is
    // the ONLY place a new candidate for `list_index` can ever come from —
    // skipping this check for even one popped entry means that list's
    // remaining elements are lost forever.
    const int next_index = element_index + 1;
    if (next_index < static_cast<int>(lists[list_index].size())) {
      heap.emplace(lists[list_index][next_index], list_index, next_index);
    }
  }

  return result;
}

// ----------------------------------------------------------------------------
// Generic template #2: find the k-th smallest element across K already-
// sorted lists, WITHOUT merging everything.
//
// Identical heap setup and replenishment logic to mergeKSortedLists, but
// instead of accumulating a result vector, counts pops and returns as soon
// as the k-th value has been popped. This early exit matters when k is much
// smaller than the total element count n: we never pay to merge the
// remaining n - k elements we were never going to look at.
//
// k is 1-indexed (k = 1 means "the smallest element overall").
// Returns std::nullopt if k exceeds the total number of elements available.
// ----------------------------------------------------------------------------
std::optional<int> kthSmallestInKSortedLists(const std::vector<std::vector<int>>& lists, int k) {
  if (k <= 0) return std::nullopt;

  MinHeap heap;
  const int num_lists = static_cast<int>(lists.size());

  for (int list_index = 0; list_index < num_lists; ++list_index) {
    if (!lists[list_index].empty()) {
      heap.emplace(lists[list_index][0], list_index, 0);
    }
  }

  int popped_count = 0;
  while (!heap.empty()) {
    auto [value, list_index, element_index] = heap.top();
    heap.pop();
    ++popped_count;

    if (popped_count == k) {
      return value;  // Early exit: stop the moment we have the k-th value.
    }

    const int next_index = element_index + 1;
    if (next_index < static_cast<int>(lists[list_index].size())) {
      heap.emplace(lists[list_index][next_index], list_index, next_index);
    }
  }

  return std::nullopt;  // k exceeded the total number of elements available.
}

// ----------------------------------------------------------------------------
// Small helper for readable test output.
// ----------------------------------------------------------------------------
void print_vector(const std::vector<int>& v) {
  std::cout << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    std::cout << v[i];
    if (i + 1 < v.size()) std::cout << ", ";
  }
  std::cout << "]";
}

int main() {
  std::cout << "=== K-way Merge: generic template demo ===\n\n";

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

  // ---- Test 1: three sorted lists of equal length ---------------------------
  {
    std::vector<std::vector<int>> lists = {{1, 4, 7}, {2, 5, 8}, {3, 6, 9}};
    auto merged = mergeKSortedLists(lists);
    std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::cout << "Test 1: merge [1,4,7], [2,5,8], [3,6,9] -> ";
    print_vector(merged);
    std::cout << "\n";
    check(merged == expected, "three equal-length lists merge correctly");
  }

  // ---- Test 2: lists of uneven length ----------------------------------------
  {
    std::vector<std::vector<int>> lists = {{1, 10, 20, 30}, {2}, {3, 4, 5}};
    auto merged = mergeKSortedLists(lists);
    std::vector<int> expected = {1, 2, 3, 4, 5, 10, 20, 30};
    std::cout << "Test 2: merge uneven-length lists -> ";
    print_vector(merged);
    std::cout << "\n";
    check(merged == expected, "uneven-length lists merge correctly");
  }

  // ---- Test 3: some lists are empty ------------------------------------------
  {
    std::vector<std::vector<int>> lists = {{}, {5, 6}, {}, {1, 2, 3}};
    auto merged = mergeKSortedLists(lists);
    std::vector<int> expected = {1, 2, 3, 5, 6};
    std::cout << "Test 3: merge with some empty lists -> ";
    print_vector(merged);
    std::cout << "\n";
    check(merged == expected, "empty lists are skipped without affecting the result");
  }

  // ---- Test 4: all lists empty ------------------------------------------------
  {
    std::vector<std::vector<int>> lists = {{}, {}, {}};
    auto merged = mergeKSortedLists(lists);
    check(merged.empty(), "all-empty input produces an empty merged result");
  }

  // ---- Test 5: single list (K = 1) --------------------------------------------
  {
    std::vector<std::vector<int>> lists = {{1, 2, 3}};
    auto merged = mergeKSortedLists(lists);
    std::vector<int> expected = {1, 2, 3};
    check(merged == expected, "K=1 degenerates to returning the single list unchanged");
  }

  // ---- Test 6: duplicate values across lists ---------------------------------
  {
    std::vector<std::vector<int>> lists = {{1, 3, 3}, {2, 3, 4}};
    auto merged = mergeKSortedLists(lists);
    std::vector<int> expected = {1, 2, 3, 3, 3, 4};
    std::cout << "Test 6: merge with duplicate values -> ";
    print_vector(merged);
    std::cout << "\n";
    check(merged == expected, "duplicate values across lists are preserved and ordered");
  }

  // ---- Test 7: kth smallest, early exit before heap empties ------------------
  {
    std::vector<std::vector<int>> lists = {{1, 5, 9}, {2, 6, 10}, {3, 7, 11}};
    // Full merge would be [1,2,3,5,6,7,9,10,11]; k=4 -> 5.
    auto result = kthSmallestInKSortedLists(lists, 4);
    check(result.has_value() && *result == 5, "kth smallest (k=4) -> 5");
  }

  // ---- Test 8: kth smallest, k = 1 (smallest overall) ------------------------
  {
    std::vector<std::vector<int>> lists = {{10, 20}, {1, 30}, {5, 6}};
    auto result = kthSmallestInKSortedLists(lists, 1);
    check(result.has_value() && *result == 1, "kth smallest (k=1) -> smallest overall value");
  }

  // ---- Test 9: kth smallest, k larger than total element count --------------
  {
    std::vector<std::vector<int>> lists = {{1, 2}, {3}};
    auto result = kthSmallestInKSortedLists(lists, 10);
    check(!result.has_value(), "kth smallest with out-of-range k returns nullopt");
  }

  // ---- Test 10: kth smallest, k equal to total element count -----------------
  {
    std::vector<std::vector<int>> lists = {{1, 4}, {2, 3}};
    // Full merge is [1,2,3,4]; k=4 -> the largest element, 4.
    auto result = kthSmallestInKSortedLists(lists, 4);
    check(result.has_value() && *result == 4, "kth smallest (k = total count) -> largest element");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
