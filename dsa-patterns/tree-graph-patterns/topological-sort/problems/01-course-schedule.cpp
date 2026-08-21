// ============================================================================
// LeetCode 207 — Course Schedule
// https://leetcode.com/problems/course-schedule/
// ============================================================================
//
// PROBLEM
// -------
// There are numCourses courses labelled 0 .. numCourses - 1. You are given a
// list prerequisites where prerequisites[i] = {a, b} means "you must take
// course b BEFORE course a." Return true if you can finish all courses.
//
// Example: numCourses = 2, prerequisites = {{1, 0}}
//          -> true  (take 0, then 1)
//          numCourses = 2, prerequisites = {{1, 0}, {0, 1}}
//          -> false (0 and 1 each require the other)
//
// This is the pattern's FIRST facet: "can this be ordered at all?" — i.e.
// pure cycle detection. The question never asks for the order itself, only
// whether one exists. Per ../README.md ("Problem" -> Term: DAG), a valid
// topological order exists IF AND ONLY IF the graph is acyclic, so
// "can I finish all courses?" and "is this graph a DAG?" are the same
// question wearing different clothes.
//
// KAHN'S (BFS) vs DFS POST-ORDER + REVERSE — the choice for this file
// -------------------------------------------------------------------
// CHOSEN: Kahn's algorithm (BFS with an in-degree queue).
// REASON: the only thing this problem needs is the cycle verdict, and in
// Kahn's algorithm that verdict is a single integer comparison at the very
// end — `placedCount == numCourses`. Nothing else. The DFS alternative would
// need a three-state colour array (white / gray / black) and a check for a
// back-edge into a gray node, which is more moving parts and more ways to be
// subtly wrong for an answer that Kahn's gets from a counter it was already
// keeping. We do not even need to store the order, so Kahn's here is
// literally "count how many nodes I could peel off."
// Use DFS instead when you are already traversing for another reason, or when
// you want the cycle's actual member nodes (the gray stack IS the cycle) —
// Kahn's leftover set tells you which nodes are stuck, but not the cycle's
// order around itself.
//
// APPROACH
// --------
// 1. Turn each {a, b} pair into the directed edge b -> a ("b before a") and
//    build adj[b] plus inDegree[a].
// 2. Seed a queue with every course whose in-degree is 0.
// 3. Pop, count it, decrement each dependent's in-degree, enqueue any that
//    reach 0.
// 4. Answer is `placedCount == numCourses`.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// The edge DIRECTION. LeetCode gives you {a, b} meaning "b is a prerequisite
// of a," so the edge that means "must come before" runs b -> a — the REVERSE
// of the pair's written order. Building adj[a].push_back(b) instead is the
// single most common bug in this problem family, and it does not always show
// up in testing: on a symmetric test graph the reversed build still reports
// the correct cycle verdict, so the bug can pass 207 and only surface in 210
// (where the order itself is returned). See ../README.md, "Common Mistakes",
// second bullet.
//
// COMPLEXITY
// ----------
// Time:  O(V + E) — V = numCourses, E = prerequisites.size(). One pass to
//        build the structures; each node enters/leaves the queue at most once;
//        each edge is walked exactly once, when its source is popped.
// Space: O(V + E) — adjacency list O(E), in-degree array O(V), queue O(V).
// ============================================================================

#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// canFinish — Kahn's algorithm, cycle verdict only.
//
// prerequisites[i] = {a, b} means "take b before a".
// ----------------------------------------------------------------------------
bool canFinish(int numCourses,
               const std::vector<std::pair<int, int> >& prerequisites) {
  // adj[u] = every course that lists u as a prerequisite, i.e. every course
  // that must come AFTER u. Writing this sentence down before coding is the
  // discipline that keeps the edge direction straight.
  std::vector<std::vector<int> > adj(static_cast<size_t>(numCourses));
  std::vector<int> inDegree(static_cast<size_t>(numCourses), 0);

  for (size_t i = 0; i < prerequisites.size(); ++i) {
    int after = prerequisites[i].first;    // a — the course being unlocked
    int before = prerequisites[i].second;  // b — the prerequisite
    // Edge runs before -> after. Note this is the REVERSE of how the pair is
    // written; see "THE DETAIL PEOPLE GET WRONG" above.
    adj[static_cast<size_t>(before)].push_back(after);
    ++inDegree[static_cast<size_t>(after)];
  }

  // Courses with no prerequisites at all — the initial frontier.
  std::queue<int> ready;
  for (int course = 0; course < numCourses; ++course) {
    if (inDegree[static_cast<size_t>(course)] == 0) {
      ready.push(course);
    }
  }

  // We deliberately do NOT accumulate the order here — only how many courses
  // we managed to peel off. That count is the entire answer.
  int placedCount = 0;

  while (!ready.empty()) {
    int current = ready.front();
    ready.pop();
    ++placedCount;

    const std::vector<int>& dependents = adj[static_cast<size_t>(current)];
    for (size_t i = 0; i < dependents.size(); ++i) {
      size_t next = static_cast<size_t>(dependents[i]);
      --inDegree[next];
      if (inDegree[next] == 0) {
        ready.push(dependents[i]);
      }
    }
  }

  // If some courses never reached in-degree 0, each was waiting on a
  // prerequisite that was itself (directly or transitively) waiting on it —
  // which is only possible inside a cycle.
  return placedCount == numCourses;
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

  typedef std::vector<std::pair<int, int> > Pairs;

  // --- LeetCode's own examples -------------------------------------------
  {
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    check(canFinish(2, p) == true, "LC example 1: 2 courses, 1 requires 0 -> true");
  }
  {
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(0, 1));
    check(canFinish(2, p) == false, "LC example 2: mutual prerequisites -> false");
  }

  // --- Edge cases --------------------------------------------------------
  {
    Pairs empty;
    check(canFinish(1, empty) == true, "single course, no prerequisites -> true");
    check(canFinish(5, empty) == true, "5 isolated courses, no edges -> true");
  }
  {
    // A self-loop is the smallest possible cycle: course 0 requires itself,
    // so its in-degree starts at 1 and can never fall to 0.
    Pairs p;
    p.push_back(std::pair<int, int>(0, 0));
    check(canFinish(1, p) == false, "self-loop (course requires itself) -> false");
  }
  {
    // DUPLICATE edges. The same prerequisite listed twice raises the
    // in-degree to 2, but adj also holds the target twice, so processing the
    // source decrements it twice — the two structures stay consistent and
    // the verdict is unaffected. Duplicates are only dangerous if you
    // de-duplicate ONE of the two structures and not the other.
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(1, 0));
    check(canFinish(2, p) == true, "duplicate identical prerequisite -> still true");
  }
  {
    // A long chain: 0 -> 1 -> 2 -> 3 -> 4. Valid, and exercises the case
    // where the queue holds exactly one node at every single step.
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(2, 1));
    p.push_back(std::pair<int, int>(3, 2));
    p.push_back(std::pair<int, int>(4, 3));
    check(canFinish(5, p) == true, "linear chain of 5 courses -> true");
  }
  {
    // A cycle buried inside an otherwise-fine graph: 0 is free, but
    // 1 -> 2 -> 3 -> 1 is a 3-cycle, and 4 depends on 3 so it is stuck too.
    // This is the "mostly a DAG with one cyclic pocket" case from
    // ../README.md, "Common Mistakes", fifth bullet.
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(2, 1));
    p.push_back(std::pair<int, int>(3, 2));
    p.push_back(std::pair<int, int>(1, 3));
    p.push_back(std::pair<int, int>(4, 3));
    check(canFinish(5, p) == false, "3-cycle hidden inside a larger graph -> false");
  }
  {
    // A DIAMOND: 3 depends on both 1 and 2, which both depend on 0. Node 3
    // needs two separate decrements before it becomes eligible — the case
    // the trace diagram walks through for node 3.
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(2, 0));
    p.push_back(std::pair<int, int>(3, 1));
    p.push_back(std::pair<int, int>(3, 2));
    check(canFinish(4, p) == true, "diamond dependency (two paths into one node) -> true");
  }
  {
    // Two independent components, one acyclic and one cyclic. Topological
    // sort reports failure for the WHOLE input, not "order what I can" —
    // see ../README.md, "Tradeoffs", last paragraph.
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(3, 2));
    p.push_back(std::pair<int, int>(2, 3));
    check(canFinish(4, p) == false, "clean component + cyclic component -> false overall");
  }

  std::cout << "\n" << passCount << " passed, " << failCount << " failed.\n";
  return failCount == 0 ? 0 : 1;
}
