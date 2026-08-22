// ============================================================================
// LeetCode 496 — Next Greater Element I
// ============================================================================
//
// PROBLEM
// -------
// The "next greater element" of some element x in an array is the first
// element to x's right that is strictly greater than x. You are given two
// arrays (0-indexed): nums1 and nums2, where nums1 is a SUBSET of nums2.
// For each element in nums1, find its next greater element in nums2
// (or -1 if none exists).
//
// Example: nums1 = [4,1,2], nums2 = [1,3,4,2]
//   - 4's next greater in nums2: nothing to its right is > 4 -> -1
//   - 1's next greater in nums2: 3 -> 3
//   - 2's next greater in nums2: nothing to its right is > 2 -> -1
//   Answer: [-1, 3, -1]
//
// APPROACH — Monotonic stack over nums2 + hash map for O(1) lookups
// ------------------------------------------------------------------
// The key observation: the answer for every element of nums2 can be computed
// for ALL elements at once with a single monotonic-stack pass, regardless of
// which subset nums1 asks about. So we never search per-query; we precompute.
//
// Scan nums2 left to right keeping a stack of indices whose values are still
// waiting for their next greater element. The stack invariant: values are
// non-increasing from bottom to top (each entry survived only because no
// strictly larger value arrived after it). When the incoming value v arrives:
//   - Every stack entry with a SMALLER value has just found its answer — v IS
//     its next greater element. Pop it and record the answer. It can never be
//     resolved by anything farther right, because v is closer.
//   - Then push the incoming index; it starts waiting too.
//
// WHY THIS IS O(n) DESPITE THE INNER WHILE LOOP (amortized argument):
// each index is pushed exactly once and popped at most once over the whole
// run. Total pushes <= n and total pops <= n, so total work <= 2n even though
// any single iteration might pop many entries. The inner loop's cost is
// charged to the pops, not to the outer iterations. A naive per-element scan
// rightward would be O(n^2); this collapses it to O(n).
//
// Finally, nums1 is just a list of lookups into the precomputed answers:
// store them in an unordered_map keyed by VALUE (nums1 queries by value, not
// index), then answer each query in O(1).
//
// COMPLEXITY
// ----------
// Time:  O(n + m) — O(n) for the monotonic pass over nums2 (amortized, see
//        above), O(m) to answer the m queries in nums1.
// Space: O(n) — the stack plus the hash map hold at most n entries each.
// ============================================================================
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<int> nextGreaterElement(const std::vector<int>& nums1,
                                    const std::vector<int>& nums2) {
  // Map: element value in nums2 -> its next greater value (-1 if none).
  std::unordered_map<int, int> next_greater;
  next_greater.reserve(nums2.size() * 2);

  // Stack holds VALUES from nums2 that are still waiting for their next
  // greater element. Values are non-increasing bottom-to-top. (Storing values
  // instead of indices is fine here because we only ever need the value as
  // the map key — no position-dependent work happens at pop time.)
  std::vector<int> waiting;
  waiting.reserve(nums2.size());

  for (int v : nums2) {
    // Everything smaller than v has been resolved BY v: v is closer than any
    // future candidate could be, so pop it now and record the answer.
    while (!waiting.empty() && waiting.back() < v) {
      next_greater[waiting.back()] = v;
      waiting.pop_back();
    }
    // v itself now waits for something strictly greater to its right.
    waiting.push_back(v);
  }
  // Anything left on the stack never found a next greater element. Rather
  // than recording -1 for each explicitly, we rely on the map's default: use
  // count()/find() or pre-fill. We choose explicit clarity below via find().

  std::vector<int> result;
  result.reserve(nums1.size());
  for (int q : nums1) {
    auto it = next_greater.find(q);
    result.push_back(it == next_greater.end() ? -1 : it->second);
  }
  return result;
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
    std::vector<int> nums1 = {4, 1, 2};
    std::vector<int> nums2 = {1, 3, 4, 2};
    std::vector<int> expected = {-1, 3, -1};
    check(nextGreaterElement(nums1, nums2) == expected,
          "[4,1,2] in [1,3,4,2] -> [-1,3,-1]");
  }

  {
    std::vector<int> nums1 = {2, 4};
    std::vector<int> nums2 = {1, 2, 3, 4};
    std::vector<int> expected = {3, -1};
    check(nextGreaterElement(nums1, nums2) == expected,
          "[2,4] in [1,2,3,4] -> [3,-1]");
  }

  {
    // Single element: no right neighbor exists at all.
    std::vector<int> nums1 = {7};
    std::vector<int> nums2 = {7};
    std::vector<int> expected = {-1};
    check(nextGreaterElement(nums1, nums2) == expected,
          "single element -> [-1]");
  }

  {
    // Strictly decreasing nums2: nothing ever pops; every query is -1.
    // This is also the worst case for stack space (all n entries pile up).
    std::vector<int> nums1 = {5, 4, 3};
    std::vector<int> nums2 = {5, 4, 3, 2, 1};
    std::vector<int> expected = {-1, -1, -1};
    check(nextGreaterElement(nums1, nums2) == expected,
          "strictly decreasing nums2 -> all -1");
  }

  {
    // Strictly increasing nums2: each new element pops exactly one previous;
    // only the last element gets -1.
    std::vector<int> nums1 = {1, 4};
    std::vector<int> nums2 = {1, 2, 3, 4};
    std::vector<int> expected = {2, -1};
    check(nextGreaterElement(nums1, nums2) == expected,
          "increasing chain resolves stepwise -> [2,-1]");
  }

  {
    // Duplicates in nums2: equal values coexist on the stack (strict '<'
    // comparison); both copies resolve together when a larger value arrives.
    std::vector<int> nums1 = {2, 3};
    std::vector<int> nums2 = {2, 2, 3};
    std::vector<int> expected = {3, -1};
    check(nextGreaterElement(nums1, nums2) == expected,
          "duplicates resolve together -> [3,-1]");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
