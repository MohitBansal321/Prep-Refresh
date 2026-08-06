// ============================================================================
// Trie (Prefix Tree) — generic reusable template (C++17)
// ============================================================================
//
// A classic Trie supporting insert, exact-word search, and prefix search,
// restricted to lowercase 'a'-'z' via a fixed 26-slot child array.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <array>
#include <iostream>
#include <string>
#include <vector>

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

  bool search(const std::string& word) const {
    Node* node = findNode(word);
    return node != nullptr && node->isWord;
  }

  bool startsWith(const std::string& prefix) const {
    return findNode(prefix) != nullptr;
  }

 private:
  struct Node {
    std::array<Node*, 26> children{};
    bool isWord = false;
  };

  Node* root_;

  Node* findNode(const std::string& s) const {
    Node* cur = root_;
    for (char c : s) {
      int idx = c - 'a';
      if (!cur->children[idx]) return nullptr;
      cur = cur->children[idx];
    }
    return cur;
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

  Trie trie;
  trie.insert("apple");

  check(trie.search("apple") == true, "search(\"apple\") after insert -> true");
  check(trie.search("app") == false, "search(\"app\") -> false (only a prefix, not inserted as a word)");
  check(trie.startsWith("app") == true, "startsWith(\"app\") -> true");

  trie.insert("app");
  check(trie.search("app") == true, "search(\"app\") after explicit insert -> true");

  check(trie.search("appl") == false, "search(\"appl\") -> false (never inserted)");
  check(trie.startsWith("appl") == true, "startsWith(\"appl\") -> true (prefix of \"apple\")");
  check(trie.startsWith("banana") == false, "startsWith(\"banana\") -> false (no such branch)");
  check(trie.search("") == false, "search(\"\") -> false (empty string never inserted)");
  check(trie.startsWith("") == true, "startsWith(\"\") -> true (empty prefix matches the root)");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
