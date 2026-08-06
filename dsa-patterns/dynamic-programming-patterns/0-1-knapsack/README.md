# 0/1 Knapsack

## Intent

Choose a subset of items, each usable **at most once**, to maximize total value while never exceeding a fixed capacity constraint.

## Real Life Analogy

Think of **packing a suitcase with a strict airline weight limit** — say 20 kg. You are standing in your bedroom with a pile of things you would like to bring: a laptop (2.3 kg, high value to you), a pair of hiking boots (1.4 kg), a stack of books (3 kg), a camera kit (2 kg). You must decide, for each item, whether it goes in the suitcase or stays home. There is no third option. You cannot bring "60% of the laptop" to save weight — a laptop with its screen sawn off is worthless. Every item is **whole or absent**; it is either fully packed or fully left behind.

Contrast this with a market stall selling **rice, gold dust, or olive oil by weight**. If you have room for 2 kg more and a sack of rice weighs 5 kg, you simply scoop out 2 kg of it — the goods are divisible, and taking a fraction of them is perfectly sensible (this is the **fractional knapsack** problem, and it is solved by an entirely different, much simpler greedy method described below). A suitcase full of discrete belongings is not like that stall. The indivisibility — pack it whole, or don't pack it at all — is the entire reason 0/1 Knapsack needs a fundamentally different algorithm than its fractional cousin, and it is the detail every "why doesn't greedy work here?" question in this module traces back to.

The 0/1 Knapsack pattern is exactly this suitcase-packing decision, formalized: a fixed capacity, a pile of indivisible items each with its own weight and value, and the goal of walking away with the most valuable combination that still zips shut.

## Problem

### What engineering problem exists?

Formally: you are given `n` items, each with a weight `w_i` and a value `v_i`, and a knapsack (container) of capacity `W`. You must choose a subset `S` of the item indices that maximizes `sum(v_i for i in S)` subject to `sum(w_i for i in S) <= W`, where each item index can appear in `S` **zero or one** times — never more. The "0/1" in the name is exactly this: for each item, the decision variable is binary (0 = leave it out, 1 = take it), not a continuous fraction and not an unbounded count.

The brute-force way to solve this is to enumerate **every possible subset** of the `n` items, discard the subsets whose total weight exceeds `W`, and keep the one with the highest total value among what remains. Since each of the `n` items independently has exactly two states (in or out), and these choices are made independently of one another, the number of distinct subsets is `2 * 2 * ... * 2` (`n` times) = `2^n`. This is precisely the same enumeration performed by the **Subsets** pattern (`../../recursion-backtracking-patterns/subsets/`) — 0/1 Knapsack's brute force *is* the Subsets brute force, just with a weight filter and a value-maximization step bolted on afterward instead of returning every subset as-is.

> **Term: Subset.** Any selection of zero or more items from a set, without regard to order. A set of `n` items has `2^n` subsets, including the empty subset and the full set itself — this count is why "try every subset" is an exponential-time strategy the moment `n` grows past a few dozen.

### Why is this problem difficult?

- **The naive recursive solution recomputes the same subproblem many times.** If you write the brute-force recursion as "decide item `i`: try including it, try excluding it, recurse on the rest," you will notice that two *different orders of decisions* can land you in an identical situation: "I have considered items `{1, 2}` already and have 7 units of capacity left" is reached whether you decided item 1 then item 2, or — in a different recursive path — arrived at the same `(items considered so far = 2, capacity remaining = 7)` state through a different subset of earlier choices. The recursion tree branches into `2^n` leaves, but the number of *distinct* `(item index, remaining capacity)` states it visits is only `n * (W + 1)` — far smaller. Recomputing the same state repeatedly, without remembering the answer, is exactly the "overlapping subproblems" signature that DP exists to eliminate.
- **The greedy instinct is tempting and wrong, and proving *why* it's wrong is the hard part.** "Just take the items with the best value-per-weight ratio first" sounds reasonable and, as covered below, is even *provably correct* for the fractional version of this problem. That it fails for the 0/1 version is not obvious until you see a concrete counterexample — and even after seeing one, articulating *why* the exchange argument that makes greedy work for the fractional case breaks down for the integral case takes real understanding, not memorization.
- **Capacity is a resource, not just a count, and it must become an array index.** Unlike "the first `i` items," which is naturally a small integer you can loop over, "capacity remaining" ranges over every integer from `0` to `W`. To turn this into a DP table you need capacity to be small enough to serve as an array dimension — if `W` is enormous (say, currency amounts in cents running into the billions), the whole table-based approach becomes infeasible even though the *recurrence* itself is still perfectly correct (see Disadvantages: pseudo-polynomial time).

### What happens if we ignore it?

- **Combinatorial explosion that makes the brute force unusable past tiny inputs.** At `n = 30` items, `2^30` is over a billion subsets to check; at `n = 50`, `2^50` is over a quadrillion — a modern CPU evaluating a billion subsets per second would still take over 13 days. Any production system with even a moderate number of candidate items (choosing which of 40 features to fund this quarter, which of 60 containers to schedule onto a node) cannot afford to enumerate subsets.
- **Wasted CPU on repeated identical work.** Even for inputs where `2^n` is "only" in the low millions, a huge fraction of that time is spent re-deriving answers to the *same* `(items considered, capacity remaining)` state over and over, because the naive recursion has no memory of what it already computed.
- **Timeouts in real systems with a hard SLA.** A resource-allocation service that recomputes an optimal subset per request (e.g., "which of these ad campaigns fits this remaining budget slot") using brute force will blow through request-latency budgets long before `n` gets anywhere near "large" in an everyday sense.

## Why Not Other Approaches?

**Brute force — try every subset.**
As established above, this is `O(2^n)` time — the exact same exponential blowup as the Subsets pattern, because it *is* the Subsets pattern's enumeration with a value/weight filter layered on top. It is correct (it genuinely checks every possibility) but useless for anything but toy input sizes. The entire value proposition of 0/1 Knapsack's DP formulation is replacing this `2^n` enumeration with an `O(n * W)` table fill — the same kind of win Two Pointers gets over nested loops, but here it's an *exponential-to-polynomial* jump rather than a quadratic-to-linear one.

**Greedy by value-to-weight ratio — proven to NOT work for 0/1, even though it DOES work for fractional.**
It is worth being precise about *why* this split exists, because it is one of the most commonly misunderstood facts in algorithms interviews.

For the **fractional** knapsack (you may take any fraction of an item, like scooping rice by weight), sorting items by `value / weight` descending and greedily filling the knapsack — taking as much of the best-ratio item as fits, then moving to the next-best ratio, possibly taking only a *fraction* of the last item you touch — is **provably optimal**. The proof is a classic exchange argument: if an optimal solution ever holds less than the maximum possible amount of the best-ratio item while holding *any* amount of a worse-ratio item, you can always trade a small amount of the worse item for more of the better one, strictly increasing (or never decreasing) total value, without changing total weight. Because fractions are allowed, you can always make this trade down to an infinitesimal amount, so the exchange argument goes through cleanly and greedy reaches the true optimum.

For the **0/1** version, that exchange argument collapses at the last step: you cannot "trade a small amount" of a whole item — you either hold it entirely or not at all. A concrete counterexample makes this vivid:

| Item | Weight | Value | Value/Weight |
|------|--------|-------|---------------|
| A | 10 | 60 | 6.0 |
| B | 20 | 100 | 5.0 |
| C | 30 | 120 | 4.0 |

Capacity `W = 50`. Greedy-by-ratio takes A first (ratio 6.0, weight 10, value 60, capacity remaining 40), then B (ratio 5.0, weight 20, value 100, capacity remaining 20) — C no longer fits (weight 30 > remaining 20) — for a **greedy total of 160**. But the true 0/1 optimum is `B + C` = weight `20 + 30 = 50` (exactly the capacity), value `100 + 120 = 220`. Greedy's locally-best-first commitment locked in item A early and never had a mechanism to reconsider that choice once it discovered, later, that A's presence was blocking a strictly better combination. This is not a rare edge case — it is the *generic* behavior of greedy on 0/1 Knapsack; there is no fix-up rule that patches it into correctness, because the failure is structural, not a bug in a particular greedy tie-break.

**Branch and bound / pruned backtracking.**
This can work well *in practice* — computing an upper bound (e.g., the fractional-knapsack relaxation's value) at each partial subset and pruning branches that cannot beat the best solution found so far often skips huge portions of the search tree on real-world data. But its **worst-case** complexity is still exponential; it is a practical speedup over raw brute force, not an asymptotic guarantee, and it does not give you the clean, predictable `O(n * W)` bound that the DP formulation does.

**Tradeoff summary:** brute force is correct but exponential; greedy is fast (`O(n log n)` for the sort) but simply **wrong** for 0/1 — not "wrong sometimes," structurally wrong whenever an early high-ratio choice blocks a better later combination; branch and bound helps average-case performance but offers no polynomial guarantee. The DP formulation below is the only approach that is both *always correct* and *polynomial in `n` and `W`* — which is precisely why it is the standard tool for this problem, with the caveat (see Disadvantages) that "polynomial in `W`" is not the same as "polynomial in the input's bit-length," a distinction that matters when `W` itself is astronomically large.

## Solution

Define `dp[i][w]` = **the maximum total value achievable by choosing among only the first `i` items, given a remaining capacity of `w`.**

The recurrence rests on one observation: for the `i`-th item, there are only ever two possibilities, and you can express the best answer at `dp[i][w]` purely in terms of answers already known for `i - 1` items:

- **Skip item `i`.** The best value is whatever the best value was using only the first `i - 1` items with the same capacity `w` — i.e., `dp[i - 1][w]`. This is always a valid option, and it is the *only* option if item `i` does not even fit (`weight[i] > w`).
- **Take item `i`** (only possible if `weight[i] <= w`). You commit `weight[i]` of your capacity to this item and gain `value[i]`. What remains is a smaller subproblem: the best value achievable from the *first `i - 1` items* with the *reduced* capacity `w - weight[i]`, i.e., `dp[i - 1][w - weight[i]] + value[i]`.

Putting it together:

```
dp[i][w] = dp[i - 1][w]                                        if weight[i] > w
dp[i][w] = max( dp[i - 1][w],  dp[i - 1][w - weight[i]] + value[i] )   otherwise
```

No code yet — the entire idea is this: **every cell's answer is the better of "don't bother with the new item" and "commit to the new item and ask a strictly smaller subproblem for the rest."** Because both branches only ever look at row `i - 1` (never row `i`), each item's presence-or-absence is decided exactly once per column — which is the structural reason this recurrence enforces the 0/1 constraint rather than allowing an item to be reused.

## Architecture

The recurrence above is realized as a **2D table**, `dp`, with `n + 1` rows and `W + 1` columns.

1. **Row index `i` (0 to `n`).** Row `i` means "using only the first `i` items" — row 0 means "no items considered at all," row `n` means "all items are available for consideration." Rows are the *item* axis.

2. **Column index `w` (0 to `W`).** Column `w` means "capacity budget of exactly `w`." Column 0 means "no capacity at all." Columns are the *capacity* axis.

3. **Base row (`i = 0`).** `dp[0][w] = 0` for every `w` — with zero items available, no value can ever be accumulated, regardless of how much capacity you have.

4. **Base column (`w = 0`).** `dp[i][0] = 0` for every `i` — with zero capacity, nothing fits, regardless of how many items are available.

5. **Every other cell (`i >= 1`, `w >= 1`).** Computed strictly from row `i - 1` using the two-choice recurrence above — never from row `i` itself. This is the single most important architectural fact about the table: **information only ever flows from one row to the next row down**, never sideways within a row. That one-directional flow between rows is exactly what makes each item's inclusion decision happen exactly once.

**Fill order.** The table must be filled with **items as the outer loop** and **capacity as the inner loop**: for `i` from 1 to `n`, for `w` from 0 to `W`, compute `dp[i][w]`. This order guarantees that by the time you compute any cell in row `i`, the *entire* row `i - 1` already holds its final, correct values — because row `i - 1` was completely finished in the previous outer-loop iteration before row `i` began. The order in which you sweep `w` *within* a fixed row does not matter for the 2D table (every read is from the frozen row above), but this stops being true the moment you compress the table to a single 1D row — see Execution Flow, step 4, and Common Mistakes.

## Execution Flow

1. **Base case.** Allocate a `(n + 1) x (W + 1)` table. Initialize row 0 and column 0 to all zeros — these represent "no items" and "no capacity" respectively, and both trivially yield zero value.

2. **Fill order.** Loop `i` from 1 to `n` (outer, items). For each `i`, loop `w` from 0 to `W` (inner, capacity). At each `(i, w)`, apply the recurrence: if `weight[i] > w`, copy `dp[i - 1][w]` straight across (the item cannot possibly fit, so it is forced to be skipped); otherwise take `max(dp[i - 1][w], dp[i - 1][w - weight[i]] + value[i])`.

3. **Read the answer.** Once every row has been filled, `dp[n][W]` — the bottom-right corner — holds the maximum value achievable using *any subset* of all `n` items within the full capacity `W`. This is the answer to the original problem.

4. **Space-optimize to a 1D array.** Notice that filling row `i` only ever needs row `i - 1` — once row `i` is done, row `i - 2` and everything before it is garbage that will never be read again. This means you do not need to keep `n + 1` full rows in memory; a **single row of length `W + 1`** is enough, *provided* you update it in the right order. For each item `i`, sweep `w` from `W` **down to** `weight[i]` (i.e., **in reverse**), updating `dp[w] = max(dp[w], dp[w - weight[i]] + value[i])` in place. Sweeping in reverse guarantees that when you read `dp[w - weight[i]]`, that cell still holds *last item's* value (the "row `i - 1`" value), because you have not yet overwritten it with this item's contribution — you only overwrite positions at or above `w` as you move downward, so `dp[w - weight[i]]` (strictly smaller, since `weight[i] >= 1`... in the typical case where weights are positive) is still untouched by the current item's pass. Sweeping **forward** would corrupt this: see Common Mistakes for exactly what breaks and why.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full flowchart distinguishing 0/1 Knapsack from Unbounded Knapsack and from the plain Subsets pattern, based on the signals a problem statement gives you (item reuse rules, presence of a capacity constraint, and what kind of answer is being asked for).

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the table-fill loop — the outer items loop, the inner capacity loop, and the skip-or-take decision made at every cell.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a concrete, cell-by-cell trace of the dp table being filled for four items (weights `[1, 3, 4, 5]`, values `[1, 4, 5, 7]`) against capacity 7 — the same example used in [code.cpp](code.cpp) and referenced throughout the Code Walkthrough below.

## Implementation

[code.cpp](code.cpp) provides two functions, both generic over any `vector<int>` of weights/values and an integer capacity — not tied to one specific LeetCode problem, so you can see the *shape* of the pattern before looking at the worked, problem-specific solutions in [problems/](problems/):

- `knapsack01(weights, values, capacity)` — the direct translation of the 2D recurrence above: a full `(n + 1) x (capacity + 1)` table, easy to read and to reason about, and useful whenever you also need to reconstruct *which* items were chosen (which requires keeping the full table, or at least a parallel "choice" table — noted but not implemented, since none of the four worked problems in this module require item reconstruction).
- `knapsack01Optimized(weights, values, capacity)` — the space-optimized 1D version described in Execution Flow step 4, using a single row of length `capacity + 1` and the mandatory reverse capacity sweep.

`main()` runs both against several hand-checkable inputs — including the classic greedy-fails counterexample from Why Not Other Approaches — and asserts the two implementations agree on every case, proving the space optimization did not change behavior.

## Code Walkthrough

**`knapsack01`** (in [code.cpp](code.cpp)). Allocates `dp` as a `vector<vector<int>>` of size `(n + 1) x (capacity + 1)`, default-initialized to zero (which is exactly the base row and base column the recurrence needs — no extra initialization code required). The outer loop runs `i` from `1` to `n`; the inner loop runs `w` from `0` to `capacity`. Inside, it checks `weights[i - 1] > w` (note the `i - 1` index shift, because `weights`/`values` are 0-indexed arrays of `n` items while `dp`'s row index `i` represents "how many items considered," a common one-off source of bugs called out explicitly in Common Mistakes) — if the item does not fit, it copies `dp[i - 1][w]` across; otherwise it takes `max(dp[i - 1][w], dp[i - 1][w - weights[i - 1]] + values[i - 1])`. Returns `dp[n][capacity]`. This function exists to show the recurrence in its most literal, least-clever form — the version you should be able to derive from the recurrence in Solution without any additional insight.

**`knapsack01Optimized`** (in [code.cpp](code.cpp)). Allocates a single `vector<int> dp(capacity + 1, 0)`. The outer loop runs over each item (by weight/value pair). The inner loop runs `w` from `capacity` **down to** `weights[i]` (inclusive) — the reverse sweep is the entire point of this function, and a comment at the loop marks exactly why. Inside, it updates `dp[w] = max(dp[w], dp[w - weights[i]] + values[i])` in place. Returns `dp[capacity]`. This function exists to demonstrate the standard space optimization used in almost every production/competitive 0/1 Knapsack implementation, and to make the forward-vs-backward distinction impossible to miss — the comment block directly above the loop explains what would break if the direction were flipped, tying straight back to Common Mistakes.

**`main()`** (in [code.cpp](code.cpp)). Runs both functions against: (1) the four-item running example (`weights = [1,3,4,5]`, `values = [1,4,5,7]`, `capacity = 7`, expected `9`) used throughout this README and in the Trace Diagram; (2) the greedy-fails counterexample (`weights = [10,20,30]`, `values = [60,100,120]`, `capacity = 50`, expected `220`, *not* the greedy answer of 160) to make the greedy-vs-DP gap concrete and testable, not just asserted in prose; (3) an edge case with `capacity = 0` (expected `0`); (4) an edge case where no item fits (all weights exceed capacity, expected `0`). Every case prints `[PASS]`/`[FAIL]` and cross-checks that `knapsack01` and `knapsack01Optimized` return the identical value, proving the space optimization preserves correctness.

## Advantages

- **Polynomial time where brute force is exponential.** `O(n * capacity)` versus `O(2^n)` — for `n = 50` items and a capacity of `1000`, that is `50,000` table cells versus over a quadrillion subsets. This is the entire reason the pattern exists.
- **Always correct, unlike greedy.** The DP recurrence considers every relevant combination implicitly (via the table's dependency structure), so it never falls into the "locally best choice blocks the global optimum" trap that dooms the ratio-greedy approach.
- **Space-optimizable with no loss of correctness.** The 1D reverse-sweep version drops space from `O(n * capacity)` to `O(capacity)`, which is often the difference between "fits comfortably in memory" and "does not," at zero cost to the answer.
- **Generalizes to an entire family of problems by reformulation.** "Can we split this array into two subsets with equal sum?" (Partition Equal Subset Sum), "how many ways can we assign +/- signs to hit a target?" (Target Sum), "what is the closest we can split stones into two piles?" (Last Stone Weight II) — all four worked problems in this module are 0/1 Knapsack in disguise, once you find the right reformulation. Recognizing that reformulation is a genuinely transferable skill, not a one-off trick.
- **The recurrence doubles as a correctness proof.** Because each cell is defined in terms of a strictly smaller, well-defined subproblem, an inductive correctness argument falls out almost for free — useful in interviews when asked to *justify* the DP, not just state it.

## Disadvantages

- **Pseudo-polynomial time — and this is not a minor footnote.** `O(n * capacity)` looks polynomial, but it is only polynomial in the *numeric value* of `capacity`, not in the size (number of bits) needed to *represent* `capacity` as an input. A capacity of `1,000,000,000` needs only about 30 bits to write down, but the DP table would need on the order of a billion columns times `n` rows — computationally infeasible even though the number "one billion" is a perfectly ordinary 32-bit integer. This is precisely what "pseudo-polynomial" means: efficient when the numbers involved are modest, but the running time grows with the *magnitude* of a numeric input rather than its *encoded length*, so it degrades badly the moment that magnitude gets large (e.g., a subset-sum problem over currency amounts in cents, or a real inventory-weight problem measured to the gram with values in the billions).
- **Reconstructing the actual chosen subset costs extra.** The functions in this module return only the maximum *value* — recovering *which* items were selected requires either keeping the full 2D table (defeating the 1D space optimization) or maintaining a parallel bookkeeping structure, adding real implementation complexity beyond the base recurrence.
- **Requires integer (or otherwise discretizable) weights.** The table's column axis is capacity, indexed as an array — this only works cleanly for integer weights and integer capacity. Real-valued weights need rounding/scaling first, which introduces its own precision tradeoffs.
- **Full 2D table costs real memory even when polynomial.** Before optimizing to 1D, `O(n * capacity)` memory can still be a meaningful constraint in memory-limited environments (embedded systems, high-concurrency services running many instances of the algorithm at once).

## Tradeoffs

**What we gain versus brute force:** we go from `O(2^n)` to `O(n * capacity)` by never re-deriving the answer to a `(items considered, capacity remaining)` state more than once — the table *is* the memoization.

**What we gain versus greedy:** correctness. Greedy is faster (`O(n log n)`) but is simply wrong for 0/1 Knapsack in the general case, as the worked counterexample shows; there is no reliable version of greedy that fixes this without effectively becoming the DP.

**What we lose versus greedy:** speed, when greedy happens to be applicable (i.e., the fractional variant) — `O(n log n)` beats `O(n * capacity)` whenever capacity is large relative to `n log n`, but that speed comes at the cost of only being *correct* for the fractional problem, not the 0/1 one.

**What we lose versus a purely combinatorial (non-table) approach:** the requirement that `capacity` be small enough to serve as an array dimension. If `capacity` is astronomically large, the DP table itself becomes the bottleneck (see Disadvantages and When NOT To Use) and a different technique — meet-in-the-middle for small `n` with huge `capacity`, or accepting an approximation — may be needed instead.

## Complexity

**Time:** `O(n * capacity)` — the table has `(n + 1) * (capacity + 1)` cells, and each cell does `O(1)` work (one comparison, one addition, one max). This holds for both the 2D and the space-optimized 1D version; the 1D version does the *same total amount of work*, just in less memory.

**Space:** `O(n * capacity)` for the straightforward 2D table; `O(capacity)` for the space-optimized 1D version, since only the previous item's row of values is ever needed at once.

**Versus brute force:** `O(2^n)` time, `O(n)` space per recursive call stack (before any memoization). The DP trades a modest amount of space for an exponential-to-polynomial time improvement — a trade that is almost always worth making the moment `n` exceeds roughly 20-25 in a real system.

| Approach | Time | Space |
|---|---|---|
| Brute force (all subsets) | O(2^n) | O(n) (recursion depth) |
| 0/1 Knapsack DP (2D table) | O(n * capacity) | O(n * capacity) |
| 0/1 Knapsack DP (1D, space-optimized) | O(n * capacity) | O(capacity) |
| Greedy by ratio (fractional only — wrong for 0/1) | O(n log n) | O(1) extra |

## Common Mistakes

- **Iterating capacity forward instead of backward when space-optimizing to 1D.** This is the single most common bug in a 1D 0/1 Knapsack implementation, and it is a *silent* one — the code compiles, runs, and produces a plausible-looking (but wrong) number. Concretely: with one item of `weight = 1, value = 10` and `capacity = 3`, sweeping `w` from `1` to `3` (forward) computes `dp[1] = max(dp[1], dp[0] + 10) = 10`, then `dp[2] = max(dp[2], dp[1] + 10) = 20` (reading the *already updated* `dp[1]`, which now reflects the item being taken *again*), then `dp[3] = max(dp[3], dp[2] + 10) = 30` — the algorithm has silently allowed the single item to be counted three times, turning the 0/1 problem into the **Unbounded** Knapsack problem by accident. Sweeping `w` from `3` down to `1` (backward) instead reads `dp[w - weight]` *before* it has been touched by the current item's pass, correctly capping the item's contribution to once. *Avoid:* always sweep capacity from high to low in the 1D version, and if you are ever unsure which direction is correct, mentally re-derive it from the 2D recurrence: the 1D update must simulate reading from "the previous row," and only a backward sweep guarantees the values you read have not yet been overwritten by the current item.
- **Off-by-one on the dp table's extra row/column for the "0 items" / "0 capacity" base case.** The table must be sized `(n + 1) x (W + 1)`, not `n x W` — row 0 and column 0 are not "wasted space," they are the base case the entire recurrence bottoms out on. A common symptom of getting this wrong is indexing `weights[i]`/`values[i]` when the loop variable `i` is actually the *row* index (which is offset by one from the 0-indexed item arrays) — this module's code consistently uses `weights[i - 1]` / `values[i - 1]` inside the loop over row `i` for exactly this reason, and getting that shift backward either crashes with an out-of-bounds access or, worse, silently reads the wrong item's weight/value. *Avoid:* write out, before coding, exactly what `dp[0][*]` and `dp[*][0]` mean in your specific problem, and keep the row-index-to-item-index shift explicit in a comment at the point where you index into the raw weight/value arrays.
- **Confusing this pattern with Unbounded Knapsack when a problem allows reuse.** If the problem statement allows an item, coin denomination, or piece to be used more than once, the correct recurrence changes (see Similar Patterns) — applying 0/1 logic (or its reverse-sweep 1D optimization) to an unbounded-reuse problem under-counts the achievable value.
- **Forgetting that greedy is not a valid fallback "when the DP is too slow."** Under time pressure, it is tempting to reach for the ratio-greedy heuristic as an approximation. It is not a bounded approximation for 0/1 Knapsack in general — there is no guarantee on how far off it can be, as the worked counterexample shows losing over 20% of optimal value on a 3-item example. If the true DP is infeasible due to a huge capacity, the correct fallback is a different exact or approximate technique (meet-in-the-middle, or a proven approximation scheme), not greedy.

## When To Use

- The problem gives a set of items, each with a weight/cost and a value, plus a **hard capacity constraint**, and each item is usable **at most once**.
- The problem can be reformulated as "does some subset sum to exactly `X`?" (a feasibility variant of 0/1 Knapsack where `value = weight` and you are asking a yes/no question rather than maximizing) — Partition Equal Subset Sum and Last Stone Weight II are both this shape.
- The problem is "count the number of subsets achieving property `X`" (a counting variant, replacing `max` with `+=` in the recurrence) — Target Sum is this shape.
- The capacity (or target sum) is small enough — realistically, in the tens of thousands to low millions — that an `O(n * capacity)` table is actually feasible to build in memory and time.
- You need a **provably optimal** answer, not a fast approximation, and greedy has already been ruled out by a counterexample or by the problem's own structure.

## When NOT To Use

- **Items can be used an unlimited number of times.** That changes the recurrence entirely — see Unbounded Knapsack in Similar Patterns. Applying 0/1 logic here under-counts the true optimum, since it forbids reusing an item that the problem explicitly allows you to reuse.
- **The capacity is astronomically large**, making even the `O(n * capacity)` table infeasible to build (see Disadvantages: pseudo-polynomial time). In that regime, look for a different structural property of the specific problem (small `n` with meet-in-the-middle, a mathematical closed form, or an approximation scheme) rather than forcing the standard table.
- **You only need to enumerate all valid subsets, not optimize a value.** That is the plain Subsets pattern (`../../recursion-backtracking-patterns/subsets/`) — there is no capacity constraint or value to maximize, just every combination.
- **The problem has no natural "weight" and "value" axis at all** — if there is nothing resembling a resource being consumed under a budget, the shape does not fit this pattern regardless of how "should I include this element or not" the decision otherwise feels; verify the constraint really behaves like a capacity, not just a filter.

## Real Interview/Production Examples

- **Resource allocation under a fixed budget.** A platform team has a fixed quarter's engineering budget (measured in, say, weeks of headcount) and a backlog of candidate projects, each with an estimated cost and an estimated business value — deciding which subset of projects to greenlight, each fundable at most once, is literally a 0/1 Knapsack instance.
- **Portfolio selection with a capital constraint.** Choosing which of several available investment opportunities (each requiring a fixed capital outlay and offering an expected return) to fund from a fixed pool of capital, where each opportunity is an all-or-nothing commitment (e.g., a fixed-size private placement, not a fractionally-purchasable public stock) is a direct 0/1 Knapsack application.
- **Cloud instance / container bin-packing for value, not just fit.** Deciding which of several candidate jobs to schedule onto a single node with fixed CPU/memory capacity, when jobs cannot be split across nodes and each job has an associated business priority/value, maps directly onto 0/1 Knapsack (as opposed to plain bin-packing, which only asks "does it fit," 0/1 Knapsack asks "what is the most valuable combination that fits").
- **Cargo/freight loading.** Deciding which shipping containers or pallets to load onto a truck or cargo hold with a fixed weight or volume limit, maximizing shipped value, where each container is indivisible — the namesake problem itself, still a real logistics optimization.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **A feature-flag rollout budget planner.** Given a fixed "risk budget" or QA-hours budget for a release window and a backlog of candidate features each with an estimated risk/cost and an estimated business value, compute the highest-value subset of features that fits the budget for that release.
2. **A CDN/edge cache pre-warming selector.** Given a fixed cache capacity at an edge node and a set of candidate objects (each with a size and a predicted hit-value), choose which objects to pre-warm into that capacity to maximize expected cache hit value.
3. **A test-suite selection tool for CI budget.** Given a fixed CI time budget per commit and a set of candidate test suites (each with a run-time cost and a "bug-catching value" derived from historical flakiness/coverage data), select the highest-value subset of suites to run within the time budget.
4. **A marketing/ad-spend allocator for a fixed daily budget.** Given a fixed daily spend cap and a set of candidate ad campaigns each requiring a fixed all-or-nothing daily spend and offering an estimated return, choose which campaigns to run that day.
5. **A batch job scheduler for a fixed compute window.** Given a fixed nightly compute-time window and a backlog of candidate batch jobs (each with an estimated run time and a business-priority value), select the highest-value subset of jobs that fits within the window.

## Similar Patterns

- **Unbounded Knapsack** (`../unbounded-knapsack/`): the identical capacity-constrained include/value-maximize shape, but each item may be used **any number of times** rather than at most once. The recurrence changes from reading `dp[i - 1][w - weight[i]]` (previous *item*, forcing a one-time use) to reading `dp[i][w - weight[i]]` (the *same* item's row, allowing it to be reconsidered again within the same row) — a one-symbol change in the recurrence with a completely different meaning. In the space-optimized 1D form, this is also exactly why Unbounded Knapsack sweeps capacity **forward** while 0/1 Knapsack must sweep **backward** — the forward sweep is precisely what 0/1 Knapsack's Common Mistakes section warns against, because forward sweeping *is* what makes an item reusable.
- **Subsets** (`../../recursion-backtracking-patterns/subsets/`): the brute-force enumeration that 0/1 Knapsack's DP replaces. Subsets asks "generate every possible subset"; 0/1 Knapsack asks "find the best subset under a capacity constraint" — the DP formulation is only possible *because* the question narrows from "show me all `2^n` answers" to "give me one optimal number," which admits overlapping-subproblem reuse that full enumeration cannot exploit.
- **Bounded Knapsack (not covered as its own module here):** a middle ground where each item has a fixed, finite reuse limit greater than one (e.g., "you have exactly 3 units of item A available") — solvable either by duplicating each item `limit` times and running 0/1 Knapsack, or with a more advanced binary/decomposition trick for efficiency.

| Pattern | Item reuse | Recurrence reads from | 1D sweep direction | Primary question |
|---|---|---|---|---|
| 0/1 Knapsack | At most once | Previous item's row (`dp[i-1][...]`) | Capacity **backward** (high to low) | "Best value under capacity, each item once?" |
| Unbounded Knapsack | Unlimited | Same item's row (`dp[i][...]`) | Capacity **forward** (low to high) | "Best value under capacity, items reusable?" |
| Subsets | At most once, no capacity | N/A (full enumeration, no table) | N/A | "Every possible subset?" |

## Interview Discussion

Experienced engineers rarely spend interview time on "write the recurrence" alone — that is table stakes. What they actually probe is whether you can **explain why greedy fails**, articulate the exchange-argument difference between fractional and 0/1 versions precisely (not just say "it doesn't always work"), and **recognize the pattern in disguise** when a problem does not literally mention weights/values/capacity but is structurally identical (Partition Equal Subset Sum, Target Sum, Last Stone Weight II — none of these say "knapsack" anywhere in the problem statement).

Common follow-up questions:
- *"Why doesn't the greedy value/weight ratio approach work here?"* — expects the specific exchange-argument reasoning (fractional trades are always possible in the continuous case, impossible in the discrete case) plus a concrete counterexample, not just "sometimes it's wrong."
- *"Can you reduce the space complexity?"* — expects the 1D array with the reverse capacity sweep, and a precise explanation of *why* the sweep must be reversed (not just "because that's how it's usually written").
- *"How would you recover the actual subset of items chosen, not just the maximum value?"* — expects recognizing that this requires either the full 2D table (not the 1D-optimized version) or a parallel choice-tracking structure, and walking backward through it comparing `dp[i][w]` to `dp[i-1][w]` to infer whether item `i` was taken.
- *"What if the capacity were a floating-point number, or astronomically large?"* — expects recognizing the pseudo-polynomial nature of the algorithm and naming the pressure points: rounding/scaling for floats, infeasible table size for huge integer capacities.
- *"How is 'Ones and Zeroes' (LeetCode 474) related to this pattern?"* — expects recognizing it as a **two-dimensional-capacity** 0/1 Knapsack (capacity is a pair `(m zeros, n ones)` rather than a single number), with the dp table gaining a third dimension and the reverse sweep needing to happen on *both* capacity axes.

Common misconceptions:
- "The DP table approach is always better than greedy." Greedy is strictly better (faster, and *correct*) for the **fractional** version of this problem — the DP is necessary specifically because the 0/1 constraint breaks greedy's correctness, not because DP is universally superior.
- "O(n * capacity) is polynomial, so it always scales fine." It is pseudo-polynomial — it scales fine only while `capacity` stays numerically modest; it degrades badly once `capacity` is astronomically large, regardless of how few bits that number takes to write down.
- "0/1 Knapsack and Unbounded Knapsack are basically the same algorithm with a minor tweak." The tweak (which row the recurrence reads from; which direction the 1D sweep goes) is genuinely minor to *write*, but it is the entire mechanism enforcing "used once" versus "used unlimited times" — getting it backward silently produces the wrong problem's answer, not a slightly-off answer to the right problem.
- "You need to literally see the words 'weight,' 'value,' and 'capacity' to recognize this pattern." Many of the most commonly asked problems in this family (Partition Equal Subset Sum, Target Sum, Last Stone Weight II) never use any of those words — recognizing the *shape* (choose a subset, each element used once, under some numeric constraint) is the actual skill.

## Summary

- 0/1 Knapsack chooses a subset of items — each usable **at most once** — to maximize value under a fixed capacity constraint.
- The brute force is `O(2^n)`, identical in structure to the Subsets pattern's full enumeration.
- Greedy-by-ratio is provably correct for the **fractional** version but provably incorrect for the **0/1** version — the exchange argument that makes greedy work requires the ability to take partial items, which 0/1 forbids.
- The DP recurrence: `dp[i][w] = max(dp[i-1][w], dp[i-1][w - weight[i]] + value[i])`, bottoming out at `dp[0][*] = dp[*][0] = 0`.
- Fill order: items outer, capacity inner; each row is built entirely from the previous, frozen row.
- Space-optimizes from `O(n * capacity)` to `O(capacity)` by keeping one row and sweeping capacity **backward** — sweeping forward silently turns 0/1 into Unbounded Knapsack.
- Time is `O(n * capacity)` — **pseudo-polynomial**, not truly polynomial in the input's bit-length, which matters when capacity is huge.
- A large family of well-known problems (Partition Equal Subset Sum, Target Sum, Last Stone Weight II, Ones and Zeroes) are 0/1 Knapsack in disguise once correctly reformulated.

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
