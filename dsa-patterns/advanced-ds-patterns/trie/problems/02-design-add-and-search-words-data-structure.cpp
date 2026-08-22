// ============================================================================
// LeetCode 211 — Design Add and Search Words Data Structure
// ============================================================================
//
// PROBLEM
// -------
// Design a structure supporting:
//   - addWord(word): add `word` to the structure.
//   - search(word):  return true iff some added word MATCHES `word`, where
//                    `word` may contain the wildcard '.' matching any single
//                    letter. Non-wildcard characters must match exactly.
// All strings consist of lowercase English letters (and '.' in search only).
//
// Example:
//   addWord("bad"); addWord("dad"); addWord("mad");
//   search("pad") -> false
//   search("bad") -> true
//   search(".ad") -> true   ('.' matches 'b', 'd', or 'm')
//   search("b..") -> true
//
// APPROACH — Trie + branching wildcard descent
// ---------------------------------------------
// addWord is the vanilla trie insert from ../code.cpp: walk/create one node
// per character, set isWord at the end. The interesting part is search.
//
// Without wildcards, search is a single-path walk — one child slot to check
// per character. A '.' breaks that: it can match ANY of the node's existing
// children, so the walk must BRANCH and try every live child, backtracking
// if none of the branches pans out. That makes search a recursive DFS over
// (trie node, string position) pairs instead of a loop:
//
//   match(node, i):
//     - if i == word.size(): return node->isWord        (same flag rule as LC 208)
//     - c = word[i]
//       * c != '.': follow children[c-'a'] or fail      (single path)
//       * c == '.': for EVERY non-null child, recurse; true if any succeeds
//
// Why this works: the trie collapses all words sharing prefixes into shared
// paths, so a leading ".a." explores at most (branching factor)^(number of
// dots) paths — not once per stored word. Words with no matching first
// letter are never even visited. The wildcard cost is paid only where '.'
// actually appears, and each '.' multiplies by at most 26 (in practice far
// less, since most nodes have few occupied slots).
//
// COMPLEXITY
// ----------
// Time:  addWord O(L). search O(L) when there are no dots; worst case
//        O(26^D · L) where D = number of dots, though real tries prune this
//        hard because only EXISTING children are explored.
// Space: O(total characters across all added words) for the trie; recursion
//        depth at most L + 1.
// ============================================================================

#include <array>
#include <iostream>
#include <string>

class WordDictionary {
 public:
  WordDictionary() : root_(new Node()) {}

  ~WordDictionary() { destroy(root_); }

  void addWord(const std::string& word) {
    Node* cur = root_;
    for (char c : word) {
      int idx = c - 'a';
      if (!cur->children[idx]) cur->children[idx] = new Node();
      cur = cur->children[idx];
    }
    cur->isWord = true;
  }

  bool search(const std::string& word) const { return match(root_, word, 0); }

 private:
  struct Node {
    std::array<Node*, 26> children{};
    bool isWord = false;
  };

  Node* root_;

  // Can any stored word match word[i..] starting from `node`?
  // Recursive because '.' forces branching; plain characters stay linear.
  bool match(const Node* node, const std::string& word, size_t i) const {
    if (i == word.size()) return node->isWord;  // consumed all chars: same
                                                // end-of-word rule as LC 208

    char c = word[i];
    if (c == '.') {
      // Wildcard: try every EXISTING child. Only branches whose next
      // character was actually inserted are explored — empty slots are
      // skipped for free, which is where the pruning comes from.
      for (const Node* child : node->children) {
        if (child && match(child, word, i + 1)) return true;
      }
      return false;  // no branch matched the remainder
    }

    const Node* next = node->children[c - 'a'];
    if (!next) return false;  // exact character missing: path breaks
    return match(next, word, i + 1);
  }

  void destroy(Node* node) {
    if (!node) return;
    for (Node* child : node->children) destroy(child);
    delete node;
  }
};

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

  {
    // Canonical LeetCode sequence.
    WordDictionary wd;
    wd.addWord("bad");
    wd.addWord("dad");
    wd.addWord("mad");
    check(wd.search("pad") == false, "search(\"pad\") -> false");
    check(wd.search("bad") == true, "search(\"bad\") -> true");
    check(wd.search(".ad") == true, "search(\".ad\") -> true (wildcard)");
    check(wd.search("b..") == true, "search(\"b..\") -> true (trailing wildcards)");
  }

  {
    // Edge: wildcard count mismatch — pattern length must equal word length.
    WordDictionary wd;
    wd.addWord("bad");
    check(wd.search("..") == false, "search(\"..\") -> false (shorter than \"bad\")");
    check(wd.search("....") == false, "search(\"....\") -> false (longer than \"bad\")");
    check(wd.search("...") == true, "search(\"...\") -> true (all-wildcard matches)");
  }

  {
    // Edge: all-wildcard query against an EMPTY dictionary must be false —
    // the branching loop finds zero live children at the root.
    WordDictionary wd;
    check(wd.search(".") == false, "search(\".\") on empty dict -> false");
    check(wd.search("") == false, "search(\"\") on empty dict -> false");
  }

  {
    // Edge: prefix-of-word distinction survives wildcards ("ba" vs "bad").
    WordDictionary wd;
    wd.addWord("bad");
    check(wd.search("ba.") == true, "search(\"ba.\") -> true");
    check(wd.search("ba") == false, "search(\"ba\") -> false (prefix only, no dot to extend)");
    check(wd.search(".a") == false, "search(\".a\") -> false (length mismatch)");
  }

  {
    // Edge: multiple words sharing a spine; wildcard picks the right branch.
    WordDictionary wd;
    wd.addWord("cat");
    wd.addWord("car");
    wd.addWord("dog");
    check(wd.search("ca.") == true, "search(\"ca.\") -> true (matches cat and car)");
    check(wd.search("c.t") == true, "search(\"c.t\") -> true");
    check(wd.search("do.") == true, "search(\"do.\") -> true");
    check(wd.search("dot") == false, "search(\"dot\") -> false ('t' never added under do-)");
    check(wd.search("d.g") == true, "search(\"d.g\") -> true");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
