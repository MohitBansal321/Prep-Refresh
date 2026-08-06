# Prefix Sum

## Intent

Precompute a running total over an array once, so that the sum of *any* contiguous range can be answered in O(1) time afterward, instead of re-scanning that range on every query.

## Real Life Analogy

Think of a **bank statement's running balance column**. Every row shows a transaction *and* a balance — the total of every transaction up to and including that row. If your bank only stored individual transactions and you asked "what was my balance change between March 3rd and March 17th?", the teller would have to add up every transaction in between, by hand, every single time you asked. But because the statement already carries a running balance, the teller just looks up the balance on March 17th, looks up the balance on March 2nd (the day before the 3rd), and subtracts. One lookup, one lookup, one subtraction — regardless of whether the range spans 2 days or 2,000.

Another everyday one: an **odometer on a car**. You do not need to remember every mile you have ever driven to answer "how far did I drive between two road trips?" You just read the odometer at the start of the trip and at the end, and subtract. The odometer is a running total; the subtraction of two readings gives you the sum over any range instantly, without replaying the whole driving history.

Prefix Sum is that running-balance column, computed once for an array, so that any "sum from here to there" question becomes a single subtraction instead of a re-scan.

## Problem

### What engineering problem exists?

A very common shape of question over a fixed array of numbers is: **"what is the sum of the elements between index `i` and index `j`?"** — and critically, this question gets asked **repeatedly**, often with different `i` and `j` each time, over an array that does not change between queries.

- A financial reporting service that answers "what was total revenue between day 10 and day 40?" for arbitrary day ranges, potentially thousands of times per hour from different dashboard widgets.
- An analytics API that serves "sum of page views between timestamp A and timestamp B" for an array of per-minute counts, queried on-demand by many concurrent dashboard users looking at different windows.
- A grading or scoring system answering "what is the total score contributed by questions 5 through 20?" across many students' answer arrays.

The naive way to answer a single range-sum query is to walk from index `i` to index `j` and add up every element in between. For one query over `n` elements, that is up to O(n) work. The problem is not that one query is slow — O(n) for a single sum is perfectly fine. **The problem is repetition.** If you get `q` such queries over the same array, the naive approach costs O(n) *per query*, so the total cost is **O(n · q)**. For an array of a million elements answering ten thousand range queries, that is up to **10 billion** element additions — seconds to minutes of wasted CPU on what should be near-instant lookups, especially when the underlying array genuinely never changes between queries.

> **Term: Prefix Sum (a.k.a. cumulative sum or running sum).** A derived array `P` where `P[k]` holds the sum of all elements of the original array from the start up to (some convention of) index `k`. Once built, the sum of any contiguous range of the original array can be recovered from just two entries of `P`.

### Why is this problem difficult?

- **The naive instinct is "just add them up when asked."** Without recognizing the pattern, re-scanning the range on every query looks correct and *is* correct — it is only wasteful, and that waste is invisible until query volume or array size grows.
- **The insight that a running total makes subtraction do all the work is not obvious the first time.** You have to notice that `sum(i, j)` can be rewritten as `sum(0, j) - sum(0, i-1)` — the difference of two "sum from the very beginning" values — and that those "sum from the beginning" values can all be precomputed once, in a single linear pass, before any query ever arrives.
- **Getting the index bookkeeping exactly right is fiddly.** The cleanest version of this trick uses a prefix array that is one element *longer* than the original array, with a leading zero, specifically so range queries never need a special case for "range starts at index 0." Missing this detail is the single most common source of off-by-one bugs in this pattern (see Common Mistakes).

### What happens if we ignore it?

- **Quadratic-or-worse total query cost hiding inside what looks like linear-time code.** A service that runs an O(n) scan per request, called `q` times, is doing O(n·q) work overall — and because each *individual* request looks fast in isolation, this kind of inefficiency is easy to miss in code review and only shows up as a slow endpoint under real traffic.
- **Wasted CPU on data that never changes.** If the underlying array is static between queries (e.g., yesterday's finalized sales figures, a submitted exam's per-question scores), re-deriving the same partial sums over and over is pure waste — the same work is being redone identically every time.
- **Poor scalability under concurrent read load.** A dashboard with many users each requesting different ranges over the same dataset multiplies the naive O(n) cost by every concurrent request, turning a CPU-bound endpoint into a bottleneck exactly when it needs to serve the most traffic.

## Why Not Other Approaches?

**"Just re-scan and sum the range every time a query comes in."**
This is correct and requires zero preprocessing, so it is the right choice if you expect **at most a handful of queries** against an array you will not touch again — building a prefix array for one query is pure overhead. But it does not scale: every additional query costs another full O(range length) pass, and the cost is paid identically each time even though the underlying data has not changed at all between queries.

**"Use a Segment Tree for range-sum queries."**
A Segment Tree answers range-sum queries in O(log n) and also supports point/range **updates** in O(log n) — genuinely more powerful than Prefix Sum. But that power costs real implementation complexity (building and maintaining a tree structure, recursive query/update logic) and O(n) space with a larger constant factor, for a capability — updates — you do not need if the array is static. Reaching for a Segment Tree when there are **no updates** is solving a problem you do not have at the cost of code you did not need to write; Prefix Sum gets you O(1) queries (strictly better than a Segment Tree's O(log n)) with far less code, precisely because it gives up the one thing a Segment Tree buys you: mutability.

**"Use a Fenwick Tree (Binary Indexed Tree) for range-sum queries."**
Same trade as the Segment Tree, just with a smaller constant factor and less code than a full Segment Tree: O(log n) update and O(log n) query, implemented over a compact array using bit tricks. Excellent when updates and queries are interleaved. Overkill — and, in query speed, strictly slower than the O(1) you would get from Prefix Sum — when the array never changes.

**"Cache each query's answer after computing it the slow way (memoization)."**
This only helps if the *exact same* range `(i, j)` is queried more than once. Real range-query workloads (a dashboard with a user-adjustable date-range picker, for instance) tend to ask for a huge variety of distinct ranges, so the cache hit rate is often low, and you still pay the full O(n) cost on every cache miss — which, for a wide-enough range of possible queries, is most of them.

**Tradeoff summary:** every alternative either repeats work that a one-time O(n) precomputation would have eliminated (naive re-scan, memoization with a cold cache) or pays for a capability — dynamic updates — you do not actually need (Segment Tree, Fenwick Tree). Prefix Sum wins precisely when the array is static (or nearly so) and you need the fastest possible query: nothing beats O(1) per query. The moment updates become frequent, this advantage inverts completely — see Disadvantages and When NOT To Use.

## Solution

The core idea: build one auxiliary array, once, that stores **the running total from the start of the array up to each position**. Every subsequent range-sum query is then answered by subtracting two entries of that array — no re-scanning, ever.

Concretely, for an original array `arr` of length `n`, define a prefix array `P` of length `n + 1`:

- `P[0] = 0` (the sum of zero elements, "before the array starts").
- `P[i] = P[i-1] + arr[i-1]` for `i` from 1 to `n` — each entry is the previous running total plus the next element of `arr`.

So `P[i]` always means "the sum of the first `i` elements of `arr`" (i.e., `arr[0] + arr[1] + ... + arr[i-1]`). That leading `P[0] = 0` is not a throwaway detail — it is what lets every range query, including one that starts at index 0, be expressed by the *same* subtraction formula with no special case.

To get the sum of `arr` over the inclusive range `[i, j]` (0-indexed, `i <= j`):

```
rangeSum(i, j) = P[j + 1] - P[i]
```

Why this works: `P[j+1]` is "everything from the start through index `j`," and `P[i]` is "everything from the start through index `i-1`" (everything *before* the range begins). Subtracting removes exactly the prefix you did not want, leaving exactly the elements from `i` through `j`. This is the same "odometer reading at the end minus odometer reading at the start" idea from the analogy above — the two "from the very beginning" totals cancel out everything outside the range you actually asked about.

The thinking behind it:

- **Pay the O(n) cost once, not once per query.** Building `P` is a single linear pass. Every query thereafter is O(1) — two array lookups and a subtraction. The insight is recognizing that the *expensive* part (adding up a lot of numbers) can be done exactly once and reused, because the underlying array does not change.
- **Turn "sum over a range" into "difference of two absolute sums."** This is the same algebraic trick used constantly in physics (work done = potential energy at end minus potential energy at start) and accounting (net change = ending balance minus beginning balance): if you can express a *relative* quantity as the difference of two *absolute* quantities that are cheap to precompute and look up, you replace a scan with a subtraction.
- **Extend the same idea to two dimensions.** The identical logic generalizes to submatrix sums using an inclusion-exclusion formula (see Architecture and Implementation below), which is the basis of "integral images" / "summed-area tables" used in real-time image processing.

## Architecture

The participants in the Prefix Sum pattern are the two arrays and the query formula that connects them:

1. **The original array (`arr`).** The raw input data, unmodified. It is read once, during the build phase, and never touched again unless a mutation forces a rebuild.

2. **The prefix array (`P`).** Length `n + 1`, not `n` — this extra slot (index 0, holding the value 0) is the participant most often overlooked, and it is what makes every range query use one uniform formula with no special-casing for "range starts at index 0." `P[i]` means "sum of the first `i` elements of `arr`," i.e., `arr[0..i-1]`.

3. **The query formula (`rangeSum(i, j) = P[j+1] - P[i]`).** This is the entire "algorithm" at query time — no loop, no scan, just two lookups and a subtraction. It is the payoff for having built `P` in the first place.

4. **(2D variant) The 2D prefix array (`P2D`).** A grid one row and one column larger than the original matrix, where `P2D[r][c]` holds the sum of the rectangular region from `(0,0)` to `(r-1, c-1)`. Built with an inclusion-exclusion recurrence (see Implementation), it plays the exact same role as `P` above, generalized to two dimensions.

Responsibilities in one line each:
- **`arr`:** the source of truth; read once at build time.
- **`P`:** the precomputed running total; the only thing every query actually touches.
- **Query formula:** converts two `P` lookups into the answer for any range, in O(1).
- **`P2D` (2D variant):** the same precomputed running total, generalized so any axis-aligned rectangle's sum is four lookups and three arithmetic operations away.

## Execution Flow

**Build phase**, step by step:

1. Allocate a prefix array `P` of length `n + 1` (one longer than `arr`).
2. Set `P[0] = 0` — the sum of "nothing before the array begins."
3. For `i` from 1 to `n`: set `P[i] = P[i-1] + arr[i-1]`. Each step adds exactly one new element of `arr` to the previous running total.
4. After the loop, `P` is complete: `P[k]` holds the sum of the first `k` elements of `arr`, for every `k` from 0 to `n`. This is a one-time O(n) cost.

**Query phase**, step by step (repeated for every incoming query):

5. Receive a range `[i, j]` (inclusive, 0-indexed) to sum over `arr`.
6. Look up `P[j + 1]`.
7. Look up `P[i]`.
8. Return `P[j + 1] - P[i]`. This is O(1) — no dependency on how wide the range `[i, j]` is.

**If the array mutates** (a value at some index changes): the prefix array must be rebuilt from that index onward (or from scratch, for simplicity) before any further query can trust its answer — see Disadvantages for why this is the pattern's central weakness.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full flowchart deciding between Prefix Sum, Segment Tree/Fenwick Tree, and Sliding Window based on the signals in a problem statement (many range-sum queries? updates present or absent? need O(1) per query?).

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the build-then-query lifecycle (one-time linear build, followed by any number of O(1) queries).

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of the prefix array being built from a concrete example array, followed by a worked range-sum query against it.

## Implementation

[code.cpp](code.cpp) is a **generic, reusable template**, not a solution to one specific LeetCode question — the goal is to see the *shape* of the pattern clearly, separated from any one problem's details, before looking at the worked, problem-specific solutions in [problems/](problems/).

It provides:

- A `PrefixSum` class — constructed from a `std::vector<int>`, it builds the 1D prefix array once in its constructor and exposes a `rangeSum(i, j)` query method plus a `rebuild()` method to recompute after a mutation.
- `buildPrefixSum2D` and `rangeSum2D` — free functions implementing the two-dimensional variant (submatrix sums) using the inclusion-exclusion recurrence, demonstrating that the same core idea extends cleanly beyond one dimension.

## Code Walkthrough

**`PrefixSum` class** (in [code.cpp](code.cpp)). The constructor takes a `const std::vector<int>&` and immediately builds `prefix_`, a member vector of length `n + 1`, with `prefix_[0] = 0` and `prefix_[i] = prefix_[i-1] + arr[i-1]`. `rangeSum(i, j)` validates the indices (throws `std::out_of_range` on an invalid range — a deliberate choice so a bug surfaces loudly instead of silently returning a wrong number) and then returns `prefix_[j+1] - prefix_[i]` in O(1). `rebuild(const std::vector<int>&)` exists specifically to make the mutability problem explicit in code: instead of pretending updates are free, the class forces the caller to explicitly ask for an O(n) rebuild, naming the exact cost that Segment Tree/Fenwick Tree exist to avoid.

**`buildPrefixSum2D`** (in [code.cpp](code.cpp)). Takes a 2D `std::vector<std::vector<int>>` matrix and returns a `(rows+1) x (cols+1)` prefix grid, where each cell is computed as `matrix[r-1][c-1] + P[r-1][c] + P[r][c-1] - P[r-1][c-1]` — add the new cell, add the sum "above," add the sum "to the left," then subtract the top-left sum because it was counted in both "above" and "to the left" (classic inclusion-exclusion). This function exists to show the pattern generalizes to two dimensions with the same "precompute once, subtract for any range" idea, just with one extra term.

**`rangeSum2D`** (in [code.cpp](code.cpp)). Given the built 2D prefix grid and a rectangle `(r1, c1)` to `(r2, c2)` inclusive, returns `P[r2+1][c2+1] - P[r1][c2+1] - P[r2+1][c1] + P[r1][c1]` — the four-corner inclusion-exclusion formula that is the 2D analog of `P[j+1] - P[i]`. This function exists to demonstrate that a submatrix sum, no matter how large, is always exactly four lookups and three additions/subtractions away once the 2D prefix grid exists.

**`main()`** (in [code.cpp](code.cpp)). Exercises `PrefixSum` (including a rebuild after simulating a mutation) and the 2D functions against small, hand-checkable inputs, printing `[PASS]`/`[FAIL]` for each assertion to prove the template compiles and runs correctly end to end.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem — not using the generic `PrefixSum` class directly (to keep each file dependency-free and independently readable), but implementing the *same* prefix-sum logic inline, with problem-specific comments tying every decision back to the general principles established in this README. See [problems/README.md](problems/README.md) for the index. Briefly: `01` is the pure "build once, query many times" use case; `02` and `03` compose a prefix-sum running total with a hash map of *frequencies of prefix-sum values seen so far* — a step up in sophistication that turns "does a subarray with property X exist" into an O(n) single pass; `04` shows a prefix/suffix product variant of the same "precompute from both directions" idea, applied to products instead of sums.

## Advantages

- **O(1) query time after a one-time O(n) build.** Once `P` exists, any range sum — no matter how wide — costs the same fixed, tiny amount of work: two lookups and a subtraction.
- **Trivial to reason about and prove correct.** The formula `P[j+1] - P[i]` has a one-sentence justification (subtract the unwanted prefix), unlike more elaborate data structures that require understanding recursive tree structure.
- **Extremely low constant factor.** No pointers, no recursion, no tree traversal — just array indexing. This makes it faster in practice than a Segment Tree/Fenwick Tree even in big-O terms it would tie or lose to (there is no big-O win to have here, since O(1) already beats O(log n)).
- **Generalizes cleanly to more dimensions.** The same "precompute a running total, subtract to isolate a range" idea extends from 1D ranges to 2D submatrices (and, in principle, further), just by adding inclusion-exclusion terms.
- **Composes with a hash map for a whole family of harder problems.** Tracking the *frequency of prefix-sum values seen so far* turns "does some subarray sum to K" or "does some subarray have equal counts of 0s and 1s" into a single O(n) pass with O(n) space — see problems 02 and 03.

## Disadvantages

- **The array must be (effectively) static.** The entire benefit rests on building `P` once and reusing it for many queries. If the underlying array changes, every query issued *after* that change is answered against stale data unless `P` is rebuilt first.
- **A single-element update forces an O(n) rebuild.** Because `P[i]` depends on `P[i-1]`, changing `arr[k]` invalidates every entry `P[k+1], P[k+2], ..., P[n]` — not just one cell. The cheapest correct fix is either recomputing the whole array (simplest, O(n)) or recomputing the suffix from `k+1` onward (still O(n) in the worst case, since an update near the front invalidates almost everything). There is no way to make a single point update cheaper than O(n) with a plain prefix array — that O(n) cost, repeated for every update, is exactly why **Segment Tree** and **Fenwick Tree** (Binary Indexed Tree) exist: both support point updates *and* range queries in O(log n), a far better bound when updates and queries are interleaved. See [../../advanced-ds-patterns/segment-tree-fenwick-tree/](../../advanced-ds-patterns/segment-tree-fenwick-tree/).
- **O(n) extra space.** The prefix array is a full second array of (roughly) the same size as the original — for the 2D variant, `O(rows * cols)` extra space, which can matter for very large matrices.
- **Integer overflow risk on large arrays.** A running total accumulates every element seen so far; if `arr` holds `int`-range values and `n` is large, `P`'s later entries can overflow a 32-bit `int` even when no individual `arr[i]` is large. Prefer a wider accumulator type (`long long` in C++) for the prefix array unless you have proven the sums cannot overflow.
- **Forgetting to rebuild silently produces wrong answers, not a crash.** Unlike many bugs, a stale prefix array does not throw an exception — it just returns a plausible-looking but incorrect number, which can be far harder to catch in production than a loud failure.

## Tradeoffs

**What we gain versus the naive re-scan:** every range query drops from O(range length) to O(1), at the one-time cost of an O(n) build — a clear win the moment more than a handful of queries are expected against the same static array.

**What we gain versus Segment Tree/Fenwick Tree:** a strictly faster query (O(1) vs. O(log n)) and drastically simpler code (no tree structure, no recursion) — but only because we give up the one capability those structures provide.

**What we lose versus Segment Tree/Fenwick Tree:** the ability to handle updates cheaply. Prefix Sum's O(n) rebuild-per-update is asymptotically worse than either structure's O(log n) update, so the moment updates are frequent and interleaved with queries, this tradeoff flips entirely in the tree structures' favor.

**What we lose versus the naive re-scan:** nothing computationally for a static array — Prefix Sum is strictly better whenever the array does not change and more than one query is expected. The only real cost is the O(n) extra space for `P`, which the naive approach avoids by doing more work per query instead.

## Complexity

**Time:**
- **Build:** O(n) — one pass over the original array, each step doing O(1) work.
- **Query (1D):** O(1) per query, regardless of range width — versus O(j - i + 1) for the naive re-scan of the same range.
- **Total for `q` queries:** O(n + q) with Prefix Sum, versus O(n · q) worst case for repeated naive re-scans (when ranges are wide) — a difference that grows without bound as `q` grows.
- **Build (2D):** O(rows * cols) — one pass over the matrix.
- **Query (2D):** O(1) per submatrix-sum query, versus O(width * height) for the naive nested-loop scan of the same submatrix.

**Space:**
- **1D:** O(n) extra for the prefix array (`n + 1` elements).
- **2D:** O(rows * cols) extra for the prefix grid.

| Scenario | Naive re-scan | Prefix Sum |
|---|---|---|
| One range-sum query | O(n) time, O(1) space | O(n) build + O(1) query = O(n) time, O(n) space (net loss for a single query) |
| `q` range-sum queries | O(n · q) time, O(1) space | O(n + q) time, O(n) space |
| Submatrix sum, one query | O(rows * cols) time, O(1) space | O(rows * cols) build + O(1) query (net loss for a single query) |
| Submatrix sum, `q` queries | O(q * rows * cols) time, O(1) space | O(rows * cols + q) time, O(rows * cols) space |

The table makes the crossover explicit: Prefix Sum only pays for itself once `q` is large enough that the saved re-scan work exceeds the one-time O(n) (or O(rows*cols)) build cost — in practice, this means "more than a couple of queries," since the build cost is exactly one full scan and every naive query is also a full-or-partial scan.

## Common Mistakes

- **Off-by-one on the prefix array's extra leading-zero slot.** Using a prefix array the *same length* as `arr` (instead of `n + 1`) forces an awkward special case for ranges starting at index 0, and is the single most common source of bugs in this pattern. *Why it happens:* it feels natural to make `P` the same size as `arr`. *Avoid:* always allocate `P` with length `n + 1`, always set `P[0] = 0`, and always use the uniform formula `rangeSum(i, j) = P[j+1] - P[i]` with no special case.
- **Forgetting to rebuild the prefix array after a mutation.** Once any element of `arr` changes, every query against the old `P` silently returns a wrong (but plausible-looking) answer — there is no crash to alert you. *Why it happens:* the mutation and the next query are often in different parts of the code, so the dependency is not visually obvious. *Avoid:* wrap `arr` and `P` in a single class (as `PrefixSum` does) so a mutation method can force (or at least flag the need for) a rebuild, rather than leaving `arr` and `P` as two independently-mutable free-floating arrays.
- **Confusing `P[i]` with `arr[i]`.** `P[i]` is "sum of the first `i` elements of `arr`," not "the prefix sum *at* `arr[i]`" — these are the same value only if you carefully track whether your convention is "sum through index `i`" (`P` same length as `arr`, no leading zero) or "sum of the first `i` elements" (`P` one longer, leading zero at `P[0]`). Mixing the two conventions mid-solution is a very common source of off-by-one errors. *Avoid:* pick the `n+1`-length, leading-zero convention used throughout this module and stay consistent.
- **Integer overflow from using `int` for the prefix array.** A running total can grow far larger than any individual element; on `int`-only accumulation this silently wraps around for large `n` or large values. *Avoid:* use `long long` (C++) for the prefix array's element type unless you have explicitly bounded the maximum possible sum.
- **Applying Prefix Sum to a frequently-updated array without noticing.** Reaching for a plain prefix array in a system where the underlying values change often (e.g., a live leaderboard, a streaming counter) leads to either constant expensive rebuilds or silently stale answers. *Avoid:* check the update frequency before choosing this pattern — see When NOT To Use.

## When To Use

- **Many range-sum (or range-count, range-average) queries over a fixed, unchanging array.** The canonical case: the array is built once (or rarely changes) and then queried many times with different ranges.
- **The array is effectively read-only for the query workload** — e.g., a finalized day's worth of sales data, a submitted exam's scores, a completed dataset being analyzed.
- **You need to check properties of subarrays via their sums** — "does a subarray summing to K exist," "is there a subarray with equal 0s and 1s," "what is the maximum-length subarray with sum <= S" — all of which reduce to comparing or looking up prefix-sum values.
- **You need submatrix (2D range) sums repeatedly** — image processing (integral images / summed-area tables for fast box blur, feature detection), 2D grid analytics.
- **You are computing prefix/suffix aggregates other than plain sums** — prefix products, prefix maximums, prefix XORs — the same "precompute once, combine two precomputed values" idea generalizes to any associative operation, though the *subtraction* trick specifically requires an invertible operation (sum and product both qualify; max/min do not, which is why "range minimum query" typically needs a different structure — a Sparse Table or Segment Tree).

## When NOT To Use

- **The array is updated frequently, and updates are interleaved with range queries.** Every update forces an O(n) rebuild (or at best an O(n) suffix-rebuild); if updates happen often, this dominates and erases the O(1)-query advantage entirely. Reach for a **Segment Tree** or **Fenwick Tree (Binary Indexed Tree)** instead — both support point updates and range queries in O(log n), which is the right tradeoff once updates are frequent. See [../../advanced-ds-patterns/segment-tree-fenwick-tree/](../../advanced-ds-patterns/segment-tree-fenwick-tree/).
- **You only expect one or two queries total against the array.** Building the full prefix array costs the same O(n) as just answering the query directly with a simple scan — the precomputation does not pay for itself.
- **The aggregate you need is not invertible (e.g., range minimum/maximum).** The `P[j+1] - P[i]` trick relies on being able to "subtract off" the unwanted prefix; minimum and maximum have no inverse operation (you cannot "un-take-the-min" of a value once folded in), so plain Prefix Sum does not apply — a Sparse Table (for static data) or Segment Tree (if updates are needed) is the right tool for range min/max queries instead.
- **Memory is severely constrained and the array is huge.** The O(n) (or O(rows*cols) in 2D) extra space for the prefix array may not be affordable; in that case, the naive per-query scan trades memory for time.

## Real Interview/Production Examples

Prefix Sum appears constantly in interviews as the "aha" behind an otherwise-quadratic-looking subarray problem — Range Sum Query - Immutable, Subarray Sum Equals K, and Contiguous Array (equal 0s and 1s) are among the most commonly cited "recognize the pattern" problems, because the brute force is an obvious O(n²) nested loop and the prefix-sum-plus-hash-map trick collapses it to O(n).

Beyond interviews, the same idea shows up directly in production systems:

- **Financial ledgers and running-balance queries.** Bank statements, accounting systems, and expense-tracking apps precompute running balances so "what changed between two dates" is a subtraction, not a re-derivation from transaction history.
- **Analytics dashboards.** Precomputed cumulative metrics (cumulative revenue, cumulative signups, cumulative page views by day) let a dashboard answer "totals over this date range" for any user-selected window without re-aggregating raw event data on every request.
- **Image processing: integral images / summed-area tables.** A 2D prefix sum over pixel intensities lets a box blur, a Haar-like feature (as used in the classic Viola-Jones face-detection algorithm), or any rectangular-region average be computed in O(1) per rectangle, regardless of the rectangle's size — a direct real-world application of the 2D prefix sum in this module.
- **Database materialized views and rollup tables.** A data warehouse precomputing daily/monthly cumulative rollups (e.g., a `running_total` column maintained by a window function like SQL's `SUM(...) OVER (ORDER BY date)`) is doing prefix summation at the database layer, for exactly the same reason: answer range aggregates without re-scanning raw rows.
- **Fenwick Tree / Binary Indexed Tree as "prefix sum with cheap updates."** Understanding plain prefix sum first is a prerequisite for understanding why the Fenwick Tree's clever bit-manipulation trick exists at all — it is, at its core, a prefix-sum structure re-engineered to support O(log n) point updates.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **A daily/weekly revenue or metrics dashboard endpoint** that precomputes a cumulative array once per day (after the day's data is finalized) and serves arbitrary date-range totals to many dashboard widgets in O(1) each, instead of re-aggregating from raw rows per request.
2. **An API rate-limit or quota usage report** ("how many requests did this API key make between hour X and hour Y") built from a precomputed per-hour request-count array.
3. **A batch data-validation job** that needs to repeatedly check "does any contiguous window of transactions sum to exactly this suspicious amount" (a Subarray Sum Equals K-style fraud-detection heuristic) across a large, finalized transaction log.
4. **An image-processing microservice** applying box blur or computing local-region brightness/contrast using a precomputed integral image (2D prefix sum) instead of recomputing each region's pixel sum from scratch.
5. **A log-analytics tool** answering "how many error events occurred between timestamp A and timestamp B" against an already-ingested, static batch of log data, using a precomputed per-time-bucket count array.

## Similar Patterns

- **Sliding Window** ([../sliding-window/](../sliding-window/)): also concerned with sums (or other aggregates) over subarrays, but answers a *different* question shape. Sliding Window incrementally maintains a running aggregate over a **contiguous window that moves through the array once**, answering "what is the best/valid window ending here" as it goes — it is built for a *single pass* that needs the answer at every position, not for answering arbitrary, independently-chosen range queries after the fact. Prefix Sum instead does all the work up front so that **any** range, chosen after the fact and in any order, can be answered in O(1). If you need "the sum of *this specific* range, given right now" repeatedly and unpredictably, that is Prefix Sum; if you need "scan through once, tracking the best contiguous window as you go," that is Sliding Window.
- **Segment Tree / Fenwick Tree** ([../../advanced-ds-patterns/segment-tree-fenwick-tree/](../../advanced-ds-patterns/segment-tree-fenwick-tree/)): answer the same kind of range-aggregate query, but are built for a world where the array **also** receives updates. Both trade Prefix Sum's O(1) query for O(log n) query, in exchange for O(log n) updates instead of Prefix Sum's O(n) rebuild-per-update. Choosing between them is entirely about update frequency, not query pattern.
- **Difference Array (the "reverse" of Prefix Sum):** where Prefix Sum precomputes running sums to answer range-sum *queries* in O(1), a Difference Array precomputes running *differences* to answer range-*update* operations in O(1) each (add a value to every element in a range), deferring the O(n) cost to a single final reconstruction pass. It is the same subtraction trick applied to the opposite problem — cheap range updates instead of cheap range queries — and the two are often taught together because recognizing "is this a query-heavy or update-heavy range problem" decides which one (or which tree structure) applies.

| Pattern | What it precomputes | Query cost | Update cost | Best for |
|---|---|---|---|---|
| Prefix Sum | Running sum from the start | O(1) | O(n) rebuild | Many range-sum queries, static/rarely-changing array |
| Sliding Window | Running aggregate over a moving window | O(1) amortized, computed once per position during a single pass | N/A (not a query-after-the-fact structure) | Single pass needing the best/valid contiguous window as it goes |
| Segment Tree | A tree of partial aggregates | O(log n) | O(log n) | Frequent updates interleaved with range queries; supports non-invertible aggregates (min/max) too |
| Fenwick Tree (BIT) | A compact tree of partial sums via bit tricks | O(log n) | O(log n) | Same as Segment Tree, but simpler/faster in practice, limited mostly to invertible aggregates (sum, XOR) |

## Interview Discussion

Experienced engineers rarely spend interview time on "how do you compute a running sum" — that is mechanical. What they actually probe is whether you recognize **when precomputation pays for itself**, and whether you can extend the base idea to the "prefix sum + hash map of frequencies" family that solves subarray-existence questions in O(n).

Common follow-up questions:
- *"Why build a prefix array of length n+1 instead of n?"* — expects the specific "range starting at index 0 needs no special case" justification, not just "that's how everyone does it."
- *"The array is updated occasionally — does Prefix Sum still work?"* — expects recognizing the O(n) rebuild cost per update, and naming Segment Tree/Fenwick Tree as the right tool once updates become frequent, rather than insisting Prefix Sum still applies unchanged.
- *"How would you find a subarray that sums to exactly K?"* — expects the prefix-sum-plus-hash-map-of-seen-prefix-sums technique (Subarray Sum Equals K), not a nested-loop brute force, and an explanation of *why* it works: `prefixSum[j] - prefixSum[i] == K` rearranges to `prefixSum[i] == prefixSum[j] - K`, so you are really asking "have I seen this specific prefix-sum value before," which a hash map answers in O(1).
- *"Can this technique handle range minimum queries the same way?"* — expects recognizing that subtraction only works for invertible operations; minimum/maximum have no inverse, so a different structure (Sparse Table, Segment Tree) is needed.
- *"How does this generalize to two dimensions?"* — expects the four-corner inclusion-exclusion formula and, ideally, a real application (integral images).

Common misconceptions:
- "Prefix Sum is only useful for a single range query." It is a net *loss* for a single query (you pay O(n) to build, for one O(1) lookup you could have gotten with an O(n) direct scan) — its value is entirely in amortizing the build cost across many queries.
- "You can just patch the prefix array in O(1) after an update." You cannot, in general — because every `P[k]` for `k` at or after the updated index depends on the updated value, so an update invalidates a whole suffix of `P`, not one cell.
- "Prefix Sum and Sliding Window solve the same problems." They answer structurally different questions (arbitrary after-the-fact range queries vs. a single pass tracking the best contiguous window) even though both involve "sums over subarrays."
- "The leading zero slot in the prefix array is just a convention, not load-bearing." It specifically eliminates the need for a special case when a query's range starts at index 0 — removing it forces exactly that special case back into the query formula.

## Summary

- Prefix Sum precomputes a running total once, so any subsequent range-sum query costs O(1) instead of O(range length).
- Build: `P[0] = 0`, `P[i] = P[i-1] + arr[i-1]` for `i = 1..n` — one O(n) pass.
- Query: `rangeSum(i, j) = P[j+1] - P[i]` — two lookups, one subtraction, regardless of range width.
- The technique's whole value proposition rests on the array being static (or rarely-changing); a single point update forces an O(n) rebuild.
- The 2D variant (submatrix sums) uses the same idea with a four-term inclusion-exclusion formula, and is the basis of integral images/summed-area tables in image processing.
- Composed with a hash map of prefix-sum frequencies, the same idea solves a whole family of "does a subarray with property X exist" problems in O(n).
- When updates and queries are both frequent, Segment Tree or Fenwick Tree (Binary Indexed Tree) are the right tools — both trade O(1) query for O(log n) query in exchange for O(log n) updates instead of O(n) rebuilds.
- Only works directly for **invertible** aggregates (sum, product, XOR) — range minimum/maximum needs a different structure entirely.

## Key Takeaways

1. Prefix Sum turns O(range length) per query into O(1) per query, at a one-time O(n) build cost.
2. The prefix array is `n + 1` long, with `P[0] = 0`, specifically so no query needs a special case for ranges starting at index 0.
3. The query formula `rangeSum(i, j) = P[j+1] - P[i]` works because subtracting two "sum from the very start" values cancels out everything outside the desired range.
4. It only pays for itself once more than a handful of queries are expected against a static (or rarely-changing) array.
5. A single point update forces an O(n) rebuild — the pattern's central weakness, and the entire reason Segment Tree/Fenwick Tree exist.
6. The 2D variant needs a four-corner inclusion-exclusion formula, and underlies real image-processing techniques like integral images.
7. Composing a prefix-sum running total with a hash map of "prefix-sum values seen so far" solves subarray-existence problems (sum equals K, equal 0s/1s) in O(n).
8. The subtraction trick only works for invertible operations (sum, product, XOR) — not for minimum/maximum.
9. Use `long long`/wide accumulators for the prefix array to avoid silent overflow on large arrays or large values.
10. Don't confuse this with Sliding Window — Prefix Sum answers arbitrary after-the-fact range queries; Sliding Window tracks a moving window during a single pass.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — covers cumulative/running-sum precomputation techniques and their role in reducing repeated-query costs, plus the general divide between static and dynamic range-query data structures (Segment Trees, Fenwick Trees) that this module contrasts against.
- *Competitive Programmer's Handbook* — Antti Laaksonen — has a concise, practical chapter on prefix sums, including the 2D generalization, and the transition to Binary Indexed Trees for the dynamic case.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes worked prefix-sum-style array problems with C++-specific implementation notes and complexity analysis.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — covers array/subarray problems where cumulative sums are the key insight, alongside general interview framing.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including prefix-sum and Fenwick Tree implementations, useful for seeing varied implementation styles.
- OpenCV's `integral()` function (part of the OpenCV project on GitHub) — a production implementation of the 2D prefix sum / summed-area table used for fast box filtering and Haar-feature computation in real computer-vision pipelines.

**Official Documentation**
- LeetCode — Range Sum Query - Immutable (problem 303).
- LeetCode — Range Sum Query 2D - Immutable (problem 304).
- LeetCode — Subarray Sum Equals K (problem 560).
- LeetCode — Contiguous Array (problem 525).
- LeetCode — Product of Array Except Self (problem 238).
- cppreference.com — `std::partial_sum` (in `<numeric>`) — the C++ Standard Library's own building block for computing a prefix-sum array in one call.
- OpenCV documentation — `cv::integral` — the official docs for OpenCV's integral-image (2D prefix sum) function, describing its use in fast rectangular-region sum computation.

**Blog Articles**
- GeeksforGeeks — "Prefix Sum Array — Implementation and Applications" — a widely used explainer covering the base technique, 2D generalization, and common problem shapes.
- NeetCode — Prefix Sum pattern videos/playlist — walks through Range Sum Query, Subarray Sum Equals K, and Product of Array Except Self with visual explanations.
- Educative.io — "Grokking the Coding Interview" style pattern write-ups on cumulative/prefix-sum techniques, part of the same pattern-based framing this repository follows.
