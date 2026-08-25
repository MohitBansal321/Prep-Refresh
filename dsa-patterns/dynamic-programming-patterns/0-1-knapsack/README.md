# 0/1 Knapsack


> **In one line:** `dp[i][w]` = best value using the first `i` items within capacity `w` — either skip item `i`, or take it and add its value to the best answer for the remaining capacity. Each item is used at most once.

```cpp
std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));

for (int i = 1; i <= n; ++i) {
  for (int w = 0; w <= capacity; ++w) {
    dp[i][w] = dp[i - 1][w];                          // skip item i-1
    if (weights[i - 1] <= w) {
      dp[i][w] = std::max(dp[i][w], dp[i - 1][w - weights[i - 1]] + values[i - 1]);   // take it
    }
  }
}
return dp[n][capacity];
```

**O(n · capacity)** time and space. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Choose a subset of items, each usable **at most once**, to maximize total value while never exceeding a fixed capacity constraint.

## Real Life Analogy

Think of **packing a suitcase with a strict airline weight limit** — say 20 kg. You have a laptop (2.3 kg), hiking boots (1.4 kg), a stack of books (3 kg), a camera kit (2 kg), and for each you decide: in the suitcase, or left at home. There is no third option — you cannot bring "60% of the laptop" to save weight. Every item is **whole or absent**.

Contrast that with a market stall selling **rice, gold dust, or olive oil by weight**: if you have room for 2 kg more and a sack of rice weighs 5 kg, you simply scoop out 2 kg of it. The goods are divisible, so taking a fraction is perfectly sensible — this is the **fractional knapsack** problem, solved by an entirely different, much simpler greedy method (see Why Not Other Approaches). The indivisibility of the suitcase — pack it whole, or don't pack it at all — is the entire reason 0/1 Knapsack needs a fundamentally different algorithm than its fractional cousin.

0/1 Knapsack is exactly this suitcase-packing decision, formalized: a fixed capacity, a pile of indivisible items each with its own weight and value, and the goal of walking away with the most valuable combination that still zips shut.

## Problem

Formally: given `n` items, each with a weight `w_i` and a value `v_i`, and a knapsack of capacity `W`, choose a subset `S` of item indices maximizing `sum(v_i for i in S)` subject to `sum(w_i for i in S) <= W`, where each index appears in `S` **zero or one** times — never more. That binary in/out decision, not a fraction and not an unbounded count, is the "0/1" in the name.

> **Term: Subset.** Any selection of zero or more items from a set, without regard to order. A set of `n` items has `2^n` subsets — the reason "try every subset" is exponential the moment `n` grows past a few dozen.

The brute-force way to solve this enumerates **every subset**, discards the ones whose weight exceeds `W`, and keeps the highest-value survivor — exactly the same `2^n` enumeration performed by the **Subsets** pattern ([../../recursion-backtracking-patterns/subsets/](../../recursion-backtracking-patterns/subsets/)), just with a weight filter and a value-maximization step bolted on afterward.

Three things make it hard to do better by hand:

- **The naive recursion recomputes the same subproblem many times.** Two different orders of "include/exclude" decisions can land on the identical state "items `{1,2}` considered, `7` capacity left." The recursion tree branches into `2^n` leaves, but the number of *distinct* `(item index, remaining capacity)` states it visits is only `n * (W+1)` — far smaller. Redoing the same state without remembering the answer is exactly the "overlapping subproblems" signature DP exists to eliminate.
- **Greedy by value-to-weight ratio is tempting and wrong**, and proving why is the hard part — it is even provably *correct* for the fractional version (see Why Not Other Approaches for the counterexample and the exchange-argument reasoning behind the split).
- **Capacity must become an array index, not just a count.** "Capacity remaining" ranges over every integer from `0` to `W`; if `W` is enormous (currency in cents running into the billions), the table-based approach becomes infeasible even though the recurrence itself stays correct (see Tradeoffs: pseudo-polynomial time).

Ignoring this costs real systems money: at `n = 30` items, `2^30` is over a billion subsets to check; at `n = 50`, `2^50` is over a quadrillion — a CPU checking a billion subsets/second would still take 13+ days. Any resource-allocation service recomputing an optimal subset per request (which of these ad campaigns fits this remaining budget slot?) using brute force blows through latency budgets long before `n` gets anywhere near "large."

## Solution

Define `dp[i][w]` = **the maximum total value achievable by choosing among only the first `i` items, given a remaining capacity of `w`.**

For the `i`-th item there are only ever two possibilities, each expressed purely in terms of answers already known for `i - 1` items:

- **Skip item `i`.** The best value is whatever it was using only the first `i - 1` items with the same capacity `w`: `dp[i-1][w]`. Always valid, and the *only* option if the item doesn't even fit (`weight[i] > w`).
- **Take item `i`** (only if `weight[i] <= w`). Commit `weight[i]` of capacity, gain `value[i]`, and ask the smaller subproblem: `dp[i-1][w - weight[i]] + value[i]`.

```
dp[i][w] = dp[i - 1][w]                                        if weight[i] > w
dp[i][w] = max( dp[i - 1][w],  dp[i - 1][w - weight[i]] + value[i] )   otherwise
```

Every cell's answer is the better of "don't bother with the new item" and "commit to it and ask a strictly smaller subproblem for the rest." Because both branches only ever look at row `i - 1` (never row `i`), each item's presence-or-absence is decided exactly once per column — the structural reason this recurrence enforces the 0/1 constraint rather than allowing reuse.

**Running it:** allocate the table with row 0 and column 0 pinned to zero (the base case), fill it with items as the outer loop and capacity as the inner loop so every cell in row `i` reads only the already-finished row `i - 1`, then read the answer off the bottom-right corner, `dp[n][W]`.

Because filling row `i` only ever needs row `i - 1`, the table collapses to a **single row of length `W + 1`**: for each item, sweep `w` from `W` **down to** `weight[i]` (reverse) and update `dp[w] = max(dp[w], dp[w - weight[i]] + value[i])` in place. Sweeping backward guarantees that `dp[w - weight[i]]`, when read, still holds *last item's* value rather than a value this same item's pass already overwrote; sweeping forward would let one item contribute more than once — see Common Mistakes and The Code for exactly what breaks.

## Architecture

The recurrence is realized as a **2D table**, `dp`, with `n + 1` rows and `W + 1` columns.

1. **Row index `i` (0 to `n`).** Row `i` means "using only the first `i` items" — row 0 means no items considered, row `n` means all items available. Rows are the *item* axis.
2. **Column index `w` (0 to `W`).** Column `w` means "capacity budget of exactly `w`." Columns are the *capacity* axis.
3. **Base row (`i = 0`).** `dp[0][w] = 0` for every `w` — zero items can never accumulate value, regardless of capacity.
4. **Base column (`w = 0`).** `dp[i][0] = 0` for every `i` — zero capacity fits nothing, regardless of how many items are available.
5. **Every other cell.** Computed strictly from row `i - 1`, never from row `i` itself — **information only ever flows from one row to the next row down**, never sideways. That one-directional flow is exactly what makes each item's inclusion decision happen exactly once.

**Fill order** must be items-outer, capacity-inner: by the time any cell in row `i` is computed, the entire row `i - 1` is already finished. The sweep direction *within* a row does not matter for the 2D table — every read is from the frozen row above — but this stops being true the moment the table is compressed to 1D (see Solution's space-optimization note and Common Mistakes).

## Why Not Other Approaches?

**Brute force — try every subset.** `O(2^n)` — the same exponential blowup as the Subsets pattern, because it *is* that pattern's enumeration with a value/weight filter layered on top. Correct, useless past toy sizes.

**Greedy by value-to-weight ratio — proven to NOT work for 0/1, even though it DOES work for fractional.** For the **fractional** knapsack, sorting by `value / weight` descending and greedily filling — taking as much of the best-ratio item as fits, possibly a *fraction* of the last one — is **provably optimal**: a classic exchange argument shows that if an optimal solution holds less than the maximum of the best-ratio item while holding any amount of a worse-ratio one, you can always trade a small amount of the worse for more of the better, never decreasing value. Because fractions are allowed, that trade can always be made down to an infinitesimal amount, so the argument goes through cleanly.

For the **0/1** version, the argument collapses at the last step: you cannot "trade a small amount" of a whole item. Concretely:

| Item | Weight | Value | Value/Weight |
|------|--------|-------|---------------|
| A | 10 | 60 | 6.0 |
| B | 20 | 100 | 5.0 |
| C | 30 | 120 | 4.0 |

Capacity `W = 50`. Greedy takes A (weight 10, value 60, capacity left 40), then B (weight 20, value 100, capacity left 20) — C no longer fits — for a **greedy total of 160**. The true optimum is `B + C` = weight `50`, value `220`. Greedy's early commitment to A never gets reconsidered once a strictly better combination is discovered later. This is the *generic* behavior of greedy on 0/1 Knapsack, not a rare edge case — there is no tie-break fix, because the failure is structural.

**Branch and bound / pruned backtracking.** Computing an upper bound (e.g. the fractional relaxation's value) at each partial subset and pruning branches that can't beat the best found so far often skips huge parts of the search tree in practice. But its **worst case** is still exponential — a practical speedup, not an asymptotic guarantee, and it doesn't give the clean `O(n * W)` bound the DP does.

**Net:** brute force is correct but exponential; greedy is fast (`O(n log n)`) but structurally wrong for 0/1; branch and bound helps average-case performance with no polynomial guarantee. The DP formulation is the only approach that is both *always correct* and *polynomial in `n` and `W`* — with the caveat that "polynomial in `W`" is not the same as "polynomial in the input's bit-length" (see Tradeoffs).

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart distinguishing 0/1 Knapsack from Unbounded Knapsack and from the plain Subsets pattern, based on item-reuse rules, presence of a capacity constraint, and the kind of answer being asked for.
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of the table-fill loop: the outer items loop, the inner capacity loop, and the skip-or-take decision made at every cell.
- [images/trace-diagram.md](images/trace-diagram.md) — cell-by-cell trace of the dp table for four items (weights `[1, 3, 4, 5]`, values `[1, 4, 5, 7]`) against capacity 7, the same example [code.cpp](code.cpp) uses.

## The Code

[code.cpp](code.cpp) provides two functions, generic over any `vector<int>` of weights/values and an integer capacity — not tied to one specific LeetCode problem, so the pattern's shape is visible before the worked, problem-specific solutions in [problems/](problems/):

- **`knapsack01(weights, values, capacity)`** — the direct translation of the 2D recurrence. Allocates `dp` as `(n+1) x (capacity+1)`, default-zero-initialized (already the correct base row/column). Outer loop `i` from `1` to `n`, inner loop `w` from `0` to `capacity`. Indexes `weights[i-1]`/`values[i-1]` (the `i-1` shift, because the raw arrays are 0-indexed while `dp`'s row `i` counts "items considered" — a common off-by-one, flagged in Common Mistakes): copies `dp[i-1][w]` across if the item doesn't fit, else takes `max(dp[i-1][w], dp[i-1][w-weights[i-1]]+values[i-1])`. Returns `dp[n][capacity]`. Useful whenever you also need to reconstruct *which* items were chosen, which needs the full table (or a parallel choice table, not implemented here since none of the four worked problems require it).
- **`knapsack01Optimized(weights, values, capacity)`** — the space-optimized 1D version: a single `vector<int> dp(capacity+1, 0)`, inner loop `w` from `capacity` **down to** `weights[i]` — the reverse sweep is the entire point, and a comment directly above the loop explains what would break if the direction were flipped (tying back to Common Mistakes). Updates `dp[w] = max(dp[w], dp[w-weights[i]]+values[i])` in place; returns `dp[capacity]`.
- **`main()`** — runs both against: the four-item running example (`weights=[1,3,4,5]`, `values=[1,4,5,7]`, `capacity=7`, expected `9`, matching the Trace Diagram); the greedy-fails counterexample (`weights=[10,20,30]`, `values=[60,100,120]`, `capacity=50`, expected `220`, not greedy's `160`); a `capacity=0` edge case (expected `0`); and a case where no item fits (expected `0`). Every case prints `[PASS]`/`[FAIL]` and cross-checks the two functions agree, proving the space optimization preserves correctness.

Each file in [problems/](problems/) is a complete, standalone solution (not calling into `code.cpp`) — see [problems/README.md](problems/README.md). All four are 0/1 Knapsack in disguise once reformulated: `01` Partition Equal Subset Sum, `02` Last Stone Weight II, `03` Target Sum, `04` Ones and Zeroes (see Interview Discussion for the two-dimensional-capacity twist that last one adds).

## Tradeoffs

**What the DP table buys you**

- **Polynomial time where brute force is exponential.** `O(n * capacity)` versus `O(2^n)` — for `n = 50` and capacity `1000`, that's 50,000 table cells versus over a quadrillion subsets.
- **Always correct, unlike greedy**, which is faster (`O(n log n)`) but structurally wrong for 0/1 — the DP never falls into the "locally best choice blocks the global optimum" trap (see Why Not Other Approaches).
- **Space-optimizable to `O(capacity)` at zero cost to correctness**, via the reverse-sweep 1D array.
- **Generalizes across a whole family of problems by reformulation.** Partition Equal Subset Sum, Last Stone Weight II, Target Sum, and Ones and Zeroes are all 0/1 Knapsack once you find the right reformulation — a transferable recognition skill, not a one-off trick.
- **The recurrence doubles as a correctness proof**: each cell is defined in terms of a strictly smaller, well-defined subproblem, so an inductive argument falls out almost for free — useful when asked to justify the DP, not just state it.

**What it costs you**

- **Pseudo-polynomial time — not a minor footnote.** `O(n * capacity)` is polynomial in the *numeric value* of capacity, not in the bits needed to represent it: a capacity of `1,000,000,000` needs ~30 bits to write down but would need on the order of a billion columns times `n` rows to actually build — infeasible even though the number itself is an ordinary 32-bit integer. This degrades badly once capacity is astronomically large (currency in cents, gram-precision weights in the billions).
- **Recovering the chosen subset costs extra** — the maximum *value* alone doesn't tell you *which* items were picked; that needs either the full 2D table (defeating the 1D optimization) or a parallel bookkeeping structure.
- **Requires integer (or otherwise discretizable) weights**, since capacity is an array index — real-valued weights need rounding/scaling first, introducing precision tradeoffs.
- **The full 2D table costs real memory even though it's polynomial** — before optimizing to 1D, `O(n * capacity)` can be a meaningful constraint in memory-limited environments (embedded systems, many concurrent instances of the algorithm).
- **Versus a purely combinatorial approach**, this requires capacity to be small enough to serve as an array dimension; if it's astronomically large the table itself becomes the bottleneck, and a different technique (meet-in-the-middle for small `n`, or an approximation) may be needed instead.

## Complexity

**Time:** `O(n * capacity)` — the table has `(n+1) * (capacity+1)` cells, each O(1) work. Holds for both the 2D and space-optimized 1D version; the 1D version does the same total work in less memory.

**Space:** `O(n * capacity)` for the 2D table; `O(capacity)` for the 1D optimization, since only the previous row's values are ever needed at once.

| Approach | Time | Space |
|---|---|---|
| Brute force (all subsets) | O(2^n) | O(n) (recursion depth) |
| 0/1 Knapsack DP (2D table) | O(n * capacity) | O(n * capacity) |
| 0/1 Knapsack DP (1D, space-optimized) | O(n * capacity) | O(capacity) |
| Greedy by ratio (fractional only — wrong for 0/1) | O(n log n) | O(1) extra |

The DP trades a modest amount of space for an exponential-to-polynomial time improvement — almost always worth it once `n` exceeds roughly 20-25 in a real system.

## Common Mistakes

- **Iterating capacity forward instead of backward when space-optimizing to 1D.** This is the single most common bug in a 1D implementation, and a *silent* one — the code compiles, runs, and produces a plausible-looking (but wrong) number. Concretely: one item with `weight=1, value=10`, `capacity=3`. Sweeping `w` from `1` to `3` (forward): `dp[1] = max(dp[1], dp[0]+10) = 10`, then `dp[2] = max(dp[2], dp[1]+10) = 20` (reading the *already updated* `dp[1]`, reflecting the item taken *again*), then `dp[3] = 30` — the item got counted three times, silently turning 0/1 into **Unbounded** Knapsack. Sweeping `3` down to `1` (backward) reads `dp[w-weight]` *before* the current item's pass touches it, correctly capping the contribution to once. *Avoid:* always sweep capacity high-to-low in the 1D version; if unsure which direction is correct, re-derive it from the 2D recurrence — the 1D update must simulate reading from "the previous row," and only a backward sweep guarantees that.
- **Off-by-one on the dp table's extra row/column for the base case.** The table must be sized `(n+1) x (W+1)`, not `n x W` — row 0 and column 0 are the base case the recurrence bottoms out on, not wasted space. A common symptom is indexing `weights[i]`/`values[i]` when `i` is actually the *row* index (offset by one from the 0-indexed item arrays) — this module's code consistently uses `weights[i-1]`/`values[i-1]` for exactly this reason. *Avoid:* write out, before coding, exactly what `dp[0][*]` and `dp[*][0]` mean, and keep the row-to-item-index shift explicit in a comment where you index the raw arrays.
- **Confusing this pattern with Unbounded Knapsack when a problem allows reuse.** If the problem allows an item, coin, or piece to be used more than once, the recurrence changes (see Similar Patterns) — applying 0/1 logic (or its reverse-sweep optimization) to an unbounded-reuse problem under-counts the achievable value.
- **Forgetting that greedy is not a valid fallback "when the DP is too slow."** It is not a bounded approximation for 0/1 Knapsack in general — the worked counterexample loses over 20% of optimal value on a 3-item example. If the true DP is infeasible due to a huge capacity, the correct fallback is a different exact or approximate technique (meet-in-the-middle, or a proven approximation scheme), not greedy.

## When To Use

- The problem gives a set of items, each with a weight/cost and a value, plus a **hard capacity constraint**, and each item is usable **at most once**.
- The problem can be reformulated as "does some subset sum to exactly `X`?" (a feasibility variant where `value = weight`) — Partition Equal Subset Sum and Last Stone Weight II are both this shape.
- The problem is "count the number of subsets achieving property `X`" (a counting variant, replacing `max` with `+=`) — Target Sum is this shape.
- The capacity (or target sum) is small enough — realistically, tens of thousands to low millions — that an `O(n * capacity)` table is actually feasible to build.
- You need a **provably optimal** answer, not a fast approximation, and greedy has already been ruled out by a counterexample or by the problem's own structure.

## When NOT To Use

- **Items can be used an unlimited number of times.** That's Unbounded Knapsack's territory (see Similar Patterns); applying 0/1 logic here under-counts the true optimum.
- **The capacity is astronomically large**, making even the `O(n * capacity)` table infeasible (see Tradeoffs: pseudo-polynomial time). Look for a different structural property instead — small `n` with meet-in-the-middle, a closed form, or an approximation scheme — rather than forcing the standard table.
- **You only need to enumerate all valid subsets, not optimize a value.** That's the plain Subsets pattern ([../../recursion-backtracking-patterns/subsets/](../../recursion-backtracking-patterns/subsets/)) — no capacity constraint or value to maximize, just every combination.
- **The problem has no natural "weight" and "value" axis at all.** If nothing resembles a resource consumed under a budget, the shape doesn't fit regardless of how "include this or not" the decision otherwise feels — verify the constraint really behaves like a capacity, not just a filter.

## Where This Shows Up

- **Resource allocation under a fixed budget.** Deciding which of a backlog of candidate projects to greenlight from a fixed budget (e.g. headcount-weeks), each fundable at most once, with an estimated cost and business value.
- **Portfolio selection with a capital constraint.** Choosing which investment opportunities to fund from a fixed capital pool when each is an all-or-nothing commitment — a fixed-size private placement, not a fractionally-purchasable stock.
- **Cloud instance / container scheduling for value, not just fit.** Deciding which jobs to schedule onto a node with fixed CPU/memory when jobs can't be split and each has a business-priority value — 0/1 Knapsack asks "what's the most valuable combination that fits," where plain bin-packing only asks "does it fit."
- **Cargo/freight loading** — the namesake problem itself: which indivisible containers or pallets to load onto a truck with a fixed weight/volume limit to maximize shipped value.
- **A feature-flag rollout budget planner** — given a fixed risk/QA-hours budget for a release and candidate features each with an estimated cost and value, compute the highest-value subset that fits.
- **A CDN/edge cache pre-warming selector** — given a fixed cache capacity and candidate objects with sizes and predicted hit-values, choose which to pre-warm to maximize expected hit value.
- **A CI test-suite selector** — given a fixed per-commit time budget and candidate test suites each with a run-time cost and a flakiness/coverage-derived "bug-catching value," select the highest-value subset to run.
- **An ad-spend allocator** — given a fixed daily spend cap and candidate campaigns each requiring a fixed all-or-nothing spend with an estimated return, choose which to run that day.
- **A batch job scheduler** — given a fixed nightly compute window and candidate jobs with an estimated run time and priority value, select the highest-value subset that fits.

## Similar Patterns

- **Unbounded Knapsack** ([../unbounded-knapsack/](../unbounded-knapsack/)): the identical capacity-constrained include/maximize shape, but each item may be used **any number of times**. The recurrence changes from reading `dp[i-1][w-weight[i]]` (previous *item*, forcing a one-time use) to reading `dp[i][w-weight[i]]` (the *same* item's row, allowing it again) — a one-symbol change with a completely different meaning. In 1D form this is also why Unbounded Knapsack sweeps capacity **forward** while 0/1 Knapsack must sweep **backward**.
- **Subsets** ([../../recursion-backtracking-patterns/subsets/](../../recursion-backtracking-patterns/subsets/)): the brute-force enumeration this pattern's DP replaces. Subsets asks "generate every subset"; 0/1 Knapsack asks "find the best subset under a capacity constraint" — the DP is only possible because the question narrows from "show all `2^n` answers" to "give one optimal number," which admits overlapping-subproblem reuse full enumeration can't exploit.
- **Bounded Knapsack** (not covered as its own module): a middle ground where each item has a fixed reuse limit greater than one — solvable by duplicating each item `limit` times and running 0/1 Knapsack, or with a more advanced binary/decomposition trick.

| Pattern | Item reuse | Recurrence reads from | 1D sweep direction | Primary question |
|---|---|---|---|---|
| 0/1 Knapsack | At most once | Previous item's row (`dp[i-1][...]`) | Capacity **backward** (high to low) | "Best value under capacity, each item once?" |
| Unbounded Knapsack | Unlimited | Same item's row (`dp[i][...]`) | Capacity **forward** (low to high) | "Best value under capacity, items reusable?" |
| Subsets | At most once, no capacity | N/A (full enumeration, no table) | N/A | "Every possible subset?" |

## Interview Discussion

Experienced engineers rarely spend time on "write the recurrence" alone — that's table stakes. What they probe is whether you can **explain why greedy fails** with the specific exchange-argument reasoning (not just "it doesn't always work"), and **recognize the pattern in disguise** when a problem never says "knapsack," "weight," "value," or "capacity" (Partition Equal Subset Sum, Target Sum, Last Stone Weight II).

Follow-ups worth rehearsing:
- *"Can you reduce the space complexity?"* — expects the 1D array with the reverse capacity sweep, and *why* it must be reversed, not just "because that's how it's usually written."
- *"How would you recover the actual subset chosen, not just the maximum value?"* — expects the full 2D table (not the 1D-optimized version) or a parallel choice-tracking structure, walking backward and comparing `dp[i][w]` to `dp[i-1][w]` to infer whether item `i` was taken.
- *"How is 'Ones and Zeroes' (LeetCode 474) related to this pattern?"* — expects recognizing it as a **two-dimensional-capacity** 0/1 Knapsack (capacity is a pair `(m zeros, n ones)`, not one number), with the table gaining a third dimension and the reverse sweep needing to happen on *both* capacity axes.

Misconceptions worth killing early:
- **"The DP table approach is always better than greedy."** Greedy is strictly better (faster, and *correct*) for the **fractional** version — the DP exists specifically because the 0/1 constraint breaks greedy's correctness, not because DP is universally superior.
- **"0/1 and Unbounded Knapsack are basically the same algorithm with a minor tweak."** The tweak (which row the recurrence reads from, which direction the sweep goes) is minor to *write* but is the entire mechanism enforcing "used once" versus "used unlimited times" — getting it backward silently produces the wrong problem's answer, not a slightly-off answer to the right problem.
- **"You need to literally see the words 'weight,' 'value,' and 'capacity' to recognize this pattern."** Many of the most commonly asked problems in this family never use any of those words — recognizing the *shape* (choose a subset, each element used once, under a numeric constraint) is the actual skill.

## Key Takeaways

1. 0/1 Knapsack = choose a subset, each item used at most once, maximizing value under a capacity constraint.
2. Brute force is `O(2^n)` — the same enumeration as the Subsets pattern, just filtered and value-maximized.
3. Greedy by value/weight ratio is correct for **fractional** knapsack but provably wrong for **0/1** knapsack — know the counterexample, not just the claim.
4. Core recurrence: `dp[i][w] = max(dp[i-1][w], dp[i-1][w-weight[i]] + value[i])`, base case `dp[0][*] = dp[*][0] = 0`.
5. Fill order is items outer, capacity inner — each row depends only on the previous, already-finished row.
6. The 1D space optimization requires sweeping capacity **backward** (high to low); sweeping forward silently converts the problem into Unbounded Knapsack.
7. Time and space are `O(n * capacity)` (or `O(capacity)` space with the 1D optimization) — **pseudo-polynomial**, which breaks down when capacity is astronomically large.
8. Many famous problems (Partition Equal Subset Sum, Target Sum, Last Stone Weight II, Ones and Zeroes) are 0/1 Knapsack in disguise — recognizing the reformulation is the real skill.
9. Recovering the *chosen subset* (not just its value) requires the full 2D table or extra bookkeeping — the space-optimized version alone cannot do it.
10. 0/1 versus Unbounded Knapsack differs by exactly which row the recurrence reads from (previous item vs. same item) — a one-symbol change with a completely different meaning.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — the standard reference for both the 0/1 Knapsack DP formulation and the greedy exchange-argument proof for fractional knapsack, presented side by side.
- *Algorithm Design* — Jon Kleinberg & Éva Tardos — a clear treatment of dynamic programming foundations, including knapsack-style problems and the general "define subproblem, find recurrence, choose fill order" methodology this module follows.
- *Competitive Programmer's Handbook* — Antti Laaksonen — a concise, practical treatment of the knapsack DP family (including space optimization) as used in competitive programming.
- *Dynamic Programming for Coding Interviews* — Meenakshi & Kamal Rawat — an interview-focused treatment covering the knapsack DP pattern family (0/1, unbounded, and their many disguised variants) in depth.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including 0/1 Knapsack and related DP implementations, useful for comparing implementation styles.
- `kadane-and-friends`-style DP problem-set repositories on GitHub tagged "dynamic-programming" and "knapsack" — searching GitHub for "0-1 knapsack" surfaces many independent implementations worth comparing for style and edge-case handling.

**Official Documentation**
- LeetCode — Partition Equal Subset Sum (problem 416).
- LeetCode — Target Sum (problem 494).
- LeetCode — Last Stone Weight II (problem 1049).
- LeetCode — Ones and Zeroes (problem 474).
- cppreference.com — `std::vector` and `std::max` — the standard library building blocks used throughout this module's code.

**Blog Articles**
- GeeksforGeeks — "0/1 Knapsack Problem" — a widely used explainer covering the recurrence, the 2D table, and the space-optimized version.
- NeetCode — Dynamic Programming pattern videos/playlist, including a dedicated walkthrough of the 0/1 Knapsack shape and several of its disguised LeetCode variants.
- Educative.io — "Grokking Dynamic Programming Patterns" — 0/1 Knapsack chapter, one of the most widely referenced pattern-based framings of this exact family of problems (the same "study by pattern" philosophy this repository follows).
- Codeforces / competitive-programming community blogs on "why greedy fails knapsack" — several widely-shared writeups walk through the fractional-vs-0/1 exchange argument distinction in more depth than a single counterexample, for readers who want the full proof sketch.
