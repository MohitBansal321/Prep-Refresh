# Subsets

## Intent

Systematically enumerate every subset (or combination, or permutation) of a given set of elements, using either an iterative "double the results so far" strategy or a recursive "include or exclude each element" strategy — with no pruning, because every possible answer is valid output.

## Real Life Analogy

Think of **deciding which toppings to include on a pizza**. You are standing at the counter looking at a list of toppings — pepperoni, mushroom, olives, onions — and for each one you make an independent yes/no decision: "do I want this topping or not?" You are not choosing an *order* (pepperoni-then-mushroom is the same pizza as mushroom-then-pepperoni), and you are not ruling any combination out in advance — a pizza with all four toppings is just as valid an order as a plain cheese pizza with none of them.

If there are 4 toppings, there are exactly `2^4 = 16` possible pizzas you could order, because each of the 4 independent yes/no choices doubles the number of distinct pizzas: with 0 toppings there is 1 possible pizza (plain), with 1 available topping there are 2 (plain, or with it), with 2 available toppings there are 4, and so on. Every one of those 16 combinations is a *legitimate* pizza someone might order — there is no rule that says "you can't have olives and onions together," so nothing gets discarded along the way. You are not searching for the *best* pizza or checking whether a combination is *allowed*; you are just listing all of them.

That is exactly what the Subsets pattern does with a list of elements instead of toppings: it walks through the elements one at a time, and for each one, doubles the running list of "combinations so far" — once without the new element, once with it — until every element has been considered and every combination has been listed.

## Problem

### What engineering problem exists?

A recurring shape in both interviews and real systems is: **given a small collection of items, produce every subset, every combination, or every ordering (permutation) of them.** Concretely:

- **Subsets / the power set:** given `{1, 2, 3}`, produce all `2^3 = 8` subsets: `{}`, `{1}`, `{2}`, `{3}`, `{1,2}`, `{1,3}`, `{2,3}`, `{1,2,3}`.
- **Combinations:** a size-constrained variant — subsets of exactly length `k`, or subsets whose elements sum to some target.
- **Permutations:** every distinct *ordering* of the elements — `{1,2,3}` has `3! = 6` orderings: `[1,2,3]`, `[1,3,2]`, `[2,1,3]`, `[2,3,1]`, `[3,1,2]`, `[3,2,1]`.

> **Term: Power set.** The set of *all* subsets of a set `S`, including the empty set `{}` and `S` itself. If `S` has `n` elements, its power set has exactly `2^n` elements — one for every possible yes/no combination of "is this element in the subset."

The defining feature of this problem shape, and the reason it deserves its own pattern rather than being folded into "just recursion": **the output itself is exponential in size.** For `n` elements there are `2^n` subsets (or `n!` permutations). This is fundamentally different from most patterns in this repo, where the *algorithm's* time complexity is the thing under the microscope. Here, even a perfect, zero-waste algorithm still has to spend `O(2^n)` time and space, because that is the size of the answer you were asked to produce — there is no clever trick that returns 8 subsets of a 3-element set in fewer than 8 print statements.

### Why is this problem difficult?

- **The "hard part" is not complexity trickery — it is systematic coverage.** Since the output is unavoidably exponential, you cannot optimize your way to a smaller answer. The actual engineering challenge is generating *exactly* the right `2^n` (or `n!`) combinations: no duplicates, no omissions, and (when the input itself has duplicate values, as in "Subsets II") no duplicate *subsets* even though the underlying algorithm naturally wants to treat every array position as distinct.
- **It is easy to double-count or under-count without a systematic rule.** If you try to generate subsets "by hand" with ad-hoc logic (see Why Not Other Approaches below), it is very easy to either miss a combination (e.g., forgetting the empty set) or emit the same combination twice (e.g., not distinguishing "chose element at index 2" from "chose the value 5, which happens to live at index 2 *and* index 5").
- **You must decide, and be consistent about, whether order matters.** `{1,2}` and `{2,1}` are the *same* subset but *different* permutations. Conflating "generate all subsets" logic with "generate all permutations" logic (or vice versa) is one of the most common ways this pattern goes wrong under interview pressure.

### What happens if we ignore it?

- **Missing or duplicate answers ship to production.** A combinatorial test-generation tool that silently skips the empty combination, or emits the same feature-flag combination twice, produces an incomplete or wasteful test matrix — either a real configuration never gets tested, or CI time is wasted re-testing the same configuration under a different name.
- **Attempting this on inputs that are "too large" silently locks up a service.** If a caller passes `n = 40` to a naive subset-generation function expecting `n <= 20` or so, `2^40` (roughly a trillion) subsets will exhaust memory or run for an intractable amount of time — not a bug in the algorithm, but a missing input-size guard that should have redirected to a different approach entirely (sampling, or a problem reformulation that does not require materializing every subset).

## Why Not Other Approaches?

**"Write out nested loops by hand for a fixed number of elements."**
For exactly 3 elements you could write `for a in {yes,no} for b in {yes,no} for c in {yes,no}` — 3 nested loops, one per element, each branching on "include or not." This works, but only for the *specific, hardcoded* `n` you wrote loops for. The moment the input has 4, 5, or `n` elements (which is the normal case — you rarely know `n` in advance in a real function), you would need to rewrite the loop nest every time. This is not a general algorithm; it is a demonstration that does not generalize, and it is the reason ad-hoc nested loops are a trap rather than a technique.

**"Enumerate bitmasks from 0 to 2^n - 1."**
This is a genuinely valid alternative worth knowing, not a straw man: since each subset corresponds to exactly one yes/no pattern across `n` elements, you can walk every integer `mask` from `0` to `2^n - 1` and read off bit `i` of `mask` to decide whether element `i` is included. It is iterative, requires no recursion, and is very fast in practice because it is just integer arithmetic and bit tests. Its downside is that it is capped by the machine's integer width (`n` must fit in 32 or 64 bits, which is a non-issue since `n > ~25` is already intractable for other reasons) and it is arguably less readable to someone who has not seen the bit-trick before — the "include/exclude" framing in this module's Solution section is the more teachable, more directly generalizable-to-Backtracking mental model, which is why it is the primary approach here. Bitmask enumeration remains a good tool to reach for when you want a compact, non-recursive, cache-friendly implementation and the audience is comfortable with bit manipulation.

**"Just recurse without a clear include/exclude structure, hoping it works out."**
Recursion is indeed the right tool, but recursing without a precise contract (what does this call return? what has already been decided by the time we reach this call?) is how duplicate or missing subsets happen. The point of this module is to give that contract a name — "at each element, explore the branch where you include it and the branch where you exclude it" — so it is impossible to lose track of which elements have already been decided.

**Tradeoff summary:** ad-hoc nested loops do not generalize past the exact `n` they were written for. Bitmask enumeration is a real, competitive alternative — iterative, no recursion, very fast — but it is capped by integer width and is a less direct stepping stone to Backtracking (which is the whole reason Subsets is taught first in this family). The include/exclude recursion and the iterative-doubling approach covered below are preferred here because they scale to arbitrary `n` (up to the point where `2^n` itself becomes intractable, which is a property of the *problem*, not the algorithm) and because the recursive framing is the direct ancestor of every Backtracking template in the next module.

## Solution

There are two equivalent ways to think about generating all subsets, and recognizing that they produce the *same* answer via different mechanics is itself a valuable insight.

**(a) Iterative "doubling" (BFS-style).** Start with a list containing just the empty subset: `[[]]`. Then, for each new element in the input, take *every subset already in the list* and make a duplicate copy of it with the new element appended — and add all those new copies into the list. After processing all `n` elements, the list has grown from 1 subset to `2^n` subsets, because every element doubled the list exactly once. This is called "BFS-style" because it builds the answer breadth-first, by *size* of the growing collection, layer by layer, rather than by following one decision path all the way to the end before backing up.

**(b) Recursive "include or exclude" (DFS-style).** Walk through the elements one at a time (say, by index). At each element, make **two** recursive calls: one where you *include* the current element in the subset being built, and one where you *exclude* it. When you have made a decision for every element (the recursion reaches the end of the input), the subset you have built at that point is one complete, valid answer — record it. This is called "DFS-style" because it commits to a full sequence of include/exclude decisions (one root-to-leaf path down a binary decision tree) before backtracking to try the sibling decision.

Both approaches visit exactly `2^n` "leaves" (complete subsets) — the iterative version builds them layer by layer across the whole collection at once; the recursive version builds them one full decision-path at a time, depth-first. Neither is "more correct" than the other; they are the same enumeration expressed with a different control-flow shape, and this recursive include/exclude framing is *exactly* the skeleton that Backtracking (the next module in this family) builds on by adding a validity check and an early-exit ("prune") step.

## Architecture

**Participants in the iterative-doubling version:**

1. **The growing list of subsets (`result`).** Starts as `[[]]` — just the empty subset — and only ever grows. Its invariant after processing the first `k` elements: it contains *exactly* the `2^k` subsets of those first `k` elements, and nothing else.
2. **The current element being processed.** On each pass, this is the one new piece of information being folded into every existing subset.
3. **The "snapshot the current size" step.** Before appending anything, the size of `result` *before this element's pass* must be captured, because `result` is being appended to *while iterating over it* — iterating over a live, growing list without snapshotting its starting length would re-process the subsets you just added, corrupting the doubling into an infinite (or wildly over-counted) growth. This is the single most important implementation detail of the iterative version (see Common Mistakes).

**Participants in the recursive include/exclude version:**

1. **The input array/vector (`nums`), read-only.** Never mutated; only ever read by index.
2. **The current index (`i`), the recursion's progress marker.** Its invariant: every index `< i` has already had an include/exclude decision made for it (baked into the current path); every index `>= i` is still undecided.
3. **The path/subset being built so far (`current`).** A single, mutable, shared buffer representing "the subset as decided along this specific root-to-leaf path." It is **pushed to** before the include-branch's recursive call and **popped from** immediately after that call returns (before the exclude-branch runs) — this push/pop discipline is exactly backtracking's "undo the choice" step, applied here even though there is no constraint to check yet.
4. **The results collection (`result`).** Where a *copy* of `current` is recorded whenever the recursion reaches a base case (`i == n`, meaning every element has been decided). Copying (not storing a reference to) `current` matters, because `current` keeps being mutated by later branches of the recursion.
5. **The recursion tree (implicit participant).** Every internal node represents "some elements decided, some remaining"; every node has exactly two children (include, exclude) except the leaves (`i == n`), where there are `2^n` leaves total — one per complete subset.

Responsibilities in one line each:
- **Iterative version — `result` list:** the single source of truth, doubling in size once per input element.
- **Iterative version — snapshot length:** prevents the growing list from accidentally processing its own freshly-added entries.
- **Recursive version — `current` path buffer:** represents "the decision made so far along one path"; pushed and popped, never left dirty across branches.
- **Recursive version — `i` index:** the recursion's sense of "how much of the input has been decided."
- **Recursive version — `result`:** accumulates a snapshot of `current` at every leaf of the decision tree.

## Execution Flow

**Iterative doubling**, step by step:

1. Initialize `result = [[]]` — the list of subsets built so far, containing only the empty subset.
2. For each element `x` in the input, in order:
   a. Record `n = result.size()` — the number of subsets that exist *before* processing `x`. This must be captured before the inner loop starts.
   b. For `i` from `0` to `n - 1`: take `result[i]` (an existing subset), make a copy of it, append `x` to the copy, and append that new subset to `result`.
   c. After this inner loop, `result` has exactly doubled: it now contains every subset that existed before `x` (unchanged) *plus* every one of those same subsets with `x` added.
3. After all elements have been processed, `result` contains all `2^n` subsets of the input.

**Recursive include/exclude**, step by step:

1. Define a recursive helper that takes the current index `i` and the shared "path so far" buffer `current`.
2. Base case: if `i == nums.size()` (every element has been decided), append a **copy** of `current` to the results list and return — this is one complete subset.
3. Otherwise (element `nums[i]` is still undecided), make the **exclude** decision first: recurse to `i + 1` with `current` unchanged (do not push `nums[i]`).
4. When that recursive call returns, make the **include** decision: push `nums[i]` onto `current`, recurse to `i + 1`, and — critically — pop `nums[i]` off `current` immediately after that call returns, restoring `current` to exactly what it was before step 4 began.
5. Return from this call. The order of steps 3 and 4 (exclude-then-include, or include-then-exclude) does not affect correctness, only the order the subsets appear in the final result — both orderings visit the same `2^n` leaves.
6. The top-level call starts the recursion at `i = 0` with an empty `current` and an empty `result`; after it returns, `result` holds all `2^n` subsets.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the flowchart deciding between Subsets (no pruning needed, enumerate everything) and Backtracking (a constraint should stop invalid branches early), based on the signals in a problem statement.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the recursion tree of include/exclude decisions for the recursive DFS-style approach, showing every branch down to the `2^n` leaves for a small concrete input.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of the iterative-doubling process building up all 8 subsets of `{1, 2, 3}`, showing exactly how the result list doubles after each element.

## Implementation

[code.cpp](code.cpp) provides three small, generic, reusable functions, deliberately kept independent of any one specific LeetCode problem so the *shape* of the pattern is visible on its own:

- `subsetsIterative(nums)` — the BFS-style doubling approach, implemented iteratively with no recursion.
- `subsetsRecursive(nums)` — the DFS-style include/exclude approach, implemented with a recursive helper and an explicit push/pop on the shared path buffer.
- `permutations(nums)` — included for completeness alongside subsets/combinations, since "enumerate every ordering" is the third member of the same family of exhaustive-enumeration problems; implemented with the same push/pop (choose → recurse → undo) discipline, tracking which elements have already been used along the current path.

All three are written as function templates (`template <typename T>`) over `std::vector<T>` so the same logic works whether the caller passes `vector<int>`, `vector<char>`, or `vector<std::string>` — the pointer/index bookkeeping is what matters, not the element type. The worked, problem-specific solutions (including the duplicate-handling variants) live in [problems/](problems/).

## Code Walkthrough

**`subsetsIterative`** (in [code.cpp](code.cpp)). Starts `result` as a single-element list containing the empty subset. For each input element, it snapshots `result.size()` into a local `existingCount` *before* the inner loop that appends new subsets — this is the detail called out in Architecture and Execution Flow: without snapshotting, appending to `result` while iterating over `result.size()` (re-evaluated every iteration) would immediately re-process the newly added subsets, corrupting the doubling. This function exists to demonstrate the non-recursive, layer-by-layer construction and is the direct ancestor of [problems/01-subsets.cpp](problems/01-subsets.cpp).

**`subsetsRecursive`** (in [code.cpp](code.cpp)). A public entry point that sets up an empty `result` and an empty `current` buffer, then calls a private recursive helper starting at index 0. The helper implements the base case (`i == n` → record a copy of `current`) and the two branches (exclude: recurse without modifying `current`; include: `push_back`, recurse, `pop_back`) exactly as described in Execution Flow. This function exists to demonstrate the DFS include/exclude framing that Backtracking extends with a validity check.

**`permutations`** (in [code.cpp](code.cpp)). Uses a `std::vector<bool> used` parallel to `nums`, marking which elements have already been placed along the current path (needed because, unlike subsets, permutations care about *every* element appearing exactly once per output, in every possible order, not about a yes/no inclusion decision per element). At each recursive step, it loops over every *unused* index, marks it used, pushes its value onto `current`, recurses, then unmarks it and pops it — the same choose/recurse/undo discipline as `subsetsRecursive`, generalized from "2 branches per level" to "up to `n` branches per level." Included for completeness because permutations are the natural third member of the subsets/combinations/permutations family, and because the choose/recurse/undo shape it demonstrates is precisely what Backtracking is built from.

**`main()`** (in [code.cpp](code.cpp)). Exercises all three functions against small, hand-checkable inputs (`{1,2,3}` for subsets, `{1,2,3}` for permutations) and prints `[PASS]`/`[FAIL]` for each assertion — including a check that `subsetsIterative` and `subsetsRecursive` produce the same *set* of subsets (order may differ) for the same input, proving the two framings from the Solution section are genuinely equivalent.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem, implementing the same include/exclude or choose/recurse/undo logic inline (not calling the generic templates above, to keep each file dependency-free and independently readable). See [problems/README.md](problems/README.md) for the index. Briefly: `01` is the pure no-duplicates subsets problem; `02` adds duplicate-input-element handling (sort + skip same-level duplicates); `03` is pure permutation generation; `04` extends the choose/recurse/undo skeleton with target-sum pruning and *unbounded reuse* of each candidate — the first real taste of what Backtracking adds on top of this module's foundation.

## Advantages

- **Conceptually simple, provably complete.** Both framings (iterative doubling, recursive include/exclude) have a one-sentence correctness argument: every element gets exactly one yes/no decision, so every combination of decisions — and therefore every subset — is produced exactly once.
- **No wasted work relative to the size of the answer.** Because the answer itself is exponential, an algorithm that touches `O(2^n * n)` cells to *produce* `O(2^n)` outputs of size up to `n` each is not wasteful — it is doing the minimum work the problem requires.
- **Directly generalizes to Backtracking.** The recursive include/exclude skeleton (and the choose/recurse/undo skeleton behind `permutations`) is *exactly* the shape Backtracking reuses, adding only a validity check and an early return. Learning this pattern well makes the next one almost free.
- **Two independent implementations to cross-check each other.** Because the iterative and recursive approaches are conceptually different but must produce the same answer, they serve as a built-in correctness check on each other — useful both for learning and for testing a real implementation.
- **No pruning logic to get wrong.** Since every leaf of the decision tree is valid output, there is no constraint-checking code that could have a subtle bug — the only correctness risks are the mechanical ones covered in Common Mistakes.

## Disadvantages

- **Exponential time and space is unavoidable — but this is inherent to the problem, not a flaw in the technique.** No algorithm can enumerate `2^n` subsets in better than `O(2^n)` time, because that many distinct outputs must be produced. This is fundamentally different from, say, a brute-force `O(n^2)` algorithm that a cleverer `O(n)` algorithm can beat — here there is no cleverer algorithm to reach for, because the *output itself* is exponential.
- **This means the pattern only scales to small `n`** — roughly `n <= 20` to `25` in practice. `2^20` is about a million (comfortable), `2^25` is about 33 million (borderline, several seconds and real memory pressure), and `2^30` is over a billion (intractable for almost any real service). If a problem's constraints allow `n` in the hundreds or thousands, "enumerate every subset" is the wrong approach *regardless of implementation quality* — the problem needs a different formulation entirely (often Dynamic Programming, which answers questions *about* the set of subsets — like "does one sum to X" — without ever materializing all of them).
- **Recursion depth is `O(n)`**, not a concern for the `n <= 25`-ish range this pattern is meant for, but worth noting: the recursive version uses call-stack space proportional to `n`, on top of the `O(2^n * n)` needed for the output itself.
- **Duplicate input elements require extra care** (see Common Mistakes) — the base recursive/iterative logic silently over-generates duplicate *subsets* when the input has repeated values, unless explicitly guarded against.

## Tradeoffs

**What we gain:** a small, provably-complete, easy-to-explain mechanism for producing every subset/combination/permutation of a small set, in the minimum time the problem's exponential output size allows, with an implementation that generalizes directly into Backtracking.

**What we lose:** applicability to anything but small `n`. There is no way to "optimize" past the `2^n` (or `n!`) output size — accepting that ceiling is the whole deal. We also lose the ability to stop early: because every leaf is valid output, there is no "prune this branch, it can't work out" logic available here (that is precisely what Backtracking adds, at the cost of losing the "no pruning to get wrong" simplicity above).

**Iterative doubling vs. recursive include/exclude, specifically:** the iterative version avoids recursion-stack overhead and is easy to reason about layer-by-layer, but requires careful snapshotting of the list's length mid-loop (a real, common bug source). The recursive version has a natural place to add a validity check later (the direct on-ramp to Backtracking) and mirrors how most engineers naturally think about "make a choice, then decide the rest" — but it pays call-stack overhead proportional to `n` and requires disciplined push/pop of the shared path buffer.

## Complexity

**Time: `O(2^n * n)`.** There are `2^n` subsets total. Each subset can be up to length `n`, and producing/copying/printing it costs `O(subset length)` — in the worst case (and on average, since subset lengths are binomially distributed around `n/2`), this contributes an extra factor of `n`. This holds for both the iterative and recursive framings; they visit the same `2^n` leaves and do the same `O(n)` work of assembling/copying each one.

**Space: `O(2^n * n)`** for the output itself (the list of `2^n` subsets, each up to length `n`) — this is unavoidable, since that is the size of what was asked for. The recursive version additionally uses `O(n)` auxiliary space for the recursion call stack and the shared `current` path buffer, which is dwarfed by the `O(2^n * n)` output cost for any `n` where this pattern is appropriate.

**Permutations, for contrast:** `O(n! * n)` time and space — `n!` permutations, each of length `n`. This grows even faster than `2^n` (`10! = 3,628,800` versus `2^10 = 1,024`), which is why "enumerate every permutation" becomes intractable at a noticeably *smaller* `n` than "enumerate every subset" — a fact worth stating explicitly in an interview if asked to compare the two.

## Common Mistakes

- **Handling duplicate input elements incorrectly.** If the input array has repeated values (e.g., `[1, 2, 2]`) and you run the plain include/exclude recursion without change, you get duplicate *subsets* in the output (`{2}` appears twice — once from "include index 1," once from "include index 2," even though the value is identical both times). *Avoid:* sort the input first, then, within the loop that considers "what to include next" at a given recursion level, skip over any value that is equal to the immediately preceding one *at that same level* (`if (i > start && nums[i] == nums[i-1]) continue;`). This is precisely what [problems/02-subsets-ii.cpp](problems/02-subsets-ii.cpp) implements — see its header comment for the exact reasoning about "same level" versus "same branch."
- **Off-by-one in loop bounds during iterative doubling.** The classic bug: writing `for (int i = 0; i < result.size(); ++i)` for the inner "duplicate and extend" loop, where `result.size()` is re-evaluated on every iteration (because `result` is being appended to *inside* the loop). This causes the loop to immediately start processing the subsets it just added, corrupting the doubling into runaway growth (or, depending on exact logic, an infinite loop). *Avoid:* capture `size_t existingCount = result.size();` once, before the inner loop starts, and loop `for (size_t i = 0; i < existingCount; ++i)`.
- **Forgetting the empty subset as a valid answer.** The empty subset `{}` is a legitimate member of the power set — LeetCode's "Subsets" (78) explicitly expects it in the output. Initializing `result` to an empty list instead of `{{}}` (iterative version), or forgetting to record `current` at the `i == n` base case even when `current` is empty (recursive version), silently drops this one required answer. *Avoid:* always start the iterative `result` as `{{}}`, and always record the base case regardless of whether `current` happens to be empty at that point.
- **Copying the shared path buffer at the wrong time (recursive version).** Appending `current` to `result` by reference, or appending it *before* a subsequent `push_back`/`pop_back` pair has settled, records a subset that later gets silently mutated as the recursion continues down other branches. *Avoid:* always append an explicit *copy* (`result.push_back(current);`, which copies a `std::vector` by value) at the moment the base case is reached, not a reference or pointer to the shared buffer.
- **Conflating subsets with permutations.** Writing subset-style include/exclude logic when the problem actually wants every *ordering* (or vice versa) produces answers of the wrong count and shape (`2^n` subset-shaped answers instead of `n!` ordering-shaped answers, or vice versa). *Avoid:* before writing any code, ask explicitly "does the order of elements within one answer matter?" — if yes, it is a permutation problem; if no, it is a subset/combination problem.

## When To Use

- The problem explicitly asks for **all subsets, all combinations, or all permutations** of a small collection, with no constraint that would let you rule out a partial answer early.
- You need to **exhaustively enumerate every possible configuration** of a small number of independent, binary (or small-domain) choices — feature flags, boolean settings, small option sets.
- You need a **brute-force baseline** to verify a cleverer algorithm's output against, on small test inputs, during development or in a test suite.
- `n` is small enough that `2^n` (or `n!`) is a number you are comfortable materializing in memory — roughly `n <= 20-25` for subsets, noticeably smaller for permutations.
- You are about to learn or teach **Backtracking**, and want the unconstrained "enumerate everything" skeleton fully understood first, so the *only* new idea when Backtracking is introduced is "add a validity check and prune."

## When NOT To Use

- **`n` is large enough that `2^n` (or `n!`) subsets/permutations is intractable.** If constraints allow `n` in the hundreds or thousands, materializing every subset is not just slow — it is a fundamentally wrong approach; look toward Dynamic Programming (to answer a *question about* the subsets, like "does one sum to X," without enumerating them) or a problem reformulation.
- **There is a pruning constraint that should stop you early.** If the problem says something like "each queen must not attack another," "each cell can only be visited once," or "the partial sum must not exceed the target" — a constraint that can invalidate a *partial* answer before it is even complete — that is **Backtracking**'s job, not Subsets'. Running the unconstrained enumeration and filtering invalid results *afterward* wastes enormous amounts of work exploring branches that could have been abandoned immediately; see [../backtracking/README.md](../backtracking/README.md).
- **You only need to know whether *some* subset satisfies a property, not all of them.** If the question is "does any subset sum to exactly `k`," full enumeration is far more work than necessary — a Dynamic Programming subset-sum formulation answers that in pseudo-polynomial time without ever listing the subsets.
- **The order of elements matters and you are using subset logic, or vice versa.** Reaching for include/exclude when the real question is about orderings (or reaching for permutation logic when the real question is about unordered groups) produces answers of the wrong shape entirely — always re-confirm this before choosing which template to reach for.

## Real Interview/Production Examples

Subsets, combinations, and permutations ("Subsets," "Subsets II," "Permutations," "Combinations," "Combination Sum") are staple interview questions specifically because they test whether a candidate can hold two mutually recursive branches straight (include vs. exclude) and correctly reason about duplicate-avoidance — a small but precise piece of correctness reasoning that is easy to get subtly wrong under pressure.

Beyond interviews, the same enumeration shape shows up in real engineering work:

- **Feature-flag combination testing.** A service with a handful of independent boolean feature flags needs its test suite (or a QA smoke-test matrix) to exercise every combination of flags on/off before a release — exactly the power-set enumeration this module implements, just over flags instead of numbers.
- **Generating all valid configurations for a small settings panel.** A configuration UI with a handful of independent toggles (e.g., "dark mode," "compact view," "notifications") may need to precompute or validate every possible resulting configuration state during testing or documentation generation.
- **Combinatorial test-case generation.** Property-based and combinatorial testing tools (e.g., pairwise/all-combinations test generators used in QA tooling) generate the Cartesian product or power set of a small number of input dimensions to guarantee coverage of every combination, not just a sampled subset.
- **Small-scale query/plan enumeration.** A database query optimizer choosing a join order for a *small* number of tables (a handful, not dozens) may exhaustively enumerate every join ordering (a permutation problem) to find the cheapest plan, before falling back to heuristics once the table count makes exhaustive enumeration too expensive.
- **Product/bundle configurator combinatorics.** An e-commerce "build your own bundle" feature with a small number of optional add-ons may need to enumerate every valid bundle combination to precompute pricing or validate inventory constraints ahead of time.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **A feature-flag test matrix generator.** Given the small set of boolean flags a service currently exposes, generate every on/off combination and drive an integration-test run against each one in CI, guaranteeing full combinatorial coverage rather than a handful of hand-picked scenarios.
2. **A settings-validation script.** For an admin panel with a small number of independent toggles, enumerate every resulting configuration and run each one through a validation function to catch invalid *combinations* of otherwise individually-valid settings before shipping a new toggle.
3. **A brute-force correctness oracle in tests.** When implementing a cleverer algorithm (say, a Dynamic Programming subset-sum solver), write a small brute-force subset enumerator as a "ground truth" to assert the fast algorithm's answer against, on small hand-crafted test inputs — a very common and very effective testing technique.
4. **A small-N deployment-ordering explorer.** When a handful of independent services must be deployed and the safe orderings are not yet known, enumerate every permutation of deployment order for a *small* number of services and simulate/validate each one to find which orderings are safe, before encoding the discovered constraint into an actual dependency graph.
5. **A combinatorial pricing/bundle validator for a product catalog.** For a "choose any subset of these add-ons" product feature with a small number of add-ons, enumerate every subset to precompute valid price tiers or to verify that no combination violates a business rule (e.g., "these two add-ons cannot both be selected") before the UI ever offers them together.

## Similar Patterns

- **Backtracking** ([../backtracking/](../backtracking/)): builds on the *exact same* recursive include/exclude (or choose/recurse/undo) skeleton, but adds a validity check at every step and abandons ("prunes") a branch the moment it can no longer lead to a valid answer. Where Subsets explores every leaf of the decision tree because every leaf is valid output, Backtracking explores only the leaves that survive the constraint — often visiting a tiny fraction of the full `2^n`/`n!` tree. Subsets is, in a very real sense, Backtracking with the pruning check always returning "still valid."
- **Dynamic Programming (subset-sum family):** answers *questions about* the set of subsets (does one sum to exactly `k`? how many subsets sum to at most `k`?) in pseudo-polynomial time, *without* ever materializing all `2^n` subsets. Reach for this instead of Subsets whenever the question is "does at least one subset satisfy X," not "list every subset."
- **Combinatorial generation via bitmasking:** the iterative alternative mentioned in Why Not Other Approaches — walking integers `0` to `2^n - 1` and reading off bits — produces the identical set of subsets as this module's iterative-doubling approach, just via a different (and in some ways more compact) mechanism.

| Pattern | Explores | Pruning | Output size | Primary question answered |
|---|---|---|---|---|
| Subsets (this module) | Every leaf of the include/exclude tree | None — every leaf is valid | `O(2^n)` subsets / `O(n!)` permutations | "List every subset/combination/permutation." |
| Backtracking | Only leaves surviving a constraint | Yes — abandons invalid branches early | Often far smaller than `2^n`/`n!` | "List every *valid* arrangement under constraint X." |
| DP (subset-sum family) | Never materializes individual subsets | N/A — works over aggregated state | `O(1)` (a count/boolean answer, not a list) | "Does some subset satisfy X? How many do?" |

## Interview Discussion

Experienced engineers rarely linger on "can you write the recursion" for this pattern — that is mechanical once you have seen it once. What they actually probe is whether you understand *why* the output is exponential (not "why is your algorithm slow," which is a different and wrong framing), whether you can correctly handle duplicate input elements without over- or under-counting, and whether you can clearly draw the line between this pattern and Backtracking.

Common follow-up questions:
- *"What is the time complexity, and can the algorithm do better?"* — expects the answer "no, because the output itself has `2^n` elements; any algorithm producing all of them is optimal at `O(2^n * n)`," not an attempt to "optimize" something that is already output-bound.
- *"How would you handle duplicate values in the input?"* — expects naming the sort-then-skip-same-level-duplicates technique precisely, and explaining *why* it is "same recursion level," not "same value anywhere in the tree," that must be checked.
- *"Extend this to return only combinations of exactly size k."* — expects recognizing this is the same include/exclude tree with an added early-stop: prune (or simply do not recurse further) once `current.size() == k`, or once too few elements remain to reach size `k` — the first small taste of Backtracking-style pruning bolted onto this exact skeleton.
- *"When would you NOT enumerate every subset?"* — expects recognizing the DP subset-sum reformulation when the actual question is a yes/no or count question, not "list them all."
- *"Compare the iterative and recursive approaches."* — expects the "same `2^n` leaves, different traversal order (breadth-by-element vs. depth-by-path)" framing, plus concrete tradeoffs (recursion stack vs. mid-loop snapshot bug risk).

Common misconceptions:
- "This pattern's time complexity can be improved with a smarter algorithm." It cannot — the exponential cost is the size of the *required output*, not a symptom of a suboptimal method.
- "Skipping duplicates means checking if a value has been used anywhere in the current subset." It specifically means skipping a value that equals its immediate predecessor *at the same recursion level, after sorting* — a materially different (and much easier to get wrong) rule.
- "Subsets and permutations are basically the same problem." They answer different questions (unordered inclusion vs. every ordering) and have different output sizes (`2^n` vs. `n!`) — conflating them produces answers of the wrong shape.
- "Backtracking is just a fancier name for this pattern." Backtracking specifically adds a validity check and early pruning; Subsets, by design, has no pruning at all, because every leaf is valid.

## Summary

- Subsets enumerates every subset/combination/permutation of a small set, via either iterative doubling (BFS-style) or recursive include/exclude (DFS-style) — two equivalent framings of the same `2^n`-leaf decision tree.
- The output is inherently exponential (`2^n` subsets, `n!` permutations); this is a property of the problem, not a flaw in any particular algorithm — no cleverer approach can beat it when the full enumeration is genuinely required.
- Iterative doubling: start with `{{}}`, and for each new element, duplicate every existing subset with the element appended — snapshotting the list's length before the inner loop is the detail most commonly gotten wrong.
- Recursive include/exclude: at each index, recurse into "exclude this element" and "include this element" (push, recurse, pop) — recording a copy of the path buffer whenever every index has been decided.
- Duplicate input values require sorting first and skipping same-level duplicates, or the same subset gets emitted more than once.
- This pattern only scales to small `n` (roughly `<= 20-25`); larger `n` needs a fundamentally different formulation (often Dynamic Programming), not a faster implementation of the same enumeration.
- The recursive include/exclude (and choose/recurse/undo, for permutations) skeleton is the direct foundation Backtracking builds on by adding a validity check and pruning.

## Key Takeaways

1. Subsets enumerates every subset/combination/permutation of a set — the output is inherently `O(2^n)` or `O(n!)`, so there is no faster algorithm to reach for, only a correct one.
2. Two equivalent framings: iterative doubling (duplicate the growing result list per element) and recursive include/exclude (branch into "skip" and "take" per index).
3. The iterative version's most common bug is failing to snapshot the result list's length before the loop that appends to it.
4. The recursive version's discipline is push-recurse-pop on a single shared path buffer, copying it into the results only at the base case.
5. Always include the empty subset — it is a valid member of the power set, easy to forget in both framings.
6. Duplicate input elements require sorting first, then skipping a value that equals its immediate predecessor at the *same recursion level* — not anywhere in the current path.
7. Permutations use the same choose/recurse/undo discipline as subsets, generalized from 2 branches per level to up to `n` branches per level via a `used[]` marker array.
8. Complexity is `O(2^n * n)` time/space for subsets, `O(n! * n)` for permutations — the extra factor of `n` is the cost of building/copying each output.
9. This pattern only scales to roughly `n <= 20-25`; larger inputs need a different formulation entirely (often Dynamic Programming), not a faster version of full enumeration.
10. Subsets is Backtracking with no pruning — the moment a problem has a constraint that can invalidate a partial answer early, reach for Backtracking instead.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — background on combinatorics and recursion trees underlying exhaustive enumeration.
- *Competitive Programmer's Handbook* — Antti Laaksonen — has a concise treatment of subset/permutation generation and bitmask enumeration as used in competitive programming.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — includes worked subset/permutation/combination problems with recursion-tree framing.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes power-set and permutation-generation problems with C++-specific implementation notes.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including backtracking and combinatorial-generation implementations, useful for seeing varied implementation styles.

**Official Documentation**
- LeetCode — Subsets (problem 78).
- LeetCode — Subsets II (problem 90).
- LeetCode — Permutations (problem 46).
- LeetCode — Combination Sum (problem 39).
- LeetCode — Combinations (problem 77).
- cppreference.com — `std::next_permutation` — the C++ standard library's own in-place permutation-generation algorithm, a production-grade reference implementation of exhaustive ordering enumeration.

**Blog Articles**
- GeeksforGeeks — "Power Set" and "Backtracking to find all subsets" — widely used explainers covering both the iterative and recursive approaches to subset generation.
- NeetCode — Subsets / Backtracking pattern videos/playlist — walks through Subsets, Subsets II, Permutations, and Combination Sum with visual recursion-tree explanations.
- Educative.io — "Grokking the Coding Interview" Subsets pattern chapter — one of the most widely referenced pattern-based framings of this exact technique.
