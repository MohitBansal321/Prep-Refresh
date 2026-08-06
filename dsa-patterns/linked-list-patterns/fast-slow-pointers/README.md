# Fast & Slow Pointers

## Intent

Detect a cycle in a sequence, or find its midpoint, by walking two pointers through it at different speeds — using O(1) extra memory instead of recording every value you have already seen.

## Real Life Analogy

Picture two runners on a **circular track**, starting at the same point, one running at twice the speed of the other. If the track really is a closed loop, the faster runner will eventually lap the slower one — they are guaranteed to be standing at the same point on the track again at some future moment, no matter how long the track is. This is not a coincidence; it follows from simple arithmetic: every lap, the faster runner gains ground on the slower one equal to the length of one lap of their speed difference, so the "distance behind" (a number that can only shrink, never grow, once both runners are on the loop) hits zero in finite time.

Now picture the same two runners on a **straight, finite track** instead of a loop. The faster runner reaches the finish line first, stops, and never meets the slower runner again along the way. Whether they *ever* meet again tells you, with total certainty, whether the track was a loop or a straight line — you never needed to mark every point either runner passed through to figure that out.

This is exactly Floyd's Tortoise and Hare algorithm (named for the fable, and for real by its inventor Robert W. Floyd): a `slow` pointer is the tortoise, moving one step at a time; a `fast` pointer is the hare, moving two steps at a time. If the linked list loops back on itself (a cycle), the hare eventually meets the tortoise again. If the list is a normal, finite, nullptr-terminated straight line, the hare reaches the end first and the tortoise never gets lapped — because there was nothing to lap it.

> **Term: Cycle.** In a linked list, a cycle exists when some node's `next` pointer, followed repeatedly, leads back to a node you have already visited — meaning there is no node whose `next` is `nullptr`. Walking such a list with a naive loop (`while (node != nullptr)`) never terminates.

## Problem

### What engineering problem exists?

Two closely related questions come up constantly when working with singly linked lists:

1. **Does this list contain a cycle?** A linked list is only supposed to be a straight chain ending in `nullptr`. If a bug (or malicious input, or a corrupted pointer) causes some node to point back at an earlier node, any code that walks the list with a plain loop will spin forever — a genuine production hazard, not just an academic curiosity.
2. **What is the middle node of this list?** Needed directly (return the middle) and indirectly, as a building block for other problems — reordering a list, checking if it is a palindrome, or splitting it into two halves for a merge-sort-style divide and conquer.

Both questions share a constraint that makes them interesting: a singly linked list gives you **no random access**. You cannot ask for "the node at index `n/2`" the way you can index into an array — you can only ever ask a node "what is your `next`?" So finding the middle *without first counting the length* is not obvious, and detecting a cycle without ever revisiting a node you have already logged is not obvious either.

> **Term: Random access.** The ability to jump directly to the `k`-th element of a collection in constant time, the way `arr[k]` works on an array. A singly linked list has no such operation — reaching the `k`-th node requires walking `k` `next` pointers one at a time, which is why "count the nodes, then walk halfway" already costs you two full passes over the list.

### Why is this problem difficult?

- **You cannot "look ahead" without a pointer already there.** There is no way to peek at node 100 without first visiting nodes 1 through 99. Any algorithm must be built entirely out of `next`-pointer hops.
- **A cycle makes naive traversal literally infinite.** A `for` loop bounded by a count works fine on an array; on a linked list with a cycle, a naive `while (node != nullptr)` loop never sees `nullptr` and runs forever, hanging the program.
- **"Find the middle" without knowing the length first seems to require two passes** — one to count nodes, one to walk half of them — which works, but is not the tightest solution, and does not even help with cycle detection at all (a cyclic list has no finite "length" to count in the first place).

### What happens if we ignore it?

- **An infinite loop / hang.** Any function that walks a linked list assuming it terminates (serialization, deep-copying, computing a length, printing it for a log) will hang the process if a cycle sneaks in — a real production bug, not a made-up interview scenario. A hung request thread consuming CPU indefinitely is exactly the kind of incident that pages someone at 3 a.m.
- **Wasted memory if you reach for the "obvious" fix.** The natural first instinct — keep a `std::unordered_set<Node*>` of every node you have visited, and check membership before visiting the next one — does correctly detect a cycle (and can find the middle by counting), but costs **O(n) extra space**: one hash-set entry per node, in addition to the list's own memory. For a list of a few million nodes, that is millions of pointer-sized entries just to answer a yes/no question.
- **Two full passes instead of one** for the naive "count, then walk half of that count" approach to finding the middle — correct, but not the tightest possible solution, and it does not generalize to the cycle-detection problem at all.

## Why Not Other Approaches

**"Keep a hash set of every visited node pointer; if you see a repeat, that's the cycle."**
This works, and is worth knowing because it is the natural first idea and a fine fallback if you are under time pressure and cannot recall Floyd's trick. But it costs **O(n) space** — a `std::unordered_set<ListNode*>` growing by one entry per node visited. On a list with a million nodes, that is a million hash-table entries purely to answer "is there a cycle." Fast & Slow Pointers answers the exact same question with **two pointer variables** — O(1) space, regardless of list length. This is the central tradeoff this whole pattern exists to make: trade a little bit of "obviousness" for an O(n) → O(1) space improvement.

**"Count the list's length first, then walk `length / 2` steps to reach the middle."**
This correctly finds the middle in two passes, and it is a perfectly reasonable thing to write under time pressure. But it makes **two full traversals** of the list where Fast & Slow Pointers needs only **one** (both pointers advance together in the same loop). It also silently assumes the list has a finite length — it cannot be adapted to detect a cycle at all, because a cyclic list has no length to count. Fast & Slow Pointers is strictly more general: the same loop shape answers both "is there a cycle" and "where is the middle," while counting-then-walking only ever answers the second, and only for acyclic lists.

**"Convert the list to an array/vector first, then use array techniques (indexing, two-pointer convergence)."**
This trades the linked-list constraint away entirely — but at the cost of an O(n)-space copy, and it still requires a full pass to build that copy before you can do anything useful. If the whole point is to avoid extra memory, copying the list into a new structure defeats the purpose from the first line of code.

**Tradeoff summary:** every alternative either costs O(n) extra space (hash set, array copy) or costs an extra full pass over the data (count-then-walk) or fails outright on cyclic input (count-then-walk cannot even be attempted). Fast & Slow Pointers is the one approach that is simultaneously O(1) space, single-pass, and correct on both cyclic and acyclic lists — which is exactly why it is the textbook answer interviewers are listening for, and why it shows up in real systems too (see Real Interview/Production Examples below).

## Solution

The core idea has one moving part duplicated at two speeds:

Start two pointers, `slow` and `fast`, both at the head of the list. On every iteration, advance `slow` by **one** node and `fast` by **two** nodes. Keep doing this and ask, after each step, whether `slow` and `fast` now point at the *same* node.

- **If the list has a cycle:** `fast` enters the cycle before (or at the same time as) `slow`. Once both pointers are somewhere inside the cycle, `fast` is closing the gap on `slow` by exactly one node per iteration (it gains two nodes of ground, `slow` gains one, net gain is one). A gap that shrinks by exactly one every iteration, on a fixed finite cycle length, is mathematically guaranteed to hit zero — `fast` cannot "hop over" `slow`, because the gap only ever changes by one at a time. When the gap reaches zero, the two pointers are on the same node: a cycle is confirmed, with **zero** extra memory beyond the two pointers.
- **If the list has no cycle:** `fast`, moving twice as fast, reaches the true end (`nullptr`) before `slow` can. Since `fast` has covered exactly twice the distance `slow` has by that point, `slow` — having covered exactly half the list — is standing at the **middle** node. The same single loop that would have detected a cycle instead hands you the midpoint, for free, the moment it terminates.

Both outcomes fall out of the *same* loop with the *same* two-pointer setup — you are not writing two different algorithms, you are asking two different questions about the result of one loop.

There is a second, less commonly needed refinement: if you need to know not just *that* a cycle exists but exactly **where it starts**, there is a second phase (Floyd's algorithm proper, in two phases) that resets one pointer to the head and walks both pointers at the *same* speed (one step each) until they meet again — that second meeting point is provably the cycle's entry node. The full argument for why this works is in Execution Flow below.

## Architecture

The "participants" here are not classes — they are two roles played by pointer variables, plus the sequence they traverse.

1. **The sequence (linked list, or any function-generated chain).** The thing being walked. It exposes exactly one operation: "given the current position, what is next?" Nothing else about its internal shape matters to the algorithm — it does not need to be a `ListNode`; it can be an integer transformed by a formula (see Happy Number below), or an array index used as an implicit pointer (see the exercises).

2. **`slow` pointer.** Its invariant: it has moved exactly half as many steps as `fast` at every point in time. Its responsibility, once the loop ends, is to be standing at the answer — either the meeting point inside a cycle, or the midpoint of a finite list.

3. **`fast` pointer.** Its invariant: it always moves twice as far per iteration as `slow`. Its responsibility is to be the one that "finds out first" — it is the pointer whose termination condition (`nullptr`, or a repeat) drives the loop's exit, and its collision with `slow` is the cycle signal.

Neither pointer "knows" about the other's role explicitly in code — the guarantee comes entirely from the fixed 1-step-vs-2-step ratio being maintained every iteration, without exception. Breaking that ratio even once (e.g. accidentally advancing `fast` only once on some iteration) breaks every guarantee the pattern relies on.

## Execution Flow

**Cycle detection (Floyd's Phase 1):**

1. Set `slow = head` and `fast = head`. Both pointers start at the same node.
2. Check the loop condition: is `fast != nullptr` **and** `fast->next != nullptr`? If either is false, `fast` cannot safely take two more steps — exit the loop; there is no cycle.
3. Advance `slow` by one step: `slow = slow->next`.
4. Advance `fast` by two steps: `fast = fast->next->next`.
5. Check if `slow == fast` (same node, i.e. same pointer value). If so, a cycle exists — stop and report it.
6. If not, go back to step 2 and repeat.
7. If the loop exits via step 2 (fast or fast->next hit `nullptr`) without ever satisfying step 5, the list terminates normally: no cycle.

**Finding the middle (no cycle assumed):**

1. Set `slow = head` and `fast = head`.
2. Loop while `fast != nullptr` and `fast->next != nullptr`: advance `slow` by one, `fast` by two.
3. When the loop exits (because `fast` or `fast->next` became `nullptr`), `slow` is standing at the middle node. For an **odd**-length list this is the single exact middle; for an **even**-length list, this exact loop condition yields the **second** of the two middle nodes (see Common Mistakes for the off-by-one if you need the first one instead).

**Finding the cycle's start (Floyd's Phase 2 — only relevant once Phase 1 confirms a cycle):**

1. After Phase 1 ends with `slow == fast` at some node `M` inside the cycle, **reset `slow` back to `head`**. Leave `fast` exactly where it is, at `M`.
2. Now advance **both** pointers by exactly **one** step per iteration (not two for `fast` anymore) — same speed for both.
3. Repeat until `slow == fast` again. That node is the cycle's entry point.

Why step 3 works (the part worth actually understanding, not memorizing): let `a` = distance from `head` to the cycle's entry node, `b` = distance from the entry node to the first meeting point `M` (going forward around the cycle), and `c` = the remaining distance from `M` back around to the entry node (so `b + c` = the cycle's total length). By the time Phase 1's pointers meet at `M`, `slow` has traveled `a + b` steps and `fast` has traveled `2(a + b)` steps (twice as far), and `fast`'s extra distance over `slow`'s must be a whole number of laps around the cycle: `2(a+b) - (a+b) = a + b` is a multiple of the cycle length `b + c`. That algebraic fact rearranges to show `a` is congruent to `c` modulo the cycle length — in plain terms, walking `a` steps from `head` lands you at the same place as walking `c` steps from `M`. That is precisely what Phase 2 does: one pointer walks `a` steps from `head`, the other walks forward from `M` (which is `c` steps from the entry node going around), and they are guaranteed to land on the entry node together.

## Recognition Diagram

See [images/recognition-diagram.md](images/recognition-diagram.md) for the full decision-tree flowchart (linked list or function-generated sequence? need to detect a cycle/repeat, or find a middle without counting? is O(1) space required? — branching to Fast & Slow Pointers vs. a hash set vs. In-place Reversal).

## Flow Diagram

See [images/flow-diagram.md](images/flow-diagram.md) for the Mermaid flowchart of Floyd's Phase 1 control flow — the single loop with its one guard condition and one collision check, laid out as a diagram.

## Trace Diagram

See [images/trace-diagram.md](images/trace-diagram.md) for a step-by-step trace of both phases on a concrete example list, `1 -> 2 -> 3 -> 4 -> 5 -> (back to 3)`, showing exactly where `slow` and `fast` are after each iteration, where they first meet, and where Phase 2 lands them at the cycle's true entry node.

## Implementation

The generic template in [code.cpp](code.cpp) is built around a minimal `ListNode` struct and three small, reusable **function templates** — `has_cycle`, `find_cycle_start`, and `find_middle` — each one a direct translation of the Execution Flow steps above into code, parameterized on the node type so the same logic works on any pointer-like structure with a `->next` member.

The template functions do not allocate anything and do not know the meaning of the data stored in each node — they only ever compare pointer identity (`slow == fast`) and follow `->next`. That is deliberate: it is what makes the exact same three functions reusable, unmodified, across every worked problem in `problems/`.

Before reading the code, notice the one line every function shares and must get exactly right:

```cpp
while (fast != nullptr && fast->next != nullptr) {
```

Both halves of that condition are load-bearing. Drop the first half and `fast->next` dereferences a null pointer the moment `fast` itself is `nullptr`. Drop the second half and `fast->next->next` dereferences past the last real node when `fast` is sitting on the final node of an odd-length list. This single line is revisited explicitly in Common Mistakes because it is, empirically, the most common bug written against this pattern.

## Code Walkthrough

See [code.cpp](code.cpp) for the full runnable file. Here is what each part does and why it exists.

**`struct ListNode`.** The minimal singly linked list node: an `int val` and a `ListNode* next`. Nothing else. Kept deliberately tiny so the pattern's mechanics are not obscured by unrelated fields.

**`build_list(values, cycle_pos)`.** A test-data helper that builds a list from a `std::vector<int>` and, if `cycle_pos >= 0`, rewires the tail's `next` to point back at the node at that index — turning the list cyclic on demand. It exists purely so the demo `main()` can construct both cyclic and acyclic test lists without repeating boilerplate in every test case.

**`free_list(head)`.** Walks a **non-cyclic** list freeing every node with `delete`. It exists to avoid memory leaks in the demo — and its doc comment explicitly warns that calling it on a cyclic list would loop forever, which is itself a small, concrete illustration of why you must detect cycles *before* doing anything else that assumes termination (serialization, deep copy, printing, freeing).

**`has_cycle<Node>(head)`.** The Phase 1 template function: implements the cycle-detection loop exactly as described in Execution Flow, returning `true`/`false`. It exists as the standalone, reusable version of the check every cycle-detection problem needs (LeetCode 141 uses it almost verbatim — see `problems/01-linked-list-cycle.cpp`).

**`find_cycle_start<Node>(head)`.** Runs Phase 1 internally, and if a cycle is found, runs Phase 2 (reset one pointer to head, advance both at speed 1) to locate and return the entry node; returns `nullptr` if there is no cycle. It exists to demonstrate the full two-phase algorithm in one reusable function, not just the yes/no check.

**`find_middle<Node>(head)`.** The middle-finding template: same loop shape, but returns `slow` once the loop naturally exits (no cycle assumed). It exists as the reusable building block behind LeetCode 876 (`problems/02-middle-of-the-linked-list.cpp`) and behind the palindrome check (`problems/04-palindrome-linked-list.cpp`), which needs the middle before it can reverse the second half.

**`print_list(head, max_nodes)`.** A demo-only helper that prints up to `max_nodes` values and stops (rather than looping forever) — deliberately capped so that accidentally calling it on a cyclic list in a demo does not hang the program.

**`main()`.** Exercises all three template functions against five categories of test data: a cyclic list (confirming both `has_cycle` and `find_cycle_start` report correctly), a plain acyclic list, an odd-length list (middle-finding), an even-length list (confirming the "second of the two middles" convention), and the single-node/empty-list edge cases. Every printed result states its expected value inline so the file is self-checking when you read its output.

**Every file in `problems/`.** Each of the four worked solutions (`01`–`04`) is intentionally **standalone** — it redefines its own `ListNode`, `build_list`, and `free_list` rather than including `code.cpp` — so that any single file can be copy-pasted into a LeetCode submission box or compiled in isolation without pulling in the rest of this folder. `problems/README.md` explains why these specific four problems were chosen (pure cycle detection, pure middle-finding, the "not actually a linked list" generalization, and the "combined with another pattern" composition).

## Advantages

- **O(1) extra space**, regardless of how long the list is — the headline advantage over any hash-set-based approach.
- **Single pass.** Both the cycle check and the middle-finding both complete in one traversal, never two.
- **Correct on cyclic input by construction.** Unlike "count the length first," this approach never assumes the list terminates — it discovers that fact as a side effect of the loop's own exit condition.
- **Generalizes beyond linked lists.** The exact same "advance a value twice as fast as another, watch for a collision" idea applies to any deterministic sequence generated by repeatedly applying a function — see Happy Number (`problems/03-happy-number.cpp`) and Find the Duplicate Number (exercises).
- **Composable with other patterns.** Finding the middle is often step one of a larger algorithm (palindrome check, reordering, merge-sort on linked lists) — see Similar Patterns below.

## Disadvantages

- **Narrow applicability.** It answers exactly two questions well — "is there a cycle" and "where is the middle" — and does not generalize to arbitrary linked-list problems the way, say, a plain traversal does.
- **Off-by-one traps.** The exact loop condition determines whether an even-length list's "middle" is the first or second of the two candidates; getting this wrong is easy and silent (no crash, just a wrong answer) if you have not internalized which convention your loop produces.
- **Finding the cycle's *start* needs a non-obvious second phase.** Phase 1 alone only tells you a cycle exists; if the problem needs the entry node, you must remember and correctly implement Phase 2 (reset-one-pointer-to-head), which is not something most people can re-derive from first principles under interview pressure without having studied the proof once.
- **Pointer-equality bugs are silent.** If you accidentally compare `slow->val == fast->val` instead of `slow == fast`, the code can "work" on lists with unique values and fail mysteriously on lists with duplicates — a subtle correctness bug rather than a crash.

## Tradeoffs

**What we gain (vs. the hash-set-of-visited-nodes approach):** O(1) space instead of O(n), and a single pass instead of one pass to build the hash set. We keep exactly the same O(n) time complexity, so this is close to a pure win on the space axis.

**What we lose:** the hash-set approach is more *obviously* correct to someone seeing it for the first time — "keep a set of what you've seen, check membership" needs no proof, while "two pointers at different speeds must meet" requires understanding (or trusting) the gap-shrinks-by-one argument. Fast & Slow Pointers also loses a small amount of flexibility: a hash set gives you, for free, a full list of every node visited before the repeat — useful if you need more than just "is there a cycle." Fast & Slow Pointers gives you only the meeting point, and getting the cycle's *start* (rather than an arbitrary point inside it) costs you the extra Phase 2 traversal.

## Complexity

**Time — Best case:** O(1) — an empty list, or a two-node list that is cyclic on itself, resolves in the first iteration or two.

**Time — Worst case:** O(n) — for cycle detection with no cycle, `fast` must reach the true end, visiting every node once (relative to `fast`'s pace, this is at most `n` total node visits summed across both pointers, still linear); for cycle detection *with* a cycle, `fast` meets `slow` within one full traversal of the cycle at most, still O(n) overall. Middle-finding is always exactly one pass, O(n).

**Time — Average case:** O(n) for both operations — there is no meaningfully "average" faster case; the loop's cost is proportional to list length (or, for a cyclic list, to the distance to the cycle plus the cycle's length) regardless of the specific values stored.

**Space — This pattern:** **O(1)** — exactly two pointer variables (`slow`, `fast`), no matter how long the list is. This is the entire point of the pattern and the number you should have memorized cold.

**Space — Replaced approach (hash set of visited nodes):** **O(n)** — one hash-table entry per node visited before a repeat is found (or before the list ends). For a list of `n` nodes, that is `n` pointer-sized entries in the worst case (a fully acyclic list, or a cycle discovered only after visiting nearly every node).

This O(n) → O(1) space reduction, at no cost to the O(n) time complexity, is exactly why Fast & Slow Pointers is the textbook answer whenever a linked-list problem explicitly calls out an O(1) space constraint.

## Common Mistakes

- **Only checking `fast != nullptr`, forgetting `fast->next != nullptr`.** This crashes with a null-pointer dereference on `fast->next->next` the moment `fast` lands on the last real node of an odd-length, acyclic list (its `next` is `nullptr`, and you then try to read `nullptr->next`). *Why it happens:* people remember "check fast isn't null" but forget that the *next* hop needs its own null check too, since `fast` advances two nodes, not one. *Avoid it:* always write the guard as `fast != nullptr && fast->next != nullptr`, as a fixed idiom you type the same way every time, not something you re-derive per problem.

- **Off-by-one on the middle of an even-length list.** With the standard guard `fast != nullptr && fast->next != nullptr`, walk `[1,2,3,4]` by hand: start `slow=1, fast=1`; iteration 1 moves `slow` to `2` and `fast` to `3` (`fast->next` was `2`, not null, so the loop ran); the loop condition is then checked again with `fast` at node `3` — `fast->next` is node `4` (not null), so a second iteration runs, moving `slow` to `3` and `fast` to `nullptr` (`fast->next->next` steps off the end); the loop then exits because `fast` is `nullptr`. Final answer: `slow` is at node `3`, the **second** of the two middle nodes (`2` and `3`). *Changing* the loop guard to stop one iteration earlier shifts which of the two middle nodes you land on, and problems differ on which one they want (LeetCode 876 wants the second; some interview variants want the first). *Why it happens:* the exact loop condition is easy to get subtly wrong without noticing, because a variant that stops one iteration earlier or later still "looks correct" and never crashes — it just returns the other middle node. *Avoid it:* test explicitly against a small even-length list (`[1,2,3,4]`) and manually verify which node your specific loop condition returns before assuming it matches what the problem wants.

- **Forgetting that Floyd's cycle-*start* phase requires resetting one pointer to `head`.** Stopping at the *first* collision point and reporting it as "the start of the cycle" is wrong — that first collision can be anywhere inside the cycle, not necessarily its entry node. *Why it happens:* Phase 1 (detect a cycle) and Phase 2 (find where it starts) are easy to conflate if you have only memorized "two pointers, they meet" without understanding *why* the reset-to-head trick works. *Avoid it:* remember there are two distinct questions ("does a cycle exist" vs. "where does it start") and two distinct phases; do not skip Phase 2 if the problem asks for the entry node specifically (LeetCode 142, not 141).

- **Comparing values instead of pointers/references.** Writing `slow->val == fast->val` instead of `slow == fast` silently breaks on any list containing duplicate values, reporting a false "cycle" or false "meeting" on two *different* nodes that merely happen to hold equal data. *Why it happens:* it is easy to reach for value comparison out of habit from array/string problems. *Avoid it:* always compare the pointers (identity), never the payload, in this pattern.

- **Walking a cyclic list with an ordinary "print/count/copy" loop before checking for a cycle first.** Any code assuming `nullptr` termination hangs forever on cyclic input. *Why it happens:* it is easy to forget that a linked list is not *guaranteed* acyclic just because most of your test data is. *Avoid it:* if a list's origin is untrusted (deserialized input, a structure built across process boundaries, or explicitly documented as possibly cyclic), run `has_cycle` first before any traversal that assumes termination.

## When To Use

- **Detecting a cycle in a singly linked list** with an O(1) space constraint (LeetCode 141 and its variants).
- **Finding the middle of a linked list** without a prior length-counting pass — directly, or as a building block for a larger algorithm (LeetCode 876; the middle-finding step inside 234 and 143).
- **Any problem framed as "a value that gets transformed by a fixed function, repeatedly — does it eventually repeat or terminate?"** even when there is no literal linked list in the problem statement (LeetCode 202 Happy Number; LeetCode 287 Find the Duplicate Number, treating the array as an implicit pointer structure).
- **As the first phase of a two-pattern composition** — finding the middle, then handing the second half to In-place Reversal, is the standard shape of palindrome checks and list-reordering problems.
- **Checking whether two separately-traversed sequences (e.g. two linked lists, or a fast/slow simulation of a pseudorandom generator) eventually converge**, when you specifically cannot afford to record every value seen.

## When NOT To Use

- **When you need every node visited before a repeat, not just the fact that one exists.** A hash set gives you that list for free; Fast & Slow Pointers only gives you a meeting point, and reconstructing "everything visited before the meeting point" from that meeting point alone is not straightforward.
- **When you have random access anyway (an array with known bounds, not a pointer-chain).** If indexing is O(1), just compute the middle as `arr[n/2]` directly — there is nothing to be gained from simulating pointer-following on a structure that already supports indexing.
- **When the problem is fundamentally about reversing, rewiring, or restructuring the list**, not about detecting a cycle or finding a midpoint. That is In-place Reversal's job (see Similar Patterns).
- **When you need the full list of nodes inside the cycle, not just its length or entry point.** You would still need a separate traversal (starting from the confirmed entry node) to enumerate them; Fast & Slow Pointers itself does not hand you that list.
- **When O(n) auxiliary space is genuinely acceptable and code clarity to unfamiliar reviewers matters more than the last bit of space efficiency.** A hash-set-based cycle check is easier for someone unfamiliar with Floyd's algorithm to verify at a glance; that is a legitimate, if less "textbook," engineering tradeoff in a one-off script that will never see a million-node list.

## Real Interview/Production Examples

- **Interviews:** Fast & Slow Pointers (under the name "Floyd's cycle detection" or "the tortoise and hare") is one of the most frequently asked linked-list techniques at nearly every company that runs standard DSA-style interview loops (commonly reported at Amazon, Microsoft, Google, Meta, and Bloomberg-style interview processes) — precisely because it has a clean O(n) time / O(1) space answer that is easy to verify but non-trivial to derive from scratch, making it a good discriminator between "has studied linked lists" and "has not."
- **Functional graph analysis.** A *functional graph* is a directed graph where every node has exactly one outgoing edge (i.e. it represents a function `f` applied to each node). Floyd's algorithm (and the related, faster Brent's algorithm) is a genuinely used technique for finding cycles in such graphs — this is exactly the shape of the Happy Number problem, and the same shape appears in dependency-resolution graphs, state-machine analysis, and hash-chain analysis.
- **Pseudorandom number generator (PRNG) period detection.** A PRNG's internal state is repeatedly transformed by a fixed function to produce the next state; a poorly-designed PRNG can enter a short repeating cycle far sooner than expected, silently degrading randomness quality. Floyd's cycle-detection algorithm (and Brent's improvement on it) is a standard, real technique cryptography and simulation engineers use to empirically measure a PRNG's cycle length, with O(1) memory being genuinely important when testing generators expected to have astronomically long periods.
- **Detecting infinite loops in "linked" configuration or dependency structures**, such as a chain of symbolic links, a chain of "redirect to" pointers in a config system, or a singly-linked parent/prototype chain — any place a corrupted or maliciously-crafted structure could otherwise hang a process that assumes termination.

## Where I Can Use This

Five realistic ideas for your own backend or systems projects:

1. **Defensive traversal of any externally-sourced linked structure** (deserialized from JSON/protobuf into an in-memory chain, or reconstructed from a database's parent-pointer rows): run a cycle check before any operation (serialize, deep-copy, print for logging) that assumes the chain terminates, so a corrupted or adversarial input cannot hang a request thread.
2. **A "resolve symlink chain" or "resolve redirect chain" utility**: treat each hop as a `next` pointer and use Floyd's algorithm to detect an infinite redirect loop in O(1) space instead of accumulating a growing "visited URLs" set per request.
3. **A middleware/plugin pipeline modeled as a linked chain of handlers**: use the middle-finding technique to split the chain in half for a divide-and-conquer style parallel dispatch, or use cycle detection to catch a misconfigured pipeline that accidentally wires a handler back to an earlier one.
4. **Health-checking a circular buffer or ring-based queue** (a common structure for log rotation or bounded event queues) for a "writer has lapped the reader" condition — conceptually the same "two speeds, does a gap close to zero" reasoning as classic cycle detection (see the exercises' Real-World Challenge for a worked-through version of this idea).
5. **Detecting unintended cycles in an in-house dependency graph or job-scheduling chain** where each job has exactly one designated "next job" pointer (a functional graph) — reusing the exact `has_cycle` template with a custom node type instead of `ListNode`.

## Similar Patterns

- **In-place Reversal** (`../in-place-reversal/`, sibling folder in this repo): rewires `next` pointers to reverse a list, or a sub-range of one, in O(1) space. Different goal (restructure the list vs. answer a yes/no or "where" question about it), but the two combine constantly: **Palindrome Linked List** (`problems/04-palindrome-linked-list.cpp`) is the textbook example — Fast & Slow Pointers finds the middle first, then In-place Reversal reverses the second half so it can be compared against the first half without any extra memory.
- **Hash-set / hash-map visited tracking:** answers the same cycle-detection question at O(n) space instead of O(1). Worth knowing as the fallback if you cannot recall or derive Floyd's algorithm under pressure, or if you specifically need the full set of visited nodes rather than just a meeting point.
- **Two Pointers (converging, on arrays):** a different family (`../../array-string-patterns/two-pointers/`) — two pointers start at *opposite ends* of a *sorted, random-access* structure and move *toward* each other, rather than one pointer chasing another at a different speed through a sequential-access structure. The "two pointers" name is shared; the mechanics and preconditions are not.

| Pattern | Structure it needs | Pointer movement | Primary question answered | Space |
|---------|--------------------|--------------------|-----------------------------|-------|
| Fast & Slow Pointers | Sequential access (linked list, or function-generated sequence) | Same direction, different speeds (1x / 2x) | "Is there a cycle?" / "Where is the middle?" | O(1) |
| In-place Reversal | Sequential access (linked list) | Rewires `next`, walking forward once | "Reverse this list / sub-range" | O(1) |
| Two Pointers (converging) | Random access, sorted (array) | Opposite ends, moving toward each other | "Find a pair / partition / palindrome check on an array" | O(1) |
| Hash-set visited tracking | Any | N/A (membership check per step) | Same as Fast & Slow Pointers, but recording every visited node | O(n) |

## Interview Discussion

Experienced engineers rarely dwell on "can you write the loop" — that part is expected to be fast and correct. The conversation that actually distinguishes candidates centers on **why it works** and **what it composes with**.

Common follow-up questions:
- *"Prove that the fast and slow pointers must meet if a cycle exists — don't just assert it."* The expected answer is the shrinking-gap argument: once both pointers are inside the cycle, the fast pointer gains exactly one node of relative distance per iteration, and a gap that only ever decreases by a fixed amount on a fixed-size finite cycle must hit zero — it cannot skip past zero the way an unbounded, variable-sized jump might.
- *"How would you find where the cycle starts, not just whether one exists?"* This is the question that separates "has memorized the yes/no check" from "understands Floyd's algorithm." The expected answer walks through resetting one pointer to `head` and advancing both at equal speed — and, ideally, a sketch of why the distances work out (see Execution Flow's proof).
- *"What if I told you there's no linked list at all — just a rule that transforms one number into another — does your technique still apply?"* This is testing whether you understand the pattern's actual generality (Happy Number, Find the Duplicate Number) rather than having memorized "linked list problem → this loop."
- *"How do you find the middle of a doubly linked list, or an array?"* A good answer recognizes these are different problems: a doubly linked list still lacks random access so the same technique still helps, while an array already has O(1) indexing, so a fast/slow simulation would be pure overhead — just compute `n/2` directly.
- *"What does an interviewer actually want to see when you say 'I'll use fast and slow pointers'?"* Not just fluent code — the loop guard `fast != nullptr && fast->next != nullptr` written correctly on the first try, without prompting, is itself a signal of real (not memorized) understanding, because it is exactly the part people get subtly wrong.

Common misconceptions:
- "Fast and slow pointers is only for linked lists." It is a technique for any deterministic, function-generated sequence — the linked list is just the most common vehicle for teaching it.
- "The first meeting point is the start of the cycle." It is not, in general — that requires the separate Phase 2 (reset-to-head) step.
- "This pattern always saves time as well as space." It does not save time over a hash set (both are O(n) time) — the entire benefit is on the space axis, from O(n) down to O(1).
- "You need to memorize a proof to use this correctly." You need to memorize the loop's *shape* (and especially the guard condition) correctly; the proof is worth understanding once so the shape stops feeling like magic, but interviews are graded on correct, working code first.

## Summary

- Fast & Slow Pointers uses two pointers moving at different speeds (1x and 2x) through a sequence to detect a cycle or find a midpoint in **O(1) extra space**.
- It replaces a hash-set-of-visited-nodes approach that answers the same questions correctly but at **O(n) space**.
- The guarantee rests on a shrinking gap: once both pointers are inside a cycle, the fast one gains exactly one node per iteration on the slow one, forcing a meeting.
- The same loop, when there is **no** cycle, naturally hands you the list's midpoint the moment the fast pointer reaches the end.
- Finding the cycle's **entry node** (not just confirming one exists) needs a second phase: reset one pointer to `head`, then advance both at equal speed.
- The pattern generalizes well beyond linked lists — anywhere a value is repeatedly transformed by a fixed function (Happy Number, PRNG period detection, functional-graph cycle analysis).
- It combines naturally with **In-place Reversal**: find the middle with Fast & Slow, then reverse the second half — the standard shape behind palindrome checks and list-reordering problems.
- The most common bug is an incomplete loop guard (`fast != nullptr` without also checking `fast->next != nullptr`), which crashes on a null dereference.

## Key Takeaways

1. Two pointers, one moving 1 step and one moving 2 steps per iteration, either collide (cycle exists) or the fast one finishes first (revealing the midpoint).
2. This replaces an O(n)-space hash-set-of-visited-nodes approach with an O(1)-space one, at the same O(n) time cost.
3. The meeting guarantee comes from a shrinking gap: once inside a cycle, the fast pointer closes exactly one node of distance per iteration.
4. Always guard the loop with `fast != nullptr && fast->next != nullptr` — the single most common source of bugs in this pattern.
5. The first collision point inside a cycle is generally NOT the cycle's start; finding the start requires Floyd's Phase 2 (reset one pointer to head, advance both at equal speed).
6. Finding a list's middle with this pattern needs only one pass, versus two passes for "count the length, then walk half of it."
7. The even-length "middle" convention (first vs. second of the two middle nodes) is determined entirely by your loop's exact guard condition — test it explicitly.
8. The idea generalizes beyond linked lists to any function-generated sequence — Happy Number and Find the Duplicate Number apply the identical loop to plain integers.
9. It composes with In-place Reversal: find the middle, then reverse the second half — the standard approach to palindrome checks and list reordering.
10. Floyd's cycle detection is a genuinely used real-world technique, not just an interview trick — functional-graph cycle analysis and PRNG period detection both rely on it.

---

## Further Reading

**Books**
- *Introduction to Algorithms* (CLRS) — Cormen, Leiserson, Rivest, Stein — background on graph/cycle concepts that underpin functional-graph cycle detection.
- *The Art of Computer Programming, Volume 2: Seminumerical Algorithms* — Donald Knuth — the classical treatment of cycle detection in iterated function sequences, the same mathematical setting as Floyd's and Brent's algorithms.
- *Elements of Programming Interviews in C++* — Aziz, Lee, Prakash — has a dedicated linked-list chapter covering cycle detection and related pointer techniques with C++ code.
- *Cracking the Coding Interview* — Gayle Laakmann McDowell — the linked-list chapter's cycle-detection section is a widely used introduction to this exact pattern.

**Open Source / GitHub Repositories**
- `keon/algorithms` — a well-known open-source collection of algorithm implementations in Python, including a clear linked-list cycle detection implementation useful for cross-checking against this module's C++.
- LeetCode's own official solutions repository style discussions (see the "Solution" tab on each problem page referenced below) for community-vetted alternative implementations of the same problems worked here.

**Official Documentation / Problem Pages**
- LeetCode 141 — Linked List Cycle.
- LeetCode 142 — Linked List Cycle II (the "find the cycle's start" variant, i.e. Floyd's Phase 2).
- LeetCode 876 — Middle of the Linked List.
- LeetCode 202 — Happy Number.
- LeetCode 234 — Palindrome Linked List.
- LeetCode 287 — Find the Duplicate Number.

**Blog Articles**
- GeeksforGeeks — "Floyd's Cycle Detection Algorithm" — a widely referenced explainer with the standard proof of why the pointers must meet.
- Educative.io — "Grokking the Coding Interview" pattern write-up on Fast & Slow Pointers (also called "Linked List Cycle" pattern in that course) — the pattern-based framing this whole repo's philosophy is inspired by.
- NeetCode — video/explainer content walking through LeetCode 141, 142, and 876 using the fast/slow pointer technique, useful as a visual companion to this README's diagrams.
- Wikipedia — "Cycle detection" — covers Floyd's Tortoise and Hare alongside Brent's algorithm and their use in functional-graph and PRNG period analysis, with references to the original papers.
