// ============================================================================
// Longest Common Subsequence — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// two-sequence 2D DP shape you will re-derive on almost every problem that
// compares two sequences and asks for a "best answer over prefixes":
//
//   dp[i][j] = the best answer for the first i characters of sequence A
//              and the first j characters of sequence B.
//
//   - If A[i-1] == B[j-1] (the characters at this position match), the
//     match extends whatever the best answer was one row and one column
//     back (the diagonal): dp[i][j] = dp[i-1][j-1] + 1.
//   - Otherwise, the match cannot include both A[i-1] and B[j-1], so we
//     take the best of "drop A[i-1]" or "drop B[j-1]":
//     dp[i][j] = max(dp[i-1][j], dp[i][j-1]).
//
// Two functions are provided:
//   1. longestCommonSubsequence(a, b) — returns only the LENGTH, using the
//      full O(n*m) table (kept, not space-optimized, because...)
//   2. reconstructLCS(a, b)           — returns the actual shared
//      subsequence by walking the same table backwards from dp[n][m] to
//      dp[0][0]. Reconstruction is why we keep the full 2D table instead of
//      the O(min(n,m)) rolling-array optimization: you cannot walk backwards
//      through a table that has already overwritten its earlier rows.
//
// Worked, problem-specific solutions (LeetCode 1143, 72, 1092, 583) live in
// problems/*.cpp and reuse this exact recurrence with small variations.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_lcs_code && /tmp/out_lcs_code
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// longestCommonSubsequence — generic, reusable LCS length function.
//
// Works over any type `Sequence` that supports .size() and operator[]
// returning comparable elements (std::string, std::vector<int>, etc.).
//
// Builds the full (n+1) x (m+1) table, where dp[i][j] means "the LCS length
// considering only the first i elements of `a` and the first j elements of
// `b`". Row 0 and column 0 are the empty-prefix base case (LCS of anything
// with an empty sequence is 0), which is why the table is sized n+1 by m+1
// instead of n by m — this avoids special-casing i==0 or j==0 inside the
// loop.
//
// Returns dp[n][m], the LCS length over the full sequences.
// ----------------------------------------------------------------------------
template <typename Sequence>
int longestCommonSubsequence(const Sequence& a, const Sequence& b) {
  const size_t n = a.size();
  const size_t m = b.size();

  // dp[i][j]: LCS length of a[0..i) and b[0..j). Extra row/column at index 0
  // represents the empty-prefix base case, already zero-initialized.
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      if (a[i - 1] == b[j - 1]) {
        // Characters match: extend the best answer found one row and one
        // column back (the diagonal neighbor), because this matched pair
        // can only ever follow the LCS of the two shorter prefixes.
        dp[i][j] = dp[i - 1][j - 1] + 1;
      } else {
        // No match at this position: the LCS of these two prefixes cannot
        // use BOTH a[i-1] and b[j-1] together, so it is the better of
        // "skip a[i-1]" (look at dp[i-1][j]) or "skip b[j-1]" (dp[i][j-1]).
        dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
      }
    }
  }

  return dp[n][m];
}

// ----------------------------------------------------------------------------
// reconstructLCS — returns the actual longest common subsequence, not just
// its length, by rebuilding the same table and then walking it BACKWARDS
// from dp[n][m] to dp[0][0].
//
// The walk-back logic mirrors the forward recurrence in reverse:
//   - If a[i-1] == b[j-1], this character MUST be part of the LCS that
//     produced dp[i][j] (that is exactly the case that wrote dp[i][j] as
//     dp[i-1][j-1] + 1). Record it, then step diagonally to (i-1, j-1).
//   - Otherwise, dp[i][j] was copied from whichever of dp[i-1][j] or
//     dp[i][j-1] was larger (the branch the forward pass actually took).
//     Step in that same direction to retrace the path that produced the
//     answer.
//
// Characters are appended in REVERSE order during the walk-back (because we
// start from the end and move toward the start), so the result is reversed
// once at the end before returning.
// ----------------------------------------------------------------------------
template <typename Sequence>
Sequence reconstructLCS(const Sequence& a, const Sequence& b) {
  const size_t n = a.size();
  const size_t m = b.size();

  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      if (a[i - 1] == b[j - 1]) {
        dp[i][j] = dp[i - 1][j - 1] + 1;
      } else {
        dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
      }
    }
  }

  Sequence result{};
  size_t i = n;
  size_t j = m;
  while (i > 0 && j > 0) {
    if (a[i - 1] == b[j - 1]) {
      // This character is part of the LCS: record it and move diagonally.
      result.push_back(a[i - 1]);
      --i;
      --j;
    } else if (dp[i - 1][j] >= dp[i][j - 1]) {
      // The table's value came from dropping a[i-1] -> retrace that step.
      // (>= rather than > is an arbitrary but consistent tie-break; either
      // choice yields a valid LCS when multiple exist.)
      --i;
    } else {
      // The table's value came from dropping b[j-1] -> retrace that step.
      --j;
    }
  }

  std::reverse(result.begin(), result.end());
  return result;
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

  std::cout << "--- longestCommonSubsequence (length only) ---\n";
  {
    check(longestCommonSubsequence(std::string("abcde"), std::string("ace")) == 3,
          "\"abcde\" vs \"ace\" -> length 3");
    check(longestCommonSubsequence(std::string("abc"), std::string("abc")) == 3,
          "identical strings -> length 3 (whole string)");
    check(longestCommonSubsequence(std::string("abc"), std::string("def")) == 0,
          "no shared characters -> length 0");
    check(longestCommonSubsequence(std::string(""), std::string("abc")) == 0,
          "empty vs non-empty -> length 0 (base case)");
    check(longestCommonSubsequence(std::string("ABCBDAB"), std::string("BDCABA")) == 4,
          "\"ABCBDAB\" vs \"BDCABA\" -> length 4 (classic textbook example)");
  }

  std::cout << "\n--- reconstructLCS (actual subsequence) ---\n";
  {
    std::string result = reconstructLCS(std::string("abcde"), std::string("ace"));
    check(result == "ace", "\"abcde\" vs \"ace\" -> reconstructed \"ace\"");

    std::string result2 = reconstructLCS(std::string("ABCBDAB"), std::string("BDCABA"));
    // Two subsequences of length 4 are valid for this classic example:
    // "BCBA" and "BDAB". Either is an acceptable LCS.
    check(result2.size() == 4 && (result2 == "BCBA" || result2 == "BDAB"),
          "\"ABCBDAB\" vs \"BDCABA\" -> a valid length-4 LCS");

    // Consistency check: the reconstructed subsequence's length must always
    // equal what the length-only function reports, for any input pair.
    std::string x = "AGGTAB";
    std::string y = "GXTXAYB";
    int len = longestCommonSubsequence(x, y);
    std::string sub = reconstructLCS(x, y);
    check(static_cast<int>(sub.size()) == len,
          "reconstructed length matches longestCommonSubsequence's length");
    check(sub == "GTAB", "\"AGGTAB\" vs \"GXTXAYB\" -> reconstructed \"GTAB\"");

    std::string empty_result = reconstructLCS(std::string(""), std::string("xyz"));
    check(empty_result.empty(), "empty vs non-empty -> reconstructed \"\" (empty)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
