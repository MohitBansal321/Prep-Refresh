# Backtracking — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "partial solution + constraint that can invalidate it early" signal that means "reach for backtracking, not plain Subsets enumeration," and (2) correctly stating, for every problem, exactly what gets undone and why forgetting the undo would corrupt sibling branches.

> Rule of thumb for every exercise: before writing a single line, ask "what is my partial-solution state?" and "what check tells me this partial state can never lead to a valid answer?" If you cannot answer both questions precisely, you are not ready to write the choose/recurse/undo loop yet.

---

## Easy — Letter Case Permutation

**LeetCode 784 — Letter Case Permutation.**

Given a string `s`, form a set of strings by transforming each letter individually to be lowercase or uppercase (digits are left unchanged), and return all possible strings.

**Constraints to notice:** every character position has exactly two choices if it is a letter (lower/upper) and exactly one choice if it is a digit (leave it alone) — there is no constraint that ever rules out a choice, every leaf of the recursion tree is valid output.

**Task:** implement it with the choose/recurse/undo skeleton anyway (build the string character by character, recurse, then pop the last character before trying the next case).

**Think about:** given that nothing ever gets pruned here, is this actually a Backtracking problem, or a Subsets-shaped problem wearing backtracking's skeleton? Justify your answer using the Recognition Diagram in [images/recognition-diagram.md](images/recognition-diagram.md).

---

## Medium — Combination Sum

**LeetCode 39 — Combination Sum.**

Given an array of distinct positive integers `candidates` and a target integer `target`, return all unique combinations of `candidates` where the chosen numbers sum to `target`. The same number may be chosen from `candidates` an unlimited number of times.

**Constraints to notice:** unlike N-Queens or Sudoku, the same "value" can be reused within one partial solution — the constraint that prunes here is purely numeric (running sum so far, compared against `target`), not a positional conflict.

**Task:** solve it with backtracking: at each step, try each candidate `>=` the previously chosen candidate (to avoid generating the same combination in a different order), add it to the running sum, recurse, then undo (remove it and subtract it back out) before trying the next candidate. Prune immediately once the running sum exceeds `target`.

**Think about:** why does restricting each step to candidates `>=` the previous choice prevent duplicate combinations like `[2,2,3]` and `[2,3,2]` from both appearing, without needing a separate deduplication pass afterward?

---

## Hard — Word Search II

**LeetCode 212 — Word Search II.**

Given an `m x n` board of characters and a list of strings `words`, return all words on the board that can be constructed from letters of sequentially adjacent cells (same adjacency rule as [problems/03-word-search.cpp](problems/03-word-search.cpp)), using each cell at most once per word.

**Constraints to notice:** running the single-word Word Search backtracking search once per word in `words` works, but re-scans the entire board from scratch for every word — wasteful when `words` is long and many words share common prefixes.

**Task:** first implement the naive version (call a Word-Search-style backtracking search once per word). Then improve it by building a **Trie** (prefix tree) of all words up front, and running a *single* backtracking DFS over the board that walks the Trie alongside the grid path, pruning a branch the instant no word in `words` shares the prefix traced so far — rather than pruning only when a complete mismatched word is reached.

**Think about:** the Trie turns "does any word start with this prefix?" into an O(1)-per-character check. Explain, in one or two sentences, why that check is itself a form of the same "is this partial state still valid?" constraint check central to Backtracking — just checked against a different structure (a Trie) instead of a board's rows/columns/boxes.

---

## Real-world Challenge — Feature-Flag Configuration Validator

You work on a backend service with a growing set of feature flags. Product has laid out two kinds of rules:

1. **Mutual exclusion:** some pairs of flags can never both be enabled at once (e.g. `"new-checkout-flow"` and `"legacy-checkout-flow"`).
2. **Prerequisites:** some flags require another flag to already be enabled (e.g. `"checkout-express-pay"` requires `"new-checkout-flow"`).

Given a list of candidate flags, a list of mutual-exclusion pairs, and a list of prerequisite pairs, you need to enumerate **every valid, maximal-or-not configuration** (every subset of flags that violates neither rule) so QA can test each one before a release.

**Task:**
1. Represent the input as `std::vector<std::string> flags`, `std::vector<std::pair<std::string,std::string>> mutuallyExclusive`, and `std::vector<std::pair<std::string,std::string>> prerequisites`.
2. Implement a backtracking search that decides, one flag at a time, whether to enable or skip it — the same choose/recurse/undo skeleton as every other problem in this module — pruning the "enable" branch immediately whenever enabling this flag would violate a mutual-exclusion rule against an already-enabled flag, or a prerequisite rule against a not-yet-enabled flag.
3. Return every valid configuration found (as a `std::vector<std::vector<std::string>>` of enabled-flag sets).

**Then discuss:** this is exactly the "configuration validator that must satisfy mutual-exclusion rules" scenario named in the README's "Real Interview/Production Examples" section. In a real system with hundreds of flags, exhaustively enumerating every valid configuration is intractable — what would you change about the requirements (or the algorithm's goal) to make this tractable in production? (Hint: do you really need *every* valid configuration, or would validating *one specific* proposed configuration, or finding *one* maximal configuration, be enough?)

---

## Bonus Challenge — Restore IP Addresses

**LeetCode 93 — Restore IP Addresses.**

Given a string `s` containing only digits, return all possible valid IP address combinations that can be formed by inserting three dots into `s` (each of the 4 resulting segments must be between 0 and 255, and cannot have a leading zero unless the segment is exactly `"0"`).

**Task:** adapt the same choose/recurse/undo shape used in [problems/04-palindrome-partitioning.cpp](problems/04-palindrome-partitioning.cpp) — instead of checking "is this piece a palindrome," check "is this piece a valid 0-255 segment with no leading zero," and instead of an unbounded number of pieces, stop as soon as you have chosen exactly 4 segments (pruning immediately if you run out of string before reaching 4, or reach 4 segments before consuming the whole string).

**Then, generalize in writing (no code required):** Palindrome Partitioning's constraint check looks only at the current candidate substring; this problem's constraint check looks at the substring **and** a running count of segments chosen so far (you must prune once you already have 4 segments but string remains, even if the next piece would otherwise be valid). Explain, using the Architecture section of the [README](README.md), why adding "a count of choices made so far" to the partial-solution state does not change the fundamental choose/recurse/undo shape — only what the constraint check is allowed to look at.

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
