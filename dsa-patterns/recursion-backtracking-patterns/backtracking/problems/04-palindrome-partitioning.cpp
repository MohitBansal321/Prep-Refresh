// ============================================================================
// LeetCode 131 — Palindrome Partitioning
// ============================================================================
//
// PROBLEM
// -------
// Given a string `s`, partition it so that every substring in the partition
// is a palindrome. Return ALL possible palindrome partitionings.
//
// Example: s = "aab" -> [["a","a","b"], ["aa","b"]]
//
// APPROACH — Backtracking
// ------------------------
// The partial-solution state is "the list of palindromic pieces chosen so
// far, covering some prefix of `s`." At each step we pick the NEXT piece: try
// every possible end boundary for a substring starting at the current
// position, check whether that substring is a palindrome (the constraint),
// and only recurse into the remainder of the string if it is:
//
//   at start index `pos`:
//     if pos == s.size(): a complete valid partition was built -- record it
//     for end in pos..s.size()-1:
//       candidate = s[pos..end]
//       if candidate is NOT a palindrome: PRUNE, do not recurse
//       else:
//         1) CHOOSE  — append candidate to the current partition
//         2) RECURSE — partition the remainder, starting at end + 1
//         3) UNDO    — pop candidate off the current partition before
//                      trying the next `end` boundary
//
// This differs from N-Queens/Sudoku/Word Search in WHERE the constraint
// lives: instead of checking a board position against neighbors, we check
// whether a candidate SUBSTRING is a palindrome. The choose/recurse/undo
// skeleton is identical regardless -- only the constraint check and the
// shape of the partial state change, which is exactly the point of learning
// Backtracking as a pattern rather than memorizing one problem (see
// ../README.md "Architecture").
//
// COMPLEXITY
// ----------
// Worst case is exponential: a string of all-identical characters (e.g.
// "aaaa...a") makes EVERY substring a palindrome, so every one of the
// 2^(n-1) possible ways to place partition cuts between n characters is a
// valid partition, and all of them get enumerated -- there is nothing to
// prune in that adversarial case, which is why this problem's worst case
// genuinely reaches the exponential ceiling described in ../README.md
// "Complexity" (unlike N-Queens/Sudoku, where the worst case is rarely
// approached in practice). On a "generic" string with few palindromic
// substrings, pruning cuts the explored tree drastically below that ceiling.
// Time:  O(n * 2^n) worst case -- 2^n partitions, O(n) to copy/check each.
// Space: O(n) recursion depth, plus O(n) for the current partition being
//        built (not counting the output, which can itself be exponential).
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

namespace {

bool isPalindrome(const std::string& s, int left, int right) {
  while (left < right) {
    if (s[left] != s[right]) return false;
    ++left;
    --right;
  }
  return true;
}

void backtrack(const std::string& s, int pos, std::vector<std::string>& current,
               std::vector<std::vector<std::string>>& allPartitions) {
  if (pos == static_cast<int>(s.size())) {
    // Base case: consumed the entire string with only palindromic pieces
    // (every piece already passed the palindrome check before being pushed
    // onto `current`), so this is a complete, valid partition. Record a
    // COPY -- `current` keeps being mutated by sibling/backtracked branches.
    allPartitions.push_back(current);
    return;
  }

  for (int end = pos; end < static_cast<int>(s.size()); ++end) {
    if (!isPalindrome(s, pos, end)) {
      continue;  // Prune: s[pos..end] is not a palindrome, skip without recursing.
    }

    current.push_back(s.substr(pos, end - pos + 1));  // 1) CHOOSE
    backtrack(s, end + 1, current, allPartitions);     // 2) RECURSE
    current.pop_back();                                // 3) UNDO
  }
}

}  // namespace

std::vector<std::vector<std::string>> partition(const std::string& s) {
  std::vector<std::vector<std::string>> allPartitions;
  std::vector<std::string> current;
  backtrack(s, 0, current, allPartitions);
  return allPartitions;
}

namespace {

bool containsPartition(const std::vector<std::vector<std::string>>& all,
                        const std::vector<std::string>& target) {
  for (const auto& p : all) {
    if (p == target) return true;
  }
  return false;
}

}  // namespace

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
    auto result = partition("aab");
    check(result.size() == 2, "\"aab\" produces exactly 2 partitions");
    check(containsPartition(result, {"a", "a", "b"}), "\"aab\" includes [\"a\",\"a\",\"b\"]");
    check(containsPartition(result, {"aa", "b"}), "\"aab\" includes [\"aa\",\"b\"]");
  }

  {
    auto result = partition("a");
    check(result.size() == 1, "single character \"a\" produces exactly 1 partition");
    check(containsPartition(result, {"a"}), "\"a\" includes [\"a\"]");
  }

  {
    // Every substring of an all-identical-character string is a palindrome,
    // so the count of partitions equals 2^(n-1) -- the adversarial case
    // discussed in the complexity notes above (n=4 -> 2^3 = 8).
    auto result = partition("aaaa");
    check(result.size() == 8, "\"aaaa\" (all-same characters) produces 2^(4-1) = 8 partitions");
  }

  {
    auto result = partition("ab");
    check(result.size() == 1, "\"ab\" has no 2-letter palindrome, so only the all-singles partition exists");
    check(containsPartition(result, {"a", "b"}), "\"ab\" includes [\"a\",\"b\"]");
  }

  {
    auto result = partition("racecar");
    check(containsPartition(result, {"racecar"}), "\"racecar\" includes the whole-string partition [\"racecar\"]");
    check(containsPartition(result, {"r", "aceca", "r"}), "\"racecar\" includes [\"r\",\"aceca\",\"r\"]");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
