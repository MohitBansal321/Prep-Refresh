# Unbounded Knapsack


> **In one line:** the same table shape as 0/1 Knapsack, but each coin/item can be reused any number of times — shown here as coin-change, minimizing the count of coins that sum to a target amount.

```cpp
const int UNREACHABLE = INT_MAX / 2;
std::vector<int> dp(amount + 1, UNREACHABLE);
dp[0] = 0;   // base case: 0 coins needed to make amount 0

for (int w = 1; w <= amount; ++w) {
  for (int c : coins) {
    if (c <= w && dp[w - c] != UNREACHABLE) {
      dp[w] = std::min(dp[w], dp[w - c] + 1);   // reuse coin c any number of times
    }
  }
}
return dp[amount] == UNREACHABLE ? -1 : dp[amount];
```

**O(amount · coins)** time · **O(amount)** space. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Given a capacity (or an exact target sum) and a set of items that can each be reused an **unlimited** number of times, compute the best achievable value, count, or minimum count — using a recurrence that is a single, deliberate change from 0/1 Knapsack ([../0-1-knapsack/](../0-1-knapsack/)): after taking an item, you are allowed to consider taking that **same item again**.

## Real Life Analogy

Think of a **vending machine making change** with an unlimited supply of each coin denomination — pennies, nickels, dimes, quarters. If the machine needs to give you $0.75 back, it does not have "one quarter" set aside with your name on it; it has an effectively bottomless tray of quarters, and it can reach for a quarter, then reach for *another* quarter, then a third quarter, until it has made exactly $0.75 (three quarters), or it decides a different mix of coins does the job in fewer total coins. The same tray, the same denomination, used more than once — that is the entire idea.

Contrast that with a **backpack you are packing for a hike** where you have exactly one tent, one stove, one sleeping bag: once the tent is in the pack, there is no second tent to add later. That second scenario is 0/1 Knapsack. The vending machine — same denominations, drawn from again and again — is Unbounded Knapsack. Everything else about "which items, under what capacity, to maximize what" is identical between the two; only the "can I reach for this one again?" answer changes.

Another everyday one: a **rod-cutting shop** that buys long metal or wood rods and cuts them into pieces of standard lengths to sell (say, pieces of length 1m, 3m, and 4m, each with its own price). The shop does not run out of "the ability to cut a 1m piece" — if cutting three 1m pieces from an 8m rod turns out to be the most profitable split, nothing stops the shop from doing that. The "item" (a cut of a given length) is reusable as many times as the rod allows.

Unbounded Knapsack is that reusable-tray, reusable-cut idea in code: the same recurrence as 0/1 Knapsack, except taking an item does not remove it from future consideration.

## Problem

### What engineering problem exists?

A large family of "optimize under a resource constraint" problems share this exact shape:

- **"What is the best value I can pack into a fixed capacity, where each item type is available in unlimited supply?"** — rod cutting (cut a rod into pieces to maximize total sale price), or a manufacturing "cutting stock" problem (cut raw material into standard part sizes to maximize yield).
- **"What is the fewest number of pieces/coins/parts needed to hit an exact target?"** — making change with the fewest coins, or breaking an integer into the fewest perfect squares.
- **"How many distinct ways can I hit an exact target using unlimited copies of each piece?"** — counting the number of distinct coin combinations that sum to an amount.

> **Term: Capacity.** The resource limit you must respect — often a weight limit, a length, or an exact target sum. It is the axis every `dp` table in this family is indexed by.

The naive way to answer any of these is **brute-force recursion**: at each step, try every item, subtract its weight (or coin value) from the remaining capacity, and recurse on what is left — trying to take the same item again is exactly what makes this "unbounded." Without memoization, this recursion **re-explores the exact same (remaining-capacity) state over and over**, because many different sequences of coin/item choices land on the same remaining capacity. For coins `[1, 2, 5]` and amount `11`, the path "take a 1, then a 2, then a 2" and the path "take a 2, then a 1, then a 2" both leave you at remaining capacity `6` — and naive recursion solves "what's the best way to make 6" **twice**, from scratch, with no memory that it already did the work.

> **Term: Overlapping subproblems.** When a recursive algorithm calls itself with the *same arguments* through different call paths, and (without caching) redoes the same work every time. This is one of the two properties (along with *optimal substructure* — the best answer for a state can be built from the best answers to smaller states) that make a problem a good fit for dynamic programming.

### Why is this problem difficult?

- **The "reuse" possibility multiplies the number of ways to reach the same state.** With 0/1 Knapsack, each item appears in a choice sequence at most once, which caps how many different orderings can reach a given (item-index, remaining-capacity) state. With unlimited reuse, the same item can appear any number of times in different positions of a choice sequence, so the raw recursion tree branches even more aggressively before memoization is applied.
- **The recurrence looks deceptively similar to 0/1 Knapsack's.** Both are "take-or-skip" decisions over items and a capacity axis. The one-line difference (do you recurse into `capacity - weight[i]` while still allowing item `i` again, or do you move on to item `i+1` after taking it) is easy to blur together, especially when translating between the 2D recursive formulation and a 1D iterative table — which is exactly why the fill *direction* of the 1D table becomes the single most consequential implementation detail in this whole module (see Common Mistakes).
- **Exact-target variants (minimize/count) need different base cases and sentinels than maximize variants**, and mixing them up produces silently wrong answers rather than crashes — a `dp[w] = 0` floor is correct for "maximize value achievable with *at most* capacity w" but wrong for "minimize coins to make *exactly* w" (see Common Mistakes).

### What happens if we ignore it?

- **Exponential blowup from redundant recursion.** Naive recursive coin-change on amount `n` with `k` denominations can branch into roughly `k^n` calls in the worst case before memoization — for `n` in the thousands (a realistic "make change for this invoice total in cents" scale), this is not "a bit slow," it is **never finishing**.
- **A production "minimum coins" or "minimum parts" service becomes unusably slow** on any input beyond a handful of items, when an O(target * items) dynamic-programming table would return in microseconds to milliseconds.
- **Silently wrong answers when the maximize/minimize/count base cases get swapped.** Treating an unreachable exact-target capacity as `0` (as a maximize problem correctly would) instead of an explicit "unreachable" sentinel produces a confidently wrong "0 coins needed" for amounts that cannot actually be made.

## Why Not Other Approaches?

**"Brute-force recursion without memoization."**
Correct, but re-explores the identical `(remaining-capacity)` state through every distinct order of item choices that lands on it — the *overlapping subproblems* described above. This is the "does it even finish" failure mode: exponential time, and for any realistic capacity/amount it does not complete in a reasonable time budget at all.

**"Memoized recursion (top-down DP) without ever moving to an iterative table."**
This is a legitimate stepping stone — memoizing on `(item index, remaining capacity)` or just `remaining capacity` collapses the exponential recursion tree down to the same O(items * capacity) states as the iterative version, because each distinct state is now solved exactly once. It is not *wrong*; it is simply more machinery (recursion, a memo table, cache-key management) than a plain iterative forward-fill loop needs once you understand the recurrence, and it carries real recursion-depth/stack-overflow risk for large capacities in a language like C++ that does not guarantee tail-call optimization. This module builds directly to the iterative 1D table because that is what production code should default to, but understanding *why* memoized recursion also works (same subproblems, same reuse-through-shared-smaller-state idea) is worth internalizing.

**"Greedily always take the largest/best-value item that fits."**
Works for some specific coin systems (e.g. US coin denominations happen to make greedy correct for making change) but is **not correct in general**. A classic counterexample: coins `[1, 3, 4]`, amount `6` — greedy takes `4`, then is stuck needing `2` more from `[1, 3]`, giving `4 + 1 + 1 = 3 coins`, while the optimal answer is `3 + 3 = 2 coins`. Greedy has no way to know, at the moment it takes the `4`, that this choice forecloses a better option later — exactly the kind of "local choice that isn't provably safe" that rules greedy out (see Greedy Patterns' family README for the general contrast). Dynamic programming does not guess; it computes the actual best answer for every smaller capacity first.

**"Enumerate every possible multiset of item counts, up to capacity / min-item-weight copies of each."**
This is brute force wearing combinatorics clothing — the number of multisets grows combinatorially with capacity and item count, and it recomputes information (the best way to fill a smaller sub-capacity) that a DP table stores and reuses for free.

**Tradeoff summary:** brute-force recursion fails to finish in reasonable time on realistic inputs; greedy fails to produce a *correct* answer in general (not just a slow one); memoized recursion is correct and roughly the same asymptotic complexity as the iterative version but adds recursion/call-stack overhead this module's iterative approach avoids entirely. The 1D forward-fill dp table is the approach that is simultaneously correct, fast, and simple to implement without recursion machinery.

## Solution

Define `dp[w]` to mean: **the best value (or fewest items, or number of distinct ways) achievable using capacity exactly `w` or at most `w`, depending on the problem's flavor, using any number of copies of any item.**

The recurrence considers, for each capacity `w` and each item `i` that fits (`weight[i] <= w`):

```
dp[w] = combine( dp[w],  dp[w - weight[i]]  "plus/aggregated with"  item i's contribution )
```

This line is **structurally identical** to 0/1 Knapsack's recurrence — same idea of "either don't use item `i` at this capacity, or use it and fall back to a smaller capacity's already-known answer." The one-line change that makes this Unbounded Knapsack instead of 0/1 Knapsack is *what has already been computed* by the time `dp[w - weight[i]]` is read: in this pattern, `dp[w - weight[i]]` is allowed to **already include item `i` itself**, because — as the Execution Flow section makes precise — the capacity axis is filled **forward** (increasing `w`), so smaller capacities are finalized before larger ones, using the same item, in the same pass. That is the entire mechanism of "reuse": there is no explicit "loop again over this item" instruction anywhere; reuse is a *consequence* of reading an already-updated, possibly-already-including-this-item value at a smaller capacity.

The three "shapes" this recurrence takes, all built on that same reuse mechanism:

- **Maximize value** (rod cutting): `dp[w] = max(dp[w], dp[w - weight[i]] + value[i])`, with `dp[0] = 0` as a genuine, valid base case (0 capacity means 0 value, not "unreachable").
- **Minimize count to an exact target** (coin change, perfect squares): `dp[w] = min(dp[w], dp[w - item] + 1)`, with `dp[0] = 0` (zero items needed to make sum zero) but every other capacity starting at an explicit "unreachable" sentinel, since — unlike the maximize case — not every target is achievable at all.
- **Count distinct combinations to an exact target** (coin change II): `dp[w] += dp[w - coin]`, with `dp[0] = 1` (exactly one way to make zero: use nothing), and a loop-nesting order (coin outer, capacity inner) that is itself part of the correctness argument, not just a stylistic choice — covered in depth in Common Mistakes and in [problems/02-coin-change-ii.cpp](problems/02-coin-change-ii.cpp).

## Architecture

The "participants" in an Unbounded Knapsack computation:

1. **The `dp` table.** A single 1D array indexed by capacity, `dp[0..capacity]` (or `dp[0..amount]` for exact-target variants). Its invariant, once capacity `w` has been processed, is: `dp[w]` holds the final, correct answer for capacity/target `w`, considering unlimited reuse of every item — and it will never be recomputed or revisited after this point in the fill.
2. **The items (weights/values, or coin denominations, or derived "parts").** Each is examined at every capacity `w` it fits into (`weight[i] <= w`). Critically, an item is *not* consumed or marked "used" anywhere — there is no bookkeeping tracking how many times an item has been taken, because the dp table itself, read at a smaller capacity, already encodes "however many times it made sense to use this item to get here."
3. **The capacity/target axis and its fill direction.** This is the one participant that behaves differently here than in 0/1 Knapsack: capacity is iterated **forward**, from `0` (or `1`) up to `capacity`/`amount`. The direction is not a stylistic choice — it is the mechanism that permits reuse (see Execution Flow).
4. **The combine/aggregation rule** (`max`, `min` + 1, or `+=`) — the yardstick that turns "what does a smaller capacity already know" into "what should this capacity's answer be." This is the piece that differs across the three problem shapes described above, while the reuse mechanism underneath stays the same.

Responsibilities in one line each:
- **`dp` table:** the single source of truth for "already-solved, smaller-capacity" answers, filled once, left-to-right, never revisited.
- **Items:** offer themselves at every capacity they fit, without being marked used — reuse is implicit, not tracked.
- **Fill direction:** forward capacity iteration is what allows a smaller, already-finalized capacity to already reflect a previous use of the same item.
- **Combine rule:** picks the correct aggregation (max/min+1/sum) for the specific "maximize / minimize-exact / count-exact" flavor of the problem.

## Execution Flow

1. **Base case.** Set `dp[0]` according to the problem's flavor: `0` for maximize-value problems (zero capacity, zero value — a real, valid answer), `0` for minimize-count-to-exact-target problems (zero coins needed to make sum zero — also a real, valid answer, not "unreachable"), or `1` for count-distinct-ways problems (there is exactly one way to make sum zero: choose nothing). All other `dp[w]` for `w > 0` start at a value the combine rule will always improve on: `0` (max), an explicit "unreachable" sentinel (min-exact), or `0` (count, meaning "no ways found yet").
2. **Fill order: capacity FORWARD, `w` from `1` up to `capacity`/`amount`.** This is the step that most sharply contrasts with 0/1 Knapsack. 0/1 Knapsack's 1D-array optimization iterates `w` **backward** (from `capacity` down to each item's weight) *for each item in turn*, specifically so that `dp[w - weight[i]]`, when read, still reflects a state from **before** the current item was considered at all — guaranteeing at most one use. Unbounded Knapsack does the opposite on purpose: filling forward means that by the time capacity `w` is processed, every smaller capacity `w' < w` — including `w - weight[i]` — has **already been finalized in this same pass**, and that finalized value may already include one or more uses of item `i`. Forward fill is not an arbitrary convention here; it is the single mechanism that makes "unlimited reuse" happen without any explicit "try this item again" instruction in the code.
3. **For each capacity `w` in that forward order, apply the combine rule across every item that fits** (`weight[i] <= w`, or `coin <= w`), reading `dp[w - weight[i]]` (already finalized, possibly already including item `i`) and updating `dp[w]` accordingly.
4. **Reading the answer.** For maximize/minimize problems, the answer is `dp[capacity]` or `dp[amount]` directly (translating a min-exact "unreachable" sentinel back to whatever the problem wants returned, e.g. `-1`). For count-distinct-ways problems, the answer is likewise `dp[amount]`, but only correct if the combine step's **outer loop was over items/coins, not over capacity** — see Common Mistakes for why that loop-nesting detail specifically matters for counting, and not for maximize/minimize.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full flowchart distinguishing Unbounded Knapsack from 0/1 Knapsack, and further splitting Unbounded Knapsack into its maximize / minimize-exact / count-exact variants based on the signals in a problem statement.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the 1D dp forward-fill loop, with the exact point where 0/1 Knapsack's backward fill diverges called out explicitly.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of the dp array being filled on the concrete example `coins = [1, 2, 5]`, `amount = 11`, including a dependency graph showing coin `5` being read twice on the path to the final answer — reuse made completely concrete rather than asserted in prose.

## Implementation

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the pattern's shape clearly before looking at the four worked, problem-specific solutions in [problems/](problems/).

It provides two small functions:

- `unboundedKnapsack` — the maximize-value flavor over generic `weights`/`values` vectors and an integer `capacity`, returning the best achievable total value using any number of copies of any item.
- `coinChangeMinCoins` — the minimize-count-to-an-exact-target flavor, returning the fewest coins summing to exactly `amount`, or `-1` if `amount` is unreachable with the given denominations.

Both share the identical 1D array, forward-fill-over-capacity shape; only the aggregation (`max` + value vs. `min` + 1) and the base-case/sentinel handling (a `0` floor vs. an explicit "unreachable" marker) differ, exactly matching the two problem flavors described in Solution above. The third flavor — counting distinct combinations — is deliberately left out of `code.cpp` and shown only in [problems/02-coin-change-ii.cpp](problems/02-coin-change-ii.cpp), because its correctness depends on a loop-nesting detail (coin outer, capacity inner) that deserves to be seen once, explained thoroughly, and not silently absorbed into a "generic template" without comment.

## Code Walkthrough

**`unboundedKnapsack`** (in [code.cpp](code.cpp)). Takes parallel `weights`/`values` vectors and an integer `capacity`. Allocates `dp` of size `capacity + 1`, all zero-initialized (a correct base case here, since "capacity 0 => value 0" and "no items chosen => value 0" are both legitimately the same "zero" state for a maximize problem). Loops `w` from `1` to `capacity` (forward), and for each `w`, loops over every item, applying `dp[w] = max(dp[w], dp[w - weights[i]] + values[i])` whenever the item fits. Returns `dp[capacity]`. This function exists to demonstrate the maximize-value flavor in its most generic, reusable form — the direct ancestor of the rod-cutting-style example exercised in `main()`.

**`coinChangeMinCoins`** (in [code.cpp](code.cpp)). Takes a `coins` vector and an integer `amount`. Allocates `dp` of size `amount + 1`, initialized to a large sentinel (`INT_MAX / 2`, chosen specifically to avoid signed-integer overflow when `+ 1` is added to it) representing "unreachable," except `dp[0] = 0` (zero coins needed to make amount zero — a real base case, not a sentinel). Loops `w` from `1` to `amount` (forward), and for each `w`, loops over every coin, applying `dp[w] = min(dp[w], dp[w - c] + 1)` only when `c <= w` **and** `dp[w - c]` is not itself the unreachable sentinel (guarding against building nonsense sums on top of "unreachable"). Returns `dp[amount]`, translated to `-1` if it is still the sentinel. This function exists to demonstrate the minimize-count-to-exact-target flavor, and is the direct ancestor of [problems/01-coin-change.cpp](problems/01-coin-change.cpp).

**`main()`** (in [code.cpp](code.cpp)). Exercises both functions against small, hand-checkable inputs — including a capacity-0 edge case and an unreachable-amount edge case — and prints `[PASS]`/`[FAIL]` for each assertion, proving the template compiles and runs correctly end to end.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem, not calling the generic functions above directly (to keep each file dependency-free and independently readable), but implementing the *same* forward-fill recurrence inline, with problem-specific comments tying every decision back to the general principles established in this README. See [problems/README.md](problems/README.md) for the index and the "why these four" rationale. Briefly: `01` is the pure minimize-count-to-exact-target flavor (the direct sibling of `coinChangeMinCoins`); `02` uses the *same inputs* as `01` but counts distinct combinations instead, and exists specifically to make the loop-nesting-order pitfall concrete and testable; `03` shows the item list itself can be *derived* (perfect squares) rather than given; `04` shows "reuse" appearing through the recurrence's own structure (`dp[i - j]` may have already chosen split-size `j` again) even when the problem statement never mentions an explicit list of reusable items at all.

## Advantages

- **Turns exponential brute-force recursion into O(items * capacity) table-filling.** The overlapping-subproblems structure is exploited exactly once per `(capacity)` state instead of once per distinct *path* to that state.
- **No recursion, no call stack, no memo-table bookkeeping.** The iterative forward-fill version needs only a flat array and two nested loops — simpler to reason about, and free of the stack-depth risk that memoized recursion carries for large capacities.
- **The same recurrence shape covers maximize, minimize-exact, and count-exact problems** by swapping only the combine rule and base case — once you have internalized the forward-fill mechanism, a wide range of problems (rod cutting, coin change, perfect squares, and more) become the same template with different aggregation.
- **Space can be reduced to O(capacity)** (a single 1D array), because — unlike 0/1 Knapsack in its naive 2D form — there is no need to keep a separate row per item; the forward fill's reuse mechanism works from a single array precisely because reading "the current row, at a smaller capacity" is exactly what you want.
- **Provable correctness, not heuristic.** Every `dp[w]` is the actual optimum/count for that capacity, computed from actual optima at smaller capacities — no guessing, unlike greedy approaches that can be subtly wrong (see Why Not Other Approaches).

## Disadvantages

- **Still pseudo-polynomial in capacity.** O(items * capacity) sounds like "polynomial," but `capacity` (or `amount`) is a *value*, not the size of the input in bits — if `capacity` is a 64-bit integer that happens to be a huge number (say, 10^12), the table itself becomes infeasible to allocate or fill, even though the *number of items* is small. This is the same pseudo-polynomial caveat that applies to 0/1 Knapsack.
- **Easy to silently implement 0/1 semantics by mistake.** Filling the capacity axis backward (copying 0/1 Knapsack's fill direction out of habit, or misremembering which pattern uses which direction) produces a program that compiles, runs, and returns a plausible-looking number — just the *wrong* one, because it silently forbids reuse. There is no crash, no exception, nothing to point you at the bug except a wrong answer on a test case that happens to require reuse.
- **Loop-nesting order matters for counting variants in a way it does not for maximize/minimize.** Getting the coin/capacity loop order backward in a "count distinct combinations" problem produces a *different, larger, wrong* number (permutations instead of combinations) without any error — see Common Mistakes.
- **Off-by-one risk in the base case is easy to get subtly wrong**, and, like the fill-direction mistake, it fails silently: treating "amount 0" as `1` (a count-of-ways answer) when the problem actually wanted `0` (a minimize-count answer), or vice versa, produces a wrong final number without any indication something is off.

## Tradeoffs

**What we gain versus brute-force recursion:** we go from exponential-time re-exploration of overlapping states down to O(items * capacity) time, in O(capacity) space, by computing each capacity's answer exactly once and reusing it (in the literal, algorithmic sense) for every larger capacity that needs it.

**What we gain versus memoized (top-down) recursion:** the same asymptotic complexity, but without recursion depth, call-stack overhead, or a hash-map/memo-table's constant-factor and memory cost — a flat array and two nested loops.

**What we lose versus 0/1 Knapsack's "each item used once" guarantee:** nothing is *lost* computationally when unlimited reuse is genuinely what the problem needs; the loss only appears if you apply this pattern's forward-fill mechanism to a problem that actually needed 0/1 semantics — then you have silently changed the problem being solved, not made a legitimate tradeoff.

**What we lose versus greedy (when greedy happens to be correct for a specific coin system):** greedy, when it is correct at all, is O(items) or O(items log items) — asymptotically better than DP's O(items * capacity). Unbounded Knapsack DP is strictly slower but is **correct for every coin/item system**, not just the specific ones (like standard currency denominations) where greedy happens to work. The tradeoff is generality and guaranteed correctness versus raw speed on a narrow class of inputs.

## Complexity

**Time:** **O(n * capacity)**, where `n` is the number of distinct items/coins and `capacity` is the capacity or target amount — `capacity` forward-fill iterations, each doing O(n) work across the items. This holds for all three flavors (maximize, minimize-exact, count-exact) identically; only the O(1) per-item work inside the loop differs (a `max`, a `min` + 1, or a `+=`).

**Space:** **O(capacity)** — a single 1D array, independent of the number of items. This is the same asymptotic space as 0/1 Knapsack's space-optimized 1D form; the two patterns are in the **same asymptotic complexity class** end to end, and the entire distinguishing factor between them is the fill *direction*, not any difference in time or space bound.

**Comparison to the brute force each replaces:**

| Variant | Brute-force recursion | Unbounded Knapsack DP |
|---|---|---|
| Maximize value under capacity (rod cutting) | Exponential (branches on every item, every level) | O(n * capacity) time, O(capacity) space |
| Minimize count to exact target (coin change) | Exponential | O(n * amount) time, O(amount) space |
| Count distinct combinations to exact target (coin change II) | Exponential | O(n * amount) time, O(amount) space |

## Common Mistakes

- **Using the same backward-capacity fill order as 0/1 Knapsack.** This is, by a wide margin, **the single most important mistake to avoid in this entire module**, and the single most important contrast with 0/1 Knapsack. Filling `w` from `capacity` down to `weight[i]` (correct for 0/1 Knapsack's 1D optimization) means `dp[w - weight[i]]` is read *before* the current item has been applied to it in this pass — which caps each item at one use per row, silently reproducing 0/1 semantics inside code that was supposed to allow unlimited reuse. *Why it happens:* the two patterns' 1D-array code looks almost identical at a glance, and "which direction does the loop go" is exactly the kind of small detail that gets copied out of muscle memory from whichever pattern you wrote most recently. *Avoid:* before writing the loop, say out loud which direction you need and why — "forward, because I need `dp[w - weight[i]]` to already reflect a possible earlier use of this same item in this same pass" — and treat that sentence as a mandatory comment in the code, not an afterthought.
- **Off-by-one on whether 0 coins/0 capacity is a valid base case returning 0, or an "unreachable" state.** For maximize-value problems, `dp[0] = 0` is a genuinely correct answer (zero capacity really does yield zero value). For minimize-count-to-exact-target problems, `dp[0] = 0` is *also* correct (zero coins really do make sum zero) — but every *other* unreached capacity must start at an explicit "unreachable" sentinel, **not** `0`, or the algorithm will confidently report "0 coins needed" for amounts that are actually impossible to make. For count-distinct-ways problems, `dp[0] = 1` (there is exactly one way to make zero: choose nothing) is the base case, and getting this wrong (using `0`) makes every derived count wrong, since every other `dp[w]` is built by summing values that trace back to `dp[0]`.
- **Getting the loop-nesting order backward in a count-distinct-combinations problem.** Looping capacity outer and coins inner (which is completely fine, and standard, for the maximize and minimize-exact flavors) counts *permutations* instead of *combinations* when counting distinct ways — see [problems/02-coin-change-ii.cpp](problems/02-coin-change-ii.cpp) for a concrete, runnable demonstration of the two loop orders diverging on the same input. *Why it happens:* the recurrence body (`dp[w] += dp[w - coin]`) looks identical either way; only the loop nesting encodes "has this coin already been fully accounted for before moving to the next" versus "try every coin fresh at every capacity." *Avoid:* for counting-combinations problems specifically, always put the item/coin loop on the **outside**.
- **Forgetting to guard against building on top of an "unreachable" sentinel.** In `dp[w] = min(dp[w], dp[w - c] + 1)`, if `dp[w - c]` is itself the unreachable sentinel, adding `1` to it produces a large-but-not-quite-sentinel number that can silently pollute later comparisons (or, worse, overflow if the sentinel is close to the integer type's maximum). *Avoid:* either check `dp[w - c] != UNREACHABLE` before using it (as [code.cpp](code.cpp) does), or choose a sentinel deliberately small enough (e.g. `INT_MAX / 2`) that a few additions cannot overflow, and still exclude it explicitly from ever "winning" a `min` comparison against a real answer.
- **Assuming the pattern always needs an explicit "list of reusable items."** [problems/04-integer-break.cpp](problems/04-integer-break.cpp) has no coins or weighted items at all — "reuse" shows up purely through `dp[i - j]` potentially having already chosen split-size `j` again. Looking for a literal "unlimited supply" phrase in a problem statement, and concluding "this isn't Unbounded Knapsack" when it is absent, will cause you to miss variants of this pattern that are dressed differently.

## When To Use

- The problem gives (or implies) a set of items/coins/pieces where **each can be used any number of times**, plus a capacity or exact target to respect.
- You need the **best value** achievable under a capacity limit, where "best" is built from smaller capacities' already-known best values (rod cutting, cutting stock, resource allocation with a reusable resource type).
- You need the **fewest pieces** to hit an **exact** target sum (coin change, minimum perfect squares, minimum "steps" of fixed sizes to close a gap).
- You need to **count the distinct ways** to hit an exact target using unlimited copies of each piece (coin change II, or "number of ways to climb n stairs taking 1, 2, or 3 steps at a time" style problems).
- You recognize the recurrence "the best/fewest/count for `i` depends on the best/fewest/count for a strictly smaller `i - something`, where `something` can repeat" — even if the problem never uses the words "unlimited" or "reusable" explicitly (see [problems/04-integer-break.cpp](problems/04-integer-break.cpp)).

## When NOT To Use

- **Items/coins/pieces are limited-use — each usable at most once (or a bounded, small number of times).** That is 0/1 Knapsack's territory ([../0-1-knapsack/](../0-1-knapsack/)); applying this pattern's forward-fill reuse mechanism there silently over-counts uses of an item that was supposed to be scarce.
- **The problem compares two different sequences** (edit distance, longest common subsequence) — that is the Longest Common Subsequence family's 2D-over-two-indices shape, not a single capacity axis.
- **The "capacity" is astronomically large relative to the number of items**, making even O(items * capacity) infeasible — this is the pseudo-polynomial ceiling; at that point you need a different technique entirely (e.g. number-theoretic reasoning about coin systems, or a completely different algorithm class), not a bigger dp array.
- **A locally-optimal greedy choice is provably always safe for the specific item/coin system at hand** (e.g. standard currency denominations for making change) — in that narrow case, greedy is strictly faster and DP's generality is unnecessary overhead. The catch is *proving* the greedy safety property holds for your specific denominations; absent that proof, default to DP.

## Real Interview/Production Examples

- **Currency and change-making systems.** Point-of-sale systems, ATMs dispensing bills, and vending machines computing the fewest coins/bills to return as change are the textbook real-world instance of the minimize-count-to-exact-target flavor — with the important caveat that real currency systems are usually (but not provably in general) friendly to a greedy approximation, while a general-purpose "works for any denomination set" implementation needs the DP version to be correct.
- **Cutting stock / rod-cutting in manufacturing.** Deciding how to cut raw stock (metal bars, lumber, fabric rolls, or even 1D "cut lengths" from a 2D sheet-cutting formulation) into standard part sizes to maximize yield or revenue is a direct instance of the maximize-value flavor, and appears in real production-planning and ERP software.
- **Resource allocation where a resource type is effectively unlimited.** Allocating a divisible, replenishable resource (compute instance-hours from an auto-scaling pool, standard-sized shipping containers/pallets, or fixed-denomination budget line items) across tasks to maximize some objective under a total-capacity constraint reduces to this same shape whenever the resource "type" (not the specific unit) is what's unlimited.
- **"Minimum number of X to reach exactly Y" problems in scheduling and billing systems** — e.g. minimum number of fixed-size billing increments, minimum number of fixed-denomination gift cards to cover a balance exactly, or minimum number of fixed-size batches to process an exact quantity — are all the minimize-count-to-exact-target flavor wearing different domain vocabulary.
- Unbounded Knapsack (specifically Coin Change and its variants) is one of the most frequently asked dynamic-programming patterns in coding interviews at major tech companies, precisely because the 0/1-vs-unbounded distinction and the forward/backward fill-direction contrast test whether a candidate actually understands *why* the recurrence works, rather than having memorized one specific problem's code.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **A "minimum denominations to settle a balance" utility** for an internal ledger or billing system — given a set of standard credit/adjustment denominations, compute the fewest line items needed to zero out an exact balance, using the minimize-count-to-exact-target flavor.
2. **A cutting-optimization service for a print/packaging/manufacturing backend** — given standard material roll widths or lengths and their costs/values, compute the best-value way to cut stock into salable pieces, using the maximize-value flavor.
3. **A "ways to reach a target" counter for gamified/progress-tracking features** — e.g. counting the number of distinct ways a user could have accumulated a point total from a fixed set of reward denominations, useful for anti-fraud pattern analysis (does a claimed point total have *any* valid combination of legitimate awards that sums to it?), using the count-distinct-combinations flavor.
4. **A capacity-planning helper for infrastructure sized in fixed instance/container tiers** — given fixed instance-hour "denominations" (small/medium/large tiers) and a total compute-hour budget, compute the maximum workload value schedulable, treating tiers as unlimited-supply items.
5. **A "minimum steps to close a numeric gap" helper** for a stepped-pricing or stepped-quota system — e.g. minimum number of fixed-size quota top-ups to reach an exact target quota, directly analogous to Perfect Squares' "minimum number of fixed-shape increments to reach an exact total."

## Similar Patterns

**Unbounded Knapsack vs. 0/1 Knapsack** is the single most commonly confused pair in dynamic programming, precisely because the recurrence, the dp table shape, and even most of the surrounding code are near-identical — the entire distinction lives in one implementation detail (fill direction) that has an outsized effect on correctness.

| Aspect | 0/1 Knapsack ([../0-1-knapsack/](../0-1-knapsack/)) | Unbounded Knapsack (this module) |
|---|---|---|
| Can an item be used more than once? | No — at most once | Yes — any number of times |
| `dp` state meaning | `dp[i][w]` (or 1D-optimized `dp[w]`): best answer considering the **first `i` items**, capacity `w` | `dp[w]`: best answer for capacity `w`, considering **all** items, any number of uses each |
| 1D-array fill direction over capacity | **Backward** (`capacity` down to `weight[i]`), per item | **Forward** (`0`/`1` up to `capacity`), across all items each step |
| Why that direction | Guarantees `dp[w - weight[i]]` reflects a state from *before* this item was considered — at most one use | Guarantees `dp[w - weight[i]]` may already reflect a *previous use of this same item* in this same pass — reuse |
| Time complexity | O(n * capacity) | O(n * capacity) — same class |
| Space complexity (1D-optimized) | O(capacity) | O(capacity) — same class |
| Canonical problems | Subset sum, partition equal subset sum, target sum | Coin change, coin change II, rod cutting, perfect squares |
| Recognition phrase | "each item usable once" | "unlimited supply" / "reused any number of times" |

- **Bounded Knapsack (each item usable up to a fixed count `k`, not unlimited):** a middle ground between the two — can be solved either by treating each item as `k` separate 0/1 items (correct but blows up the item count) or by a binary/power-of-two decomposition of the count to keep the item count logarithmic in `k`, or by a sliding-window-over-capacity optimization of the unbounded recurrence. Not covered as its own module here, but worth knowing as the natural generalization sitting between 0/1 and Unbounded.
- **Longest Common Subsequence family** ([../longest-common-subsequence/](../longest-common-subsequence/)): also a 2D-table-collapsible-to-1D DP, but the two axes index **two different sequences**, not "items considered so far" and "capacity" — a different question shape (comparing two sequences) rather than "pack under one capacity."
- **DP on Grids** ([../dp-on-grids/](../dp-on-grids/)): a 2D table indexed by an explicit row/column position rather than an abstract capacity — related in that it is also "best answer reachable at this state, built from smaller states," but the state itself is spatial, not a resource capacity.

## Interview Discussion

Experienced engineers do not spend much time on "can you write the recurrence" — that is close to rote once you have seen the pattern. What they actually probe is whether you can **articulate, precisely, why the fill direction is what makes this pattern different from 0/1 Knapsack** — not just recite "forward for unbounded, backward for 0/1" as a memorized rule, but explain the underlying reason: forward fill lets a smaller capacity's already-finalized answer already include a previous use of the item currently being considered.

Common follow-up questions:
- *"How would you change this code to make it 0/1 Knapsack instead?"* — expects "flip the capacity loop to iterate backward, per item," and ideally a clear statement of *why* that single change removes the possibility of reuse.
- *"Why does Coin Change II need the coin loop on the outside, when Coin Change (minimum coins) does not care about loop order?"* — expects recognizing that minimize/maximize aggregations (`min`, `max`) are invariant to loop order because they only ever want "the best across all valid ways," while summing counts (`+=`) is not invariant, because it will double-count (or under/over count differently) depending on whether items are fully processed one at a time (combinations) or interleaved (permutations).
- *"What if the capacity were extremely large (say, 10^12) but the number of item types were small?"* — expects recognizing the pseudo-polynomial ceiling and naming an alternative direction (e.g. matrix exponentiation of the recurrence, or number-theoretic analysis specific to the coin system), rather than insisting on scaling the same dp array.
- *"Is greedy ever correct here, and how would you know?"* — expects the honest answer: only for specific coin/item systems that satisfy a provable exchange property (standard currency denominations happen to, arbitrary denominations do not), and that without a proof for the specific system at hand, DP is the safe default.
- *"Walk me through why dp[0] is 0 in one variant and 1 in another."* — expects distinguishing "zero capacity legitimately yields zero value/zero coins" (a real answer) from "there is exactly one way to make a sum of zero: use nothing" (a different kind of base case, for counting).

Common misconceptions:
- "Unbounded Knapsack is just 0/1 Knapsack with a bigger loop." It is the *same* asymptotic complexity class, but the correctness of "reuse" hinges entirely on fill direction, not on loop count.
- "The loop order for the coin/capacity axes never matters — it's just a stylistic choice." True for maximize/minimize aggregations; false, and a source of real bugs, for counting distinct combinations.
- "If greedy gives the right answer on my test cases, it's correct." Greedy correctness depends on a property of the *specific* item/coin system, not on how many test cases happened to pass; the classic `[1, 3, 4]`, amount `6` counterexample shows greedy failing on an entirely reasonable-looking input.
- "Unbounded Knapsack requires the problem to explicitly say 'unlimited' or 'reusable.'" [problems/04-integer-break.cpp](problems/04-integer-break.cpp) is a direct counterexample: the reuse is structural (`dp[i-j]` may reselect `j`), never stated as "unlimited supply" anywhere in the problem.

## Summary

- Unbounded Knapsack solves the same capacity-constrained maximize/minimize/count problem as 0/1 Knapsack, except each item/coin/piece may be reused an unlimited number of times.
- The recurrence is a one-line variation of 0/1 Knapsack's: `dp[w]` may read `dp[w - weight[i]]` after it has already potentially been updated using item `i` in this same pass.
- That "already potentially updated" behavior comes entirely from filling the capacity axis **forward** (increasing) — the single most important implementation detail distinguishing this pattern from 0/1 Knapsack's **backward** fill.
- Three problem flavors share this mechanism: maximize value (rod cutting), minimize count to an exact target (coin change, perfect squares), and count distinct combinations to an exact target (coin change II) — differing only in the combine rule, base case, and (for counting) loop-nesting order.
- Complexity is O(n * capacity) time, O(capacity) space — the **same asymptotic class** as 0/1 Knapsack; the entire distinction between the two patterns is correctness-affecting, not performance-affecting.
- Counting-combinations problems require the item/coin loop **outside** the capacity loop, or they silently count permutations instead — a distinct, separate pitfall from the fill-direction one.
- Real systems use this shape for change-making, cutting-stock/manufacturing yield optimization, and any "minimum/maximum/count under a capacity built from a reusable resource type" problem.

## Key Takeaways

1. Unbounded Knapsack = 0/1 Knapsack's recurrence, with the one-line change that taking an item does not remove it from future consideration.
2. Brute-force recursion without memoization re-explores identical remaining-capacity states exponentially — the classic overlapping-subproblems failure mode.
3. `dp[w]` means "best value / fewest items / number of ways for capacity or exact target `w`," built from smaller, already-finalized capacities.
4. The capacity axis is filled **forward** (increasing) — this is what allows `dp[w - weight[i]]` to already reflect a previous use of item `i`, which is the entire reuse mechanism.
5. 0/1 Knapsack fills the same-shaped 1D array **backward**, specifically to prevent that same reuse — this single fill-direction difference is the most important contrast to internalize in this whole module.
6. Three flavors share the mechanism: maximize value (`max`, `dp[0]=0`), minimize count to exact target (`min`+1, `dp[0]=0` but other cells start "unreachable"), and count distinct combinations (`+=`, `dp[0]=1`, coin loop outer).
7. Counting distinct combinations specifically needs the item/coin loop on the **outside** of the capacity loop, or it silently counts permutations instead — a separate pitfall from the fill-direction one.
8. Complexity is O(n * capacity) time, O(capacity) space — the same asymptotic class as 0/1 Knapsack; only correctness, not performance, hinges on getting the pattern right.
9. Still pseudo-polynomial: a huge capacity value (not a huge item count) can still make the table infeasible, regardless of how few items there are.
10. Greedy is only correct for specific item/coin systems with a provable exchange property (not all currency systems, and not arbitrary denominations) — default to DP unless you can prove greedy safety for your specific system.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — the standard reference for dynamic programming foundations, optimal substructure, and overlapping subproblems.
- *Algorithms* — Sanjoy Dasgupta, Christos Papadimitriou, Umesh Vazirani — has a clear, concise treatment of knapsack-style DP recurrences.
- *Competitive Programmer's Handbook* — Antti Laaksonen — includes a direct, practical treatment of both bounded and unbounded knapsack-style DP, and coin-change-style problems specifically.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes worked knapsack and coin-change-family DP problems with C++-specific implementation notes.
- *Dynamic Programming for Coding Interviews* — Meenakshi & Kamal Rawat — a problem-pattern-organized treatment (similar philosophy to this repo) covering knapsack variants explicitly.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including unbounded-knapsack-style problems (coin change, rod cutting) implemented directly, useful for seeing varied implementation styles.
- `kamyu104/LeetCode-Solutions` — a large, well-organized collection of LeetCode solutions across languages, useful for cross-checking alternate implementations of Coin Change, Coin Change II, Perfect Squares, and Integer Break specifically.

**Official Documentation**
- LeetCode — Coin Change (problem 322).
- LeetCode — Coin Change II (problem 518).
- LeetCode — Perfect Squares (problem 279).
- LeetCode — Integer Break (problem 343).
- cppreference.com — `std::vector`, `<climits>` (`INT_MAX`) — the standard library facilities used directly in this module's dp table and sentinel handling.

**Blog Articles**
- GeeksforGeeks — "Unbounded Knapsack (Repetition of items allowed)" — a widely used explainer directly covering this pattern and its contrast with 0/1 Knapsack.
- NeetCode — Dynamic Programming pattern videos/playlist, including Coin Change and Coin Change II walkthroughs with visual explanations of the dp table fill.
- Educative.io — "Grokking Dynamic Programming Patterns for Coding Interviews," Unbounded Knapsack chapter — one of the most widely referenced pattern-based framings of this exact technique, and part of the inspiration for organizing DSA study by pattern rather than by individual problem.
- Aditya Verma's Dynamic Programming YouTube playlist (Knapsack series) — a well-known, thorough walkthrough of exactly the 0/1-vs-unbounded distinction and the fill-direction reasoning, widely referenced in interview-prep communities.
