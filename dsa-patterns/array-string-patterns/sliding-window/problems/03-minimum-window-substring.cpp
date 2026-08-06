// ============================================================================
// LeetCode 76 — Minimum Window Substring
// ============================================================================
// Statement:
//   Given two strings `s` and `t`, return the minimum-length substring of `s`
//   that contains every character of `t` (including duplicates — if `t` has
//   two 'a's, the window must contain at least two 'a's). Return "" if no
//   such window exists.
//
// Example:
//   s = "ADOBECODEBANC", t = "ABC" -> answer "BANC"
//   s = "a", t = "aa"              -> answer "" (impossible: only one 'a' in s)
//
// Sliding Window shape: VARIABLE-SIZE window, "shortest window satisfying a
// condition" — the condition here is "window contains at least as many of
// each character as `t` requires."
//
// Approach:
//   1. Build a frequency map `need` counting each character required by `t`.
//      Track `required` = number of *distinct* characters that must be
//      satisfied (not total character count).
//   2. Expand `right`, adding characters to a running frequency map `window`.
//      Whenever a character's count in `window` first reaches its required
//      count in `need`, increment `satisfied` (one more distinct requirement
//      met).
//   3. Whenever `satisfied == required` (the whole window is currently
//      valid), try to shrink from the left as far as possible while it stays
//      valid — every shrink step is a candidate for a new minimum window.
//      Shrinking removes s[left] from `window`; if that drops a required
//      character below its needed count, `satisfied` decreases and the
//      shrink loop stops.
//   4. Track the best (start, length) seen across the whole scan.
//
// Complexity:
//   Time:  O(|s| + |t|) — building `need` is O(|t|); the main scan has
//          `right` advance |s| times and `left` advance at most |s| times
//          in total (each index visited by `left` at most once), so the
//          two-pointer sweep is O(|s|).
//   Space: O(|s_chars| + |t_chars|) for the two frequency maps — bounded by
//          the alphabet size in practice (e.g. <= 128 for ASCII).
//   Contrast: the brute force tries every (start, end) pair and checks
//   containment with a fresh frequency count each time: O(|s|^2 * |t|) in the
//   worst case. For |s| = 2*10^4, that is on the order of 10^8-10^9
//   operations vs. the sliding window's ~2*10^4 — this is the problem where
//   the payoff of the pattern is most dramatic.
// ============================================================================

#include <climits>
#include <iostream>
#include <string>
#include <unordered_map>

std::string minWindow(const std::string& s, const std::string& t) {
    if (s.empty() || t.empty() || t.size() > s.size()) {
        return "";
    }

    std::unordered_map<char, int> need;
    for (char c : t) {
        need[c]++;
    }
    int required = static_cast<int>(need.size());  // distinct chars to satisfy

    std::unordered_map<char, int> window;
    int satisfied = 0;  // how many distinct chars currently meet their requirement

    int bestLen = INT_MAX;
    int bestStart = 0;
    int left = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        char c = s[right];
        window[c]++;

        // If this character is one we need, and we just reached exactly the
        // required count for it, one more distinct requirement is satisfied.
        auto needIt = need.find(c);
        if (needIt != need.end() && window[c] == needIt->second) {
            ++satisfied;
        }

        // While the window fully satisfies every requirement, try to shrink
        // it from the left — every valid, smaller window is a better answer.
        while (satisfied == required) {
            if (right - left + 1 < bestLen) {
                bestLen = right - left + 1;
                bestStart = left;
            }

            char leftChar = s[left];
            window[leftChar]--;

            auto leftNeedIt = need.find(leftChar);
            if (leftNeedIt != need.end() && window[leftChar] < leftNeedIt->second) {
                // Removing this character just broke a requirement; the
                // window is no longer valid, so stop shrinking.
                --satisfied;
            }

            ++left;
        }
    }

    return (bestLen == INT_MAX) ? "" : s.substr(bestStart, bestLen);
}

// ----------------------------------------------------------------------------
// Test harness
// ----------------------------------------------------------------------------
static int failures = 0;

static void check(const std::string& actual, const std::string& expected, const std::string& label) {
    bool pass = (actual == expected);
    std::cout << (pass ? "[PASS] " : "[FAIL] ") << label
              << " -> got \"" << actual << "\", expected \"" << expected << "\"\n";
    if (!pass) ++failures;
}

int main() {
    std::cout << "=== Minimum Window Substring (LC 76) ===\n\n";

    check(minWindow("ADOBECODEBANC", "ABC"), "BANC", "s=\"ADOBECODEBANC\", t=\"ABC\"");
    check(minWindow("a", "a"), "a", "s=\"a\", t=\"a\"");
    check(minWindow("a", "aa"), "", "s=\"a\", t=\"aa\" (impossible)");
    check(minWindow("ab", "b"), "b", "s=\"ab\", t=\"b\"");
    check(minWindow("aa", "aa"), "aa", "s=\"aa\", t=\"aa\" (needs both duplicates)");
    check(minWindow("cabwefgewcwaefgcf", "cae"), "cwae", "s=\"cabwefgewcwaefgcf\", t=\"cae\"");

    std::cout << '\n' << (failures == 0 ? "All tests PASSED." : "Some tests FAILED.") << '\n';
    return failures == 0 ? 0 : 1;
}
