// ============================================================================
// LeetCode 720 — Longest Word in Dictionary
// ============================================================================
//
// PROBLEM
// -------
// Given an array of strings `words`, return the longest word that can be
// built one character at a time by other words in `words` — i.e. every
// prefix of it (starting from the single first character) is itself a word
// in `words`. If multiple answers exist, return the lexicographically
// smallest; if none exists, return "".
//
// Example:
//   words = ["w","wo","wor","worl","world"]        -> "world"
//   words = ["a","banana","app","appl","ap","apply","apple"] -> "apple"
//     ("apply" ties "apple" at length 5, but "apple" < "apply")
//
// APPROACH — build one Trie, then DFS only through complete-word nodes
// -------------------------------------------------------------
// The condition "every prefix is a word" has a direct structural meaning in
// a trie: a word W is buildable iff EVERY node along W's path from the root
// carries isWord = true. So:
//
//   1. Insert all words into one trie (O(total chars)).
//   2. DFS from the root, but only descend into a child if the CURRENT node
//      is flagged as a word (the root, depth 0, is exempt). Any branch whose
//      intermediate node lacks the flag cannot lead to a buildable word,
//      because that intermediate node IS some word's missing prefix.
//
// Why this beats checking each word against a hash set of prefixes: the DFS
// shares work across words with common prefixes exactly once. Once the walk
// has established that "wo", "wor", "worl" are all words, extending to
// "world" costs one more step — no per-word re-verification of its whole
// prefix chain.
//
// Lexicographic tie-breaking comes for free: children are visited in 'a'..'z'
// order, so among equal-length candidates the FIRST one reached is the
// lexicographically smallest, and the strict `depth > best.size()` update
// never replaces it with an equally long later candidate.
//
// COMPLEXITY
// ----------
// Time:  O(total characters) to build + O(nodes) for the DFS — each trie
//        node is visited at most once.
// Space: O(total characters) for the trie + O(max word length) recursion
//        depth and current-path buffer.
// ============================================================================

#include <array>
#include <iostream>
#include <string>
#include <vector>

struct Node {
  std::array<Node*, 26> children{};  // value-init: all nullptr
  bool isWord = false;
};

class Trie {
 public:
  Trie() : root_(new Node()) {}

  ~Trie() { destroy(root_); }

  void insert(const std::string& word) {
    Node* cur = root_;
    for (char c : word) {
      int idx = c - 'a';
      if (!cur->children[idx]) cur->children[idx] = new Node();
      cur = cur->children[idx];
    }
    cur->isWord = true;
  }

  // Depth-first search restricted to nodes on all-word paths.
  // `current` mirrors the path from root to `node`; `best` holds the
  // longest buildable word found so far.
  void dfs(const Node* node, int depth, std::string& current,
           std::string& best) const {
    // Prune: past the root, every node on a valid answer's path must be a
    // complete word itself. If not, nothing below can be buildable either.
    if (depth > 0 && !node->isWord) return;

    // Strict '>' keeps the FIRST (lexicographically smallest) word of any
    // given length, since children are explored in alphabetical order.
    if (depth > static_cast<int>(best.size())) best = current;

    for (int i = 0; i < 26; ++i) {
      const Node* child = node->children[i];
      if (!child) continue;
      current.push_back(static_cast<char>('a' + i));
      dfs(child, depth + 1, current, best);
      current.pop_back();  // backtrack: restore shared buffer
    }
  }

  const Node* getRoot() const { return root_; }

 private:
  Node* root_;

  void destroy(Node* node) {
    if (!node) return;
    for (Node* child : node->children) destroy(child);
    delete node;
  }
};

std::string longestWord(const std::vector<std::string>& words) {
  Trie trie;
  for (const std::string& w : words) trie.insert(w);

  std::string current;  // reused path buffer (push/pop, no reallocation churn)
  std::string best;     // starts empty: correct answer when nothing builds
  trie.dfs(trie.getRoot(), 0, current, best);
  return best;
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

  {
    // Canonical case: full unbroken chain down to "world".
    std::vector<std::string> words = {"w", "wo", "wor", "worl", "world"};
    check(longestWord(words) == "world",
          "[w,wo,wor,worl,world] -> \"world\"");
  }

  {
    // Tie-break: "apple" vs "apply" both length 5 -> lexicographically
    // smaller "apple" must win.
    std::vector<std::string> words = {"a", "banana", "app", "appl",
                                      "ap", "apply", "apple"};
    check(longestWord(words) == "apple",
          "tie between apple/apply -> \"apple\"");
    // Edge: "banana" is longer than everything else but NOT buildable —
    // its prefixes are absent, so the DFS prunes that branch entirely.
    check(longestWord(words) != "banana", "\"banana\" correctly rejected");
  }

  {
    // Edge: no single-character seeds -> no word is buildable -> "".
    std::vector<std::string> words = {"ab", "bc"};
    check(longestWord(words) == "", "[ab,bc] (no 1-char words) -> \"\"");
  }

  {
    // Edge: chain broken mid-way — "breakf" exists but "break" does not,
    // so neither "breakf" nor anything under it qualifies.
    std::vector<std::string> words = {"b", "br", "bre", "brea", "breakf"};
    check(longestWord(words) == "brea",
          "chain broken before \"break\" -> \"brea\"");
  }

  {
    // Edge: single word, trivially its own answer.
    std::vector<std::string> words = {"a"};
    check(longestWord(words) == "a", "single word [a] -> \"a\"");
  }

  {
    // Edge: duplicate words must not break anything.
    std::vector<std::string> words = {"t", "t", "ti", "tig", "tige", "tiger"};
    check(longestWord(words) == "tiger", "duplicates tolerated -> \"tiger\"");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
