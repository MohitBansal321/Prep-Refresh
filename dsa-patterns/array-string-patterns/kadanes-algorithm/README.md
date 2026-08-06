# Kadane's Algorithm

## Intent

Find the maximum-sum contiguous subarray in a single linear pass, by tracking only the best sum ending at the current position and discarding it the moment it stops helping future choices.

## Real Life Analogy

Think of a **hiker tracking elevation gain on a trail**, using a GPS watch that logs "net climb since the last time I reset my counter." Every step either goes up (a positive contribution) or down (a negative one). The hiker is trying to find the single best continuous uphill stretch of the whole trail — the segment where they gained the most net elevation without stopping.

Here is the key habit a smart hiker develops: if their running "net climb since reset" counter ever drops below zero, they should just **reset it to zero right there** and treat the next step as a fresh start. Why? Because a negative running total can only ever *drag down* whatever climb comes after it. Carrying forward "I'm currently 200 meters below where I started this stretch" never helps the next uphill push — it only makes the next stretch's total worse than if the hiker had just started counting from the current point. So the hiker's rule becomes: "keep extending my current climb as long as it's net-positive-or-better-than-restarting; the moment it turns net-negative, abandon it and start a fresh count from here." Meanwhile, a second, separate number — "the best climb I've recorded on any stretch so far today" — only ever gets updated, never reset, because the record for the whole trail must remember every good stretch, not just the current one.

Another everyday version: a **stock trader's "current losing streak" mental counter** versus their "best single winning streak this month" counter. The two counters serve different purposes and reset on different rules — that distinction is the entire algorithm.

Kadane's Algorithm is that hiker's two counters in code: a **running sum** ("my current stretch's total, reset when it goes negative") and a **best sum** ("the best stretch I've seen across the whole trail, never reset").

## Problem

### What engineering problem exists?

Given an array of numbers — which can include negative numbers — find the contiguous subarray (a run of consecutive elements, not any arbitrary subset) whose sum is the largest possible.

> **Term: Contiguous subarray.** A slice of consecutive elements from the original array, `arr[i..j]` for some `i <= j`, with nothing skipped in between. This is different from a *subsequence*, which can skip elements arbitrarily as long as relative order is preserved. Kadane's Algorithm only ever answers questions about **contiguous** ranges.

The naive way to solve this is to check **every possible contiguous subarray** and compute its sum, keeping track of the largest one seen:

- **O(n³) approach:** for every pair of start/end indices `(i, j)`, sum `arr[i..j]` from scratch by iterating over it again. There are `~n²/2` such pairs, and summing each one naively costs up to `O(n)` — giving `O(n³)` overall.
- **O(n²) approach:** precompute nothing special, but avoid re-summing from scratch — for each start index `i`, extend `j` one step at a time and keep a running sum, updating the best as you go. This drops the "sum from scratch" cost, giving `O(n²)` overall: still a nested loop, just a smarter inner loop.

For `n = 10,000` elements — a perfectly realistic size for, say, a day's worth of per-minute price deltas — the `O(n²)` version is already 100 million operations, and the naive `O(n³)` version is on the order of a trillion. Both are wasteful for a problem that, as you'll see, has a linear-time solution.

### Why is this problem difficult?

- **The instinct to "check every subarray" is strong, because the answer really could be anywhere.** Unlike Two Pointers or Sliding Window, there's no obvious sortedness or fixed window size to exploit at first glance — the array can be arbitrary, with positives and negatives mixed freely.
- **Negative numbers make the "best" answer non-obvious.** If every number were non-negative, the answer would trivially be "the whole array." It's the negative numbers that force you to reason about *when a running total is worth carrying forward* versus *when it's actively hurting you*.
- **The key insight is a *local* decision that turns out to produce a *global* optimum.** It is not obvious, the first time you see this problem, that you can make a decision using only "the sum ending exactly here" without looking further back or ahead, and still guarantee correctness for the whole array. Proving that this greedy, one-pass rule is actually correct (not just a heuristic) is the crux of understanding the algorithm rather than merely memorizing it.

### What happens if we ignore it?

- **Quadratic-or-worse time on a problem with a linear solution.** A batch job scanning millions of per-second sensor readings, or per-trade P&L deltas, to find the best contiguous run would take minutes instead of milliseconds if implemented as nested loops — a real, not hypothetical, cost difference in a production data pipeline.
- **Unnecessary memory for prefix-sum arrays.** A common "clever" fallback is to precompute prefix sums and then still scan pairs of them — this drops the per-subarray sum cost to O(1) but still leaves an O(n²) pair scan, and it burns O(n) extra memory for the prefix array that Kadane's O(1)-space approach doesn't need at all.
- **Missed edge cases.** Without understanding *why* the reset rule works, engineers often build a version that reports `0` for an all-negative array (as if "not choosing any subarray" were valid) instead of correctly returning the least-negative single element — a subtle correctness bug, not just a performance one (see Common Mistakes).

## Why Not Other Approaches?

**"Brute-force: check every contiguous subarray."** As described above, `O(n²)` (with a running inner sum) or `O(n³)` (re-summing every subarray from scratch). Both are correct but leave real performance on the table for a problem that admits a single linear pass — this is a case where the "obviously correct" approach is asymptotically much worse than the "clever" one, and the cleverness is genuinely simple once you see it.

**"Divide and conquer."** Split the array in half, recursively find the best subarray fully within the left half and the best fully within the right half, and separately compute the best subarray that *crosses* the midpoint (by scanning outward from the midpoint in both directions, which is `O(n)` work per level). This gives a correct answer in `O(n log n)` time — genuinely better than brute force, and a fine answer to give in an interview if asked to "solve it differently" as a follow-up. But it is strictly worse than Kadane's `O(n)`, and it is meaningfully more code: you need the recursive split, the "combine" step that scans across the midpoint, and careful base-case handling for arrays of size 0 or 1. It exists as an important *teaching* exercise (it's the same recursive shape as merge sort, and shows up as a natural interview follow-up: "can you solve this without the linear trick?"), but there is no reason to reach for it in real code once Kadane's is available.

**"Precompute prefix sums, then look for the maximum `prefix[j] - prefix[i]` over `i < j`."** This is a legitimate reformulation — the sum of `arr[i+1..j]` is `prefix[j] - prefix[i]` — and it correctly reduces the problem to "for each `j`, find the smallest `prefix[i]` seen so far for `i < j`." If you track the running minimum prefix sum as you scan left to right, this collapses to the *same* `O(n)` time, `O(1)` extra space (if you fold the prefix-sum accumulation into the same loop) as Kadane's Algorithm — in fact, it is provably the same algorithm wearing different notation, since "current sum ending here" and "prefix[j] minus the best prefix[i] so far" turn out to be identical quantities. Kadane's formulation (track a running sum, reset when negative) is simply the more direct and more commonly taught way to arrive at the same O(n) result, without introducing prefix-sum bookkeeping as a separate concept first.

**Tradeoff summary:** brute force is correct but asymptotically wasteful; divide-and-conquer is a genuine `O(n log n)` improvement and a useful "second solution" to know, but adds real implementation complexity for no benefit over Kadane's; the prefix-sum reformulation collapses to the same algorithm once optimized, so it isn't really a different approach at all. Kadane's wins because it is the simplest possible statement of the linear-time solution: one pass, two running variables, one comparison per step.

## Solution

The whole algorithm rests on one question, asked at every index: **"is it better to extend the current run, or to abandon it and start fresh from here?"**

Maintain two running numbers as you scan left to right:

1. **`current_sum`** — the maximum sum of a contiguous subarray that **ends exactly at the current index**. At each step, you have exactly two choices for what `current_sum` becomes: either **extend** the previous run by adding the current element (`current_sum + arr[i]`), or **abandon** the previous run and start a brand-new one consisting of just the current element (`arr[i]` alone). You take whichever of those two is larger — you never need to consider "start the new run somewhere further back," because any run starting further back and ending here would necessarily include the same trailing elements, and the two-choice comparison already captures the only decision point that matters.
2. **`best_sum`** — the maximum sum seen across *any* index scanned so far, updated by comparing it against `current_sum` after every step. Unlike `current_sum`, `best_sum` **never resets** — it is the record book for the whole array, remembering the best run found anywhere, even if the current run has since gotten worse.

The "reset" intuition from the analogy is really just the extend-vs-abandon choice stated differently: if `current_sum` (extending) would be *smaller* than starting fresh with just `arr[i]`, that can only happen when the sum you were carrying forward was itself negative — a negative prefix can only ever *subtract* from whatever comes next, so it is never worth carrying. Choosing "start fresh" in that case is mathematically equivalent to "reset the running sum to zero, then add the current element" — both phrasings produce the same `current_sum` value; the "reset to 0" framing is just a shortcut for "the previous best contribution to a new run is 0, since anything negative would only hurt."

The critical subtlety, saved for its own callout because it is the single most common bug (see Common Mistakes): **"reset" means reset to the current element, not literally to zero.** If every number in the array is negative, correctly resetting `current_sum` to `0` on every step would make the whole algorithm report `0` — an answer that is wrong, because the problem asks for the best *subarray*, and a subarray must contain at least one element. The correct behavior naturally falls out of the extend-vs-abandon framing (`current_sum = max(arr[i], current_sum + arr[i])`) because "abandon" means "start over with just `arr[i]`," not "start over with nothing."

No code yet — the two-variable, one-comparison-per-step shape above **is** the entire algorithm; everything in Implementation below is just this idea expressed in C++.

## Architecture

Only two pieces of running state exist, and understanding what invariant each one protects is the whole game:

1. **`current_sum` (the "best-ending-here" tracker).** Its invariant: at the moment just after processing index `i`, `current_sum` holds the maximum possible sum of any contiguous subarray whose **last element is `arr[i]`**. It is recomputed every step from only two candidates — extend the previous `current_sum` by adding `arr[i]`, or restart at `arr[i]` alone — and takes whichever is larger. It can go up or down as you scan, and conceptually "resets" whenever restarting beats extending.

2. **`best_sum` (the "best-anywhere-so-far" record).** Its invariant: after processing index `i`, `best_sum` holds the maximum sum of any contiguous subarray fully contained within `arr[0..i]`, regardless of where it ends. It is updated by a single `best_sum = max(best_sum, current_sum)` comparison every step, and it **only ever increases or stays the same** — it never resets, because forgetting a good run found earlier would be a correctness bug, not an optimization choice.

3. **(For the indices-tracking variant used in this module's `code.cpp`) `current_start` / `best_start` / `best_end`.** Bookkeeping indices that ride alongside the two sums so the implementation can report *which* subarray achieved the best sum, not just the number. `current_start` marks where the current run began (reset to the current index whenever the run restarts); `best_start`/`best_end` are copied from `current_start`/the current index whenever `best_sum` improves.

Responsibilities in one line each:
- **`current_sum`:** answers "what's the best run ending exactly here?" — resets (to the current element) whenever extending would make things worse.
- **`best_sum`:** answers "what's the best run anywhere so far?" — a monotonically non-decreasing record, never reset.
- **Index bookkeeping:** exists purely to let you report *which* subarray won, not to change the sum computation itself.

## Execution Flow

1. If the array is empty, there is no valid subarray — handle this as an explicit precondition/error case rather than silently returning a number (see Common Mistakes).
2. Initialize `current_sum = arr[0]` and `best_sum = arr[0]` (start both trackers at the first element — this is what correctly handles an all-negative array, since there is no "before the array" sum of zero to fall back on).
3. Initialize index bookkeeping: `current_start = 0`, `best_start = 0`, `best_end = 0`.
4. For each index `i` from `1` to `n - 1`:
   a. Decide whether to **extend** the current run or **abandon** it and start fresh at `arr[i]`: if `current_sum + arr[i]` is greater than or equal to `arr[i]` alone, extend (`current_sum += arr[i]`); otherwise abandon and restart (`current_sum = arr[i]`, `current_start = i`).
   b. Compare `current_sum` against `best_sum`. If `current_sum` is strictly greater, update `best_sum = current_sum`, `best_start = current_start`, `best_end = i` — a new best run has been found, and its boundaries are recorded.
5. After the loop finishes, `best_sum` holds the maximum-sum contiguous subarray's total, and `best_start`/`best_end` hold its inclusive index range.
6. Return (or print) `best_sum` alongside `[best_start, best_end]`.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full flowchart deciding between Kadane's Algorithm, Sliding Window, and Dynamic Programming based on the signals in a problem statement (contiguous? best/max/min sum? no window-size given?).

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the running-sum extend-or-reset loop.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of `current_sum` and `best_sum` across a concrete mixed-sign example array.

## Implementation

[code.cpp](code.cpp) provides one generic, reusable function: `maxSubarraySum`, which takes a `std::vector<int>` and returns **both** the maximum sum and the inclusive `[start, end]` indices of the winning subarray, bundled in a small result struct. Returning the indices (not just the sum) is deliberate: most real uses of this pattern — "which stretch of trades was the best," "which time window had the worst latency regression" — need to know *where* the answer lives, not just its magnitude, and that requires the extra bookkeeping described in Architecture above.

The function is written to be dependency-free and directly reusable in the four worked [problems/](problems/) that follow, each of which either calls the same extend-or-restart idea directly or adapts it (tracking a running min alongside a running max, for the product variant; folding in "total sum minus the minimum subarray," for the circular variant).

## Code Walkthrough

**`struct KadaneResult`** (in [code.cpp](code.cpp)). A small plain-data struct holding `max_sum`, `start_index`, and `end_index`. It exists so `maxSubarraySum` can return three related values together without an awkward tuple of unnamed fields — callers immediately see what each field means.

**`maxSubarraySum(std::vector<int>& nums)`** (in [code.cpp](code.cpp)). Implements exactly the Execution Flow above: initializes `current_sum`/`best_sum` to `nums[0]` (never to `0` — this is what makes the all-negative case correct without a special branch), then for each subsequent index chooses between extending and restarting via `std::max`, updates the bookkeeping indices, and updates `best_sum` (and its indices) only on a strict improvement. It throws `std::invalid_argument` if given an empty vector, because "the maximum sum subarray of nothing" is not a well-defined question — a function that silently returned `0` for empty input would be indistinguishable from a legitimate all-negative-array answer of `0`, which is exactly the ambiguity this module warns about in Common Mistakes.

**`main()`** (in [code.cpp](code.cpp)). Exercises `maxSubarraySum` against several hand-checkable cases: a typical mixed-sign array, an all-positive array (where the answer is the whole array), an **all-negative array** (where the answer must be the single least-negative element, not `0`), a single-element array, and a case where the best subarray sits in the middle. Each case prints the computed sum and index range next to the expected values so a reader can verify correctness by eye, printing `[PASS]`/`[FAIL]` for each assertion.

## Advantages

- **Linear time where brute force is quadratic or cubic.** Turns an `O(n²)` (or `O(n³)`) all-subarrays scan into a single `O(n)` pass — often the difference between "instant" and "unusable" on realistic input sizes (millions of ticks, log lines, or sensor readings).
- **Constant extra space.** Only a handful of scalar variables (`current_sum`, `best_sum`, and a couple of indices) regardless of array size — no auxiliary array, no prefix-sum table, no recursion stack.
- **Single forward pass, streaming-friendly.** Because each step only needs `current_sum` from the *previous* step (not the whole array so far), the core sum-only version can process data as it arrives — e.g. a live feed of price deltas — without needing to buffer the whole input in memory. (The index-tracking variant still only needs O(1) extra state, just slightly more of it.)
- **Provably correct, not heuristic.** The extend-vs-abandon decision at each step has an exact, stateable justification ("a negative running sum can only subtract from anything appended after it"), making the algorithm straightforward to prove correct by induction, and straightforward to explain precisely in a review or interview.
- **A genuine gateway into one-dimensional dynamic programming.** `current_sum` at index `i` is exactly `dp[i]` in a one-dimensional DP formulation (`dp[i] = max(nums[i], dp[i-1] + nums[i])`), and `best_sum` is `max(dp[0..i])`. Kadane's is often the first "real" DP most engineers encounter, precisely because the recurrence is this short and this intuitive.

## Disadvantages

- **Only answers the max-sum contiguous subarray question — nothing more.** It does not, by itself, tell you *how many* subarrays achieve that maximum sum, or enumerate all of them; that requires separate bookkeeping (counting ties as you scan) layered on top.
- **Does not handle non-contiguous constraints.** If the problem allows skipping elements (a true *subsequence*, not a contiguous run), Kadane's does not apply at all — that is a different, generally harder DP problem (e.g. Longest Increasing Subsequence, Maximum Sum Increasing Subsequence).
- **Needs real adaptation for variants that look similar but aren't identical.** Circular arrays (the subarray may wrap from the end back to the start) and product-instead-of-sum (where a single negative number can flip a large negative running product into a large positive one) both break the vanilla algorithm's core assumption and require a modified formulation — see [problems/02](problems/02-maximum-product-subarray.cpp) and [problems/04](problems/04-maximum-sum-circular-subarray.cpp).
- **Silently wrong on empty input if not guarded.** As discussed above, "the max subarray sum of an empty array" is not a well-defined value; an implementation that doesn't explicitly reject empty input risks papering over a caller bug with a meaningless default.

## Tradeoffs

**What we gain versus brute force:** we go from `O(n²)`/`O(n³)` time down to `O(n)`, in `O(1)` extra space, by replacing "check every subarray explicitly" with a single local decision per element that provably captures the same information.

**What we gain versus divide-and-conquer:** the same asymptotic destination in a strictly simpler form — `O(n)` beats `O(n log n)`, with far less code (no recursive split, no "combine across the midpoint" step, no base-case juggling).

**What we lose versus a more general DP formulation:** Kadane's is deliberately narrow — it only answers "best contiguous sum." The moment a problem's shape changes even slightly (non-contiguous, circular, product-based, "at most k elements," "exactly k subarrays"), you either need a genuinely different algorithm or a nontrivial adaptation of this one; there is no free generalization.

## Complexity

**Time: O(n).** A single forward pass over the array, doing a fixed, constant amount of work (one addition, one comparison, possibly one more comparison for the index bookkeeping) per element. This holds in the best, worst, and average case alike — the loop always runs exactly `n - 1` times after initialization, regardless of the data.

**Space: O(1)** extra space beyond the input array itself — a small, fixed number of scalar variables (`current_sum`, `best_sum`, and index bookkeeping), independent of `n`.

**Comparison to the brute force it replaces:**

| Approach | Time | Space |
|---|---|---|
| Brute force, re-sum every subarray from scratch | O(n³) | O(1) |
| Brute force, running sum per start index | O(n²) | O(1) |
| Divide and conquer | O(n log n) | O(log n) recursion stack |
| Kadane's Algorithm | **O(n)** | **O(1)** |

## Common Mistakes

- **Forgetting the all-negative-array edge case.** A common bug is initializing `best_sum = 0` (instead of `nums[0]`) on the theory that "the empty subarray has sum 0 and is always a valid fallback." For an array like `[-5, -3, -8]`, this incorrectly reports `0` — a sum that no actual (non-empty) subarray of this array produces — instead of the correct answer, `-3` (the least-negative single element). *Avoid:* always initialize both `current_sum` and `best_sum` from `nums[0]`, never from `0`, and never treat "pick nothing" as a valid competing option unless the problem statement explicitly allows an empty subarray.
- **Confusing "reset to 0" with "reset to the current element."** The colloquial explanation ("if the running sum goes negative, reset it to 0") is a helpful shortcut *only* when you also separately add the current element back in on the same step — i.e., "restart at `nums[i]`" is the correct operation, and writing literal code that sets `current_sum = 0` and then moves to the *next* index without incorporating `nums[i]` at all skips an element's contribution entirely and produces wrong sums. *Avoid:* prefer the `current_sum = max(nums[i], current_sum + nums[i])` formulation directly — it makes "restart at `nums[i]`" explicit and sidesteps the "reset to what, exactly?" ambiguity altogether.
- **Resetting incorrectly by comparing against zero when the array can be all non-positive.** Some implementations special-case `if (current_sum < 0) current_sum = 0;` computed *before* adding `nums[i]`, which is a different (and for all-negative arrays, buggy) recurrence than the correct extend-vs-restart comparison. *Avoid:* stick to the two-candidate `max(extend, restart)` formulation; do not special-case zero.
- **Not updating `best_sum` on every iteration.** Some buggy versions only check `best_sum` when `current_sum` "looks like a new run started" — but the best sum can occur mid-run, not only at a reset boundary. *Avoid:* compare `current_sum` against `best_sum` unconditionally, every single iteration, not just when a reset happens.
- **Off-by-one in the index bookkeeping.** Forgetting to update `current_start` at the moment a restart happens (or updating it a step too late/early) produces a correct `max_sum` but a wrong reported `[start, end]` range. *Avoid:* update `current_start = i` in the exact same branch where you decide to restart, not in a separate pass afterward.
- **Applying vanilla Kadane's to a circular array without adaptation.** Treating the array as if it doesn't wrap (missing subarrays that span from near the end back to near the start) silently misses better answers that exist only in the wrapped configuration. *Avoid:* use the "total sum minus minimum subarray sum" trick (see [problems/04](problems/04-maximum-sum-circular-subarray.cpp)), with a guard for the all-negative case where that trick degenerates.

## When To Use

- The problem asks for the **maximum (or minimum) sum of a contiguous subarray**, with **no fixed or bounded window size** given (if a window size *is* given or bounded, that's Sliding Window's territory instead — see Similar Patterns).
- You need a **single linear pass, O(1) extra space** solution to a "best contiguous run" question — e.g. finding the best/worst contiguous stretch in a stream of financial deltas, sensor readings, or latency measurements.
- The problem is a thin variant of "max subarray sum" — maximum product subarray (track running max *and* min), maximum circular subarray sum (total minus minimum subarray), or best single buy/sell day for a stock (Kadane's applied to day-over-day price deltas) — all of which reuse the same extend-or-restart core idea with a small twist.
- You recognize the recurrence shape `dp[i] = max(nums[i], dp[i-1] + nums[i])` in a larger dynamic-programming problem — Kadane's is frequently the "inner" one-dimensional subroutine of a larger DP formulation.

## When NOT To Use

- **Non-contiguous subsequence problems.** If the problem allows skipping elements while preserving order (a true subsequence, not a contiguous run) — e.g. Longest Increasing Subsequence, Maximum Sum Increasing Subsequence, Longest Common Subsequence — Kadane's does not apply; that is full dynamic-programming territory with a different recurrence and typically `O(n²)` (or `O(n log n)` with extra structure) complexity.
- **Fixed or bounded window-size constraints.** "Best sum of exactly `k` consecutive elements" or "smallest window with sum ≥ target" is Sliding Window's shape, not Kadane's — see Similar Patterns and the family README's recognition table.
- **You need to count or enumerate all maximum-sum subarrays, not just find one.** Vanilla Kadane's tracks a single running best; counting ties or enumerating every optimal subarray needs additional state layered on top.
- **The problem is fundamentally 2D (e.g. "maximum sum sub-*matrix*").** That requires combining Kadane's with an outer loop over row ranges (compressing each row range into a 1D array first) — see DP on Grids in Similar Patterns — not applying 1D Kadane's directly.
- **Order doesn't matter at all (just "the k largest elements") or the operation isn't associative/summable in a way that supports the extend-vs-restart argument.** Kadane's leans entirely on the fact that "sum" is monotonic under adding a non-negative number and degraded by adding a negative one; an operation without that property (e.g. counting distinct elements in a window) needs a different technique (often Sliding Window with a hash map).

## Real Interview/Production Examples

- **Stock trend / best trading window analysis.** Given a series of day-over-day price changes (deltas, not raw prices), Kadane's directly finds the best contiguous holding period — the core idea behind [problems/03](problems/03-best-time-to-buy-and-sell-stock.cpp) (a single buy/sell) and a natural extension to "best k-day run" reporting in a trading dashboard or backtesting tool.
- **Signal processing / peak detection.** In a stream of sensor deltas or an audio/vibration signal, finding the contiguous window with the largest net positive change (or largest net negative change, by negating the array) is a direct application — used in anomaly detection pipelines to flag "the worst sustained drop" or "the strongest sustained surge" in a monitored metric.
- **Interview frequency.** LeetCode 53 (Maximum Subarray) is one of the most frequently cited "everyone has seen this exact question" problems across interview-prep communities (alongside Two Sum and Container With Most Water), specifically because it tests whether a candidate can find the O(n) insight rather than settling for the O(n²) brute force — and its variants (152, 121, 918, covered in this module's `problems/`) are common "same idea, different twist" follow-ups.
- **Production monitoring.** Finding the longest/worst contiguous stretch of a metric exceeding (or under) a threshold in a time-series (e.g. "the worst sustained latency regression window in the last 24 hours of per-minute p99 latency deltas") is a direct, real application of the same running-sum-with-reset idea.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **Latency regression detection.** Given a time series of per-minute p99 latency deltas (this-minute minus baseline), run Kadane's to find the worst sustained regression window for an incident postmortem or an automated alert that reports "latency was elevated for this specific 14-minute stretch," not just "latency was elevated at some point today."
2. **Cost/usage anomaly windows.** Over a series of day-over-day cloud spend deltas, find the contiguous stretch of days responsible for the largest cumulative unexpected cost increase — more actionable for a FinOps report than a single spike day.
3. **A/B test metric-lift analysis.** Given day-by-day deltas between a treatment and control group's conversion rate, find the best (or worst) contiguous run of days — useful for spotting whether a lift is sustained or a one-off blip before trusting the aggregate average.
4. **Trading/backtesting tooling.** Reuse the buy/sell variant ([problems/03](problems/03-best-time-to-buy-and-sell-stock.cpp)) as a building block inside a larger backtesting engine that evaluates many candidate holding-window strategies against historical price series.
5. **Log-based error-rate burst detection.** Over a time-bucketed series of (error count minus acceptable baseline) deltas parsed from application logs, find the worst contiguous burst window to prioritize which incident window to investigate first.

## Similar Patterns

- **Sliding Window** ([../sliding-window/](../sliding-window/)): also scans a contiguous range with running state, but the window's *size* is either fixed or explicitly grows/shrinks based on a constraint (sum, distinct-count, character frequency) that is checked and enforced every step. Kadane's has no window-size concept at all — `current_sum` can represent a subarray of any length, and there is no "shrink from the left" operation; the only decision is "extend by one, or restart here." The recognition question that separates them: does the problem give (or imply) a window-size constraint, or bound (e.g. "at most k distinct characters")? If yes, Sliding Window. If the question is purely "best contiguous sum, no size given," that's Kadane's.
- **DP on Grids** ([../../dynamic-programming-patterns/dp-on-grids/](../../dynamic-programming-patterns/dp-on-grids/)): a 2D generalization of the same "define state, express it in terms of a smaller subproblem" thinking — a grid DP's `dp[i][j]` typically depends on `dp[i-1][j]` and `dp[i][j-1]` (from above and from the left), the 2D analogue of Kadane's 1D `dp[i]` depending only on `dp[i-1]`. The "maximum sum sub-*matrix*" problem is the direct bridge between the two: solved by fixing a pair of row boundaries, collapsing every column between them into a single 1D array (summing each column's values across those rows), and running 1D Kadane's on that collapsed array — repeated over all `O(rows²)` row-boundary pairs. Kadane's is the 1D core; DP on Grids is what you reach for when the "contiguous" constraint spans two dimensions instead of one.

| Pattern | Structure | State tracked | Primary question answered |
|---|---|---|---|
| Kadane's Algorithm | One array, one pass | `current_sum` (ending here) + `best_sum` (record) | "What's the max/min sum of *any* contiguous subarray, no size given?" |
| Sliding Window | One array/string, `left`/`right` bound a range | Running property of the current window (sum, count, frequency map) | "What's the best contiguous subarray/substring under a size or property constraint?" |
| DP on Grids | 2D grid, filled row by row (or diagonally) | `dp[i][j]` depending on neighboring cells | "What's the optimal path/value reachable through a 2D grid of choices?" |

## Interview Discussion

Experienced engineers rarely spend interview time on "how do you write the loop" for Kadane's — the code is a few lines. What they actually probe is whether you can **state the extend-vs-restart argument precisely** and immediately recognize when a "looks similar" variant needs a different formulation. A candidate who says "I reset the running sum to zero whenever it goes negative" has memorized a shortcut; a candidate who says "I compare extending the previous run against restarting fresh at the current element, because a negative prefix can only subtract from what follows" has understood the actual argument and won't get tripped up by the all-negative edge case or by variants like the product version.

Common follow-up questions:
- *"What if the array is all negative numbers?"* — expects you to recognize that `best_sum` must initialize to `nums[0]`, not `0`, and that the answer is the single least-negative element, not `0`.
- *"Can you also return the indices of the winning subarray, not just the sum?"* — expects the `current_start`/`best_start`/`best_end` bookkeeping shown in this module's `code.cpp`.
- *"What changes for Maximum Product Subarray?"* — expects recognizing that a single negative number can flip a large negative running product into a large positive one, so you must track a running **minimum** product alongside the running maximum, and swap them whenever the current element is negative (see [problems/02](problems/02-maximum-product-subarray.cpp)).
- *"What if the array is circular (the best subarray might wrap around)?"* — expects the "total sum minus the minimum subarray sum" trick, with a guard for the case where the minimum subarray sum equals the total sum (which would incorrectly return 0 for an all-negative array) — see [problems/04](problems/04-maximum-sum-circular-subarray.cpp).
- *"Can you solve this with divide and conquer instead? What's the complexity?"* — expects recognizing the merge-sort-shaped recursion (best-in-left-half, best-in-right-half, best-crossing-the-middle) and stating its `O(n log n)` complexity, while acknowledging Kadane's `O(n)` is strictly better.
- *"Prove this greedy local choice actually produces the global optimum."* — expects an inductive argument: assuming `current_sum` correctly represents the best sum ending at `i-1`, show that `max(nums[i], current_sum + nums[i])` correctly represents the best sum ending at `i`, because any subarray ending at `i` either has length 1 (just `nums[i]`) or extends some subarray ending at `i-1`.

Common misconceptions:
- "Kadane's Algorithm resets the sum to zero." Not quite — it resets to the **current element**, which only coincides with "reset to zero, then add the element" when phrased as a two-step operation; stated as literal code, resetting the variable to `0` and moving on without incorporating the current element is a bug.
- "If a subarray sum could be zero, an empty subarray with sum zero is always a safe answer." The problem requires a non-empty subarray unless explicitly stated otherwise; this misconception is exactly what produces the all-negative-array bug.
- "Kadane's works on any 'find the best contiguous X' problem unmodified." It works specifically for **sum** (and closely related monotonic aggregations); product, circular wraparound, and 2D-matrix versions all need a real (if related) adaptation, not a verbatim reapplication.
- "This is just a special case of Sliding Window." They share "contiguous range, one pass" but differ in the fundamental question being asked — Sliding Window enforces or searches over an explicit size/property constraint on the window; Kadane's has no window-size concept at all.

## Summary

- Kadane's Algorithm finds the maximum-sum contiguous subarray in a single linear pass by tracking "best sum ending here" (`current_sum`) and "best sum anywhere so far" (`best_sum`).
- At every index, `current_sum` chooses between **extending** the previous run (`current_sum + arr[i]`) or **restarting** at the current element alone (`arr[i]`) — never anything in between.
- `best_sum` only ever increases; it is the permanent record across the whole scan, checked (and possibly updated) on every single iteration, not just at reset points.
- The all-negative-array edge case is the sharpest correctness test: both trackers must initialize from `nums[0]`, never from `0`, or the algorithm silently reports a wrong answer.
- Typical complexity win: `O(n²)`/`O(n³)` brute force down to `O(n)` time, `O(1)` extra space.
- It answers exactly one question — max/min contiguous sum — and needs real (if small) adaptation for product-based, circular, or 2D-matrix variants.
- Closely related but distinct: Sliding Window (explicit window-size/property constraint) and DP on Grids (the 2D generalization of the same "define state from a smaller subproblem" thinking).
- The recurrence `dp[i] = max(nums[i], dp[i-1] + nums[i])` is frequently a beginner's first real encounter with one-dimensional dynamic programming.

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
