# Trie (Prefix Tree) — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Advanced DS pattern — character-tree (prefix) data structure. |
| **Recognition Signal** | The problem talks about **prefixes**: autocomplete, "does any stored word start with X", word dictionaries queried repeatedly, or counting/aggregating over shared string prefixes. |
| **Problem** | Scanning a dictionary per query costs O(n · L); a hash set does exact-match lookups but has **no concept of prefix at all** — it cannot answer "any word starting with `ap`?" without scanning every key. |
| **Solution** | A tree where each edge consumes one character: insert walks/creates one node per character and sets an `isWord` flag on the final node; search and prefix-check walk the same path — search additionally demands `isWord`, prefix-check accepts mere existence. Shared prefixes are stored exactly once. |
| **Time / Space Complexity** | O(L) time per insert/search/prefix-check, where L is the string's own length — **independent of how many words are stored**. Space: O(total characters across all words), reduced proportionally by shared prefixes. |
| **Pros** | All operations scale with the *query string*, not the dictionary size · prefix queries are first-class (impossible with hashing) · shared prefixes stored once · naturally supports ordered traversal, wildcard search, and per-prefix aggregation · deterministic O(L) worst case (no hashing collisions). |
| **Cons** | Memory-hungry per node (up to 26 child pointers each) vs. a flat hash set · building the whole structure up front isn't worth it for one-off lookups · fixed-size child arrays assume a known alphabet · more code than `std::unordered_set<std::string>` for pure exact-match. |
| **Use When** | Autocomplete / typeahead · "does any word start with X" · repeated searches against a large, mostly-fixed dictionary · problems that decompose into walking strings character-by-character with branching (word builders, wildcard search, XOR-over-bits). |
| **Avoid When** | You only need exact-match lookups and never a prefix query (use a hash set) · the dictionary is tiny or queried once (linear scan) · memory is tight and the key space is huge/sparse (consider a compressed trie or map-based children). |
| **Related Patterns** | Hashing (exact-match champion, zero prefix support) · Modified Binary Search (`lower_bound` on a sorted word list — prefix-adjacent but no structural sharing) · Backtracking (composes with a Trie for grid word searches like Word Search II). |

### Template Skeleton

```cpp
class Trie {
  struct Node {
    std::array<Node*, 26> children{};  // nullptr = no child for that letter
    bool isWord = false;
  };
  Node* root = new Node();

  Node* findNode(const std::string& s) {
    Node* cur = root;
    for (char c : s) {
      int idx = c - 'a';
      if (!cur->children[idx]) return nullptr;   // path breaks -> not present
      cur = cur->children[idx];
    }
    return cur;
  }

 public:
  void insert(const std::string& word) {
    Node* cur = root;
    for (char c : word) {
      int idx = c - 'a';
      if (!cur->children[idx]) cur->children[idx] = new Node();
      cur = cur->children[idx];
    }
    cur->isWord = true;                          // mark end-of-word
  }

  bool search(const std::string& word) {
    Node* n = findNode(word);
    return n != nullptr && n->isWord;            // existence AND end flag
  }

  bool startsWith(const std::string& prefix) {
    return findNode(prefix) != nullptr;          // mere existence suffices
  }
};
```

### Remember In One Sentence
> **A Trie trades one node per character of memory for O(L) operations that depend only on the string's own length — making "anything starting with this prefix" a cheap walk down an existing path instead of a scan over every key.**

### Two Facts People Get Wrong
- "`search("app")` should be true because `"apple"` is inserted"? **No** — path existence is not word membership; without checking the `isWord` flag on the final node you report every prefix of every word as a full word.
- "A Trie beats a hash set at everything string-related"? **No** — for pure exact-match lookup a hash set is simpler, lighter on memory, and equally fast; the Trie only earns its keep when *prefix* structure itself is part of the query.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. What exactly distinguishes `search` from `startsWith`, both conceptually and in code?
2. Why is every operation O(L) regardless of how many words the Trie holds?
3. What does the `isWord` flag protect against, and what wrong answers appear if you omit it?
4. What is the space complexity of a Trie, and why do shared prefixes make it smaller than the worst-case bound suggests?
5. Name the two situations where a plain hash set is the better choice over a Trie.
6. How would you extend `search` to support a wildcard character (`.` matching any letter)? Where does the single-path walk have to become branching?
7. Why does a fixed 26-slot child array break down, and what replaces it when the alphabet grows?
8. What is the recognition signal — in one sentence — that means "reach for a Trie" rather than sorting or hashing?
9. How does a bitwise Trie (one child slot per 0/1 bit) solve Maximum XOR of Two Numbers, and what is the greedy rule at each bit?
10. Give one real production system context (not a LeetCode problem) where a Trie-shaped structure shows up.
