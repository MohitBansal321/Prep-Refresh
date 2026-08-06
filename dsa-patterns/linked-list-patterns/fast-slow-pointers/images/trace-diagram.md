# Trace Diagram — Worked Example

This traces Floyd's cycle detection step by step on one concrete list:

```
1 -> 2 -> 3 -> 4 -> 5 -> (back to 3)
```

Node `5`'s `next` points back to node `3`, forming a cycle of length 3 (`3 -> 4 -> 5 -> 3 -> ...`). `slow` starts at `1` and moves one node per step; `fast` starts at `1` and moves two nodes per step.

```mermaid
sequenceDiagram
    autonumber
    participant List as List: 1→2→3→4→5→(back to 3)
    participant Slow as slow pointer
    participant Fast as fast pointer

    Note over Slow,Fast: Initial state: slow = 1, fast = 1

    List->>Slow: step 1 -> node 2
    List->>Fast: step 1 -> node 3 (2 steps: 1->2->3)
    Note over Slow,Fast: slow=2, fast=3 — not equal, continue

    List->>Slow: step 2 -> node 3
    List->>Fast: step 2 -> node 5 (2 steps: 3->4->5)
    Note over Slow,Fast: slow=3, fast=5 — not equal, continue

    List->>Slow: step 3 -> node 4
    List->>Fast: step 3 -> node 4 (2 steps: 5->3->4)
    Note over Slow,Fast: slow=4, fast=4 — MEET! Cycle confirmed.

    Note over List,Fast: Phase 2 (find cycle start): reset slow to head (node 1),<br/>keep fast at node 4. Advance both by 1 step at a time.

    List->>Slow: phase 2, step 1 -> node 2
    List->>Fast: phase 2, step 1 -> node 5
    Note over Slow,Fast: slow=2, fast=5 — not equal, continue

    List->>Slow: phase 2, step 2 -> node 3
    List->>Fast: phase 2, step 2 -> node 3
    Note over Slow,Fast: slow=3, fast=3 — MEET at cycle start = node 3
```

## How to read it

Read this top to bottom as a timeline, not as message-passing between real objects — "List" here just represents the act of following a `next` pointer; it is not a participant with its own behavior. Each pair of arrows (one to `slow`, one to `fast`) represents one iteration of the `while` loop in `has_cycle()`: `slow` always gets exactly one arrow (one `next` hop) while `fast` always gets one arrow labeled with two hops, because it moves twice as fast.

The first three iterations are Phase 1. Watch the position notes after each iteration: the gap between `slow` and `fast` shrinks by one node per iteration once `fast` is inside the cycle (iteration 2: gap is 2 nodes apart going into the cycle; iteration 3: they land on the same node). That shrinking gap — not a fixed distance — is *why* they are guaranteed to meet rather than `fast` perpetually skipping over `slow`.

The last two iterations are Phase 2, which only runs after Phase 1 confirms a cycle exists. Notice `slow` is reset all the way back to the list's head while `fast` stays exactly where the two pointers first met — then both move at the *same* speed (one step each) until they meet again. That second meeting point, node 3, is mathematically guaranteed to be the cycle's entry node; the full proof (distance from head to cycle start equals distance from the meeting point back around to the cycle start) is in the README's Execution Flow section.
