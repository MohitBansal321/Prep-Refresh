// ============================================================================
// LeetCode 208 — Implement Trie (Prefix Tree)
// ============================================================================
//
// PROBLEM
// -------
// Implement a Trie (prefix tree) supporting three operations:
//   - insert(word):    add `word` to the trie.
//   - search(word):    return true iff `word` was previously inserted
//                      (as a complete word, not merely as a prefix).
//   - startsWith(pfx): return true iff some inserted word starts with `pfx`.
// All strings consist of lowercase English letters 'a'-'z'.
//
// Example:
//   insert("apple")
//   search("apple")    -> true
//   search("app")      -> false   ("app" is only a prefix of "apple")
//   startsWith("app")  -> true
//   insert("app")
//   search("app")      -> true
//
// APPROACH — character-path tree with an end-of-word flag
// --------------------------------------------------------
// Each node owns a fixed array of 26 child pointers (one slot per possible
// next letter) plus a boolean `isWord`. A path from the root spells out the
// characters seen along it; shared prefixes are therefore stored exactly
// once, which is both the space saving and the reason prefix queries are so
// cheap (see ../README.md and ../images/recognition-diagram.md).
//
//   - insert walks the path, CREATING any missing child as it goes, then
//     sets isWord on the final node. Insert can never fail: absence of a
//     node just means "allocate one".
//   - search and startsWith walk the same path but treat a missing child as
//     immediate failure — the string cannot be present if its character
//     path was never built. They differ only at the end:
//       * startsWith accepts mere path existence,
//       * search additionally demands isWord on the final node.
//
// The isWord check is not optional polish: without it, search("app") would
// return true after only insert("apple"), because "app"'s path exists as a
// byproduct of storing the longer word. Path existence != word membership.
//
// COMPLEXITY
// ----------
// Time:  O(L) per operation, where L is the string's own length — every
//        step consumes exactly one character and does O(1) work (one array
//        index + one pointer dereference). Crucially independent of how many
//        words are stored; that is the property hashing cannot offer.
// Space: O(total characters across all inserted words) worst case; less in
//        practice because shared prefixes are stored once.
// ============================================================================

#include <array>
#include <iostream>
#include <string>

class Trie {
 public:
  Trie() : root_(new Node()) {}

  ~Trie() { destroy(root_); }

  void insert(const std::string& word) {
    Node* cur = root_;
    for (char c : word) {
      int idx = c - 'a';
      // Missing child? Create it — insert always succeeds.
      if (!cur->children[idx]) cur->children[idx] = new Node();
      cur = cur->children[idx];
    }
    // Mark that a complete word ends exactly here. Longer words passing
    // through this node do NOT set this flag; shorter ones do.
    cur->isWord = true;
  }

  bool search(const std::string& word) const {
    Node* node = findNode(word);
    // Two separate conditions: (1) the full character path exists,
    // (2) a complete word ends there — not just a longer word's prefix.
    return node != nullptr && node->isWord;
  }

  bool startsWith(const std::string& prefix) const {
    // Mere path existence is enough for a prefix query.
    return findNode(prefix) != nullptr;
  }

 private:
  struct Node {
    std::array<Node*, 26> children{};  // value-init: all nullptr
    bool isWord = false;
  };

  Node* root_;

  // Walks the character path for `s`. Returns the final node, or nullptr
  // if the path breaks at any point (string/prefix not present).
  Node* findNode(const std::string& s) const {
    Node* cur = root_;
    for (char c : s) {
      int idx = c - 'a';
      if (!cur->children[idx]) return nullptr;
      cur = cur->children[idx];
    }
    return cur;
  }

  // Recursive post-order teardown; every insert may allocate up to
  // `word.size()` nodes, so a long-lived trie must free them.
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
    // Canonical LeetCode sequence: prefix-of-word must NOT count as a word
    // until explicitly inserted.
    Trie trie;
    trie.insert("apple");
    check(trie.search("apple") == true, "search(\"apple\") after insert -> true");
    check(trie.search("app") == false, "search(\"app\") -> false (prefix only)");
    check(trie.startsWith("app") == true, "startsWith(\"app\") -> true");
    trie.insert("app");
    check(trie.search("app") == true, "search(\"app\") after explicit insert -> true");
  }

  {
    // Edge: empty string. It is a prefix of everything (matches root) but
    // is never a word unless explicitly inserted.
    Trie trie;
    check(trie.startsWith("") == true, "startsWith(\"\") -> true (empty prefix matches root)");
    check(trie.search("") == false, "search(\"\") -> false (never inserted)");
    trie.insert("");
    check(trie.search("") == true, "search(\"\") after inserting \"\" -> true");
  }

  {
    // Edge: single-character words sharing a first letter with longer words.
    Trie trie;
    trie.insert("a");
    trie.insert("ab");
    check(trie.search("a") == true, "search(\"a\") -> true");
    check(trie.search("ab") == true, "search(\"ab\") -> true");
    check(trie.search("abc") == false, "search(\"abc\") -> false (path breaks)");
    check(trie.startsWith("abc") == false, "startsWith(\"abc\") -> false");
  }

  {
    // Edge: unrelated branches coexist; queries on absent branches fail fast.
    Trie trie;
    trie.insert("banana");
    check(trie.search("apple") == false, "search(\"apple\") -> false (no such branch)");
    check(trie.startsWith("ban") == true, "startsWith(\"ban\") -> true");
    check(trie.startsWith("bananas") == false, "startsWith(\"bananas\") -> false (path ends early)");
  }

  {
    // Edge: many words sharing one long spine — the whole point of a trie.
    Trie trie;
    trie.insert("w");
    trie.insert("wo");
    trie.insert("wor");
    trie.insert("worl");
    trie.insert("world");
    check(trie.search("worl") == true, "search(\"worl\") -> true");
    check(trie.search("world") == true, "search(\"world\") -> true");
    check(trie.search("wor") == true, "search(\"wor\") -> true");
    check(trie.search("words") == false, "search(\"words\") -> false (extends past stored path)");
    check(trie.startsWith("worl") == true, "startsWith(\"worl\") -> true");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
