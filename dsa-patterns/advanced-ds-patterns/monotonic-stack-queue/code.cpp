// ============================================================================
// Monotonic Stack/Queue — generic reusable template (C++17)
// ============================================================================
//
// nextGreaterElement: monotonic stack, O(n) total.
// maxSlidingWindow: monotonic deque, O(n) total.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <deque>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// For each index, the next element to its right that is strictly greater,
// or -1 if none exists. Stack holds indices whose values increase from
// bottom to top; a new larger value pops (and resolves) everything smaller.
// ----------------------------------------------------------------------------
std::vector<int> nextGreaterElement(const std::vector<int>& nums) {
  int n = static_cast<int>(nums.size());
  std::vector<int> result(n, -1);
  std::vector<int> stack;  // holds indices; values increase bottom-to-top

  for (int i = 0; i < n; ++i) {
    while (!stack.empty() && nums[stack.back()] < nums[i]) {
      result[stack.back()] = nums[i];
      stack.pop_back();
    }
    stack.push_back(i);
  }

  return result;
}

// ----------------------------------------------------------------------------
// Maximum of every sliding window of size k. Deque holds indices whose
// values strictly decrease from front to back; the front is always the
// current window's maximum.
// ----------------------------------------------------------------------------
std::vector<int> maxSlidingWindow(const std::vector<int>& nums, int k) {
  std::deque<int> dq;
  std::vector<int> result;

  for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
    while (!dq.empty() && nums[dq.back()] < nums[i]) dq.pop_back();
    dq.push_back(i);

    if (dq.front() <= i - k) dq.pop_front();  // fell out of the window

    if (i >= k - 1) result.push_back(nums[dq.front()]);
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
    std::vector<int> nums = {2, 1, 2, 4, 3};
    std::vector<int> expected = {4, 2, 4, -1, -1};
    check(nextGreaterElement(nums) == expected,
          "nextGreaterElement([2,1,2,4,3]) -> [4,2,4,-1,-1]");
  }
  {
    std::vector<int> nums = {5, 4, 3, 2, 1};
    std::vector<int> expected = {-1, -1, -1, -1, -1};
    check(nextGreaterElement(nums) == expected,
          "nextGreaterElement(strictly decreasing) -> all -1");
  }

  {
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    std::vector<int> expected = {3, 3, 5, 5, 6, 7};
    check(maxSlidingWindow(nums, 3) == expected,
          "maxSlidingWindow(k=3) classic example -> [3,3,5,5,6,7]");
  }
  {
    std::vector<int> nums = {1, -1};
    std::vector<int> expected = {1, -1};
    check(maxSlidingWindow(nums, 1) == expected,
          "maxSlidingWindow(k=1) -> window max equals each element");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
