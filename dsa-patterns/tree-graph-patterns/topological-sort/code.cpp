// ============================================================================
// Topological Sort — Kahn's Algorithm, generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// BFS-based (Kahn's algorithm) shape of Topological Sort in its most generic
// form: given a number of nodes (0-indexed) and a list of directed edges
// {u, v} meaning "u must come before v," compute a valid processing order,
// or detect that no such order exists because the graph contains a cycle.
//
// The worked, problem-specific solutions (Course Schedule, Course Schedule
// II, Alien Dictionary, Minimum Height Trees) live in problems/*.cpp and each
// reimplement this same core idea inline so they stay dependency-free.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// Result of a topological sort attempt.
//
//   order    — the computed order, one entry per node that was successfully
//              placed. If hasCycle is false, this contains every node
//              (0 .. numNodes - 1) exactly once, in a valid dependency order.
//   hasCycle — true if the graph contains at least one cycle, in which case
//              `order` is a partial (and NOT usable) result: it contains only
//              the nodes that were reachable outside the cyclic portion of
//              the graph.
// ----------------------------------------------------------------------------
struct TopoResult {
  std::vector<int> order;
  bool hasCycle;
};

// ----------------------------------------------------------------------------
// topologicalSort — Kahn's algorithm.
//
// numNodes — total number of nodes, numbered 0 .. numNodes - 1.
// edges    — list of directed edges {u, v} meaning "u must come before v"
//            (equivalently: v depends on u, or v has u as a prerequisite).
//
// Returns a TopoResult. See the struct comment above for the exact contract.
//
// Time complexity:  O(V + E) — building adj/inDegree is O(E); the main loop
//                   enqueues/dequeues each node at most once (O(V)) and
//                   decrements each edge's target in-degree exactly once
//                   across the whole run (O(E) total, not per iteration).
// Space complexity: O(V + E) — the adjacency list stores each edge once,
//                   plus O(V) for inDegree, the queue, and the output order.
// ----------------------------------------------------------------------------
TopoResult topologicalSort(int numNodes,
                            const std::vector<std::pair<int, int>>& edges) {
  // adj[u] holds every node that directly depends on u — i.e. every v such
  // that the edge u -> v exists. inDegree[v] counts how many not-yet-placed
  // prerequisites node v currently has.
  std::vector<std::vector<int>> adj(numNodes);
  std::vector<int> inDegree(numNodes, 0);

  for (const auto& edge : edges) {
    int u = edge.first;
    int v = edge.second;
    adj[u].push_back(v);
    ++inDegree[v];
  }

  // Seed the queue with every node that has no prerequisites at all — these
  // are safe to place first, in any order relative to each other.
  std::queue<int> readyQueue;
  for (int node = 0; node < numNodes; ++node) {
    if (inDegree[node] == 0) {
      readyQueue.push(node);
    }
  }

  std::vector<int> order;
  order.reserve(static_cast<size_t>(numNodes));

  while (!readyQueue.empty()) {
    int current = readyQueue.front();
    readyQueue.pop();
    order.push_back(current);

    // "Remove" current from the graph: every node that depended on it now
    // has one fewer unmet prerequisite.
    for (int neighbor : adj[current]) {
      --inDegree[neighbor];
      if (inDegree[neighbor] == 0) {
        readyQueue.push(neighbor);
      }
    }
  }

  // The tell-tale sign of a cycle: if some nodes never reached in-degree 0,
  // they (and everything depending exclusively on them) never got placed.
  bool hasCycle = order.size() != static_cast<size_t>(numNodes);
  return {order, hasCycle};
}

// ----------------------------------------------------------------------------
// Helper used only by main(): verifies that a claimed order respects every
// edge constraint (every u appears before its corresponding v). This lets
// the demo confirm correctness without assuming any single "the" answer,
// since Kahn's algorithm only guarantees "a" valid order when multiple nodes
// share in-degree 0 at the same time.
// ----------------------------------------------------------------------------
bool respectsAllEdges(const std::vector<int>& order,
                       const std::vector<std::pair<int, int>>& edges) {
  std::vector<int> position(order.size());
  for (size_t i = 0; i < order.size(); ++i) {
    position[static_cast<size_t>(order[i])] = static_cast<int>(i);
  }
  for (const auto& edge : edges) {
    if (position[static_cast<size_t>(edge.first)] >=
        position[static_cast<size_t>(edge.second)]) {
      return false;  // u did not appear strictly before v.
    }
  }
  return true;
}

int main() {
  int passCount = 0;
  int failCount = 0;

  auto check = [&](bool condition, const std::string& label) {
    if (condition) {
      std::cout << "[PASS] " << label << "\n";
      ++passCount;
    } else {
      std::cout << "[FAIL] " << label << "\n";
      ++failCount;
    }
  };

  std::cout << "--- Case 1: valid DAG (course-prerequisite style graph) ---\n";
  {
    // 0: Intro to Programming
    // 1: Data Structures       (requires 0)
    // 2: Discrete Math         (no prerequisite)
    // 3: Algorithms            (requires 1 and 2)
    // 4: Operating Systems     (requires 1)
    // Edges mean "u must come before v".
    int numNodes = 5;
    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 3}, {2, 3}, {1, 4}};

    TopoResult result = topologicalSort(numNodes, edges);

    check(!result.hasCycle, "5-course DAG reports no cycle");
    check(result.order.size() == 5, "5-course DAG places all 5 courses");
    check(respectsAllEdges(result.order, edges),
          "5-course DAG order respects every prerequisite edge");

    std::cout << "  computed order: ";
    for (int node : result.order) std::cout << node << " ";
    std::cout << "\n";
  }

  std::cout << "\n--- Case 2: graph containing a cycle ---\n";
  {
    // 0 -> 1 -> 2 -> 0 forms a cycle; node 3 depends on 2 but can never be
    // placed because 2 itself never reaches in-degree 0.
    int numNodes = 4;
    std::vector<std::pair<int, int>> edges = {
        {0, 1}, {1, 2}, {2, 0}, {2, 3}};

    TopoResult result = topologicalSort(numNodes, edges);

    check(result.hasCycle, "4-node cyclic graph reports hasCycle == true");
    check(result.order.size() < 4,
          "4-node cyclic graph's partial order is shorter than numNodes");

    std::cout << "  partial order (unusable, cycle detected): ";
    for (int node : result.order) std::cout << node << " ";
    std::cout << "\n";
  }

  std::cout << "\n--- Case 3: edge cases ---\n";
  {
    // No edges at all: every node has in-degree 0, any permutation is valid.
    TopoResult result = topologicalSort(3, {});
    check(!result.hasCycle, "no edges -> no cycle");
    check(result.order.size() == 3, "no edges -> all 3 nodes placed");

    // A single self-loop is itself a (trivial) cycle: node 0 depends on
    // itself and can never reach in-degree 0.
    TopoResult selfLoop = topologicalSort(1, {{0, 0}});
    check(selfLoop.hasCycle, "self-loop is detected as a cycle");
    check(selfLoop.order.empty(), "self-loop node is never placed");
  }

  std::cout << "\n" << passCount << " passed, " << failCount << " failed.\n";
  return failCount == 0 ? 0 : 1;
}
