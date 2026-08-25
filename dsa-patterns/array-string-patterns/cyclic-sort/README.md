# Cyclic Sort


> **In one line:** swap each value into its home index (`v - 1`) until every in-range slot holds its correct value — no comparisons, no extra array.

```cpp
void cyclic_sort(std::vector<int>& nums) {
  const int n = static_cast<int>(nums.size());
  int i = 0;

  while (i < n) {
    const int correct_index = nums[i] - 1;          // home for value v is v-1
    const bool in_range = nums[i] >= 1 && nums[i] <= n;

    if (in_range && nums[i] != nums[correct_index]) {
      std::swap(nums[i], nums[correct_index]);
      // do NOT advance i: re-check whatever just landed here
    } else {
      ++i;   // out of range, already home, or a duplicate — move on
    }
  }
}
```

**O(n)** time (each swap places at least one value home for good) · **O(1)** space. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Place every value that belongs to a known, bounded index range directly at its "home" index in a single in-place pass, so that any slot whose value does not match its expected value exposes a missing, duplicate, or misplaced number — in O(n) time and O(1) extra space.

## Real Life Analogy

Think of a small hotel's key rack with exactly `n` numbered pigeonholes, numbered 1 through `n`. A porter collects a pile of `n` room keys that fell off the rack, in random order — in the simple case, exactly one key per room. Instead of sorting the whole pile like a librarian, or writing down every key seen in a notebook (the mental model of a hash set), the porter works through the pile once: pick up the key in hand, and if it is not already in its own pigeonhole, swap it directly into that hole. Whatever key was resting there comes back into the porter's hand, and gets the same check immediately. Only when the key in hand is already in its own hole does the porter move to the next position. After one pass, every key with a real home is in it — and a hole holding the wrong key, or two identical keys for one room, is precisely the missing or duplicate key revealing itself, with nothing ever written down.

A second, equally common version: a teacher handing back graded exams to `n` students at shuffled desks numbered 1 through `n`. Instead of calling names one at a time, the teacher walks each exam directly to the desk it belongs to, swaps out whatever exam is already there, and repeats on that swapped-out exam until one is genuinely at its own desk. Each swap in the chain permanently seats at least one exam correctly, which is exactly why the chain cannot run forever.

Cyclic Sort is that porter/teacher move in code: one cursor walking the array, and every "this value is not home yet" observation triggers a swap that sends it there, instead of a broader sort or a side notebook (hash set).

## Problem

### What engineering problem exists?

A recurring shape in array problems (and in real data-integrity checks) is: an array of size `n` holds integers drawn from a **bounded range tightly coupled to the indices themselves** — most commonly `[1..n]` or `[0..n-1]` — and the question is some variant of "what value is missing?", "what value appears twice?", or "what is the smallest positive integer that is missing?" This shows up whenever verifying that a supposedly complete, unique sequence of IDs actually is: a batch of ticket numbers that should span `1..n` exactly once, a shard-assignment array that should cover every shard ID exactly once, or a randomized bucket-assignment array in an A/B test that should be a clean permutation of `0..n-1`.

The brute-force answers are: **sort, then scan for a gap or repeat** — O(n log n) time, O(1) extra space with an in-place sort, but paying a full comparison-based sort for information already known for free; or **use a hash set (or frequency array)** to record seen values, then scan for anything unseen or seen twice — O(n) time, O(n) extra space.

> **Term: Bounded range.** A constraint stating the array's values are guaranteed to fall within some fixed span directly tied to the array's own size — e.g. "every value is between 1 and `n`." This is what makes Cyclic Sort possible: without it, there is no deterministic "home index" for a value.

### Why is this problem difficult?

- **The first instinct throws away free information.** A hash set treats every value as opaque — but when values are known to be (mostly) a permutation of the indices, the array itself already *is* a perfect hash table: value `v`'s one legitimate home is index `v - 1` (or `v`, for 0-indexed ranges). Recognizing the array can double as its own lookup structure, instead of allocating a second one, is the non-obvious leap.
- **Sorting rediscovers something you already know.** A comparison sort spends `O(log n)` work per element deciding where it belongs relative to its neighbors, but for a bounded-range array you already know exactly which index a value belongs at — paying for comparisons to re-derive known information is wasted work.
- **Getting the swap-and-recheck logic right is fiddly the first time.** After swapping a value into its home, the value that comes back must be re-examined *at the same cursor position*, not skipped — and if two identical values both want the same home, blindly repeating the swap forever produces an infinite loop.

### What happens if we ignore it?

- **Unnecessary O(n log n) time when O(n) is achievable.** On a batch validation job processing millions of IDs (an overnight ticket-number reconciliation, or a nightly shard-assignment integrity check), the gap between O(n) and O(n log n) is seconds versus noticeably longer, for no algorithmic reason.
- **Unnecessary O(n) extra memory.** A hash set sized to `n` costs real memory every run; in a memory-constrained environment (a Lambda function with a tight ceiling, or a service validating large batches concurrently), that avoidable allocation can be the difference between fitting comfortably and hitting a limit under load.
- **Missing the more general family of problems.** Knowing only "sort it" or "hash it" means missing that problems like *First Missing Positive* — where the input isn't even guaranteed to be a clean permutation — belong to the same family, and reaching for something slower and more complex than the elegant in-place solution that generalizes cleanly.

## Solution

The core idea: under a `[1..n]` range convention, every value `v` has exactly one legitimate home — index `v - 1`. (Under `[0..n-1]`, value `v`'s home is index `v`.) The algorithm walks a single cursor `i` from `0` to `n - 1`, asking one question at each position: *does the value currently at `i` belong here?*

- If yes, the cursor advances. Nothing more to do at this position.
- If no — and the value is in range, and its home slot does not already hold an identical value — swap `nums[i]` with whatever sits at its home. This sends the current value where it belongs and brings back whatever occupied that home. The cursor **does not advance** after a swap: it re-examines the new value that just landed at `i`, since it might also need to move.
- The inner "keep swapping" step stops — and only then does the cursor advance — once the value at `i` is correctly placed, out of range entirely (it can never have a home here), or a duplicate of the value already at its home (swapping would be an infinite no-op).

Why this terminates in O(n) total work: every swap either (a) places some value into its correct final home for good — and a placed value is never touched again — or (b) is refused as an infinite no-op. Since there are only `n` slots and case (a) can happen at most `n` times across the entire run, total swaps are bounded by `n`.

Once the pass finishes, a **second, separate O(n) scan** finds every index `i` where `nums[i] != i + 1` (or `!= i`, 0-indexed). Each mismatch is exactly where a missing or duplicate value reveals itself: the expected value is missing from the whole array, and whatever is there instead is a duplicate occupying two slots.

In short: (1) start cursor `i = 0`; (2) while `i < n`, compute `correct_index` — if `nums[i]` is out of range or already matches `nums[correct_index]`, advance `i`; otherwise swap and re-check the same `i`; (3) run the separate verification scan over the settled array, reporting every mismatch as the answer the specific problem needs.

## Architecture

The participants in a Cyclic Sort pass:

1. **The cursor `i`.** Walks forward through the array. Invariant: everything at index `< i` is either in its correct home or confirmed unplaceable (out-of-range or an unresolvable duplicate) — settled, and never revisited.
2. **The candidate value `nums[i]`.** Whatever occupies the cursor's position; re-examined after every swap, since a swap can bring a *new* candidate into `i` that itself needs to move.
3. **The home-index mapping.** The deterministic function from value to index (`v - 1` for `[1..n]`, `v` for `[0..n-1]`) that makes the whole technique possible — without a bounded, known range there is no such mapping.
4. **The swap operation.** The only mechanism that moves data: each swap either makes final, irreversible progress or is deliberately skipped to avoid an infinite loop.
5. **The termination condition (per position).** The inner loop stops precisely when `nums[i]` is correctly placed, out of range, or a duplicate of the value already at its home — getting this exactly right separates a correct implementation from an infinite loop.
6. **The verification pass.** A separate, subsequent O(n) scan that reads off the answer: any index where the value doesn't match what it should hold identifies the missing/duplicate value.

In one line each: the cursor advances only once settled; the candidate value is never assumed settled just because a swap happened; the home-index mapping is the fact that makes the array double as its own hash table; the swap is the sole progress-making move, refused exactly when it would loop forever; and the verification pass turns the sorted-as-far-as-possible array into the actual answer.

## Why Not Other Approaches?

**"Sort, then scan for the first gap or repeated value."** Correct, O(1) extra space via an in-place sort, but O(n log n) time — wasteful specifically here (not in general — see [../two-pointers/](../two-pointers/) for when sorting *is* the right first step) because a comparison sort spends effort discovering an ordering you already know deterministically: value `v` belongs at index `v - 1`, no comparisons needed.

**"Use a hash set (or a `bool`/count array sized `n`)."** O(n) time, matching Cyclic Sort — but O(n) *extra* space. The classic "can you do it in O(1) extra space?" follow-up: the array itself can serve as its own O(1)-space hash table once the bounded-range guarantee holds, so paying for a second structure is strictly wasteful.

**"Use the sum/XOR trick (`n*(n+1)/2 - sum(nums)` for a single missing number)."** A genuinely good O(n)/O(1), **non-mutating** solution — but only for exactly one missing value, no duplicates, no out-of-range noise. It gives a wrong (or meaningless) answer the moment duplicates are possible (*Find the Duplicate Number*), and has no extension to "smallest missing positive" when the array can contain negatives or values far outside `[1..n]` (*First Missing Positive*). Cyclic Sort is the general mechanism the whole family shares; this trick is a narrow special case not worth generalizing from.

**Net:** sorting pays an unneeded `log n` factor for information the bounded-range guarantee already implies; hashing pays O(n) space for information the array can hold about itself; the sum/XOR trick gets O(1) space but only for the narrowest sub-problem. Cyclic Sort is the one approach that gets O(n) time **and** O(1) space **and** generalizes across the whole missing/duplicate/first-missing-positive family, because it alone treats the array's own indices as the hash table.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — flowchart deciding between Cyclic Sort, hashing, and sorting based on the signals in a problem statement (bounded value range tied to array size? need the missing/duplicate value specifically? O(1) extra space required?).
- [images/flow-diagram.md](images/flow-diagram.md) — control-flow diagram of the swap-until-in-place loop, including the two distinct exit conditions (correctly placed vs. unplaceable) that both advance the cursor.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of a concrete small array being cyclically sorted, swap by swap, followed by the verification scan that reads off the answer.

## The Code

[code.cpp](code.cpp) is a **generic, problem-agnostic template**, not a solution to one specific LeetCode question — the goal is to see the pattern's shape before the worked solutions in [problems/](problems/). Plain `int`/`std::vector<int>` signatures are deliberate here (unlike the template-heavy style used for Two Pointers): Cyclic Sort's mechanism is inseparable from integer index arithmetic, so there is no meaningful generic version over arbitrary element types.

**`cyclic_sort(nums)`.** Takes a mutable `std::vector<int>&` assumed to hold values that should be (mostly) a permutation of `1..n`. Maintains cursor `i`, computes `correct_index = nums[i] - 1`, and checks: is `nums[i]` within `[1, n]`, and does `nums[correct_index]` already equal it. If out of range or already matching (including already being home), the cursor advances without swapping; otherwise it swaps and does *not* advance `i`, letting the next iteration re-examine whatever just arrived.

**`find_first_misplaced(nums)`.** A linear scan over an already-cyclically-sorted array, returning the first index where `nums[i] != i + 1`, wrapped in `std::optional<size_t>`. It demonstrates the verification pass as a separate, reusable step — every problem in [problems/](problems/) builds its answer on this same "find where reality disagrees with expectation" scan, interpreting the mismatch differently per problem.

**`main()`.** Exercises both functions against three hand-checkable cases — a complete permutation, an array with a duplicate (implying a missing value), and an array with out-of-range noise that must be left untouched — printing `[PASS]`/`[FAIL]`.

**Files in [problems/](problems/).** Each is a complete, standalone solution to one named LeetCode problem, implemented inline (not calling into `code.cpp`) so every file stays dependency-free — see [problems/README.md](problems/README.md) for the index. Briefly: `01` is one missing value from a complete range (with a same-complexity XOR alternative in comments); `02` extends to *multiple* missing values; `03` finds a duplicate via in-place placement, contrasted explicitly with the Floyd's Cycle Detection approach used elsewhere in this repo for the same problem; `04` is the hardest variant, where the input isn't guaranteed to be a clean permutation and out-of-range noise must be handled explicitly.

## Tradeoffs

**What Cyclic Sort buys you**

- **O(n) time, O(1) extra space.** No hash set, no frequency array, no second buffer — just the array and a couple of index variables. Matters directly in memory-constrained batch jobs and high-throughput services, with no GC/heap pressure from an allocation proportional to `n`.
- **Generalizes across a whole problem family.** The same swap-until-settled mechanism, followed by a verification scan, answers "what's missing," "what's duplicated," "what are *all* the missing values," and (with a small extension) "what's the smallest missing positive."
- **Provable termination, not a heuristic.** Every swap either places a value permanently or is refused as a no-op — a clean, provable O(n) total-swap bound rather than an empirical "seems fast" argument.
- **Exposes data-integrity violations directly.** The verification pass reads off mismatches positionally, so it reports *which* values are missing/duplicated, not just whether a violation exists.

**What it costs you**

- **Mutates the input array.** If the caller needs the original order preserved, you must work on a copy, reintroducing O(n) space.
- **Only applies when values map to a bounded, known index range.** Outside `[1..n]`/`[0..n-1]`, there is no home-index mapping and the technique is inapplicable.
- **Not safe for concurrent reads during the pass.** A concurrent reader sees a partially-sorted, temporarily-inconsistent view — fine for a single-threaded batch check, a real constraint for shared live state.
- **The "keep swapping without advancing" step is easy to get wrong.** Forgetting to skip advancing `i` after a swap, or inverting the duplicate check, produces wrong answers or an infinite loop — less forgiving than hashing, which cannot loop forever by construction.
- **Against hashing:** same O(n) time, but hashing works on any values/range with no mutation — Cyclic Sort trades that generality for O(1) space.
- **Against the sum/XOR trick (single-missing-number case only):** that trick matches O(n)/O(1) *without* mutating — but only for that one narrow sub-case; Cyclic Sort trades the non-mutating advantage for generalizing to duplicates and first-missing-positive.

## Complexity

**Time:** **O(n)** for the sorting pass plus **O(n)** for the verification scan — **O(n)** total. Each swap either places a value in its final home for good (at most `n` such swaps) or is refused as a no-op; the cursor also advances at most `n` times. This holds in the best, worst, and average case alike, since the bound is structural, not data-dependent.

**Space:** **O(1)** extra — a small, fixed number of index/temporary variables, independent of `n`.

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

## Where This Shows Up

Cyclic Sort variants (Missing Number, Find All Numbers Disappeared in an Array, Find the Duplicate Number, First Missing Positive) are frequently asked across major tech company interview loops precisely because they test whether a candidate notices the bounded-range signal and reaches for an O(1)-space in-place technique instead of defaulting to a hash set — *First Missing Positive* is one of the most commonly cited "hard" array problems for exactly this reason (LeetCode tags it as one most engineers first solve with O(n) space, then are asked to redo in O(1)).

Beyond interviews, the same "use the array as its own lookup structure" idea shows up directly in real systems, and translates into concrete backend/systems ideas of your own:

- **Validating a batch of assigned IDs or shard numbers** — a startup-time sanity check confirming a shard-assignment array (a permutation of `0..n-1` across `n` shards) has no gaps or collisions before routing traffic, failing fast with the exact missing/duplicate shard ID, without an auxiliary set proportional to shard count.
- **In-place bucket/permutation verification for A/B testing infrastructure** — auditing a randomized bucket-assignment array before it goes live, catching a randomization bug before it causes uneven traffic splits.
- **Compacting/repairing sparse ID arrays during data migration** — when records should have contiguous IDs from `1..n`, a cyclic-sort-style pass (or just its verification-scan half) identifies exactly which IDs are missing/duplicated, indicating deleted or corrupted rows, without a second data structure.
- **Counting/frequency-array-free duplicate detection in constrained environments** — embedded systems, tight-memory serverless functions validating a bounded-range array (e.g. confirming all page numbers of a paginated export arrived) without room to spare for a hash set, and validating uploaded CSV/batch files of sequential IDs in O(1) extra memory instead of loading a hash set sized to a potentially very large `n`.

## Similar Patterns

- **Fast & Slow Pointers** ([../../linked-list-patterns/fast-slow-pointers/](../../linked-list-patterns/fast-slow-pointers/)): also finds a duplicate value in `[1..n]` context (*Find the Duplicate Number* is solvable both ways), but treats the array as an implicit linked list (`nums[i]` points to the next index) and uses Floyd's Cycle Detection to find the cycle's entry point — provably the duplicate. **Non-mutating**, a real advantage for this one problem, but it does not generalize to "all missing values" or "first missing positive." See [problems/03-find-the-duplicate-number.cpp](problems/03-find-the-duplicate-number.cpp) for both approaches contrasted directly.
- **Plain hashing:** same family, same O(n) time, O(n) extra space — but unlike Cyclic Sort, works on any values in any range and never mutates. Hashing is strictly more general; Cyclic Sort is strictly more space-efficient once its bounded-range precondition holds.
- **Two Pointers** ([../two-pointers/](../two-pointers/)): shares the "single pass, O(1) space, in-place" flavor but answers pairs/triplets-summing-to-target or compaction questions, and needs *sorted* input rather than a bounded value-to-index range.

| Pattern | Precondition | Mechanism | Mutates input? | Extra space | Generalizes to |
|---|---|---|---|---|---|
| **Cyclic Sort** | Values bounded to `[1..n]`/`[0..n-1]` | In-place swap to home index | Yes | O(1) | Missing / all-missing / duplicate / first-missing-positive |
| Fast & Slow Pointers (Floyd's) | Values in `[1..n]`, treated as implicit links | Cycle detection at different speeds | No | O(1) | Finding *one* duplicate only |
| Hashing (hash set / frequency array) | None — any values, any range | Record seen values, check membership | No | O(n) | Same family, plus unbounded ranges |
| Sorting | None — any comparable values | Full comparison-based sort, then scan | Depends (in-place or copy) | O(1) or O(n) | Same family, plus general ordering needs |

## Interview Discussion

Experienced engineers do not spend interview time on "how do you write the swap" — that is mechanical. What they probe is whether you **recognize the bounded-range signal** immediately, and can articulate *why* that fact avoids the extra O(n) space a hash set would cost.

Common follow-up questions:
- *"Can you do this without extra space?"* — the canonical invitation to move from a hash-set solution to Cyclic Sort; expects naming the bounded-range precondition explicitly.
- *"What if there could be more than one missing number?"* — expects recognizing the swap-then-verify mechanism generalizes directly: the verification pass reports *every* mismatched index instead of stopping at the first (see [problems/02-find-all-numbers-disappeared-in-an-array.cpp](problems/02-find-all-numbers-disappeared-in-an-array.cpp)).
- *"Can you solve Find the Duplicate Number without modifying the array?"* — expects naming Floyd's Cycle Detection (Similar Patterns above) as the non-mutating alternative.

Common misconceptions: the nested `while` inside the cursor loop making this O(n²) (it does not — see Complexity for the amortized bound); that Cyclic Sort produces a fully sorted array (it produces one that is only *as sorted as possible*, since unplaceable values are left in place); that this is the same as counting sort (related in spirit, but counting sort builds a separate count array while Cyclic Sort rearranges the original in place with no auxiliary structure); and that you must fully sort before finding the answer (the swap pass and verification scan are two separate steps — a common point of confusion when first learning this pattern).

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
