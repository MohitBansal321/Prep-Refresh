// ============================================================================
// LeetCode 238 - Product of Array Except Self
// ============================================================================
// Problem: given an integer array nums, return an array answer such that
// answer[i] is the product of all elements of nums except nums[i]. Must run
// in O(n) time and must not use division; per the problem's follow-up, the
// returned output array does not count toward the O(1) extra-space goal.
//
// Approach (Prefix/Suffix Product -- the "product" sibling of prefix sum):
// answer[i] = (product of everything to the LEFT of i) * (product of
// everything to the RIGHT of i). Build a prefix-product pass left-to-right
// (answer[i] initially holds the product of nums[0 .. i-1]), then a second
// pass right-to-left multiplies in the running suffix product (product of
// nums[i+1 .. n-1]) using a single accumulator variable instead of a second
// full array -- exactly the same "precompute a running aggregate, then
// combine two precomputed values" idea as prefix sum, just with
// multiplication instead of addition.
//
// Complexity: O(n) time (two passes), O(1) EXTRA space beyond the required
// output array (the running suffix product is a single accumulator, not a
// second array).
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

std::vector<long long> productExceptSelf(const std::vector<int>& nums) {
  const size_t n = nums.size();
  std::vector<long long> answer(n, 1);

  // Pass 1 (left to right): answer[i] = product of nums[0 .. i-1].
  long long prefix_product = 1;
  for (size_t i = 0; i < n; ++i) {
    answer[i] = prefix_product;
    prefix_product *= nums[i];
  }

  // Pass 2 (right to left): multiply in the product of nums[i+1 .. n-1],
  // tracked with a single running accumulator -- no second array needed.
  long long suffix_product = 1;
  for (size_t i = n; i-- > 0;) {
    answer[i] *= suffix_product;
    suffix_product *= nums[i];
  }

  return answer;
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

  auto to_string_vec = [](const std::vector<long long>& v) {
    std::string s = "[";
    for (size_t i = 0; i < v.size(); ++i) {
      s += std::to_string(v[i]);
      if (i + 1 < v.size()) s += ", ";
    }
    return s + "]";
  };

  {
    std::vector<int> nums = {1, 2, 3, 4};
    std::vector<long long> expected = {24, 12, 8, 6};
    auto result = productExceptSelf(nums);
    check(result == expected,
          "[1,2,3,4] -> " + to_string_vec(expected) + ", got " +
              to_string_vec(result));
  }
  {
    std::vector<int> nums = {-1, 1, 0, -3, 3};
    std::vector<long long> expected = {0, 0, 9, 0, 0};
    auto result = productExceptSelf(nums);
    check(result == expected,
          "[-1,1,0,-3,3] -> " + to_string_vec(expected) + ", got " +
              to_string_vec(result));
  }
  {
    std::vector<int> nums = {2, 3};
    std::vector<long long> expected = {3, 2};
    auto result = productExceptSelf(nums);
    check(result == expected, "[2,3] -> " + to_string_vec(expected) +
                                   ", got " + to_string_vec(result));
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
