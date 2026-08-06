# Union Find — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Union Find across increasingly demanding shapes: pure component counting, cycle detection while processing edges in order, merging records keyed by a shared attribute (not a direct edge list), and fully online/incremental connectivity as data streams in one cell at a time. Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-number-of-provinces.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Number of Provinces | [547](https://leetcode.com/problems/number-of-provinces/) | Medium | Union every adjacency-matrix edge; the DisjointSet's live component count IS the answer. | O(n² · α(n)) time, O(n) space | [01-number-of-provinces.cpp](01-number-of-provinces.cpp) |
| Redundant Connection | [684](https://leetcode.com/problems/redundant-connection/) | Medium | Process edges in order; the first edge whose `unionSets` returns false (already connected) is the redundant one. | O(E · α(V)) time, O(V) space | [02-redundant-connection.cpp](02-redundant-connection.cpp) |
| Accounts Merge | [721](https://leetcode.com/problems/accounts-merge/) | Hard | Union account *indices* keyed by shared emails, then group emails by root account. | O(n·k · log(n·k)) time, O(n·k) space | [03-accounts-merge.cpp](03-accounts-merge.cpp) |
| Number of Islands II | [305](https://leetcode.com/problems/number-of-islands-ii/) | Hard | Flatten grid to 1D indices; add land incrementally, union with already-land neighbors, track a running island count. | O(k · α(m·n)) time, O(m·n) space | [04-number-of-islands-ii.cpp](04-number-of-islands-ii.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):

- **01** is the purest form of "count connected components from a full edge list" — the DisjointSet's `componentCount()` doing all the work, no extra bookkeeping needed.
- **02** is the canonical "process edges incrementally, detect the first one that closes a cycle" problem — the single clearest illustration of what `unionSets()` returning `false` actually means.
- **03** shows Union Find applied where the elements being unioned (account indices) are *not* the same thing the "edges" are keyed by (emails) — a mapping step most Union Find tutorials skip, but that shows up constantly in real problems (records merged by a shared key rather than an explicit adjacency list).
- **04** is the fully **online** case this whole pattern exists for: connectivity queries interleaved with incremental updates, one cell at a time, with no re-traversal of the grid ever allowed.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
