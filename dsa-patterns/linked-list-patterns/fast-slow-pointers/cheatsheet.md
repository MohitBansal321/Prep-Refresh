# Fast & Slow Pointers — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Linked List pattern (also generalizes to any function-generated sequence). |
| **Recognition Signal** | Problem statement mentions detecting a cycle/repeat, finding the middle without counting first, or finding where two things "meet" — usually with an implicit or explicit "no extra memory" constraint. |
| **Problem** | Detecting a cycle or finding the middle of a linked list without random access. The brute-force fix (a hash set of visited node pointers) works but costs **O(n) space**. |
| **Solution** | Two pointers walk the same sequence at different speeds — `slow` one step, `fast` two steps. If a cycle exists, the constantly-shrinking gap forces them to collide. If not, `fast` reaches the end first, and `slow` is then exactly at the midpoint. |
| **Participants** | **`slow` pointer** (moves 1 step/iteration; ends at the midpoint, or is the meeting-point half of cycle detection) · **`fast` pointer** (moves 2 steps/iteration; hits the end first if no cycle, or laps `slow` if one exists). |
| **Flow** | Init both at `head` → loop while `fast != nullptr && fast->next != nullptr` → advance slow by 1, fast by 2 → check `slow == fast` (cycle) or check `fast`'s null-ness after the loop (middle found). |
| **Pros** | O(1) space (vs. O(n) for a hash set) · single pass · simple to implement once memorized · generalizes beyond linked lists (any "repeatedly apply a function" sequence). |
| **Cons** | Only answers a narrow set of questions (cycle / middle / meeting point) · off-by-one prone on even-length "middle" · needs a second phase (reset to head) to find the cycle's *start*, not just its existence. |
| **Use When** | Cycle detection, finding a list's middle, problems on implicit sequences (Happy Number, Find the Duplicate Number), palindrome checks (combined with In-place Reversal). |
| **Avoid When** | You need random access into the middle repeatedly (use an array) · the "list" is not naturally sequential (use hash-based lookups) · you need the actual reversed structure, not just a meeting point (that is In-place Reversal's job). |
| **Related Patterns** | In-place Reversal (rewires `next`; often combined, e.g. palindrome check) · hash-set visited tracking (same goal, O(n) space instead of O(1)) · Two Pointers on arrays (same "two cursors" spirit, but converging on sorted random-access data instead of two speeds on a sequential structure). |

### Two Core Uses, One Loop Shape

```cpp
// Cycle detection (Floyd's Tortoise and Hare)
Node* slow = head;
Node* fast = head;
while (fast != nullptr && fast->next != nullptr) {
  slow = slow->next;
  fast = fast->next->next;
  if (slow == fast) { /* cycle found */ break; }
}

// Finding the middle (no cycle assumed)
Node* slow = head;
Node* fast = head;
while (fast != nullptr && fast->next != nullptr) {
  slow = slow->next;
  fast = fast->next->next;
}
// slow now points at the middle node
// (2nd of the two middles, for an even-length list)
```

### Floyd's Phase 2 — Find the Cycle's Start Node

```cpp
// After Phase 1 confirms slow == fast somewhere inside the cycle:
slow = head;               // reset ONE pointer to head
while (slow != fast) {     // both now move at speed 1
  slow = slow->next;
  fast = fast->next;
}
// slow == fast == the cycle's entry node
```

### Complexity

| | Time | Space |
|---|------|-------|
| Fast & Slow Pointers | O(n) | **O(1)** |
| Hash-set of visited nodes (the replaced approach) | O(n) | O(n) |
| "Count length, then walk n/2" for middle | O(n) (2 passes) | O(1) |

### Remember In One Sentence
> **A slow pointer moving one step and a fast pointer moving two steps through the same sequence are guaranteed to either collide (a cycle exists) or have the fast one finish first (revealing the midpoint) — trading the hash-set approach's O(n) space for O(1).**

### One Fact People Get Wrong
- Detecting **that** a cycle exists and finding **where** it starts are two different questions. The first collision point (Phase 1) is almost never the cycle's entry node — you need the reset-to-head second phase (Phase 2) to find that.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. State the intent in one sentence. What two problems does this pattern solve?
2. Why are the slow and fast pointers *guaranteed* to meet if a cycle exists, instead of the fast pointer just skipping over the slow one forever?
3. What exact loop condition must guard the fast pointer's two-step advance, and what crash happens if you get it wrong?
4. For an even-length list, does `find_middle` return the first or second of the two middle nodes with the standard loop condition? What would you change to get the other one?
5. What is the O(n)-space alternative to this whole pattern, and in what situation would you actually still choose it?
6. Describe Floyd's Phase 2 in your own words: what gets reset, what stays the same, and why does the second meeting point land exactly at the cycle's start?
7. Name a problem where Fast & Slow Pointers applies to something that is **not** a linked list. What plays the role of `next`?
8. Which sibling pattern does Palindrome Linked List combine Fast & Slow Pointers with, and in what order do the two steps happen?
9. What is the time and space complexity of Fast & Slow Pointers cycle detection, and how does it compare to the hash-set approach?
10. Name one real system (outside of interview problems) where cycle detection over a sequence generated by repeated function application is a genuinely used technique.
