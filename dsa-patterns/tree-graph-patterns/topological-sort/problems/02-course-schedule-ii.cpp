// ============================================================================
// LeetCode 210 — Course Schedule II
// https://leetcode.com/problems/course-schedule-ii/
// ============================================================================
//
// PROBLEM
// -------
// Same input as LeetCode 207: numCourses courses labelled 0 .. numCourses - 1,
// and prerequisites[i] = {a, b} meaning "you must take course b BEFORE course
// a." But now return an ACTUAL ORDER in which all courses can be taken. If
// no such order exists, return an empty vector. If several orders are valid,
// return any one of them.
//
// Example: numCourses = 4, prerequisites = {{1,0},{2,0},{3,1},{3,2}}
//          -> {0, 1, 2, 3} or {0, 2, 1, 3} — both are valid.
//
// This is the pattern's SECOND facet: "produce one valid order." Note how
// little changes from 01-course-schedule.cpp — the algorithm is identical, we
// just keep the popped nodes instead of only counting them. That is the whole
// difference between the two most-asked topological sort problems, and it is
// why ../images/recognition-diagram.md draws them as two branches off the
// same node rather than as two different techniques.
//
// KAHN'S (BFS) vs DFS POST-ORDER + REVERSE — the choice for this file
// -------------------------------------------------------------------
// CHOSEN: Kahn's algorithm (BFS with an in-degree queue).
// REASON: two concrete reasons, both about the OUTPUT.
//   (1) Kahn's emits nodes in FORWARD order — the first node popped is the
//       first course you take. DFS post-order emits them BACKWARD (a node is
//       finished only after everything it depends on... in the reversed
//       edge sense), so DFS needs a final std::reverse before the answer is
//       usable. One more step, one more thing to forget.
//   (2) The failure contract here is "return an empty vector," and Kahn's
//       gives that for free: if order.size() != numCourses, throw the partial
//       order away. With DFS you must abort a recursion that is already
//       several frames deep the moment you touch a gray node, and unwind
//       cleanly without leaving a half-built result behind.
// Use DFS instead when recursion is already the shape of your traversal, or
// when the graph is given as a lazily-explored structure where computing
// in-degrees up front would mean an extra full pass you cannot afford.
//
// APPROACH
// --------
// 1. Build adj[b] and inDegree[a] from each {a, b} pair (edge b -> a).
// 2. Seed a queue with every zero-in-degree course.
// 3. Pop -> APPEND TO `order` -> decrement dependents -> enqueue new zeros.
// 4. If order.size() == numCourses, return it; otherwise return {}.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// Returning the partial order when a cycle exists. The loop terminates
// normally on a cyclic graph — the queue simply runs dry early — so nothing
// crashes and nothing warns you. Without the final length check you hand back
// a list that is a perfectly valid order for a SUBSET of the courses and looks
// completely plausible. The length check is the only thing standing between
// "correct" and "silently, confidently wrong." See ../README.md, "Common
// Mistakes", first bullet, and ../images/flow-diagram.md's LengthCheck box.
//
// COMPLEXITY
// ----------
// Time:  O(V + E). Space: O(V + E), including the O(V) output.
// ============================================================================

#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// findOrder — Kahn's algorithm, returning the order itself.
//
// Returns a valid course order, or an empty vector if the prerequisite graph
// contains a cycle. (numCourses == 0 also yields an empty vector; the two
// cases are indistinguishable in this return type, which is exactly why
// ../code.cpp's topologicalSort returns an explicit `hasCycle` flag instead.
// LeetCode's signature forces the ambiguity on us here.)
// ----------------------------------------------------------------------------
std::vector<int> findOrder(int numCourses,
                           const std::vector<std::pair<int, int> >& prerequisites) {
  // adj[u] = every course that must come AFTER u.
  std::vector<std::vector<int> > adj(static_cast<size_t>(numCourses));
  std::vector<int> inDegree(static_cast<size_t>(numCourses), 0);

  for (size_t i = 0; i < prerequisites.size(); ++i) {
    int after = prerequisites[i].first;    // a
    int before = prerequisites[i].second;  // b, the prerequisite
    adj[static_cast<size_t>(before)].push_back(after);
    ++inDegree[static_cast<size_t>(after)];
  }

  std::queue<int> ready;
  for (int course = 0; course < numCourses; ++course) {
    if (inDegree[static_cast<size_t>(course)] == 0) {
      ready.push(course);
    }
  }

  std::vector<int> order;
  order.reserve(static_cast<size_t>(numCourses));

  while (!ready.empty()) {
    int current = ready.front();
    ready.pop();
    // Unlike 01, we KEEP the node. This single line is the entire difference
    // between "is it possible?" and "give me the plan."
    order.push_back(current);

    const std::vector<int>& dependents = adj[static_cast<size_t>(current)];
    for (size_t i = 0; i < dependents.size(); ++i) {
      size_t next = static_cast<size_t>(dependents[i]);
      --inDegree[next];
      if (inDegree[next] == 0) {
        ready.push(dependents[i]);
      }
    }
  }

  if (order.size() != static_cast<size_t>(numCourses)) {
    // Cycle. The partial `order` is a valid order for the courses it does
    // contain, which is precisely what makes returning it so dangerous.
    return std::vector<int>();
  }
  return order;
}

// ----------------------------------------------------------------------------
// Test helper: verify a claimed order is legal, WITHOUT assuming one specific
// answer. When several courses sit at in-degree 0 simultaneously, Kahn's
// algorithm's output depends on node numbering and FIFO order, so asserting a
// single hard-coded permutation would be testing an implementation detail
// rather than correctness. What actually defines a correct answer is:
//   (a) it is a permutation of 0 .. numCourses - 1 (each course exactly once);
//   (b) every prerequisite appears strictly before the course it unlocks.
// ----------------------------------------------------------------------------
bool isValidOrder(int numCourses,
                  const std::vector<std::pair<int, int> >& prerequisites,
                  const std::vector<int>& order) {
  if (order.size() != static_cast<size_t>(numCourses)) return false;

  std::vector<int> position(static_cast<size_t>(numCourses), -1);
  for (size_t i = 0; i < order.size(); ++i) {
    int course = order[i];
    if (course < 0 || course >= numCourses) return false;
    if (position[static_cast<size_t>(course)] != -1) return false;  // duplicate
    position[static_cast<size_t>(course)] = static_cast<int>(i);
  }

  for (size_t i = 0; i < prerequisites.size(); ++i) {
    int after = prerequisites[i].first;
    int before = prerequisites[i].second;
    if (position[static_cast<size_t>(before)] >= position[static_cast<size_t>(after)]) {
      return false;  // prerequisite did not come strictly first
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

  typedef std::vector<std::pair<int, int> > Pairs;

  // --- LeetCode's own examples -------------------------------------------
  {
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    std::vector<int> order = findOrder(2, p);
    check(isValidOrder(2, p, order), "LC example 1: {{1,0}} -> valid 2-course order");
    check(order.size() == 2 && order[0] == 0 && order[1] == 1,
          "LC example 1: order is forced, must be exactly {0, 1}");
  }
  {
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(2, 0));
    p.push_back(std::pair<int, int>(3, 1));
    p.push_back(std::pair<int, int>(3, 2));
    std::vector<int> order = findOrder(4, p);
    check(isValidOrder(4, p, order), "LC example 2: diamond graph -> valid order");
    check(order.front() == 0 && order.back() == 3,
          "LC example 2: 0 must be first and 3 must be last (only 1 vs 2 is free)");

    std::cout << "  computed order: ";
    for (size_t i = 0; i < order.size(); ++i) std::cout << order[i] << " ";
    std::cout << "\n";
  }
  {
    Pairs empty;
    std::vector<int> order = findOrder(1, empty);
    check(order.size() == 1 && order[0] == 0, "LC example 3: single course -> {0}");
  }

  // --- Cycles must yield an EMPTY vector, not a partial order ------------
  {
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(0, 1));
    check(findOrder(2, p).empty(), "mutual prerequisites -> empty vector");
  }
  {
    // The dangerous shape: course 0 is orderable, courses 1/2/3 form a cycle.
    // A missing length check would return {0} here and look reasonable.
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(2, 1));
    p.push_back(std::pair<int, int>(3, 2));
    p.push_back(std::pair<int, int>(1, 3));
    check(findOrder(4, p).empty(),
          "orderable prefix + downstream cycle -> empty, NOT the partial order");
  }
  {
    Pairs p;
    p.push_back(std::pair<int, int>(0, 0));
    check(findOrder(1, p).empty(), "self-loop -> empty vector");
  }

  // --- Edge cases --------------------------------------------------------
  {
    Pairs empty;
    std::vector<int> order = findOrder(4, empty);
    check(isValidOrder(4, empty, order),
          "no prerequisites at all -> every course present, any permutation valid");
  }
  {
    // Reverse-numbered chain: 0 requires 1 requires 2 requires 3. The only
    // valid order is descending, which also proves the edge direction is not
    // accidentally aligned with the natural node numbering.
    Pairs p;
    p.push_back(std::pair<int, int>(0, 1));
    p.push_back(std::pair<int, int>(1, 2));
    p.push_back(std::pair<int, int>(2, 3));
    std::vector<int> order = findOrder(4, p);
    check(isValidOrder(4, p, order), "reverse-numbered chain -> valid order");
    check(order.size() == 4 && order[0] == 3 && order[3] == 0,
          "reverse-numbered chain: order must be exactly {3, 2, 1, 0}");
  }
  {
    // Duplicate edges: in-degree counts them twice and adj lists the target
    // twice, so the two decrements cancel the two increments. Consistency
    // between the structures is what makes this safe.
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(1, 0));
    std::vector<int> order = findOrder(2, p);
    check(order.size() == 2 && order[0] == 0, "duplicate edges -> still a valid order");
  }
  {
    // A wide graph: one root unlocking many independent courses. Every
    // permutation of 1..4 after 0 is valid, so only the constraint check
    // is meaningful.
    Pairs p;
    p.push_back(std::pair<int, int>(1, 0));
    p.push_back(std::pair<int, int>(2, 0));
    p.push_back(std::pair<int, int>(3, 0));
    p.push_back(std::pair<int, int>(4, 0));
    std::vector<int> order = findOrder(5, p);
    check(isValidOrder(5, p, order), "one root unlocking four leaves -> valid order");
    check(order[0] == 0, "one root unlocking four leaves: root comes first");
  }

  std::cout << "\n" << passCount << " passed, " << failCount << " failed.\n";
  return failCount == 0 ? 0 : 1;
}
