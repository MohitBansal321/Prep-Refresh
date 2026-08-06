// ============================================================================
// LeetCode 3 — Longest Substring Without Repeating Characters
// ============================================================================
// Statement:
//   Given a string `s`, find the length of the longest substring (contiguous)
//   without repeating characters.
//
// Example:
//   s = "abcabcbb" -> answer 3 (the substring "abc")
//   s = "bbbbb"     -> answer 1 (the substring "b")
//   s = "pwwkew"    -> answer 3 (the substring "wke")
//
// Sliding Window shape: VARIABLE-SIZE window, "longest window satisfying a
// condition" (no repeated character inside the window).
//
// Approach:
//   Keep a window [left, right] and a map from character -> the most recent
//   index at which it appeared *inside the current window*.
//   1. Expand `right` one character at a time.
//   2. If s[right] was already seen at an index >= left (i.e. its last
//      occurrence is still inside the current window), that occurrence is a
//      duplicate. Jump `left` to one past that occurrence instead of
//      shrinking one character at a time — this keeps the whole scan O(n)
//      instead of degrading to O(n^2) on adversarial inputs like "aaaa...a".
//   3. Record/refresh the last-seen index of s[right].
//   4. Track the best (longest) window length seen so far as right - left + 1.
//
//   Why a direct jump instead of a `while` shrink loop? Because with a
//   "last seen index" map we know *exactly* how far left must move — there is
//   no need to remove characters one at a time and re-check a condition.
//   This is a common, valid variant of the variable-size window template.
//
// Complexity:
//   Time:  O(n)   — each character is visited once by `right`; `left` only
//                    ever moves forward, so it also advances at most n times
//                    in total across the whole run.
//   Space: O(min(n, alphabet size)) for the last-seen-index map — at most one
//          entry per distinct character.
//   Contrast: the brute force checks every substring for uniqueness, which is
//   O(n^2) substrings times O(n) to verify each = O(n^3) naively, or O(n^2)
//   with a smarter O(1)-per-character check. For n = 5*10^4, n^2 is
//   2.5*10^9 — far too slow; sliding window's O(n) = 5*10^4 is instant.
// ============================================================================

#include <iostream>
#include <string>
#include <unordered_map>

int lengthOfLongestSubstring(const std::string& s) {
    std::unordered_map<char, int> lastSeenIndex;  // char -> most recent index
    int left = 0;
    int best = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        char c = s[right];

        auto it = lastSeenIndex.find(c);
        if (it != lastSeenIndex.end() && it->second >= left) {
            // The previous occurrence of c is still inside the window —
            // shrink by jumping left to just past that occurrence.
            left = it->second + 1;
        }

        lastSeenIndex[c] = right;  // record/refresh this character's position
        best = std::max(best, right - left + 1);
    }

    return best;
}

// ----------------------------------------------------------------------------
// Test harness
// ----------------------------------------------------------------------------
static int failures = 0;

static void check(int actual, int expected, const std::string& label) {
    bool pass = (actual == expected);
    std::cout << (pass ? "[PASS] " : "[FAIL] ") << label
              << " -> got " << actual << ", expected " << expected << '\n';
    if (!pass) ++failures;
}

int main() {
    std::cout << "=== Longest Substring Without Repeating Characters (LC 3) ===\n\n";

    check(lengthOfLongestSubstring("abcabcbb"), 3, "\"abcabcbb\"");
    check(lengthOfLongestSubstring("bbbbb"), 1, "\"bbbbb\"");
    check(lengthOfLongestSubstring("pwwkew"), 3, "\"pwwkew\"");
    check(lengthOfLongestSubstring(""), 0, "\"\" (empty string)");
    check(lengthOfLongestSubstring(" "), 1, "\" \" (single space)");
    check(lengthOfLongestSubstring("dvdf"), 3, "\"dvdf\"");   // classic tricky case
    check(lengthOfLongestSubstring("abba"), 2, "\"abba\"");   // classic tricky case
    check(lengthOfLongestSubstring("au"), 2, "\"au\"");

    std::cout << '\n' << (failures == 0 ? "All tests PASSED." : "Some tests FAILED.") << '\n';
    return failures == 0 ? 0 : 1;
}
