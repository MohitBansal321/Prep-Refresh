// ============================================================================
// Bit Manipulation — generic reusable template (C++17)
// ============================================================================
//
// singleNumber: XOR cancellation. countSetBits: n & (n-1) trick.
// isPowerOfTwo: same trick, different test. allSubsetsViaBitmask: enumerate
// every subset of a small set using integers as bitmasks.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

int singleNumber(const std::vector<int>& nums) {
  int result = 0;
  for (int n : nums) result ^= n;
  return result;
}

int countSetBits(unsigned int n) {
  int count = 0;
  while (n) {
    n &= (n - 1);  // clears the lowest set bit
    ++count;
  }
  return count;
}

bool isPowerOfTwo(unsigned int n) {
  return n != 0 && (n & (n - 1)) == 0;
}

std::vector<std::vector<int>> allSubsetsViaBitmask(const std::vector<int>& nums) {
  int n = static_cast<int>(nums.size());
  std::vector<std::vector<int>> result;

  for (int mask = 0; mask < (1 << n); ++mask) {
    std::vector<int> subset;
    for (int i = 0; i < n; ++i) {
      if (mask & (1 << i)) subset.push_back(nums[i]);
    }
    result.push_back(std::move(subset));
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

  check(singleNumber({2, 2, 1}) == 1, "singleNumber([2,2,1]) -> 1");
  check(singleNumber({4, 1, 2, 1, 2}) == 4, "singleNumber([4,1,2,1,2]) -> 4");
  check(singleNumber({7}) == 7, "singleNumber([7]) -> 7 (single element)");

  check(countSetBits(0) == 0, "countSetBits(0) -> 0");
  check(countSetBits(11) == 3, "countSetBits(11 = 0b1011) -> 3");
  check(countSetBits(255) == 8, "countSetBits(255 = 0b11111111) -> 8");

  check(isPowerOfTwo(1) == true, "isPowerOfTwo(1) -> true");
  check(isPowerOfTwo(16) == true, "isPowerOfTwo(16) -> true");
  check(isPowerOfTwo(0) == false, "isPowerOfTwo(0) -> false");
  check(isPowerOfTwo(18) == false, "isPowerOfTwo(18) -> false");

  {
    auto result = allSubsetsViaBitmask({1, 2, 3});
    check(result.size() == 8, "allSubsetsViaBitmask({1,2,3}) -> 8 subsets (2^3)");
    check(result[0].empty(), "mask 0 -> empty subset");
    check(result[7] == std::vector<int>({1, 2, 3}), "mask 7 (0b111) -> full set");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
