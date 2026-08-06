// ============================================================================
// LeetCode 303 - Range Sum Query - Immutable
// ============================================================================
// Problem: given an integer array nums that does NOT change, implement a
// NumArray class supporting sumRange(left, right) -- the sum of the elements
// between indices left and right inclusive -- called an arbitrary number of
// times.
//
// Approach (Prefix Sum): this is the pattern in its purest form. Build a
// prefix array once in the constructor: prefix[0] = 0, prefix[i] =
// prefix[i-1] + nums[i-1]. Each sumRange(left, right) call then returns
// prefix[right+1] - prefix[left] in O(1), regardless of range width.
//
// Complexity: O(n) one-time build (constructor), O(1) per sumRange query,
// O(n) extra space for the prefix array.
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

class NumArray {
 public:
  explicit NumArray(std::vector<int>& nums) {
    prefix_.assign(nums.size() + 1, 0LL);
    for (size_t i = 1; i <= nums.size(); ++i) {
      prefix_[i] = prefix_[i - 1] + nums[i - 1];
    }
  }

  int sumRange(int left, int right) const {
    return static_cast<int>(prefix_[right + 1] - prefix_[left]);
  }

 private:
  std::vector<long long> prefix_;
};

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

  std::vector<int> nums = {-2, 0, 3, -5, 2, -1};
  NumArray numArray(nums);

  check(numArray.sumRange(0, 2) == 1, "sumRange(0,2) -> 1");
  check(numArray.sumRange(2, 5) == -1, "sumRange(2,5) -> -1");
  check(numArray.sumRange(0, 5) == -3, "sumRange(0,5) -> -3");

  std::vector<int> single = {7};
  NumArray na2(single);
  check(na2.sumRange(0, 0) == 7, "single-element array sumRange(0,0) -> 7");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
