# Cyclic Sort

## Intent

Place every value that belongs to a known, bounded index range directly at its "home" index in a single in-place pass, so that any slot whose value does not match its expected value exposes a missing, duplicate, or misplaced number — in O(n) time and O(1) extra space.

## Real Life Analogy

Think of a small hotel's key rack with exactly `n` numbered pigeonholes, one per room, numbered 1 through `n`. A porter collects a pile of `n` room keys that fell off the rack and now sit in random order — in the simple case, exactly one key per room. Instead of sorting the whole pile like a librarian alphabetizing books, or writing down every key seen so far in a notebook (the mental model of a hash set), the porter works through the pile once, key by key: pick up the key currently in hand, look at its room number, and if it is not already in its own pigeonhole, swap it directly into that hole — not "note it and set it aside," an actual swap. Whatever key was resting in that target hole comes back into the porter's hand, and the porter repeats the exact same check on it immediately. Only when the key currently in hand is already sitting in its own hole does the porter move on to the next position in the pile. After one pass over the rack, every key that has a real home is in it — and if a hole ends up holding the wrong key, or two identical keys turn up for the same room, that is precisely the missing or duplicate key revealing itself, without the porter ever writing anything down.

A second everyday version: a teacher hands back graded exams to `n` students seated at desks numbered 1 through `n`, but the stack got shuffled before handing out. Instead of calling names one at a time and waiting for a hand to go up (a linear search per exam), the teacher walks the room once: the exam currently in hand belongs to desk 7, so the teacher walks to desk 7. If the exam already sitting at desk 7 belongs to some *other* desk, the teacher swaps it out, takes that exam, and repeats — walking to whatever desk it belongs to — until an exam that is genuinely at its own desk ends the chain. Each swap in that chain permanently seats at least one exam at its correct desk, which is exactly why the chain cannot run forever.

Cyclic Sort is that same porter/teacher move in code: one cursor walking the array, and every "this value is not home yet" observation triggers a swap that sends it there, instead of a broader sort or a side notebook (hash set) to keep track of what has been seen.

## Problem

### What engineering problem exists?

A recurring shape in array problems (and in real data-integrity checks) is: an array of size `n` holds integers drawn from a **bounded range tightly coupled to the indices themselves** — most commonly `[1..n]` or `[0..n-1]` — and the question is some variant of "what value is missing?", "what value appears twice?", or "what is the smallest positive integer that is missing?" This shape shows up whenever you are verifying that a supposedly complete, supposedly-unique sequence of IDs actually is complete and unique: a batch of ticket numbers that should span `1..n` exactly once, a shard-assignment array that should cover every shard ID exactly once, or a randomized bucket-assignment array used in an A/B test that is supposed to be a clean permutation of `0..n-1`.

The brute-force ways to answer "what's missing/duplicated" are:
- **Sort the array, then scan for a gap or a repeat** — O(n log n) time, and it usually can be done in O(1) extra space (in-place sort), but it pays a full comparison-based sort for information you already have for free (see below).
- **Use a hash set (or a frequency array) to record which values have been seen**, then scan for anything unseen or seen twice — O(n) time, but O(n) extra space for the hash structure.

> **Term: Bounded range.** A constraint stating the array's values are guaranteed to fall within some fixed span directly tied to the array's own size — e.g. "every value is between 1 and `n`" where `n` is the array's length. This is what makes Cyclic Sort possible: without it, there is no deterministic "home index" for a value.

### Why is this problem difficult?

- **The first instinct throws away free information.** A hash set treats every value as an opaque thing to remember — but when the values are known to be (mostly) a permutation of the indices, the array itself already *is* a perfect hash table: value `v` has one and only one legitimate home, index `v - 1` (or `v`, for 0-indexed ranges). Recognizing that the array can double as its own lookup structure, instead of allocating a second one, is the non-obvious leap.
- **Sorting rediscovers something you already know.** A comparison sort spends `O(log n)` work per element deciding where it belongs relative to its neighbors. But for a bounded-range array, you already know exactly which index a value belongs at *before* comparing it to anything — paying for comparisons to re-derive already-known information is wasted work.
- **Getting the swap-and-recheck logic right, and stopping correctly on duplicates, is fiddly the first time.** After swapping a value into its home, the value that comes back needs to be re-examined *at the same cursor position*, not skipped — and if two identical values both want the same home, blindly repeating the swap forever produces an infinite loop. Handling that correctly is the crux of implementing this pattern without bugs.

### What happens if we ignore it?

- **Unnecessary O(n log n) time when O(n) is achievable.** On a batch validation job processing millions of IDs (an overnight reconciliation of ticket numbers, or a nightly integrity check over shard assignments), the gap between O(n) and O(n log n) is the difference between a job that finishes in seconds and one that takes noticeably longer for no algorithmic reason.
- **Unnecessary O(n) extra memory.** A hash set sized to `n` costs real memory on every run; in a memory-constrained environment (a Lambda function with a tight memory ceiling, or a service validating very large batches concurrently), that avoidable allocation adds up, and can be the difference between fitting comfortably and hitting a memory limit under load.
- **Missing the more general family of problems.** Once you only know "sort it" or "hash it," you will not spot problems like *First Missing Positive*, where the input isn't even guaranteed to be a clean permutation, as belonging to the same family — and you'll reach for a slower, more complex solution than the elegant in-place one that generalizes cleanly.

## Why Not Other Approaches?

**"Sort the array, then scan for the first gap or repeated value."**
Correct, and O(1) extra space if done via an in-place sort — but O(n log n) time. The reason this is wasteful specifically here (not in general — see the Two Pointers module for when sorting is the *right* first step) is that a comparison sort spends effort discovering ordering relationships you already know deterministically: value `v` belongs at index `v - 1`, full stop, no comparisons needed. Paying a log factor to re-derive a fact you already have is the exact waste Cyclic Sort eliminates.

**"Use a hash set (or a `bool`/count array sized `n`) to track which values have been seen."**
O(n) time, matching Cyclic Sort's time bound — but O(n) *extra* space for the hash structure or frequency array. This is the classic "can you do it in O(1) extra space?" interview follow-up, and it exists for a real reason: the array itself can serve as its own O(1)-space hash table, because the bounded-range guarantee means every value already encodes exactly where it "should" be counted. Paying for a second structure to store information the first structure can already hold is strictly wasteful once that guarantee holds.

**"Use the sum/XOR trick (e.g. `n*(n+1)/2 - sum(nums)` for a single missing number)."**
This is a genuinely good O(n) time, O(1) space, **non-mutating** solution — but only for the single narrowest case: exactly one missing value in an otherwise-perfect `[1..n]` (or `[0..n-1]`) range, no duplicates, no out-of-range noise. It does not generalize: it silently gives a wrong (or meaningless) answer the moment duplicates are possible (*Find the Duplicate Number*), and it has no natural extension to "find the smallest missing positive" when the array can contain arbitrary negative numbers or values far outside `[1..n]` (*First Missing Positive*). Cyclic Sort is the general mechanism that all of those variants share; the sum/XOR trick is a narrow special case worth knowing but not worth generalizing from.

**Tradeoff summary:** sorting pays an unneeded `log n` factor for information already implied by the bounded-range guarantee; hashing pays O(n) space for information the array can hold about itself; the sum/XOR trick achieves O(1) space but only for the single narrowest sub-problem and does not extend to duplicates or unbounded noise. Cyclic Sort is the one approach that gets O(n) time **and** O(1) extra space **and** generalizes across the whole missing/duplicate/first-missing-positive family — precisely because it is the only technique that treats the array's own indices as the hash table, rather than building a separate one.

## Solution

The core idea: under a `[1..n]` range convention, every value `v` has exactly one legitimate home — index `v - 1`. (Under a `[0..n-1]` convention, value `v`'s home is simply index `v`.) The algorithm walks a single cursor `i` from `0` to `n - 1`. At each position, it asks one question: *does the value currently at `i` belong here?* — i.e., is `nums[i]` already equal to the value this index should hold?

- If yes, the cursor advances to `i + 1`. Nothing more to do at this position.
- If no — and the value is within the valid range, and its home slot does not already hold an identical value — swap `nums[i]` with whatever sits at its home index. This sends the current value where it belongs, and brings back whatever value was previously occupying that home. Critically, the cursor **does not advance** after a swap: it re-examines the new value that just landed at `i`, because that value might *also* need to move.
- The inner "keep swapping" step stops — and only then does the cursor advance — once the value at `i` is either correctly placed, out of the valid range entirely (it can never have a home in this array, so it is left where it is), or a duplicate of the value already sitting at its home (swapping would be pointless: the home slot already holds an identical value, so nothing changes and the loop must give up and move on instead of swapping forever).

Why this terminates in O(n) total work: every swap either (a) places some value into its correct final home for good — and a value, once correctly placed, is never touched again — or (b) is refused because that would be an infinite no-op (a duplicate whose home is already taken by an identical value). Since there are only `n` slots and case (a) can happen at most `n` times across the entire run, the total number of swaps — not just cursor advances — is bounded by `n`.

Once the single pass finishes, a **second, separate O(n) scan** finds every index `i` where `nums[i] != i + 1` (or `!= i`, for 0-indexed ranges). Each such mismatch is exactly where a missing or duplicate value reveals itself: the value that *should* be there is missing from the whole array, and the value that *is* there instead is a duplicate occupying two slots.

## Architecture

The participants in a Cyclic Sort pass:

1. **The cursor `i`.** A single index walking forward through the array. Its invariant: everything at index `< i` is either already in its correct home, or is confirmed to be a value (out-of-range or an unresolvable duplicate) that can never be placed — either way, positions before `i` are settled and will not be revisited.

2. **The candidate value `nums[i]`.** Whatever value currently occupies the cursor's position. It is re-examined after every swap, because a swap can bring a *new* candidate into position `i` that itself needs to move.

3. **The home-index mapping.** The deterministic function from value to index (`v - 1` for `[1..n]`, `v` for `[0..n-1]`) that makes this whole technique possible. Without a bounded, known range, there is no such mapping and Cyclic Sort cannot be applied at all.

4. **The swap operation.** The only mechanism that moves data. Each swap either makes final, irreversible progress (placing one value correctly) or is deliberately skipped to avoid an infinite loop (the duplicate/out-of-range case).

5. **The termination condition (per cursor position).** The inner loop at position `i` stops precisely when `nums[i]` is already correctly placed, is out of range, or is a duplicate of the value already occupying its home. Getting this condition exactly right is what separates a correct implementation from an infinite loop.

6. **The verification pass.** A separate, subsequent O(n) scan (not part of the sorting pass itself) that reads off the answer: any index where the value does not match what that index should hold identifies the missing/duplicate value for that problem.

Responsibilities in one line each:
- **Cursor:** advances only once its current position is settled — placed correctly, or provably unplaceable.
- **Candidate value:** re-checked after every swap, never assumed settled just because a swap happened.
- **Home-index mapping:** the fact that makes the array double as its own hash table.
- **Swap:** the sole progress-making move; refused exactly when it would loop forever.
- **Verification pass:** turns the sorted-as-far-as-possible array into the actual answer.

## Execution Flow

1. Initialize cursor `i = 0`.
2. While `i < n`:
   a. Compute `correct_index`, the home index for the value currently at `nums[i]` (`nums[i] - 1` for a `[1..n]` range).
   b. If `nums[i]` is outside the valid range for this problem, leave it in place and advance `i` — it can never be at home in this array.
   c. Otherwise, if `nums[i]` already equals `nums[correct_index]` (this includes the case where `i == correct_index`, i.e. the value is already home), the position is settled: advance `i`.
   d. Otherwise, swap `nums[i]` and `nums[correct_index]`. Do **not** advance `i` — the value that just arrived at `i` from the swap must be checked from step (a) again.
3. When the loop ends (`i == n`), every index either holds its correct value or holds a value that can never be placed (a duplicate or an out-of-range value) — the array is now "cyclically sorted" as far as it is possible to be.
4. Run a second pass: for each index `j` from `0` to `n - 1`, check whether `nums[j]` equals the value that index should hold. Any mismatch identifies a missing value (the expected value for that slot, which is absent from the whole array) and/or a duplicate value (whatever wrong value is actually sitting there, which must be occupying some other index too).
5. Report the answer required by the specific problem (the missing number, the list of missing numbers, the duplicate, or the first missing positive) using the mismatches found in step 4.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the flowchart deciding between Cyclic Sort, hashing, and sorting based on the signals in a problem statement (bounded value range tied to array size? need the missing/duplicate value specifically? O(1) extra space required?).

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the swap-until-in-place loop, including the two distinct exit conditions (correctly placed vs. unplaceable) that both advance the cursor.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of a concrete small array being cyclically sorted, swap by swap, followed by the verification scan that reads off the answer.

## Implementation

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the shape of the pattern clearly before looking at the worked, problem-specific solutions in [problems/](problems/).

It provides two small, reusable functions:

- `cyclic_sort` — performs the in-place swap-until-settled pass over a `std::vector<int>` under the `[1..n]` convention, leaving out-of-range values untouched and giving up gracefully on duplicates rather than looping forever.
- `find_first_misplaced` — the verification-pass helper: scans the (now cyclically-sorted) array and returns the first index `i` where `nums[i] != i + 1`, wrapped in `std::optional<size_t>` (empty if every slot is already correct).

Plain `int`/`std::vector<int>` signatures are used deliberately here (unlike the template-heavy style used for Two Pointers) because Cyclic Sort's entire mechanism is inseparable from integer index arithmetic — there is no meaningful generic version over arbitrary element types, since the value-to-index mapping only makes sense for integers in a bounded range.

## Code Walkthrough

**`cyclic_sort`** (in [code.cpp](code.cpp)). Takes a mutable `std::vector<int>&` assumed to hold values that are *supposed* to be (mostly) a permutation of `1..n`, where `n` is the vector's size. Maintains cursor `i` starting at 0. On each iteration, computes `correct_index = nums[i] - 1` and checks three things in order: is `nums[i]` within `[1, n]` at all; if so, does `nums[correct_index]` already equal `nums[i]`. If the value is out of range, or already matches what its home slot holds (which includes the case where the value is already sitting at its own home), the cursor advances without swapping. Otherwise it swaps `nums[i]` and `nums[correct_index]` and deliberately does *not* advance `i`, letting the next loop iteration re-examine whatever just arrived. This function exists to isolate the sorting pass itself, independent of any specific problem's follow-up question.

**`find_first_misplaced`** (in [code.cpp](code.cpp)). A plain linear scan over an already-cyclically-sorted array, returning the first index where `nums[i] != i + 1`. This function exists to demonstrate the verification pass as a separate, reusable step — every worked problem in [problems/](problems/) builds its specific answer (missing number, list of missing numbers, the duplicate, first missing positive) on top of this same "find where reality disagrees with the expected value" scan, just interpreting the mismatch differently per problem.

**`main()`** (in [code.cpp](code.cpp)). Exercises `cyclic_sort` and `find_first_misplaced` against three hand-checkable cases — a complete permutation with no mismatches, an array containing a duplicate (which also implies a missing value), and an array containing out-of-range noise (values that must be left untouched) — printing `[PASS]`/`[FAIL]` for each assertion.

**Files in [problems/](problems/).** Each file is a complete, standalone solution to one specific, named LeetCode problem, implemented inline (not calling into `code.cpp`) so every file stays dependency-free and independently readable, with comments tying every decision back to the general principles established above. See [problems/README.md](problems/README.md) for the index. Briefly: `01` is the pure "one missing value from a complete range" case (with a same-complexity XOR alternative mentioned in comments); `02` extends the same idea to *multiple* missing values; `03` is the case where Cyclic Sort finds a duplicate specifically via the in-place placement technique (contrasted explicitly with the Floyd's Cycle Detection approach used elsewhere in this repo for the exact same problem); `04` is the hardest variant, where the input is not even guaranteed to be a clean permutation and out-of-range noise must be handled explicitly.

## Advantages

- **O(n) time, O(1) extra space.** No hash set, no frequency array, no second buffer — just the array itself and a couple of index variables. This matters directly in memory-constrained batch jobs and high-throughput services processing large ID arrays.
- **In-place mutation with no allocation.** Unlike hashing, there is no allocation proportional to `n`, so there is no GC/heap pressure from a temporary structure sized to the input.
- **Generalizes across a whole problem family.** The exact same swap-until-settled mechanism, followed by a verification scan, answers "what's missing," "what's duplicated," "what are *all* the missing values," and (with a small extension) "what's the smallest missing positive" — one mental model, many problems.
- **Provable termination, not a heuristic.** Every swap either places a value permanently or is refused because it would be a no-op; this gives a clean, provable O(n) total-swap bound rather than an empirical "seems fast" argument.
- **Exposes data-integrity violations directly.** Because the verification pass reads off mismatches positionally, it naturally reports *which* values are missing/duplicated, not just *whether* a violation exists — useful for the real-world integrity-check use case, not just the interview version of the problem.

## Disadvantages

- **Mutates the input array.** The technique only works by rearranging elements in place; if the caller needs the original order preserved (e.g. the array represents an ordered log, or is shared/read elsewhere), you must work on a copy, which reintroduces O(n) space.
- **Only applies when values map to a bounded, known index range.** If the values are not (mostly) confined to `[1..n]`/`[0..n-1]`, there is no home-index mapping and the whole technique is inapplicable — you fall back to hashing or sorting.
- **Not thread-safe / not safe for concurrent reads during the pass.** Because the array is being actively rearranged element by element, any concurrent reader sees a partially-sorted, temporarily-inconsistent view — a non-issue for a single-threaded batch check, but a real constraint if the array is shared live state.
- **The inner "keep swapping without advancing" step is easy to implement incorrectly.** Forgetting to skip advancing `i` after a swap, or getting the duplicate-detection check backward, produces either wrong answers or an infinite loop — a strictly less forgiving failure mode than a hash-based approach, which simply cannot loop forever by construction.

## Tradeoffs

**What we gain versus sorting:** the same O(n) *(vs O(n log n))* — an asymptotic win — by using the bounded-range guarantee to know each value's destination index for free, with no comparisons needed.

**What we gain versus hashing:** the same O(n) time, but O(1) extra space instead of O(n), because the array becomes its own hash table once the bounded-range guarantee holds.

**What we lose versus hashing:** hashing works on **any** values, any range, with **no mutation** of the input — Cyclic Sort requires both a bounded, known range *and* that mutating the array in place is acceptable.

**What we lose versus the sum/XOR trick (for the single-missing-number sub-case):** the sum/XOR trick achieves the same O(n)/O(1) bound *without* mutating the array at all — but only for that one narrow sub-case. Cyclic Sort trades away that narrow non-mutating advantage for a mechanism that generalizes to duplicates and to the first-missing-positive family.

## Complexity

**Time:** **O(n)** for the sorting pass itself, plus **O(n)** for the verification scan — **O(n)** total. The sorting pass's bound rests on the fact that each swap either places a value in its final home for good (at most `n` such swaps total, across the entire run, not per cursor position) or is refused outright as a no-op; the cursor itself also advances at most `n` times. This holds in the best, worst, and average case alike, because the bound is structural (total swaps + total advances), not data-dependent.

**Space:** **O(1)** extra — the array is sorted in place using a small, fixed number of index/temporary variables, independent of `n`.

**Comparison to the brute force each replaces:**

| Approach | Time | Extra Space | Mutates input? |
|---|---|---|---|
| Sort, then scan for gap/repeat | O(n log n) | O(1) (in-place sort) or O(n) (copy first) | Yes (or no, if sorting a copy) |
| Hash set / frequency array | O(n) | O(n) | No |
| Sum/XOR trick (single missing value only) | O(n) | O(1) | No |
| **Cyclic Sort** | **O(n)** | **O(1)** | **Yes** |

## Common Mistakes

- **Advancing the cursor after a swap.** The single most common bug: after swapping `nums[i]` with `nums[correct_index]`, a new value has landed at `i` that has not been checked yet. Advancing `i` anyway skips that check and can leave the array incorrectly sorted. *Avoid:* only advance `i` inside the branch where no swap happened this iteration.
- **Getting the wrong termination condition and looping forever on duplicates.** If two copies of the same value both want the same home index, repeatedly swapping them achieves nothing and never terminates unless the loop explicitly checks "does the home slot already hold this exact value?" before swapping. *Avoid:* always compare `nums[i]` against `nums[correct_index]` (not just check whether `i == correct_index`) before deciding to swap.
- **Off-by-one between value and index.** Mixing up the 1-indexed convention (`home = value - 1`) with a 0-indexed convention (`home = value`) — or applying the wrong one for the specific problem's stated range — produces an off-by-one that either corrupts unrelated slots or throws an out-of-bounds access. *Avoid:* write down, before coding, exactly which convention this problem uses, and keep the `value <-> index` formula consistent throughout.
- **Forgetting that input values can be duplicated or entirely out of range.** A cyclic sort written only for a clean permutation will crash (index out of bounds) or misbehave the moment a value like `0`, a negative number, or a value greater than `n` shows up — which is exactly the input shape of *First Missing Positive*. *Avoid:* always bounds-check `nums[i]` against `[1, n]` (or `[0, n-1]`) before computing `correct_index` and attempting a swap.
- **Confusing the sorting pass with the answer.** Cyclic Sort by itself does not produce "the missing number" — it produces an array that is *as sorted as it can be*. The actual answer only appears after the separate verification scan. *Avoid:* keep the two passes conceptually (and, ideally, in code) distinct.
- **Assuming the technique still applies once the range is unbounded.** If nothing in the problem constrains values to a range tied to `n`, there is no home-index mapping, and Cyclic Sort simply does not apply — reach for hashing or sorting instead.

## When To Use

- The array's values are guaranteed (or can be filtered/clamped) to lie within `[1..n]` or `[0..n-1]`, where `n` is the array's own length.
- You need to find a **single missing value**, **multiple missing values**, a **duplicate**, or the **smallest missing positive integer**, and mutating the input array is acceptable.
- An interviewer or a resource-constrained system explicitly asks for **O(1) extra space** on a problem that would otherwise be solved with a hash set or frequency array.
- You are validating **data integrity of an ID/index sequence** — e.g. confirming a batch of ticket numbers, shard assignments, or a permutation-based bucket assignment genuinely covers its expected range exactly once.

## When NOT To Use

- **The values are not confined to a range tied to the array's size.** Arbitrary integers (e.g. large IDs, hashes, or floating-point values) have no meaningful home-index mapping — use hashing instead.
- **The input must not be mutated.** If the array is shared, read concurrently, or must retain its original order for other consumers, either work on a copy (losing the O(1)-space advantage) or use hashing.
- **You only need to detect "is there a duplicate/gap at all," not identify which value.** A simpler check (e.g. comparing `sum` or `count` against an expected closed-form value) can sometimes answer a yes/no question more simply than a full sort-and-scan, though it is far less general.
- **You need the actual sorted order of arbitrary (non-bounded-range) data.** That is a general-purpose sorting problem — reach for `std::sort` or a comparison-based algorithm, not Cyclic Sort.

## Real Interview/Production Examples

Cyclic Sort variants (Missing Number, Find All Numbers Disappeared in an Array, Find the Duplicate Number, First Missing Positive) are frequently asked across major tech company interview loops precisely because they test whether a candidate notices the bounded-range signal and reaches for an O(1)-space in-place technique instead of defaulting to a hash set — *First Missing Positive* in particular is one of the most commonly cited "hard" array problems for exactly this reason (LeetCode explicitly tags it as a problem most engineers first solve with O(n) space, then are asked to redo in O(1)).

Beyond interviews, the same "use the array as its own lookup structure" idea shows up in real systems:

- **Validating a batch of assigned IDs or shard numbers** — confirming a nightly-generated array of shard assignments (which should be a permutation of `0..n-1` across `n` shards) has no gaps or collisions before it is used to route traffic, without allocating an auxiliary set proportional to shard count.
- **In-place bucket/permutation verification for A/B testing infrastructure** — confirming a randomized bucket-assignment array genuinely covers every bucket exactly once before it is used to split live traffic, catching a bug in the randomization step before it causes uneven traffic splits.
- **Compacting/repairing sparse ID arrays during data migration** — when migrating records that are supposed to have contiguous IDs from `1..n`, a cyclic-sort-style pass can identify exactly which IDs are missing (indicating deleted/corrupted rows) without a second data structure.
- **Counting/frequency-array-free duplicate detection in constrained environments** — embedded systems or very tight-memory serverless functions validating a bounded-range array without room to spare for a hash set.

## Where I Can Use This

Five realistic ideas for your own backend/systems work:

1. **A startup-time sanity check for a sharding table.** Before a service starts routing requests, verify its shard-assignment array is a genuine permutation of `0..n-1` using an in-place cyclic-sort pass, failing fast with the exact missing/duplicate shard ID if it is not.
2. **Validating uploaded CSV/batch files of sequential IDs.** When ingesting a nightly batch file that claims to contain ticket/order numbers `1..n` exactly once, run a cyclic-sort-style check in O(1) extra memory instead of loading a hash set sized to a potentially very large `n`.
3. **Auditing a randomized experiment's bucket assignment.** Confirm an A/B test's user-to-bucket array is a clean permutation before it goes live, catching randomization bugs (a bucket appearing twice, or one never appearing) with no auxiliary memory.
4. **Detecting corrupted/duplicated primary keys after a data migration.** After migrating rows that should have contiguous integer IDs, run the verification-scan half of this pattern over the extracted ID array to report exactly which IDs are missing or duplicated.
5. **A memory-constrained microservice endpoint that validates array completeness.** In a low-memory serverless function processing a bounded-range array (e.g. verifying all page numbers of a paginated export were received), use Cyclic Sort to avoid allocating a frequency array proportional to the (potentially large) page count.

## Similar Patterns

- **Fast & Slow Pointers** ([../../linked-list-patterns/fast-slow-pointers/](../../linked-list-patterns/fast-slow-pointers/)): also finds a duplicate value in an array in `[1..n]` context (*Find the Duplicate Number* is solvable both ways), but by a completely different mechanism — it treats the array as an implicit linked list (`nums[i]` is a "pointer" to the next index) and uses Floyd's Cycle Detection (slow/fast pointers at different speeds) to find the cycle's entry point, which is provably the duplicate value. That approach is **non-mutating** (a genuine advantage over Cyclic Sort for this specific problem) but is a distinct algorithm — cycle detection on an implicit graph, not in-place value placement — and does not generalize to "find all missing values" or "first missing positive" the way Cyclic Sort does. See [problems/03-find-the-duplicate-number.cpp](problems/03-find-the-duplicate-number.cpp) for the Cyclic Sort solution to this exact problem, contrasted explicitly against the fast/slow-pointer approach.
- **Plain hashing (hash set / frequency array):** answers the exact same family of questions (missing/duplicate/first-missing-positive) in the same O(n) time, but O(n) extra space, and — unlike Cyclic Sort — works on **any** values in **any** range and never mutates the input. Hashing is strictly more general; Cyclic Sort is strictly more space-efficient when its narrower bounded-range precondition holds.
- **Two Pointers** ([../two-pointers/](../two-pointers/)): shares the "single pass, O(1) extra space, in-place" flavor, but solves a different question shape (pairs/triplets summing to a target, or in-place compaction under a keep/discard predicate) and requires *sorted* input rather than a bounded value-to-index range. The two patterns are siblings in spirit (both replace hashing with a structural trick) but recognize different signals.

| Pattern | Precondition | Mechanism | Mutates input? | Extra space | Generalizes to |
|---|---|---|---|---|---|
| **Cyclic Sort** | Values bounded to `[1..n]`/`[0..n-1]` | In-place swap to home index | Yes | O(1) | Missing / all-missing / duplicate / first-missing-positive |
| Fast & Slow Pointers (Floyd's) | Values in `[1..n]`, treated as implicit links | Cycle detection at different speeds | No | O(1) | Finding *one* duplicate only |
| Hashing (hash set / frequency array) | None — any values, any range | Record seen values, check membership | No | O(n) | Same family, plus unbounded ranges |
| Sorting | None — any comparable values | Full comparison-based sort, then scan | Depends (in-place or copy) | O(1) or O(n) | Same family, plus general ordering needs |

## Interview Discussion

Experienced engineers do not spend interview time on "how do you write the swap" — that is mechanical once you have seen it once. What they actually probe is whether you **recognize the bounded-range signal** as soon as you see it (values confined to `[1..n]`/`[0..n-1]`), and whether you can articulate *why* that specific fact lets you avoid the extra O(n) space a hash set would cost — i.e., that the array can serve as its own hash table because every value already encodes its own destination index.

Common follow-up questions:
- *"Can you do this without extra space?"* — the canonical invitation to move from a hash-set solution to Cyclic Sort; expects you to name the bounded-range precondition explicitly, not just "swap things around."
- *"What if there could be more than one missing number?"* — expects recognizing that the same swap-then-verify mechanism generalizes directly: the verification pass simply reports *every* mismatched index instead of stopping at the first one (see [problems/02-find-all-numbers-disappeared-in-an-array.cpp](problems/02-find-all-numbers-disappeared-in-an-array.cpp)).
- *"Can you solve Find the Duplicate Number without modifying the array?"* — expects naming Floyd's Cycle Detection (Fast & Slow Pointers) as the non-mutating alternative, and being able to explain *why* it works (treating `nums[i]` as a pointer to index `nums[i]`, which forms a cycle whose entry is the duplicate) — a strong signal of genuinely understanding both techniques rather than having memorized one.
- *"What breaks if the array can contain zero or negative numbers?"* — the *First Missing Positive* twist; expects explicit bounds-checking before computing a home index, and correctly leaving out-of-range values untouched rather than crashing or corrupting other slots.
- *"What is the time complexity, and can you prove the O(n) bound given the nested while loop?"* — expects the "total swaps across the *entire* run, not per cursor position, is bounded by n because each swap permanently places a value" argument — not a hand-wave like "it looks linear."

Common misconceptions:
- "The nested `while` loop inside the `for`/cursor loop makes this O(n²)." It does not — the inner loop's total iterations across the *whole* run are bounded by `n` (each swap is final), so the amortized bound is O(n), not per-position O(n) work repeated `n` times.
- "Cyclic Sort actually produces a fully sorted array." It produces an array that is *as sorted as possible given duplicates/out-of-range values* — positions holding unplaceable values are left as-is, which is fine because the technique only cares about the verification pass afterward, not about producing a textbook-sorted array.
- "This is the same as counting sort." They are related in spirit (both exploit a bounded value range) but counting sort builds a separate count array indexed by value (O(n) or O(k) extra space); Cyclic Sort rearranges the *original* array in place with no auxiliary structure at all.
- "You need to fully sort the array before you can find the answer." The swap pass and the verification scan are conceptually two separate O(n) steps; forgetting the second step and trying to read the answer off mid-sort is a common source of confusion when first learning this pattern.

## Summary

- Cyclic Sort places each value at its home index (`value - 1` for `[1..n]`, `value` for `[0..n-1]`) via in-place swaps, using the array itself as an implicit hash table.
- It requires — and only applies when — array values are known to be bounded to a range tied to the array's own size.
- The algorithm is two conceptually separate passes: the swap-until-settled sorting pass, then a verification scan that reads off mismatches as the actual answer.
- Correctness rests on refusing to swap when the home slot already holds an identical value (the duplicate case) — this is what prevents an infinite loop.
- Complexity: O(n) time (total swaps across the whole run are bounded by `n`, not per cursor position), O(1) extra space — beating sorting's O(n log n) and hashing's O(n) space.
- It generalizes across a family of LeetCode problems: Missing Number, Find All Numbers Disappeared, Find the Duplicate Number, and First Missing Positive — one mechanism, several verification interpretations.
- Its closest relative for duplicate-finding specifically is Fast & Slow Pointers (Floyd's Cycle Detection), which is non-mutating but does not generalize to the missing-value variants.
- The technique mutates its input, which is its central limitation; a copy is needed if the original order/content must be preserved elsewhere.

## Key Takeaways

1. Cyclic Sort works only when values are bounded to `[1..n]`/`[0..n-1]` — that bound is what lets the array serve as its own hash table.
2. The mechanism: walk a cursor, and at each position swap the current value to its home index until the position holds a correctly-placed value or an unplaceable one (duplicate/out-of-range).
3. Never advance the cursor immediately after a swap — the value that just arrived must be re-examined first.
4. Refuse to swap when the home slot already holds an identical value; this is exactly what prevents an infinite loop on duplicates.
5. The sorting pass and the verification pass (finding mismatches `nums[i] != i + 1`) are conceptually two separate O(n) steps — the answer only exists after the second one.
6. Total complexity: O(n) time, O(1) extra space — because total swaps across the entire run (not per position) are bounded by `n`.
7. This beats sorting's O(n log n) (an unneeded comparison cost) and hashing's O(n) space (an unneeded second structure) precisely because the bounded range makes both unnecessary.
8. It generalizes to a whole problem family: one missing value, multiple missing values, a duplicate, and (with explicit bounds-checking) the smallest missing positive integer even with unbounded/negative noise in the input.
9. For finding one duplicate specifically, Fast & Slow Pointers (Floyd's Cycle Detection) is a non-mutating alternative worth knowing — but it does not generalize the way Cyclic Sort does.
10. The technique mutates the input array — if that is unacceptable, fall back to hashing (O(n) space) or work on a copy.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — covers counting sort and the general idea of using array position as an implicit key, the conceptual ancestor of using an array as its own hash table.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — includes worked array problems exploiting bounded value ranges and in-place techniques, with C++-specific implementation notes.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — covers array/hashing tradeoffs and in-place array manipulation techniques relevant to recognizing when Cyclic Sort applies.
- *Grokking the Coding Interview* (Educative.io) — the pattern-based course that popularized naming and teaching "Cyclic Sort" explicitly as one of its named patterns.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — a community-maintained collection of classic algorithms in C++, including bounded-range and in-place array techniques useful for seeing varied implementation styles.
- The C++ Standard Library's own `<algorithm>` header (`std::swap`, `std::iter_swap`) — the primitive operation this entire pattern is built from; reading how these are specified in the standard is a direct look at the building block in industrial code.

**Official Documentation**
- LeetCode — Missing Number (problem 268).
- LeetCode — Find All Numbers Disappeared in an Array (problem 448).
- LeetCode — Find the Duplicate Number (problem 287).
- LeetCode — First Missing Positive (problem 41).
- cppreference.com — `std::swap` — precise semantics and complexity guarantees for the primitive operation Cyclic Sort relies on.

**Blog Articles**
- GeeksforGeeks — "Cyclic Sort" — a widely used explainer covering the technique and its common problem shapes.
- NeetCode — array/hashing pattern videos covering Missing Number, First Missing Positive, and Find the Duplicate Number, including the Floyd's Cycle Detection alternative for the latter.
- Educative.io — "Grokking the Coding Interview" Cyclic Sort pattern chapter — the most widely referenced pattern-based framing of this exact technique.
