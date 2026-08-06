// ============================================================================
// LeetCode 167 — Two Sum II - Input Array Is Sorted
// ============================================================================
//
// PROBLEM
// -------
// Given a 1-indexed array of integers `numbers` that is already sorted in
// non-decreasing order, find two numbers such that they add up to a specific
// `target` number. Return the 1-indexed indices of the two numbers (index1,
// index2), with index1 < index2. Exactly one solution is guaranteed to
// exist, and you may not use the same element twice.
//
// Example: numbers = [2, 7, 11, 15], target = 9  ->  [1, 2]  (2 + 7 = 9)
//
// APPROACH — Two Pointers (converging, on sorted data)
// -----------------------------------------------------
// This is the canonical two-pointers problem: the array is ALREADY sorted,
// and we need a pair whose sum matches a target. That combination — sorted
// input + pair-sum search — is exactly the recognition signal for this
// pattern (see ../README.md and ../images/recognition-diagram.md).
//
// Place `left` at index 0 and `right` at the last index. At each step:
//   - if numbers[left] + numbers[right] == target: found it, stop.
//   - if the sum is TOO SMALL: we need a bigger sum. Since the array is
//     sorted ascending, the only way to increase the sum is to move `left`
//     forward to a larger value. Moving `right` backward would only shrink
//     the sum further — it can never help, so we never even consider it.
//   - if the sum is TOO LARGE: symmetric argument — move `right` backward
//     to a smaller value.
//
// Why this cannot miss the answer: at every step we discard exactly one
// index, and we only ever discard an index that PROVABLY cannot participate
// in the (unique) solution given the current bounds. This is the same
// argument as a two-sided binary search collapsing the search space by one
// element per step, except here we make monotonic progress toward a sum
// rather than toward a single sorted position.
//
// COMPLEXITY
// ----------
// Time:  O(n) — each of `left` and `right` moves at most n times combined
//               before they meet; every step does O(1) work.
// Space: O(1) — only two index variables, no auxiliary structure.
//
// Contrast with brute force (nested loop over all pairs): O(n^2) time.
// Contrast with hashing (store seen values, look up the complement):
// O(n) time but O(n) extra space for the hash set — two pointers gets the
// same time complexity in O(1) space by exploiting the sort order that a
// hash-based solution would throw away.
// ============================================================================

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Returns 1-indexed {index1, index2} with index1 < index2 such that
// numbers[index1-1] + numbers[index2-1] == target.
// Throws std::invalid_argument if no such pair exists (should not happen
// per problem's guarantee, but we do not silently return garbage).
std::vector<int> twoSum(const std::vector<int>& numbers, int target) {
  int left = 0;
  int right = static_cast<int>(numbers.size()) - 1;

  while (left < right) {
    // Use a wider type for the sum to avoid overflow if numbers are near
    // INT_MAX; LeetCode's constraints make plain int safe here, but adding
    // long long costs nothing and removes the overflow question entirely.
    long long sum = static_cast<long long>(numbers[left]) + numbers[right];

    if (sum == target) {
      return {left + 1, right + 1};  // Convert to 1-indexed per problem spec.
    } else if (sum < target) {
      ++left;   // Need a larger sum -> advance the low pointer.
    } else {
      --right;  // Need a smaller sum -> retreat the high pointer.
    }
  }

  throw std::invalid_argument("No valid pair found for the given target.");
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
    std::vector<int> numbers = {2, 7, 11, 15};
    auto result = twoSum(numbers, 9);
    std::vector<int> expected = {1, 2};
    check(result == expected, "[2,7,11,15], target 9 -> [1,2]");
  }

  {
    std::vector<int> numbers = {2, 3, 4};
    auto result = twoSum(numbers, 6);
    std::vector<int> expected = {1, 3};
    check(result == expected, "[2,3,4], target 6 -> [1,3]");
  }

  {
    std::vector<int> numbers = {-1, 0};
    auto result = twoSum(numbers, -1);
    std::vector<int> expected = {1, 2};
    check(result == expected, "[-1,0], target -1 -> [1,2]");
  }

  {
    // Requires several steps of both pointers moving before converging:
    // right retreats twice (15, 14 too large), then left advances three
    // times (1, 2, 3 too small) before landing on 4 + 9 = 13.
    std::vector<int> numbers = {1, 2, 3, 4, 6, 8, 9, 14, 15};
    auto result = twoSum(numbers, 13);
    std::vector<int> expected = {4, 7};  // numbers[3]=4, numbers[6]=9 -> 13
    check(result == expected, "longer array, multi-step convergence -> [4,7]");
  }

  {
    bool threw = false;
    try {
      std::vector<int> numbers = {1, 2, 3};
      twoSum(numbers, 100);
    } catch (const std::invalid_argument&) {
      threw = true;
    }
    check(threw, "no valid pair -> throws std::invalid_argument");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
