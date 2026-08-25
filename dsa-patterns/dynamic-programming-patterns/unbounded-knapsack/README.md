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

Think of a **vending machine making change** with an unlimited supply of each coin denomination. If it needs to give back $0.75, it doesn't have "one quarter" set aside — it has a bottomless tray, and can reach for a quarter, then another, then a third, until it's made exactly $0.75 (three quarters), or a different mix does the job in fewer total coins. The same tray, the same denomination, used more than once — that's the entire idea.

Contrast that with a **backpack you are packing for a hike** with exactly one tent, one stove, one sleeping bag: once the tent is in the pack, there is no second tent to add later. That is 0/1 Knapsack. The vending machine — same denominations, drawn from again and again — is Unbounded Knapsack. Everything else about "which items, under what capacity, to maximize what" is identical; only the "can I reach for this one again?" answer changes.

Another everyday one: a **rod-cutting shop** that cuts long rods into standard-length pieces to sell (say 1m, 3m, 4m, each with its own price). The shop doesn't run out of "the ability to cut a 1m piece" — if three 1m pieces from an 8m rod is most profitable, nothing stops it. The "item" (a cut of a given length) is reusable as many times as the rod allows.

Unbounded Knapsack is that reusable-tray, reusable-cut idea in code: the same recurrence as 0/1 Knapsack, except taking an item does not remove it from future consideration.

## Problem

A large family of "optimize under a resource constraint" problems share this shape, each item type available in unlimited supply:

- **"Best value packed into a fixed capacity"** — rod cutting, or a manufacturing "cutting stock" problem (cut raw material into standard part sizes to maximize yield).
- **"Fewest pieces to hit an exact target"** — making change with the fewest coins, or breaking an integer into the fewest perfect squares.
- **"How many distinct ways to hit an exact target"** — counting the number of distinct coin combinations summing to an amount.

> **Term: Overlapping subproblems.** When a recursive algorithm calls itself with the *same arguments* through different call paths and, without caching, redoes the same work every time — one of the two properties (with *optimal substructure*) that make a problem a good DP fit.

The naive way to answer any of these is **brute-force recursion**: try every item, subtract its weight/value from the remaining capacity, recurse on what's left — trying the same item again is exactly what makes this "unbounded." Without memoization it re-explores the same remaining-capacity state repeatedly: for coins `[1, 2, 5]`, amount `11`, the paths "1, 2, 2" and "2, 1, 2" both leave remaining capacity `6`, and naive recursion solves "best way to make 6" **twice**, from scratch.

Three things make this hard to fix by hand:

- **Reuse multiplies the number of ways to reach the same state**, since the same item can now appear any number of times in different positions of a choice sequence — 0/1 Knapsack's "at most once" caps this; unlimited reuse doesn't.
- **The recurrence looks deceptively similar to 0/1 Knapsack's.** Both are "take-or-skip" over items and a capacity axis; the one-line difference (recurse into `capacity - weight[i]` while still allowing item `i` again, versus moving to item `i+1` after taking it) is easy to blur — which is why the fill *direction* of the 1D table becomes the single most consequential implementation detail here (see Common Mistakes).
- **Exact-target variants (minimize/count) need different base cases and sentinels than maximize variants**, and mixing them up produces silently wrong answers, not crashes — `dp[w] = 0` is correct for "maximize value at *at most* capacity w" but wrong for "minimize coins to make *exactly* w."

Ignoring this is expensive: naive coin-change recursion on amount `n` with `k` denominations can branch into roughly `k^n` calls before memoization — for `n` in the thousands (a realistic invoice-total-in-cents scale), that never finishes, and a production "minimum coins/parts" service becomes unusably slow where an `O(target * items)` table would return in microseconds. Treating an unreachable exact-target capacity as `0` instead of an explicit sentinel produces a confidently wrong "0 coins needed" for genuinely unreachable amounts.

## Solution

Define `dp[w]` to mean: **the best value (or fewest items, or number of distinct ways) achievable using capacity exactly `w` or at most `w`, depending on the problem's flavor, using any number of copies of any item.**

For each capacity `w` and each item `i` that fits (`weight[i] <= w`):

```
dp[w] = combine( dp[w],  dp[w - weight[i]]  "plus/aggregated with"  item i's contribution )
```

This is **structurally identical** to 0/1 Knapsack's recurrence — either don't use item `i` at this capacity, or use it and fall back to a smaller capacity's already-known answer. The one-line change that makes this Unbounded rather than 0/1 is *what has already been computed* by the time `dp[w - weight[i]]` is read: because the capacity axis is filled **forward** (increasing `w`), smaller capacities are finalized — potentially already including item `i` itself — before larger ones, in the same pass. There is no explicit "loop again over this item" instruction anywhere; reuse is a *consequence* of reading an already-updated, possibly-already-including-this-item value at a smaller capacity.

Three shapes built on that same mechanism:

- **Maximize value** (rod cutting): `dp[w] = max(dp[w], dp[w - weight[i]] + value[i])`, `dp[0] = 0` (0 capacity really does mean 0 value — a genuine base case, not "unreachable").
- **Minimize count to an exact target** (coin change, perfect squares): `dp[w] = min(dp[w], dp[w - item] + 1)`, `dp[0] = 0` but every other capacity starts at an explicit "unreachable" sentinel, since — unlike maximize — not every target is achievable at all.
- **Count distinct combinations to an exact target** (coin change II): `dp[w] += dp[w - coin]`, `dp[0] = 1` (exactly one way to make zero: use nothing), and a loop-nesting order (coin outer, capacity inner) that is itself part of the correctness argument, not a style choice — see Common Mistakes and [problems/02-coin-change-ii.cpp](problems/02-coin-change-ii.cpp).

**Running it:** set `dp[0]` per the problem's flavor (`0` for maximize/minimize, `1` for counting) and every other cell to a value the combine rule will always improve on; fill capacity **forward**, `w` from `1` up to `capacity`/`amount` — the opposite of 0/1 Knapsack's backward 1D sweep, and the entire mechanism that makes reuse happen without any explicit "try this item again" instruction; at each `w`, apply the combine rule across every item that fits; read the answer off `dp[capacity]`/`dp[amount]` (translating an unreachable sentinel to whatever the problem wants, e.g. `-1`). For counting problems specifically, this is only correct if the combine step's outer loop is over items/coins, not capacity — see Common Mistakes.

## Architecture

1. **The `dp` table.** A single 1D array indexed by capacity, `dp[0..capacity]` (or `dp[0..amount]`). Once capacity `w` is processed, `dp[w]` holds the final answer for `w`, considering unlimited reuse of every item, and is never revisited.
2. **The items** (weights/values, coin denominations, or derived "parts"). Each is examined at every capacity it fits into. No item is ever marked "used" — the dp table, read at a smaller capacity, already encodes however many times it made sense to use this item to get there.
3. **The capacity/target axis and its fill direction.** Iterated **forward**, `0`/`1` up to `capacity`/`amount` — the one participant that behaves differently than in 0/1 Knapsack. The direction is not stylistic; it is the mechanism that permits reuse.
4. **The combine/aggregation rule** (`max`, `min` + 1, or `+=`) — turns "what does a smaller capacity already know" into "what should this capacity's answer be." This is what differs across the three problem shapes, while the reuse mechanism underneath stays the same.

## Why Not Other Approaches?

**Brute-force recursion without memoization.** Correct, but re-explores the identical remaining-capacity state through every distinct order of choices that lands on it — the overlapping-subproblems failure mode. Exponential time; for any realistic amount it simply does not finish.

**Memoized recursion (top-down DP) without ever moving to an iterative table.** A legitimate stepping stone: memoizing on `(item index, remaining capacity)` or just `remaining capacity` collapses the exponential tree down to the same `O(items * capacity)` states as the iterative version. Not *wrong*, just more machinery (recursion, a memo table, cache-key management) than a plain forward-fill loop needs, and it carries real stack-overflow risk for large capacities in a language like C++ with no guaranteed tail-call optimization.

**Greedily always take the largest/best-value item that fits.** Works for some specific coin systems (US denominations happen to make greedy correct) but is **not correct in general**. Classic counterexample: coins `[1, 3, 4]`, amount `6` — greedy takes `4`, then needs `2` more from `[1, 3]`, giving `4+1+1 = 3 coins`, while the optimum is `3+3 = 2 coins`. Greedy has no way to know, at the moment it takes the `4`, that this choice forecloses a better option later.

**Enumerate every possible multiset of item counts.** Brute force wearing combinatorics clothing — the number of multisets grows combinatorially, and it recomputes information (the best way to fill a smaller sub-capacity) a DP table stores and reuses for free.

**Net:** brute-force recursion fails to finish in reasonable time; greedy fails to be *correct* in general (not just slow); memoized recursion is correct and asymptotically equal to the iterative version but adds call-stack overhead the iterative approach avoids entirely. The 1D forward-fill table is simultaneously correct, fast, and simple to implement with no recursion machinery.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart distinguishing Unbounded Knapsack from 0/1 Knapsack, and further splitting it into its maximize / minimize-exact / count-exact variants based on the signals in a problem statement.
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of the 1D dp forward-fill loop, with the exact point where 0/1 Knapsack's backward fill diverges called out explicitly.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of the dp array on `coins = [1, 2, 5]`, `amount = 11`, including a dependency graph showing coin `5` being read twice on the path to the final answer — reuse made concrete rather than asserted in prose.

## The Code

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the pattern's shape before the four worked, problem-specific solutions in [problems/](problems/).

- **`unboundedKnapsack(weights, values, capacity)`** — the maximize-value flavor. Allocates `dp` of size `capacity+1`, all zero (a correct base case: "capacity 0 => value 0" and "no items chosen => value 0" are legitimately the same "zero" state here). Loops `w` from `1` to `capacity` (forward); for each `w`, loops over every item, applying `dp[w] = max(dp[w], dp[w-weights[i]]+values[i])` whenever it fits. Returns `dp[capacity]`.
- **`coinChangeMinCoins(coins, amount)`** — the minimize-count-to-exact-target flavor. Allocates `dp` of size `amount+1`, initialized to `INT_MAX / 2` (chosen specifically to avoid signed-integer overflow when `+1` is added), representing "unreachable," except `dp[0] = 0` (a real base case, not a sentinel). Loops `w` forward from `1` to `amount`, and for each coin `c <= w` **where `dp[w-c]` is not itself the sentinel** (guarding against building nonsense sums on top of "unreachable"), applies `dp[w] = min(dp[w], dp[w-c]+1)`. Returns `dp[amount]`, translated to `-1` if still the sentinel.
- **`main()`** — exercises both against small, hand-checkable inputs, including a capacity-0 edge case and an unreachable-amount edge case, printing `[PASS]`/`[FAIL]` for each.

The third flavor — counting distinct combinations — is deliberately left out of `code.cpp` and shown only in [problems/02-coin-change-ii.cpp](problems/02-coin-change-ii.cpp), because its correctness depends on the loop-nesting detail (coin outer, capacity inner) that deserves to be seen once and explained thoroughly rather than silently absorbed into a "generic template."

Each file in [problems/](problems/) is complete and standalone (not calling into `code.cpp`), implementing the same forward-fill recurrence inline with problem-specific comments — see [problems/README.md](problems/README.md). Briefly: `01` is the pure minimize-count-to-exact-target flavor (sibling of `coinChangeMinCoins`); `02` uses the *same inputs* as `01` but counts distinct combinations, making the loop-nesting pitfall concrete and testable; `03` shows the item list itself can be *derived* (perfect squares) rather than given; `04` (Integer Break) shows "reuse" appearing through the recurrence's own structure (`dp[i-j]` may have already chosen split-size `j` again) even when the problem never mentions a list of reusable items at all.

## Tradeoffs

**What the forward-fill table buys you**

- **Turns exponential brute-force recursion into `O(items * capacity)` table-filling** — each `(capacity)` state is solved exactly once instead of once per distinct *path* to it.
- **No recursion, no call stack, no memo-table bookkeeping** — just a flat array and two nested loops, free of the stack-depth risk memoized recursion carries for large capacities.
- **The same recurrence shape covers maximize, minimize-exact, and count-exact problems** by swapping only the combine rule and base case — rod cutting, coin change, perfect squares, and more become the same template with different aggregation.
- **Space reduces to `O(capacity)`** (a single 1D array) — unlike 0/1 Knapsack's naive 2D form, there's no need for a separate row per item, because reading "the current row, at a smaller capacity" is exactly what reuse wants.
- **Provable correctness, not heuristic** — every `dp[w]` is the actual optimum/count for that capacity, computed from actual optima at smaller capacities, unlike greedy approaches that can be subtly wrong.
- **Versus greedy (when greedy happens to be correct for a specific coin system)**: greedy is asymptotically faster (`O(items)` or `O(items log items)`), but correct only for systems (like standard currency) where an exchange property provably holds — this DP is strictly slower but correct for *every* coin/item system, trading raw speed on a narrow input class for generality and guaranteed correctness.

**What it costs you**

- **Still pseudo-polynomial in capacity.** `O(items * capacity)` sounds polynomial, but `capacity`/`amount` is a *value*, not the size of the input in bits — a capacity of `10^12` makes the table infeasible to allocate even with very few items. Same caveat as 0/1 Knapsack.
- **Easy to silently implement 0/1 semantics by mistake.** Filling the capacity axis backward (out of habit, or misremembering which pattern uses which direction) compiles, runs, and returns a plausible-looking but *wrong* number — no crash, nothing to point at the bug except a wrong answer on a test case that happens to require reuse.
- **Loop-nesting order matters for counting variants in a way it does not for maximize/minimize.** Getting the coin/capacity loop order backward in a counting problem produces a *different, larger, wrong* number (permutations instead of combinations) with no error at all.
- **Off-by-one risk in the base case is easy to get subtly wrong, and fails just as silently** — treating "amount 0" as `1` (a counting answer) when the problem wanted `0` (a minimize-count answer), or vice versa, produces a wrong final number with no indication anything is off.
- **What's lost versus 0/1 Knapsack's "each item used once" guarantee**: nothing, computationally, when unlimited reuse is genuinely what's needed — the loss only appears if this pattern's forward-fill mechanism gets applied to a problem that actually needed 0/1 semantics, silently changing the problem being solved rather than making a legitimate tradeoff.

## Complexity

**Time:** `O(n * capacity)`, where `n` is the number of distinct items/coins — `capacity` forward-fill iterations, each `O(n)` work. Holds identically for all three flavors; only the O(1) per-item work inside the loop differs (`max`, `min`+1, or `+=`).

**Space:** `O(capacity)` — a single 1D array, independent of item count. This is the **same asymptotic class** as 0/1 Knapsack's space-optimized 1D form; the entire distinguishing factor between the two patterns is the fill *direction*, not any difference in time or space bound.

| Variant | Brute-force recursion | Unbounded Knapsack DP |
|---|---|---|
| Maximize value under capacity (rod cutting) | Exponential | O(n * capacity) time, O(capacity) space |
| Minimize count to exact target (coin change) | Exponential | O(n * amount) time, O(amount) space |
| Count distinct combinations to exact target (coin change II) | Exponential | O(n * amount) time, O(amount) space |

## Common Mistakes

- **Using the same backward-capacity fill order as 0/1 Knapsack.** By a wide margin the single most important mistake to avoid in this module. Filling `w` from `capacity` down to `weight[i]` (correct for 0/1 Knapsack) means `dp[w-weight[i]]` is read *before* the current item has been applied in this pass — capping each item at one use per row, silently reproducing 0/1 semantics inside code meant to allow unlimited reuse. *Why it happens:* the two patterns' 1D code looks almost identical at a glance, and loop direction is exactly the kind of detail copied from muscle memory. *Avoid:* before writing the loop, say out loud which direction you need and why — "forward, because I need `dp[w-weight[i]]` to already reflect a possible earlier use of this same item in this same pass" — and make that sentence a mandatory comment.
- **Off-by-one on whether 0 coins/0 capacity is a valid base case returning 0, or "unreachable."** For maximize problems, `dp[0]=0` is genuinely correct. For minimize-count-to-exact-target, `dp[0]=0` is *also* correct — but every *other* unreached capacity must start at an explicit "unreachable" sentinel, not `0`, or the algorithm confidently reports "0 coins needed" for impossible amounts. For counting problems, `dp[0]=1` (one way to make zero: choose nothing) is the base case; using `0` instead makes every derived count wrong, since every other `dp[w]` traces back to `dp[0]`.
- **Getting the loop-nesting order backward in a count-distinct-combinations problem.** Looping capacity outer and coins inner (fine, and standard, for maximize/minimize-exact) counts *permutations* instead of *combinations* when counting distinct ways — see [problems/02-coin-change-ii.cpp](problems/02-coin-change-ii.cpp) for the two loop orders diverging on the same input. *Why it happens:* the recurrence body (`dp[w] += dp[w-coin]`) looks identical either way; only the nesting encodes "has this coin already been fully accounted for" versus "try every coin fresh at every capacity." *Avoid:* for counting problems specifically, always put the item/coin loop on the **outside**.
- **Forgetting to guard against building on top of an "unreachable" sentinel.** In `dp[w] = min(dp[w], dp[w-c]+1)`, if `dp[w-c]` is itself the sentinel, adding `1` produces a large-but-not-quite-sentinel number that can silently pollute later comparisons (or overflow, if the sentinel is close to the integer type's maximum). *Avoid:* check `dp[w-c] != UNREACHABLE` before using it (as [code.cpp](code.cpp) does), or pick a sentinel deliberately small enough (`INT_MAX / 2`) that a few additions can't overflow.
- **Assuming the pattern always needs an explicit "list of reusable items."** [problems/04-integer-break.cpp](problems/04-integer-break.cpp) has no coins or weighted items at all — "reuse" shows up purely through `dp[i-j]` potentially having already chosen split-size `j` again. Looking for a literal "unlimited supply" phrase and concluding "this isn't Unbounded Knapsack" when it's absent will cause you to miss variants dressed differently.

## When To Use

- The problem gives (or implies) a set of items/coins/pieces where **each can be used any number of times**, plus a capacity or exact target to respect.
- You need the **best value** achievable under a capacity limit, built from smaller capacities' already-known best values (rod cutting, cutting stock, resource allocation with a reusable resource type).
- You need the **fewest pieces** to hit an **exact** target sum (coin change, minimum perfect squares, minimum fixed-size "steps" to close a gap).
- You need to **count the distinct ways** to hit an exact target using unlimited copies of each piece (coin change II, "number of ways to climb n stairs taking 1/2/3 steps" style problems).
- You recognize the recurrence "the best/fewest/count for `i` depends on the best/fewest/count for a strictly smaller `i - something`, where `something` can repeat" — even if the problem never says "unlimited" or "reusable" (see [problems/04-integer-break.cpp](problems/04-integer-break.cpp)).

## When NOT To Use

- **Items/coins/pieces are limited-use** — each usable at most once, or a bounded small number of times. That's 0/1 Knapsack's territory ([../0-1-knapsack/](../0-1-knapsack/)); applying this pattern's reuse mechanism there silently over-counts uses of an item meant to be scarce.
- **The problem compares two different sequences** (edit distance, longest common subsequence) — that's the Longest Common Subsequence family's 2D-over-two-indices shape, not a single capacity axis.
- **The "capacity" is astronomically large relative to the number of items**, making even `O(items * capacity)` infeasible — the pseudo-polynomial ceiling; at that point you need a different technique entirely (number-theoretic reasoning about the coin system, or a different algorithm class), not a bigger dp array.
- **A locally-optimal greedy choice is provably always safe for the specific item/coin system at hand** (e.g. standard currency denominations) — in that narrow case greedy is strictly faster and DP's generality is unnecessary overhead. The catch is *proving* the greedy safety property for your specific denominations; absent that proof, default to DP.

## Where This Shows Up

Unbounded Knapsack (especially Coin Change and its variants) is one of the most frequently asked DP patterns at major tech companies, precisely because the 0/1-vs-unbounded and forward/backward fill-direction contrasts test whether a candidate understands *why* the recurrence works, rather than having memorized one problem's code.

In production systems:

- **Currency and change-making.** Point-of-sale systems, ATMs, and vending machines computing the fewest coins/bills to return as change are the textbook instance of the minimize-count-to-exact-target flavor — real currency systems are usually (not provably, in general) friendly to greedy, while a general "works for any denomination set" implementation needs the DP version.
- **Cutting stock / rod-cutting in manufacturing.** Deciding how to cut raw stock (metal bars, lumber, fabric rolls, or 1D cut-lengths from a 2D sheet) into standard part sizes to maximize yield or revenue — the maximize-value flavor, used in real production-planning and ERP software.
- **Resource allocation where a resource type is effectively unlimited** — compute instance-hours from an auto-scaling pool, standard-sized shipping containers, fixed-denomination budget line items — allocated across tasks to maximize an objective under a total-capacity constraint.
- **"Minimum number of X to reach exactly Y"** in scheduling and billing — minimum billing increments, minimum fixed-denomination gift cards to cover a balance, minimum fixed-size batches to process an exact quantity — the minimize-count flavor in different domain vocabulary. A ledger/billing utility computing the fewest line items to zero out an exact balance is this same idea.
- **A "ways to reach a target" counter** for gamified/progress-tracking features — counting distinct ways a user could have accumulated a point total from fixed reward denominations, useful for anti-fraud checks (does a claimed total have *any* valid combination of legitimate awards summing to it?).
- **A capacity-planning helper** for infrastructure sized in fixed instance/container tiers — maximum workload value schedulable from a total compute-hour budget, treating tiers as unlimited-supply items.
- **A "minimum steps to close a numeric gap" helper** for stepped-pricing/quota systems — minimum fixed-size quota top-ups to reach an exact target, directly analogous to Perfect Squares.

## Similar Patterns

**Unbounded Knapsack vs. 0/1 Knapsack** is the single most commonly confused pair in dynamic programming — the recurrence, table shape, and most surrounding code are near-identical, and the entire distinction lives in one implementation detail (fill direction) with an outsized effect on correctness.

| Aspect | 0/1 Knapsack ([../0-1-knapsack/](../0-1-knapsack/)) | Unbounded Knapsack (this module) |
|---|---|---|
| Can an item be used more than once? | No — at most once | Yes — any number of times |
| `dp` state meaning | `dp[i][w]` (or 1D `dp[w]`): best answer considering the **first `i` items**, capacity `w` | `dp[w]`: best answer for capacity `w`, considering **all** items, any number of uses each |
| 1D fill direction | **Backward** (`capacity` down to `weight[i]`), per item | **Forward** (`0`/`1` up to `capacity`), across all items each step |
| Why that direction | `dp[w-weight[i]]` reflects a state from *before* this item was considered — at most one use | `dp[w-weight[i]]` may already reflect a *previous use of this same item* in this same pass — reuse |
| Complexity | O(n * capacity) time, O(capacity) space (1D-optimized) | Same class — O(n * capacity) time, O(capacity) space |
| Canonical problems | Subset sum, partition equal subset sum, target sum | Coin change, coin change II, rod cutting, perfect squares |
| Recognition phrase | "each item usable once" | "unlimited supply" / "reused any number of times" |

- **Bounded Knapsack** (each item usable up to a fixed count `k`, not unlimited): a middle ground, solvable by treating each item as `k` separate 0/1 items (correct, but blows up item count), a binary/power-of-two decomposition to keep the item count logarithmic in `k`, or a sliding-window optimization of the unbounded recurrence. Not covered as its own module here, but worth knowing as the natural generalization sitting between 0/1 and Unbounded.
- **Longest Common Subsequence family** ([../longest-common-subsequence/](../longest-common-subsequence/)): also a 2D-table-collapsible-to-1D DP, but the two axes index **two different sequences**, not "items considered" and "capacity" — comparing two sequences, not packing under one capacity.
- **DP on Grids** ([../dp-on-grids/](../dp-on-grids/)): a 2D table indexed by an explicit row/column position rather than an abstract capacity — also "best answer reachable at this state, built from smaller states," but the state is spatial, not a resource capacity.

## Interview Discussion

Experienced engineers do not spend much time on "can you write the recurrence" — close to rote once you've seen the pattern. What they probe is whether you can **articulate, precisely, why the fill direction is what makes this pattern different from 0/1 Knapsack** — not recite "forward for unbounded, backward for 0/1" as a memorized rule, but explain that forward fill lets a smaller capacity's already-finalized answer already include a previous use of the item currently being considered.

Follow-ups worth rehearsing:
- *"How would you change this code to make it 0/1 Knapsack instead?"* — expects "flip the capacity loop to iterate backward, per item," and a clear statement of why that single change removes the possibility of reuse.
- *"Why does Coin Change II need the coin loop on the outside, when Coin Change (minimum coins) does not care about loop order?"* — expects recognizing that `min`/`max` are invariant to loop order because they only ever want "the best across all valid ways," while summing counts (`+=`) is not invariant, since it double-counts (or under/over-counts differently) depending on whether items are fully processed one at a time (combinations) or interleaved (permutations).
- *"What if the capacity were extremely large (say 10^12) but the number of item types were small?"* — expects naming an alternative direction (matrix exponentiation of the recurrence, or number-theoretic analysis specific to the coin system) rather than insisting on scaling the same dp array.
- *"Walk me through why dp[0] is 0 in one variant and 1 in another."* — expects distinguishing "zero capacity legitimately yields zero value/zero coins" (a real answer) from "there is exactly one way to make a sum of zero: use nothing" (a different kind of base case, for counting).

Misconceptions worth killing early:
- **"Unbounded Knapsack is just 0/1 Knapsack with a bigger loop."** Same asymptotic complexity class — but the correctness of "reuse" hinges entirely on fill direction, not loop count.
- **"If greedy gives the right answer on my test cases, it's correct."** Greedy correctness depends on a property of the *specific* item/coin system, not on how many test cases happened to pass — the `[1,3,4]`, amount `6` counterexample shows greedy failing on an entirely reasonable-looking input.
- **"Unbounded Knapsack requires the problem to explicitly say 'unlimited' or 'reusable.'"** [problems/04-integer-break.cpp](problems/04-integer-break.cpp) is a direct counterexample: the reuse is structural (`dp[i-j]` may reselect `j`), never stated as "unlimited supply" anywhere.

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
