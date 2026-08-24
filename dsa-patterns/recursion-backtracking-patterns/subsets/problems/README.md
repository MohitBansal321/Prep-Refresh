# Subsets — Worked Problems

Four fully worked, standalone C++17 solutions demonstrating the include/exclude and choose/recurse/undo skeleton across the subsets/combinations/permutations family. Each file is self-contained: compile and run it directly to see printed `[PASS]`/`[FAIL]` output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-subsets.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Subsets | [78](https://leetcode.com/problems/subsets/) | Medium | The pattern in its purest form — no duplicates, nothing to prune, only enumeration. | O(2ⁿ · n) time and space | [01-subsets.cpp](01-subsets.cpp) |
| Subsets II | [90](https://leetcode.com/problems/subsets-ii/) | Medium | Same skeleton as 01, plus one rule: sort first, then skip same-level duplicates. | O(2ⁿ · n) worst case, smaller with duplicates | [02-subsets-ii.cpp](02-subsets-ii.cpp) |
| Permutations | [46](https://leetcode.com/problems/permutations/) | Medium | The same choose/recurse/undo skeleton, generalized to when order matters (a `used` marker instead of an index cursor). | O(n! · n) time and space | [03-permutations.cpp](03-permutations.cpp) |
| Combination Sum | [39](https://leetcode.com/problems/combination-sum/) | Medium | Extends choose/recurse/undo with target-sum pruning and *unbounded reuse* of each candidate — the first taste of what Backtracking adds on top of this module. | Exponential, pruned by the remaining-target check | [04-combination-sum.cpp](04-combination-sum.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the pattern in its purest form: no duplicates, every subset valid, nothing to check or prune — only enumeration.
- **02** shows the entire problem reduces to one added rule: sort, then skip a duplicate value at the same recursion depth as its sibling.
- **03** generalizes the same skeleton to permutations, where a `used` marker replaces the include/exclude branch, and shows how much faster the output grows than subsets (10! vs 2^10).
- **04** is the bridge into [Backtracking](../../backtracking/): the same choose/recurse/undo shape, now with a real pruning condition (remaining target) and unbounded reuse of each candidate.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
