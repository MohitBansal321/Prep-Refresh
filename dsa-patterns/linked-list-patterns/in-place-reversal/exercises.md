# In-place Reversal — Exercises

Work through these in order. Do not look at any solution; the goal is to build two reflexes: (1) writing the three-pointer `prev`/`curr`/`next` rewiring loop correctly, without losing the rest of the list, and (2) recognizing when a dummy head node (and, for grouped reversals, a lookahead check) is required versus when it can be skipped.

> Rule of thumb for every exercise: before you write `curr->next = prev;`, always save `ListNode* next = curr->next;` first. Getting this order backwards is the single most common bug in this pattern — see the README's Common Mistakes section if you get stuck on a list that seems to "lose" nodes.

---

## Easy — Reverse the First N Nodes Only

Given the head of a singly linked list and an integer `n` (where `1 <= n <= length`), reverse only the **first `n` nodes**, leaving the rest of the list in its original order and correctly attached.

**Requirements:**
- Use the standard three-pointer loop, bounded to exactly `n` iterations (not until `curr` hits `nullptr`).
- Reconnect the reversed portion's new tail (the original first node) to whatever followed the first `n` nodes.
- Do not use a dummy node for this one — think about why it is not actually needed here, unlike in `reverseBetween`.

**Acceptance:**
- On `[1,2,3,4,5]` with `n = 3`, your function returns `3 -> 2 -> 1 -> 4 -> 5`.
- On `[1,2,3]` with `n = 3` (the whole list), your function returns `3 -> 2 -> 1`.
- On any list with `n = 1`, your function returns the list unchanged.

---

## Medium — Reverse Alternating k-Groups (LeetCode 25 variant)

Given the head of a linked list and an integer `k`, reverse the first `k` nodes, then **leave the next `k` nodes untouched**, then reverse the following `k` nodes, and so on, alternating reverse/skip all the way to the end of the list. A group at the very end that is shorter than `k` should still follow whatever rule (reverse or skip) its position in the alternation calls for, but if it needs to be reversed and is short, reverse just the nodes that remain.

**Task:**
1. Reuse the `reverseKGroup` bookkeeping (dummy head, `groupPrev`, lookahead) from `code.cpp`, but add a boolean flag that flips after every group to decide whether to reverse or skip.
2. When skipping a group, you still need to advance `groupPrev` past it correctly — think about what "advance past an untouched group" requires compared to "advance past a just-reversed group."

**Acceptance:**
- On `[1,2,3,4,5,6,7,8]` with `k = 2`: reverse `[1,2]`, skip `[3,4]`, reverse `[5,6]`, skip `[7,8]` → `2,1,3,4,6,5,7,8`.
- On `[1,2,3,4,5]` with `k = 3`: reverse `[1,2,3]`, skip `[4,5]` (a short trailing group that happens to fall on a "skip" turn) → `3,2,1,4,5`.

**Think about:** why does this problem need you to be extra careful about the *lookahead* check interacting with the *skip* branch — specifically, does a "skip" group still need to check that a full `k` nodes are present, or does that check only matter for groups you are about to reverse?

---

## Hard — Rotate a Linked List Using Only Reversals (LeetCode 61, reversal-based variant)

Given the head of a singly linked list and an integer `k`, rotate the list to the right by `k` places — LeetCode 61's usual solution finds the new break point and re-links two pointers directly, but here you must solve it using **three whole-list-style reversals** instead, a classic array-rotation trick adapted to linked lists:

1. Reverse the entire list.
2. Reverse the first `k` nodes (of the now-reversed list).
3. Reverse the remaining `(length - k)` nodes.

**Task:**
- Implement rotation using exactly the three reversal calls above (you will need to compute the list's length first, and reduce `k` modulo the length to handle `k > length`).
- Prove to yourself on paper *why* three reversals of the right sub-ranges produce a correct rotation before you write any code — this is not obvious the first time you see it, and the point of the exercise is understanding why it works, not just getting the output to match.

**Acceptance:**
- On `[1,2,3,4,5]` with `k = 2`, the result is `4,5,1,2,3`.
- On `[1,2,3,4,5]` with `k = 7` (`k > length`), the result is the same as `k = 2` (`7 mod 5 = 2`).
- On any single-node list, the result is unchanged regardless of `k`.

---

## Real-World Challenge — Reversible Audit Log Chain with a Safety Rail

You are building an internal audit-log viewer for a compliance tool. Log entries are stored as an in-memory singly linked list (most recent entry at the head, oldest at the tail) so new entries can be prepended in O(1). A new requirement comes in: analysts need a "chronological view" button that displays entries oldest-first instead of newest-first, without the tool re-fetching or re-copying the (potentially huge) list from storage.

**Task:**
1. Implement an in-place `reverse_for_display()` that reverses the chain when "chronological view" is toggled on, and reverses it back when toggled off — using the exact three-pointer mechanism from this module, not a copy.
2. **The safety rail:** because this list is shared with a background writer that periodically prepends new audit entries, reversing it in place while a write is in progress would corrupt the structure (the writer expects to prepend at the "head," but "head" just silently changed meaning). Design a simple guard (e.g., a flag or lock) that prevents a reversal from starting while a write is in flight, and prevents a write from starting while a reversal is in flight — and explain, in a short write-up, what could go wrong for each ordering if the guard were missing.
3. Explain why an **immutable/persistent list** (see the README's "When NOT To Use" section) would sidestep this entire safety-rail problem, and what it would cost you in exchange (hint: think about what "prepend in O(1)" costs when nodes cannot be shared safely between the reversed and non-reversed views at the same time).

---

## Bonus Challenge — Three Ways to Reverse a Linked List, Compared

Implement LeetCode 206 (Reverse Linked List) three different ways:

1. **Iterative three-pointer reversal** (the pattern this module teaches) — one pass, O(1) space.
2. **Recursive reversal** — `reverse(head) = reverse(head->next)` with a fix-up step, no explicit loop.
3. **Copy into a `std::vector<int>`, reverse the vector, then overwrite each node's `val` in order** (not rearranging nodes at all — just their stored values).

For each, answer:
- What is its exact space complexity, and what specifically occupies that space (a fixed set of pointers? the call stack? a vector)?
- Which one actually rearranges the *node objects themselves*, and which one merely reshuffles *values* between fixed node positions? Why does that distinction matter if each node additionally stored, say, a `std::unique_ptr<BigPayload>` instead of a plain `int`?
- Which would you actually write in a production codebase handling lists that could have hundreds of thousands of nodes, and why?

**Then, the off-by-one twist:** modify your iterative solution so it reverses only the **first half** of an even-length list, leaving the second half in its original order and correctly connected. Explain exactly which loop bound and which reconnection line change, and why leaving out the reconnection line (even if the reversal loop itself is otherwise correct) produces a list with a dangling or wrong tail.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
