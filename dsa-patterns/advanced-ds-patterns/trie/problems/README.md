# Trie — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating the Trie across its main variants: the vanilla dictionary triple, wildcard-branching search, prefix-chain construction, and the bitwise alphabet swap. Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-implement-trie.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Implement Trie (Prefix Tree) | [208](https://leetcode.com/problems/implement-trie-prefix-tree/) | Medium | Vanilla 26-slot Trie: walk/create one node per char on insert; search demands the `isWord` flag, `startsWith` accepts mere path existence. | O(L) per op time, O(total chars) space | [01-implement-trie.cpp](01-implement-trie.cpp) |
| Design Add and Search Words Data Structure | [211](https://leetcode.com/problems/design-add-and-search-words-data-structure/) | Medium | Same Trie, but search branches recursively at each `'.'` wildcard, trying every existing child. | O(L) add / up to O(26^L) wildcard-search worst case, O(total chars) space | [02-design-add-and-search-words-data-structure.cpp](02-design-add-and-search-words-data-structure.cpp) |
| Longest Word in Dictionary | [720](https://leetcode.com/problems/longest-word-in-dictionary/) | Medium | Build one Trie from all words, then DFS from the root descending only through nodes flagged as complete words; first-found longest wins lexicographic ties. | O(total chars) time, O(total chars) space | [03-longest-word-in-dictionary.cpp](03-longest-word-in-dictionary.cpp) |
| Maximum XOR of Two Numbers in an Array | [421](https://leetcode.com/problems/maximum-xor-of-two-numbers-in-an-array/) | Medium | Bitwise Trie over 31 binary digits; for each number greedily descend toward the opposite bit at every position to maximize XOR. | O(n · 31) time, O(n · 31) trie nodes | [04-maximum-xor-of-two-numbers-in-an-array.cpp](04-maximum-xor-of-two-numbers-in-an-array.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the pattern in its purest form — the exact template from [code.cpp](../code.cpp), including the `isWord` distinction that separates `search` from `startsWith`.
- **02** shows where the single-path walk must become branching: a wildcard forces the walker to try every live child, turning linear descent into bounded backtracking.
- **03** shows the Trie composed with a graph traversal to answer a global question ("longest buildable word"), exercising the invariant that intermediate nodes along a valid answer must themselves be flagged words.
- **04** proves the pattern generalizes beyond letters: swap the 26-slot array for two slots keyed by bits, and the same walk-one-character-at-a-time skeleton solves a problem that looks like pure bit manipulation.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
