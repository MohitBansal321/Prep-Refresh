// ============================================================================
// In-place Reversal — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// one mechanical idea that every "reverse a linked list" problem is built
// from: walk the list once, and at each node, redirect its `next` pointer
// backwards instead of forwards, using exactly three pointer variables
// (`prev`, `curr`, `next`) and zero extra memory proportional to the list's
// length.
//
//   1. reverseList   — reverse the ENTIRE list (LeetCode 206).
//   2. reverseBetween — reverse only the sub-range [left, right] (1-indexed,
//                       inclusive), leaving the rest of the list untouched
//                       (LeetCode 92).
//   3. reverseKGroup  — reverse the list in consecutive groups of k nodes,
//                       leaving a final group shorter than k untouched
//                       (LeetCode 25).
//
// All three are expressed as small, generic, reusable functions so you can
// see the *shape* of the pattern independent of any one problem. The worked,
// problem-specific solutions live in problems/*.cpp.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <vector>

// ----------------------------------------------------------------------------
// The node type shared by every example in this file. Deliberately minimal:
// one payload (`val`) and one link (`next`) — the only two things a singly
// linked list needs. Matches the ListNode used throughout
// ../fast-slow-pointers/code.cpp so the two sibling modules read consistently.
// ----------------------------------------------------------------------------
struct ListNode {
  int val;
  ListNode* next;
  ListNode(int x) : val(x), next(nullptr) {}
};

// ----------------------------------------------------------------------------
// Helper: build a singly linked, nullptr-terminated list from a
// std::vector<int>. Returns the head pointer (nullptr for an empty vector).
// ----------------------------------------------------------------------------
ListNode* build_list(const std::vector<int>& values) {
  if (values.empty()) return nullptr;

  ListNode* head = new ListNode(values[0]);
  ListNode* tail = head;
  for (size_t i = 1; i < values.size(); ++i) {
    tail->next = new ListNode(values[i]);
    tail = tail->next;
  }
  return head;
}

// ----------------------------------------------------------------------------
// Helper: free every node of a list, walking `next` until nullptr.
// ----------------------------------------------------------------------------
void free_list(ListNode* head) {
  while (head != nullptr) {
    ListNode* next = head->next;
    delete head;
    head = next;
  }
}

// ----------------------------------------------------------------------------
// Helper: print up to `max_nodes` values, space-arrow-separated.
// ----------------------------------------------------------------------------
void print_list(ListNode* head, int max_nodes = 50) {
  int count = 0;
  while (head != nullptr && count < max_nodes) {
    std::cout << head->val;
    head = head->next;
    if (head != nullptr && count + 1 < max_nodes) std::cout << " -> ";
    ++count;
  }
  std::cout << (count == max_nodes ? " -> ... (truncated)" : "") << "\n";
}

// ----------------------------------------------------------------------------
// Pattern function #1: reverse the WHOLE list in place.
//
// The three pointers and their jobs:
//   - `curr`  walks forward through the original list, one node at a time.
//   - `prev`  trails behind `curr`, holding the already-reversed portion's
//             new head (starts at nullptr, because the very first node's
//             `next` must eventually become nullptr — it will be the new
//             tail).
//   - `next`  saves `curr->next` BEFORE we overwrite it, because the moment
//             we write `curr->next = prev`, the original forward link is
//             gone forever unless we saved it first.
//
// Loop invariant, true at the top of every iteration: everything from `head`
// up to (not including) `curr` has already been reversed and its new head is
// `prev`; everything from `curr` onward is still in its original order.
// ----------------------------------------------------------------------------
ListNode* reverseList(ListNode* head) {
  ListNode* prev = nullptr;
  ListNode* curr = head;

  while (curr != nullptr) {
    ListNode* next = curr->next;  // Save before we overwrite curr->next.
    curr->next = prev;            // Rewire this node to point backwards.
    prev = curr;                  // The reversed portion's new head grows.
    curr = next;                  // Advance to the node we saved earlier.
  }

  return prev;  // curr is nullptr; prev is standing on the last original
                // node, which is now the new head.
}

// ----------------------------------------------------------------------------
// Pattern function #2: reverse only the sub-range [left, right] (1-indexed,
// inclusive), leaving everything before node `left` and after node `right`
// untouched and correctly reconnected.
//
// Uses a DUMMY head node so that "left == 1" (the reversal includes the true
// head of the list) needs no special-case branch: `dummy.next` always plays
// the role of "the node before the range," even when that node does not
// really exist yet.
// ----------------------------------------------------------------------------
ListNode* reverseBetween(ListNode* head, int left, int right) {
  ListNode dummy(0);
  dummy.next = head;

  // Walk `prevRange` to the node just BEFORE position `left`.
  ListNode* prevRange = &dummy;
  for (int i = 1; i < left; ++i) {
    prevRange = prevRange->next;
  }

  // `curr` starts at position `left`. By the end of the loop below, this
  // same node will have been pushed to the TAIL of the reversed range, so we
  // keep a name for it (`rangeTail`) to reconnect it afterward.
  ListNode* rangeTail = prevRange->next;
  ListNode* prev = nullptr;
  ListNode* curr = rangeTail;

  // Standard three-pointer reversal, but only for (right - left + 1) nodes.
  for (int i = 0; i <= right - left; ++i) {
    ListNode* next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
  }

  // After the loop: `prev` is the new head of the reversed range (the old
  // node at position `right`); `curr` is the first node AFTER the range
  // (or nullptr if `right` was the last node).
  prevRange->next = prev;       // Splice the reversed range in.
  rangeTail->next = curr;       // Old `left` node (now the tail) reconnects
                                 // to whatever came after `right`.

  return dummy.next;
}

// ----------------------------------------------------------------------------
// Pattern function #3: reverse the list in consecutive groups of `k` nodes.
// If the number of nodes remaining is not a multiple of `k`, the final,
// short group is left untouched (this is the LeetCode 25 convention).
//
// Iterative, O(1) space: for each group, first walk `k` nodes ahead to
// confirm a full group exists (a group cannot be reversed correctly if
// fewer than `k` nodes remain — that is exactly the case we must leave
// alone), then run the same three-pointer reversal on just that group, and
// finally reconnect the group's new head/tail to its neighbors using a
// dummy head, exactly as in reverseBetween.
// ----------------------------------------------------------------------------
ListNode* reverseKGroup(ListNode* head, int k) {
  if (head == nullptr || k <= 1) return head;

  ListNode dummy(0);
  dummy.next = head;
  ListNode* groupPrev = &dummy;  // Node just before the current group.

  while (true) {
    // Walk `k` steps ahead from groupPrev to find the k-th node of the
    // NEXT group. If we fall off the end before taking k steps, fewer
    // than k nodes remain — stop, leaving that final short group as-is.
    ListNode* kth = groupPrev;
    for (int i = 0; i < k && kth != nullptr; ++i) {
      kth = kth->next;
    }
    if (kth == nullptr) break;

    ListNode* groupStart = groupPrev->next;    // First node of this group.
    ListNode* nextGroupStart = kth->next;      // First node AFTER this group.

    // Reverse exactly the k nodes from groupStart up to and including kth.
    // `prev` is seeded with nextGroupStart so the group's new tail already
    // points at whatever comes after the group once the loop finishes.
    ListNode* prev = nextGroupStart;
    ListNode* curr = groupStart;
    while (curr != nextGroupStart) {
      ListNode* next = curr->next;
      curr->next = prev;
      prev = curr;
      curr = next;
    }

    // `kth` is now this group's new head; `groupStart` is now its new tail.
    groupPrev->next = kth;
    groupPrev = groupStart;
  }

  return dummy.next;
}

int main() {
  std::cout << "=== In-place Reversal: generic template demo ===\n\n";

  // ---- Test 1: reverse the whole list ---------------------------------
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    std::cout << "Test 1 (reverseList on [1,2,3,4,5]):\n";
    std::cout << "  Before: ";
    print_list(head);
    head = reverseList(head);
    std::cout << "  After:  ";
    print_list(head);
    std::cout << "  (expected: 5 -> 4 -> 3 -> 2 -> 1)\n\n";
    free_list(head);
  }

  // ---- Test 2: reverse the whole list, single node ----------------------
  {
    ListNode* head = build_list({42});
    std::cout << "Test 2 (reverseList on a single node [42]):\n";
    head = reverseList(head);
    std::cout << "  After: ";
    print_list(head);
    std::cout << "  (expected: 42)\n\n";
    free_list(head);
  }

  // ---- Test 3: reverse the whole list, empty list -----------------------
  {
    ListNode* head = build_list({});
    std::cout << "Test 3 (reverseList on an empty list):\n";
    head = reverseList(head);
    std::cout << "  After: " << (head == nullptr ? "null (empty)" : "not null")
               << " (expected: null)\n\n";
  }

  // ---- Test 4: reverse a sub-range that INCLUDES the true head ----------
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    std::cout << "Test 4 (reverseBetween([1,2,3,4,5], left=1, right=3)):\n";
    std::cout << "  Before: ";
    print_list(head);
    head = reverseBetween(head, 1, 3);
    std::cout << "  After:  ";
    print_list(head);
    std::cout << "  (expected: 3 -> 2 -> 1 -> 4 -> 5)\n\n";
    free_list(head);
  }

  // ---- Test 5: reverse a sub-range strictly inside the list -------------
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    std::cout << "Test 5 (reverseBetween([1,2,3,4,5], left=2, right=4)):\n";
    std::cout << "  Before: ";
    print_list(head);
    head = reverseBetween(head, 2, 4);
    std::cout << "  After:  ";
    print_list(head);
    std::cout << "  (expected: 1 -> 4 -> 3 -> 2 -> 5)\n\n";
    free_list(head);
  }

  // ---- Test 6: reverse a sub-range of length 1 (left == right, no-op) ---
  {
    ListNode* head = build_list({1, 2, 3});
    std::cout << "Test 6 (reverseBetween([1,2,3], left=2, right=2)):\n";
    head = reverseBetween(head, 2, 2);
    std::cout << "  After: ";
    print_list(head);
    std::cout << "  (expected: 1 -> 2 -> 3, unchanged)\n\n";
    free_list(head);
  }

  // ---- Test 7: reverse in groups of k, list length a multiple of k ------
  {
    ListNode* head = build_list({1, 2, 3, 4, 5, 6});
    std::cout << "Test 7 (reverseKGroup([1,2,3,4,5,6], k=2)):\n";
    std::cout << "  Before: ";
    print_list(head);
    head = reverseKGroup(head, 2);
    std::cout << "  After:  ";
    print_list(head);
    std::cout << "  (expected: 2 -> 1 -> 4 -> 3 -> 6 -> 5)\n\n";
    free_list(head);
  }

  // ---- Test 8: reverse in groups of k, trailing short group untouched ---
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    std::cout << "Test 8 (reverseKGroup([1,2,3,4,5], k=3)):\n";
    std::cout << "  Before: ";
    print_list(head);
    head = reverseKGroup(head, 3);
    std::cout << "  After:  ";
    print_list(head);
    std::cout << "  (expected: 3 -> 2 -> 1 -> 4 -> 5, last 2 nodes untouched)\n\n";
    free_list(head);
  }

  // ---- Test 9: reverse in groups of k, k larger than the whole list -----
  {
    ListNode* head = build_list({1, 2, 3});
    std::cout << "Test 9 (reverseKGroup([1,2,3], k=5)):\n";
    head = reverseKGroup(head, 5);
    std::cout << "  After: ";
    print_list(head);
    std::cout << "  (expected: 1 -> 2 -> 3, unchanged — fewer than k nodes)\n\n";
    free_list(head);
  }

  std::cout << "All demo cases printed above. Compare each result against\n"
               "its \"expected\" comment to confirm the template behaves.\n";

  return 0;
}
