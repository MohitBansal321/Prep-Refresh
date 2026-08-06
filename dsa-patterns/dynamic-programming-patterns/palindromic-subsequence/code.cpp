// ============================================================================
// Palindromic Subsequence / Substring — generic reusable template (C++17)
// ============================================================================
//
// longestPalindromicSubstring: expand-around-center, O(n^2) time, O(1) space.
// longestPalindromicSubsequenceLength: interval DP, filled by increasing
// interval length, O(n^2) time and space.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

std::string longestPalindromicSubstring(const std::string& s) {
  if (s.empty()) return "";

  int start = 0;
  int maxLen = 1;

  auto expand = [&](int left, int right) {
    while (left >= 0 && right < static_cast<int>(s.size()) && s[left] == s[right]) {
      --left;
      ++right;
    }
    int len = right - left - 1;
    if (len > maxLen) {
      maxLen = len;
      start = left + 1;
    }
  };

  for (int center = 0; center < static_cast<int>(s.size()); ++center) {
    expand(center, center);      // odd-length palindromes centered on one char
    expand(center, center + 1);  // even-length palindromes centered on a gap
  }

  return s.substr(start, maxLen);
}

int longestPalindromicSubsequenceLength(const std::string& s) {
  int n = static_cast<int>(s.size());
  if (n == 0) return 0;

  std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

  // Fill by increasing interval length: i decreasing, j increasing from i+1.
  // dp[i][j] depends on dp[i+1][j-1], a strictly smaller interval, so this
  // order guarantees every dependency is already computed.
  for (int i = n - 1; i >= 0; --i) {
    dp[i][i] = 1;
    for (int j = i + 1; j < n; ++j) {
      if (s[i] == s[j]) {
        dp[i][j] = dp[i + 1][j - 1] + 2;
      } else {
        dp[i][j] = std::max(dp[i + 1][j], dp[i][j - 1]);
      }
    }
  }

  return dp[0][n - 1];
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
    auto result = longestPalindromicSubstring("babad");
    check(result == "bab" || result == "aba",
          "longestPalindromicSubstring(\"babad\") -> \"bab\" or \"aba\"");
  }
  check(longestPalindromicSubstring("cbbd") == "bb",
        "longestPalindromicSubstring(\"cbbd\") -> \"bb\"");
  check(longestPalindromicSubstring("a") == "a",
        "longestPalindromicSubstring(\"a\") -> \"a\"");
  check(longestPalindromicSubstring("") == "", "longestPalindromicSubstring(\"\") -> \"\"");

  check(longestPalindromicSubsequenceLength("bbbab") == 4,
        "longestPalindromicSubsequenceLength(\"bbbab\") -> 4 (\"bbbb\")");
  check(longestPalindromicSubsequenceLength("cbbd") == 2,
        "longestPalindromicSubsequenceLength(\"cbbd\") -> 2 (\"bb\")");
  check(longestPalindromicSubsequenceLength("a") == 1,
        "longestPalindromicSubsequenceLength(\"a\") -> 1");
  check(longestPalindromicSubsequenceLength("") == 0,
        "longestPalindromicSubsequenceLength(\"\") -> 0");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
