# Sliding Window Pattern

## Intent

Compute something about every contiguous subarray or substring of an array/string in a single linear pass, by maintaining a **window** `[left, right]` and a small running aggregate that is updated incrementally as the window moves — instead of re-examining every element of every window from scratch.

## Real Life Analogy

Think of a **security guard watching a bank of CCTV monitors through a physical viewfinder** that only covers a few feet of a hallway at a time. The guard does not re-walk the entire hallway from the start every time something new happens at the far end. Instead, they slide the viewfinder: as a new section of hallway comes into view on the right, they take note of it; as an old section falls out of view on the left, they stop tracking it. Their mental tally — "how many people are currently in view," "is anyone suspicious in view right now" — is updated incrementally with each slide, never recomputed by starting over.

Another everyday one: a **bus with a fixed number of seats moving down a route**. At each stop, some passengers get off (leave the window) and some get on (enter the window). The driver does not recount every passenger from scratch at every stop — they just track who got on and who got off. The "current occupancy" is a running total, adjusted by one entry and one exit per stop.

The Sliding Window pattern is that viewfinder or that bus: a bounded, contiguous range that moves across the data, with a running summary that is nudged forward rather than rebuilt.

## Problem

### What engineering problem exists?

A large class of array/string problems asks you to compute something about **every contiguous range** of the input:

- "What is the maximum sum of any 5 consecutive elements?"
- "What is the longest substring with no repeated characters?"
- "What is the shortest subarray whose sum is at least a target value?"
- "Does some window of size K contain all the characters of another string?"

> **Term: Contiguous.** A subarray or substring made of elements that sit next to each other in the original sequence, with no gaps and no reordering. `[3,4,5]` is a contiguous subarray of `[1,2,3,4,5,6]`; `[3,5]` is not (it skips 4). This distinction matters because it is exactly what makes Sliding Window applicable — the pattern only works when the range you care about is a single unbroken slice.

The natural first instinct is to check every possible contiguous range directly: for a fixed size K, check every window of that size; for "find the longest/shortest," check every `(start, end)` pair. Both approaches redo work that a smarter approach would reuse.

### Why is this problem difficult?

- **Adjacent windows overlap almost entirely.** A window of size K starting at index `i` and the next window starting at index `i+1` share `K-1` elements. If you recompute each window's answer from scratch (summing K elements again, rebuilding a frequency count again), you throw away nearly all of the previous window's work every single step.
- **"Longest/shortest satisfying a condition" has no obvious starting size.** Unlike a fixed-K problem, you do not know in advance how big the answer window is — a naive approach tries every `(start, end)` pair, which is inherently quadratic (there are `n(n+1)/2` such pairs).
- **The "state" of a window (its sum, or which characters it contains and how many times) is fiddly to maintain correctly when elements leave, not just when they enter.** Forgetting to undo the effect of a departing element is the single most common source of bugs in this pattern (see Common Mistakes).

### What happens if we ignore it — brute force, with concrete numbers?

**Fixed-size window, brute force:** for an array of length `n` and window size `k`, summing every window from scratch is `O(k)` work per window, and there are `O(n - k + 1)` windows, giving **O(n·k)** total. For `n = 100,000` and `k = 1,000`, that is roughly **10^8 operations** just to answer one query — noticeably slow, and it gets worse linearly as `k` grows.

**Variable-size window ("longest/shortest satisfying a condition"), brute force:** checking every `(start, end)` pair is `O(n^2)` pairs, and if verifying each pair's condition itself costs `O(n)` (e.g., scanning to check for duplicate characters), the total is **O(n^3)**. Even with an `O(1)`-per-pair check via prebuilt counts, you are still at **O(n^2)**. For `n = 50,000` (a realistic LeetCode/production string length), `n^2` is **2.5 × 10^9** operations — this will time out in any interview or production budget measured in milliseconds.

Both brute-force shapes share the same root cause: they treat every window as an independent problem instead of noticing that consecutive windows are related by exactly one element entering and (usually) one element leaving.

## Why Not Other Solutions?

**"Just recompute each window's sum/count from scratch."** This is the brute force above. It is correct but wasteful — it is O(n·k) or O(n²)/O(n³) precisely because it never reuses the (k-1) or (window-size − 1) elements shared between adjacent windows. Every millisecond spent re-summing or re-counting those shared elements is pure waste.

**"Sort the array first."** Sorting destroys contiguity — once you reorder elements, "subarray" and "substring" stop meaning anything, because the pattern's entire value comes from exploiting the *original order* of the data. Sorting is the right move for a different family of problems (see Two Pointers, below), but it is actively wrong here.

**"Use dynamic programming with a 2D table of every (start, end) pair."** For some variants (e.g., "is s[i..j] a palindrome") a DP table over all `(i, j)` pairs is a legitimate O(n²)-space, O(n²)-time solution. It works, but it is strictly worse than Sliding Window whenever the underlying condition can be tracked incrementally with O(1) or O(alphabet)-size state — which is true for the vast majority of window problems (sums, distinct-character counts, frequency matching). Reach for the 2D table only when the condition genuinely cannot be summarized by a small running aggregate (e.g., "is this substring a palindrome" needs more than a running count).

**Contrast with Two Pointers.** Two Pointers (see `../two-pointers/`) also uses two indices moving through the array, but the classic use case is a **sorted** array where the two pointers move *toward* or *away from* each other (e.g., "find a pair that sums to target" — move `left` right or `right` left based on a comparison). Sliding Window's two pointers (`left` and `right`) almost always move in the **same direction** (both only ever increase), tracing out a contiguous range rather than searching from both ends inward. Sliding Window is best understood as **Two Pointers specialized to contiguous ranges with a running aggregate** — the family README (`../README.md`) calls Sliding Window a direct generalization of Two Pointers for exactly this reason.

**Tradeoff summary:** every brute-force alternative pays for information the previous window's computation already had. Sliding Window's entire value proposition is refusing to throw that information away — it costs a small amount of bookkeeping (the running aggregate) in exchange for turning O(n·k) or O(n²)/O(n³) into O(n).

## Solution

The core idea has one sentence: maintain a window `[left, right]` over the array/string that only ever **grows** (by advancing `right`) or **shrinks** (by advancing `left`) — it never resets to the beginning and never re-scans elements it has already accounted for.

Alongside the window, keep a small **running aggregate** that summarizes "what does the current window look like" — a running sum, a frequency map of characters, a count of distinct elements, or similar. Every time the window changes (an element enters on the right, or an element leaves on the left), you update the aggregate incrementally in O(1) (or O(1) amortized) time rather than recomputing it from the window's contents.

There are exactly two shapes this takes:

1. **Fixed-size window:** the width `k` is given by the problem. You build the first window's aggregate once, then slide one step at a time — one element enters, one leaves, the aggregate updates, you check/record the answer. The window width never changes.

2. **Variable-size window:** the width is not given; you are searching for the longest or shortest contiguous range that satisfies some condition. You grow the window (advance `right`) until the condition is violated or satisfied, then shrink it (advance `left`) while the appropriate side of that condition still holds, recording candidate answers along the way. The window width is an emergent property of the data, not an input.

The thinking behind it: **never redo work the previous window already did.** Every element should be examined a small, bounded number of times — typically once when it enters the window and once when it leaves — no matter how many total windows the algorithm considers. That is what turns a quadratic-or-worse brute force into a linear scan.

## Architecture

Every Sliding Window solution has the same small cast of participants:

1. **`left` pointer:** marks the start (inclusive) of the current window. It only ever moves forward (never resets backward). Its job is to shrink the window when growing it further would violate — or, depending on the problem, satisfy — the constraint being tracked.

2. **`right` pointer:** marks the end (inclusive) of the current window. It only ever moves forward, one step per outer-loop iteration. Its job is to grow the window by bringing the next element into consideration.

3. **The running aggregate (window state):** the piece of data that summarizes "what does the window currently contain" — a running sum (`long long windowSum`), a frequency map (`unordered_map<char,int>`), a count of distinct keys, or a small fixed-size array of counts. Its job is to be updated in O(1) (or O(1) amortized) on every single-element change to the window, so that "is the window currently valid?" can be answered without re-scanning the window's contents.

4. **The best-answer tracker:** a single variable (`best`, `maxSum`, `bestLen`) that records the best value seen across all windows visited so far. Its job is to be checked/updated at the right moment — after every slide for fixed-size windows; after every valid-window state for variable-size windows.

Responsibilities in one line each:
- **`left`:** shrinks the window, one element at a time, undoing that element's effect on the aggregate.
- **`right`:** grows the window, one element at a time, folding that element's effect into the aggregate.
- **Aggregate:** answers "is the window valid?" or "what is the window's current value?" in O(1), without looking at the window's raw contents.
- **Best tracker:** remembers the answer so the rest of the algorithm can forget it and keep scanning.

## Execution Flow

**Fixed-size window, step by step:**

1. Initialize the aggregate using the first `k` elements directly (this costs `O(k)`, paid exactly once).
2. Record the first window's value as the current best.
3. Advance `right` from index `k` to `n-1`. On each step:
   a. Fold `arr[right]` into the aggregate (the entering element).
   b. Remove `arr[right - k]` from the aggregate (the leaving element — always exactly `k` positions behind `right`).
   c. Compare the updated aggregate against the best tracker and update it if this window is better.
4. After the loop, the best tracker holds the answer across every window of size `k`.

**Variable-size window, step by step (generic "longest valid window" shape):**

1. Initialize `left = 0` and an empty/zero aggregate.
2. Advance `right` from `0` to `n-1`. On each step:
   a. Fold `arr[right]`/`s[right]` into the aggregate (grow).
   b. While the aggregate currently **violates** the constraint (e.g., too many distinct characters, sum below a required threshold in a "must stay valid" sense — the exact condition is problem-specific): remove `arr[left]`/`s[left]` from the aggregate and advance `left`. Repeat until the window is valid again.
   c. Now that the window is guaranteed valid, compare `right - left + 1` against the best tracker and update it if longer.
3. After the loop, the best tracker holds the length (or content) of the longest valid window.

**Variable-size window, step by step (generic "shortest valid window" shape — the mirror image):**

1. Initialize `left = 0` and an empty/zero aggregate.
2. Advance `right` from `0` to `n-1`. On each step:
   a. Fold `arr[right]`/`s[right]` into the aggregate (grow).
   b. While the aggregate currently **satisfies** the constraint (e.g., sum ≥ target, or all required characters present): record `right - left + 1` as a candidate (update the best tracker if shorter), then remove `arr[left]`/`s[left]` from the aggregate and advance `left` — because a shorter valid window is strictly better, and shrinking is the only way to find one.
3. After the loop, the best tracker holds the length of the shortest valid window (or a sentinel meaning "no valid window exists").

The only difference between the "longest" and "shortest" shapes is **when** you record the candidate answer relative to the shrink loop — before/during shrinking (shortest) or after the window is confirmed valid post-shrink (longest). The grow/shrink mechanics on `left`, `right`, and the aggregate are otherwise identical.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full Mermaid flowchart deciding "is this a Sliding Window problem, and which variant?" against Two Pointers, Prefix Sum, and Kadane's Algorithm.

**How to read it:** the gate is always "contiguous or not" first, then "is the window size given (fixed) or something to search for (variable)," and — for the variable-size branch — whether the constraint is **monotonic** in window size (growing the window only ever helps or only ever hurts the condition, never both). That monotonicity check is the one people skip and is exactly why "subarray sum equals K with negative numbers allowed" is *not* a Sliding Window problem (it needs Prefix Sum + a hash map instead).

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the full Mermaid flowchart of the variable-size window's control flow: expand right, update the aggregate, while-loop shrink from the left while invalid (updating the aggregate on every shrink step), record the best answer, repeat.

**How to read it:** `right` always advances exactly once per outer iteration; `left` advances a *variable* number of times (zero or more) inside the inner while-loop. The while-loop, not an if-statement, is what correctly handles cases where a single shrink step is not enough to restore validity — see Common Mistakes below.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a full step-by-step trace of `left`, `right`, and the window's character set on the concrete example `s = "abcabcbb"` (Longest Substring Without Repeating Characters).

**How to read it:** watch how, at `right = 6` and `right = 7`, the shrink loop runs *twice* in a single outer iteration before the window becomes valid again — direct, concrete proof of why the shrink step must be a `while`, not a single conditional check.

## Implementation

Before looking at [code.cpp](code.cpp), understand what it is demonstrating: two **generic, reusable template functions**, not solutions to one specific interview problem. The point of the file is to show the *shape* of the pattern in its purest form so that the four fully worked problems in `problems/` read as "the same shape, applied," rather than four unrelated tricks.

`code.cpp` implements:

1. **`maxSumSubarrayFixedWindow`** — the fixed-size window template. Builds the first window's sum directly, then slides with one addition and one subtraction per step.
2. **`shortestSubarrayWithSumAtLeast`** — the variable-size, "shortest valid window" template, using a running sum as the aggregate and a numeric threshold as the condition.
3. **`longestSubstringWithAtMostKDistinct`** — the variable-size, "longest valid window" template, using a frequency map as the aggregate and "at most K distinct keys" as the condition.

All three share the same skeleton described in Execution Flow above; they differ only in what the aggregate is and what "valid"/"invalid" means for that aggregate.

## Code Walkthrough

See [code.cpp](code.cpp) for the full runnable implementation, and `problems/` for four fully worked, standalone LeetCode-style solutions. Here is what each part does and why it exists.

**`maxSumSubarrayFixedWindow(arr, k)`.** Computes the maximum sum among all contiguous windows of size `k`. It builds the initial window sum with a direct loop over the first `k` elements (this is the only "expensive" step, and it happens once), then for every subsequent position adds the entering element and subtracts the element leaving exactly `k` positions behind. This function exists to be the simplest possible demonstration of "reuse the previous window's work" — it is the base case every other fixed-size-window problem specializes.

**`shortestSubarrayWithSumAtLeast(arr, target)`.** Finds the length of the shortest contiguous subarray whose sum is at least `target`, assuming non-negative elements (see Common Mistakes for why that assumption is load-bearing). It grows the window by adding `arr[right]` to a running sum, then — whenever the sum already meets the target — shrinks from the left as far as it can while still meeting the target, recording the shortest length seen along the way. This function exists to demonstrate the "shortest valid window" control-flow variant, where the candidate answer is recorded *during* the shrink loop rather than after it.

**`longestSubstringWithAtMostKDistinct(s, k)`.** Finds the length of the longest substring containing at most `k` distinct characters. It grows the window by incrementing a character's count in a frequency map, then — whenever the map has *more* than `k` distinct keys (i.e., the window has become invalid) — shrinks from the left, decrementing counts and **erasing** any character whose count hits zero (this erase step is the most commonly forgotten line in this entire pattern; a stale zero-count entry would make `freq.size()` overcount distinct characters and silently produce a wrong answer). This function exists to demonstrate the "longest valid window" control-flow variant with a non-scalar aggregate (a map instead of a single number).

**`main()`.** Exercises all three templates against known, hand-verified expected outputs and prints `PASS`/`FAIL` per case, so the file proves itself correct every time it is compiled and run — no external test framework needed.

**`problems/01-max-sum-subarray-of-size-k.cpp`.** The canonical Grokking/GfG "Maximum Sum Subarray of Size K" problem. It exists to be the very first fixed-size-window problem a learner sees, including a brute-force reference implementation side by side so you can see the O(n·k) vs. O(n) contrast directly in code, not just in prose.

**`problems/02-longest-substring-without-repeating-characters.cpp`** (LeetCode 3). Uses a slightly different but equally valid variable-size-window technique: instead of a `while` shrink loop, it keeps a **last-seen-index** map and jumps `left` directly to one past a duplicate's previous position. It exists to show that "shrink one element at a time" is the general mechanism, but when you can compute *exactly* how far to shrink in one step, doing so is a valid and common optimization that keeps the same O(n) complexity with less bookkeeping.

**`problems/03-minimum-window-substring.cpp`** (LeetCode 76). The hardest common shape: the window must satisfy a **multiset** requirement (character counts from `t`, not just distinct characters), tracked via a `satisfied`/`required` counter pair so that "is the window currently valid" is an O(1) check instead of a scan over the frequency map. It exists to show the pattern's most sophisticated real form, and to demonstrate why a *count of satisfied requirements* — not a raw map comparison — is what keeps the validity check O(1).

**`problems/04-minimum-size-subarray-sum.cpp`** (LeetCode 209). The concrete, LeetCode-numbered version of the "shortest valid window" template. It exists specifically to be the worked example for the "why must inputs be non-negative" discussion, since that constraint is exactly what LeetCode 209 specifies and it is not an accident.

**Interactions.** All five files (`code.cpp` plus the four `problems/` files) share the same `left`/`right`/aggregate/best-tracker architecture described above. The differences between them are entirely in what the aggregate tracks and what "valid" means — proof that internalizing the one control-flow shape lets you solve a wide variety of problems that look unrelated on the surface.

## Advantages

- **Turns O(n·k) or O(n²)/O(n³) into O(n).** The single biggest reason to reach for this pattern — see Complexity below for the exact numbers.
- **Constant or near-constant extra space.** A running sum is O(1); a frequency map is bounded by the alphabet size, not the input size.
- **Every element is processed a bounded number of times.** Each index is added to the aggregate exactly once (by `right`) and removed at most once (by `left`), which is what makes the amortized analysis O(n) even though there is a nested-looking `while` loop inside the `for` loop.
- **Directly matches how many real systems already work.** Network buffers, TCP flow control, and rate limiters all use a literal sliding window over a byte stream or a time axis — this is not just an interview trick (see Real Interview/Production Examples).
- **Short, teachable, reusable skeleton.** Once you internalize "grow right, conditionally shrink left, update aggregate both times," a huge fraction of array/string interview problems become "which aggregate, which condition" instead of "invent a new algorithm."

## Disadvantages

- **Only applies to contiguous ranges.** If the problem allows skipping elements or reordering, Sliding Window does not apply at all — you need a different pattern entirely (subsequence DP, hashing, sorting).
- **Variable-size windows require the constraint to be monotonic in window size.** If growing the window can make an invalid window valid again (non-monotonic), the "shrink while invalid" logic is unsound — this rules out things like "subarray sum equals K" with negative numbers allowed.
- **The running aggregate must be updated symmetrically.** Every operation that folds an element in on the right must have a matching, correct undo when that element leaves on the left. Getting this wrong (see Common Mistakes) produces answers that are subtly wrong on some inputs and correct on others — a nasty class of bug to debug.
- **Can obscure edge cases.** Empty input, `k` larger than the array, "no valid window exists" — these are easy to skip past when focused on the main sliding mechanism, and they are exactly where interview candidates lose points.

## Tradeoffs

**What we gain:** linear time instead of quadratic-or-worse, small constant/near-constant extra space, and a single reusable mental model across a large family of problems.

**What we lose:** generality. The pattern only fires when the problem is about contiguous ranges *and* (for the variable-size case) the condition being tracked is monotonic with window size. Outside those two constraints, Sliding Window either does not apply or silently gives wrong answers if forced onto a non-monotonic condition.

## Complexity

**Time:**
- **Fixed-size window:** O(n) — build the first window in O(k), then O(1) per slide for `n - k` slides. Best, worst, and average case are all O(n); there is no input that makes this pattern degrade, because every step does the same fixed amount of work regardless of the data.
- **Variable-size window:** O(n) amortized — `right` visits every index exactly once (n steps total); `left` also visits every index at most once across the *entire* run (it only ever moves forward and can advance at most n times total, even though it can advance zero or several times within any single outer iteration). The total work is therefore O(n) + O(n) = O(n), not O(n²), despite the nested-looking loop structure. This holds for best, worst, and average case alike.
- **Contrast with the brute force it replaces:** O(n·k) for fixed-size windows (e.g., n=100,000, k=1,000 → ~10^8 operations) collapses to O(n) (~10^5 operations) — roughly a 1,000x reduction. O(n²) or O(n³) for variable-size "longest/shortest" problems (e.g., n=50,000 → 2.5×10^9 or worse) collapses to O(n) (~5×10^4) — a reduction of 5 to 6 orders of magnitude.

**Space:**
- A running sum aggregate is O(1).
- A frequency-map aggregate is O(min(n, alphabet size)) — bounded by the number of distinct elements/characters that can appear, not by the input length. For lowercase English letters this is at most 26 regardless of how long the string is.
- No auxiliary array or table proportional to n is needed (contrast with a 2D DP table over all `(start, end)` pairs, which would be O(n²) space).

## Common Mistakes

- **Shrinking the window incorrectly (using `if` instead of `while`).** A single shrink step is sometimes not enough to restore validity — e.g., in Longest Substring Without Repeating Characters, evicting one character might still leave the actual duplicate inside the window if `left` has to pass over more than one character to reach it. *Why it happens:* the mental model "shrink once and move on" feels sufficient after the first easy test case passes. *Avoid it:* always use a `while` loop for the shrink condition, and test with an input where the shrink must happen multiple times per outer iteration (see the trace diagram's `right = 6, 7` steps).

- **Off-by-one on window bounds.** Window length is `right - left + 1`, not `right - left` — both pointers are inclusive. *Why it happens:* it is easy to think of `right` as "one past the end" (a common convention elsewhere in C++, like `.end()` iterators) when in this pattern it is not. *Avoid it:* fix one convention (both pointers inclusive) for the whole solution and sanity-check the length formula against a window of size 1 (`left == right` should give length 1).

- **Forgetting to update the running aggregate when shrinking.** It is easy to remember to update the aggregate when an element *enters* (because that is the "interesting" step you were focused on) and forget that removing an element from the window must *also* update the aggregate — decrement the sum, decrement the frequency count. *Why it happens:* the expand step is usually written first and gets more attention; the shrink step feels like an afterthought. *Avoid it:* write the shrink step's aggregate update in the same line/block where `left` is incremented, so the two changes cannot be separated by accident.

- **Forgetting to erase a frequency-map entry once its count hits zero.** Leaving a stale `key -> 0` entry in an `unordered_map` makes `map.size()` overcount the number of distinct keys, silently breaking any "at most K distinct" check. *Why it happens:* `map[key]--` is a natural, terse way to decrement, and it is easy to forget the follow-up `if (map[key] == 0) map.erase(key)`. *Avoid it:* treat decrement-and-possibly-erase as one atomic step you always write together, exactly as done in `longestSubstringWithAtMostKDistinct` in `code.cpp`.

- **Confusing "at most K" with "exactly K."** "At most K distinct characters" is directly a single sliding window (shrink whenever distinct count exceeds K). "Exactly K distinct characters" is a *different* problem — it is not simply "shrink whenever count != K," because growing the window can go from too-few to just-right to too-many, and a naive single window cannot track "was it ever exactly K." The standard fix is `exactly(K) = atMost(K) - atMost(K-1)`. *Why it happens:* the two phrasings sound almost identical in English. *Avoid it:* explicitly ask "is the condition monotonic in one direction (at most/at least), or does it require an exact match" before writing the shrink condition.

- **Applying Sliding Window to a sum-threshold problem with negative numbers.** The "shrink while sum >= target" logic assumes shrinking the window can only decrease the sum, which requires non-negative elements. With negatives, growing the window does not monotonically increase the sum, so the whole "shrink while valid" logic becomes unsound. *Why it happens:* the pattern works so well for non-negative sums that it is tempting to reach for it everywhere sums appear. *Avoid it:* check the sign of the inputs before choosing the pattern; with negatives, switch to Prefix Sum + a hash map (see Similar Patterns).

## When To Use

- The problem explicitly mentions a **contiguous** subarray or substring.
- A **fixed window size K** is given, and you need a running statistic (sum, max, average, count) over every window of that size.
- You need the **longest or shortest** contiguous range satisfying a condition that is **monotonic** in window size (adding elements only ever helps or only ever hurts, never both).
- The "state" of the window can be tracked with a small, incrementally-updatable aggregate (a number, a fixed-size count array, a small hash map) — not something that requires re-scanning the window to answer "is it currently valid."
- Streaming or buffer-style problems: you are processing a sequence and only need to remember a bounded recent history, not the entire sequence so far.

## When NOT To Use

- The problem allows **non-contiguous** subsets (any subsequence, not a contiguous slice) — Sliding Window fundamentally does not apply; look at subsequence DP or backtracking instead.
- The array needs to be **sorted first** and you are looking for pairs/triplets — that is Two Pointers' territory (see `../two-pointers/`), not Sliding Window's.
- The condition you are tracking is **not monotonic** in window size (e.g., subarray sum equals K with negative numbers present) — the shrink loop's correctness argument breaks down; use Prefix Sum with a hash map instead.
- You need to answer **many independent range-sum queries** on a static array with no sliding relationship between them — Prefix Sum answers each query in O(1) after an O(n) precompute, which is a better fit than re-sliding a window per query.
- The window's "state" cannot be summarized by a small incremental aggregate (e.g., "is this substring a palindrome," which genuinely needs more information than a running count) — a different technique (often DP) is required.

## Real Interview/Production Examples

- **Interview contexts:** Sliding Window is one of the most frequently asked pattern families at nearly every major tech company's SWE interviews (Amazon, Google, Meta, Microsoft, Bloomberg among others commonly feature it) — Longest Substring Without Repeating Characters (LC 3) and Minimum Window Substring (LC 76) are two of the most-repeated medium/hard string questions in practice, precisely because they test whether a candidate can maintain a running aggregate correctly rather than just "knowing the trick."
- **Rate limiting.** A "sliding window rate limiter" is a real, named production technique: instead of a fixed per-minute bucket that can be gamed by bursting at a bucket boundary, you track request timestamps in a rolling time window and reject requests once the window's count exceeds a threshold — an almost literal translation of the algorithmic pattern onto a time axis instead of an array index.
- **TCP flow control.** TCP's sliding window protocol governs how many unacknowledged bytes a sender may have in flight at once; the window slides forward as acknowledgments arrive, exactly mirroring "grow on new data, shrink as old data is confirmed/consumed."
- **Network packet buffering / stream processing.** Systems that process a continuous stream of data (log lines, sensor readings, packets) and need a rolling statistic (moving average, recent-max, recent-distinct-count) over the last N items or the last T seconds use the same expand/shrink mechanics as this pattern.
- **Media/analytics dashboards.** "Requests per second over the last 60 seconds" or "moving average latency over the last 100 requests" widgets are Sliding Window computations running continuously against a live stream rather than a fixed array.

## Where I Can Use This

Five realistic ideas for your own backend projects:

1. **A per-user or per-IP rate limiter** for an API gateway: maintain a rolling window of request timestamps per key and reject requests once the count in the last N seconds exceeds a threshold — the exercises.md "Real-World Challenge" walks through exactly this.
2. **A rolling metrics window** for a monitoring/observability service: "average response time over the last 100 requests" or "error rate over the last 60 seconds," updated incrementally as new data points arrive instead of recomputing from full history each time.
3. **Log anomaly detection:** scan a stream of log lines for the longest run of lines that stay within a normal-latency threshold, flagging the first window where the condition breaks — the same "longest valid window" shape as Longest Substring Without Repeating Characters.
4. **Chat/moderation systems:** detect the shortest recent window of messages from a user that trips a spam/toxicity threshold (e.g., total flagged-word count exceeds N within the last M messages), directly reusing the Minimum Window Substring / Minimum Size Subarray Sum shape.
5. **Data pipeline deduplication:** find the longest contiguous batch of records that can be processed together without exceeding a distinct-key limit (e.g., a batch write API that only allows K distinct partition keys per batch) — the same shape as Longest Substring With At Most K Distinct Characters.

## Similar Patterns

- **Two Pointers (`../two-pointers/`):** the parent idea Sliding Window specializes. Two Pointers is the general technique of tracking two indices instead of nested loops; its classic form moves the two pointers toward or away from each other over a **sorted** array (e.g., pair-sum problems). Sliding Window is Two Pointers restricted to **contiguous ranges**, where both pointers move in the same direction and a running aggregate summarizes the range between them.
- **Prefix Sum (`../prefix-sum/`):** a different axis entirely — *precompute* once, then answer each range-sum query in O(1), versus Sliding Window's *single incremental scan*. Prefix Sum shines when you have many independent range queries on a static array (no sliding relationship between them) or when negative numbers break Sliding Window's monotonicity assumption (e.g., "number of subarrays with sum exactly K" with negatives allowed is a Prefix Sum + hash map problem, not a Sliding Window problem).
- **Kadane's Algorithm:** closely related "running state" thinking — track a best-sum-ending-here value and reset it when it drops below zero — but it has no window-size constraint at all; it is really a 1D dynamic programming recurrence, not a two-pointer window. Use Kadane's when the problem is "best contiguous sum, no size constraint whatsoever," and Sliding Window when a size constraint (fixed K) or an explicit longest/shortest search is involved.

| Pattern | Core mechanism | Input requirement | Typical question shape | Complexity win |
|---------|----------------|---------------------|--------------------------|------------------|
| Sliding Window | Two same-direction pointers + running aggregate over a contiguous range | None (works on unsorted data) | "Longest/shortest/best window of ... satisfying ..." | O(n·k) or O(n²)/O(n³) → O(n) |
| Two Pointers | Two pointers, often converging, over sorted data | Usually sorted | "Find a pair/triplet that sums to ..." | O(n²) → O(n) |
| Prefix Sum | Precomputed running totals, O(1) range queries | None, but static (no updates) | "Sum of range [i,j]?" repeated many times | O(n) per query → O(1) per query |

## Interview Discussion

Experienced engineers rarely need to be shown *how* to write the two-pointer loop — that is mechanical once you have seen it. What they actually probe is whether you can **recognize** the pattern quickly and **justify** the window's correctness, especially the shrink loop's monotonicity assumption.

Common follow-up questions:
- *"Why is a `while` loop needed for the shrink, not an `if`?"* Because restoring validity can require removing more than one element from the left in a single step — a single check is not sufficient in general (see the "abcabcbb" trace at right=6,7).
- *"What is the time complexity, and why isn't the nested loop O(n²)?"* Because `left` only ever moves forward and, across the entire run, advances at most n times total — the amortized argument, not a per-iteration one, is what gives O(n).
- *"Does this work if the array has negative numbers?"* Only if the condition you are tracking is still monotonic with negatives present. Sum-threshold problems generally are not (this is the most common "gotcha" follow-up on Minimum Size Subarray Sum).
- *"How would you adapt this to a fixed-size window instead?"* Drop the `while` shrink loop entirely; instead subtract the element exactly `k` positions behind `right` on every step — a simpler, unconditional slide rather than a conditional shrink.
- *"How is this different from Two Pointers?"* Sliding Window's pointers move in the same direction over a contiguous range with a running aggregate; classic Two Pointers often moves pointers toward each other over sorted data with a pairwise comparison, no aggregate needed.

Common misconceptions:
- "Sliding Window only works on strings." It works identically on arrays of any element type — strings are just the most common teaching medium because character frequency maps are visually intuitive.
- "A nested while-inside-for loop is automatically O(n²)." Not if the inner loop's total iterations across the *entire* outer loop are bounded by n — always check whether the inner pointer resets or only ever advances.
- "Sliding Window can find the maximum-sum subarray with no window-size constraint." That is Kadane's Algorithm's job, not Sliding Window's — Sliding Window needs either a fixed size or an explicit optimization target (longest/shortest) with a monotonic condition.
- "Any 'find the best subarray' problem is Sliding Window." Only if the condition is monotonic in window size; otherwise reach for Prefix Sum, DP, or a different technique entirely.

## Summary

- Sliding Window computes something about every contiguous subarray/substring in O(n) by maintaining a window `[left, right]` and a running aggregate updated incrementally, instead of re-scanning each window from scratch.
- Two shapes: **fixed-size** (width given, unconditional slide) and **variable-size** (width searched for, conditional grow/shrink).
- The variable-size shape requires the tracked condition to be **monotonic** with window size — this is the check people forget before reaching for the pattern.
- Complexity: replaces O(n·k) (fixed-size brute force) or O(n²)/O(n³) (variable-size brute force) with O(n), because each index is added to the aggregate once and removed at most once.
- The most common bugs are all about the **shrink step**: using `if` instead of `while`, forgetting to undo the aggregate's update, or forgetting to erase a zero-count map entry.
- It generalizes Two Pointers to contiguous ranges, and contrasts with Prefix Sum (precompute vs. scan) and Kadane's Algorithm (no size constraint at all).
- It shows up in real systems, not just interviews: rate limiting, TCP flow control, and rolling metrics/monitoring dashboards.

## Key Takeaways

1. Sliding Window = a window `[left, right]` that only grows or shrinks, plus a running aggregate updated incrementally — never re-scan a window from scratch.
2. Two shapes: fixed-size (unconditional slide) and variable-size (conditional grow-then-shrink, longest or shortest).
3. Variable-size windows require the condition to be **monotonic** in window size — check this before reaching for the pattern.
4. Complexity win: O(n·k) → O(n) for fixed-size; O(n²)/O(n³) → O(n) for variable-size, via an amortized "each index touched a bounded number of times" argument.
5. The shrink step must be a `while`, not an `if` — a single shrink is not always enough to restore validity.
6. Every element removed from the window must have its effect on the aggregate undone — this is the single most common source of bugs.
7. "At most K" and "exactly K" are different problems; `exactly(K) = atMost(K) - atMost(K-1)` bridges them.
8. Sums with negative numbers break the shrink monotonicity assumption — use Prefix Sum + a hash map instead.
9. Sliding Window is a specialization of Two Pointers to contiguous ranges; contrast with Prefix Sum (precompute) and Kadane's (no size constraint).
10. It appears in production, not just interviews: rate limiters, TCP flow control, and rolling metrics are literal sliding windows over time or byte streams.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — for the general algorithmic foundations (amortized analysis, in particular, underlies why the variable-size window is O(n) despite the nested loop).
- *Competitive Programmer's Handbook* — Antti Laaksonen — has a concise treatment of two-pointer/sliding-window techniques as used in competitive programming.
- *Elements of Programming Interviews (in C++)* — Aziz, Lee, Prakash — includes several sliding-window-shaped array/string problems with detailed walkthroughs.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — covers the array/string pattern family this module belongs to, including several sliding-window problems.

**Open Source / GitHub Repositories**
- `kdn251/interviews` — a widely-starred collection of interview problems by pattern, including several sliding-window examples in multiple languages.
- `keon/algorithms` — a Python repository organized by algorithm/pattern, useful for cross-referencing the same sliding-window ideas in a different language.
- The LeetCode company-tagged problem lists (available via LeetCode's own site once logged in) — useful for finding which real companies ask sliding-window problems most often.

**Official Documentation**
- LeetCode — Problem 3: Longest Substring Without Repeating Characters.
- LeetCode — Problem 76: Minimum Window Substring.
- LeetCode — Problem 209: Minimum Size Subarray Sum.
- cppreference.com — `std::unordered_map` (the frequency-map aggregate used throughout this module) and `std::deque` (used in the Sliding Window Maximum bonus exercise).

**Blog Articles**
- GeeksforGeeks — "Window Sliding Technique" — the canonical explainer for the fixed-size window shape and the "Maximum Sum Subarray of Size K" problem used in `problems/01`.
- NeetCode — sliding window playlist/explainers on YouTube and neetcode.io — widely used, clear walkthroughs of the variable-size window shape on real LeetCode problems.
- Educative.io — "Grokking the Coding Interview" course's Sliding Window chapter — the original source of the "Maximum Sum Subarray of Size K" framing used as this module's first worked problem.
- AWS / Cloudflare engineering blogs on rate limiting algorithms — several public posts compare fixed-window, sliding-window, and token-bucket rate limiting; useful for connecting this pattern to the production rate-limiter use case described above.
