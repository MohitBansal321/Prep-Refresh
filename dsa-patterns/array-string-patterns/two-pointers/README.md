# Two Pointers

## Intent

Replace a nested-loop scan over pairs (or triplets) of array elements with a single linear pass, by walking two indices through the array according to a rule that never needs to revisit a discarded position.

## Real Life Analogy

Think of two friends searching for each other's names in a **phone book sorted alphabetically**, standing at opposite ends of the book, each flipping toward the middle. If one friend is looking for "Nguyen" and lands on "Anderson" (too early alphabetically), they flip forward. If they land on "Zimmerman" (too late), they flip backward. They never need to re-check pages they have already ruled out, and because the book is sorted, every flip is a *guaranteed* step toward the answer, not a guess. Two people cooperating, each discarding half the remaining problem on every step, find the match in a handful of flips instead of one person reading the whole book page by page.

Another everyday one: **two lock-keepers closing a canal from both ends toward the middle**, each only moving their gate inward when the water level on their side proves it is safe to do so. Neither gate ever needs to move backward — once ruled out, a position stays ruled out.

Two Pointers is that same idea in code: two indices, each moving in a direction justified by what has already been compared, converging on (or sweeping past) the answer without ever re-examining discarded ground.

## Problem

### What engineering problem exists?

A huge fraction of array and string problems boil down to one of these shapes:

- **"Does some pair (or triplet) of elements satisfy a condition?"** — e.g. do two numbers in this sorted list sum to a target? Do three numbers sum to zero?
- **"What is the best value derivable from two positions in the array?"** — e.g. the largest rectangle of water held between two walls.
- **"Compact this array in place, keeping only elements that satisfy some rule, without extra memory."** — e.g. remove duplicates from a sorted array, or move all zeroes to the end, while preserving relative order.

The naive way to answer any of these is to check **every combination**. For a pair-sum search over `n` elements, that means a nested loop: for each `i`, scan every `j > i` and test the pair. That is `n * (n-1) / 2` comparisons — quadratic in `n`. For `n = 10,000` elements, that is roughly 50 million comparisons for something a human intuition says should be almost instant.

> **Term: Two Pointers.** A technique where you maintain two integer indices (`left`/`right`, or `slow`/`write` and `fast`/`read`) into the same array or string, and advance them according to a rule tied to the data's order, instead of comparing every pair with nested loops.

### Why is this problem difficult?

- **The naive instinct is always "compare everything."** Without a name for the pattern, most engineers reach for a nested loop by default — it is the most obvious way to check "does any pair satisfy X?"
- **Knowing WHICH pointer to move, and why it is safe, is not obvious the first time.** The correctness of Two Pointers rests on a proof, not a heuristic: at each step you must be able to argue that discarding a position can *never* cost you the correct answer. Getting this backward (moving the wrong pointer) either produces wrong answers or an infinite loop.
- **It only works when the data has exploitable order.** The technique leans on sortedness (for the converging variant) or on "we only need to preserve relative order while filtering" (for the same-direction variant). Applying it to genuinely unordered data without first sorting silently produces wrong answers, because the "moving this pointer can never help" argument depends entirely on the data being ordered.

### What happens if we ignore it?

- **Quadratic time on problems that admit a linear solution.** A pair-sum search coded as nested loops is O(n²); at `n = 100,000` (a realistic input size for a backend batch job, not just a contrived interview constraint) that is **10 billion** comparisons — multiple seconds to minutes of wasted CPU time where a linear pass would take milliseconds.
- **Unnecessary extra memory.** Reaching for a hash set to solve a problem that sorted-input Two Pointers could solve in O(1) extra space wastes memory proportional to `n` — relevant when processing large batches or running inside a memory-constrained environment (a Lambda function, an embedded system, a hot path processing millions of requests).
- **In-place compaction done wrong becomes O(n²).** Removing elements from the middle of a `std::vector` one at a time (e.g. repeated `erase()`) shifts every subsequent element down by one on every call — turning what should be a single O(n) pass into O(n²) element movement.

## Why Not Other Approaches?

**"Brute-force nested loops over every pair/triplet."**
For pair-sum search: O(n²) time, O(1) space. For 3Sum: O(n³) time, O(1) space. Correct, but the complexity is the whole problem — a service processing large arrays will time out or burn CPU that a linear-time approach would not. This is the "does it even finish in time" failure mode, not a subtle bug.

**"Sort, then use a hash map (or hash set) to find complements."**
For an unsorted pair-sum search where you must preserve **original indices**, this is actually the *right* choice (classic LeetCode "Two Sum," not "Two Sum II") — O(n) time, O(n) space, and it does not require the array to be sorted at all. But once the array **is** sorted (or index-order does not matter), Two Pointers gets the same O(n) time in O(1) space, because the sort order itself does the work a hash set would otherwise need memory to do. Using a hash set on already-sorted data is strictly wasteful — you are paying for information you already have for free.

**"Sort, then binary-search for the complement of each element."**
This works — O(n log n) for the sort, then O(log n) per element for `n` elements, giving O(n log n) total. That is asymptotically worse than Two Pointers' O(n) (after the same O(n log n) sort, if a sort was even needed), and it is strictly more code: you still need the sort, plus a binary search routine, plus edge-case handling for the search. Two Pointers reaches the same or better bound with less machinery once the input is sorted.

**"Use recursion / backtracking to enumerate all pairs/triplets."**
This is essentially the brute force above wearing a different syntax — same O(n²)/O(n³) complexity, plus function-call overhead and stack depth to worry about. There is no complexity benefit, only added ceremony.

**Tradeoff summary:** every alternative either pays for information the sorted array already encodes (hashing), adds an unnecessary log factor (binary search per element), or is a straightforward restatement of the brute force (recursion). Two Pointers wins specifically *because* it is the only approach that turns "the array is sorted" into a direct, O(1)-space, O(n)-time elimination rule — that is its entire value proposition, and it is worthless the moment the sortedness assumption is not actually true.

## Solution

The core idea splits into two flavors, and recognizing which one a problem wants is half the battle.

**Converging pointers (opposite ends, moving inward).** Place one pointer, `left`, at the start of a **sorted** array, and another, `right`, at the end. At each step, compute something from both elements (their sum, or the area they bound) and compare it to what you need. Based on that comparison, you can *prove* that moving one specific pointer inward is the only way to make progress — moving the other pointer would only make things worse or leave you no better off. You repeat until the pointers meet or you find what you need. This is the shape behind pair-sum search, 3Sum's inner loop, and container/area-maximization problems.

**Same-direction pointers (both start together, move at different rates).** Place a "slow" pointer (often called `write`) and a "fast" pointer (`read`) both at the start of the array, both moving forward, but `read` advances every step while `write` only advances when it finds something worth keeping. `write` marks the boundary of the "already processed / already compacted" region; `read` is the one doing the scanning. This is the shape behind in-place deduplication, filtering, and partitioning.

Both flavors share the same underlying discipline: **every pointer movement must be justifiable** — you should be able to say, in one sentence, why moving this specific pointer (and not the other, and not both) cannot possibly cost you the correct final answer. If you cannot state that justification, you likely have the direction (or the pattern itself) wrong.

## Architecture

The "participants" in a Two Pointers algorithm are the pointers themselves and the invariant each one protects:

1. **`left` pointer (converging variant).** Starts at index 0 (the smallest element, given sorted input). Its invariant: everything at index `< left` has already been proven **not** to be part of the answer, given the current `right`. Moving `left` forward means "commit to the belief that the element currently at `left` cannot combine with anything at or beyond the current `right` to satisfy what we need — move on to a larger candidate."

2. **`right` pointer (converging variant).** Starts at the last index. Its invariant: everything at index `> right` has already been proven **not** to be part of the answer, given the current `left`. Moving `right` backward means the symmetric commitment: "the element at `right` cannot combine with anything at or before `left` — try a smaller candidate."

3. **`write` pointer (same-direction variant).** Starts at index 0. Its invariant: `arr[0 .. write-1]` is the fully compacted, final result **so far** — every element in that range is confirmed correct and will never be touched again. Moving `write` forward means "the element just read has been accepted into the final result."

4. **`read` pointer (same-direction variant).** Starts at index 0, always at or ahead of `write`. Its invariant: it has examined every element from index 0 up to its current position exactly once. Moving `read` forward means "this element has been judged (kept or discarded); move to the next candidate."

5. **The comparison/goal (implicit participant).** Whether it is `target` (an exact sum to match), a running "best so far" (for optimization problems), or a `keep_predicate` (for compaction), this is what every pointer movement is justified *against*. Without a precise goal, "which pointer moves" has no answer.

Responsibilities in one line each:
- **`left` / `right`:** narrow the search space from both ends, each only moving when doing so is provably safe.
- **`write` / `read`:** `read` observes everything once; `write` remembers only what has been confirmed worth keeping.
- **Goal/comparison:** the yardstick that turns "what did we just observe" into "which pointer moves next."

## Execution Flow

**Converging pointers**, step by step:

1. Confirm (or make true by sorting) that the array is sorted in the order the problem needs.
2. Initialize `left = 0` and `right = n - 1`.
3. While `left < right`:
   a. Compute the value derived from `arr[left]` and `arr[right]` (a sum, a product of width and height, etc.).
   b. Compare that value against the target or update a running best answer.
   c. If searching for an exact match and it is found, stop and return the answer.
   d. Otherwise, decide — using the comparison — exactly one of `left++` or `right--`, based on which side is *provably* not going to yield a better/valid result by staying put.
4. If the loop ends because `left` and `right` met (or crossed) without a match, report "not found" (or return the best answer accumulated so far, for optimization problems).

**Same-direction pointers**, step by step:

1. Initialize `write = 0`.
2. For `read` from `0` to `n - 1`:
   a. Evaluate whether `arr[read]` should be kept, using whatever rule the problem defines (differs from the last kept value, is non-zero, satisfies a predicate, etc.).
   b. If it should be kept, copy `arr[read]` into `arr[write]` and increment `write`.
   c. If not, do nothing except let `read` continue — `write` stays where it is.
3. After the loop, `arr[0 .. write-1]` holds the compacted result; `write` itself is the new logical length.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full flowchart deciding between Two Pointers, Sliding Window, Hashing, and Binary Search based on the signals in a problem statement.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the converging-pointers loop (initialize, compare, decide which pointer moves, repeat).

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of `left`/`right` positions and comparisons across the concrete Pair with Target Sum example (array `[1,2,3,4,6,8,9,14,15]`, target `13`).

## Implementation

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the *shape* of the pattern clearly, separated from any one problem's details, before looking at the worked, problem-specific solutions in [problems/](problems/).

It provides four small function templates:

- `two_pointer_find_pair` — the converging, exact-match search (works over any random-access container and any "combine" function, defaulting to addition), returning the pair of indices if found.
- `two_pointer_max_area` — the converging, optimization search, tracking a running best answer and always discarding the "limiting" (shorter) side.
- `two_pointer_compact` — the same-direction, in-place compaction routine, parameterized by an arbitrary keep-predicate.
- `two_pointer_dedupe_sorted` — a convenience wrapper over `two_pointer_compact` implementing "keep at most K copies of each value in a sorted range," which generalizes both "remove duplicates" (K=1) and "remove duplicates II" (K=2)-style problems.

Templates (rather than hard-coded `int`/`vector<int>` signatures) are used deliberately so the same four functions work over `vector<long long>`, `vector<double>`, or any other random-access container without rewriting the pointer logic — the pointer *movement rules* are what matter, not the element type.

## Code Walkthrough

**`two_pointer_find_pair`** (in [code.cpp](code.cpp)). Takes a sorted container, a target value, and an optional `combine` function (defaulting to `std::plus<>`, i.e. plain addition). Places `left` at 0 and `right` at the last index, and loops while `left < right`, computing `combine(sorted[left], sorted[right])` each iteration. If it matches `target`, the pair of indices is returned immediately wrapped in `std::optional`. If the combined value is too small, `left` advances (only a larger left element can raise the sum, given sorted ascending order); if too large, `right` retreats. Returns `std::nullopt` if the pointers cross without a match. This function exists to demonstrate the exact-match converging flavor in its most generic form — it is the direct ancestor of [problems/01-pair-with-target-sum.cpp](problems/01-pair-with-target-sum.cpp).

**`two_pointer_max_area`** (in [code.cpp](code.cpp)). Same initial placement (`left = 0`, `right = n - 1`), but instead of stopping at a match, it computes `width * min(heights[left], heights[right])` on every iteration, keeps a running `best`, and always advances the pointer at the **shorter** of the two current walls — because the shorter wall is what caps the area, and keeping it in place while shrinking the width can never help. This function exists to demonstrate the optimization-flavor converging search, and is the direct ancestor of [problems/04-container-with-most-water.cpp](problems/04-container-with-most-water.cpp).

**`two_pointer_compact`** (in [code.cpp](code.cpp)). Takes a mutable container and a `keep_predicate(container, read_index, write_index) -> bool`. Loops `read` from 0 to the end; whenever the predicate says to keep the current element, it is copied to `arr[write]` and `write` advances. Returns the final `write` value — the new logical length of the compacted prefix. This function exists as the single generic engine behind every same-direction compaction problem; only the predicate changes between "remove zeroes," "remove a specific value," and "deduplicate."

**`two_pointer_dedupe_sorted`** (in [code.cpp](code.cpp)). A thin wrapper around `two_pointer_compact` whose predicate keeps an element unless it duplicates the value sitting `max_allowed_duplicates` slots back in the already-written prefix. This exists to show how a single generic engine (`two_pointer_compact`) can express a whole family of related problems (allow 1 copy, allow 2 copies, ...) just by changing the predicate — no new pointer logic needed.

**`main()`** (in [code.cpp](code.cpp)). Exercises all four functions against small, hand-checkable inputs and prints `[PASS]`/`[FAIL]` for each assertion, proving the template compiles and runs correctly end to end.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem — not using the generic templates above directly (to keep each file dependency-free and independently readable), but implementing the *same* pointer-movement logic inline, with problem-specific comments tying every decision back to the general principles established in this README. See [problems/README.md](problems/README.md) for the index and the "why these four" rationale. Briefly: `01` is the pure exact-match converging search; `02` is the pure same-direction compaction; `03` composes converging pointers with an outer loop and sorting to solve a harder (triplet) problem, and is the canonical example of duplicate-skipping; `04` is the pure optimization-flavor converging search with a non-obvious but provable elimination rule.

## Advantages

- **Linear time where brute force is quadratic (or cubic).** Turns O(n²) pair searches into O(n), and O(n³) triplet searches into O(n²) — often the difference between "fast enough" and "times out" on realistic input sizes.
- **Constant extra space.** No hash set, no auxiliary array — just a couple of integer indices. This matters directly in memory-constrained contexts (embedded systems, high-throughput services processing many arrays concurrently, Lambda's memory limits).
- **In-place mutation.** The same-direction variant modifies the array where it stands, avoiding the allocation and copy cost of building a new filtered array.
- **Provable correctness, not heuristic.** Every pointer movement has an explicit justification ("this element cannot be part of any valid answer given what we already know"), which makes the algorithm easy to prove correct and easy to explain precisely — a genuine advantage in code review and interviews alike.
- **Composable.** The converging search nests naturally inside an outer loop (3Sum: fix one element, converge on the rest), extending the technique from pairs to triplets (and, in principle, further) without inventing new machinery.

## Disadvantages

- **Requires sorted (or sort-tolerant) input.** If the array is unsorted and original index order matters (as in classic "Two Sum," not "Two Sum II"), Two Pointers cannot be applied directly — you would need to sort a copy (losing original indices) or fall back to hashing.
- **Direction mistakes are silent, not loud.** Moving the wrong pointer, or moving both when only one should move, does not usually crash — it just silently produces a wrong answer or an infinite loop, which can be harder to catch than an exception.
- **Duplicate handling adds real complexity in triplet+ problems.** 3Sum's duplicate-skipping logic is easy to get subtly wrong (skip too early and you miss a valid triplet; skip too late and you emit a duplicate) — see Common Mistakes below.
- **Does not generalize to arbitrary conditions.** The technique depends on the comparison being **monotonic** as pointers move (moving `left` forward always increases the tracked value; moving `right` backward always decreases it, or similar). If the relationship is not monotonic, no direction is provably safe, and Two Pointers cannot be applied at all.
- **Sorting cost, if needed, is not free.** If the input arrives unsorted and sorting is required first, that is an unavoidable O(n log n) up front — Two Pointers only wins over a hash-based approach if the array is *already* sorted, or if the problem needs a sort anyway for other reasons (as 3Sum does, for duplicate-skipping).

## Tradeoffs

**What we gain versus brute force:** we go from O(n²)/O(n³) time down to O(n)/O(n²), in O(1) extra space, by exploiting the sortedness of the data to eliminate large portions of the search space per step instead of checking every combination.

**What we gain versus hashing:** the same O(n) time bound (for pair-sum search) but in O(1) space instead of O(n) — because the sort order does the "have I seen a complement" work that a hash set would otherwise need memory to do.

**What we lose versus hashing:** hashing works on **unsorted** data and preserves **original indices** for free; Two Pointers needs the data sorted (destroying original index order unless you separately track it) and cannot be applied at all if the array cannot be sorted or reordered.

**What we lose versus brute force:** nothing computationally — Two Pointers is strictly better in complexity whenever it applies. The only thing "lost" is applicability: brute force works on any condition, sorted or not, monotonic or not; Two Pointers only works when the monotonicity argument actually holds.

## Complexity

**Time:**
- Converging pointers (pair search / optimization): **O(n)** — `left` and `right` together move at most `n - 1` times before meeting, and each step is O(1) work. This holds in the **best, worst, and average** case alike, because the loop bound is structural (total pointer movement), not data-dependent.
- Same-direction pointers (compaction): **O(n)** — `read` visits each element exactly once; `write` moves at most as often as `read`. Same bound in best, worst, and average case.
- 3Sum (converging pointers nested in an outer loop, plus the initial sort): **O(n log n)** for the sort, **O(n²)** for the outer loop times the inner O(n) scan — dominated by the O(n²) term, so **O(n²)** overall.

**Space:**
- Converging and same-direction variants: **O(1)** extra space — a fixed, small number of index/accumulator variables, independent of `n`.
- 3Sum: **O(1)** extra space beyond the output list itself (the sort is typically in-place or uses O(log n) recursion stack, depending on the standard library's introsort implementation).

**Comparison to the brute force each replaces:**

| Problem shape | Brute force | Two Pointers |
|---|---|---|
| Pair-sum search (sorted input) | O(n²) time, O(1) space | O(n) time, O(1) space |
| Triplet-sum search (3Sum) | O(n³) time, O(1) space | O(n²) time, O(1) extra space |
| In-place deduplication | O(n²) time (repeated `erase()` shifts) | O(n) time, O(1) space |
| Area/capacity maximization | O(n²) time, O(1) space | O(n) time, O(1) space |

## Common Mistakes

- **Using Two Pointers on unsorted data.** The entire "moving this pointer is safe" argument depends on sort order. Applying the pattern to an unsorted array either produces wrong answers or requires re-deriving a completely different (and usually invalid) justification. *Avoid:* explicitly confirm sortedness (or sort first, tracking original indices separately if you need them) before writing any pointer logic.
- **Off-by-one on pointer initialization or the loop condition.** Starting `right` at `n` instead of `n - 1`, or using `left <= right` when the algorithm assumes distinct elements (`left < right`), silently reads out of bounds or double-counts an element as both `left` and `right`. *Avoid:* be explicit about whether the two pointers are allowed to be equal, and write the loop condition to match that decision exactly.
- **Moving the wrong pointer, or moving both when only one should move.** This breaks the monotonicity guarantee and either misses the answer or loops forever (if a `while` condition depends on the pointers converging and neither ever moves). *Avoid:* before writing the comparison branch, state in a comment which pointer moves in each branch and why the other one provably cannot help right now.
- **Forgetting to skip duplicates in 3Sum (and similar triplet+ problems).** Without the `while (left < right && nums[left] == nums[left+1]) ++left;` (and the symmetric check on `right`, and the outer-loop check on `i`), the same triplet *value* gets emitted multiple times from different index combinations. *Why it happens:* the uniqueness requirement is about **values**, not indices, and that distinction is easy to overlook when transplanting the Two Sum II logic wholesale. *Avoid:* after recording a match, explicitly advance past every remaining duplicate on both sides before continuing.
- **Assuming the same-direction predicate can look arbitrarily far back or forward.** A dedupe predicate that only compares `arr[read]` to `arr[write - 1]` is correct **only because the input is sorted** (so all duplicates of a value are contiguous). Applying the same one-comparison predicate to unsorted data would miss duplicates that are not adjacent. *Avoid:* be explicit in comments about which invariant (sortedness, or something else) makes a narrow local comparison sufficient.
- **Using Two Pointers for a variable-size contiguous window problem.** Confusing "sum of some subarray with constraint" (Sliding Window's territory) with "pair of elements at two ends" (Two Pointers' territory) leads to writing pointer logic that does not actually match the problem's shape. *Avoid:* re-check the Recognition Diagram — a *contiguous range with a running property* wants Sliding Window; a *pair/triplet from anywhere in a sorted array* wants Two Pointers.

## When To Use

- The input is sorted (or you can sort it without losing information you still need) and you are searching for a **pair or triplet** satisfying a sum/comparison condition.
- You need to find an **optimum** (maximum area, maximum capacity, minimum difference) that is naturally expressed as a function of two positions in the array.
- You need **in-place compaction** under an O(1)-extra-space constraint: removing duplicates, removing a target value, moving zeroes, partitioning around a pivot.
- You are **merging two already-sorted sequences** (arrays, or streams/logs) into one sorted result — the same-direction "advance whichever side is smaller" idea, structurally identical to the merge step of merge sort.
- You are asked to solve a problem in **O(1) extra space** and the input's order can be exploited — a strong hint the interviewer wants Two Pointers over a hash-based solution.

## When NOT To Use

- **The array is unsorted and original indices matter**, and sorting is not an option (e.g. you must return the *original* positions of the pair, as in classic Two Sum) — use hashing instead.
- **You need a contiguous subarray/substring with a running-property constraint** (longest substring without repeats, smallest subarray with sum >= target) — that is Sliding Window's shape, not Two Pointers'.
- **The relationship between the two positions is not monotonic** as pointers move — if you cannot prove "moving this pointer can only help or stay neutral," the technique's whole justification collapses, and you likely need a different approach (full scan, DP, or a different data structure).
- **You need to search for a single element/index**, not a pair — that is Binary Search's job, not Two Pointers'.
- **The data is a linked list and you need cycle detection or the middle element** — that calls for **Fast & Slow Pointers** (different rates on one structure), a related but distinct pattern (see Similar Patterns below).

## Real Interview/Production Examples

Two Pointers is one of the most frequently asked patterns at essentially every major tech company's coding interview (Google, Amazon, Meta, Microsoft, and most mid-size and startup interview loops) precisely because it tests whether a candidate can move past "brute force works, ship it" and reason about a provable elimination rule — Two Sum II, 3Sum, Container With Most Water, and Trapping Rain Water are among the most commonly cited "everyone has seen this exact question" problems in interview-prep communities.

Beyond interviews, the same-direction pointer idea is not just an interview trick — it shows up directly in real systems:

- **The merge step of merge sort** is a same-direction two-pointer walk over two sorted subarrays, always taking the smaller of the two current heads — the exact mechanical ancestor of "merge two sorted arrays" problems.
- **Diffing two sorted logs or event streams** (e.g. comparing a service's local event log against a downstream system's log to find discrepancies, or reconciling two sorted export files during a data migration) uses the same converging/same-direction walk: advance whichever stream is "behind" in sort order, and flag mismatches when timestamps/keys do not line up.
- **Database merge joins.** A merge join in a relational database's query planner (used when both inputs are already sorted on the join key, e.g. via an index scan) is a two-pointer walk over the two sorted inputs — conceptually identical to merging two sorted arrays.
- **Two-pointer partitioning is the core of quicksort's partition step** (Lomuto or Hoare partition schemes both use pointers converging or advancing at different rates to separate elements around a pivot) — an algorithmic building block, not just an interview trick.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **De-duplicating a sorted export or CSV dump in place** before writing it back out — the same-direction compaction pattern avoids allocating a second buffer for a potentially large file.
2. **Reconciling two sorted transaction logs** (e.g. your service's outbound-payment log against a payment provider's settlement log) to find entries present in one but not the other — the merge/diff idea above, run as a nightly batch job.
3. **Finding the tightest matching pair of values across two sorted price lists or inventories** (e.g. "find the pair of buy/sell prices closest to a target spread") — a direct application of the converging pair-search template.
4. **Trimming or compacting an in-memory ring buffer or sliding cache** where you need to remove stale/duplicate entries in place without allocating a new array, keeping memory footprint flat under load.
5. **Implementing a k-way (or 2-way) merge for a batch ETL pipeline** that combines multiple pre-sorted chunks (e.g. sorted Parquet row groups, or sorted shards from a distributed sort) into one output stream without loading everything into memory at once.

## Similar Patterns

- **Sliding Window** ([../sliding-window/](../sliding-window/)): also uses two indices (`left`/`right`) over an array, but they bound a **contiguous range** that grows and shrinks based on a running property (sum, distinct-count, etc.), rather than converging from opposite fixed ends toward each other. Sliding Window generalizes "variable-size contiguous subarray/substring" problems; Two Pointers generalizes "pair/triplet from possibly-distant positions in sorted data" problems. It is common to describe Sliding Window as "Two Pointers where both pointers move in the same direction and never cross backward" — related, but the recognition signal (contiguous window vs. pair search) is different enough to treat as siblings, not the same technique.
- **Fast & Slow Pointers** ([../../linked-list-patterns/fast-slow-pointers/](../../linked-list-patterns/fast-slow-pointers/)): also two pointers over one structure, but on a **linked list**, moving at **different speeds** (typically one step vs. two steps per iteration) rather than from opposite ends of an array. Used for cycle detection, finding the middle node, and finding the k-th-from-end node. The "two indices, one structure" shape is shared with Two Pointers' same-direction variant, but the mechanism (speed difference vs. positional convergence) and the target data structure (linked list vs. array) differ.
- **Binary Search:** also exploits sorted data and a monotonic comparison, but narrows toward a **single** index/value rather than converging two independent pointers toward each other for a pair. Binary Search halves the search space each step (O(log n)); Two Pointers removes one element per step but from two ends simultaneously (O(n)) — different complexity classes for different question shapes ("find one value" vs. "find a pair/triplet").
- **Merge Sort's merge step:** structurally the same-direction two-pointer walk, applied across **two separate sorted arrays** rather than one array split into regions.

| Pattern | Structure | Pointer movement | Primary question answered |
|---|---|---|---|
| Two Pointers (converging) | One sorted array | Two pointers start at opposite ends, move inward | "Is there a pair/triplet satisfying X?" / "What's the optimum from two endpoints?" |
| Two Pointers (same-direction) | One array | Slow `write` + fast `read`, both forward | "Compact this array in place preserving order." |
| Sliding Window | One array/string | `left`/`right` bound a contiguous range that grows/shrinks | "What's the best contiguous subarray/substring under constraint X?" |
| Fast & Slow Pointers | One linked list | Two pointers move at different speeds | "Is there a cycle? Where's the middle / k-th-from-end node?" |
| Binary Search | One sorted array | Single search window halves each step | "Where is (or should be) this single value?" |

## Interview Discussion

Experienced engineers rarely spend interview time on "how do you write the loop" — that is mechanical. What they actually probe is whether you can **state the elimination argument precisely**: *why* is it safe to move this specific pointer and not the other one, in this specific comparison branch? A candidate who can answer "because moving `right` instead would only shrink the width without any chance of a taller limiting wall, since `height[left]` is unchanged and already the smaller of the two" is demonstrating real understanding, not memorized code.

Common follow-up questions:
- *"Can you solve this in O(1) space instead of using a hash set?"* — this is almost always an invitation to notice the array is sorted (or can be) and pivot to Two Pointers.
- *"What if the array were not sorted?"* — expects you to recognize that Two Pointers' argument collapses, and to name the fallback (sort first if index order does not matter, otherwise hash-based).
- *"How do you avoid duplicate triplets in 3Sum?"* — expects the specific skip-adjacent-equal-values logic on all three levels (outer `i`, inner `left`, inner `right`), not a hand-wave like "just use a set to dedupe the output," which papers over the complexity cost of building and comparing triplets after the fact.
- *"Extend this to 4Sum."* — expects recognizing the pattern generalizes by adding another outer loop around the same converging two-pointer core, with complexity climbing from O(n²) to O(n³).
- *"What is the time complexity, and can you prove the O(n) bound?"* — expects the "total pointer movement is bounded by the initial gap between them" argument, not just "I've seen this is O(n)."

Common misconceptions:
- "Two Pointers is just a faster nested loop." It is not a constant-factor speedup — it is an asymptotic complexity reduction (O(n²) to O(n)), because it *eliminates* whole regions of the search space rather than scanning them faster.
- "Two Pointers and Sliding Window are the same thing." They share a "two indices into an array" shape but answer different question types (pair/triplet from anywhere vs. best contiguous range) — conflating them leads to reaching for the wrong template under pressure.
- "You can always tell which pointer to move by just trying both and seeing what works." There is always a provable, stateable reason; "trying both" is a sign the elimination argument has not actually been understood yet.
- "Sorting an array to use Two Pointers is free." Sorting costs O(n log n) and, if you need original indices, destroys the information needed to report them — a real, not hypothetical, tradeoff to mention explicitly.

## Summary

- Two Pointers replaces nested-loop scans over pairs/triplets with a single linear pass by walking two indices whose movement is provably monotonic.
- Two flavors: **converging** (opposite ends of sorted data, moving inward) for pair/triplet search and endpoint-based optimization; **same-direction** (slow `write` + fast `read`) for in-place compaction.
- The technique's entire justification rests on the input's order — sorted for the converging variant, order-preserving for the same-direction variant — and breaks down completely without it.
- Typical complexity win: O(n²) → O(n) for pair search and compaction; O(n³) → O(n²) for triplet search (3Sum).
- Space cost is O(1) extra in both flavors — no hash structure needed, unlike the hashing alternative.
- 3Sum's duplicate-skipping is the single most commonly botched detail; it is required for output *correctness*, not just style.
- The same mechanical idea (advance whichever side is "behind") underlies the merge step of merge sort, database merge joins, and diffing two sorted logs/streams in production.
- Closely related but distinct: Sliding Window (contiguous range, not two anchored endpoints) and Fast & Slow Pointers (different speeds on a linked list, not converging ends of an array).

## Key Takeaways

1. Two Pointers turns an O(n²)/O(n³) brute-force scan into O(n)/O(n²) by eliminating provably-irrelevant positions instead of checking every combination.
2. Converging variant: `left`/`right` start at opposite ends of sorted data and move inward based on a monotonic comparison.
3. Same-direction variant: slow `write` + fast `read`, used for in-place compaction under an O(1)-space constraint.
4. Every pointer move must be justifiable in one sentence — if you cannot state why the other pointer/side is safe to leave alone, the direction is probably wrong.
5. The technique requires sorted (or order-preserving) input; on unsorted data with index requirements, use hashing instead.
6. Space cost is O(1) extra — the main structural advantage over a hash-based O(n)-time alternative.
7. 3Sum composes converging pointers with an outer loop and sorting, and requires explicit duplicate-skipping at every level for correct (not just fast) output.
8. Don't confuse this with Sliding Window (contiguous window, running property) or Fast & Slow Pointers (linked list, different speeds) — related shapes, different mechanisms and question types.
9. Real systems use the same-direction idea directly: merge sort's merge step, database merge joins, and diffing sorted logs/streams.
10. When an interviewer asks for O(1) extra space on a problem you'd otherwise hash, that is almost always a signal to check whether the data is (or can be) sorted and reach for Two Pointers.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — covers the merge step of merge sort and quicksort's partition schemes, the mechanical ancestors of the two-pointer idea.
- *Competitive Programmer's Handbook* — Antti Laaksonen — has a concise, practical treatment of two-pointer techniques as used in competitive programming.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes worked two-pointer problems (including variants of pair-sum and array partitioning) with C++-specific implementation notes.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — includes array/string problems commonly solved with the two-pointer technique, alongside general interview framing.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including multiple two-pointer style array problems, useful for seeing varied implementation styles.
- The C++ Standard Library's own `<algorithm>` header (e.g. `std::partition`, `std::unique`, `std::remove_if`) — production-grade implementations of the same-direction compaction idea used throughout this module; reading libstdc++'s or libc++'s source for these functions is a direct look at the pattern in industrial code.

**Official Documentation**
- LeetCode — Two Sum II - Input Array Is Sorted (problem 167).
- LeetCode — 3Sum (problem 15).
- LeetCode — Container With Most Water (problem 11).
- LeetCode — Remove Duplicates from Sorted Array (problem 26).
- cppreference.com — `std::unique`, `std::partition`, `std::remove` — the standard library's own same-direction two-pointer algorithms, with precise complexity and invariant documentation.

**Blog Articles**
- GeeksforGeeks — "Two Pointers Technique" — a widely used explainer covering the general technique and common problem shapes.
- NeetCode — Two Pointers pattern videos/playlist — walks through Two Sum II, 3Sum, Container With Most Water, and Trapping Rain Water with visual explanations.
- Educative.io — "Grokking the Coding Interview" Two Pointers pattern chapter — one of the most widely referenced pattern-based framings of this exact technique (the inspiration for organizing DSA study by pattern rather than by individual problem).
