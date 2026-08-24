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

### Why is this problem difficult?

- **Reversing a pointer destroys your only way to reach what came after it.** The instant you set `curr->next = prev`, the original forward link — the only route from `curr` to the rest of the unprocessed list — is gone. If you did not save it first, that part of the list is permanently unreachable (a memory leak at best; silently wrong output at worst).
- **Sub-ranges and groups need correct "seam" reconnection.** Reversing everything is comparatively easy — the seam at the very front and back of the list barely needs attention (the new head just becomes whatever the reversal produces, and the new tail's `next` is simply `nullptr`). A sub-range reversal has **two internal seams** that must be reconnected exactly right: the node just *before* the range must now point at the range's *new* head, and the range's *new* tail must point at whatever came *after* the range — and if the range happens to start at the list's true head, there is no "node before it" to update, which is its own special case unless you plan for it.
- **k-group reversal must know, in advance, whether a full group even exists.** You cannot reverse a partial trailing group (LeetCode's convention is to leave it untouched) without first confirming, by walking ahead, that at least `k` nodes remain — which means every group requires a lookahead pass before its own reversal pass.

### What happens if we ignore it?

- **Losing the rest of the list.** Forgetting to save `curr->next` before overwriting it turns most of the list into unreachable memory — no crash, just a badly truncated result that might not even be noticed until much later (see Common Mistakes).
- **A crash or infinite loop from broken seams.** Get the sub-range or k-group reconnection wrong, and you can create a cycle (node A points to node B which, through the broken rewiring, eventually points back to node A), or a dangling pointer into memory that has already been overwritten — either one turns a normal, terminating list into a structure that hangs or crashes any code that walks it afterward.
- **Wasted memory if you reach for the "obvious" fix.** The natural first instinct — copy every value into a `std::vector<int>`, reverse the vector, then build a brand-new linked list (or new sub-list) from the reversed values — works correctly, but costs **O(n) extra space** for the copy, on top of the memory the original list already occupies. For a sub-range or k-group problem, you would need to be careful to only copy the affected slice, but the fundamental waste remains: you are paying for a second data structure to solve a problem that can be solved by rearranging the first one.

## Why Not Other Approaches

**"Copy every value into an array or `std::vector`, reverse that, and either overwrite the original nodes' values or build an entirely new list from it."**
This is correct and easy to reason about, which is exactly why it is most people's first instinct. But it costs **O(n) extra space** — one array slot per node in the affected range — in addition to the list's own memory. It also, in the "overwrite values" variant, technically violates the spirit of most linked-list reversal problems: interviewers (and, more importantly, real production code moving actual objects rather than copyable primitives) usually mean "rearrange the *nodes themselves*," not "keep the same nodes but shuffle which value each one happens to hold." If a node carries a large payload (a struct with several fields, or a pointer to an even larger object), copying *values* between nodes is far more expensive than just relinking pointers. In-place Reversal answers the exact same question with **three pointer variables** — O(1) space — regardless of how large the list or its payload is.

**"Reverse it recursively: `reverse(head) = reverse(head->next)` then fix up the last link."**
This is a genuinely elegant way to *write* whole-list reversal, and it reads beautifully in languages that make recursion cheap. But every recursive call adds a stack frame, and the recursion depth here is exactly the list's length — so a recursive reversal of a list with a few hundred thousand nodes can blow the call stack in a way an iterative loop never would. This is the same "hidden O(n) space" trap that shows up whenever recursion is used to process a linear structure: the space complexity analysis is not actually O(1) just because there is no explicit `std::vector` in the code — the call stack **is** the hidden data structure, and it is proportional to `n`. This module's `code.cpp` and all worked problems intentionally use the **iterative** three-pointer version for every variant, precisely to keep the O(1) space claim honest, including for k-group reversal, where a recursive formulation is common in textbooks but pays this same hidden cost.

**"Just build a doubly linked list to begin with, so you always have a `prev` pointer."**
This sidesteps the specific problem of reversing a *singly* linked list, but it does not actually solve anything — it changes the data structure's shape rather than the algorithm, permanently pays extra memory (one more pointer per node, forever, not just during a reversal), and most problems (and most real systems that hand you a singly linked list from elsewhere) do not give you the option to redesign the input structure just because reversing it would be more convenient with a different one.

**Tradeoff summary:** every alternative either costs O(n) extra space (array copy, new list) or hides an O(n) space cost inside the call stack (recursion) or solves a different problem than the one actually posed (redesigning the data structure). In-place Reversal is the one approach that is simultaneously O(1) space, single-pass, and works directly on the singly linked list you were actually given — which is exactly why it is the expected answer whenever a linked-list problem says "reverse" and "O(1) space" in the same sentence.

## Solution

The core mechanism has one moving part, repeated once per node:

Walk the list with a pointer, `curr`, starting at the first node to be reversed. At each node, before doing anything else, **save `curr->next`** — call it `next` — because that is your only remaining route to the rest of the unprocessed list. Then **rewire** `curr->next` to point at `prev`, a pointer trailing one step behind `curr` that holds the already-reversed portion's new head (it starts at `nullptr`, since the very first node processed will become the new tail, and a tail's `next` must be `nullptr`). Finally, **advance** both pointers: `prev` becomes `curr` (the node just rewired), and `curr` becomes `next` (the node you saved before rewiring). Repeat until there is nothing left to process; `prev` is left standing on the new head of the reversed portion.

Three things must be true simultaneously for this to work, which is why the mechanism needs exactly three pointer variables and not fewer:

- You need to know **where you came from** (`prev`), to rewire the current node toward it.
- You need to know **where you are** (`curr`), to actually perform the rewrite.
- You need to know **where you were going** (`next`), because the instant you perform the rewrite, `curr`'s own record of "where I was going" is destroyed — so that information must be captured in a fourth place *before* the rewrite happens, or it is gone forever.

Reversing a **sub-range** `[left, right]` is the same three-pointer loop, run only across that slice, with two additional pieces of bookkeeping: you must remember the node **just before** the range (so it can be redirected to the range's new head once the loop finishes), and you must remember the range's **original first node** (which becomes its new tail, and must be redirected to whatever followed the range). Because the range might start at the list's true head — in which case there is no "node before it" — a **dummy head node**, wired to point at the real head before anything else happens, gives you a stand-in "node before position 1" so this case needs no special branch.

Reversing in **groups of k** is the sub-range idea applied repeatedly: reverse the first `k` nodes as a sub-range, reconnect, then repeat for the next `k` nodes, and so on — except each group must first be confirmed to have a full `k` nodes available (by walking ahead and counting) before it is reversed at all, since a trailing group shorter than `k` is conventionally left untouched rather than reversed.

## Architecture

The "participants" here are not classes — they are roles played by a small, fixed number of pointer variables as they walk the list, plus (for two of the three variants) a dummy node used purely as bookkeeping scaffolding.

1. **`prev`.** Holds the head of the already-reversed portion of the list. Starts at `nullptr` for a whole-list or range-starting reversal (because the first node processed will become a tail, whose `next` must be `nullptr`) — except inside `reverseKGroup`, where `prev` is seeded with "whatever follows this group" instead of `nullptr`, so the group's new tail is pre-wired to the correct successor the moment the loop finishes.

2. **`curr`.** The pointer actively walking forward through the still-unprocessed portion of the list. Its job each iteration is to have its own `next` pointer rewritten to point backwards, then hand off to `next`.

3. **`next` (a local, temporary variable, not a persistent role).** Exists for exactly one purpose: to hold `curr->next`'s original value across the single line of code that overwrites it. Without this variable, the loop has no way to continue past the node it just rewired.

4. **The dummy head node (`reverseBetween` and `reverseKGroup` only).** A throwaway node whose `next` is wired to point at the list's real head before any reversal begins. It exists purely so that "the node just before the range/group I am about to reverse" always exists as a real, dereferenceable node — even when the range or group starts at position 1 — removing the need for an `if (left == 1)`-style special case. It is discarded (never part of the returned list) once `dummy.next` is read as the final answer.

Responsibilities in one line each: **`prev`** remembers the reversed-so-far result; **`curr`** is the node currently being flipped; **`next`** is the one-line lifeline back to the rest of the list; the **dummy head** turns "is this the true head?" from a special case into a non-issue.

## Execution Flow

**1. Whole-list reversal (`reverseList`):**

1. Set `prev = nullptr` and `curr = head`.
2. While `curr != nullptr`: save `next = curr->next`.
3. Rewire: `curr->next = prev`.
4. Advance: `prev = curr`, then `curr = next`.
5. Repeat from step 2 until `curr` is `nullptr`.
6. Return `prev` — it is now the head of the fully reversed list.

**2. Sub-range reversal `[left, right]` (`reverseBetween`), 1-indexed and inclusive:**

1. Create a dummy node; set `dummy.next = head`.
2. Walk a pointer `prevRange` forward from `dummy`, `left - 1` times, so it lands on the node just before position `left`.
3. Save `rangeTail = prevRange->next` — this is the *original* node at position `left`, which will become the reversed range's *new tail*.
4. Run the standard `prev`/`curr`/`next` reversal loop (steps 2-4 above) starting with `curr = rangeTail`, but only for exactly `(right - left + 1)` iterations — not until `curr` hits `nullptr`.
5. After the loop, `prev` holds the new head of the reversed range (the *original* node at position `right`), and `curr` holds the first node *after* the range (or `nullptr` if `right` was the list's last position).
6. Reconnect: `prevRange->next = prev` (splice the reversed range in after `prevRange`), and `rangeTail->next = curr` (the old tail's node — now the new tail — points at whatever followed the range).
7. Return `dummy.next`.

**3. k-group reversal (`reverseKGroup`):**

1. Create a dummy node; set `dummy.next = head`. Set `groupPrev = &dummy` (the node just before the current group).
2. **Lookahead check:** walk a pointer `k` steps forward from `groupPrev`. If you run out of nodes (hit `nullptr`) before completing `k` steps, stop the whole algorithm here — fewer than `k` nodes remain, and that final short group is left untouched.
3. If the lookahead succeeded, that k-th node is the current group's original last node. Save `groupStart = groupPrev->next` (the group's original first node, which becomes its new tail) and `nextGroupStart` (the k-th node's original `next`, i.e. the first node of whatever comes after this group).
4. Run the standard reversal loop across exactly this group's `k` nodes, seeding `prev = nextGroupStart` (so the group's new tail is pre-wired to the correct successor) and `curr = groupStart`.
5. After the loop, the k-th node (originally the group's last node) is now the group's new head, and `groupStart` is now the group's new tail.
6. Reconnect: `groupPrev->next` = the k-th node (the group's new head).
7. Advance `groupPrev = groupStart` (the just-reversed group's new tail is the "node before" the next group).
8. Repeat from step 2 for the next group.
9. Return `dummy.next`.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full decision-tree flowchart (linked list? asked to reverse it, a sub-range, or groups of k? is O(1) space required? — branching to In-place Reversal vs. Fast & Slow Pointers vs. a copy-based approach, then into the three reversal variants).

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the Mermaid flowchart of the core `prev`/`curr`/`next` rewiring loop — the single shape shared by all three variants of this pattern.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of whole-list reversal on a concrete example list, `1 -> 2 -> 3 -> 4 -> 5`, showing exactly where `prev`, `curr`, and the saved `next` value are positioned after each iteration, and what the partially-reversed list looks like at each step.

## Implementation

The generic template in [code.cpp](code.cpp) is built around a minimal `ListNode` struct (matching the one used in `../fast-slow-pointers/code.cpp`, so the two sibling modules read consistently) and three small, reusable functions — `reverseList`, `reverseBetween`, and `reverseKGroup` — each one a direct translation of the corresponding Execution Flow section above into code.

All three functions are iterative, not recursive — deliberately, to keep the O(1) space claim honest across every variant, including k-group reversal, where a recursive formulation is common in textbooks but pays a hidden O(n/k) call-stack cost (see Why Not Other Approaches).

Before reading the code, notice the one line every function shares and must get exactly right:

```cpp
ListNode* next = curr->next;  // save BEFORE overwriting curr->next
curr->next = prev;
```

Both lines are load-bearing, and their order cannot be swapped. Save first, rewire second — reverse that order, and `curr->next` would already be pointing backwards by the time you tried to read "what comes next," permanently losing everything after that node. This exact ordering is revisited in Common Mistakes because it is, empirically, the most common bug written against this pattern.

## Code Walkthrough

See [code.cpp](code.cpp) for the full runnable file. Here is what each part does and why it exists.

**`struct ListNode`.** The minimal singly linked list node: an `int val` and a `ListNode* next`. Kept deliberately tiny so the pattern's mechanics are not obscured by unrelated fields, and kept identical in shape to `../fast-slow-pointers/code.cpp`'s `ListNode` for consistency across the family.

**`build_list(values)`.** A test-data helper that builds a plain, `nullptr`-terminated list from a `std::vector<int>`. Exists so `main()` can construct test lists without repeating boilerplate.

**`free_list(head)`.** Walks the list freeing every node with `delete`. Exists to avoid memory leaks in the demo; safe to call unconditionally here because none of this module's lists are ever cyclic (unlike `../fast-slow-pointers/code.cpp`, which must warn against calling it on a cyclic list).

**`reverseList(head)`.** The whole-list template function: implements the three-pointer loop exactly as described in Execution Flow §1, returning the new head. It exists as the standalone, reusable version of the reversal every other variant in this file (and `problems/01-reverse-linked-list.cpp`) builds on.

**`reverseBetween(head, left, right)`.** The sub-range template function: implements Execution Flow §2, using a local dummy node so `left == 1` needs no special case. It exists as the reusable building block behind `problems/02-reverse-linked-list-ii.cpp`.

**`reverseKGroup(head, k)`.** The k-group template function: implements Execution Flow §3, looping over successive groups, each one checked for a full `k` nodes before being reversed. It exists as the reusable building block behind `problems/03-reverse-nodes-in-k-group.cpp` and, specialized to `k = 2`, `problems/04-swap-nodes-in-pairs.cpp`.

**`print_list(head, max_nodes)`.** A demo-only helper that prints up to `max_nodes` values, used so `main()`'s output is easy to read and compare against the inline "expected" comments.

**`main()`.** Exercises all three template functions against nine categories of test data: whole-list reversal on a typical list, a single-node list, and an empty list; sub-range reversal that includes the true head, one that sits strictly inside the list, and a length-1 "no-op" range; and k-group reversal where the length is an exact multiple of `k`, where a trailing short group must be left untouched, and where `k` exceeds the entire list's length. Every printed result states its expected value inline so the file is self-checking when you read its output.

**Every file in `problems/`.** Each of the four worked solutions (`01`-`04`) is intentionally **standalone** — it redefines its own `ListNode`, `build_list`, and `free_list` rather than including `code.cpp` — so that any single file can be copy-pasted into a LeetCode submission box or compiled in isolation without pulling in the rest of this folder. `problems/README.md` explains why these specific four problems were chosen (the pure whole-list case, the sub-range case, the hard k-group generalization, and the k=2 special case presented as its own numbered problem).

## Advantages

- **O(1) extra space**, regardless of how long the list or the reversed range is — the headline advantage over any copy-based approach.
- **Single pass** for whole-list and sub-range reversal; k-group reversal makes one lookahead pass plus one reversal pass per group, still linear overall.
- **Works directly on the given structure.** No redesign of the list, no auxiliary array, no new nodes — the exact same node objects end up in the new order, which matters when nodes carry large or non-copyable payloads.
- **One mechanism, three problems.** The identical `prev`/`curr`/`next` loop, with only its start/stop points and reconnection logic changing, answers "reverse the whole thing," "reverse a slice," and "reverse in groups" — you are learning one idea, not three.
- **Composable with other patterns.** Reversing the second half of a list (found via Fast & Slow Pointers) is the standard second step in palindrome checks and list-reordering problems — see Similar Patterns below.

## Disadvantages

- **Destroys the original list structure.** After an in-place reversal, the original forward order is gone unless you explicitly reverse it back (or kept a separate copy) — this is a real problem if some other part of a system still holds a reference to the list and expects the original order.
- **Iterative whole-list reversal is easy; the other two variants are fiddly.** The three-pointer loop itself is simple, but correctly bookkeeping the dummy-node seams for `reverseBetween` and, especially, the repeated lookahead-then-reverse cycle for `reverseKGroup`, is where real bugs live. The Hard difficulty rating on LeetCode 25 reflects this bookkeeping burden, not a harder core algorithm.
- **Off-by-one traps are numerous and silent.** Getting a sub-range boundary or a group-size lookahead wrong typically does not crash — it just reverses one node too many, one too few, or reverses a partial trailing group that should have been left alone. See Common Mistakes.
- **Not safe on a list with an unknown or untrusted structure.** If a list might be cyclic (see `../fast-slow-pointers/`), reversing it with a loop that assumes `nullptr` termination will loop forever or corrupt the structure; cycle detection is a separate, prerequisite concern this pattern does not handle.

## Tradeoffs

**What we gain:** O(1) space instead of O(n) for a copy-based rebuild, working directly on the caller's node objects (no copying of potentially expensive payloads), and a single unifying mechanism for three different-looking problems.

**What we lose:** readability at a glance, compared to "copy to an array, reverse the array, rebuild" — a reviewer unfamiliar with the three-pointer idiom has to trace pointer rewrites mentally, whereas `std::reverse` on a vector is self-evidently correct. We also give up the *original* list's structure permanently (unless we deliberately reverse it back), which matters if something else in the system still expects to read it in its original order.

## Complexity

**Time:** O(n) for whole-list and sub-range reversal — every node in the affected portion is visited exactly once. O(n) for k-group reversal as well: each node is visited once during its group's lookahead check and once during its group's reversal pass, which is still a constant number of visits per node, not proportional to the number of groups.

**Space:** **O(1)** for all three variants — a small, fixed number of pointer variables (plus one dummy node on the stack for `reverseBetween` and `reverseKGroup`), regardless of list length.

**Space — replaced approach (copy into an array/new list):** **O(n)** — one array slot or new node per element of the affected range.

**Space — replaced approach (recursive whole-list or k-group reversal):** technically **O(n)** (or O(n/k) for the grouped case) due to call-stack depth, even though no explicit array appears in the code — a subtlety worth remembering when asked to justify an "O(1) space" claim for a recursive solution.

This O(n) → O(1) space reduction, at no cost to the O(n) time complexity, is exactly why In-place Reversal is the textbook answer whenever a linked-list reversal problem explicitly calls out an O(1) space constraint.

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

## Real Interview/Production Examples

- **Interviews:** LeetCode 206 (Reverse Linked List) is one of the most frequently asked "warm-up" linked-list questions across nearly every company running standard DSA-style interview loops, precisely because it has a clean O(n) time / O(1) space answer that is easy to state but easy to get subtly wrong (the save-before-rewrite ordering). LeetCode 92 and 25 are common follow-ups used to test whether a candidate can generalize the same idea under added bookkeeping constraints, with 25 specifically used as a discriminator between "knows the loop" and "can manage the seams correctly under pressure."
- **Undo/redo and command-history chains.** Some editor and application undo systems model history as a singly linked chain of command objects; reversing the direction of traversal (to walk from oldest to newest instead of newest to oldest, or vice versa) without rebuilding the chain is a direct, if less commonly discussed, application of this exact rewiring idea.
- **Reversing a chain of middleware/interceptor handlers.** A pipeline modeled as a singly linked chain of handler nodes (each holding a reference to "the next handler to invoke") occasionally needs its execution order reversed (e.g., request handlers run forward, response handlers run in reverse) — reusing the same `prev`/`curr`/`next` rewiring rather than maintaining two separately-ordered chains.
- **Compiler/interpreter intermediate representations.** Some IR data structures represent instruction sequences as singly linked lists of instruction nodes; certain optimization or code-generation passes reverse a basic block's instruction order in place rather than allocating a new list, for the same memory-locality and allocation-avoidance reasons this pattern exists.

## Where I Can Use This

Five realistic ideas for your own backend or systems projects:

1. **An undo-history chain modeled as a singly linked list of command objects**: reverse it in place (rather than rebuilding) when you need to replay history in the opposite direction, e.g., generating a "redo from scratch" sequence, without allocating a second history structure.
2. **A pending-jobs queue implemented as a singly linked list** where a batch of jobs needs to run in reverse priority order under specific conditions: reverse the affected sub-range of the queue in place instead of dequeuing into an array and re-enqueuing.
3. **A middleware/interceptor chain that needs its handlers invoked in reverse order for the "response" phase** of a request/response cycle (mirroring how many real HTTP middleware stacks conceptually unwind): reuse the k-group or whole-list reversal idea if the chain is represented as a singly linked structure internally.
4. **A log-replay or event-sourcing structure where events are linked in insertion order** and a tool needs to present them oldest-last instead of oldest-first: reverse a singly linked event chain in place rather than materializing a second array purely to flip the presentation order.
5. **A "reverse every k records" data-transformation utility** (e.g., normalizing a chain of paginated result nodes fetched in reversed page batches from an upstream service): apply `reverseKGroup`'s exact bookkeeping pattern to correctly stitch reversed batches back into one coherent chain.

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

Common follow-up questions:
- *"Walk me through why you need to save `next` before rewiring `curr->next`."* The expected answer names the specific hazard directly: `curr->next` is the only remaining reference to the rest of the list at that point in the traversal, and overwriting it before reading it discards that reference permanently.
- *"Now do it for just a sub-range `[left, right]`."* This tests whether the candidate reaches for a dummy head node proactively (to make `left == 1` a non-special case) or discovers the need for it mid-way through, after their first attempt breaks on that exact input.
- *"Now do it in groups of k."* This is the question that separates "has memorized the sub-range trick" from "understands the reconnection logic well enough to repeat it in a loop." The expected answer explicitly calls out the lookahead-before-reversing check for a full group.
- *"Can you do the whole-list version recursively? What's the catch?"* A strong answer produces the elegant recursive version quickly, then proactively names the O(n) call-stack cost as the catch — showing they understand *why* the iterative version is generally preferred in production code, not just that "recursion is different."
- *"What if the list turns out to be cyclic — what happens to your reversal loop?"* Tests whether the candidate connects this pattern to its sibling (Fast & Slow Pointers): a reversal loop written to assume `nullptr` termination will misbehave (infinite loop or corrupted structure) on cyclic input, so a list from an untrusted source should be checked for cycles first.

Common misconceptions:
- "Reversing a linked list is just one algorithm." It is one *mechanism* (the three-pointer rewiring loop) applied to three distinct problems (whole list, sub-range, k-group), each with its own seam-reconnection bookkeeping.
- "A recursive solution is just as good if it produces the right output." Correct output is necessary but not sufficient — the recursive version's hidden call-stack space cost is a real production concern on long lists, and claiming "O(1) space" for it is inaccurate.
- "In-place reversal has no downside since it saves memory." It genuinely mutates the caller's data structure; if anything else expects to read the list in its original order afterward, that is a real, not hypothetical, bug source — not a purely academic concern.
- "The k-group version is basically the sub-range version, so it must be similarly easy." The *core* rewiring is the same, but managing it correctly across an unknown number of repeated groups, each requiring its own lookahead check, is exactly why LeetCode rates 25 as Hard while rating 92 as only Medium.

## Summary

- In-place Reversal rewires existing `next` pointers to reverse a whole list, a sub-range, or fixed-size groups, using **O(1) extra space**.
- The core mechanism is three pointers — `prev`, `curr`, and a temporary `next` — used in a fixed order every iteration: save `next`, rewire `curr->next` to `prev`, advance both pointers.
- It replaces a copy-into-an-array-or-new-list approach that works correctly but costs **O(n) space**, and a recursive approach that hides an O(n) (or O(n/k)) call-stack cost behind seemingly simple code.
- Sub-range (`[left, right]`) and k-group reversal reuse the identical three-pointer loop, adding a **dummy head node** so a reversal starting at the true head needs no special case.
- k-group reversal additionally needs a **lookahead check** before reversing each group, to correctly leave a trailing short group untouched.
- The most common bug is rewiring `curr->next` before saving its original value, which silently discards the rest of the list.
- It combines naturally with **Fast & Slow Pointers**: find the middle first, then reverse the second half — the standard shape behind palindrome checks and list-reordering problems.

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
