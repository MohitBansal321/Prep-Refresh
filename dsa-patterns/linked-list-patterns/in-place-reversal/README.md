# In-place Reversal


> **In one line:** walk the list once; at each node, save `next` before overwriting the pointer to point backwards, then advance both `prev` and `curr`.

```cpp
ListNode* reverseList(ListNode* head) {
  ListNode* prev = nullptr;
  ListNode* curr = head;

  while (curr != nullptr) {
    ListNode* next = curr->next;   // save before we overwrite curr->next
    curr->next = prev;             // rewire this node to point backwards
    prev = curr;
    curr = next;
  }
  return prev;   // curr is nullptr; prev now stands on the new head
}
```

**O(n)** time · **O(1)** space. Full runnable version: [code.cpp](code.cpp)

### Pick your depth

| Time | Path |
|------|------|
| **5 min** — refresher | [cheatsheet.md](cheatsheet.md) → answer its recall questions from memory |
| **20 min** — never seen this before | the code above → run [code.cpp](code.cpp) yourself → [problems/](problems/) |
| **40 min** — want the *why* | read on ↓ for the reasoning and tradeoffs, then [exercises.md](exercises.md) |

---
## Intent

Reverse a linked list — the whole thing, a sub-range of it, or consecutive groups of k nodes — by rewiring existing `next` pointers in a single pass, using O(1) extra memory instead of building a new list.

## Real Life Analogy

Picture a **conga line** of people, each person's hands on the shoulders of the person in front of them. To turn the whole line around so it faces the opposite direction, you do not pull everyone out, line up new people in reverse order, and send the originals home — that would be absurd and wasteful. Instead, you walk down the line once, and at each person, you have them let go of the shoulders in front of them and instead put their hands on the shoulders of the person **behind** them. Do this for every person, one at a time, front to back, and by the time you reach the last person, the whole line is facing the other way — same people, same physical positions relative to their neighbors, just every connection flipped.

The tricky part, and the part that trips people up the first time they try this: before you tell someone to let go of the person in front of them, you had better remember who was standing behind them — because the moment they turn around, that is now the only way you would ever find that person again. Forget to note it first, and everyone behind that point in the line is lost; you have no way back to them.

This is exactly In-place Reversal. Each node's `next` pointer is like a hand on a shoulder. You walk the list once, and at each node, you redirect its `next` pointer to point at the node *behind* it (already processed) instead of the node *ahead* of it (not yet processed) — but only after you have saved a reference to that "ahead" node, or you lose the rest of the list the instant you let go.

> **Term: In-place.** An operation that rearranges existing data using only a constant (or otherwise bounded, not proportional to input size) amount of extra memory, rather than allocating a new copy of the structure. "In-place reversal" means the *same* nodes end up reversed — no new nodes are ever allocated.

## Problem

### What engineering problem exists?

A singly linked list is a chain of nodes where each node only knows what comes **after** it — there is no `prev` pointer to walk backwards. Three closely related tasks come up constantly when working with such a list:

1. **Reverse the entire list.** Given `1 -> 2 -> 3 -> 4 -> 5`, produce `5 -> 4 -> 3 -> 2 -> 1`, using the exact same five node objects — not new ones.
2. **Reverse only a sub-range.** Given `1 -> 2 -> 3 -> 4 -> 5` and positions `left=2, right=4`, produce `1 -> 4 -> 3 -> 2 -> 5` — everything outside the range stays exactly where it was, in its original order.
3. **Reverse in fixed-size groups.** Given `1 -> 2 -> 3 -> 4 -> 5` and group size `k=2`, produce `2 -> 1 -> 4 -> 3 -> 5` — the list is chopped into consecutive groups of `k` nodes, each group internally reversed, with any leftover group shorter than `k` left untouched.

All three share the same underlying constraint: because there is no backward pointer, you cannot simply "walk to the end and read values right to left" the way you can index a `std::vector` from `arr[n-1]` down to `arr[0]`. Reversing must happen by physically changing which node each `next` pointer targets.

> **Term: Singly linked list.** A chain of nodes where each node stores a value and a single pointer, `next`, to the following node (or `nullptr` if it is the last node). There is no way to reach a node's predecessor except by having remembered it during an earlier forward traversal.

### Why is this problem difficult, and what happens if you get it wrong?

- **Reversing a pointer destroys your only way to reach what came after it.** The instant you set `curr->next = prev`, the original forward link is gone. Forgetting to save it first (see Common Mistakes) turns most of the list into unreachable memory — no crash, just a badly truncated result that might not even be noticed until much later.
- **Sub-ranges and groups need correct "seam" reconnection, and getting it wrong doesn't just look wrong — it can crash or hang.** Reversing everything is comparatively easy: the seams at the very front and back barely need attention (the new head is just whatever the reversal produces, and the new tail's `next` is simply `nullptr`). A sub-range reversal has **two internal seams** that must be reconnected exactly right — the node just *before* the range must point at the range's *new* head, and the range's *new* tail must point at whatever came *after* the range — and if the range happens to start at the list's true head, there is no "node before it" to update, which is its own special case unless you plan for it. Get either seam wrong and you can create a cycle or a dangling pointer, turning a normal, terminating list into one that hangs or crashes any code that walks it afterward.
- **k-group reversal must know, in advance, whether a full group even exists.** You cannot reverse a partial trailing group (LeetCode's convention is to leave it untouched) without first confirming, by walking ahead, that at least `k` nodes remain — which means every group requires a lookahead pass before its own reversal pass.
- **The "obvious" fix — copy every value into an array, reverse the array, rebuild — works, but wastes memory.** It costs O(n) extra space on top of the list's own memory, and in the "overwrite values in place" variant, it moves *values* between nodes rather than the nodes themselves, which is far more expensive if a node carries a large or non-copyable payload. See Why Not Other Approaches, right after the mechanism below, for exactly what this and every other alternative costs.

## Solution

The core mechanism has one moving part, repeated once per node. Walk the list with a pointer, `curr`, starting at the first node to be reversed. At each node, before doing anything else, **save `curr->next`** — call it `next` — because that is your only remaining route to the rest of the unprocessed list. Then **rewire** `curr->next` to point at `prev`, a pointer trailing one step behind `curr` that holds the already-reversed portion's new head (it starts at `nullptr`, since the very first node processed will become the new tail, and a tail's `next` must be `nullptr`). Finally **advance** both pointers: `prev` becomes `curr` (the node just rewired), and `curr` becomes `next` (the node you saved before rewiring). Repeat until there is nothing left to process; `prev` is left standing on the new head of the reversed portion.

Three things must be true simultaneously for this to work, which is why the mechanism needs exactly three pointer variables and not fewer: you need to know **where you came from** (`prev`), to rewire the current node toward it; **where you are** (`curr`), to actually perform the rewrite; and **where you were going** (`next`) — because the instant you perform the rewrite, `curr`'s own record of "where I was going" is destroyed, so that information must be captured in a fourth place *before* the rewrite happens, or it is gone forever.

**Whole-list reversal (`reverseList`):**

1. Set `prev = nullptr` and `curr = head`.
2. While `curr != nullptr`: save `next = curr->next`.
3. Rewire: `curr->next = prev`.
4. Advance: `prev = curr`, then `curr = next`. Repeat from step 2.
5. Return `prev` — it is now the head of the fully reversed list.

**Sub-range reversal `[left, right]` (`reverseBetween`)**, 1-indexed and inclusive, is the same three-pointer loop, run only across that slice, with two additional pieces of bookkeeping: remember the node **just before** the range (so it can be redirected to the range's new head once the loop finishes), and remember the range's **original first node** (which becomes its new tail, and must be redirected to whatever followed the range):

1. Create a dummy node; set `dummy.next = head`. Walk a pointer `prevRange` forward from `dummy`, `left - 1` times, so it lands on the node just before position `left`.
2. Save `rangeTail = prevRange->next` — the *original* node at position `left`, which will become the reversed range's *new tail*.
3. Run the standard reversal loop, starting with `curr = rangeTail`, for exactly `right - left + 1` iterations — not until `curr` hits `nullptr`.
4. After the loop, `prev` holds the new head of the reversed range; `curr` holds the first node after the range (or `nullptr` if `right` was the list's last position).
5. Reconnect: `prevRange->next = prev` (splice the reversed range in after `prevRange`), and `rangeTail->next = curr` (the old tail — now the new tail — points at whatever followed the range).
6. Return `dummy.next`.

Because the range might start at the list's true head, where there is no "node before it," the **dummy head node** — wired to point at the real head before anything else happens — gives you a stand-in "node before position 1" so this case needs no special branch.

**Group-of-k reversal (`reverseKGroup`)** is the sub-range idea applied repeatedly: reverse the first `k` nodes as a sub-range, reconnect, then repeat for the next `k` nodes, and so on — except each group must first be confirmed to have a full `k` nodes available before it is reversed at all, since a trailing group shorter than `k` is conventionally left untouched:

1. Create a dummy node; set `dummy.next = head`, and `groupPrev = &dummy` (the node just before the current group).
2. **Lookahead check:** walk a pointer `k` steps forward from `groupPrev`. If you run out of nodes (hit `nullptr`) before completing `k` steps, stop the whole algorithm here — the final short group is left untouched.
3. If the lookahead succeeded, save `groupStart = groupPrev->next` (the group's original first node, which becomes its new tail) and `nextGroupStart` (the k-th node's original `next`, i.e. the first node after this group).
4. Run the standard reversal loop across exactly this group's `k` nodes, seeding `prev = nextGroupStart` (so the new tail is pre-wired to the correct successor) and `curr = groupStart`.
5. The k-th node (originally the group's last node) is now the group's new head; `groupStart` is now its new tail.
6. Reconnect: `groupPrev->next` = the k-th node. Advance `groupPrev = groupStart` (the just-reversed group's new tail is the "node before" the next group). Repeat from step 2.
7. Return `dummy.next`.

## Architecture

The "participants" here are not classes — they are roles played by a small, fixed number of pointer variables as they walk the list, plus (for two of the three variants) a dummy node used purely as bookkeeping scaffolding.

1. **`prev`.** Holds the head of the already-reversed portion of the list. Starts at `nullptr` for a whole-list or range-starting reversal (because the first node processed will become a tail, whose `next` must be `nullptr`) — except inside `reverseKGroup`, where `prev` is seeded with "whatever follows this group" instead of `nullptr`, so the group's new tail is pre-wired to the correct successor the moment the loop finishes.

2. **`curr`.** The pointer actively walking forward through the still-unprocessed portion of the list. Its job each iteration is to have its own `next` pointer rewritten to point backwards, then hand off to `next`.

3. **`next` (a local, temporary variable, not a persistent role).** Exists for exactly one purpose: to hold `curr->next`'s original value across the single line of code that overwrites it. Without this variable, the loop has no way to continue past the node it just rewired.

4. **The dummy head node (`reverseBetween` and `reverseKGroup` only).** A throwaway node whose `next` is wired to point at the list's real head before any reversal begins, so "the node just before the range/group I am about to reverse" always exists as a real, dereferenceable node — even at position 1 (see Solution for why this removes a special case). It is discarded once `dummy.next` is read as the final answer.

## Why Not Other Approaches?

**"Copy every value into an array or `std::vector`, reverse that, and either overwrite the original nodes' values or build an entirely new list from it."**
Correct, and easy to reason about — most people's first instinct. But it costs **O(n) extra space**, one array slot per node in the affected range, on top of the list's own memory. The "overwrite values" variant also technically violates the spirit of most reversal problems: they usually mean "rearrange the *nodes themselves*," not "shuffle which value each node happens to hold" — and if a node carries a large payload, copying *values* is far more expensive than relinking pointers. In-place Reversal answers the exact same question with **three pointer variables** — O(1) space, regardless of list or payload size.

**"Reverse it recursively: `reverse(head) = reverse(head->next)` then fix up the last link."**
Genuinely elegant to *write*, and it reads beautifully in languages that make recursion cheap. But every recursive call adds a stack frame, and the recursion depth here is exactly the list's length — a recursive reversal of a list with a few hundred thousand nodes can blow the call stack in a way an iterative loop never would. This is the same "hidden O(n) space" trap that shows up whenever recursion processes a linear structure: the space complexity is not actually O(1) just because no explicit `std::vector` appears in the code — the call stack **is** the hidden data structure, proportional to `n`. [code.cpp](code.cpp) and every worked problem here use the **iterative** three-pointer version for every variant, deliberately, to keep the O(1) space claim honest — including for k-group reversal, where a recursive formulation is common in textbooks but pays this same hidden cost.

**"Just build a doubly linked list to begin with, so you always have a `prev` pointer."**
Sidesteps the specific problem of reversing a *singly* linked list without actually solving anything — it changes the data structure's shape rather than the algorithm, permanently pays extra memory (one more pointer per node, forever, not just during a reversal), and most real systems that hand you a singly linked list don't give you the option to redesign the input just because reversing it would be more convenient with a different shape.

**Net:** every alternative either costs O(n) extra space (array copy, new list) or hides an O(n) space cost inside the call stack (recursion) or solves a different problem than the one actually posed (redesigning the data structure). In-place Reversal is the one approach that is simultaneously O(1) space, single-pass, and works directly on the singly linked list you were actually given.

## Diagrams

- [images/recognition-diagram.md](images/recognition-diagram.md) — decision-tree flowchart: linked list? asked to reverse it, a sub-range, or groups of k? is O(1) space required? — branching to In-place Reversal vs. Fast & Slow Pointers vs. a copy-based approach, then into the three reversal variants.
- [images/flow-diagram.md](images/flow-diagram.md) — Mermaid flowchart of the core `prev`/`curr`/`next` rewiring loop, the single shape shared by all three variants.
- [images/trace-diagram.md](images/trace-diagram.md) — step-by-step trace of whole-list reversal on `1 -> 2 -> 3 -> 4 -> 5`, showing exactly where `prev`, `curr`, and the saved `next` value are positioned after each iteration, and what the partially-reversed list looks like at each step.

## The Code

The generic template in [code.cpp](code.cpp) is built around a minimal `ListNode` struct (matching the one used in `../fast-slow-pointers/code.cpp`, so the two sibling modules read consistently) and three small, reusable functions — `reverseList`, `reverseBetween`, and `reverseKGroup` — each a direct translation of the corresponding Solution section above into code. All three are iterative, not recursive, deliberately, to keep the O(1) space claim honest across every variant (see Why Not Other Approaches).

Every function shares one pair of lines that must be exactly right, and cannot be swapped:

```cpp
ListNode* next = curr->next;  // save BEFORE overwriting curr->next
curr->next = prev;
```

Save first, rewire second — reverse that order, and `curr->next` would already be pointing backwards by the time you tried to read "what comes next," permanently losing everything after that node. This exact ordering is revisited in Common Mistakes because it is, empirically, the most common bug written against this pattern.

**`struct ListNode`.** The minimal singly linked list node: an `int val` and a `ListNode* next`. Kept deliberately tiny, and identical in shape to `../fast-slow-pointers/code.cpp`'s `ListNode`, for consistency across the family.

**`build_list(values)`.** A test-data helper that builds a plain, `nullptr`-terminated list from a `std::vector<int>`, so `main()` can construct test lists without repeating boilerplate.

**`free_list(head)`.** Walks the list freeing every node with `delete`. Safe to call unconditionally here because none of this module's lists are ever cyclic (unlike `../fast-slow-pointers/code.cpp`, which must warn against calling it on one).

**`reverseList(head)`.** The whole-list template function: the three-pointer loop exactly as described in Solution, returning the new head. It is the standalone version every other variant (and `problems/01-reverse-linked-list.cpp`) builds on.

**`reverseBetween(head, left, right)`.** The sub-range template function, using a local dummy node so `left == 1` needs no special case. Backs `problems/02-reverse-linked-list-ii.cpp`.

**`reverseKGroup(head, k)`.** The k-group template function, looping over successive groups, each checked for a full `k` nodes before being reversed. Backs `problems/03-reverse-nodes-in-k-group.cpp` and, specialized to `k = 2`, `problems/04-swap-nodes-in-pairs.cpp`.

**`print_list(head, max_nodes)`.** A demo-only helper that prints up to `max_nodes` values, so `main()`'s output is easy to read against the inline "expected" comments.

**`main()`.** Exercises all three template functions against nine categories of test data: whole-list reversal on a typical list, a single-node list, and an empty list; sub-range reversal that includes the true head, one that sits strictly inside the list, and a length-1 "no-op" range; and k-group reversal where the length is an exact multiple of `k`, where a trailing short group must be left untouched, and where `k` exceeds the entire list's length. Every printed result states its expected value inline, so the file is self-checking.

**Every file in `problems/`.** Each of the four worked solutions (`01`-`04`) is intentionally **standalone** — it redefines its own `ListNode`, `build_list`, and `free_list` rather than including `code.cpp` — so any single file can be copy-pasted into a LeetCode submission box or compiled in isolation. `problems/README.md` explains why these specific four problems were chosen (the pure whole-list case, the sub-range case, the hard k-group generalization, and the k=2 special case presented as its own numbered problem).

## Tradeoffs

**What it buys you**

- **O(1) extra space**, regardless of how long the list or the reversed range is — the headline advantage over any copy-based approach.
- **Single pass** for whole-list and sub-range reversal; k-group reversal makes one lookahead pass plus one reversal pass per group, still linear overall.
- **Works directly on the given structure.** No redesign of the list, no auxiliary array, no new nodes — the exact same node objects end up in the new order, which matters when nodes carry large or non-copyable payloads.
- **One mechanism, three problems.** The identical `prev`/`curr`/`next` loop, with only its start/stop points and reconnection logic changing, answers "reverse the whole thing," "reverse a slice," and "reverse in groups" — you are learning one idea, not three.
- **Composable.** Reversing the second half of a list (found via Fast & Slow Pointers) is the standard second step in palindrome checks and list-reordering problems — see Similar Patterns below.

**What it costs you**

- **Destroys the original list structure.** After an in-place reversal, the original forward order is gone unless you explicitly reverse it back or kept a separate copy — a real problem if another part of a system still holds a reference to the list and expects the original order.
- **Readability at a glance.** Compared to "copy to an array, reverse the array, rebuild," a reviewer unfamiliar with the three-pointer idiom has to trace pointer rewrites mentally, where `std::reverse` on a vector is self-evidently correct.
- **The bookkeeping, not the core loop, is where bugs live.** Whole-list reversal's three-pointer loop is simple; correctly managing the dummy-node seams for `reverseBetween`, and the repeated lookahead-then-reverse cycle for `reverseKGroup`, is what earns LeetCode 25 its Hard rating (a bookkeeping burden, not a harder core algorithm) — see Common Mistakes for the specific traps.
- **Not safe on a list with an unknown or untrusted structure.** A reversal loop that assumes `nullptr` termination will loop forever or corrupt the structure on a cyclic list; cycle detection (`../fast-slow-pointers/`) is a separate, prerequisite concern this pattern does not handle.

This O(n) → O(1) space reduction, at no cost to the O(n) time complexity, is exactly why In-place Reversal is the textbook answer whenever a linked-list reversal problem explicitly calls out an O(1) space constraint.

## Complexity

**Time:** O(n) for whole-list and sub-range reversal — every node in the affected portion is visited exactly once. O(n) for k-group reversal as well: each node is visited once during its group's lookahead check and once during its group's reversal pass, still a constant number of visits per node, not proportional to the number of groups.

**Space:** **O(1)** for all three variants — a small, fixed number of pointer variables (plus one dummy node on the stack for `reverseBetween` and `reverseKGroup`), regardless of list length.

**Space — replaced approach (copy into an array/new list):** **O(n)** — one array slot or new node per element of the affected range.

**Space — replaced approach (recursive whole-list or k-group reversal):** technically **O(n)** (or O(n/k) for the grouped case) due to call-stack depth, even though no explicit array appears in the code — a subtlety worth remembering when asked to justify an "O(1) space" claim for a recursive solution.

## Common Mistakes

- **Losing the rest of the list by rewiring before saving `next`.** Writing `curr->next = prev;` before capturing the original `curr->next` in a temporary variable overwrites the only remaining reference to the unprocessed tail of the list. *Why it happens:* the two lines look almost interchangeable if you have not internalized *why* the order matters — nothing crashes immediately, but the loop can no longer advance past that node correctly, and most of the list becomes unreachable. *Avoid it:* always write `ListNode* next = curr->next;` as the very first line inside the loop body, before touching `curr->next` at all, as a fixed idiom you type the same way every time.

- **Off-by-one on sub-range boundaries.** Getting `reverseBetween`'s loop count wrong (running it `right - left` times instead of `right - left + 1`, or walking `prevRange` `left` times instead of `left - 1`) reverses one node too many or too few, or leaves `prevRange` pointing at the wrong node entirely. *Why it happens:* 1-indexed, inclusive ranges are notoriously easy to miscount by one in either direction, especially when the loop bound is expressed as a subtraction. *Avoid it:* test explicitly against a small list where `left` and `right` are adjacent (`left = 2, right = 3`) and a case where `left == right` (a single-node "range," which must be a true no-op) before trusting the loop bound.

- **Forgetting a dummy head node when the reversal might include the true head.** Without a dummy node, `reverseBetween(head, 1, k)` and every group inside `reverseKGroup` need a special-case branch for "is this the very first node of the list?" because there is no real "node before position 1" to update. *Why it happens:* whole-list reversal (`reverseList`) never needs this, so it is easy to under-generalize from that simpler case and assume "the node before the range" always exists. *Avoid it:* reach for a dummy node (`ListNode dummy(0); dummy.next = head;`) by default any time a reversal might start at the true head, and return `dummy.next` instead of `head` at the end.

- **Reversing a trailing partial group in `reverseKGroup`.** Skipping the lookahead check (or getting it wrong) and reversing whatever nodes remain, even when fewer than `k` are left, silently violates the problem's own convention (LeetCode 25 requires the final short group to stay untouched) and produces a wrong-but-plausible-looking answer. *Why it happens:* it is tempting to just "reverse everything you can reach" without first counting how many nodes are actually available. *Avoid it:* always walk `k` steps ahead first and check for `nullptr` before reversing a single node of the group; if the walk runs out early, stop the whole algorithm there.

- **Confusing "reverse a sub-range" with "reverse the whole list starting partway through."** A sub-range reversal must reconnect its new tail to whatever followed the range — if you reverse from `left` to the end of the list as though `right` did not exist, you silently drop the "leave everything after `right` untouched" requirement. *Why it happens:* whole-list reversal's success (no reconnection needed at the tail — it just becomes `nullptr`) can make it easy to forget that a bounded range has a second seam to close, not just one. *Avoid it:* explicitly track and use `nextGroupStart` / the node after the range, and write the reconnection line as its own step, not as an afterthought.

## When To Use

- **Reversing an entire singly linked list** under an O(1) space constraint (LeetCode 206).
- **Reversing a specific `[left, right]` sub-range** while leaving the rest of the list untouched (LeetCode 92).
- **Reversing a list in fixed-size groups**, with a defined convention for a trailing short group (LeetCode 25; LeetCode 24 as the k=2 special case).
- **As the second phase of a two-pattern composition** — after Fast & Slow Pointers finds the middle of a list, reversing the second half is the standard next step in palindrome checks (LeetCode 234) and list-reordering problems (LeetCode 143).
- **Anywhere a real system holds a singly linked chain of objects and must physically reverse their traversal order** without allocating new nodes — e.g., replaying an undo/redo history chain in the opposite direction, or reversing a chain of pending job handlers.

## When NOT To Use

- **When the original list order must be preserved elsewhere.** In-place Reversal mutates the existing nodes' `next` pointers; if another part of a system holds a reference to the same nodes and expects to traverse them in their original order, reversing in place silently breaks that other consumer. Either reverse a copy, or reverse and then reverse back once you are done using the reversed order (as `problems/04` in `../fast-slow-pointers/` does for its palindrome check).
- **When working with an immutable or persistent list structure.** Some functional-programming-style linked structures are deliberately immutable — every "modification" produces a new structure sharing unmodified tails with the old one, specifically so existing references stay valid. Rewiring `next` pointers in place is not a meaningful operation on such a structure at all; you would build a new reversed structure instead, and the O(1)-space argument no longer applies in the same way.
- **When you only need to detect a cycle, find a middle, or check for a repeated value.** Those are Fast & Slow Pointers' job (`../fast-slow-pointers/`) — reversing anything is unnecessary work if the actual question is a yes/no or "where" question about the list's shape, not a request to change it.
- **When the list might be cyclic and you have not verified it isn't.** A reversal loop that assumes `nullptr` termination will loop forever (or corrupt the structure) on a cyclic list; run a cycle check first if the list's origin is untrusted.

## Where This Shows Up

LeetCode 206 (Reverse Linked List) is one of the most frequently asked "warm-up" linked-list questions across nearly every company running standard DSA-style interview loops, precisely because it has a clean O(n) time / O(1) space answer that is easy to state but easy to get subtly wrong (the save-before-rewrite ordering). LeetCode 92 and 25 are common follow-ups used to test whether a candidate can generalize the same idea under added bookkeeping constraints, with 25 specifically used as a discriminator between "knows the loop" and "can manage the seams correctly under pressure."

In production systems:

- **Undo/redo and command-history chains.** Some editor and application undo systems model history as a singly linked chain of command objects; reversing the direction of traversal (oldest-to-newest vs. newest-to-oldest) without rebuilding the chain — or reversing just the affected sub-range when a batch of queued jobs needs to run in reverse priority order — is a direct, if less commonly discussed, application of this exact rewiring idea.
- **Middleware, interceptor, and job-scheduling pipelines.** A pipeline modeled as a singly linked chain of handler nodes occasionally needs its execution order reversed (e.g. request handlers run forward, response handlers run in reverse); reusing the same `prev`/`curr`/`next` rewiring — via whole-list or k-group reversal — avoids maintaining two separately-ordered chains.
- **Compiler/interpreter intermediate representations.** Some IR data structures represent instruction sequences as singly linked lists of instruction nodes; certain optimization or code-generation passes reverse a basic block's instruction order in place rather than allocating a new list, for the same memory-locality and allocation-avoidance reasons this pattern exists.
- **Log-replay and event-sourcing presentation.** A tool that needs to present insertion-ordered events oldest-last instead of oldest-first can reverse the linked event chain in place, rather than materializing a second array purely to flip the presentation order.
- **Batch data-transformation utilities.** A "reverse every k records" utility — e.g. normalizing a chain of paginated result nodes fetched in reversed page batches from an upstream service — applies `reverseKGroup`'s exact bookkeeping to stitch reversed batches back into one coherent chain.

## Similar Patterns

- **Fast & Slow Pointers** (`../fast-slow-pointers/`, sibling folder in this repo): answers a "yes/no" or "where" question about a list's shape (does it cycle? where is the middle?) without changing anything, using two pointers moving at different speeds. Different goal (answer a question about the list vs. restructure it), but the two combine constantly: **Palindrome Linked List** (`../fast-slow-pointers/problems/04-palindrome-linked-list.cpp`) is the textbook example — Fast & Slow Pointers finds the middle of the list first, in one pass, and then In-place Reversal reverses the second half so it can be walked backwards (by walking it forwards, since it is now reversed) and compared against the first half, all without allocating any extra memory.
- **Two Pointers (converging, on arrays):** a different family (`../../array-string-patterns/two-pointers/`) that reverses (or checks) a random-access array by swapping elements at two indices moving toward each other from opposite ends. The *goal* (reverse a sequence) can look similar, but the *mechanism* is entirely different: an array has O(1) indexing, so reversing it needs only index swaps, no pointer rewiring, and no dummy nodes — the entire "seam" and "lookahead" bookkeeping this module discusses is specific to sequential-access structures that lack indexing.
- **Recursion (whole-list or k-group reversal, written recursively):** produces the identical result as the iterative version for whole-list reversal, and a workable (if trickier) result for k-group reversal, but trades the O(1) space guarantee for O(n) (or O(n/k)) call-stack depth — see Why Not Other Approaches.

| Pattern | Structure it needs | Pointer movement | Primary question answered | Space |
|---------|--------------------|--------------------|-----------------------------|-------|
| In-place Reversal | Sequential access (singly linked list) | Rewires `next`, walking forward once (or once per group) | "Reverse this list / sub-range / groups of k" | O(1) |
| Fast & Slow Pointers | Sequential access (linked list, or function-generated sequence) | Same direction, different speeds (1x / 2x), no rewiring | "Is there a cycle?" / "Where is the middle?" | O(1) |
| Two Pointers (converging, on arrays) | Random access, indexable (array) | Opposite ends, swapping elements, moving toward each other | "Reverse / palindrome-check / partition an array" | O(1) |
| Recursive reversal | Sequential access (linked list) | Recursion unwinds the fix-up, from tail back to head | Same as In-place Reversal, written recursively | O(n) call stack |

## Interview Discussion

Experienced engineers rarely dwell on "can you reverse a linked list" in isolation — that specific loop is expected to be fast, correct, and typed without hesitation. The conversation that actually distinguishes candidates centers on the **variants and their bookkeeping**, and on **why an iterative approach is preferred**.

- *"Now do it for just a sub-range `[left, right]`. Now do it in groups of k."* The standard follow-up progression — it is not retesting the core loop, which is assumed correct by now. It tests whether a candidate reaches for a dummy head node proactively (rather than discovering the need for it after their first attempt breaks on `left == 1`), and whether they remember the lookahead-before-reversing check once groups enter the picture.
- *"Can you do the whole-list version recursively? What's the catch?"* A strong answer produces the elegant recursive version quickly, then proactively names the O(n) call-stack cost as the catch — showing they understand *why* the iterative version is generally preferred in production code, not just that "recursion is different."
- *"What if the list turns out to be cyclic — what happens to your reversal loop?"* Tests whether the candidate connects this pattern to its sibling (Fast & Slow Pointers): a reversal loop written to assume `nullptr` termination will misbehave (infinite loop or corrupted structure) on cyclic input, so a list from an untrusted source should be checked for cycles first.

Common misconceptions:
- "Reversing a linked list is just one algorithm." It is one *mechanism* (the three-pointer rewiring loop) applied to three distinct problems (whole list, sub-range, k-group), each with its own seam-reconnection bookkeeping.
- "A recursive solution is just as good if it produces the right output." Correct output is necessary but not sufficient — the recursive version's hidden call-stack space cost is a real production concern on long lists, and claiming "O(1) space" for it is inaccurate.
- "The k-group version is basically the sub-range version, so it must be similarly easy." The *core* rewiring is the same, but managing it correctly across an unknown number of repeated groups, each requiring its own lookahead check, is exactly why LeetCode rates 25 as Hard while rating 92 as only Medium.

## Key Takeaways

1. In-place Reversal rewires `next` pointers using three pointer variables (`prev`, `curr`, a saved `next`) to reverse a list — or part of one — in O(1) extra space.
2. The one line that must never be reordered: save `curr->next` into a temporary *before* overwriting `curr->next` to point backwards.
3. Whole-list reversal (LeetCode 206) is the pattern in its simplest form: no dummy node, no lookahead, just the core loop start to finish.
4. Sub-range reversal (LeetCode 92) needs a dummy head node so a range starting at the true head (`left == 1`) needs no special-case branch.
5. k-group reversal (LeetCode 25) repeats sub-range reversal group by group, with a mandatory lookahead check per group to correctly skip a trailing short group.
6. LeetCode 24 (Swap Nodes in Pairs) is exactly k-group reversal with `k` fixed at 2 — recognizing "this is a special case of a pattern I already know" is itself an interview-relevant skill.
7. A recursive formulation exists for every variant and can look elegant, but costs O(n) (or O(n/k)) call-stack space — not truly O(1), despite appearances.
8. In-place Reversal genuinely mutates the caller's nodes; if the original order must survive elsewhere, reverse a copy, or reverse-then-restore once done (as the Palindrome Linked List solution does).
9. It composes with **Fast & Slow Pointers**: find the middle, then reverse the second half — the standard approach behind palindrome checks and list-reordering problems.
10. If the list's origin is untrusted, verify it is not cyclic before reversing — a reversal loop written for a normal, terminating list will misbehave on cyclic input.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — background on linked data structures and in-place algorithm design that underpins this whole pattern.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — has a dedicated linked-list chapter covering reversal and related in-place pointer rewiring techniques with C++ code.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — the linked-list chapter's coverage of in-place manipulation is a widely used introduction to this exact family of techniques.
- *Structure and Interpretation of Computer Programs* — Abelson & Sussman — background on the tradeoffs between mutable, in-place data structures and immutable/persistent ones, directly relevant to the "When NOT To Use" discussion of persistent list structures.

**Open Source / GitHub Repositories**
- `keon/algorithms` — a well-known open-source collection of algorithm implementations in Python, including linked-list reversal implementations useful for cross-checking against this module's C++.
- LeetCode's own official solutions repository style discussions (see the "Solution" tab on each problem page referenced below) for community-vetted alternative implementations of the same problems worked here.

**Official Documentation / Problem Pages**
- LeetCode 206 — Reverse Linked List.
- LeetCode 92 — Reverse Linked List II.
- LeetCode 25 — Reverse Nodes in k-Group.
- LeetCode 24 — Swap Nodes in Pairs.
- LeetCode 234 — Palindrome Linked List (the composition with Fast & Slow Pointers).
- LeetCode 143 — Reorder List (another composition with Fast & Slow Pointers).

**Blog Articles**
- GeeksforGeeks — "Reverse a linked list" — a widely referenced explainer covering both iterative and recursive approaches, with a discussion of their respective complexity.
- NeetCode — video/explainer content walking through LeetCode 206, 92, and 25 using the same three-pointer technique, useful as a visual companion to this README's diagrams.
- Educative.io — "Grokking the Coding Interview" pattern write-up on In-place Reversal of a Linked List — the pattern-based framing this whole repo's philosophy is inspired by.
