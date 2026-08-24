# Trie (Prefix Tree)

> **5-min refresher instead?** [cheatsheet.md](cheatsheet.md) has the one-table summary and recall questions.

## Intent

Store strings character-by-character in a tree so prefix search, autocomplete, and dictionary lookups run in `O(length)`, independent of how many words are in the dictionary.

## Recognition Signal

The problem is about prefixes — autocomplete, "does any word start with X", or searching a large fixed dictionary of strings repeatedly.

## Core Idea

A Trie is a tree where each node represents one character, and a path from the root spells out a prefix. Each node holds a fixed-size (or map-based) array of child pointers — one slot per possible next character — plus a boolean flag marking "a complete word ends here." Inserting a word walks/creates one node per character; searching for a word or a prefix walks the same path, checking existence at each step. Because the path length is bounded by the string's own length, every operation is `O(length)`, regardless of how many other words share the structure — shared prefixes are stored exactly once, which is both the space saving and the reason prefix queries are so cheap.

## Template

```cpp
class Trie {
  struct Node {
    std::array<Node*, 26> children{};  // nullptr = no child for that letter
    bool isWord = false;
  };
  Node* root = new Node();

 public:
  void insert(const std::string& word) {
    Node* cur = root;
    for (char c : word) {
      int idx = c - 'a';
      if (!cur->children[idx]) cur->children[idx] = new Node();
      cur = cur->children[idx];
    }
    cur->isWord = true;
  }

  bool search(const std::string& word) {
    Node* node = findNode(word);
    return node != nullptr && node->isWord;
  }

  bool startsWith(const std::string& prefix) {
    return findNode(prefix) != nullptr;
  }

 private:
  Node* findNode(const std::string& s) {
    Node* cur = root;
    for (char c : s) {
      int idx = c - 'a';
      if (!cur->children[idx]) return nullptr;
      cur = cur->children[idx];
    }
    return cur;
  }
};
```

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart for deciding whether a problem is a Trie fit.
- [images/flow-diagram.md](images/flow-diagram.md) — control flow of insert/search/prefix-check.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of inserting `"apple"` then `"app"`, showing exactly which nodes get shared vs. newly allocated, and why `search("appl")` returns `false` even though that node exists.

## Complexity

**Time:** `O(length)` per insert/search/prefix-check, where `length` is the string's length — independent of how many words are stored.
**Space:** `O(total characters across all inserted words)` in the worst case (no shared prefixes); much less when words share prefixes, since shared prefixes are stored only once.

## Common Mistakes

- **Forgetting the `isWord` flag**, and instead treating "the path exists" as "the word exists" — this incorrectly reports a word as present when it's actually just a prefix of some other inserted word.
- **Not freeing nodes** in a long-lived C++ Trie (or not caring, in a short-lived program) — every insert can allocate up to `length` new nodes.
- **Using a fixed 26-slot array when the alphabet includes more than lowercase letters** — needs a map-based child structure instead, at some space/speed cost.

## When To Use

- Prefix search, autocomplete, "does any word start with X", or repeated searching against a large, fixed dictionary of strings.

## When NOT To Use

- **You only need exact-match lookups, no prefix queries** — a hash set is simpler and just as fast for that narrower case.
- **The dictionary is tiny or queried only once** — the upfront cost of building the tree isn't worth it for a one-off linear scan.

## Similar Patterns

- **Modified Binary Search** ([../../searching-sorting-patterns/modified-binary-search/](../../searching-sorting-patterns/modified-binary-search/)): also good for prefix-adjacent queries (`lower_bound` on a sorted word list), but requires the dictionary to be pre-sorted and doesn't share structure across words the way a Trie does.
- **Hashing**: faster for pure exact-match lookups, but has no concept of "prefix" at all.

## Further Reading

- LeetCode — Implement Trie (Prefix Tree) (208), Word Search II (212), Design Add and Search Words Data Structure (211).
- *Introduction to Algorithms* (CLRS) — general tree-based string structures.
