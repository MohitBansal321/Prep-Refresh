# Backtracking


> **In one line:** choose a value for the next decision slot, recurse, then undo the choice before trying the next candidate — skipping (pruning) any candidate that cannot possibly lead to a valid solution.

```cpp
void backtrack(int n, int row, std::vector<int>& colOfRow,
               std::vector<std::vector<std::string>>& solutions) {
  if (row == n) { solutions.push_back(buildBoard(colOfRow, n)); return; }

  for (int col = 0; col < n; ++col) {
    if (!isSafe(colOfRow, row, col)) continue;   // PRUNE: skip without recursing

    colOfRow[row] = col;                          // 1) CHOOSE
    backtrack(n, row + 1, colOfRow, solutions);    // 2) RECURSE
    colOfRow[row] = -1;                            // 3) UNDO — next sibling must see a clean slate
  }
}
```

Exponential in the worst case (N-Queens: bounded by O(n!)) — pruning via `isSafe` is what keeps it tractable in practice. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Build a solution incrementally, one choice at a time, and the instant a partial choice can no longer lead anywhere valid, undo it ("backtrack") and try the next option — instead of exploring every possibility to full depth before checking whether any of it was ever valid.

## Real Life Analogy

Think of **solving a maze on foot**. You walk forward, choosing a direction at every junction. Eventually you hit a dead end — a wall, a locked door, a path that loops back on itself. You do not start over from the entrance. You **retrace your steps** ("backtrack") to the last junction where you had an unexplored option, and you try that option instead. If that also dead-ends, you retreat one junction further, and so on, until either you find the exit or you have proven every path from your current position is a dead end.

Two things make this efficient rather than wasteful:
1. **You stop the instant a path is provably wrong** — you do not walk the entire remaining maze length past a locked door just to "be thorough."
2. **You only retreat as far as necessary** — one junction, not all the way to the entrance — because everything before that junction was still a valid, still-promising choice.

Another everyday one: **filling in a paper Sudoku puzzle in pencil**. You write a digit into a cell, and if three cells later you realize that digit forces a contradiction somewhere else in the same row, you erase it (not the whole grid) and try the next digit in that one cell.

Backtracking is that same discipline in code: build a partial solution, and the moment you can prove it is doomed, undo the last decision and try the next one — never further back than necessary, and never forward past the point where you already know you have failed.

## Problem

### What engineering problem exists?

A large class of problems asks you to find (or enumerate) arrangements that satisfy a set of **constraints** — rules that a valid final answer must obey:

- **N-Queens:** place `n` queens on an `n x n` board so that no two attack each other (no shared row, column, or diagonal).
- **Sudoku:** fill a 9x9 grid so every row, column, and 3x3 box contains each digit 1-9 exactly once.
- **Word Search:** find a path through a grid of letters, moving to adjacent cells, that spells out a target word without reusing a cell.
- **Combination Sum / Palindrome Partitioning:** build a list of pieces (numbers, substrings) from a source, where every piece — and the whole list — must satisfy some rule (sum to a target; be a palindrome).

> **Term: Constraint-satisfaction problem (CSP).** A problem defined by a set of variables, each with a domain of possible values, plus constraints restricting which combinations of values are allowed. N-Queens, Sudoku, and Word Search are all CSPs: the "variables" are board positions (or path steps), the "domain" is which piece/digit/letter can go there, and the "constraints" are the attack rules, the row/column/box rules, and the adjacency/no-reuse rule, respectively.

**Most of the raw decision tree these problems generate is invalid.** Placing queens row by row, trying all `n` columns per row with no regard for earlier rows, generates `n^n` combinations — 16,777,216 for an 8x8 board — of which only 92 are valid solutions. The overwhelming majority of that tree is dead weight, and the ratio only worsens as `n` grows.

### Why is this problem difficult?

- **Full-depth exploration wastes work on branches doomed from step one.** Building a complete board (or word path, or partition) and checking every constraint only *then* means paying the full construction cost for every invalid branch before discovering it never worked — exactly the waste the 92-out-of-16.7-million ratio above illustrates.
- **"Provably doomed" needs a precise, checkable rule**, not a vague sense of "this looks wrong." For N-Queens: "does the new queen share a row, column, or diagonal with any queen already placed." For Sudoku: "does this digit already appear in this row, column, or 3x3 box." Too loose misses real conflicts; too tight rejects valid placements — either way, wrong answers or missed solutions.
- **The state constraints are checked against is shared and mutated in place** (a single board array, a single "used" grid, a single running partition list), for memory efficiency and simplicity. Every choice made must be **precisely undone** before trying the next option, or the shared state silently drifts out of sync with reality for every sibling branch still to come.

### What happens if we ignore it?

- **Programs that "technically work" but time out or exhaust memory** on inputs only slightly larger than a toy example — an `n=12` N-Queens brute force, or a Sudoku solver that fills the whole board before checking any rule, becomes impractically slow long before `n` or the grid size gets genuinely large.
- **Silent state corruption if the undo step is skipped**, which is worse than a slow program — it produces **wrong answers** that look plausible, because the shared board/grid/list is left in a state that no longer reflects "what has actually been decided so far" for the branches still to come.

## Solution

The core idea has three moving parts, repeated at every decision point in the recursion:

1. **Choose.** Commit to one candidate value for the current decision slot (a column for this row's queen, a digit for this cell, a neighbor cell for the next letter of the word, a substring for the next partition piece). This mutates the shared partial-solution state.
2. **Recurse.** Attempt to complete the rest of the solution given that choice, by making the next decision the same way.
3. **Undo.** The moment the recursive call returns — whether it found a complete solution, hit a dead end, or (in "find all solutions") already extracted everything it could — reverse the mutation made in step 1, so the shared state is exactly as it was before this candidate was tried. This makes the *next* candidate in the current decision slot see a clean, correct starting point.

Sitting in front of "Choose" is the step that makes this different from brute force: **before** committing to a candidate, check whether it is even legal given everything decided so far. If it is not, skip it entirely — no Choose, no Recurse, no Undo needed, because nothing was ever mutated. This is **pruning**: cutting off an entire subtree before it is ever explored, rather than exploring it and discovering the problem at the bottom.

Put together as one control-flow loop, at every decision slot:

1. Check the base case first: if the partial state is already **complete**, record it (or return success immediately, for "find one" problems) and return — nothing was chosen in this call, so nothing needs undoing.
2. Otherwise, loop over every remaining candidate for this slot. The constraint check runs **before** anything else; a candidate that fails it is skipped immediately, with no mutation at all (the prune).
3. A candidate that passes gets **choose -> recurse -> undo**, in that order, before the loop tries the next candidate. Once every candidate has been tried, return to the caller — this slot's own choice (made one level up) is about to be undone by that caller in turn.

The thinking behind it:

- **Every unit of exploration should be justified by the constraint, not by "we'll find out eventually."** A candidate is only worth recursing into if it is still *possible*, given current information, that it leads somewhere valid. If it is already provably impossible, recursing into it can only waste time.
- **The recursion structure mirrors the problem's natural decision order**, not an arbitrary one — one row at a time for N-Queens, one empty cell at a time (in scan order) for Sudoku, one grid step at a time for Word Search, one partition boundary at a time for Palindrome Partitioning. Choosing a decision order that lets you check constraints as early as possible (e.g. "one queen per row" rather than "place all queens, then check") is itself part of designing an efficient backtracking solution.

See [Architecture](#architecture) for the participants behind this loop, then [The Code](#the-code) for how it plays out in [code.cpp](code.cpp).

## Architecture

The "participants" in a backtracking search are the pieces of state and logic every problem in this family shares, whatever their concrete form:

1. **The partial-solution state.** The evolving, shared, in-place-mutated representation of "what has been decided so far" — a `colOfRow` array of queen columns (N-Queens), the Sudoku board itself, a grid with cells temporarily marked "in use" (Word Search), or a running list of partition pieces (Palindrome Partitioning). It is *partial* until the recursion reaches its base case, at which point it represents one complete candidate answer.
2. **The constraint check ("is this partial state still valid?").** A function that, given the partial state and a proposed next choice, answers **before any recursion happens** whether that choice keeps the partial state on a path that could still possibly lead to a valid complete solution — the single most important piece of a backtracking solution to get right (see [Tradeoffs](#tradeoffs) for what happens when it's wrong).
3. **The choose/recurse/undo triple.** The control-flow skeleton wrapping every candidate: mutate the shared state to reflect the choice, recurse, then unconditionally revert the mutation before moving to the next candidate (or returning to the caller). This is what turns "try this option" into "try this option, and leave no trace if it does not pan out."
4. **The base case.** The condition marking a *complete* solution — all `n` rows have a queen (N-Queens), every cell is filled (Sudoku), the whole target word has been matched (Word Search), the whole source string has been consumed (Palindrome Partitioning). Reaching it means recording the solution (or returning immediately, if only one is needed), then letting the call stack unwind back through the same undo steps.

## Why Not Other Approaches?

**"Brute-force generate every possibility, then filter for validity at the end (generate-then-filter)."**
The same underlying idea as the [Subsets](../subsets/) pattern — recurse through every combination of choices — except Subsets is used precisely when **every leaf is valid output**. Applying "explore everything, decide at the leaf" to a constraint-satisfaction problem means building out full-depth branches that are already invalid after the very first choice, and discovering that only at the end. Both are technically exponential in the worst case, but generate-then-filter is exponentially slower **in practice** — it pays the full-depth cost for every doomed branch instead of stopping at the first step that proves it doomed. For 8-Queens: generate-then-filter over "one queen per row" considers all `8^8` = 16,777,216 raw row/column assignments; backtracking with row/column/diagonal pruning explores a tiny fraction of that before finding all 92 valid boards.

**"Throw a generic solver at it (SAT solver, ILP, constraint programming library)."**
For genuinely large, real-world constraint problems (scheduling hundreds of resources, large-scale configuration validation), this is often the *right* production answer — dedicated CSP/SAT/ILP solvers have decades of engineering behind their pruning heuristics. But for interview-sized and most application-level puzzles (an 8x8 or 9x9 board, a few dozen items), reaching for an external solver is overhead disproportionate to the problem — hand-written backtracking with a precise constraint check is simpler, has zero dependencies, and is fast enough.

**"Recurse with no constraint check, and validate the whole thing at the base case."**
Not a different approach from the first one — it is generate-then-filter written recursively instead of iteratively. The recursion tree shape is identical; only *when* the constraint gets checked changes (at the leaf instead of never). Same exponential-in-practice blowup, because skipping the check at each internal node throws away the only opportunity to prune.

Backtracking's entire value proposition is checking the constraint **as early as possible, at every single decision point**, turning "this branch cannot work" into an immediate skip rather than a fully-built, fully-checked, ultimately-discarded leaf — and it is only as good as how early and how precisely that constraint check is written.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart deciding between Backtracking, Subsets, and Dynamic Programming based on the signals in a problem statement.
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of the choose -> check-constraint -> recurse-or-prune -> undo loop.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of the N-Queens (`n=4`) decision tree, showing one branch getting pruned and fully backtracked, and one branch succeeding.

## The Code

[code.cpp](code.cpp) is a **generic, reusable N-Queens solver** — `solveNQueens(n)` — the cleanest possible illustration of the choose/recurse/undo skeleton: the partial state is a single small array (`colOfRow`), the constraint check (`isSafe`) is a handful of comparisons, and there is exactly one thing to undo per decision (retract the one queen just placed). It returns every valid board as a `vector<vector<string>>` (each solution is `n` strings of `'Q'`/`'.'` characters), matching the shape LeetCode 51 expects.

- **`isSafe`.** The constraint check. Given the columns already chosen for every earlier row (`colOfRow[0..row-1]`), it answers whether `(row, col)` would share a column or a diagonal with any of them. It never checks for a shared *row*, because the recursion places exactly one queen per row by construction. Called **before** any mutation, which is what makes it a prune rather than a post-hoc check.
- **`buildBoard`.** A pure formatting helper converting the compact `colOfRow` representation into the `vector<string>` board format the problem expects — no bearing on the backtracking logic itself, and it only runs once a solution is found.
- **`backtrack`.** The recursive engine, implementing the [Solution](#solution) loop almost line for line: base case (`row == n`) records and returns; otherwise it loops over every candidate column, skips (prunes) any failing `isSafe`, and for each that passes, **chooses** (`colOfRow[row] = col`), **recurses**, then **undoes** (`colOfRow[row] = -1`) before the next candidate. The file's comments number each of these three steps explicitly.
- **`solveNQueens`.** The public entry point — allocates the initial empty `colOfRow` and kicks off the recursion at row 0, so callers never see or manage `colOfRow` directly.
- **`main`.** Exercises `solveNQueens` against known answers (`n=1` has 1 solution, `n=2` and `n=3` have 0, `n=4` has exactly 2, `n=8` has the standard 92) and prints `[PASS]`/`[FAIL]`, plus the two `n=4` boards themselves.

This is deliberately the simplest case, before the more elaborate, problem-specific variations in [problems/](problems/) — a 2D grid with a three-part constraint for Sudoku; state living *inside* the grid for Word Search; a substring-property constraint instead of a positional one for Palindrome Partitioning. Each file there is a complete, standalone solution to one named LeetCode problem, implementing the same skeleton inline (not calling into `code.cpp`, to stay dependency-free and independently readable) — see [problems/README.md](problems/README.md) for the index and rationale. Briefly: [problems/01-n-queens.cpp](problems/01-n-queens.cpp) restates the skeleton above as a standalone LeetCode 51 solution; `02` applies it to Sudoku's row/column/box constraint, with an "undo only on failure" variation; `03` puts the partial state *inside* the grid itself (an in-place "used" marker), making the undo step's importance vivid; `04` moves the constraint check onto a substring property (palindrome-ness), showing the skeleton generalizes past grids.

## Tradeoffs

**What backtracking buys you**

- **Eliminates doomed work before it happens, not after.** Pruning at the earliest possible decision point avoids building out entire subtrees that were never going to produce a valid answer — the core efficiency win over generate-then-filter (see [Why Not Other Approaches?](#why-not-other-approaches)).
- **One skeleton for "all," "one," or "the best" solution.** The choose/recurse/undo loop needs only minor adjustment (return early vs. keep searching) regardless of which of the three a problem wants.
- **Memory-efficient by construction.** State is mutated in place and undone rather than copied at every recursive call, so the extra memory cost is proportional to recursion *depth*, not to the number of branches explored — bounded even for a search spanning millions of candidate branches.
- **The constraint check doubles as documentation of the problem's rules.** `isSafe`, the Sudoku row/column/box check, and the palindrome check each state precisely, in code, what makes a partial solution valid.
- **Composable with problem-specific pruning heuristics.** Nothing prevents adding smarter, tighter pruning (e.g. choosing the most-constrained cell first in Sudoku) on top of the basic loop — exactly how production-grade constraint solvers extend this same idea.
- **Versus a generic CSP/SAT/ILP solver:** zero external dependencies and a solution sized to the problem, rather than integrating a general-purpose solver library for a board that fits in a `vector<vector<string>>`.

**What it costs you**

- **Still exponential in the worst case, even with pruning.** Pruning reduces how much of the tree is *actually explored* — often by orders of magnitude, as [Complexity](#complexity) shows for N-Queens — but not the asymptotic ceiling. An adversarial input, with little exploitable structure, can still approach the full exponential tree.
- **The "undo" step is easy to forget, and its absence fails silently.** Skipping it doesn't crash — it leaves the shared state mutated for every sibling branch still to be explored, silently corrupting the search. This is the single most common bug in backtracking code, precisely because the code still *runs* — just wrong.
- **Getting the constraint check even slightly wrong breaks the entire search**, with no error message either way: too loose lets invalid "solutions" leak into the output; too tight silently prunes away valid ones.
- **Recursion depth is bounded by problem size, but stack usage still matters** for very large boards or very long strings — a real, practical concern distinct from the algorithm's time complexity.
- **Correctness depends on two things the compiler cannot check for you** — the constraint check's precision, and the undo step's completeness. A generic solver library, by contrast, has had its correctness properties independently verified across far more edge cases than a hand-written check typically covers.

## Complexity

**Time (general):** exponential in the worst case — the raw decision tree, before any pruning, has a branching factor tied to the number of candidates per decision slot, raised to the number of decision slots. Pruning does not change this worst-case bound; it changes how much of that tree is *actually explored* for a given input.

**How pruning changes practice, concretely:**
- **N-Queens (`n=8`):** raw "one queen per row, no constraint" placements: `8^8` = 16,777,216. With column/diagonal pruning, the search explores only a small fraction of that before finding all 92 valid boards — the practical runtime is effectively instant, even though the *asymptotic* worst case (for general `n`) remains exponential (specifically bounded by `O(n!)`, since pruning enforces "one queen per column" implicitly by construction, collapsing the raw `n^n` space down to at most `n!` row/column permutations before diagonal pruning even starts).
- **Sudoku:** worst case is `O(9^m)` for `m` empty cells — astronomically large for `m` near 81. In practice, row/column/box pruning leaves only 1-3 plausible digits per empty cell on a realistically-constrained puzzle, so real Sudoku puzzles solve near-instantly despite the exponential bound.
- **Word Search:** worst case is `O(m * n * 4^L)` for an `L`-letter word on an `m x n` grid. In practice, the very first letter-mismatch or already-visited check prunes almost every branch within one or two steps for words that are not actually present.
- **Palindrome Partitioning:** worst case is `O(n * 2^n)`. Unlike the problems above, this one has a genuinely adversarial input that reaches close to that ceiling: a string of all-identical characters makes *every* substring a palindrome, so there is nothing to prune, and the algorithm enumerates close to the full `2^(n-1)` partitions. This is the clearest example in this module of pruning's limits — it only helps when the constraint actually rules out candidates; if the constraint is satisfied almost everywhere, backtracking degrades toward full enumeration.

**Space:** O(depth) for the recursion stack in every case discussed here (`n` for N-Queens, `m` for Sudoku's empty cells, `L` for Word Search's word length, `n` for Palindrome Partitioning's string length), plus whatever space the output solutions themselves require (which can itself be exponential if there are exponentially many valid solutions to return).

This is exactly why generate-then-filter (see [Why Not Other Approaches?](#why-not-other-approaches)) is the difference between "instant" and "does not finish" here: for a problem with exploitable constraints, it pays the complete raw-combination cost regardless of how quickly a branch becomes invalid, with no early exit.

## Common Mistakes

- **Forgetting the "undo" step after a recursive call returns.** This is the single most common and most damaging mistake in backtracking code: the shared partial-solution state (a board, a grid, a list) is left mutated from a choice that has already been explored, silently corrupting every sibling branch still to be tried at the current decision slot. *Why it happens:* it is easy to reason about "choose, then recurse" and simply forget that the recursive call's return is not the end of the story — there is a mandatory third step. *Avoid:* treat choose/recurse/undo as one indivisible unit while writing the code; write the undo line immediately after writing the choose line, before writing anything in between, as a discipline.
- **Checking constraints too late — after fully building an invalid partial state instead of pruning as early as possible.** This does not produce wrong answers (a validity check at the leaf still catches the problem eventually), but it silently reintroduces the generate-then-filter performance problem this whole pattern exists to avoid. *Why it happens:* it can feel simpler to "build the whole thing, then check it once" than to thread a constraint check into every intermediate step. *Avoid:* identify the earliest point in construction at which a violation becomes *detectable*, and check there — not at the end.
- **Off-by-one in board/index bounds.** Checking `row <= n` instead of `row < n` for the base case, or scanning columns `1..n` instead of `0..n-1`, either misses valid candidates or reads/writes out of bounds. *Why it happens:* board and grid problems mix 0-indexed arrays with 1-indexed problem descriptions (LeetCode's 1-indexed board coordinates in some problem statements, for instance), and it is easy to transcribe the wrong convention into code. *Avoid:* be explicit, in a comment at the top of the function, about which indexing convention is in effect and stick to it consistently.
- **Mutating the partial state in the constraint check itself.** A constraint check that accidentally *writes* to the shared state (instead of only reading it) corrupts the very state it was supposed to validate, and the corruption may not be visible until several decision slots later. *Avoid:* keep constraint-check functions strictly read-only; if a check needs to try something hypothetically, do it via a local copy or a parameter, never the shared state.
- **Not distinguishing "find one solution" from "find all solutions" in the return-value handling.** A "find one" search should stop and propagate success immediately once a complete base case is reached (no need to keep exploring sibling candidates); a "find all" search must keep going even after recording a solution. Mixing these up either wastes enormous time (continuing to search after the single needed answer is already found) or misses solutions (stopping too early when all of them were required). *Avoid:* decide up front which of the two the problem needs, and make the base case's control flow (`return true` vs. `push_back(...); [continue searching]`) match that decision explicitly.

## When To Use

- **The problem is a constraint-satisfaction problem**: placing items on a board or into slots such that pairwise or positional rules must hold (N-Queens, Sudoku, graph coloring).
- **You need every valid arrangement (or one valid arrangement, or the best one)**, not just a count or an existence check that a smarter closed-form or DP approach could answer more cheaply.
- **A partial solution can be proven invalid before it is complete**, and that proof is cheap to check (a handful of comparisons against already-made choices) — the entire value proposition depends on this being true.
- **The state naturally supports efficient mutate-then-undo**, so you can avoid copying the whole partial solution at every recursive call (an array index update, a grid cell flip, a list push/pop).
- **The search space, after pruning, is small enough to explore exhaustively** in the time you have — small boards, short words, modest string lengths; backtracking is not a substitute for genuinely large-scale combinatorial optimization.

## When NOT To Use

- **There is no constraint to prune on — every leaf of the naive recursion is valid output.** That is [Subsets](../subsets/)'s job (enumerate every subset/combination/permutation), not Backtracking's; adding an unused "constraint check" that always passes is pure overhead dressed up as backtracking.
- **The problem has overlapping subproblems that could be memoized, and you only need an optimal value or count, not every distinct arrangement.** That is [Dynamic Programming](../../dynamic-programming-patterns/)'s territory: if the same sub-state recurs via multiple different paths and caching its answer would save real recomputation, re-deriving it from scratch on every branch (as plain backtracking does) wastes work DP would not.
- **The search space, even after pruning, is too large to explore exhaustively in the time available** (large-scale scheduling, real-world constraint programming over thousands of variables) — reach for a dedicated CSP/SAT/ILP solver, or a heuristic/approximate method, instead of hand-rolled backtracking.
- **You only need to know whether *a* solution exists, and a direct constructive or greedy algorithm already solves that in polynomial time** — do not reach for exponential-worst-case backtracking when a cheaper, purpose-built algorithm answers the same question.

## Where This Shows Up

Backtracking is a mainstay of technical interviews specifically because N-Queens, Sudoku, and Word Search are compact enough to code in 30-40 minutes while still requiring the candidate to articulate a precise constraint check and correctly reason about the undo step — a genuine test of whether "I understand recursion" extends to "I understand recursion over shared, mutated state."

In production and tooling:

- **Constraint solvers** embedded in scheduling, configuration, and resource-allocation tools, often augmented with far more sophisticated pruning heuristics — constraint propagation, most-constrained-variable ordering.
- **Puzzle generators and solvers** — Sudoku generators verifying a puzzle has a *unique* solution, crossword-construction tools, logic-puzzle solvers — run a backtracking search as their correctness-verification step, not just to solve one instance.
- **Configuration/feature-flag validators with mutual-exclusion rules** — package/dependency resolvers checking conflicting version constraints, infrastructure-as-code validators — use "choose a setting, check it against everything chosen so far, undo if it conflicts" to *find* a valid combination, not just validate one.
- **Compilers and type systems** occasionally use backtracking-style search during type inference or overload resolution, trying candidate types/overloads and backing out on a contradiction.
- **A feature-flag combination validator** enumerating every valid enabled-flag combination for QA to test before a release (see the Real-world Challenge in [exercises.md](exercises.md)).
- **A test-data generator for constrained schemas** — e.g. "if `country` is `US`, `state` must be one of 50 values; if `plan` is `enterprise`, `seat_count` must be `>= 10`" — enumerating valid combinations as test fixtures instead of hand-writing every case.
- **A dependency/version conflict resolver prototype** — "choose a version per package, check compatibility against already-chosen versions, undo on conflict" mirrors, at small scale, what real package managers' resolvers do.
- **A crossword or word-placement puzzle generator** — the same Word-Search-style search, run in the "generate" direction instead of "verify."
- **A rule-based access-control combination checker** — given roles/permissions with separation-of-duty constraints (financial/healthcare systems: "the same person cannot both approve and submit a transaction"), enumerating valid role-assignment combinations for an audit tool.

## Similar Patterns

- **[Subsets](../subsets/):** the exact same recursive choose/recurse(/undo) skeleton, but with **no constraint to prune on** — every leaf is valid output, so the search is enumerated rather than pruned. Backtracking is Subsets *plus* a constraint check that cuts off invalid branches early; Subsets is the special case where that check always passes. See the [family README](../README.md) for the "how to tell them apart" table.
- **[Dynamic Programming](../../dynamic-programming-patterns/):** also explores a decision tree, but targets problems with **overlapping subproblems** where only an optimal value or count is needed, not every arrangement. Backtracking re-derives every branch independently; DP caches a sub-state's answer once and reuses it, trading the ability to reconstruct every arrangement for a often-dramatic cut in redundant work.
- **Brute-force recursion / generate-then-filter:** the same recursive tree, without early pruning — every branch built to full depth before one validity check at the leaf. Backtracking's direct conceptual predecessor, and exactly what its pruning step improves upon.
- **Branch and Bound:** a close relative for *optimization* (not just satisfaction) problems — prunes not only on hard-constraint violation but also whenever a computed bound proves a branch cannot beat the best solution found so far. Same skeleton, with a numeric bound driving extra pruning.

| Pattern | Prunes invalid branches? | Needs every distinct arrangement? | Handles overlapping subproblems? | Primary question answered |
|---|---|---|---|---|
| Backtracking | Yes, as early as possible | Yes (or one, or the best) | No — re-derives each branch | "Which arrangement(s) satisfy these constraints?" |
| Subsets | No — nothing to prune | Yes, always all of them | No | "What are all the subsets/combinations/permutations?" |
| Dynamic Programming | N/A (different mechanism: memoization) | Usually no — value/count only | Yes — this is its defining strength | "What is the optimal value/count, given choices that recur?" |
| Branch and Bound | Yes, plus bound-based pruning | Usually no — best solution only | No | "What is the single best arrangement, pruning by a numeric bound?" |

## Interview Discussion

Experienced engineers evaluating a backtracking solution rarely dwell on "does the recursion terminate" — that is usually clear from the base case. What they actually probe is whether the candidate can **state the constraint check precisely** and **prove the undo step is complete and unconditional**. A candidate who says "I check for conflicts against every already-placed queen before placing a new one, and I always remove the queen after the recursive call returns, whether or not it succeeded" is demonstrating the two things that actually determine correctness here.

Common follow-up questions:
- *"How would you prune more aggressively?"* — expects recognition that the *order* in which candidates or decision slots are tried can matter (e.g. in Sudoku, choosing the empty cell with the fewest remaining valid digits first, rather than always scanning left-to-right) — a real technique called constraint propagation / most-constrained-variable ordering.
- *"What is the time complexity, and is that bound ever actually reached?"* — expects pointing to [Complexity](#complexity)'s distinction between the worst-case asymptotic bound and the practically-explored tree size, and naming at least one adversarial input (like Palindrome Partitioning's all-same-character string) where the bound genuinely is approached.
- *"How would you find just one solution instead of all of them?"* — expects recognizing that the recursive function's return type/control flow needs to short-circuit (return `true`/stop immediately) on the first success, rather than continuing to explore sibling candidates.
- *"What happens if you forget to undo?"* — expects the precise answer: not a crash, but silent corruption of the shared state for every sibling branch, with no error signal.
- *"When would you NOT use backtracking here?"* — expects recognizing the two off-ramps: no real constraint to prune on (use Subsets), or overlapping subproblems where caching would help (use DP).

Common misconceptions:
- "Backtracking is always fast because of pruning." It shrinks the *practically explored* tree, often dramatically, but the worst-case bound stays exponential, and some inputs approach it closely.
- "Backtracking and brute-force recursion are the same thing." They share the recursive skeleton, but backtracking's defining feature is checking constraints *before* recursing, not after building a complete candidate.
- "If a problem needs 'all valid arrangements,' it must be backtracking." Only if there is an actual constraint to prune on — if there is not, it is Subsets wearing backtracking's clothing (see Exercise 1: Letter Case Permutation).

## Key Takeaways

1. Backtracking's skeleton is choose -> recurse -> undo, guarded by a constraint check that runs **before** choosing, so invalid branches are pruned rather than explored to a doomed leaf — this is its entire value over brute-force generate-then-filter.
2. The undo step must be unconditional — it runs whether the recursive call succeeded, failed, or exhausted every possibility beneath it.
3. Pruning does not change the asymptotic worst case (still exponential); it changes how much of the tree is actually explored for realistic inputs, often by many orders of magnitude.
4. Some inputs are adversarial with respect to pruning (an all-same-character string for Palindrome Partitioning) and genuinely approach the exponential worst case — pruning is only as good as how often the constraint actually rules something out.
5. Forgetting to undo is the single most common and most damaging mistake: it silently corrupts shared state for sibling branches rather than crashing.
6. Checking constraints too late (post-hoc, at a fully-built leaf) reintroduces the generate-then-filter performance problem, even though the answer stays correct.
7. No constraint to prune on -> reach for Subsets instead. Overlapping subproblems needing only an optimal value/count -> reach for Dynamic Programming instead.
8. Decide up front whether you need one solution, all solutions, or the best solution — the base case's control flow must match that choice.
9. Real production analogues: constraint solvers, puzzle generators/verifiers, configuration validators with mutual-exclusion rules, dependency-resolver prototypes — not just interview puzzles.
10. Getting the constraint check even slightly wrong breaks the whole search silently — too loose lets invalid answers through, too tight prunes away valid ones with no error message either way.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — covers backtracking as part of its broader treatment of algorithmic search and pruning strategies.
- *The Art of Computer Programming, Volume 4A: Combinatorial Algorithms* — Donald Knuth — the definitive, deep treatment of backtracking, N-Queens, and exhaustive search/pruning techniques.
- *Algorithm Design Manual* — Steven Skiena — has a practical, engineering-oriented chapter on backtracking with worked examples and pruning heuristics.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes worked backtracking problems (including N-Queens-style and combinatorial search problems) with C++-specific implementation notes.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — includes a dedicated backtracking/recursion chapter covering N-Queens, permutations, and related problems.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including N-Queens, Sudoku solving, and other backtracking-style search implementations, useful for seeing varied implementation styles.
- Constraint-programming solver projects such as `OR-Tools` (Google's open-source operations research library, includes a CP-SAT constraint solver) — a production-grade look at how backtracking-style search is extended with sophisticated pruning and propagation for real optimization problems.

**Official Documentation**
- LeetCode — N-Queens (problem 51).
- LeetCode — Sudoku Solver (problem 37).
- LeetCode — Word Search (problem 79).
- LeetCode — Palindrome Partitioning (problem 131).
- LeetCode — Combination Sum (problem 39), Word Search II (problem 212), Restore IP Addresses (problem 93) — referenced in [exercises.md](exercises.md).

**Blog Articles**
- GeeksforGeeks — "Backtracking Algorithms" — a widely used explainer covering the general technique and its classic problem set (N-Queens, Sudoku, Rat in a Maze, Hamiltonian Cycle).
- NeetCode — Backtracking pattern videos/playlist — walks through N-Queens, Sudoku Solver, Word Search, Combination Sum, and Palindrome Partitioning with visual explanations of the recursion tree.
- Educative.io — "Grokking the Coding Interview" backtracking-adjacent pattern chapters (Subsets, Combinations) — useful for seeing how the same recursion skeleton is framed just short of a constraint check.
