// ============================================================================
// LeetCode 785 — Is Graph Bipartite?
// https://leetcode.com/problems/is-graph-bipartite/
// ============================================================================
//
// PROBLEM
// -------
// There is an UNDIRECTED graph with n nodes, given as an adjacency list where
// graph[u] is the list of u's neighbours (each edge appears in both lists).
// Return true if the graph is bipartite: if its nodes can be split into two
// disjoint sets A and B such that EVERY edge connects a node in A to a node in
// B — i.e. no edge ever joins two nodes of the same set.
//
// Example: graph = { {1,2,3}, {0,2}, {0,1,3}, {0,2} }  -> false
//                   (nodes 1 and 2 are neighbours, and both must land in the
//                    same set as each other -> impossible)
//
//          graph = { {1,3}, {0,2}, {1,3}, {0,2} }      -> true
//                   (a 4-cycle 0-1-2-3-0: sets {0,2} and {1,3})
//
// RECOGNITION SIGNAL — "two-colouring," which is really odd-cycle detection
// -------------------------------------------------------------------------
// The theorem underneath this problem: **a graph is bipartite if and only if it
// contains no cycle of ODD length.** Two-colouring is just the constructive
// proof. Walk the graph assigning alternating colours; if you ever reach an
// already-coloured node whose colour equals the colour you are trying to assign
// it, you have closed a cycle after an even number of hops from the node you
// started colouring — which is an odd-length cycle — and no valid split exists.
//
// So this file is the *cycle detection* member of this module's four, the
// undirected counterpart to `hasCycleDirected` in ../code.cpp. Note how
// differently the two are built, and why:
//   - DIRECTED cycle detection needs a three-state marker (unvisited /
//     in-progress / done) because reaching an already-finished node via a
//     second parent is legal and is NOT a cycle.
//   - UNDIRECTED bipartite checking needs no third state at all: the `color`
//     array (-1 / 0 / 1) doubles as the visited array, because "coloured" means
//     "visited," and the *conflict* — not the revisit — is the failure signal.
//     Any undirected edge to an already-coloured node closes a cycle; whether
//     that cycle is fatal depends purely on the colour parity.
//
// BFS vs DFS — WHICH ONE AND WHY
// -------------------------------
// **BFS here, but DFS is equally correct — and, unusually, the reason to prefer
// BFS is NOT the shortest-path guarantee.** Two-colouring never asks how far
// anything is; it only ever asks about the colour of an adjacent node. Both
// traversals visit every node once and check every edge once, both are
// O(V + E), and both find the same answer.
//
// BFS is chosen for one concrete reason: **recursion depth.** A DFS version
// recurses up to V frames deep on a long chain-shaped graph, which is a genuine
// stack-overflow risk at LeetCode's constraint of n up to 100 only in spirit,
// but a real one on production graphs with hundreds of thousands of nodes (see
// the README's Disadvantages). The BFS version's queue lives on the heap and has
// no such ceiling. When the traversal order does not matter, prefer the one that
// cannot blow the stack.
//
// (The DFS version is a genuinely useful thing to be able to write on the spot:
// colour the start node, then for each neighbour, if uncoloured recurse with the
// opposite colour, else check for a conflict. Same five lines, different
// scaffolding.)
//
// APPROACH
// --------
//   1. `color` array of size n, every entry -1 meaning "not yet coloured" —
//      which also means "not yet visited." One array, two jobs.
//   2. Loop over every node as a potential BFS start. A disconnected graph can
//      hide its odd cycle in a component that the first traversal never
//      reaches, so this outer loop is a correctness requirement, not a tidiness
//      one (see the disconnected test case below).
//   3. For an uncoloured start node: give it colour 0 and push it. Then run an
//      ordinary BFS. For each neighbour of the dequeued node:
//        - uncoloured -> paint it the OPPOSITE colour (1 - color[node]) and
//          push it. This is mark-on-discovery: the colour is assigned the
//          instant the neighbour is discovered, never later.
//        - already coloured the SAME as the current node -> conflict, return
//          false immediately. The graph is not bipartite.
//        - already coloured the opposite colour -> fine, skip it. It was
//          reached earlier via a consistent path.
//   4. If every component colours without conflict, the graph is bipartite.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// Treating "already visited" as the failure condition instead of "already
// visited AND same colour." In an undirected graph every edge is traversed from
// both ends, so a plain visited check fires on literally every edge — node u
// colours v, then v looks back at u and sees it visited. That is not a conflict;
// it is the same edge seen from the other side. Only a colour MATCH is a
// conflict. (Second most common: forgetting the outer loop, and reporting true
// for a graph whose only odd cycle sits in a second component.)
//
// COMPLEXITY
// ----------
// Time:  O(V + E) -- every node is enqueued at most once (the colour guard) and
//                    every edge is examined at most twice, once from each
//                    endpoint.
// Space: O(V)     -- the colour array plus the BFS queue, which in the worst
//                    case (a star graph) holds nearly every node at once.
// ============================================================================

#include <iostream>
#include <queue>
#include <string>
#include <vector>

bool isBipartite(const std::vector<std::vector<int> >& graph) {
  int n = static_cast<int>(graph.size());

  // -1 = uncoloured/unvisited, 0 = set A, 1 = set B. This single array is both
  // the answer being constructed AND the visited guard that makes the
  // traversal terminate on a cyclic graph.
  std::vector<int> color(n, -1);

  for (int start = 0; start < n; ++start) {
    if (color[start] != -1) continue;  // Already handled by an earlier component.

    // Colour choice for a fresh component is arbitrary: the two sets are
    // interchangeable, so starting every component at 0 loses no generality.
    color[start] = 0;

    std::queue<int> frontier;
    frontier.push(start);

    while (!frontier.empty()) {
      int node = frontier.front();
      frontier.pop();

      for (size_t k = 0; k < graph[node].size(); ++k) {
        int neighbor = graph[node][k];

        if (color[neighbor] == -1) {
          // Discovered for the first time: paint it the opposite colour and
          // enqueue it in the SAME step. Splitting these two apart (enqueue
          // now, colour when popped) is the classic mark-on-pop bug — two
          // different nodes could both enqueue this neighbour before either
          // colours it, and the second one would then read -1 and paint it a
          // second, possibly contradictory, colour.
          color[neighbor] = 1 - color[node];
          frontier.push(neighbor);
        } else if (color[neighbor] == color[node]) {
          // Conflict: an edge between two nodes of the same set. Equivalently,
          // we just closed an odd-length cycle. No two-colouring exists, and
          // no amount of recolouring elsewhere can fix it, so we can return
          // immediately rather than finishing the traversal.
          return false;
        }
        // else: color[neighbor] == 1 - color[node] -- consistent already.
        // In an undirected graph this branch fires constantly, because every
        // edge is also seen from the neighbour's side. It is NOT an error.
      }
    }
  }

  return true;
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

  // Helper: build a symmetric adjacency list from a list of undirected edges.
  auto buildGraph = [](int n, const std::vector<std::pair<int, int> >& edges) {
    std::vector<std::vector<int> > g(n);
    for (size_t e = 0; e < edges.size(); ++e) {
      g[edges[e].first].push_back(edges[e].second);
      g[edges[e].second].push_back(edges[e].first);
    }
    return g;
  };

  {
    std::vector<std::vector<int> > g;
    g.push_back(std::vector<int>{1, 2, 3});
    g.push_back(std::vector<int>{0, 2});
    g.push_back(std::vector<int>{0, 1, 3});
    g.push_back(std::vector<int>{0, 2});
    check(isBipartite(g) == false, "LeetCode example 1: contains a triangle 0-1-2 -> false");
  }

  {
    std::vector<std::vector<int> > g;
    g.push_back(std::vector<int>{1, 3});
    g.push_back(std::vector<int>{0, 2});
    g.push_back(std::vector<int>{1, 3});
    g.push_back(std::vector<int>{0, 2});
    check(isBipartite(g) == true, "LeetCode example 2: 4-cycle, sets {0,2}/{1,3} -> true");
  }

  {
    std::vector<std::vector<int> > g;  // zero nodes
    check(isBipartite(g) == true, "empty graph -> true (vacuously bipartite)");
  }

  {
    std::vector<std::vector<int> > g(1);
    check(isBipartite(g) == true, "single node, no edges -> true");
  }

  {
    std::vector<std::vector<int> > g(5);  // five isolated nodes
    check(isBipartite(g) == true, "five isolated nodes -> true (every node its own component)");
  }

  {
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::pair<int, int>(0, 1));
    check(isBipartite(buildGraph(2, edges)) == true, "single edge 0-1 -> true");
  }

  {
    // THE ODD CYCLE. A triangle is the smallest non-bipartite graph: colour 0
    // as A, 1 as B, then 2 is adjacent to both and has nowhere to go.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::pair<int, int>(0, 1));
    edges.push_back(std::pair<int, int>(1, 2));
    edges.push_back(std::pair<int, int>(2, 0));
    check(isBipartite(buildGraph(3, edges)) == false, "triangle (3-cycle, odd) -> false");
  }

  {
    // A 5-cycle is odd too -> false. Confirms it is cycle PARITY that matters,
    // not cycle length or the mere presence of a cycle.
    std::vector<std::pair<int, int> > edges;
    for (int i = 0; i < 5; ++i) {
      edges.push_back(std::pair<int, int>(i, (i + 1) % 5));
    }
    check(isBipartite(buildGraph(5, edges)) == false, "5-cycle (odd) -> false");
  }

  {
    // A 6-cycle is even -> true. Same shape as above, one more node.
    std::vector<std::pair<int, int> > edges;
    for (int i = 0; i < 6; ++i) {
      edges.push_back(std::pair<int, int>(i, (i + 1) % 6));
    }
    check(isBipartite(buildGraph(6, edges)) == true, "6-cycle (even) -> true");
  }

  {
    // A tree has no cycles at all, so it is ALWAYS bipartite — colour by depth
    // parity. Here: a star, node 0 joined to 1,2,3,4.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::pair<int, int>(0, 1));
    edges.push_back(std::pair<int, int>(0, 2));
    edges.push_back(std::pair<int, int>(0, 3));
    edges.push_back(std::pair<int, int>(0, 4));
    check(isBipartite(buildGraph(5, edges)) == true, "star graph (a tree, no cycles) -> true");
  }

  {
    // A long path is also a tree -> true, and it is the case that would make a
    // recursive DFS version recurse deepest.
    std::vector<std::pair<int, int> > edges;
    for (int i = 0; i + 1 < 40; ++i) {
      edges.push_back(std::pair<int, int>(i, i + 1));
    }
    check(isBipartite(buildGraph(40, edges)) == true, "40-node path -> true (deepest-traversal case)");
  }

  {
    // THE DISCONNECTED TEST. Component {0,1} is a harmless single edge;
    // component {2,3,4} is a triangle. A solution that only ran BFS from node 0
    // and stopped would answer true. The outer loop is what catches this.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::pair<int, int>(0, 1));
    edges.push_back(std::pair<int, int>(2, 3));
    edges.push_back(std::pair<int, int>(3, 4));
    edges.push_back(std::pair<int, int>(4, 2));
    check(isBipartite(buildGraph(5, edges)) == false,
          "odd cycle hidden in a SECOND component -> still false (outer loop required)");
  }

  {
    // Two separate bipartite components -> true. The mirror of the case above:
    // the outer loop must not invent a conflict ACROSS components either.
    std::vector<std::pair<int, int> > edges;
    edges.push_back(std::pair<int, int>(0, 1));
    edges.push_back(std::pair<int, int>(2, 3));
    check(isBipartite(buildGraph(4, edges)) == true, "two disjoint edges -> true");
  }

  {
    // A self-loop is an odd cycle of length 1: node 0 would have to be in a
    // different set from itself. The colour-match branch catches it, since
    // color[0] == color[0] trivially.
    std::vector<std::vector<int> > g(1);
    g[0].push_back(0);
    check(isBipartite(g) == false, "self-loop (odd cycle of length 1) -> false");
  }

  {
    // Complete bipartite K(2,3): every node in {0,1} joined to every node in
    // {2,3,4}. Dense, many revisits of already-coloured nodes, zero conflicts —
    // exercises the "already coloured, opposite colour, skip" branch heavily.
    std::vector<std::pair<int, int> > edges;
    for (int a = 0; a < 2; ++a) {
      for (int b = 2; b < 5; ++b) {
        edges.push_back(std::pair<int, int>(a, b));
      }
    }
    check(isBipartite(buildGraph(5, edges)) == true, "complete bipartite K(2,3) -> true");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
