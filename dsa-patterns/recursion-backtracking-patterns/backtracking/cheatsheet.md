# Backtracking — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Recursion pattern — decision-tree search with early pruning. |
| **Recognition Signal** | Problem asks to build/place/arrange things under **constraints** (N-Queens, Sudoku, word search, valid partitions) where most partial states are invalid and should be abandoned **before** being built out to full depth. |
| **Problem** | Generate-then-filter (build every full-depth combination, check validity only at the leaf) wastes exponential work on branches that were already doomed after the first choice. |
| **Solution** | At every decision point: check the constraint **before** recursing (prune if it fails); otherwise **choose** (mutate shared state), **recurse** (solve the rest), **undo** (revert the mutation) unconditionally before trying the next candidate. |
| **Time / Space Complexity** | Exponential worst case regardless of pruning (pruning shrinks the *practically explored* tree, not the asymptotic ceiling); O(depth) extra space for the recursion stack + partial state. |
| **Pros** | Prunes doomed branches before wasting work on them · memory-efficient (mutate + undo, not copy) · same skeleton finds one/all/best solutions with minor changes · constraint check doubles as precise documentation of the rules. |
| **Cons** | Still exponential worst-case time · forgetting "undo" silently corrupts state for sibling branches (no crash, just wrong answers) · an imprecise constraint check breaks correctness in either direction with no error message · some inputs are adversarial and approach the worst case despite pruning. |
| **Use When** | Constraint-satisfaction problems (board placement, grid path-finding, valid partitions) where a partial solution can be proven invalid early and cheaply, and you need actual arrangement(s), not just a count. |
| **Avoid When** | No constraint to prune on (use Subsets) · overlapping subproblems where only an optimal value/count is needed (use Dynamic Programming) · search space too large to exhaustively explore even after pruning (use a dedicated CSP/SAT solver or heuristic). |
| **Related Patterns** | Subsets (same skeleton, no pruning — every leaf is valid) · Dynamic Programming (caches overlapping sub-states instead of re-deriving every branch) · Branch and Bound (adds numeric-bound pruning for optimization problems). |

### Template Skeleton

```cpp
// Generic choose -> recurse -> undo skeleton, with pruning via a constraint
// check evaluated BEFORE any mutation happens.
void backtrack(State& state, int decisionSlot /* row, cell index, position, ... */,
                Results& results) {
  if (isComplete(state, decisionSlot)) {
    results.push_back(snapshot(state));  // record a full valid solution
    return;                              // nothing chosen in THIS frame -> nothing to undo here
  }

  for (auto candidate : candidatesFor(decisionSlot)) {
    if (!isValid(state, decisionSlot, candidate)) {
      continue;  // PRUNE: skip without recursing -- the entire win over brute force
    }

    apply(state, decisionSlot, candidate);           // 1) CHOOSE
    backtrack(state, nextSlot(decisionSlot), results); // 2) RECURSE
    revert(state, decisionSlot, candidate);           // 3) UNDO -- unconditional, every time
  }
}
```

### Remember In One Sentence
> **Backtracking builds a solution one choice at a time, checking the constraint before every recursive call so invalid branches are pruned rather than explored to a doomed leaf, and undoing each choice unconditionally once its subtree has been fully explored.**

### Two Facts People Get Wrong
- Pruning makes backtracking **polynomial**? **No** — the asymptotic worst case is still exponential; pruning shrinks the *practically explored* tree (often dramatically), not the theoretical ceiling. Some adversarial inputs (an all-same-character string for Palindrome Partitioning) genuinely approach that ceiling.
- The "undo" step is just tidy **cleanup**? **No** — it is load-bearing correctness logic. Skipping it does not crash the program; it silently corrupts the shared state for every sibling branch still to be explored, producing wrong or incomplete results with no error signal.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the three steps of the backtracking skeleton, in order, and what "pruning" adds in front of them.
2. Why is checking the constraint *before* recursing strictly better than building the full solution and checking it only at the base case?
3. What specifically goes wrong if the "undo" step is forgotten — does the program crash, and if not, what actually happens?
4. For N-Queens, why does `isSafe` never need to check whether two placed queens share a row?
5. Name the adversarial input for Palindrome Partitioning that makes it approach its exponential worst case, and explain why pruning does not help there.
6. What is the single question that distinguishes "this is a Backtracking problem" from "this is actually a Subsets problem"?
7. What is the single question that distinguishes "this is a Backtracking problem" from "this is actually a Dynamic Programming problem"?
8. In the Sudoku solver, why does a successful recursive call skip the "undo" step for the digit that led to success, while a failed one does not?
9. Why does backtracking typically mutate shared state in place (and undo it) rather than copy the whole partial solution at every recursive call? What would copying cost instead?
10. Give one real production/system context (not a LeetCode problem) where backtracking-style constraint search shows up.
