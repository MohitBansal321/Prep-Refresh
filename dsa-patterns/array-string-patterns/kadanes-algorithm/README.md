# Kadane's Algorithm


> **In one line:** track the best sum of a subarray *ending exactly at* index i, restarting the running sum whenever it goes negative — a negative prefix can only drag down whatever follows it.

```cpp
long long current_sum = nums[0];
long long best_sum = nums[0];

for (size_t i = 1; i < nums.size(); ++i) {
  // Extend the run, or abandon it and restart at nums[i] alone — restarting
  // is better exactly when the carried-in run was negative.
  if (current_sum + nums[i] >= nums[i]) {
    current_sum += nums[i];
  } else {
    current_sum = nums[i];
  }
  best_sum = std::max(best_sum, current_sum);   // checked every iteration, not just on restart
}
```

**O(n)** time, one pass · **O(1)** space. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Find the maximum-sum contiguous subarray in a single linear pass, by tracking only the best sum ending at the current position and discarding it the moment it stops helping future choices.

## Real Life Analogy

Think of a **hiker tracking elevation gain on a trail** with a GPS watch that logs "net climb since the last reset." Every step goes up (positive) or down (negative), and the hiker wants the single best continuous uphill stretch of the whole trail.

The key habit a smart hiker develops: if the running counter ever drops below zero, reset it right there and start fresh — a negative running total can only *drag down* whatever climb follows, so carrying forward "I'm 200 meters below where this stretch started" never helps. The rule becomes "keep extending as long as it beats restarting; the moment it turns net-negative, abandon it and start counting from here." Meanwhile a second, separate number — the best climb recorded on any stretch so far today — only ever gets updated, never reset, because it must remember every good stretch, not just the current one. (A trader's "current losing streak" versus "best winning streak this month" is the same everyday split.)

Kadane's Algorithm is that hiker's two counters in code: a **running sum** (reset when it goes negative) and a **best sum** (never reset).

## Problem

### What engineering problem exists?

Given an array of numbers — which can include negative numbers — find the contiguous subarray (a run of consecutive elements, not any arbitrary subset) whose sum is the largest possible.

> **Term: Contiguous subarray.** A slice of consecutive elements from the original array, `arr[i..j]` for some `i <= j`, with nothing skipped — different from a *subsequence*, which can skip elements while preserving relative order. Kadane's Algorithm only answers questions about **contiguous** ranges.

The naive way checks **every possible contiguous subarray**: an **O(n³) approach** sums `arr[i..j]` from scratch for every pair of start/end indices (`~n²/2` pairs, each costing up to `O(n)`); an **O(n²) approach** extends `j` one step at a time per start index `i`, keeping a running sum — still a nested loop, just a smarter inner one. For `n = 10,000` elements (a realistic size for a day's worth of per-minute price deltas), `O(n²)` is already 100 million operations and `O(n³)` is on the order of a trillion — both wasteful for a problem with a linear-time solution.

### Why is this problem difficult?

- **The instinct to "check every subarray" is strong, because the answer really could be anywhere.** Unlike Two Pointers or Sliding Window, there's no obvious sortedness or fixed window size to exploit — positives and negatives are mixed freely.
- **Negative numbers make the "best" answer non-obvious.** If every number were non-negative the answer would trivially be "the whole array" — negatives force reasoning about *when a running total is worth carrying forward* versus *when it's actively hurting you*.
- **The key insight is a *local* decision that produces a *global* optimum.** It's not obvious that a decision using only "the sum ending exactly here," with no look-back or look-ahead, guarantees correctness for the whole array. Proving this greedy, one-pass rule correct — not just a heuristic — is the crux of understanding the algorithm rather than memorizing it.

### What happens if we ignore it?

- **Quadratic-or-worse time on a problem with a linear solution.** A batch job scanning millions of per-second sensor readings or per-trade P&L deltas for the best contiguous run takes minutes instead of milliseconds as nested loops — a real cost in a production pipeline.
- **Unnecessary memory for prefix-sum arrays.** A common "clever" fallback precomputes prefix sums and still scans pairs of them — dropping the per-subarray sum cost to O(1) but leaving an O(n²) pair scan and burning O(n) extra memory Kadane's O(1)-space approach doesn't need.
- **Missed edge cases.** Without understanding *why* the reset rule works, engineers often report `0` for an all-negative array instead of the correct least-negative single element — a correctness bug, not just a performance one (see Common Mistakes).

## Solution

The whole algorithm rests on one question, asked at every index: **"is it better to extend the current run, or to abandon it and start fresh from here?"**

Maintain two running numbers scanning left to right:

1. **`current_sum`** — the maximum sum of a contiguous subarray that **ends exactly at the current index**. At each step there are exactly two choices: **extend** the previous run (`current_sum + arr[i]`), or **abandon** it and start a brand-new run of just `arr[i]`. Take whichever is larger — there is never a need to consider "start further back," because any run starting further back and ending here would include the same trailing elements, and the two-choice comparison already captures the only decision point that matters.
2. **`best_sum`** — the maximum sum seen across *any* index scanned so far, updated by comparing it against `current_sum` after every step. Unlike `current_sum`, `best_sum` **never resets**.

The "reset" intuition from the analogy is just the extend-vs-abandon choice stated differently: extending being *smaller* than starting fresh can only happen when the sum being carried forward was itself negative — a negative prefix can only ever subtract from what comes next, so it's never worth carrying. Choosing "start fresh" is mathematically equivalent to "reset to zero, then add the current element."

The single most common bug (see Common Mistakes) hinges on one subtlety: **"reset" means reset to the current element, not literally to zero.** If every number is negative, resetting `current_sum` to `0` on every step would make the algorithm report `0` — wrong, since the problem asks for the best *subarray*, which must contain at least one element. The correct behavior falls out naturally from `current_sum = max(arr[i], current_sum + arr[i])`, because "abandon" means "start over with just `arr[i]`," not "start over with nothing."

Put together: (1) an empty array has no valid subarray — treat this as an explicit precondition error, not a silent default; (2) initialize `current_sum = best_sum = arr[0]` and, if reporting the winning range, `current_start = best_start = best_end = 0`; (3) for each index `i` from `1` to `n-1`, choose extend-or-restart (updating `current_start` on restart), then compare `current_sum` against `best_sum`, updating it (and `best_start`/`best_end`) on strict improvement; (4) after the loop, `best_sum` and its index range are the answer. No further code is needed — this two-variable, one-comparison-per-step shape **is** the entire algorithm.

## Architecture

Only two pieces of running state exist, and understanding what invariant each one protects is the whole game:

1. **`current_sum` (the "best-ending-here" tracker).** Invariant: just after processing index `i`, it holds the maximum possible sum of any contiguous subarray whose **last element is `arr[i]`**. Recomputed every step from exactly two candidates — extend or restart at `arr[i]` — taking whichever is larger. It can go up or down as you scan, and conceptually "resets" whenever restarting beats extending.
2. **`best_sum` (the "best-anywhere-so-far" record).** Invariant: after processing index `i`, it holds the maximum sum of any contiguous subarray fully contained within `arr[0..i]`. Updated by a single `best_sum = max(best_sum, current_sum)` comparison every step, and it **only ever increases or stays the same**.
3. **Index bookkeeping (`current_start` / `best_start` / `best_end`).** Used by this module's `code.cpp` to report *which* subarray achieved the best sum, not just its value. `current_start` marks where the current run began (reset whenever the run restarts); `best_start`/`best_end` are copied from it whenever `best_sum` improves.

In one line each: `current_sum` answers "what's the best run ending exactly here?" and resets to the current element whenever extending would make things worse; `best_sum` answers "what's the best run anywhere so far?" as a monotonically non-decreasing record, never reset; and the index bookkeeping exists purely to let you report *which* subarray won, without changing the sum computation itself.

## Why Not Other Approaches?

**"Brute-force: check every contiguous subarray."** `O(n²)` (running inner sum) or `O(n³)` (re-summing from scratch). Both correct, both leaving real performance on the table — the "obviously correct" approach is asymptotically much worse than the "clever" one, and the cleverness is genuinely simple once seen.

**"Divide and conquer."** Split the array in half, recursively find the best subarray fully within each half, and separately compute the best subarray *crossing* the midpoint (scanning outward from it, `O(n)` work per level). Correct in `O(n log n)` time — a fine answer to "solve it differently," and the same recursive shape as merge sort — but strictly worse than Kadane's `O(n)`, and meaningfully more code: a recursive split, a cross-midpoint combine step, base-case handling.

**"Precompute prefix sums, then look for the maximum `prefix[j] - prefix[i]` over `i < j`."** A legitimate reformulation — `sum(arr[i+1..j]) = prefix[j] - prefix[i]` — reducing the problem to "for each `j`, find the smallest `prefix[i]` seen so far." Tracking that running minimum collapses this to the *same* `O(n)`/`O(1)` bound as Kadane's — it is provably the same algorithm wearing different notation, since "current sum ending here" and "prefix[j] minus the best prefix[i] so far" are identical quantities.

**Net:** brute force is correct but asymptotically wasteful; divide-and-conquer is a genuine `O(n log n)` improvement worth knowing but adds real implementation complexity for no benefit over Kadane's; the prefix-sum reformulation collapses to the same algorithm once optimized. Kadane's wins because it is the simplest possible statement of the linear-time solution: one pass, two running variables, one comparison per step.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart deciding between Kadane's Algorithm, Sliding Window, and Dynamic Programming based on the signals in a problem statement (contiguous? best/max/min sum? no window-size given?).
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of the running-sum extend-or-reset loop.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of `current_sum` and `best_sum` across a concrete mixed-sign example array.

## The Code

[code.cpp](code.cpp) provides one generic, reusable function, `maxSubarraySum`, which takes a `std::vector<int>` and returns **both** the maximum sum and the inclusive `[start, end]` indices of the winning subarray, bundled in a small `KadaneResult` struct — returning the indices, not just the sum, matters because most real uses ("which stretch of trades was best," "which time window had the worst latency regression") need to know *where* the answer lives. The function is dependency-free and reused directly by the four worked [problems/](problems/), each of which either calls the same extend-or-restart idea directly or adapts it (a running min alongside the running max, for the product variant; "total sum minus the minimum subarray," for the circular variant).

**`maxSubarraySum(nums)`.** Initializes `current_sum`/`best_sum` to `nums[0]` (never `0` — what makes the all-negative case correct without a special branch), then for each subsequent index chooses extend-vs-restart via `std::max`, updates the bookkeeping indices, and updates `best_sum` (and its indices) only on strict improvement. Throws `std::invalid_argument` on an empty vector, because "the max sum of nothing" is not well-defined — silently returning `0` would be indistinguishable from a legitimate all-negative-array answer of `0`.

**`main()`.** Exercises `maxSubarraySum` against a typical mixed-sign array, an all-positive array (answer is the whole array), an **all-negative array** (answer must be the single least-negative element, not `0`), a single-element array, and a case where the best subarray sits in the middle — each printed alongside its expected value, with `[PASS]`/`[FAIL]` per assertion.

**Files in [problems/](problems/).** Each is a complete, standalone solution to one named LeetCode problem, reusing the sort-then-sweep idea with a small variation: `01` (LeetCode 53) is the pure "build once, apply directly" case; `02` (LeetCode 152, Maximum Product Subarray) tracks a running min alongside the max since a single negative number can flip a large negative product positive; `03` (LeetCode 121, Best Time to Buy and Sell Stock) applies Kadane's to day-over-day price deltas; `04` (LeetCode 918, Maximum Sum Circular Subarray) uses "total sum minus minimum subarray sum," guarded for the all-negative degenerate case.

## Tradeoffs

**What Kadane's buys you**

- **Linear time where brute force is quadratic or cubic.** `O(n²)`/`O(n³)` collapses to a single `O(n)` pass — often "instant" versus "unusable" on realistic input sizes.
- **Constant extra space.** A handful of scalars regardless of array size — no auxiliary array, no prefix-sum table, no recursion stack, beating divide-and-conquer's `O(log n)` recursion stack too.
- **Single forward pass, streaming-friendly.** Each step only needs the previous step's `current_sum`, so the sum-only core can process data as it arrives (a live feed of price deltas) without buffering the whole input.
- **Provably correct, not heuristic.** The extend-vs-abandon decision has an exact justification ("a negative running sum can only subtract from anything appended after it"), making the algorithm easy to prove correct by induction and easy to explain precisely.
- **A genuine gateway into one-dimensional dynamic programming.** `current_sum` at index `i` is exactly `dp[i] = max(nums[i], dp[i-1] + nums[i])`, and `best_sum` is `max(dp[0..i])` — often a beginner's first "real" DP encounter.

**What it costs you**

- **Only answers the max-sum contiguous subarray question.** It doesn't tell you *how many* subarrays achieve that maximum, or enumerate them, without separate bookkeeping.
- **Does not handle non-contiguous constraints.** A true *subsequence* (elements may be skipped) is different, generally harder DP territory (Longest Increasing Subsequence and similar).
- **Needs real adaptation for look-alike variants.** Circular arrays (the subarray may wrap) and product-instead-of-sum (a single negative can flip a large negative product positive) both break the vanilla algorithm's core assumption — see [problems/02](problems/02-maximum-product-subarray.cpp) and [problems/04](problems/04-maximum-sum-circular-subarray.cpp).
- **Silently wrong on empty input if unguarded.** "The max subarray sum of an empty array" isn't well-defined; failing to reject it risks a meaningless default that looks like a legitimate answer.
- **Narrow versus a more general DP formulation.** The moment the shape changes even slightly (non-contiguous, circular, product-based, "at most k elements") a genuinely different algorithm or a nontrivial adaptation is needed — there is no free generalization, though the gain versus brute force (`O(n)` in `O(1)` space, replacing "check every subarray" with one local decision per element) and versus divide-and-conquer (same destination, far less code) both remain intact.

## Complexity

**Time: O(n).** A single forward pass, doing a fixed, constant amount of work per element. Holds in the best, worst, and average case alike — the loop always runs exactly `n - 1` times after initialization, regardless of the data.

**Space: O(1)** extra beyond the input array — a small, fixed number of scalar variables, independent of `n`.

| Approach | Time | Space |
|---|---|---|
| Brute force, re-sum every subarray from scratch | O(n³) | O(1) |
| Brute force, running sum per start index | O(n²) | O(1) |
| Divide and conquer | O(n log n) | O(log n) recursion stack |
| Kadane's Algorithm | **O(n)** | **O(1)** |

## Common Mistakes

- **Forgetting the all-negative-array edge case.** A common bug is initializing `best_sum = 0` (instead of `nums[0]`) on the theory that "the empty subarray has sum 0 and is always a valid fallback." For `[-5, -3, -8]`, this incorrectly reports `0` instead of the correct `-3` (the least-negative single element). *Avoid:* always initialize both `current_sum` and `best_sum` from `nums[0]`, never from `0`.
- **Confusing "reset to 0" with "reset to the current element."** The colloquial explanation ("if the running sum goes negative, reset it to 0") is only correct when you also add the current element back in on the same step — writing literal code that sets `current_sum = 0` and moves to the *next* index without incorporating `nums[i]` skips an element's contribution and produces wrong sums. *Avoid:* prefer `current_sum = max(nums[i], current_sum + nums[i])` directly.
- **Resetting by comparing against zero when the array can be all non-positive.** Some implementations special-case `if (current_sum < 0) current_sum = 0;` computed *before* adding `nums[i]` — a different (and for all-negative arrays, buggy) recurrence. *Avoid:* stick to the two-candidate `max(extend, restart)` formulation; do not special-case zero.
- **Not updating `best_sum` on every iteration.** Some buggy versions only check `best_sum` when `current_sum` "looks like a new run started" — but the best sum can occur mid-run. *Avoid:* compare `current_sum` against `best_sum` unconditionally, every iteration.
- **Off-by-one in the index bookkeeping.** Forgetting to update `current_start` at the moment a restart happens (or updating it a step too late/early) produces a correct `max_sum` but a wrong `[start, end]`. *Avoid:* update `current_start = i` in the exact same branch where you decide to restart.
- **Applying vanilla Kadane's to a circular array without adaptation.** Treating the array as non-wrapping silently misses better answers that exist only in the wrapped configuration. *Avoid:* use "total sum minus minimum subarray sum" (see [problems/04](problems/04-maximum-sum-circular-subarray.cpp)), guarded for the all-negative case where that trick degenerates.

## When To Use

- The problem asks for the **maximum (or minimum) sum of a contiguous subarray**, with **no fixed or bounded window size** given (if a window size *is* given or bounded, that's Sliding Window's territory — see Similar Patterns).
- You need a **single linear pass, O(1) extra space** solution to a "best contiguous run" question — e.g. the best/worst contiguous stretch in a stream of financial deltas, sensor readings, or latency measurements.
- The problem is a thin variant of "max subarray sum" — maximum product subarray (track running max *and* min), maximum circular subarray sum (total minus minimum subarray), or best single buy/sell day for a stock (day-over-day price deltas) — all reusing the same extend-or-restart core with a small twist.
- You recognize the recurrence shape `dp[i] = max(nums[i], dp[i-1] + nums[i])` in a larger dynamic-programming problem — Kadane's is frequently the "inner" one-dimensional subroutine of a larger DP formulation.

## When NOT To Use

- **Non-contiguous subsequence problems.** If the problem allows skipping elements while preserving order (Longest Increasing Subsequence, Maximum Sum Increasing Subsequence, Longest Common Subsequence), Kadane's does not apply — that is full dynamic-programming territory with a different recurrence and typically `O(n²)` (or `O(n log n)` with extra structure) complexity.
- **Fixed or bounded window-size constraints.** "Best sum of exactly `k` consecutive elements" or "smallest window with sum ≥ target" is Sliding Window's shape, not Kadane's.
- **You need to count or enumerate all maximum-sum subarrays, not just find one.** Vanilla Kadane's tracks a single running best; counting ties or enumerating every optimal subarray needs additional state layered on top.
- **The problem is fundamentally 2D (e.g. "maximum sum sub-*matrix*").** That requires combining Kadane's with an outer loop over row ranges (compressing each row range into a 1D array first) — see DP on Grids in Similar Patterns — not applying 1D Kadane's directly.
- **Order doesn't matter at all, or the operation isn't associative/summable the way the extend-vs-restart argument needs.** Kadane's leans entirely on "sum" being monotonic under adding a non-negative number and degraded by adding a negative one; an operation without that property (e.g. counting distinct elements in a window) needs a different technique (often Sliding Window with a hash map).

## Where This Shows Up

LeetCode 53 (Maximum Subarray) is one of the most frequently cited "everyone has seen this exact question" problems across interview-prep communities (alongside Two Sum and Container With Most Water), because it tests whether a candidate finds the O(n) insight rather than settling for O(n²) brute force — its variants (152, 121, 918, all covered in this module's `problems/`) are common "same idea, different twist" follow-ups.

In production, the same running-sum-with-reset idea shows up directly, and translates into concrete backend/systems uses:

- **Stock trend / best trading window analysis.** Given day-over-day price deltas, Kadane's finds the best contiguous holding period — the core idea behind [problems/03](problems/03-best-time-to-buy-and-sell-stock.cpp), extending naturally to "best k-day run" reporting in a trading dashboard, and reusable as a building block inside a backtesting engine evaluating many candidate holding-window strategies.
- **Signal processing / peak detection and production monitoring.** In a stream of sensor deltas or an audio/vibration signal, finding the contiguous window with the largest net positive (or, negating the array, largest net negative) change flags "the worst sustained drop" or "strongest sustained surge" in anomaly-detection pipelines. The same idea over per-minute p99 latency deltas names the specific worst-regression window for an incident postmortem or alert — more actionable than "latency was elevated at some point today."
- **Cost, experiment, and log analysis.** Over day-over-day cloud-spend deltas, the worst contiguous cost-increase stretch is more actionable for a FinOps report than a single spike day; over day-by-day treatment-vs-control deltas in an A/B test, the same technique checks whether a lift is sustained or a one-off blip; and over time-bucketed (error count minus baseline) deltas from application logs, it prioritizes which incident window to investigate first.

## Similar Patterns

- **Sliding Window** ([../sliding-window/](../sliding-window/)): also scans a contiguous range with running state, but the window's *size* is fixed or explicitly grows/shrinks against a constraint checked every step. Kadane's has no window-size concept — `current_sum` can represent a subarray of any length, and there is no "shrink from the left" operation; the only decision is "extend by one, or restart here." Recognition question: does the problem give (or imply) a window-size constraint or bound? If yes, Sliding Window; if it's purely "best contiguous sum, no size given," that's Kadane's.
- **DP on Grids** ([../../dynamic-programming-patterns/dp-on-grids/](../../dynamic-programming-patterns/dp-on-grids/)): a 2D generalization of the same "define state in terms of a smaller subproblem" thinking — a grid DP's `dp[i][j]` typically depends on `dp[i-1][j]` and `dp[i][j-1]`, the 2D analogue of Kadane's 1D `dp[i]` depending only on `dp[i-1]`. "Maximum sum sub-*matrix*" bridges the two directly: fix a pair of row boundaries, collapse every column between them into a single 1D array (summing each column across those rows), and run 1D Kadane's on that collapsed array, repeated over all `O(rows²)` row-boundary pairs.

| Pattern | Structure | State tracked | Primary question answered |
|---|---|---|---|
| Kadane's Algorithm | One array, one pass | `current_sum` (ending here) + `best_sum` (record) | "What's the max/min sum of *any* contiguous subarray, no size given?" |
| Sliding Window | One array/string, `left`/`right` bound a range | Running property of the current window (sum, count, frequency map) | "What's the best contiguous subarray/substring under a size or property constraint?" |
| DP on Grids | 2D grid, filled row by row (or diagonally) | `dp[i][j]` depending on neighboring cells | "What's the optimal path/value reachable through a 2D grid of choices?" |

## Interview Discussion

Experienced engineers rarely spend time on "how do you write the loop" — the code is a few lines. What they probe is whether you can **state the extend-vs-restart argument precisely** and recognize when a "looks similar" variant needs a different formulation. A candidate who says "I reset the running sum to zero whenever it goes negative" has memorized a shortcut; one who says "I compare extending the previous run against restarting fresh at the current element, because a negative prefix can only subtract from what follows" has understood the argument and won't get tripped up by the all-negative edge case or the product variant.

Common follow-up questions:
- *"What if the array is all negative numbers?"* — expects recognizing `best_sum` must initialize to `nums[0]`, not `0`.
- *"Can you also return the indices of the winning subarray, not just the sum?"* — expects the `current_start`/`best_start`/`best_end` bookkeeping shown in this module's `code.cpp`.
- *"What changes for Maximum Product Subarray?"* — expects recognizing a single negative number can flip a large negative running product into a large positive one, so you must track a running **minimum** alongside the maximum, swapping them when the current element is negative (see [problems/02](problems/02-maximum-product-subarray.cpp)).
- *"What if the array is circular?"* — expects "total sum minus the minimum subarray sum," guarded for the all-negative degenerate case (see [problems/04](problems/04-maximum-sum-circular-subarray.cpp)).
- *"Prove this greedy local choice produces the global optimum."* — expects an inductive argument: any subarray ending at `i` either has length 1 or extends one ending at `i-1`, so if `current_sum` correctly represents the best sum ending at `i-1`, `max(nums[i], current_sum + nums[i])` correctly represents it at `i`.

Common misconceptions: that Kadane's "resets the sum to zero" (it resets to the *current element*; literal-code zero-reset that skips incorporating the element is a bug); that an empty subarray with sum zero is always a safe fallback (the problem requires non-empty unless stated otherwise); and that Kadane's works on any "find the best contiguous X" problem unmodified (it works specifically for sum and closely related monotonic aggregations — product, circular, and 2D-matrix versions all need a real adaptation).

## Key Takeaways

1. Kadane's Algorithm turns an `O(n²)`/`O(n³)` all-subarrays scan into `O(n)` by tracking only two running numbers: the best sum ending here, and the best sum seen anywhere.
2. At every step, choose between extending the previous run or restarting fresh at the current element — `current_sum = max(nums[i], current_sum + nums[i])`.
3. "Reset when negative" really means "reset to the current element," not "reset to zero and skip it" — this distinction is the single most common implementation bug.
4. `best_sum` must initialize from `nums[0]` (never `0`) so an all-negative array correctly returns its least-negative single element instead of a wrong `0`.
5. `best_sum` never resets — it is updated by comparison every iteration, independent of whether the current run just restarted.
6. Space cost is `O(1)` — a small, fixed number of scalar variables regardless of array size.
7. The pattern requires no sortedness and no window-size constraint — that absence of a size constraint is exactly what distinguishes it from Sliding Window.
8. Product, circular-array, and 2D-matrix variants all need real (if closely related) adaptations, not verbatim re-application of the sum-based recurrence.
9. The recurrence is a genuine one-dimensional DP — a natural bridge to DP on Grids (the 2D generalization) once you see `current_sum` as `dp[i]`.
10. Non-contiguous (subsequence) problems are out of scope entirely — that is separate, generally harder, dynamic-programming territory (e.g. Longest Increasing Subsequence).

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — presents the maximum-subarray problem explicitly as the motivating example for the divide-and-conquer chapter, then as a segue into the linear-time approach; a good source for the divide-and-conquer proof this README summarizes.
- *Programming Pearls* — Jon Bentley — the maximum-subarray problem (and Kadane's linear-time solution) is one of the book's most famous worked examples of iteratively improving an algorithm from brute force to linear time.
- *Competitive Programmer's Handbook* — Antti Laaksonen — covers Kadane's Algorithm concisely alongside prefix sums, useful for seeing the prefix-sum reformulation mentioned in this README's "Why Not Other Approaches."
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes the maximum-subarray problem and variants with C++-specific implementation notes.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including a Kadane's Algorithm implementation, useful for seeing an alternate implementation style.
- The C++ Standard Library's `<numeric>` header (e.g. `std::accumulate`, `std::partial_sum`) — while not Kadane's itself, these are the standard building blocks for the prefix-sum reformulation discussed above; reading libstdc++'s implementation is a useful side-study.

**Official Documentation**
- LeetCode — Maximum Subarray (problem 53).
- LeetCode — Maximum Product Subarray (problem 152).
- LeetCode — Best Time to Buy and Sell Stock (problem 121).
- LeetCode — Maximum Sum Circular Subarray (problem 918).

**Blog Articles**
- GeeksforGeeks — "Kadane's Algorithm" — a widely used explainer covering the core idea, proof sketch, and common variants.
- NeetCode — Maximum Subarray / Kadane's Algorithm video walkthrough — visual explanation of the extend-vs-restart decision and the all-negative edge case.
- Jon Bentley's original "maximum subarray" writeup (as popularized in *Programming Pearls* and widely referenced in algorithm course notes) — the historical origin of framing this as an algorithm-design case study, from `O(n³)` to `O(n)`.
