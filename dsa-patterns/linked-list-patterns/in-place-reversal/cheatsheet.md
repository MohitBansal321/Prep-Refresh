# In-place Reversal — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Linked List pattern. |
| **Recognition Signal** | Problem statement says "reverse" — the whole list, a `[left, right]` sub-range, or groups of k nodes — usually with an explicit or implicit "O(1) extra space" constraint. |
| **Problem** | Reversing a singly linked list without random access or a `prev` pointer. The brute-force fix (copy values into an array/new list, rebuild) works but costs **O(n) space**; a recursive fix hides an **O(n) call-stack** cost instead. |
| **Solution** | Walk the list once with three pointers. At each node: save `next = curr->next` (or you lose the rest of the list), rewire `curr->next = prev`, then advance `prev = curr; curr = next`. Sub-range and k-group variants run this same loop on a slice, using a dummy head node to reconnect the seams. |
| **Participants** | **`prev`** (already-reversed portion's new head; starts `nullptr`, or the group's successor for k-group) · **`curr`** (walks forward through the unprocessed portion) · **`next`** (temporary; the only saved route to the rest of the list) · **dummy head node** (stand-in "node before position 1," needed whenever the reversal might include the true head). |
| **Flow** | Init `prev = nullptr, curr = head` (or the range/group's start) → loop for the right number of iterations → save `next`, rewire `curr->next = prev`, advance both → after the loop, `prev` is the new head of what was just reversed; reconnect seams if this was a sub-range or group. |
| **Pros** | O(1) space (vs. O(n) for a copy, or hidden O(n) for recursion) · single mechanism covers whole-list, sub-range, and k-group reversal · rearranges real node objects, not just copied values. |
| **Cons** | Mutates the original list permanently (original order is gone unless reversed back) · sub-range/k-group variants are fiddly (dummy node + lookahead bookkeeping) · off-by-one prone on range boundaries and group-size checks · unsafe on an unverified/possibly-cyclic list. |
| **Use When** | Reversing a whole list, a `[left, right]` slice, or fixed-size groups; as the second half of a two-pattern composition (find the middle with Fast & Slow Pointers, then reverse the second half — palindrome checks, list reordering). |
| **Avoid When** | The original order must be preserved elsewhere and you cannot restore it afterward · the structure is immutable/persistent (rewiring is not a valid operation there) · the actual question is about cycles/middles, not reversal (that is Fast & Slow Pointers' job) · the list might be cyclic and has not been checked first. |
| **Related Patterns** | Fast & Slow Pointers (different goal — detects cycles/middles rather than restructuring; the two combine in Palindrome Linked List) · Two Pointers on arrays (same "reverse a sequence" goal, but via index swaps on random-access data, no pointer rewiring or dummy nodes needed) · Recursive reversal (same result, but O(n) call-stack space instead of O(1)). |

### Three Shapes, One Loop

```cpp
// 1. Whole list (LeetCode 206)
ListNode* reverseList(ListNode* head) {
  ListNode* prev = nullptr;
  ListNode* curr = head;
  while (curr != nullptr) {
    ListNode* next = curr->next;  // save BEFORE overwriting
    curr->next = prev;
    prev = curr;
    curr = next;
  }
  return prev;
}

// 2. Sub-range [left, right], 1-indexed inclusive (LeetCode 92)
ListNode* reverseBetween(ListNode* head, int left, int right) {
  ListNode dummy(0);
  dummy.next = head;
  ListNode* prevRange = &dummy;
  for (int i = 1; i < left; ++i) prevRange = prevRange->next;

  ListNode* rangeTail = prevRange->next;  // becomes the new tail
  ListNode* prev = nullptr;
  ListNode* curr = rangeTail;
  for (int i = 0; i <= right - left; ++i) {
    ListNode* next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
  }
  prevRange->next = prev;   // splice new head in
  rangeTail->next = curr;   // reconnect new tail to what follows
  return dummy.next;
}

// 3. Groups of k (LeetCode 25); k=2 is Swap Nodes in Pairs, LeetCode 24
ListNode* reverseKGroup(ListNode* head, int k) {
  ListNode dummy(0);
  dummy.next = head;
  ListNode* groupPrev = &dummy;

  while (true) {
    ListNode* kth = groupPrev;
    for (int i = 0; i < k && kth != nullptr; ++i) kth = kth->next;
    if (kth == nullptr) break;  // fewer than k nodes remain — stop

    ListNode* groupStart = groupPrev->next;
    ListNode* nextGroupStart = kth->next;

    ListNode* prev = nextGroupStart;
    ListNode* curr = groupStart;
    while (curr != nextGroupStart) {
      ListNode* next = curr->next;
      curr->next = prev;
      prev = curr;
      curr = next;
    }
    groupPrev->next = kth;      // kth is the group's new head
    groupPrev = groupStart;     // groupStart is the group's new tail
  }
  return dummy.next;
}
```

### Complexity

| | Time | Space |
|---|------|-------|
| In-place Reversal (all 3 variants) | O(n) | **O(1)** |
| Copy into array/new list, then rebuild | O(n) | O(n) |
| Recursive reversal | O(n) | O(n) call stack (or O(n/k) for k-group) |

### Remember In One Sentence
> **Walk the list once with three pointers — save `next` before rewiring `curr->next` to `prev` — and reuse that exact loop, anchored by a dummy head node, to reverse the whole list, a `[left, right]` slice, or groups of k, all in O(1) extra space.**

### One Fact People Get Wrong
- Reversing the *whole* list never needs a dummy head node (there is no "node before the head" to update — the new head just becomes `prev`). But the *moment* a reversal might start at the true head while other nodes exist before/after it conceptually (sub-range, k-group), a dummy head becomes necessary to avoid an `if (left == 1)`-style special case.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. State the intent in one sentence. What three variants of "reverse" does this pattern handle?
2. Name the three pointers used in the core loop and, for each one, say in one phrase what it is responsible for.
3. What is the exact bug that happens if you write `curr->next = prev;` before saving `curr->next` into a temporary variable? Why does it not crash immediately?
4. Why does `reverseBetween` need a dummy head node, but `reverseList` does not?
5. What extra check does `reverseKGroup` need before reversing each group that `reverseBetween` never needs at all? What happens if you skip it?
6. What is the O(n)-space alternative to iterative reversal, and what specifically is the O(n) cost hiding inside if there is no explicit array in the code?
7. Name the sibling pattern this one is most often combined with, which specific LeetCode problem is the textbook example of that combination, and in what order do the two patterns run?
8. What is the time and space complexity of whole-list, sub-range, and k-group reversal? Are they all the same?
9. Give one reason `reverseKGroup` is rated Hard on LeetCode while `reverseBetween` is only rated Medium, even though both reuse the same core loop.
10. Name one situation where In-place Reversal should NOT be used, even though the data is technically a linked list that could be reversed.
