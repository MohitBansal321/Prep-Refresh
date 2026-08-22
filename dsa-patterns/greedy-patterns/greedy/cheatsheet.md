# Greedy — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Optimization strategy pattern — irrevocable local decisions, usually after sorting on a proven key. |
| **Recognition Signal** | The problem asks for a **maximum/minimum count or extremum** over items you select, order, or schedule; there is a **sort key** making each step's choice decidable in `O(1)` from one scalar of state; and you can state an **exchange argument** ("swapping my choice into any optimal solution never hurts") in one sentence. |
| **Problem** | Brute force over subsets/orderings is `O(2^n)`/`O(n!)`; DP explores both "take it" and "skip it" at every decision for `O(n · W)` time and `O(W)` space even when one branch is provably never worse. |
| **Solution** | Sort by the proven key, then walk once committing to each choice permanently. Maintain `O(1)` running state (`lastEnd`, `farthest`, a deficit, a pointer into a second sorted list); one `if` per item decides take-or-skip. **Exactly one branch is ever explored** — that absence of comparison is the pattern's signature. |
| **Time / Space Complexity** | `O(n log n)` time (sort dominates; pass itself `O(n)`), `O(1)` extra space. No-sort flavors (Jump Game frontier, Gas Station deficit) are `O(n)` time. |
| **Pros** | Fastest correct-by-proof option available · tiny code surface (comparator + scalar + one `if`) · streams naturally (state is `O(1)`, decisions irrevocable) · explains itself in plain English · composes inside Dijkstra/Kruskal/Huffman. |
| **Cons** | **Fails silently** — a wrong rule terminates and returns a plausible answer · correctness depends on the input distribution, not just the code ({1,5,10,25} vs {1,3,4} coin change) · proof obligation is real work, sometimes non-trivial · wrong sort key = wrong answer with identical-looking code · brittle: one added constraint (weights, history) usually kills the rule outright. |
| **Use When** | You can state the exchange argument in one sentence · unweighted counting/extremum over intervals, assignments, or reachability · constraint compresses to `O(1)` state · input is huge/streaming so DP memory is unavailable · optimizing an already-correct-but-slow DP. |
| **Avoid When** | You genuinely need to compare "take it" vs "skip it" (that is DP) · no exchange argument exists · weights/values added to what was a counting problem (unweighted interval scheduling is greedy; weighted is DP) · you must enumerate all optima · deciding item `i` needs the full set of past choices, not one scalar · being subtly wrong is expensive and the proof is shaky. |
| **Related Patterns** | Dynamic Programming (explores both branches; the fallback when the exchange argument fails) · Merge Intervals (a specific greedy: sort by *start*, extend-or-flush — note Activity Selection sorts by *end*) · Two Pointers (mechanical vehicle for assignment-style greedies like Assign Cookies) · Backtracking (prunes invalid branches; greedy prunes every alternative branch on proof alone). |

### Template Skeleton

```cpp
// Flavor 1: sort-then-commit (interval scheduling shape)
std::sort(items.begin(), items.end(),
          [](const Item& a, const Item& b) { return a.key < b.key; });  // THE decision
State state = neutral;                                                   // e.g. lastEnd
for (const auto& item : items) {
    if (compatible(item, state)) {   // one irrevocable if -- no else-branch revisits
        ++answer;
        state = update(state, item);
    }
}

// Flavor 2: no-sort running frontier (Jump Game shape)
int farthest = 0;
for (int i = 0; i < n; ++i) {
    if (i > farthest) return false;              // cursor overtook the frontier
    farthest = std::max(farthest, i + nums[i]);  // extend it -- route irrelevant
}
```

### Remember In One Sentence
> **Greedy makes one provably-safe local choice per step — usually after sorting on an exchange-argument-backed key — exploring exactly one branch where DP explores both, buying `O(n log n)`/`O(1)` speed at the price of owing a proof, because it is the only pattern whose failures are silent.**

### Two Facts People Get Wrong
- Passing the samples proves the greedy rule? **No** — samples can only *disprove* a rule, never establish one. Greedy coin change passes every {1, 5, 10, 25} example and is wrong for {1, 3, 4}, target 6 (takes 4+1+1 = 3 coins; optimum is 3+3 = 2).
- Greedy always requires sorting? **No** — Jump Game tracks a running frontier and Gas Station a running deficit in one `O(n)` pass with no sort at all. Sorting is the common way to make local choices safe, not a definitional requirement.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the two conditions a problem must satisfy for a greedy algorithm to be correct, and define each in one sentence.
2. Describe the exchange argument as a mechanical procedure — what do you assume, what do you swap, and what must you verify?
3. Why does Activity Selection sort by END time? Give the one-sentence exchange argument, and a three-interval counter-example that kills shortest-duration-first.
4. In the {1, 3, 4} coin-change counter-example, which condition breaks — greedy choice property or optimal substructure — and why does that same diagnosis explain why DP still works there?
5. What does the running state represent in each flavor: `lastEnd` (interval scheduling), `farthest` (Jump Game), `tank`/`start` (Gas Station)?
6. Why is Jump Game `O(n)` while interval scheduling is `O(n log n)`?
7. In Gas Station, once the tank goes negative at station `j`, why is it safe to restart from `j+1` rather than retrying any station between the old start and `j`?
8. In Task Scheduler, why does the most frequent task dictate the entire schedule length, and what role do the other max-frequency tasks play in the formula?
9. How would you practically validate a greedy rule you cannot formally prove? Why are hand-picked test cases structurally insufficient?
10. Name the modification that turns each of these from greedy-solvable into DP-required: (a) interval scheduling, (b) coin change, (c) a scheduling problem with history-dependent cooldowns.
