// ============================================================================
// LeetCode 547 — Number of Provinces
// https://leetcode.com/problems/number-of-provinces/
// ============================================================================
//
// PROBLEM
// -------
// There are n cities. Some are connected directly, some only indirectly (city
// a is connected to b, and b to c, so a is connected to c). A *province* is a
// group of cities that are directly or indirectly connected, with no
// connections to any city outside the group.
//
// You are given an n x n matrix `isConnected` where isConnected[i][j] == 1
// means city i and city j are directly connected. The matrix is symmetric
// (isConnected[i][j] == isConnected[j][i]) and isConnected[i][i] == 1 always.
// Return the total number of provinces.
//
// Example: isConnected = { {1,1,0},
//                          {1,1,0},
//                          {0,0,1} }  -> 2   (provinces {0,1} and {2})
//
// RECOGNITION SIGNAL — "connected components," stated in domain language
// ----------------------------------------------------------------------
// "A group with no connections to anything outside the group" is the definition
// of a connected component, word for word, with "city" substituted for "node"
// and "province" for "component." This is `dfsConnectedComponents` from
// ../code.cpp with one representational difference: the graph arrives as an
// **adjacency matrix**, not an adjacency list, so "the neighbours of u" means
// "every j where isConnected[u][j] == 1" rather than a ready-made list.
//
// 01-number-of-islands.cpp counted components in an *implicit* graph (a grid,
// where the edges are geometric). This file counts them in an *explicit* one
// (a matrix handed to you). Same algorithm, and recognising that the two are
// the same problem in different clothing is the point of pairing them.
//
// BFS vs DFS — WHICH ONE AND WHY
// -------------------------------
// **DFS, but only because it is shorter — BFS is equally correct here.** The
// question is "how many groups," a pure connectivity question with no notion of
// distance anywhere in it. Neither traversal order has any advantage: both
// visit exactly the nodes of one component before returning, both are O(n^2) on
// an adjacency matrix, both need the same `visited` array and the same outer
// loop over start nodes.
//
// This is worth stating explicitly because it is the counter-example to the
// habit of always reaching for BFS: the traversal choice only *matters* when the
// question is about distance (04-word-ladder.cpp) or about the shape of the
// current path (see `hasCycleDirected` in ../code.cpp). For component counting
// it is a style choice, and answering "either — here is why it does not matter"
// is a stronger interview answer than picking one and staying silent.
//
// A third option is genuinely different, not just a style variant: **Union
// Find**. Iterate every pair (i, j) with isConnected[i][j] == 1 and union them,
// then count distinct roots. Same O(n^2) here (you must still read every matrix
// cell) and it wins when edges arrive incrementally and you need repeated
// "are these two connected?" queries. With the whole matrix handed to you up
// front, a traversal is the simpler tool — see the README's Similar Patterns.
//
// APPROACH
// --------
//   1. `visited` array of n booleans, all false.
//   2. Loop over every city i from 0 to n-1. If i is unvisited, it belongs to a
//      province nothing has reached yet: increment the province count, then run
//      one DFS from i that marks every city transitively connected to it.
//   3. Cities already marked by an earlier province's DFS are skipped by the
//      outer loop, so the counter increments exactly once per province.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// The diagonal. isConnected[i][i] is always 1 — every city is trivially
// "connected to itself" — which means every node has a self-loop. The DFS must
// not treat that as anything meaningful. It happens to be harmless here,
// because by the time node i examines neighbour i, node i is ALREADY marked
// visited (the mark happens on entry, before the neighbour scan), so the
// self-edge is skipped by the ordinary visited check. But it is only harmless
// because of mark-on-discovery — a version that marked visited *after* the
// neighbour loop would recurse from i straight back into i, forever.
//
// COMPLEXITY
// ----------
// Time:  O(n^2) -- unavoidable with an adjacency-matrix input: finding node u's
//                  neighbours requires scanning a full row of n entries, and
//                  every one of the n nodes is entered exactly once. Note this
//                  is O(V^2), NOT the O(V + E) of an adjacency list: with the
//                  matrix representation you must read n^2 cells even if the
//                  graph has only a handful of edges.
// Space: O(n)   -- the visited array, plus DFS recursion depth up to n on a
//                  single chain-shaped province.
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// dfsMarkProvince — plain recursive DFS over an adjacency MATRIX.
//
// The only structural difference from an adjacency-list DFS: instead of
// `for (int v : adj[city])`, the neighbour enumeration is a scan over the whole
// row, testing each entry. Everything else — mark on entry, recurse into
// unvisited neighbours — is identical.
// ----------------------------------------------------------------------------
void dfsMarkProvince(const std::vector<std::vector<int> >& isConnected,
                     std::vector<bool>& visited,
                     int city) {
  visited[city] = true;  // MARK ON ENTRY, before looking at any neighbour.

  int n = static_cast<int>(isConnected.size());
  for (int next = 0; next < n; ++next) {
    // isConnected[city][next] == 1 means "there is an edge." The `!visited`
    // test is what makes this terminate: the matrix is symmetric, so every
    // edge exists in both directions, and without the guard city A would
    // recurse into B which would recurse straight back into A.
    //
    // The self-edge (next == city) needs no special case: `visited[city]` was
    // set to true on the line above, so this test rejects it.
    if (isConnected[city][next] == 1 && !visited[next]) {
      dfsMarkProvince(isConnected, visited, next);
    }
  }
}

int findCircleNum(const std::vector<std::vector<int> >& isConnected) {
  int n = static_cast<int>(isConnected.size());
  if (n == 0) return 0;

  std::vector<bool> visited(n, false);
  int provinces = 0;

  // The outer loop over EVERY city is the non-negotiable part. A single DFS
  // from city 0 finds only city 0's own province; provinces are by definition
  // mutually unreachable, so the outer loop is the only thing that can find
  // the second, third, ... one. This is the exact omission the README's
  // Common Mistakes section calls out as the bug that "does not crash, it just
  // quietly under-reports."
  for (int city = 0; city < n; ++city) {
    if (!visited[city]) {
      ++provinces;
      dfsMarkProvince(isConnected, visited, city);
    }
  }

  return provinces;
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

  // Small helper: build an n x n symmetric matrix with 1s on the diagonal from
  // a list of undirected edges, so the test cases below stay readable.
  // (Written as a plain lambda returning by value; no CTAD, no structured
  // bindings — this file targets a C++11-era library.)
  auto buildMatrix = [](int n, const std::vector<std::pair<int, int> >& edges) {
    std::vector<std::vector<int> > m(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; ++i) m[i][i] = 1;
    for (size_t e = 0; e < edges.size(); ++e) {
      m[edges[e].first][edges[e].second] = 1;
      m[edges[e].second][edges[e].first] = 1;  // symmetric: undirected.
    }
    return m;
  };

  {
    std::vector<std::vector<int> > m;
    m.push_back(std::vector<int>{1, 1, 0});
    m.push_back(std::vector<int>{1, 1, 0});
    m.push_back(std::vector<int>{0, 0, 1});
    check(findCircleNum(m) == 2, "LeetCode example 1: {0,1} and {2} -> 2");
  }

  {
    std::vector<std::vector<int> > m;
    m.push_back(std::vector<int>{1, 0, 0});
    m.push_back(std::vector<int>{0, 1, 0});
    m.push_back(std::vector<int>{0, 0, 1});
    check(findCircleNum(m) == 3, "LeetCode example 2: no connections -> 3 provinces");
  }

  {
    // Degenerate input: the n == 0 early return must fire before indexing.
    std::vector<std::vector<int> > m;
    check(findCircleNum(m) == 0, "empty matrix -> 0 provinces");
  }

  {
    std::vector<std::vector<int> > m;
    m.push_back(std::vector<int>{1});
    check(findCircleNum(m) == 1, "single city -> 1 province (the self-edge is not a second one)");
  }

  {
    // TRANSITIVITY: 0-1 and 1-2 are the only direct edges, but 0 and 2 are in
    // the same province because DFS reaches 2 through 1. A solution that only
    // grouped DIRECTLY connected cities would answer 2 here.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::pair<int, int>(0, 1));
    edges.push_back(std::pair<int, int>(1, 2));
    check(findCircleNum(buildMatrix(3, edges)) == 1,
          "chain 0-1-2: indirect connection still counts -> 1 province");
  }

  {
    // Fully connected: every city reachable from every other.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::pair<int, int>(0, 1));
    edges.push_back(std::pair<int, int>(0, 2));
    edges.push_back(std::pair<int, int>(0, 3));
    edges.push_back(std::pair<int, int>(1, 2));
    edges.push_back(std::pair<int, int>(1, 3));
    edges.push_back(std::pair<int, int>(2, 3));
    check(findCircleNum(buildMatrix(4, edges)) == 1, "complete graph on 4 cities -> 1 province");
  }

  {
    // A CYCLE inside one province. The extra edge 2-0 closes a triangle: a
    // cycle changes nothing about the component count, and (critically) the
    // visited guard means the traversal does not loop around it forever.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::pair<int, int>(0, 1));
    edges.push_back(std::pair<int, int>(1, 2));
    edges.push_back(std::pair<int, int>(2, 0));
    check(findCircleNum(buildMatrix(3, edges)) == 1, "triangle (a cycle) is still 1 province");
  }

  {
    // Two provinces of different sizes plus one isolated city -> 3. This is
    // the case that fails outright if the outer loop over all start nodes is
    // dropped: a DFS from city 0 alone would report 1.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::pair<int, int>(0, 1));
    edges.push_back(std::pair<int, int>(1, 2));
    edges.push_back(std::pair<int, int>(3, 4));
    check(findCircleNum(buildMatrix(6, edges)) == 3,
          "{0,1,2}, {3,4}, {5} -> 3 (isolated city 5 is its own province)");
  }

  {
    // Long chain: the deepest recursion in this file (n frames).
    std::vector<std::pair<int, int> > edges;
    for (int i = 0; i + 1 < 50; ++i) {
      edges.push_back(std::pair<int, int>(i, i + 1));
    }
    check(findCircleNum(buildMatrix(50, edges)) == 1, "50-city chain -> 1 province (max recursion depth)");
  }

  {
    // The province found LAST, not first, is the one with the edges. Exercises
    // the same outer-loop requirement from the opposite direction.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::pair<int, int>(2, 3));
    check(findCircleNum(buildMatrix(4, edges)) == 3, "{0}, {1}, {2,3} -> 3 (edges live at the end)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
