# Trie — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the prefix/dictionary signal that means "reach for a Trie," and (2) knowing when the single-path walk of `insert`/`search` must branch — wildcards, multiple continuations, or a grid — because that is where nearly every interesting Trie problem lives.

> Rule of thumb for every exercise: before writing a single line, ask "does the query involve a *prefix* of strings, or only whole strings?" If only whole strings, stop and consider whether a hash set solves it more simply. If a prefix is involved, ask next: "is the walk a single path, or must it branch at some characters?"

---

## Easy — Longest Common Prefix

**LeetCode 14 — Longest Common Prefix.**

Given an array of strings, find the longest common prefix string shared by all of them. If there is none, return `""`.

**Constraints to notice:** the array can contain as few as **one** string (the whole string is then its own common prefix), and it can contain the empty string (which forces the answer to `""`). Vertical scanning — compare character column by column — is already effectively a Trie walk; notice that.

**Task:** solve it two ways: (1) the direct vertical-scan, then (2) by actually building a Trie of all the words and deriving the common prefix from the tree shape. For approach (2), think about what property a node must have to be part of the common prefix — a useful hint: a node is "on the shared spine" only while it has **exactly one** child and no word has ended above it.

**Think about:** which definition of "common prefix" does the Trie give you for free that the vertical scan has to re-derive manually? What does the root's fan-out tell you immediately?

---

## Medium — Replace Words

**LeetCode 648 — Replace Words.**

You are given a dictionary of "roots" (e.g. `["cat", "bat", "rat"]`) and a sentence. Replace every word in the sentence with the **shortest root that is a prefix of it** (e.g. "cattle" becomes "cat"); if no root matches, keep the word unchanged.

**Constraints to notice:** among candidate roots, the *shortest* prefix wins — this defines a precise stopping condition during the walk. Words and roots consist of lowercase letters; the sentence is space-separated.

**Task:** insert all roots into a Trie, then for each sentence word walk down character by character, stopping at the first node flagged `isWord` (that is automatically the shortest matching root). If the walk dies before hitting any flag, emit the original word.

**Think about:** why does "stop at the *first* `isWord` encountered on the way down" guarantee the shortest root, given that shorter roots are always ancestors of longer ones sharing their prefix?

---

## Hard — Word Search II

**LeetCode 212 — Word Search II.**

Given an `m x n` grid of letters and a list of dictionary words, find all words that can be formed by sequences of adjacent cells (up/down/left/right, no cell reused within a word).

**Task:** brute-forcing each word independently against the grid costs roughly O(words × m × n × 3^L). Instead, insert all dictionary words into one Trie and run DFS from every grid cell simultaneously with the Trie walker — pruning any branch whose next character has no Trie child. Mark found words (and consider unmarking them once found so duplicates don't rescan).

**Hint toward the invariant, not the solution:** the grid DFS carries a Trie node pointer alongside the board position; backtracking in the grid corresponds exactly to walking back up the Trie, so no extra state is needed beyond the recursion stack.

**Then answer:** why does the Trie turn the complexity from "per word" into "shared across words with common prefixes," and which part of the Trie structure delivers that saving during a grid search specifically?

---

## Real-World Challenge — Typeahead/Autocomplete Service

You are designing the suggestion box of a search bar: as the user types each character, the UI must show the top-k most-searched completions of the current query prefix, updated live. Queries arrive continuously; popularity counts change over time (today's hot topic should outrank last month's).

**Task:**
1. Design (and implement, with a `std::vector<std::pair<std::string, long long>>` standing in for the click log) a Trie where each terminal node stores a popularity counter, plus a method `suggest(prefix, k)` returning the k highest-count words under that prefix.
2. Decide where the top-k computation happens: precompute a per-node top-k list on every count update, or collect all subtree words and heap-select at query time. Analyze the tradeoff under a read-heavy vs. write-heavy workload.
3. Discuss: your service runs on many machines and counts must be global, but a Trie is an in-memory local structure. What do you shard by, and what happens to prefix queries that cross a shard boundary?

---

## Bonus Challenge — Map Sum Pairs

**LeetCode 677 — Map Sum Pairs.**

Implement a structure supporting `insert(key, value)` (overwriting a previous value for the same key) and `sum(prefix)` returning the total of all values whose keys start with that prefix.

**Task:** two natural designs exist — store values at terminal nodes and sum by walking the entire subtree of the prefix node, or push partial sums *down* every node on insertion (updating all nodes along the key's path, with care for overwrite deltas). Implement both.

**Then compare in writing:** under `insert` costing O(L) and `sum` costing O(subtree size) vs. O(L) respectively, which design wins when inserts vastly outnumber sums? Which wins in the reverse workload? Justify using the Complexity section of the [README](README.md).

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
