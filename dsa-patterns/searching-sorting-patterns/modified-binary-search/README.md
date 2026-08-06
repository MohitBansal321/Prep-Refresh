# Modified Binary Search

## Intent

Generalize classic binary search's "halve the search space" idea from a narrow "find this exact value" tool into a general technique for finding boundaries, first/last occurrences, and target values inside rotated or otherwise piecewise-sorted arrays.

## Real Life Analogy

Think about **looking up a word in a printed dictionary**. You don't start at page 1 and scan forward — you open to roughly the middle, see whether your word comes before or after that page alphabetically, and throw away the half you now know can't contain it. Repeat on the remaining half, and again, and again, and within about 20 splits you can find any word in a 1,000,000-word dictionary.

Now imagine a slightly stranger dictionary: someone has taken the last third of the pages and moved them to the front (a "rotated" dictionary). It's still locally sorted — within the moved section and within the rest, alphabetical order holds — but the split point breaks the simple "is my word before or after the middle page?" rule. You can still binary search it, but at each step you first have to work out **which of the two halves is actually a normal, sorted range** before you can decide which half to keep. That extra one-line check — "which half is guaranteed sorted, and does my target fall inside it?" — is the entire generalization this module is about.

## Problem

### What engineering problem exists?

Any time input is sorted (or "sorted enough" — piecewise sorted, like a rotated array), a linear scan wastes the sortedness entirely: it's an `O(n)` algorithm applied to data that supports something far faster.

- **Finding an exact value** in a sorted array — the classic case.
- **Finding a boundary**: the first or last position where a value or property holds (e.g. "the first index where `nums[i] >= target`").
- **Finding a value in a rotated sorted array** — an array that was sorted, then rotated at some unknown pivot (common in circular buffers, log files that wrap, or "find where a cycle starts" style questions).
- **Finding an answer over an implicit, monotonic search space** — not a literal array at all, but a range of candidate answers (e.g. "the smallest capacity that lets you ship all packages within D days") where the property "is this candidate feasible?" is monotonic, so the same halving logic applies.

> **Term: Monotonic property.** A true/false (or comparable) property that, once it flips from false to true (or vice versa) as you scan across a sorted range, never flips back. Binary search fundamentally only needs a monotonic property to halve on — the property doesn't have to be "equals the target," it just has to never oscillate.

The naive way to answer any of these is a linear scan: `O(n)`.

### Why is this problem difficult?

- **A single halving rule only works for the narrowest case.** `if (target == mid) return; else if (target < mid) search left; else search right` finds an exact match in a plain sorted array, but breaks the moment the question is "find the FIRST occurrence" (there might be several matches) or "find where the array was rotated" (there's no single target to compare against).
- **Rotated arrays need an extra decision before the usual one.** At any given `mid`, exactly one of the two halves (`[lo, mid]` or `[mid, hi]`) is guaranteed to be a normal, non-rotated sorted range — but which one depends on comparing `nums[lo]`, `nums[mid]`, and `nums[hi]` first. Only after identifying the sorted half can you ask "does my target fall inside that half's range?" and decide which side to keep.
- **Off-by-one errors compound quickly.** Whether the loop condition is `lo <= hi` or `lo < hi`, whether `mid` is included or excluded from the next search range, and whether you're searching for "the leftmost true" versus "the rightmost true" all interact — a single wrong inequality can produce an infinite loop or silently skip the answer.

### What happens if we ignore it?

- **`O(n)` scans on data structures that support `O(log n)`.** For large sorted datasets (database indexes, sorted logs, large arrays), this is the difference between an instant lookup and a noticeably slow one.
- **Incorrect answers on boundary questions.** A "find first occurrence" search that isn't carefully adapted from "find any occurrence" will non-deterministically return any matching index, not specifically the first — silently wrong for problems that require the boundary specifically.
- **Infinite loops or missed answers on rotated/implicit search spaces**, if the "which half is sorted" or "is this candidate feasible" checks are wrong.

## Why Not Other Approaches?

**"Linear scan."** Always correct, but `O(n)` — throws away the one piece of structure (sortedness) that the problem handed you for free.

**"Sort the data first, then binary search."** If the data isn't sorted yet, sorting costs `O(n log n)` up front — worse than a single `O(n)` scan if you only need to search once. Modified Binary Search is only a win when the sortedness (or piecewise sortedness) already exists, or when you'll perform the search many times against the same sorted data (amortizing the one-time sort cost).

**"Use a hash map for exact-match lookups."** A hash map gives `O(1)` average lookup for exact matches — genuinely faster than binary search for that one narrow case. But it cannot answer "first/last occurrence," "nearest value," "smallest feasible candidate," or anything involving order at all — a hash map has no concept of "before" or "after." The moment the question involves order or boundaries rather than pure membership, binary search's structure is required.

**Tradeoff summary:** linear scan is always correct but wastes the sortedness; sorting first only pays off when the sort cost is amortized; hashing is faster for pure exact-match but blind to order. Modified Binary Search is the only approach that exploits existing (possibly piecewise) order to answer boundary, rotated-array, and "smallest feasible candidate" questions in `O(log n)`.

## Solution

Generalize the halving rule from "compare to a target" to **"which half of the search space is guaranteed to have the property I care about, and does my answer lie in that half?"** Concretely:

- **Exact match:** compare `nums[mid]` to `target` directly; discard the half that cannot contain it.
- **First/last occurrence:** on finding a match, don't stop — record it as a candidate answer, then keep searching the half that could contain an *earlier* (or *later*) match.
- **Rotated array:** first determine which half, `[lo, mid]` or `[mid, hi]`, is itself normally sorted (compare `nums[lo]` to `nums[mid]`, or `nums[mid]` to `nums[hi]`). Then check whether the target falls within that sorted half's value range; if so, search there, otherwise search the other (rotated) half.

In every variant, the loop still halves the search space every iteration — only the one-line decision of "which half do I keep" changes.

## Architecture

The "participants" are:

1. **`lo` and `hi`**, the current search boundaries — always narrowing, never widening.
2. **`mid`**, computed each iteration as the midpoint of `[lo, hi]` (using `lo + (hi - lo) / 2` to avoid integer overflow — see Common Mistakes).
3. **The halving decision** — the one piece of logic that differs between variants: a direct value comparison (exact match), a "have I seen a match yet, and which direction do I still need to search" check (first/last occurrence), or a "which half is sorted, and does the target fall inside it" check (rotated array).
4. **The loop invariant** — the answer, if it exists, is always still inside `[lo, hi]` at the start of every iteration. Every variant of this pattern must preserve that invariant; the mechanism used to decide which half to discard is the only thing that changes.

## Execution Flow

1. **Classic exact match:** while `lo <= hi`, compute `mid`. If `nums[mid] == target`, return `mid`. If `nums[mid] < target`, discard the left half (`lo = mid + 1`). Otherwise discard the right half (`hi = mid - 1`). If the loop ends without a match, the target isn't present.
2. **First/last occurrence:** same loop, but on a match, record `mid` as the best-so-far answer and *keep searching* — toward `lo` for the first occurrence (`hi = mid - 1`), or toward `hi` for the last occurrence (`lo = mid + 1`) — rather than returning immediately.
3. **Rotated sorted array:** at each `mid`, first check whether `nums[lo] <= nums[mid]` (left half is normally sorted) or `nums[mid] <= nums[hi]` (right half is normally sorted) — exactly one of these holds at any given split (see Common Mistakes for the edge case where `lo == mid` or `mid == hi`). Then check whether `target` falls within that sorted half's value range (`nums[lo] <= target < nums[mid]`, or the symmetric case for the right half); if so, search that half, otherwise search the other one.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the flowchart distinguishing Modified Binary Search from a plain linear scan and from Two Pointers, based on whether the input is sorted/piecewise-sorted and whether the search space can be halved.

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the control-flow diagram of the `lo`/`mid`/`hi` halving loop, including the extra "which half is sorted" branch needed for rotated arrays.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of `lo`, `mid`, `hi` across iterations searching a concrete rotated sorted array.

## Implementation

[code.cpp](code.cpp) provides four generic, reusable functions covering the main variants of this pattern: `binarySearch` (classic exact match), `findFirstOccurrence`, `findLastOccurrence`, and `searchRotated` (rotated sorted array). All four share the same `lo`/`mid`/`hi` loop skeleton, differing only in the halving decision, so they're best read side by side to see exactly what changes and what doesn't.

## Code Walkthrough

**`binarySearch(nums, target)`** (in [code.cpp](code.cpp)). The textbook version: `lo <= hi` loop, `mid = lo + (hi - lo) / 2`, three-way comparison against `target`, returns the index on an exact match or `-1` if the loop exhausts without finding one.

**`findFirstOccurrence(nums, target)`** and **`findLastOccurrence(nums, target)`** (in [code.cpp](code.cpp)). Both reuse the exact same loop structure as `binarySearch`, but on a match they record the index as a candidate answer and continue searching — toward `lo` (shrinking `hi = mid - 1`) for the first occurrence, toward `hi` (growing `lo = mid + 1`) for the last. This demonstrates the core generalization: "found a match" is not automatically "done."

**`searchRotated(nums, target)`** (in [code.cpp](code.cpp)). Adds the "which half is sorted" check before the usual target comparison. At each `mid`, compares `nums[lo]` to `nums[mid]` to determine whether the left half is the normally-ordered one; if so, checks whether `target` falls in `[nums[lo], nums[mid])` to decide whether to search left or right; otherwise performs the symmetric check on the right half.

**Files in [problems/](problems/).** Each is a complete, standalone solution to one named LeetCode problem applying one of these four variants (or a small combination of them) to a specific, real problem statement. See [problems/README.md](problems/README.md) for the full index.

## Advantages

- **`O(log n)` instead of `O(n)`** — for large sorted datasets, this is the difference between an instant answer and a noticeably slow scan.
- **One mental model covers many superficially different problems.** Exact match, first/last occurrence, and rotated-array search are all "the same loop, different halving decision," not three unrelated algorithms to memorize.
- **Generalizes beyond literal arrays.** The same halving logic applies to any monotonic search space — "smallest capacity that works," "first day a condition becomes true," etc. — even when there's no physical sorted array at all.

## Disadvantages

- **Only works on sorted (or piecewise-sorted, or monotonic) data.** Applying this pattern to genuinely unordered data silently produces wrong answers rather than an obvious failure.
- **Easy to get subtly wrong.** Off-by-one errors in the loop condition, the `mid` update, or the half-selection logic are the single most common bug source in this pattern — see Common Mistakes.
- **Rotated-array variants require an extra comparison per iteration** (determining which half is sorted) — still `O(log n)` overall, but with a larger constant factor than the plain exact-match version.

## Tradeoffs

**What we gain:** replacing an `O(n)` linear scan with an `O(log n)` halving search, for any problem where a monotonic or sorted structure exists to exploit.

**What we lose:** the requirement that the data actually be sorted (or the search space actually be monotonic) in the first place — this pattern simply doesn't apply otherwise, and forcing it onto unordered data doesn't degrade gracefully, it silently returns wrong answers.

## Complexity

**Time:** `O(log n)` for all variants — each iteration halves the remaining search space, so the loop runs at most `O(log n)` times regardless of which half-selection rule is used.

**Space:** `O(1)` iteratively (just `lo`, `mid`, `hi`, and a candidate-answer variable for the boundary variants) — or `O(log n)` if implemented recursively, due to the call stack.

**Comparison to the brute force it replaces:**

| Operation | Linear scan | Modified Binary Search |
|---|---|---|
| Exact match in sorted array | `O(n)` | `O(log n)` |
| First/last occurrence | `O(n)` | `O(log n)` |
| Search in rotated sorted array | `O(n)` | `O(log n)` |

## Common Mistakes

- **Computing `mid = (lo + hi) / 2` instead of `mid = lo + (hi - lo) / 2`.** The first form can overflow a fixed-width integer type when `lo` and `hi` are both large, even though their difference is small — a classic, easy-to-miss bug in any language with fixed-width integers.
- **Infinite loops from an inconsistent loop condition and boundary update.** Mixing `lo <= hi` with `hi = mid` (instead of `hi = mid - 1`) — or vice versa — can leave the search space unchanged forever when `lo` and `hi` converge to adjacent values.
- **Off-by-one on `<=` vs `<` in the loop condition**, which determines whether a single-element search space (`lo == hi`) is checked at all.
- **In rotated-array search, choosing the wrong half as "sorted."** When `lo == mid` (a two-element or one-element remaining range), comparing `nums[lo]` to `nums[mid]` is trivially true and doesn't actually tell you anything about which half is sorted — this edge case needs explicit handling, not just the general comparison.

## When To Use

- **Exact-match, first-occurrence, or last-occurrence lookups in sorted data.**
- **Searching a rotated sorted array**, or any array that is sorted except for a single "wrap-around" point.
- **"Find the smallest/largest value satisfying a monotonic feasibility condition"** problems (e.g. "smallest capacity that ships all packages in D days," "first bad version"), even when there's no literal sorted array — as long as the feasibility check is monotonic across the candidate range.
- **`std::lower_bound`/`std::upper_bound`-style range queries** against sorted data.

## When NOT To Use

- **The data isn't sorted and has no monotonic property to exploit** — a linear scan (or a hash map for pure exact-match) is simpler and just as correct.
- **You need order-independent membership testing only, with no concept of "before/after" or boundaries** — a hash set is faster (`O(1)` average) and simpler.
- **The array is small enough that the constant-factor overhead of binary search's extra bookkeeping isn't worth it** compared to a simple linear scan — a real, if minor, practical consideration for tiny inputs.

## Real Interview/Production Examples

- **Database index range scans.** B-tree indexes in relational databases use the same halving principle to locate rows within a sorted key range in `O(log n)`.
- **`std::lower_bound`/`std::upper_bound` (C++ standard library) and `bisect` (Python)** are direct, battle-tested implementations of the boundary-search variant of this pattern.
- **Version-control bisection** (`git bisect`) applies the exact same halving idea to a monotonic "is this commit good or bad" property to find the first bad commit in `O(log n)` steps instead of checking every commit.
- **Log file analysis** — finding the first log entry after a given timestamp in an already time-ordered log file.

## Where I Can Use This

Five realistic ideas for your own backend projects:

1. **A time-range query endpoint** over a sorted-by-timestamp event log, finding the first event at or after a given time in `O(log n)` instead of scanning the whole log.
2. **A feature-flag rollout percentage finder** — binary search over rollout percentages to find the smallest percentage at which a metric first crosses a threshold, if the metric behaves monotonically with rollout size.
3. **A capacity-planning tool** — binary search over candidate server counts/instance sizes to find the minimum capacity that keeps latency under a target SLA, given a monotonic "does this capacity satisfy the SLA" check.
4. **A paginated API's cursor lookup** — locating the correct starting position in a sorted result set given an opaque cursor value, in `O(log n)` instead of scanning from the beginning.
5. **A deployment bisection tool** — mirroring `git bisect`, binary search over a sorted list of deployments/releases to find the first one exhibiting a regression, given an automated "is this release good or bad" check.

## Similar Patterns

- **Two Pointers** ([../../array-string-patterns/two-pointers/](../../array-string-patterns/two-pointers/)): also exploits sortedness, but with two simultaneously-moving indices converging from opposite ends, rather than repeatedly halving a single range. Reach for Two Pointers when the question involves pairs/triplets across a sorted array; reach for Modified Binary Search when the question is "find one specific value or boundary."
- **Top K Elements** ([../top-k-elements/](../top-k-elements/)): a heap-based technique for a different kind of "search" question (the K largest/smallest/most-frequent), not a halving search over sorted order.

| Pattern | Core mechanism | Best for |
|---|---|---|
| Modified Binary Search | Halve the search space each step | Exact match, first/last occurrence, rotated-array search, monotonic feasibility search |
| Two Pointers | Two converging/co-moving indices | Pair/triplet sums, in-place compaction on sorted or otherwise structured arrays |
| Top K Elements | Fixed-size heap | K largest/smallest/most-frequent, without a full sort |

## Interview Discussion

Experienced engineers rarely dwell on writing the basic exact-match loop — they probe whether you can correctly adapt the halving decision to boundary and rotated-array variants, and whether you can spot the overflow and off-by-one traps without being prompted.

Common follow-up questions:
- *"Can you find the first occurrence, not just any occurrence, of a duplicated value?"* — expects the "keep searching after a match" adaptation, not a linear scan bolted onto the result.
- *"How do you binary search a rotated sorted array?"* — expects identifying which half is sorted at each step before deciding which half to search, and correctly handling the edge case where the array isn't rotated at all (or the rotation point coincides with `lo`/`mid`/`hi`).
- *"Why use `lo + (hi - lo) / 2` instead of `(lo + hi) / 2`?"* — expects recognizing the integer-overflow risk in fixed-width arithmetic when both bounds are large.
- *"Can binary search apply even without a literal sorted array?"* — expects recognizing that any monotonic feasibility check over an ordered candidate range is halvable the same way, citing an example like "smallest capacity that works."

Common misconceptions:
- "Binary search only works on arrays." It works on any monotonic search space — arrays are just the most common concrete example.
- "Finding a match means you're done." Not for first/last-occurrence variants — a match is a candidate answer that must be refined further.
- "A rotated sorted array can't be binary searched in `O(log n)`." It can — the extra "which half is sorted" check adds only a constant amount of work per iteration.

## Summary

- Generalizes binary search from "does `nums[mid]` equal `target`?" to "which half of the search space can be discarded, given whatever property this specific problem cares about?"
- Exact match, first/last occurrence, and rotated-array search are the same `lo`/`mid`/`hi` loop with three different halving decisions.
- Rotated-array search requires an extra check per iteration — determine which half is normally sorted before deciding which half contains the target.
- The pattern generalizes beyond literal arrays to any monotonic feasibility search over an ordered candidate range.
- `O(log n)` time, `O(1)` space (iteratively) for every variant.

## Key Takeaways

1. The core loop (`lo`, `mid`, `hi`, halve each iteration) never changes — only the one-line halving decision differs between variants.
2. First/last occurrence: don't stop on a match — record it as a candidate and keep searching in the direction that could improve it.
3. Rotated array search: first determine which half is normally sorted, then check whether the target falls inside that half's range.
4. Always compute `mid = lo + (hi - lo) / 2` to avoid integer overflow.
5. Keep the loop condition (`<=` vs `<`) and the boundary update (`mid - 1`/`mid + 1` vs `mid`) consistent to avoid infinite loops.
6. This pattern generalizes to any monotonic feasibility search, not just literal sorted arrays.
7. Complexity is `O(log n)` time, `O(1)` space (iterative) for every variant covered here.
8. Two Pointers is the sibling pattern for pair/triplet problems on sorted arrays; reach for Modified Binary Search when you need one specific value or boundary instead.
9. Real production uses: database index range scans, `std::lower_bound`/`upper_bound`, `git bisect`-style regression hunting.
10. Never apply this pattern to genuinely unordered data — it fails silently (wrong answer), not loudly (crash).

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — binary search and its correctness invariants.
- *The Algorithm Design Manual* — Steven Skiena — binary search framed as a general search-space-halving technique, not just an array lookup.

**Open Source Projects / GitHub Repositories**
- `TheAlgorithms/C++` — community-maintained classic algorithm implementations, including binary search variants.

**Official Documentation**
- cppreference — `std::lower_bound` / `std::upper_bound` — the C++ standard library's own boundary-search implementations.
- LeetCode — Binary Search (problem 704).
- LeetCode — Find First and Last Position of Element in Sorted Array (problem 34).
- LeetCode — Search in Rotated Sorted Array (problem 33).
- LeetCode — Find Minimum in Rotated Sorted Array (problem 153).

**Blog Articles**
- GeeksforGeeks — "Binary Search" explainer, covering the standard variants and common pitfalls.
- Educative.io — "Grokking the Coding Interview," the Modified Binary Search pattern chapter.
