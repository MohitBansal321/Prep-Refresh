# Fast & Slow Pointers — Exercises

Work through these in order. Do not look at any solution; the goal is to build two reflexes: (1) writing the slow/fast pointer loop correctly without a null-dereference bug, and (2) recognizing when the "two speeds meet" idea applies to something that is not a linked list at all.

> Rule of thumb for every exercise: before you advance `fast` by two, always check **both** `fast != nullptr` and `fast->next != nullptr`. Getting this half-right is the single most common bug in this pattern — see the README's Common Mistakes section if you get stuck on a crash.

---

## Easy — Detect the Cycle's Length

Given the head of a linked list that is known to contain a cycle, write a function that returns the **length** of the cycle (the number of nodes in it), not just whether one exists.

**Requirements:**
- Use Floyd's Phase 1 to find a meeting point inside the cycle.
- From the meeting point, walk forward counting steps until you return to the same node — that count is the cycle length.
- Do not use a hash set.

**Acceptance:**
- On a list where nodes `3 -> 4 -> 5 -> 3` form the cycle, your function returns `3`.
- On a list with no cycle, define and document a sensible return value (e.g. `0` or `-1`) and justify your choice in a comment.

---

## Medium — Reorder List (LeetCode 143)

Given a linked list `L0 -> L1 -> ... -> Ln-1 -> Ln`, reorder it in place to `L0 -> Ln -> L1 -> Ln-1 -> L2 -> Ln-2 -> ...` without changing any node's `val` — only rewire `next` pointers.

**Task:**
1. Use Fast & Slow Pointers to find the middle of the list (same template as `find_middle` in `code.cpp`).
2. Reverse the second half in place (In-place Reversal — see `../in-place-reversal/`).
3. Merge the two halves by alternating nodes from each.

**Constraint:** O(n) time, O(1) extra space — no array of node pointers.

**Think about:** why does this problem, like Palindrome Linked List, require combining two different Linked List patterns rather than one? What would the code look like if you cheated and used an O(n)-space array of node pointers instead — and why is that a worse solution even though it is simpler to write?

---

## Hard — Find the Duplicate Number (LeetCode 287)

Given an array `nums` of `n + 1` integers where every integer is in the range `[1, n]` inclusive, there is exactly one repeated number (which may repeat more than once). Find it **without modifying the array** and using **O(1) extra space**.

**The reframing you must make:** there is no linked list here at all. Treat the array itself as an implicit linked list: define `next(i) = nums[i]`. Because every value is in `[1, n]` and there are `n + 1` slots, this implicit "list" is guaranteed to contain a cycle (pigeonhole principle), and the duplicate number is exactly the cycle's **entry node**.

**Task:**
1. Run Floyd's Phase 1 treating array indices/values as the "next" function, starting from index `0`.
2. Run Floyd's Phase 2 to find the cycle's entry point.
3. Return that value — it is the duplicate.

**Prove it:** test against `[1,3,4,2,2]` (expected: `2`) and `[3,1,3,4,2]` (expected: `3`).

**Think about:** why must a cycle exist at all here? Walk through the pigeonhole argument in a comment: `n+1` values, each in `[1, n]`, treated as pointers into an array of size `n+1` — why can this never terminate at a "nullptr-equivalent" the way a normal linked list can?

---

## Real-World Challenge — TTL-Based Cache Ring with Stale-Entry Detection

You are building an in-memory cache service where cache entries are stored as nodes in a **fixed-size circular buffer** (a ring), and a background "reaper" pointer walks the ring evicting expired entries. A bug report comes in: under certain configurations, the reaper's cursor and the writer's cursor can end up chasing each other indefinitely without the reaper ever "lapping" the writer to confirm the ring has wrapped fully — memory grows unbounded because stale entries are never reclaimed.

**Task:**
1. Model the ring as a circular linked list of fixed size `k`, where each node holds a cache entry and a `write_index` (or timestamp) of when it was last written.
2. Using the Fast & Slow Pointers idea, implement a health-check routine that determines whether the writer's cursor has "lapped" the reaper's cursor — i.e., whether the reaper is falling behind badly enough that it will never catch up before the writer wraps around and overwrites unreaped entries.
3. Explain, in a short write-up, how this maps onto Floyd's cycle detection: which cursor is "fast," which is "slow," and what does "meeting" mean in this context (hint: it does not mean the system is broken — figure out what it *does* mean here and why the meaning is different from classic cycle detection).
4. Propose a fix so the reaper cannot fall permanently behind (e.g. a minimum reap rate tied to the write rate), and explain the tradeoff between reaper CPU usage and staleness risk.

---

## Bonus Challenge — Three Ways to Solve "Middle of the Linked List," Compared

Implement LeetCode 876 three different ways:

1. **Fast & Slow Pointers** (the pattern this module teaches) — one pass, O(1) space.
2. **Count then walk** — one pass to count length `n`, a second pass to walk `n/2` steps.
3. **Copy into a `std::vector<ListNode*>`** and index directly into the middle — O(n) space.

For each, answer:
- How many total passes over the list does it make?
- What is its exact space complexity, and what specifically occupies that space?
- Which would you actually write in a production codebase (not an interview), and why might "count then walk" sometimes be *more readable* to a teammate even though it is not the "clever" answer?

**Then, the off-by-one twist:** modify your Fast & Slow solution so that for an **even-length** list it returns the **first** of the two middle nodes instead of the second. Explain exactly which line changes and why (this is one of the Common Mistakes called out in the README — reproduce it deliberately here so you feel the bug, then fix it correctly).

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
