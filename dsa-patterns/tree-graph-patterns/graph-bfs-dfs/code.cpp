// ============================================================================
// Graph BFS/DFS — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// three building blocks you will re-derive on almost every problem that fits
// this pattern, once the input is a general GRAPH (possibly with cycles)
// rather than a tree:
//
//   1. bfsShortestPath  — BFS from a single source, returning the minimum
//                         number of hops (edges) to every other node. Only
//                         correct for UNWEIGHTED graphs — see the README's
//                         "When NOT To Use" section for why weighted edges
//                         break this.
//
//   2. dfsConnectedComponents — DFS (or BFS, either works) used to count how
//                         many disjoint connected components the graph has.
//                         Treats the graph as UNDIRECTED: it builds a
//                         symmetric view internally, so it gives the right
//                         answer whether the caller's adjacency list already
//                         stores edges in both directions or not.
//
//   3. hasCycleDirected  — DFS with an explicit "on the current recursion
//                         stack" marker, used to detect a cycle in a
//                         DIRECTED graph. This is the mechanism behind
//                         deadlock detection and "can this dependency graph
//                         even be scheduled" checks (see Topological Sort).
//
// The one idea that ties all three together, and the one thing tree BFS/DFS
// never had to worry about: a GRAPH can have cycles, so every one of these
// functions carries an explicit "have I been here before" marker. Without
// it, a cycle sends the traversal around forever.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_graph_code && /tmp/out_graph_code
// ============================================================================

#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// bfsShortestPath — BFS from `src`, returning distance (in edges/hops) to
// every node reachable from `src`. Unreachable nodes are left at -1.
//
// Graph representation: `adj` is an adjacency list — adj[u] is the list of
// nodes directly reachable from u in one hop. Works for directed or
// undirected graphs; for undirected graphs the caller is expected to have
// added each edge in both directions when building `adj` (adj[u] contains v
// AND adj[v] contains u), which is the standard convention for representing
// an undirected graph as an adjacency list.
//
// Why BFS (not DFS) gives shortest hops: BFS explores the graph in
// "rings" — it fully exhausts every node at distance 1 before looking at any
// node at distance 2, and so on. The FIRST time a node is reached is
// therefore always via a shortest possible path, because no path through a
// farther ring could possibly be shorter. DFS gives no such guarantee: it
// can stumble onto a long, winding path to a node long before finding the
// short one.
//
// The critical detail: a node is marked "seen" (dist set to something other
// than -1) the moment it is DISCOVERED (pushed into the queue), not when it
// is popped/processed. Marking on pop would let the same node be pushed
// multiple times by different in-progress neighbors before any of them gets
// a chance to mark it, which both wastes work and can corrupt the distance
// calculation. See the README's Common Mistakes section for the bug this
// avoids.
// ----------------------------------------------------------------------------
std::vector<int> bfsShortestPath(const std::vector<std::vector<int>>& adj, int src) {
  int n = static_cast<int>(adj.size());
  std::vector<int> dist(n, -1);  // -1 means "not yet reached / unreachable".

  if (src < 0 || src >= n) {
    return dist;  // Out-of-range source: nothing is reachable.
  }

  std::queue<int> frontier;
  dist[src] = 0;       // Mark visited THE MOMENT we enqueue, not on pop.
  frontier.push(src);

  while (!frontier.empty()) {
    int node = frontier.front();
    frontier.pop();

    for (int neighbor : adj[node]) {
      if (dist[neighbor] == -1) {           // Not yet discovered.
        dist[neighbor] = dist[node] + 1;    // One hop farther than `node`.
        frontier.push(neighbor);            // Mark-on-enqueue, not on-pop.
      }
    }
  }

  return dist;
}

// ----------------------------------------------------------------------------
// dfsConnectedComponents — counts the number of connected components,
// treating the graph as UNDIRECTED regardless of how `adj` was built.
//
// Real graphs are frequently disconnected: a social network has isolated
// users with no friends yet; a service-dependency graph has services no one
// else calls. Counting components requires looping over EVERY node as a
// potential unvisited start point — a single traversal from one node only
// ever reaches that node's own component, never the others. Forgetting this
// outer loop is one of the most common Graph BFS/DFS bugs (see Common
// Mistakes).
//
// This function symmetrizes the adjacency internally (adding both u->v and
// v->u for every edge it finds) so it produces a correct undirected
// component count even if `adj` was built as a directed structure — a
// deliberate robustness choice so callers do not need to remember which
// convention their adjacency list follows.
// ----------------------------------------------------------------------------
int dfsConnectedComponents(const std::vector<std::vector<int>>& adj) {
  int n = static_cast<int>(adj.size());

  std::vector<std::vector<int>> undirected(n);
  for (int u = 0; u < n; ++u) {
    for (int v : adj[u]) {
      undirected[u].push_back(v);
      undirected[v].push_back(u);
    }
  }

  std::vector<bool> visited(n, false);
  int components = 0;

  // Recursive DFS. Marks `node` visited the instant it is entered — the DFS
  // analogue of "mark on discovery," which prevents the same node from
  // being explored twice through two different paths.
  std::function<void(int)> dfs = [&](int node) {
    visited[node] = true;
    for (int neighbor : undirected[node]) {
      if (!visited[neighbor]) {
        dfs(neighbor);
      }
    }
  };

  for (int start = 0; start < n; ++start) {
    if (!visited[start]) {
      ++components;   // Found a node nothing else has reached yet: new component.
      dfs(start);
    }
  }

  return components;
}

// ----------------------------------------------------------------------------
// hasCycleDirected — detects whether a DIRECTED graph contains a cycle,
// using DFS with a three-state marker per node:
//   0 (unvisited) — never entered.
//   1 (in progress / "on the recursion stack") — currently being explored;
//     an edge from the current DFS path back to one of these nodes is a
//     BACK EDGE, which is exactly what a cycle looks like in a directed
//     graph.
//   2 (done) — fully explored, including everything reachable from it; safe
//     to ignore on any future visit.
//
// Why a simple `visited` boolean is NOT enough here (unlike the undirected
// case above): in a directed graph, reaching an already-visited node again
// is completely normal and does NOT imply a cycle — e.g. 0->1, 0->2, 1->3,
// 2->3 revisits 3's *ancestor set* but 3 is not on the current path when
// either 1 or 2 reaches it. Only a back edge to a node still ON THE CURRENT
// PATH (state 1) proves a cycle. This distinction is the entire reason the
// recursion-stack marker exists.
// ----------------------------------------------------------------------------
bool hasCycleDirected(const std::vector<std::vector<int>>& adj) {
  int n = static_cast<int>(adj.size());
  std::vector<int> state(n, 0);  // 0 = unvisited, 1 = in progress, 2 = done.

  std::function<bool(int)> dfs = [&](int node) -> bool {
    state[node] = 1;  // Push onto the conceptual recursion stack.

    for (int neighbor : adj[node]) {
      if (state[neighbor] == 1) {
        return true;  // Back edge to a node on the current path: cycle.
      }
      if (state[neighbor] == 0 && dfs(neighbor)) {
        return true;  // Cycle found deeper in the recursion.
      }
      // state[neighbor] == 2: already fully explored via some other path,
      // and NOT on the current path -> safe, not a cycle, do not recurse
      // again (this is also what keeps the algorithm at O(V + E) instead
      // of re-exploring shared subgraphs).
    }

    state[node] = 2;  // Pop from the conceptual recursion stack: done.
    return false;
  };

  for (int start = 0; start < n; ++start) {
    if (state[start] == 0 && dfs(start)) {
      return true;
    }
  }

  return false;
}

// ============================================================================
// main() — demonstrates all three functions with printed, verifiable output.
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

  std::cout << "--- bfsShortestPath ---\n";
  {
    // Undirected graph, built symmetrically (edge u-v added both ways):
    //   0-1, 1-2, 2-3, 3-0 (a 4-cycle), plus a separate 4-5 edge,
    //   plus an isolated node 6.
    int n = 7;
    std::vector<std::vector<int>> adj(n);
    auto add_undirected_edge = [&](int u, int v) {
      adj[u].push_back(v);
      adj[v].push_back(u);
    };
    add_undirected_edge(0, 1);
    add_undirected_edge(1, 2);
    add_undirected_edge(2, 3);
    add_undirected_edge(3, 0);
    add_undirected_edge(4, 5);

    std::vector<int> dist = bfsShortestPath(adj, 0);
    std::vector<int> expected = {0, 1, 2, 1, -1, -1, -1};
    check(dist == expected,
          "BFS from node 0 on a 4-cycle + disjoint component -> correct hop counts, -1 for unreachable");

    // Cycle does not cause infinite loop or wrong distances: node 2 is
    // reachable via 0-1-2 (2 hops) or 0-3-2 (2 hops) -- BFS finds the
    // shortest one and does not loop forever around the cycle.
    check(dist[2] == 2, "shortest hop count through a cycle is still correct (2, not looping forever)");
  }

  std::cout << "\n--- dfsConnectedComponents ---\n";
  {
    // Same graph as above (built as DIRECTED edges this time, to prove the
    // function still treats it as undirected internally): 3 components ->
    // {0,1,2,3}, {4,5}, {6}.
    int n = 7;
    std::vector<std::vector<int>> adj(n);
    adj[0].push_back(1);
    adj[1].push_back(2);
    adj[2].push_back(3);
    adj[3].push_back(0);
    adj[4].push_back(5);
    // node 6 has no edges at all.

    int components = dfsConnectedComponents(adj);
    check(components == 3, "one-directional edges still yield 3 undirected components: {0,1,2,3}, {4,5}, {6}");
  }
  {
    // Fully connected graph -> exactly 1 component.
    int n = 4;
    std::vector<std::vector<int>> adj(n);
    auto add_undirected_edge = [&](int u, int v) {
      adj[u].push_back(v);
      adj[v].push_back(u);
    };
    add_undirected_edge(0, 1);
    add_undirected_edge(1, 2);
    add_undirected_edge(2, 3);
    check(dfsConnectedComponents(adj) == 1, "a connected path graph has exactly 1 component");
  }
  {
    // No edges at all -> every node is its own component.
    int n = 5;
    std::vector<std::vector<int>> adj(n);
    check(dfsConnectedComponents(adj) == 5, "5 isolated nodes -> 5 components");
  }

  std::cout << "\n--- hasCycleDirected ---\n";
  {
    // 0 -> 1 -> 2 -> 0 : a directed cycle.
    int n = 3;
    std::vector<std::vector<int>> adj(n);
    adj[0].push_back(1);
    adj[1].push_back(2);
    adj[2].push_back(0);
    check(hasCycleDirected(adj), "0->1->2->0 is a directed cycle -> true");
  }
  {
    // 0 -> 1, 0 -> 2, 1 -> 3, 2 -> 3 : a DAG (diamond shape, node 3 has two
    // parents but that is NOT a cycle).
    int n = 4;
    std::vector<std::vector<int>> adj(n);
    adj[0].push_back(1);
    adj[0].push_back(2);
    adj[1].push_back(3);
    adj[2].push_back(3);
    check(!hasCycleDirected(adj), "diamond-shaped DAG (node revisited via two parents, no back edge) -> false");
  }
  {
    // Disconnected graph where only the SECOND component has a cycle --
    // proves the outer loop over all start nodes is required.
    int n = 5;
    std::vector<std::vector<int>> adj(n);
    adj[0].push_back(1);  // component A: 0 -> 1, no cycle.
    adj[2].push_back(3);
    adj[3].push_back(4);
    adj[4].push_back(2);  // component B: 2 -> 3 -> 4 -> 2, a cycle.
    check(hasCycleDirected(adj), "cycle hidden in a second, otherwise-unreached component -> still detected");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
