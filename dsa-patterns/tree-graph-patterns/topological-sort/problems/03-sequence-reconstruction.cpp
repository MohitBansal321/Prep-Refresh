// ============================================================================
// LeetCode 444 — Sequence Reconstruction
// https://leetcode.com/problems/sequence-reconstruction/
// ============================================================================
//
// PROBLEM
// -------
// You are given an integer array `nums` of length n, which is a permutation of
// the integers 1 .. n. You are also given a list `sequences`, where each
// sequences[i] is a subsequence of nums. Decide whether `nums` is the SHORTEST
// possible supersequence of all the given sequences AND the ONLY one of that
// shortest length. Return true only if both hold.
//
// Example: nums = {1,2,3}, sequences = {{1,2},{1,3}}          -> false
//            (both {1,2,3} and {1,3,2} are valid length-3 supersequences)
//          nums = {1,2,3}, sequences = {{1,2},{1,3},{2,3}}    -> true
//            ({1,2,3} is the only length-3 supersequence)
//
// Reframed as this module's pattern: each sequence {a, b, c} is really the
// constraint chain a -> b -> c ("a must come before b, b before c"). So the
// question becomes: does the resulting dependency graph have EXACTLY ONE
// topological order, and is that order `nums`?
//
// This is the pattern's THIRD facet: the UNIQUENESS question. ../README.md's
// "Interview Discussion" lists "topological sort always produces a unique
// answer" as the first misconception, and ../images/trace-diagram.md's "How to
// read it" spells out why: the moment two nodes sit at in-degree 0
// simultaneously, they can be emitted in either relative order, so at least
// two valid orders exist. This problem turns that observation into the test.
//
// KAHN'S (BFS) vs DFS POST-ORDER + REVERSE — the choice for this file
// -------------------------------------------------------------------
// CHOSEN: Kahn's algorithm (BFS with an in-degree queue). Not merely
// convenient here — it is the only one of the two that can answer the
// question directly.
// REASON: uniqueness has an exact, local characterisation in Kahn's terms:
// the topological order is unique IF AND ONLY IF the ready queue holds exactly
// ONE node at every single step. If it ever holds two, those two are mutually
// unordered and swapping them yields a second, equally valid order; if it
// holds one every time, no choice was ever available and the order that came
// out is the only one that could have. The ready queue IS "the set of things I
// am currently free to choose between," so its size is literally a count of
// the branching factor at that step.
// DFS post-order + reverse has no equivalent observable. DFS commits to one
// child at a time based on adjacency-list order and never materialises "the
// set of currently-legal next nodes," so recovering uniqueness from it means
// re-deriving Kahn's frontier on the side — i.e. writing Kahn's anyway. This
// is the sharpest example in the module of the two approaches NOT being
// interchangeable.
//
// APPROACH
// --------
// 1. For every sequence, add an edge for each CONSECUTIVE pair only:
//    {a, b, c} contributes a -> b and b -> c. Nothing else.
// 2. De-duplicate edges (the same pair can appear in several sequences), then
//    build adj + inDegree from the de-duplicated set.
// 3. Run Kahn's algorithm, and at the top of every iteration assert
//    ready.size() == 1. The moment it is larger, return false: more than one
//    valid order exists, so `nums` cannot be the only one.
// 4. Each node popped must match nums at the current index; a mismatch means
//    the unique order exists but is not `nums`.
// 5. After the loop, the emitted count must equal n (otherwise a cycle exists
//    and no order at all is valid).
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// There are THREE separate conditions, and most attempts implement one or two
// of them: (a) the queue must never hold more than one node — uniqueness;
// (b) the popped node must equal nums[i] at every step — it is the RIGHT
// unique order; (c) the total emitted count must equal n — no cycle. Dropping
// (a) accepts {1,2,3} for sequences {{1,2},{1,3}}. Dropping (b) accepts nums
// that is merely SOME valid order rather than the computed one. Dropping (c)
// accepts cyclic input.
//
// A second, subtler trap: a value in 1..n that appears in NO sequence is
// unconstrained, so `nums` cannot be the shortest supersequence (a shorter one
// simply omits that value). No extra guard is needed for it, and the reason is
// worth working through — see the comment inside the function.
//
// Third trap: adding an edge for every pair (i < j) in a sequence instead of
// only consecutive pairs. Transitivity already implies a -> c from a -> b and
// b -> c, so the extra edges change no answer but turn an O(total length) edge
// build into O(total length squared).
//
// COMPLEXITY
// ----------
// Let L be the total number of integers across all sequences, so the number of
// candidate edges is O(L).
// Time:  O(n + L log L) — the log factor comes from the std::set used to
//        de-duplicate edges; with a hash set it is O(n + L) expected.
// Space: O(n + L) — the de-duplicated edge set, adjacency list, in-degree
//        array, and queue.
// ============================================================================

#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// sequenceReconstruction — Kahn's algorithm with a per-step uniqueness check.
// ----------------------------------------------------------------------------
bool sequenceReconstruction(const std::vector<int>& nums,
                            const std::vector<std::vector<int> >& sequences) {
  int n = static_cast<int>(nums.size());
  if (n == 0) return false;

  // Nodes are the values 1 .. n, stored at indices 1 .. n (index 0 unused) so
  // the array index and the value itself match — no off-by-one juggling.
  std::vector<std::vector<int> > adj(static_cast<size_t>(n) + 1);
  std::vector<int> inDegree(static_cast<size_t>(n) + 1, 0);

  // De-duplicate edges first. The same ordered pair can legitimately appear in
  // several sequences (LeetCode's own example has 5 -> 2 in two of them).
  //
  // Why this matters: inDegree[v] is supposed to mean "how many DISTINCT
  // prerequisites does v still have." If you count a duplicate pair twice in
  // inDegree you must also store it twice in adj so the two decrements cancel
  // the two increments. That works, but it makes the invariant depend on both
  // structures being built from the identical multiset of edges — and the
  // classic bug here is de-duplicating one and not the other (e.g. computing
  // in-degrees from a set of predecessors while pushing raw pairs into adj),
  // which leaves nodes that can never reach 0. Building both from ONE
  // de-duplicated set removes the whole class of mistake.
  std::set<std::pair<int, int> > edges;
  for (size_t s = 0; s < sequences.size(); ++s) {
    const std::vector<int>& seq = sequences[s];
    for (size_t i = 0; i + 1 < seq.size(); ++i) {
      int from = seq[i];
      int to = seq[i + 1];
      // Values outside 1..n cannot be part of a permutation of 1..n, so their
      // presence makes `nums` an impossible supersequence outright.
      if (from < 1 || from > n || to < 1 || to > n) return false;
      // CONSECUTIVE pairs only. seq[i] -> seq[i+2] is implied transitively.
      edges.insert(std::pair<int, int>(from, to));
    }
  }

  std::set<std::pair<int, int> >::const_iterator it;
  for (it = edges.begin(); it != edges.end(); ++it) {
    adj[static_cast<size_t>(it->first)].push_back(it->second);
    ++inDegree[static_cast<size_t>(it->second)];
  }

  std::queue<int> ready;
  for (int value = 1; value <= n; ++value) {
    if (inDegree[static_cast<size_t>(value)] == 0) {
      ready.push(value);
    }
  }

  // No separate guard is needed for "some value appears in no sequence at
  // all," even though such a value makes the answer false. Such a value has
  // in-degree 0 and no outgoing edges. If n == 1 it is the whole input, and
  // LeetCode guarantees at least one non-empty sequence, so it must have been
  // mentioned. If n > 1, the remaining nodes either contain a cycle (caught by
  // the final count check) or form a non-empty DAG, which always has at least
  // one node of in-degree 0 of its own — so the queue starts with at least two
  // entries and the size check below rejects the input on the first iteration.
  int index = 0;
  while (!ready.empty()) {
    // CONDITION (a) — uniqueness. Two or more simultaneously-ready nodes means
    // a genuine choice, and a choice means a second valid order exists.
    if (ready.size() > 1) return false;

    int current = ready.front();
    ready.pop();

    // CONDITION (b) — it must be nums, not merely some unique order.
    if (current != nums[static_cast<size_t>(index)]) return false;
    ++index;

    const std::vector<int>& dependents = adj[static_cast<size_t>(current)];
    for (size_t i = 0; i < dependents.size(); ++i) {
      size_t next = static_cast<size_t>(dependents[i]);
      --inDegree[next];
      if (inDegree[next] == 0) {
        ready.push(dependents[i]);
      }
    }
  }

  // CONDITION (c) — no cycle. Same length check as every other file here.
  return index == n;
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

  typedef std::vector<int> Ints;
  typedef std::vector<std::vector<int> > Seqs;

  // --- LeetCode's own examples -------------------------------------------
  {
    // {1,3,2} is an equally short supersequence, so nums is not the only one.
    // Both 2 and 3 sit at in-degree 0 after 1 is emitted -> queue size 2.
    Ints nums;
    nums.push_back(1); nums.push_back(2); nums.push_back(3);
    Seqs seqs;
    Ints a; a.push_back(1); a.push_back(2); seqs.push_back(a);
    Ints b; b.push_back(1); b.push_back(3); seqs.push_back(b);
    check(sequenceReconstruction(nums, seqs) == false,
          "LC example 1: {{1,2},{1,3}} leaves 2 and 3 unordered -> false");
  }
  {
    // Only {1,2} given: 3 is never mentioned, so a shorter supersequence
    // ({1,2}) exists. The queue starts as {1, 3} -> size 2 -> rejected.
    Ints nums;
    nums.push_back(1); nums.push_back(2); nums.push_back(3);
    Seqs seqs;
    Ints a; a.push_back(1); a.push_back(2); seqs.push_back(a);
    check(sequenceReconstruction(nums, seqs) == false,
          "LC example 2: value 3 unconstrained -> false (nums is not shortest)");
  }
  {
    // Every adjacent pair pinned down: 1 -> 2, 2 -> 3, plus a redundant
    // 1 -> 3. Exactly one node is ready at every step.
    Ints nums;
    nums.push_back(1); nums.push_back(2); nums.push_back(3);
    Seqs seqs;
    Ints a; a.push_back(1); a.push_back(2); seqs.push_back(a);
    Ints b; b.push_back(1); b.push_back(3); seqs.push_back(b);
    Ints c; c.push_back(2); c.push_back(3); seqs.push_back(c);
    check(sequenceReconstruction(nums, seqs) == true,
          "LC example 3: all three pairs constrained -> true (unique order)");
  }
  {
    // The larger LeetCode example. Note 5 -> 2 appears in BOTH sequences,
    // which is exactly the duplicate-edge case the set de-duplicates.
    Ints nums;
    nums.push_back(4); nums.push_back(1); nums.push_back(5);
    nums.push_back(2); nums.push_back(6); nums.push_back(3);
    Seqs seqs;
    Ints a; a.push_back(5); a.push_back(2); a.push_back(6); a.push_back(3);
    seqs.push_back(a);
    Ints b; b.push_back(4); b.push_back(1); b.push_back(5); b.push_back(2);
    seqs.push_back(b);
    check(sequenceReconstruction(nums, seqs) == true,
          "LC example 4: 6-value chain with a duplicated 5->2 edge -> true");
  }

  // --- Uniqueness exists, but the order is NOT nums ----------------------
  {
    // Edges 2 -> 1 and 1 -> 3 force the unique order {2,1,3}, which is not
    // the given nums {1,2,3}. Condition (b) catches this; a solution that
    // only checked queue size and final count would wrongly return true.
    Ints nums;
    nums.push_back(1); nums.push_back(2); nums.push_back(3);
    Seqs seqs;
    Ints a; a.push_back(2); a.push_back(1); seqs.push_back(a);
    Ints b; b.push_back(1); b.push_back(3); seqs.push_back(b);
    check(sequenceReconstruction(nums, seqs) == false,
          "unique order {2,1,3} exists but differs from nums -> false");
  }

  // --- Cycles ------------------------------------------------------------
  {
    // 1 -> 2 and 2 -> 1: no valid order at all. The queue is empty from the
    // start, so index stays 0 and the final count check rejects it.
    Ints nums;
    nums.push_back(1); nums.push_back(2);
    Seqs seqs;
    Ints a; a.push_back(1); a.push_back(2); seqs.push_back(a);
    Ints b; b.push_back(2); b.push_back(1); seqs.push_back(b);
    check(sequenceReconstruction(nums, seqs) == false,
          "contradictory sequences form a cycle -> false");
  }
  {
    // A 3-cycle: 1 -> 2 -> 3 -> 1. Nothing ever becomes ready.
    Ints nums;
    nums.push_back(1); nums.push_back(2); nums.push_back(3);
    Seqs seqs;
    Ints a; a.push_back(1); a.push_back(2); a.push_back(3); seqs.push_back(a);
    Ints b; b.push_back(3); b.push_back(1); seqs.push_back(b);
    check(sequenceReconstruction(nums, seqs) == false,
          "3-cycle across two sequences -> false");
  }

  // --- Edge cases --------------------------------------------------------
  {
    Ints nums; nums.push_back(1);
    Seqs seqs;
    Ints a; a.push_back(1); seqs.push_back(a);
    check(sequenceReconstruction(nums, seqs) == true,
          "single value, single one-element sequence -> true");
  }
  {
    // A one-element sequence contributes NO edges (there is no consecutive
    // pair inside it), so with n = 2 both values start ready.
    Ints nums; nums.push_back(1); nums.push_back(2);
    Seqs seqs;
    Ints a; a.push_back(1); seqs.push_back(a);
    Ints b; b.push_back(2); seqs.push_back(b);
    check(sequenceReconstruction(nums, seqs) == false,
          "two singleton sequences give no edges -> false (order not forced)");
  }
  {
    // Exactly the same pair repeated: de-duplication must not change the
    // verdict, and the single chain must remain unique.
    Ints nums; nums.push_back(1); nums.push_back(2); nums.push_back(3);
    Seqs seqs;
    Ints a; a.push_back(1); a.push_back(2); seqs.push_back(a);
    Ints b; b.push_back(1); b.push_back(2); seqs.push_back(b);
    Ints c; c.push_back(2); c.push_back(3); seqs.push_back(c);
    check(sequenceReconstruction(nums, seqs) == true,
          "identical duplicated pair plus a chain -> true");
  }
  {
    // One long sequence covering everything: the full order is pinned by a
    // single chain of consecutive pairs.
    Ints nums;
    nums.push_back(1); nums.push_back(2); nums.push_back(3); nums.push_back(4);
    Seqs seqs;
    Ints a;
    a.push_back(1); a.push_back(2); a.push_back(3); a.push_back(4);
    seqs.push_back(a);
    check(sequenceReconstruction(nums, seqs) == true,
          "one sequence equal to nums itself -> true");
  }
  {
    // Reversed nums against that same chain: unique order exists, wrong nums.
    Ints nums;
    nums.push_back(4); nums.push_back(3); nums.push_back(2); nums.push_back(1);
    Seqs seqs;
    Ints a;
    a.push_back(1); a.push_back(2); a.push_back(3); a.push_back(4);
    seqs.push_back(a);
    check(sequenceReconstruction(nums, seqs) == false,
          "nums reversed relative to the forced chain -> false");
  }

  std::cout << "\n" << passCount << " passed, " << failCount << " failed.\n";
  return failCount == 0 ? 0 : 1;
}
