// ============================================================================
// LeetCode 269 — Alien Dictionary
// https://leetcode.com/problems/alien-dictionary/
// ============================================================================
//
// PROBLEM
// -------
// There is a language that uses the English lowercase letters but in an
// unknown alphabetical order. You are given a list of `words` from that
// language's dictionary, and the list IS sorted lexicographically according to
// that unknown order. Return a string of the unique letters in the correct
// alien order. If the input is inconsistent (no such order exists), return "".
// If several orders are valid, return any of them.
//
// Example: words = {"wrt","wrf","er","ett","rftt"}  ->  "wertf"
//          words = {"z","x"}                        ->  "zx"
//          words = {"z","x","z"}                    ->  ""   (contradiction)
//
// This is the pattern's FOURTH facet, and the hardest kind: THE GRAPH IS NOT
// GIVEN. Nobody hands you nodes and edges — you get a list of words. The real
// work is realising that "this word list is sorted" is a compressed pile of
// "letter X must come before letter Y" constraints, extracting them, and only
// THEN running a completely ordinary topological sort. ../README.md's
// "Interview Discussion" flags this directly: "Topological sort only applies to
// explicitly-graph-shaped problems" is listed as a misconception, and
// ../images/recognition-diagram.md's closing line warns to watch for problems
// that hide the graph entirely. This is that problem.
//
// HOW THE GRAPH IS DERIVED — the actual insight
// ----------------------------------------------
// Take two ADJACENT words in the sorted list, say "wrt" and "wrf". Because the
// list is sorted, "wrt" <= "wrf" in alien order. Scan them together from the
// left: 'w' == 'w', 'r' == 'r', then 't' != 'f'. That FIRST mismatch is the
// only position that decided the comparison, so it tells you exactly one fact:
// 't' comes before 'f'. Every later character tells you NOTHING — once the
// comparison was decided at index 2, the rest of both words is irrelevant.
//
// Two consequences people miss:
//   * Only the FIRST differing character of each adjacent pair yields an edge.
//     Emitting an edge for every differing position invents constraints that
//     the input never implied, and will happily produce "" (a false
//     contradiction) on perfectly valid input.
//   * Non-adjacent pairs add nothing. "word 1 before word 3" follows
//     transitively from the two adjacent comparisons, so comparing all pairs
//     is O(W^2) work for zero extra information.
//
// And one hard failure case that has nothing to do with cycles: if the two
// words share a full prefix and the FIRST one is LONGER — {"abc", "ab"} — then
// no letter ordering can explain it, because a prefix always sorts before the
// longer word it is a prefix of. This input is invalid and must return "". It
// is the single most commonly forgotten branch in this problem, because it
// fails a length comparison rather than the graph algorithm.
//
// KAHN'S (BFS) vs DFS POST-ORDER + REVERSE — the choice for this file
// -------------------------------------------------------------------
// CHOSEN: DFS post-order + reverse (with white/gray/black colouring).
// REASON: three of them, and the first is deliberate.
//   (1) This is the module's one concrete implementation of the alternative
//       described in ../README.md's "Solution" and "Tradeoffs" sections. Files
//       01-03 all use Kahn's; reading the same pattern written the other way
//       is what makes the contrast real rather than a paragraph of prose.
//   (2) The node set is DISCOVERED, not given. There is no `numNodes`
//       parameter — you learn which letters exist only by scanning the words.
//       DFS needs nothing but a colour per letter, which the same scan can
//       fill in. Kahn's needs an in-degree count per node, and in-degree is a
//       property of INCOMING edges, so it cannot be finalised until the edge
//       set is complete: one more pass, over a node set you had to derive.
//   (3) Recursion depth is bounded at 26 — there are only 26 possible letters,
//       so the deepest possible chain is 26 frames. The usual argument against
//       DFS here (a deep graph can overflow the call stack, per ../README.md's
//       Disadvantages discussion of the DFS variant) simply does not apply at
//       this scale. On a build graph with 100k targets it would, and Kahn's
//       iterative loop would be the safer default.
// The cost of this choice, paid explicitly below: cycle detection needs a
// THREE-state colour (not a boolean visited flag), and the post-order output
// comes out backwards and must be reversed. Both are visible in the code.
//
// WHY THREE COLOURS AND NOT A VISITED BOOLEAN
// --------------------------------------------
//   WHITE (0) — not yet reached.
//   GRAY  (1) — currently on the recursion stack: we entered this node and
//               have not finished exploring its descendants.
//   BLACK (2) — fully explored; every descendant is finished and this node has
//               already been appended to the post-order output.
// Reaching a GRAY node means you followed edges from it and came back to it —
// a cycle, so no valid order exists. Reaching a BLACK node is completely fine:
// it just means two different letters both point at something already sorted
// out. A single `visited` boolean cannot tell those two cases apart, and
// conflating them either reports cycles that do not exist (treating any repeat
// visit as a cycle) or misses real ones (treating every repeat visit as fine).
//
// COMPLEXITY
// ----------
// Let C be the total number of characters across all words.
// Time:  O(C) — one pass to collect letters, one pass over adjacent word pairs
//        (each comparison stops at the first mismatch, and the total work is
//        bounded by C), then a DFS over at most 26 nodes and at most 26*25
//        distinct edges, which is O(1) at this alphabet size.
// Space: O(C) is not needed; the graph itself is O(1) — at most 26 nodes and
//        650 edges regardless of input size. The recursion depth is at most 26.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace {

const int ALPHABET = 26;
const int WHITE = 0;
const int GRAY = 1;
const int BLACK = 2;

// ----------------------------------------------------------------------------
// dfs — explore one letter, appending it to `postOrder` only after every
// letter reachable from it is finished. Returns false if a cycle is found.
// ----------------------------------------------------------------------------
bool dfs(int node,
         const std::vector<std::vector<int> >& adj,
         std::vector<int>& colour,
         std::string& postOrder) {
  colour[static_cast<size_t>(node)] = GRAY;  // now on the recursion stack

  const std::vector<int>& next = adj[static_cast<size_t>(node)];
  for (size_t i = 0; i < next.size(); ++i) {
    size_t child = static_cast<size_t>(next[i]);
    if (colour[child] == GRAY) {
      return false;  // back-edge into a node we are still inside -> cycle
    }
    if (colour[child] == WHITE) {
      if (!dfs(next[i], adj, colour, postOrder)) return false;
    }
    // colour[child] == BLACK: already fully ordered, nothing to do. This is
    // the case a plain `visited` boolean would confuse with the GRAY case.
  }

  colour[static_cast<size_t>(node)] = BLACK;
  // POST-ORDER: append only now, after all descendants. Every letter that must
  // come AFTER this one is therefore already in the string, which is exactly
  // why the finished string is the reverse of the answer.
  postOrder.push_back(static_cast<char>('a' + node));
  return true;
}

}  // namespace

// ----------------------------------------------------------------------------
// alienOrder — derive the constraint graph from the sorted word list, then
// topologically sort it with DFS post-order + reverse.
//
// Returns a valid letter order, or "" if the input is inconsistent.
// ----------------------------------------------------------------------------
std::string alienOrder(const std::vector<std::string>& words) {
  // ---- Step 1: discover the node set. Only letters that actually appear in
  // the input are part of the answer; the other letters of the 26 do not
  // exist in this language as far as we can tell.
  std::vector<bool> present(ALPHABET, false);
  for (size_t w = 0; w < words.size(); ++w) {
    for (size_t c = 0; c < words[w].size(); ++c) {
      present[static_cast<size_t>(words[w][c] - 'a')] = true;
    }
  }

  // ---- Step 2: derive the edges from adjacent word pairs.
  // A std::set de-duplicates: the same "x before y" fact can be implied by
  // many different word pairs, and letting it in twice would put the same
  // child twice in an adjacency list. Harmless for DFS (the second visit sees
  // BLACK and returns), but it would inflate in-degrees in a Kahn's-based
  // rewrite, so keeping the edge set clean keeps the two approaches
  // interchangeable.
  std::set<std::pair<int, int> > edges;

  for (size_t i = 0; i + 1 < words.size(); ++i) {
    const std::string& first = words[i];
    const std::string& second = words[i + 1];

    size_t limit = first.size() < second.size() ? first.size() : second.size();
    bool foundDifference = false;

    for (size_t k = 0; k < limit; ++k) {
      if (first[k] != second[k]) {
        // The FIRST mismatch is the whole story: first[k] sorts before
        // second[k]. Everything after index k is irrelevant, so stop.
        edges.insert(std::pair<int, int>(static_cast<int>(first[k] - 'a'),
                                        static_cast<int>(second[k] - 'a')));
        foundDifference = true;
        break;
      }
    }

    // The prefix trap: no mismatch inside the shared length, and the earlier
    // word is LONGER. "abc" can never sort before "ab" under any letter
    // ordering, so the dictionary itself is invalid. No graph work can detect
    // this — it is a pure length comparison.
    if (!foundDifference && first.size() > second.size()) {
      return "";
    }
  }

  std::vector<std::vector<int> > adj(ALPHABET);
  std::set<std::pair<int, int> >::const_iterator it;
  for (it = edges.begin(); it != edges.end(); ++it) {
    adj[static_cast<size_t>(it->first)].push_back(it->second);
  }

  // ---- Step 3: DFS every present letter, collecting post-order.
  std::vector<int> colour(ALPHABET, WHITE);
  std::string postOrder;

  for (int node = 0; node < ALPHABET; ++node) {
    if (!present[static_cast<size_t>(node)]) continue;
    if (colour[static_cast<size_t>(node)] != WHITE) continue;
    if (!dfs(node, adj, colour, postOrder)) {
      return "";  // cycle -> no consistent alphabet exists
    }
  }

  // ---- Step 4: reverse. This is the step Kahn's algorithm does not need,
  // and forgetting it yields a string that is exactly backwards — which,
  // annoyingly, still passes a naive "does it contain every letter" test.
  std::reverse(postOrder.begin(), postOrder.end());
  return postOrder;
}

// ----------------------------------------------------------------------------
// Test helpers. When several letter orders are valid, asserting one specific
// string tests the adjacency-iteration order rather than correctness. What
// actually defines a correct answer is: it contains exactly the distinct input
// letters (each once), and every derived "x before y" constraint holds.
// ----------------------------------------------------------------------------
bool hasExactlyLetters(const std::string& result, const std::string& expectedSet) {
  if (result.size() != expectedSet.size()) return false;
  std::string a = result;
  std::string b = expectedSet;
  std::sort(a.begin(), a.end());
  std::sort(b.begin(), b.end());
  return a == b;
}

bool respectsPairs(const std::string& result,
                   const std::vector<std::pair<char, char> >& mustPrecede) {
  for (size_t i = 0; i < mustPrecede.size(); ++i) {
    size_t posBefore = result.find(mustPrecede[i].first);
    size_t posAfter = result.find(mustPrecede[i].second);
    if (posBefore == std::string::npos || posAfter == std::string::npos) return false;
    if (posBefore >= posAfter) return false;
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

  typedef std::vector<std::string> Words;
  typedef std::vector<std::pair<char, char> > Constraints;

  // --- LeetCode's own examples -------------------------------------------
  {
    // Derived edges: t->f (wrt|wrf), w->e (wrf|er), r->t (er|ett),
    // e->r (ett|rftt). That is the chain w -> e -> r -> t -> f, which pins
    // the order completely, so an exact string comparison is legitimate here.
    Words words;
    words.push_back("wrt"); words.push_back("wrf"); words.push_back("er");
    words.push_back("ett"); words.push_back("rftt");
    std::string result = alienOrder(words);
    check(result == "wertf", "LC example 1: {wrt,wrf,er,ett,rftt} -> exactly \"wertf\"");
    std::cout << "  computed order: " << result << "\n";
  }
  {
    Words words;
    words.push_back("z"); words.push_back("x");
    check(alienOrder(words) == "zx", "LC example 2: {z,x} -> \"zx\"");
  }
  {
    // z->x from the first pair, x->z from the second: a 2-cycle. The DFS hits
    // a GRAY node and bails.
    Words words;
    words.push_back("z"); words.push_back("x"); words.push_back("z");
    check(alienOrder(words) == "", "LC example 3: {z,x,z} contradicts itself -> \"\"");
  }

  // --- The prefix trap ---------------------------------------------------
  {
    Words words;
    words.push_back("abc"); words.push_back("ab");
    check(alienOrder(words) == "",
          "prefix trap: longer word before its own prefix -> \"\"");
  }
  {
    // The legal direction of the same shape: prefix first is fine, and yields
    // no edge at all.
    Words words;
    words.push_back("ab"); words.push_back("abc");
    std::string result = alienOrder(words);
    check(hasExactlyLetters(result, "abc"),
          "prefix first is legal: {ab,abc} -> all three letters, no constraint");
  }
  {
    // Identical adjacent words: no mismatch, equal lengths, so no edge and no
    // error.
    Words words;
    words.push_back("z"); words.push_back("z");
    check(alienOrder(words) == "z", "duplicate identical words -> \"z\"");
  }

  // --- Only the FIRST differing character may become an edge -------------
  {
    // {"ba", "ab"} gives ONLY b->a (index 0). If you also emitted the index-1
    // difference a->b you would invent a 2-cycle and wrongly return "".
    Words words;
    words.push_back("ba"); words.push_back("ab");
    std::string result = alienOrder(words);
    Constraints c;
    c.push_back(std::pair<char, char>('b', 'a'));
    check(result == "ba", "only the first mismatch counts: {ba,ab} -> \"ba\", not \"\"");
    check(respectsPairs(result, c), "{ba,ab}: b precedes a");
  }

  // --- Partial orders: several valid answers -----------------------------
  {
    // Single edge b->d; a and c are unconstrained, so many orders are valid.
    Words words;
    words.push_back("ab"); words.push_back("adc");
    std::string result = alienOrder(words);
    Constraints c;
    c.push_back(std::pair<char, char>('b', 'd'));
    check(hasExactlyLetters(result, "abcd"), "{ab,adc}: all four letters present exactly once");
    check(respectsPairs(result, c), "{ab,adc}: b precedes d");
    std::cout << "  computed order: " << result << "\n";
  }
  {
    // A single word: every letter present, zero constraints.
    Words words;
    words.push_back("abc");
    std::string result = alienOrder(words);
    check(hasExactlyLetters(result, "abc"), "single word -> its distinct letters, any order");
  }
  {
    // Repeated letters inside words must not create self-loops (a letter is
    // never compared against itself, since the mismatch position differs).
    Words words;
    words.push_back("aa"); words.push_back("ab");
    std::string result = alienOrder(words);
    Constraints c;
    c.push_back(std::pair<char, char>('a', 'b'));
    check(hasExactlyLetters(result, "ab") && respectsPairs(result, c),
          "repeated letters: {aa,ab} -> a precedes b, no self-loop");
  }

  // --- Larger cycle, spread across several pairs -------------------------
  {
    // ac|bc gives a->b, ba|ca gives b->c, cb|ab gives c->a: a 3-cycle.
    Words words;
    words.push_back("ac"); words.push_back("bc");
    words.push_back("ba"); words.push_back("ca");
    words.push_back("cb"); words.push_back("ab");
    check(alienOrder(words) == "", "3-cycle spread over several word pairs -> \"\"");
  }

  // --- Edge cases --------------------------------------------------------
  {
    Words empty;
    check(alienOrder(empty) == "", "empty word list -> \"\" (no letters exist)");
  }
  {
    Words words;
    words.push_back("");
    check(alienOrder(words) == "", "single empty word -> \"\"");
  }
  {
    // Disconnected components: x->y from the first pair, and a->b from the
    // third, with nothing linking the two halves. Both constraints must hold.
    Words words;
    words.push_back("x"); words.push_back("y");
    words.push_back("ya"); words.push_back("yb");
    std::string result = alienOrder(words);
    Constraints c;
    c.push_back(std::pair<char, char>('x', 'y'));
    c.push_back(std::pair<char, char>('a', 'b'));
    check(hasExactlyLetters(result, "abxy") && respectsPairs(result, c),
          "two disconnected constraint components -> both respected");
    std::cout << "  computed order: " << result << "\n";
  }

  std::cout << "\n" << passCount << " passed, " << failCount << " failed.\n";
  return failCount == 0 ? 0 : 1;
}
