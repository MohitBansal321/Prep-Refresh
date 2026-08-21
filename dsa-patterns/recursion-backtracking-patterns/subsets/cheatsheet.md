# Subsets — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Recursion/Backtracking family — exhaustive combinatorial enumeration with **no pruning**. |
| **Recognition Signal** | Problem says **"return all subsets / all combinations / all permutations"** of a *small* collection · every complete arrangement is valid output (nothing to reject) · `n` is small enough that `2^n` or `n!` fits in memory (roughly `n <= 20-25` for subsets, `n <= 10-12` for permutations). |
| **Problem** | You must produce *every* combination of independent yes/no (or ordering) choices — exactly once each, with no omissions and no duplicates, even when the input itself contains repeated values. |
| **Solution** | Three equivalent mechanics over the same `2^n`-leaf decision tree: **iterative doubling** (start `{{}}`, and per element append a copy of every existing subset with that element added) · **recursive include/exclude** (per index, recurse "skip" then "take" with push/recurse/pop, record a copy at `i == n`) · **bitmask enumeration** (for `mask` in `[0, 2^n)`, include `nums[i]` when bit `i` of `mask` is set). |
| **Time / Space Complexity** | `O(2^n · n)` time and `O(2^n · n)` output space for subsets — `2^n` subsets, up to `n` elements each to build/copy. `O(n! · n)` for permutations. Auxiliary space beyond the output is only `O(n)` (call stack + the shared `current` buffer). |
| **Pros** | One-sentence correctness proof — every element gets exactly one decision, so every combination appears exactly once · no pruning logic that could carry a subtle bug · optimal, since the cost *is* the output size · three independent implementations that cross-check each other · the recursive shape is the literal foundation Backtracking extends. |
| **Cons** | Exponential output caps usable `n` at ~20-25 (permutations far sooner) · cannot stop early — no partial answer is ever invalid, so there is nothing to abandon · duplicate input values silently over-generate unless explicitly guarded · the iterative version's growing-list loop bound is a real hang-the-program bug source · recursive version costs `O(n)` stack depth. |
| **Use When** | "List every subset/combination/permutation" of a small set · exhaustive configuration coverage (feature flags, toggles, small option sets) · a brute-force oracle to validate a cleverer algorithm against in tests · you are about to learn Backtracking and want the unpruned skeleton solid first. |
| **Avoid When** | `n` is large enough that `2^n`/`n!` is intractable — reformulate, do not optimize · the question is a yes/no, a count, or a best value *about* the subsets rather than the subsets themselves (use DP subset-sum) · a constraint can invalidate a **partial** answer early (use Backtracking) · order matters and you reached for subset logic, or vice versa. |
| **Related Patterns** | Backtracking (same recursion plus a validity check and an early prune — Subsets is Backtracking whose check always passes) · Dynamic Programming, subset-sum family (answers questions *about* the power set without materializing it) · bitmask enumeration (the same power set via integer bit tests instead of recursion). |

### Template Skeleton

```cpp
// (a) Recursive include/exclude — the shape Backtracking extends.
void subsetsHelper(const std::vector<int>& nums, size_t index,
                   std::vector<int>& current,                 // ONE shared buffer
                   std::vector<std::vector<int>>& result) {
    if (index == nums.size()) {      // every element decided -> one complete subset
        result.push_back(current);   // COPY, not a reference: current keeps mutating
        return;
    }
    subsetsHelper(nums, index + 1, current, result);  // EXCLUDE nums[index]

    current.push_back(nums[index]);                   // INCLUDE: choose
    subsetsHelper(nums, index + 1, current, result);  //          recurse
    current.pop_back();                               //          UNDO (backtrack)
}

// (b) Iterative doubling — no recursion; one pass per element.
std::vector<std::vector<int>> subsetsIterative(const std::vector<int>& nums) {
    std::vector<std::vector<int>> result = {{}};      // the empty subset counts!
    for (int num : nums) {
        size_t existing = result.size();               // SNAPSHOT before appending --
        for (size_t i = 0; i < existing; ++i) {        // re-reading result.size() here
            std::vector<int> extended = result[i];     // never terminates
            extended.push_back(num);
            result.push_back(std::move(extended));
        }
    }
    return result;                                     // exactly 2^n subsets
}

// (c) Bitmask enumeration — subset <-> integer in [0, 2^n).
for (unsigned mask = 0; mask < (1u << n); ++mask) {
    std::vector<int> subset;
    for (unsigned i = 0; i < n; ++i)
        if (mask & (1u << i)) subset.push_back(nums[i]);   // bit i set => take nums[i]
    result.push_back(subset);
}

// (d) DUPLICATE INPUTS: sort first, then the start-index loop shape, and skip a value
//     equal to its predecessor AT THE SAME LEVEL (i > start), not anywhere in current.
void subsetsWithDupes(const std::vector<int>& nums /* SORTED */, size_t start,
                      std::vector<int>& current,
                      std::vector<std::vector<int>>& result) {
    result.push_back(current);                        // every node is an answer here
    for (size_t i = start; i < nums.size(); ++i) {
        if (i > start && nums[i] == nums[i - 1]) continue;   // <-- the whole lesson
        current.push_back(nums[i]);
        subsetsWithDupes(nums, i + 1, current, result);
        current.pop_back();
    }
}
```

### Remember In One Sentence
> **Subsets enumerates every leaf of an `n`-level decision tree where each level decides one element and nothing is ever rejected — push/recurse/pop a single shared buffer and record a *copy* at the base case (or double a growing list per element, or read bits off `0..2^n-1`) — and the moment a *partial* answer can be judged invalid, you are no longer doing Subsets, you are doing Backtracking.**

### Two Facts People Get Wrong
- The exponential complexity means the algorithm is inefficient and a better one exists? **No** — `O(2^n · n)` is the size of the *required output*, so any correct algorithm pays it; this is output-bound, not a slow method. The right response to "n is too big" is to reformulate the problem (usually DP subset-sum, which answers questions *about* the subsets without listing them), never to look for a faster enumerator.
- Skipping duplicates means "don't reuse a value already in the current subset"? **No** — after sorting, it means skipping `nums[i]` when `i > start && nums[i] == nums[i-1]`: a duplicate at the **same recursion level** (a sibling branch), which is a different rule entirely. `[1,2,2]` must still produce `{2,2}`, so the same *value* absolutely may appear twice down a single branch — what must not happen is two sibling branches at one level both starting with `2`. See [problems/02-subsets-ii.cpp](problems/02-subsets-ii.cpp).

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. Give the one-sentence correctness argument for why include/exclude recursion produces every subset exactly once — no duplicates and no omissions.
2. Why is `O(2^n · n)` **not** evidence that a better algorithm exists, and what is the correct response when `n` is too large?
3. In `subsetsIterative` ([code.cpp](code.cpp)), what exactly goes wrong if you write `for (size_t i = 0; i < result.size(); ++i)` for the inner loop instead of snapshotting into `existing_count` — and is the symptom a wrong answer or a hang?
4. In `subsetsRecursiveHelper`, why must `result.push_back(current)` copy the buffer rather than store a reference, and what would the eight results look like if it stored a reference?
5. State the duplicate-skip rule from [problems/02-subsets-ii.cpp](problems/02-subsets-ii.cpp) exactly, and explain why `{2,2}` is still a required output for input `[1,2,2]` even though the rule "skips duplicates."
6. What is the invariant on `index` in the recursive version, and why is the base case `index == nums.size()` rather than "current is full"? How does `permutations`' base case differ, and why?
7. Name the single structural change that turns this pattern into Backtracking, and give a concrete example of a constraint that *can* be pruned versus one that *cannot*.
8. Map the subset `{1,3}` of `nums = {1,2,3}` to its bitmask integer, and explain why the recursion visits the masks in the order `0,4,2,6,1,5,3,7` while the bitmask loop in [problems/01-subsets.cpp](problems/01-subsets.cpp) visits them as `0..7`.
9. Why does "enumerate every permutation" become intractable at a noticeably *smaller* `n` than "enumerate every subset"? Give the concrete numbers at `n = 10`.
10. Which of these belongs to DP rather than Subsets, and why: (a) list every subset summing to 10, (b) does any subset sum to 10, (c) list every subset of even length? Justify each.
