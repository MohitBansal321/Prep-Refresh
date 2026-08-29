// ============================================================================
// LeetCode 1462 — Course Schedule IV
// https://leetcode.com/problems/course-schedule-iv/
// ============================================================================
//
// PROBLEM
// -------
// n courses (0..n-1), prerequisites[i] = (u, v) meaning course u must be
// taken before course v (prerequisites are TRANSITIVE: if u is a
// prerequisite of v, and v is a prerequisite of w, then u is also,
// indirectly, a prerequisite of w). Given a list of queries (u, v), answer
// for each whether u is a prerequisite of v.
//
// APPROACH -- Floyd-Warshall's waypoint relaxation, specialized to booleans
// -----------------------------------------------------------------------
// This is the 0/1-weight special case of all-pairs shortest paths: instead
// of tracking a numeric distance, reach[i][j] tracks whether ANY path exists
// from i to j at all. The waypoint relaxation rule becomes an OR instead of
// a MIN: reach[i][j] becomes true if it was already true, OR if reach[i][k]
// AND reach[k][j] are both true for some waypoint k. This is exactly
// Floyd-Warshall's transitive-closure use case -- the same triple loop,
// with "is there a path" replacing "what is the cheapest path."
//
// Time:  O(V^3) for the closure computation, O(1) per query afterward.
// Space: O(V^2) for the reachability matrix.
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

std::vector<bool> checkIfPrerequisite(
    int numCourses, const std::vector<std::vector<int>>& prerequisites,
    const std::vector<std::vector<int>>& queries) {
  std::vector<std::vector<bool>> reach(
      numCourses, std::vector<bool>(numCourses, false));

  for (const auto& p : prerequisites) {
    reach[p[0]][p[1]] = true;  // a direct prerequisite edge
  }

  // Transitive closure: k is the waypoint, exactly as in Floyd-Warshall,
  // but the relaxation is boolean OR instead of numeric MIN.
  for (int k = 0; k < numCourses; ++k) {
    for (int i = 0; i < numCourses; ++i) {
      if (!reach[i][k]) continue;  // no path to the waypoint -- nothing to combine
      for (int j = 0; j < numCourses; ++j) {
        if (reach[k][j]) {
          reach[i][j] = true;
        }
      }
    }
  }

  std::vector<bool> answers;
  answers.reserve(queries.size());
  for (const auto& q : queries) {
    answers.push_back(reach[q[0]][q[1]]);
  }
  return answers;
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
    // numCourses=2, prerequisites=[[1,0]] (course 1 before course 0),
    // queries=[[0,1],[1,0]].
    std::vector<std::vector<int>> prereqs = {{1, 0}};
    std::vector<std::vector<int>> queries = {{0, 1}, {1, 0}};
    std::vector<bool> result = checkIfPrerequisite(2, prereqs, queries);
    check(result[0] == false, "0 is NOT a prerequisite of 1 (edge only goes 1->0)");
    check(result[1] == true, "1 IS a prerequisite of 0 (direct edge)");
  }

  {
    // Transitivity: 0->1->2 as a chain. 0 must be a prerequisite of 2 even
    // though no direct edge 0->2 exists -- this is the entire point of the
    // waypoint relaxation.
    std::vector<std::vector<int>> prereqs = {{0, 1}, {1, 2}};
    std::vector<std::vector<int>> queries = {{0, 2}, {2, 0}};
    std::vector<bool> result = checkIfPrerequisite(3, prereqs, queries);
    check(result[0] == true, "0 -> 2 transitively via 1, even with no direct edge");
    check(result[1] == false, "2 is not a prerequisite of 0 in either direction");
  }

  {
    // No prerequisites at all: every query must be false.
    std::vector<std::vector<int>> prereqs = {};
    std::vector<std::vector<int>> queries = {{0, 1}, {1, 0}};
    std::vector<bool> result = checkIfPrerequisite(2, prereqs, queries);
    check(!result[0] && !result[1], "no prerequisites -> every query is false");
  }

  {
    // A diamond: 0->1, 0->2, 1->3, 2->3. Both 1 and 2 are transitive
    // prerequisites of 3 via different intermediate paths.
    std::vector<std::vector<int>> prereqs = {{0, 1}, {0, 2}, {1, 3}, {2, 3}};
    std::vector<std::vector<int>> queries = {{0, 3}, {1, 3}, {2, 3}};
    std::vector<bool> result = checkIfPrerequisite(4, prereqs, queries);
    check(result[0] && result[1] && result[2],
          "diamond dependency graph -> all three queries resolve true");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
