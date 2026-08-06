// ============================================================================
// LeetCode 354 — Russian Doll Envelopes
// ============================================================================
//
// PROBLEM
// -------
// You are given a 2D array `envelopes` where envelopes[i] = [width_i,
// height_i]. One envelope can fit inside another if and only if BOTH its
// width and height are strictly smaller than the other envelope's width
// and height. Return the maximum number of envelopes you can Russian-doll
// (nest one inside another, like the nesting dolls in the README's
// analogy).
//
// Example: envelopes = [[5,4],[6,4],[6,7],[2,3]] -> 3
//          (nest [2,3] -> [5,4] -> [6,7])
//
// APPROACH — sort by width, LIS on height, with a tie-breaking trick
// ----------------------------------------------------------------------
// This is a genuinely 2D problem disguised as a 1D LIS problem, and the
// trick for collapsing it to 1D is the entire difficulty:
//
// 1. Sort envelopes by width ASCENDING. Now any nesting chain we build by
//    walking left to right automatically has non-decreasing width, so
//    width no longer needs separate tracking — the array order itself
//    guarantees it.
//
// 2. THE TIE-BREAK TRICK: when two envelopes share the same width, sort
//    those by height DESCENDING (not ascending). Why: if two envelopes
//    have equal width, neither can ever nest inside the other (nesting
//    requires STRICTLY smaller width, and they are tied). If we sorted
//    same-width envelopes by height ascending, an LIS pass over heights
//    could be fooled into treating two same-width envelopes as a valid
//    "increasing height" pair — which would be wrong, since same-width
//    envelopes can never actually nest. Sorting same-width groups by
//    height DESCENDING means their heights appear in DECREASING order in
//    the array, so a strictly-increasing-height LIS scan can never pick
//    more than one envelope from any same-width group — exactly the
//    correctness guarantee we need.
//
// 3. After this sort, the problem reduces EXACTLY to: find the length of
//    the longest strictly increasing subsequence of the height column.
//    Any strictly increasing run of heights, given the sort order, also
//    has strictly increasing widths (because of step 2's guarantee) — so
//    it is a valid nesting chain, and every valid nesting chain shows up
//    as some increasing run of heights after this sort.
//
// 4. Run the O(n log n) patience-sorting LIS (std::lower_bound on a
//    `tails` array of heights) over that height column.
//
// COMPLEXITY
// ----------
// Time:  O(n log n) — O(n log n) to sort, then O(n log n) for the
//        patience-sorting LIS pass (n binary searches).
// Space: O(n) — for the `tails` array (and the sort is typically done
//        in place or with O(log n) extra recursion stack).
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int maxEnvelopes(std::vector<std::vector<int>> envelopes) {
  if (envelopes.empty()) return 0;

  // Sort by width ascending; for ties on width, sort by height DESCENDING
  // so that same-width envelopes can never look like a valid "increasing
  // height" step in the LIS pass that follows.
  std::sort(envelopes.begin(), envelopes.end(),
            [](const std::vector<int>& a, const std::vector<int>& b) {
              if (a[0] != b[0]) return a[0] < b[0];
              return a[1] > b[1];
            });

  // Now: find the length of the longest strictly increasing subsequence
  // of the height column — identical patience-sorting technique as
  // 01-longest-increasing-subsequence.cpp.
  std::vector<int> tails;
  tails.reserve(envelopes.size());

  for (const auto& envelope : envelopes) {
    int height = envelope[1];
    auto it = std::lower_bound(tails.begin(), tails.end(), height);
    if (it == tails.end()) {
      tails.push_back(height);
    } else {
      *it = height;
    }
  }

  return static_cast<int>(tails.size());
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](std::vector<std::vector<int>> envelopes, int expected,
                    const std::string& label) {
    int result = maxEnvelopes(envelopes);
    if (result == expected) {
      std::cout << "[PASS] " << label << " -> " << result << "\n";
      ++pass_count;
    } else {
      std::cout << "[FAIL] " << label << " -> got " << result
                << ", expected " << expected << "\n";
      ++fail_count;
    }
  };

  check({{5, 4}, {6, 4}, {6, 7}, {2, 3}}, 3,
        "classic example -> [2,3] -> [5,4] -> [6,7]");

  check({{1, 1}, {1, 1}, {1, 1}}, 1,
        "all identical envelopes -> none can nest -> 1");

  check({{4, 5}, {4, 6}, {6, 7}, {2, 3}}, 3,
        "same-width tie-break must not overcount -> 3");

  check({{1, 1}}, 1, "single envelope -> 1");

  check({}, 0, "no envelopes -> 0");

  check({{3, 4}, {14, 12}, {11, 11}, {2, 3}}, 4,
        "another mixed example -> [2,3]->[3,4]->[11,11]->[14,12] -> 4");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
