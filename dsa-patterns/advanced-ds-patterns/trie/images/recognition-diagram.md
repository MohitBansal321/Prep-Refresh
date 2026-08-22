# Trie — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether a Trie is the right tool, or whether the problem actually wants a hash set, sorting + binary search, or something else entirely.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Do the queries involve<br/>PREFIXES of strings —<br/>'anything starting with X',<br/>autocomplete, shared prefixes?}

    Q1 -- "No, only whole-string<br/>exact-match lookups" --> Q1b{How many lookups,<br/>how big is the dictionary?}
    Q1b -- "One-off or tiny input" --> LinearScan[["Just scan / sort directly<br/>no structure needed"]]
    Q1b -- "Many lookups, large set" --> HashSet[["Use a Hash Set<br/>O(1) average exact-match,<br/>simpler and lighter than a Trie"]]

    Q1 -- Yes, prefix queries --> Q2{Is the dictionary fixed<br/>and queried repeatedly?}
    Q2 -- "Queried once or twice" --> SortScan[["Sort the word list once<br/>+ binary search / linear check<br/>cheaper than building a tree"]]
    Q2 -- "Yes, repeated queries" --> Q3{What shape is the query?}

    Q3 -- "'Does any word start with P?' /<br/>autocomplete / longest common prefix" --> Trie[["Use a Trie<br/>insert: walk + create one node per char,<br/>set isWord; search/prefix: walk the path.<br/>O(L) per op, independent of dictionary size"]]

    Q3 -- "Wildcard '.' matching any char" --> WildTrie["Trie with BRANCHING search:<br/>at '.', try every existing child<br/>(recursive backtracking)"]

    Q3 -- "Per-prefix aggregation:<br/>sum/count over all keys with prefix P" --> SumTrie["Trie storing values at terminals<br/>or partial sums along paths"]

    Q3 -- "Bit-level prefixes:<br/>max XOR of pairs" --> BitTrie["Bitwise Trie (child slots 0/1)<br/>greedy: at each bit prefer the<br/>opposite bit to maximize XOR"]

    Trie --> Done([Trie applies])
    WildTrie --> Done
    SumTrie --> Done
    BitTrie --> Done
```

## How to read it

Start at the top and answer each diamond honestly before moving on. The **first fork** is the entire decision in miniature: does the word *prefix* appear in the problem's own vocabulary ("starts with", "common prefix", "complete this word")? If queries only ever name whole strings, a hash set wins on simplicity and memory, and no amount of cleverness makes a Trie better at exact match — that is the right-hand exit.

The **second fork** guards against the other classic mistake: building a Trie for data you will query once or twice. A Trie pays an up-front construction cost of O(total characters); if there is no amortization across many queries, sorting once and binary searching (see [Modified Binary Search](../../../searching-sorting-patterns/modified-binary-search/)) is cheaper and simpler.

The **third fork** picks the Trie *variant*. The vanilla insert/search/prefix triple from this module's [README](../README.md) covers autocomplete-style queries; wildcards force the single-path walk to branch recursively ([problems/02](../problems/02-design-add-and-search-words-data-structure.cpp)); per-prefix aggregation stores values or running sums in nodes; and bit-level problems reuse the identical skeleton with two child slots instead of 26 ([problems/04](../problems/04-maximum-xor-of-two-numbers-in-an-array.cpp)). Recognizing that all four are "the same pattern with different alphabets" is the real skill this diagram is trying to train.
