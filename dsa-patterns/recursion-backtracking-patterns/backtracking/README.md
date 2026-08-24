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

The problem: **most of the decision tree these problems generate is invalid.** If you place queens row by row, trying all `n` columns in every row independently of what came before, you generate `n^n` raw combinations for an 8x8 board — that is 16,777,216 raw placements — yet only 92 of them are valid solutions. The overwhelming majority of that tree is dead weight.

### Why is this problem difficult?

- **Full-depth exploration wastes enormous work on branches that were doomed from step one.** If you build a complete board (or complete word path, or complete partition) and only *then* check whether it satisfies every constraint, you have already paid the full cost of constructing every invalid branch, all the way to a leaf, before discovering it was never going to work.
- **Knowing exactly when a partial state is "provably doomed" requires a precise, checkable rule**, not a vague sense of "this looks wrong." For N-Queens that rule is "does the new queen share a row, column, or diagonal with any queen already placed." For Sudoku it is "does this digit already appear in this row, column, or 3x3 box." Getting this check wrong — too loose (misses real conflicts) or too tight (rejects valid placements) — produces either wrong answers or missed solutions.
- **The state that constraints are checked against is shared and mutated in place**, for both memory efficiency and simplicity (a single board array, a single "used" grid, a single running partition list). That means every choice made must be **precisely undone** before trying the next option, or the shared state silently drifts out of sync with reality for every sibling branch still to be explored.

### What happens if we ignore it?

- **Combinatorial explosion in wasted work.** Generating every raw combination and filtering only at the end costs time proportional to the size of the *entire* raw combination space, not just the valid (or even the "reasonably close to valid") portion of it. For 8-Queens, that is millions of full boards built and checked instead of a search that abandons most branches after just one or two rows.
- **Programs that "technically work" but time out or exhaust memory** on inputs only slightly larger than a toy example — an `n=12` N-Queens brute force, or a Sudoku solver that fills the whole board before checking any rule, becomes impractically slow long before `n` or the grid size gets genuinely large.
- **Silent state corruption if the undo step is skipped**, which is worse than a slow program — it produces **wrong answers** that look plausible, because the shared board/grid/list is left in a state that no longer reflects "what has actually been decided so far" for the branches still to come.

## Why Not Other Approaches

**"Brute-force generate every possibility, then filter for validity at the end (generate-then-filter)."**
This is the same underlying idea as the [Subsets](../subsets/) pattern — recurse through every combination of choices — except Subsets is used precisely when **every leaf is valid output** (there is nothing to filter; you want all of them). Applying that same "explore everything, decide at the leaf" strategy to a constraint-satisfaction problem means building out full-depth branches that are already invalid after the very first choice, and only discovering that at the very end. Both approaches are technically exponential in the worst case, but generate-then-filter is exponentially slower **in practice**, because it does the full-depth work for every doomed branch instead of stopping at the first step that proves the branch is doomed. For 8-Queens: generate-then-filter over "one queen per row" considers all `8^8` = 16,777,216 raw row/column assignments; backtracking with row/column/diagonal pruning explores a tiny fraction of that before finding all 92 valid boards.

**"Formulate it as an optimization/search problem and throw a generic solver at it (SAT solver, ILP, constraint programming library)."**
For genuinely large, real-world constraint problems (scheduling hundreds of resources, large-scale configuration validation), this is often the *right* production answer — dedicated CSP/SAT/ILP solvers have decades of engineering behind their pruning heuristics. But for problems of the size interviews and most application-level puzzles present (an 8x8 or 9x9 board, a few dozen items), reaching for an external solver is enormous overhead for no real benefit — hand-written backtracking with a precise constraint check is simpler, has zero dependencies, and is fast enough.

**"Use recursion without any constraint check, and validate the whole thing at the base case (same idea as generate-then-filter, phrased as recursion)."**
This is not a different approach from the first one — it is generate-then-filter written recursively instead of iteratively. The recursion tree shape is identical; the only difference is *when* the constraint gets checked (at the leaf instead of never during construction). It suffers the exact same exponential-in-practice blowup, because skipping the constraint check at each internal node throws away the only opportunity to prune.

**Tradeoff summary:** every alternative either pays the full cost of building out doomed branches to their leaves (generate-then-filter, unconstrained recursion) or brings in machinery disproportionate to the problem's actual size (generic CSP/SAT solvers). Backtracking wins specifically because it checks the constraint **as early as possible, at every single decision point**, and turns "this branch cannot work" into an immediate skip rather than a fully-built, fully-checked, ultimately-discarded leaf. That is its entire value proposition — and it is only as good as how early and how precisely the constraint check is written.

## Solution

The core idea has three moving parts, repeated at every decision point in the recursion:

1. **Choose.** Commit to one candidate value for the current decision slot (a column for this row's queen, a digit for this cell, a neighbor cell for the next letter of the word, a substring for the next partition piece). This mutates the shared partial-solution state.
2. **Recurse.** Attempt to complete the rest of the solution given that choice, by making the next decision the same way.
3. **Undo.** The moment the recursive call returns — whether it found a complete solution, hit a dead end, or (in "find all solutions") already extracted everything it could — reverse the mutation made in step 1, so the shared state is exactly as it was before this candidate was tried. This makes the *next* candidate in the current decision slot see a clean, correct starting point.

Sitting in front of "Choose" is the step that makes this different from brute force: **before** committing to a candidate, check whether it is even legal given everything decided so far. If it is not, skip it entirely — no Choose, no Recurse, no Undo needed, because nothing was ever mutated. This is **pruning**: cutting off an entire subtree before it is ever explored, rather than exploring it and discovering the problem at the bottom.

The thinking behind it:

- **Every unit of exploration should be justified by the constraint, not by "we'll find out eventually."** A candidate is only worth recursing into if it is still *possible*, given current information, that it leads somewhere valid. If it is already provably impossible, recursing into it can only waste time.
- **Shared, mutated state is the efficient choice — as long as the undo discipline is airtight.** Backtracking typically mutates one board/grid/list in place rather than copying it at every recursive call (which would cost extra memory and time proportional to the state size, at every single node of the tree). The price of that efficiency is that every mutation *must* be paired with an exact, unconditional undo before returning control to the caller.
- **The recursion structure mirrors the problem's natural decision order**, not an arbitrary one — one row at a time for N-Queens, one empty cell at a time (in scan order) for Sudoku, one grid step at a time for Word Search, one partition boundary at a time for Palindrome Partitioning. Choosing a decision order that lets you check constraints as early as possible (e.g. "one queen per row" rather than "place all queens, then check") is itself part of designing an efficient backtracking solution.

No code yet — see [Architecture](#architecture) for the participants, then [Implementation](#implementation) for how this plays out in [code.cpp](code.cpp).

## Architecture

The "participants" in a backtracking search are the pieces of state and logic every problem in this family shares, whatever their concrete form:

1. **The partial-solution state.** The evolving, shared, in-place-mutated representation of "what has been decided so far" — a `colOfRow` array of queen columns (N-Queens), the Sudoku board itself, a grid with cells temporarily marked "in use" (Word Search), or a running list of partition pieces (Palindrome Partitioning). It is *partial* until the recursion reaches its base case, at which point it represents one complete candidate answer.

2. **The constraint check ("is this partial state still valid?").** A function that, given the partial state and a proposed next choice, answers **before any recursion happens** whether that choice keeps the partial state on a path that could still possibly lead to a valid complete solution. This is the single most important piece of a backtracking solution to get right — too loose, and invalid solutions leak through; too tight, and valid solutions are wrongly pruned away.

3. **The choose/recurse/undo triple.** The control-flow skeleton wrapping every candidate at every decision point: mutate the shared state to reflect the choice, recurse into the next decision, then unconditionally revert the mutation before moving on to the next candidate (or returning to the caller). This triple is what turns "try this option" into "try this option, and leave no trace if it does not pan out."

4. **The base case.** The condition marking a *complete* solution — all `n` rows have a queen (N-Queens), every cell is filled (Sudoku), the whole target word has been matched (Word Search), the whole source string has been consumed (Palindrome Partitioning). Reaching it means: record this solution (or return immediately, if only one solution is needed), then let the call stack unwind back through the very same undo steps described above.

Responsibilities in one line each:
- **Partial-solution state:** holds "what has been decided so far," shared and mutated in place across the whole search.
- **Constraint check:** the gatekeeper deciding, before any recursion, whether a candidate is even worth trying.
- **Choose/recurse/undo:** the mechanical loop that tries a candidate, explores its consequences, then cleans up regardless of outcome.
- **Base case:** the stopping condition that turns "still partial" into "one complete, valid answer."

## Execution Flow

Generalized, step by step, for any backtracking search:

1. Enter the current decision slot (a row, an empty cell, a grid position, the "next piece" boundary) with whatever partial-solution state has been built so far.
2. Check the base case: is the partial state already **complete**? If yes, record it as a full valid solution (or return success immediately, for "find one solution" problems), then return to the caller — nothing was chosen in this call, so nothing needs to be undone here.
3. If not complete, loop over every remaining candidate for this decision slot.
4. For each candidate, run the constraint check against the current partial state **before doing anything else**. If the candidate fails the check, skip it immediately — do not recurse, do not mutate anything (this is the prune).
5. If the candidate passes the check: **choose** it — mutate the shared partial-solution state to reflect this candidate.
6. **Recurse** into the next decision slot with the updated state.
7. Whatever the recursive call returns (a found solution, a dead end, or having exhausted every remaining possibility beneath it), **undo** the mutation from step 5 — always, unconditionally — restoring the partial state to what it was immediately before this candidate was chosen.
8. Continue the loop from step 3 with the next candidate, until every candidate for this decision slot has been tried.
9. Once all candidates for this decision slot are exhausted, return to the caller — this decision slot's exploration is complete, and its own choice (made one level up) is about to be undone by that caller in turn.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full flowchart deciding between Backtracking, Subsets, and Dynamic Programming based on the signals in a problem statement.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the choose -> check-constraint -> recurse-or-prune -> undo loop.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of the N-Queens (`n=4`) decision tree, showing one branch getting pruned and fully backtracked, and one branch succeeding.

## Implementation

[code.cpp](code.cpp) is a **generic, reusable N-Queens solver** — `solveNQueens(n)` — chosen as the implementation vehicle because it is the cleanest possible illustration of the choose/recurse/undo skeleton: the partial state is a single small array (`colOfRow`), the constraint check (`isSafe`) is a handful of comparisons, and there is exactly one thing to undo per decision (retract the one queen just placed). Every line of the choose/recurse/undo triple is commented inline with which of the three steps it implements, so you can see the skeleton in its purest form before looking at the more elaborate, problem-specific variations in [problems/](problems/) (a 2D grid with a three-part constraint for Sudoku; state living *inside* the grid itself for Word Search; a substring-property constraint instead of a positional one for Palindrome Partitioning).

`solveNQueens` returns every valid board as a `vector<vector<string>>` (each solution is `n` strings of `'Q'`/`'.'` characters), matching the shape LeetCode 51 expects, so [problems/01-n-queens.cpp](problems/01-n-queens.cpp) is essentially a standalone restatement of this same function with its own test suite.

## Code Walkthrough

**`isSafe`** (in [code.cpp](code.cpp)). The constraint check. Given the columns already chosen for every row before the current one (`colOfRow[0..row-1]`), it answers whether placing a queen at `(row, col)` would share a column or a diagonal with any of them. It deliberately never checks for a shared *row*, because the recursion places exactly one queen per row by construction — two placed queens can never already occupy the same row. This function is called **before** any mutation happens, which is what makes it a prune rather than a post-hoc validity check.

**`buildBoard`** (in [code.cpp](code.cpp)). A pure formatting helper: converts the compact `colOfRow` representation (one integer per row) into the `vector<string>` board format the problem expects. It has no bearing on the backtracking logic itself — it only runs once a complete, valid solution has been found.

**`backtrack`** (in [code.cpp](code.cpp)). The recursive engine, implementing the Execution Flow above almost line for line. The base case (`row == n`) records a solution and returns. Otherwise, it loops over every candidate column, skips (prunes) any that fails `isSafe`, and for every candidate that passes: **chooses** (`colOfRow[row] = col`), **recurses** (`backtrack(n, row + 1, ...)`), then **undoes** (`colOfRow[row] = -1`) before the loop moves to the next candidate column. The comments in the file explicitly number each of these three steps so the skeleton is impossible to miss.

**`solveNQueens`** (in [code.cpp](code.cpp)). The public entry point: allocates the initial (empty) `colOfRow` state and kicks off the recursion at row 0. This function exists to give the recursive engine a clean, argument-free-of-bookkeeping public interface — callers never see or manage `colOfRow` directly.

**`main`** (in [code.cpp](code.cpp)). Exercises `solveNQueens` against several `n` values with known answers (`n=1` has 1 solution, `n=2` and `n=3` have 0, `n=4` has exactly 2, `n=8` has the standard 92) and prints `[PASS]`/`[FAIL]` for each assertion, plus the two `n=4` boards themselves, proving the implementation compiles and runs correctly end to end.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem, implementing the same choose/recurse/undo skeleton inline (not calling into `code.cpp`, to keep each file dependency-free and independently readable) with problem-specific comments tying every decision back to the general principles established in this README. See [problems/README.md](problems/README.md) for the index and the "why these four" rationale. Briefly: `01` restates the N-Queens skeleton above as a standalone LeetCode 51 solution; `02` applies the identical skeleton to Sudoku's more elaborate row/column/box constraint, with an "undo only on failure" variation; `03` shows the partial state living *inside* the grid itself (an in-place "used" marker), making the undo step's importance vivid; `04` moves the constraint check away from board positions entirely, onto a substring property (palindrome-ness), showing the skeleton generalizes past grids.

## Advantages

- **Eliminates doomed work before it happens, not after.** Pruning at the earliest possible decision point avoids building out entire subtrees that were never going to produce a valid answer — the core efficiency win over generate-then-filter.
- **Finds every valid solution (or one, or the best one) using the same mechanical skeleton.** Whether a problem wants "all solutions," "does any solution exist," or "the first/best solution found," the choose/recurse/undo loop needs only minor adjustment (return early vs. keep searching) — the skeleton itself does not change.
- **Memory-efficient by construction.** Because state is mutated in place and undone rather than copied at every recursive call, the extra memory cost is proportional to the recursion *depth*, not to the number of branches explored — a small, bounded amount of extra memory even for a search that explores millions of candidate branches over its lifetime.
- **The constraint check doubles as documentation of the problem's rules.** `isSafe`, the Sudoku row/column/box check, and the palindrome check each state precisely, in code, what makes a partial solution valid — there is no ambiguity about the rules once the check is written.
- **Composable with problem-specific pruning heuristics.** Nothing about the skeleton prevents adding smarter, tighter pruning (e.g. choosing the most-constrained cell first in Sudoku) on top of the basic choose/recurse/undo loop, which is exactly how production-grade constraint solvers extend this same idea.

## Disadvantages

- **Still exponential in the worst case, even with pruning.** Pruning reduces the constant factor and often the *practically* explored tree dramatically — sometimes by many orders of magnitude, as with N-Queens' 92 solutions out of `8^8` raw placements — but it does not change the asymptotic ceiling. A problem with genuinely little exploitable structure (an adversarial input, or a constraint that rarely fires) can still explore close to the full exponential tree. See [Complexity](#complexity) below for exactly where this bites and where it does not.
- **The "undo" step is easy to forget, and its absence fails silently.** Skipping the undo does not throw an exception or crash the program — it leaves the shared partial-solution state mutated for every sibling branch still to be explored, silently corrupting the search into producing wrong or incomplete answers. This is the single most common bug in backtracking code, precisely because the code still *runs* — it just runs wrong.
- **Getting the constraint check even slightly wrong breaks the entire search.** Too loose, and invalid "solutions" leak into the output. Too tight, and valid solutions are pruned away and silently never found — there is no error message for "you pruned away the answer," only a wrong result.
- **Recursion depth is bounded by problem size, but stack usage still matters.** For very large boards or very long strings, deep recursion can approach stack limits — a real, practical concern distinct from the algorithm's time complexity.

## Tradeoffs

**What we gain versus generate-then-filter (Subsets-style full enumeration + post-hoc validity checking):** dramatically less work explored *in practice*, because invalid branches are cut off at the earliest possible decision point instead of being built out to a complete, doomed leaf. The asymptotic worst-case bound does not improve — both approaches are exponential — but the *constant factor*, and very often the practically-explored fraction of the tree, shrinks enormously (millions of raw placements down to the handful that actually matter, for N-Queens).

**What we gain versus a generic CSP/SAT/ILP solver:** zero external dependencies and a solution sized appropriately to the problem — a hand-rolled `isSafe` check plus a small recursive function, rather than integrating and configuring a general-purpose solver library for a board that fits in a `vector<vector<string>>`.

**What we lose:** correctness now depends entirely on two things the programmer must get exactly right — the constraint check's precision, and the undo step's completeness — neither of which is enforced by the compiler or the type system. A generic solver library, by contrast, has had its correctness properties independently verified across many more edge cases than a hand-written check typically covers.

## Complexity

**Time (general):** exponential in the worst case — the raw decision tree, before any pruning, has a branching factor tied to the number of candidates per decision slot, raised to the number of decision slots. Pruning does not change this worst-case bound; it changes how much of that tree is *actually explored* for a given input.

**How pruning changes practice, concretely:**
- **N-Queens (`n=8`):** raw "one queen per row, no constraint" placements: `8^8` = 16,777,216. With column/diagonal pruning, the search explores only a small fraction of that before finding all 92 valid boards — the practical runtime is effectively instant, even though the *asymptotic* worst case (for general `n`) remains exponential (specifically bounded by `O(n!)`, since pruning enforces "one queen per column" implicitly by construction, collapsing the raw `n^n` space down to at most `n!` row/column permutations before diagonal pruning even starts).
- **Sudoku:** worst case is `O(9^m)` for `m` empty cells — astronomically large for `m` near 81. In practice, row/column/box pruning leaves only 1-3 plausible digits per empty cell on a realistically-constrained puzzle, so real Sudoku puzzles solve near-instantly despite the exponential bound.
- **Word Search:** worst case is `O(m * n * 4^L)` for an `L`-letter word on an `m x n` grid. In practice, the very first letter-mismatch or already-visited check prunes almost every branch within one or two steps for words that are not actually present.
- **Palindrome Partitioning:** worst case is `O(n * 2^n)`. Unlike the problems above, this one has a genuinely adversarial input that reaches close to that ceiling: a string of all-identical characters makes *every* substring a palindrome, so there is nothing to prune, and the algorithm enumerates close to the full `2^(n-1)` partitions. This is the clearest example in this module of pruning's limits — it only helps when the constraint actually rules out candidates; if the constraint is satisfied almost everywhere, backtracking degrades toward full enumeration.

**Space:** O(depth) for the recursion stack in every case discussed here (`n` for N-Queens, `m` for Sudoku's empty cells, `L` for Word Search's word length, `n` for Palindrome Partitioning's string length), plus whatever space the output solutions themselves require (which can itself be exponential if there are exponentially many valid solutions to return).

**Contrast with Subsets-style full enumeration + post-hoc filtering:** for a problem that *does* have exploitable constraints (N-Queens, Sudoku, Word Search), full enumeration followed by filtering pays the complete cost of the raw combination space regardless of how quickly a given branch becomes invalid — there is no early exit. Backtracking's early pruning is the entire difference between "instant" and "does not finish" on realistically-sized inputs for exactly these problems.

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

## Real Interview/Production Examples

Backtracking is a mainstay of technical interviews specifically because N-Queens, Sudoku, and Word Search are compact enough to code in 30-40 minutes while still requiring the candidate to articulate a precise constraint check and correctly reason about the undo step — a genuine test of whether "I understand recursion" extends to "I understand recursion over shared, mutated state."

Beyond interviews, the same choose/recurse/undo idea with early pruning shows up directly in production and tooling contexts:

- **Constraint solvers** embedded in scheduling, configuration, and resource-allocation tools use backtracking (often augmented with much more sophisticated pruning heuristics — constraint propagation, most-constrained-variable ordering) as their core search strategy.
- **Puzzle generators and solvers** (Sudoku generators that must verify a puzzle has a *unique* solution, crossword-construction tools, logic-puzzle solvers) run a backtracking search as their correctness-verification step, not just for solving a single instance.
- **Configuration validators that must satisfy mutual-exclusion rules** — feature-flag systems, package/dependency resolvers checking for conflicting version constraints, infrastructure-as-code validators checking that a proposed set of resource settings does not violate mutually-exclusive policy rules — use the same "choose a setting, check it against everything chosen so far, undo if it conflicts" search when they need to find a valid combination rather than just validate one proposed combination.
- **Compilers and type systems** occasionally use backtracking-style search during type inference or overload resolution, trying candidate types/overloads and backing out when a choice leads to a contradiction elsewhere in the expression.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **A feature-flag or configuration combination validator** that, given mutual-exclusion and prerequisite rules between flags, enumerates every valid enabled-flag combination for QA to test before a release (see the Real-world Challenge in [exercises.md](exercises.md)).
2. **A test-data generator for constrained schemas** — given a set of field-level constraints that interact (e.g. "if `country` is `US`, `state` must be one of 50 values; if `plan` is `enterprise`, `seat_count` must be `>= 10`"), backtracking search can enumerate valid combinations to use as test fixtures, instead of hand-writing every case.
3. **A dependency/version conflict resolver prototype** — given a set of packages each with a list of compatible version ranges for their dependencies, a backtracking search over "choose a version for each package, check compatibility against already-chosen versions, undo on conflict" mirrors (at a small scale) what real package managers' resolvers do.
4. **A crossword or word-placement puzzle generator** for an internal tool or game feature — placing words onto a grid such that intersecting letters match is a direct Word-Search-style backtracking search run in the "generate" direction instead of the "verify" direction.
5. **A rule-based access-control combination checker** — given a set of roles/permissions with mutual-exclusion rules (separation-of-duty constraints, common in financial and healthcare systems: "the same person cannot both approve and submit a transaction"), backtracking can enumerate valid role-assignment combinations for an audit tool.

## Similar Patterns

- **[Subsets](../subsets/):** shares the exact same recursive choose/recurse(/undo) skeleton, but with **no constraint to prune on** — every leaf of the tree is valid output, so the entire search is enumerated rather than pruned. Backtracking is best understood as Subsets *plus* a constraint check that lets you cut off invalid branches early; Subsets is the special case where that check would always pass. See the [family README](../README.md) for the "how to tell them apart" table.
- **[Dynamic Programming](../../dynamic-programming-patterns/):** also explores a decision tree of choices, but targets problems where the same sub-state recurs across multiple different paths (**overlapping subproblems**) and where only an optimal value or count is needed, not every distinct arrangement. Where Backtracking re-derives every branch independently (because it typically needs the actual arrangements, and sub-states rarely repeat usefully across branches), DP caches a sub-state's answer once and reuses it, trading the ability to reconstruct every arrangement for a often-dramatic reduction in redundant work.
- **Brute-force recursion / generate-then-filter:** structurally the same recursive tree as backtracking, but without early pruning — every branch is built to full depth before a single validity check at the leaf decides whether to keep it. This is backtracking's direct predecessor conceptually, and exactly what backtracking's pruning step improves upon.
- **Branch and Bound:** a close relative used for *optimization* (not just satisfaction) problems — it prunes branches not only when they violate a hard constraint, but also whenever a computed bound proves the branch cannot possibly beat the best solution found so far. Same choose/recurse/undo skeleton, with an additional numeric bound driving extra pruning.

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
- *"What is the time complexity, and is that bound ever actually reached?"* — expects distinguishing the worst-case asymptotic bound from the practically-explored tree size, and being able to name at least one adversarial input (like Palindrome Partitioning's all-same-character string) where the bound genuinely is approached.
- *"How would you find just one solution instead of all of them?"* — expects recognizing that the recursive function's return type/control flow needs to short-circuit (return `true`/stop immediately) on the first success, rather than continuing to explore sibling candidates.
- *"What happens if you forget to undo?"* — expects the precise answer: not a crash, but silent corruption of the shared state for every sibling branch, producing wrong or incomplete results without any error signal.
- *"When would you NOT use backtracking here?"* — expects recognizing the two off-ramps: no real constraint to prune on (use Subsets), or overlapping subproblems where caching would help (use DP).

Common misconceptions:
- "Backtracking is always fast because of pruning." It reduces the *practically explored* tree size, often dramatically, but the worst-case bound is still exponential, and some inputs (as with all-same-character Palindrome Partitioning) approach that worst case closely.
- "The undo step is optional cleanup, not core logic." It is core logic — without it, the algorithm's correctness for every sibling branch is compromised, not just its tidiness.
- "Backtracking and brute-force recursion are the same thing." They share the recursive skeleton, but backtracking's defining feature is checking constraints *before* recursing (pruning), not after building a complete candidate.
- "If a problem needs 'all valid arrangements,' it must be backtracking." Only if there is an actual constraint to prune on — if there is not, it is Subsets wearing backtracking's clothing (see Exercise 1: Letter Case Permutation).

## Summary

- Backtracking builds a solution one choice at a time and undoes ("backtracks") the instant a partial choice can no longer lead anywhere valid.
- The skeleton at every decision point: check the constraint **before** recursing (prune if it fails), otherwise choose, recurse, then unconditionally undo.
- Its entire value over brute-force generate-then-filter is checking validity **as early as possible**, cutting off doomed subtrees before they are ever built out to a leaf.
- Worst-case time remains exponential even with pruning — pruning changes the practically-explored tree size (often dramatically), not the asymptotic ceiling.
- The most common bug is forgetting the undo step, which silently corrupts shared state for sibling branches rather than crashing loudly.
- No constraint to prune on -> that is Subsets. Overlapping subproblems where only an optimal value/count is needed -> that is Dynamic Programming. Backtracking sits precisely between those two neighbors.
- Real production uses: constraint solvers, puzzle generators/verifiers, configuration/feature-flag validators with mutual-exclusion rules, dependency-resolver prototypes.

## Key Takeaways

1. Backtracking's skeleton is choose -> recurse -> undo, guarded by a constraint check that runs **before** choosing, so invalid branches are pruned rather than explored to a doomed leaf.
2. The undo step must be unconditional — it runs whether the recursive call succeeded, failed, or exhausted every possibility beneath it.
3. Pruning does not change the asymptotic worst case (still exponential); it changes how much of the tree is actually explored for realistic inputs, often by many orders of magnitude.
4. Some inputs are adversarial with respect to pruning (an all-same-character string for Palindrome Partitioning) and genuinely approach the exponential worst case — pruning is only as good as how often the constraint actually rules something out.
5. Forgetting to undo is the single most common and most damaging mistake: it silently corrupts shared state for sibling branches rather than crashing.
6. Checking constraints too late (post-hoc, at a fully-built leaf) reintroduces the generate-then-filter performance problem, even though the answer stays correct.
7. No constraint to prune on -> reach for Subsets instead. Overlapping subproblems needing only an optimal value/count -> reach for Dynamic Programming instead.
8. Decide up front whether you need one solution, all solutions, or the best solution — the base case's control flow must match that choice.
9. Real production analogues: constraint solvers, puzzle generators/verifiers, configuration validators with mutual-exclusion rules — not just interview puzzles.
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
