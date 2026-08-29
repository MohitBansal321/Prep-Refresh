// ============================================================================
// LeetCode 2101 — Detonate the Maximum Bombs
// https://leetcode.com/problems/detonate-the-maximum-bombs/
// ============================================================================
//
// PROBLEM
// -------
// n bombs, each bombs[i] = [x, y, r] (position and blast radius). Detonating
// bomb i triggers every OTHER bomb j whose center lies within bomb i's blast
// radius r -- and each newly triggered bomb, in turn, can trigger further
// bombs the same way. Choosing exactly one bomb to detonate first, return
// the MAXIMUM number of bombs that can be detonated in total.
//
// APPROACH -- transitive closure over an ASYMMETRIC reachability relation
// -------------------------------------------------------------------
// This is the closure facet again (like Course Schedule IV), but with one
// important twist: "bomb i can trigger bomb j" is NOT necessarily symmetric
// -- bomb i's radius might reach bomb j's center, while bomb j's smaller
// radius might not reach back to bomb i. So reach[i][j] must be built
// directly from the actual distance/radius check per DIRECTED pair, not
// assumed reciprocal the way an undirected road network would be.
//
// Once the direct edges are built, the SAME waypoint relaxation as Course
// Schedule IV computes full transitive reachability: reach[i][j] becomes
// true if some chain of triggers connects i to j, however many bombs long.
// The answer is then, for each candidate starting bomb i, how many OTHER
// bombs it can transitively reach -- take the maximum over every choice of
// i, plus 1 for the starting bomb itself.
//
// Time:  O(V^2) to build direct edges (checking every ordered pair's
//        distance) + O(V^3) for the closure + O(V^2) for the final count.
// Space: O(V^2) for the reachability matrix.
// ============================================================================

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

int maximumDetonation(const std::vector<std::vector<int>>& bombs) {
  int n = static_cast<int>(bombs.size());
  std::vector<std::vector<bool>> reach(n, std::vector<bool>(n, false));

  // Build direct edges: bomb i triggers bomb j iff j's center lies within
  // i's blast radius. This is checked per ORDERED pair -- i triggering j
  // does not imply j triggers i.
  for (int i = 0; i < n; ++i) {
    long long xi = bombs[i][0], yi = bombs[i][1];
    long long ri = bombs[i][2];
    for (int j = 0; j < n; ++j) {
      if (i == j) continue;
      long long dx = xi - bombs[j][0];
      long long dy = yi - bombs[j][1];
      // Compare squared distance to squared radius to avoid floating point.
      if (dx * dx + dy * dy <= ri * ri) {
        reach[i][j] = true;
      }
    }
  }

  // Transitive closure, exactly as in Course Schedule IV -- boolean OR
  // relaxation through every waypoint.
  for (int k = 0; k < n; ++k) {
    for (int i = 0; i < n; ++i) {
      if (!reach[i][k]) continue;
      for (int j = 0; j < n; ++j) {
        if (reach[k][j]) reach[i][j] = true;
      }
    }
  }

  int best = 1;  // detonating any single bomb always detonates at least itself
  for (int i = 0; i < n; ++i) {
    int count = 1;  // the starting bomb itself
    for (int j = 0; j < n; ++j) {
      if (i != j && reach[i][j]) ++count;
    }
    best = std::max(best, count);
  }
  return best;
}

// ============================================================================
// main() -- printed, verifiable output.
// ============================================================================

int g_pass = 0;
int g_fail = 0;

void check(bool condition, const std::string& label) {
  if (condition) {
    std::cout << "[PASS] " << label << "\n";
    ++g_pass;
  } else {
    std::cout << "[FAIL] " << label << "\n";
    ++g_fail;
  }
}

int main() {
  {
    // Asymmetric reach via unequal radii: bomb 0 at (0,0) has a radius (25)
    // large enough to directly reach both bomb 1 (distance 10) and bomb 2
    // (distance 20). Bombs 1 and 2 have tiny radii (1) that reach nothing
    // back. This isolates the asymmetry cleanly: 0 -> 1 and 0 -> 2 exist,
    // but neither 1 nor 2 can trigger anything at all.
    std::vector<std::vector<int>> bombs = {{0, 0, 25}, {10, 0, 1}, {20, 0, 1}};
    check(maximumDetonation(bombs) == 3,
          "asymmetric radii: only bomb 0's large radius reaches both others -> 3 total");
  }

  {
    // No bomb reaches any other -- detonating any single bomb only
    // detonates itself.
    std::vector<std::vector<int>> bombs = {{0, 0, 1}, {100, 100, 1}};
    check(maximumDetonation(bombs) == 1,
          "no bomb's radius reaches another -> best is detonating just 1");
  }

  {
    // A genuine chain requiring transitive closure: 0 triggers 1, 1
    // triggers 2, but 0 alone does NOT directly reach 2 -- only the
    // waypoint relaxation discovers 0 can reach 2 transitively through 1.
    // 0 at (0,0) r=5, 1 at (5,0) r=5, 2 at (10,0) r=1.
    // 0->1: distance 5 <= 5, triggers. 1->2: distance 5 <= 5, triggers.
    // 0->2 directly: distance 10 > 5, does NOT trigger directly.
    std::vector<std::vector<int>> bombs = {{0, 0, 5}, {5, 0, 5}, {10, 0, 1}};
    check(maximumDetonation(bombs) == 3,
          "0 reaches 2 only transitively via 1 -> starting at 0 detonates all 3");
  }

  {
    // Single bomb: detonating it always counts as 1.
    std::vector<std::vector<int>> bombs = {{0, 0, 1}};
    check(maximumDetonation(bombs) == 1, "single bomb -> always 1");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
