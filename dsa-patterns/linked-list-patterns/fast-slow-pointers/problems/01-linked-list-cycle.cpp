// ============================================================================
// LeetCode 141 — Linked List Cycle
// ============================================================================
//
// PROBLEM (summary):
//   Given the head of a singly linked list, determine if the list has a
//   cycle in it. A cycle exists if some node's `next` pointer, followed
//   repeatedly, eventually leads back to a node already visited (there is
//   no "true" tail / nullptr terminator reachable).
//
//   Return true if there is a cycle, false otherwise. You must NOT use
//   O(n) extra memory (i.e. do not use a hash set of visited node
//   pointers) — the intended solution is O(1) space.
//
// APPROACH (Fast & Slow Pointers — Floyd's Tortoise and Hare):
//   Start both a `slow` pointer and a `fast` pointer at head. Advance
//   `slow` by one node per step and `fast` by two nodes per step. If the
//   list has no cycle, `fast` reaches nullptr first and we return false.
//   If the list DOES have a cycle, `fast` enters the cycle before `slow`
//   does (or at the same time), and because `fast` gains exactly one
//   node of relative distance on `slow` per iteration once both are
//   inside the cycle, `fast` cannot "jump over" `slow` — it must land on
//   the same node within at most (cycle length) iterations. That
//   collision is the signal.
//
//   This is the canonical, textbook use of the Fast & Slow Pointers
//   pattern — see the module README for the full derivation of why the
//   speed difference guarantees a meeting.
//
// COMPLEXITY:
//   Time:  O(n) — in the worst case (no cycle) fast walks the whole list
//          once; if a cycle exists, fast meets slow within one full lap
//          of the cycle, still bounded by O(n) total node visits.
//   Space: O(1) — only two pointers, regardless of list length. Contrast
//          with the hash-set approach, which needs O(n) space to record
//          every visited node's address.
//
// Compile:
//   g++ -std=c++17 -Wall 01-linked-list-cycle.cpp -o /tmp/out_p1 && /tmp/out_p1
// ============================================================================

#include <iostream>
#include <vector>

struct ListNode {
  int val;
  ListNode* next;
  ListNode(int x) : val(x), next(nullptr) {}
};

// Standalone helper: build a list from values, optionally wiring a cycle
// back to the node at `cycle_pos` (0-indexed). Pass -1 for no cycle.
ListNode* build_list(const std::vector<int>& values, int cycle_pos = -1) {
  if (values.empty()) return nullptr;

  ListNode* head = new ListNode(values[0]);
  ListNode* tail = head;
  ListNode* cycle_target = (cycle_pos == 0) ? head : nullptr;

  for (size_t i = 1; i < values.size(); ++i) {
    tail->next = new ListNode(values[i]);
    tail = tail->next;
    if (static_cast<int>(i) == cycle_pos) cycle_target = tail;
  }

  if (cycle_pos >= 0 && cycle_target != nullptr) {
    tail->next = cycle_target;
  }

  return head;
}

// Only safe to call on a NON-cyclic list — walking `next` on a cyclic list
// here would loop forever.
void free_list(ListNode* head) {
  while (head != nullptr) {
    ListNode* next = head->next;
    delete head;
    head = next;
  }
}

// ----------------------------------------------------------------------------
// The solution itself.
// ----------------------------------------------------------------------------
bool hasCycle(ListNode* head) {
  ListNode* slow = head;
  ListNode* fast = head;

  // Guarding BOTH fast and fast->next avoids dereferencing a null pointer
  // on the very last node of an odd-length, non-cyclic list.
  while (fast != nullptr && fast->next != nullptr) {
    slow = slow->next;
    fast = fast->next->next;
    if (slow == fast) return true;
  }

  return false;
}

// ----------------------------------------------------------------------------
// Test harness: run against known cases and print PASS/FAIL.
// ----------------------------------------------------------------------------
void check(const std::string& name, bool actual, bool expected) {
  std::cout << (actual == expected ? "PASS" : "FAIL") << " — " << name
             << " (got " << std::boolalpha << actual << ", expected "
             << expected << ")\n";
}

int main() {
  std::cout << "=== LeetCode 141: Linked List Cycle ===\n\n";

  // Case 1: classic example, cycle back to index 1 (value 2).
  {
    ListNode* head = build_list({3, 2, 0, -4}, /*cycle_pos=*/1);
    check("cycle at index 1", hasCycle(head), true);
    // Cyclic — not freed (see free_list()'s doc comment).
  }

  // Case 2: two-node list, cycle back to head.
  {
    ListNode* head = build_list({1, 2}, /*cycle_pos=*/0);
    check("two-node cycle back to head", hasCycle(head), true);
  }

  // Case 3: single node, no cycle (next is nullptr).
  {
    ListNode* head = build_list({1});
    check("single node, no cycle", hasCycle(head), false);
    free_list(head);
  }

  // Case 4: plain list, no cycle.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    check("five-node list, no cycle", hasCycle(head), false);
    free_list(head);
  }

  // Case 5: empty list.
  {
    ListNode* head = build_list({});
    check("empty list", hasCycle(head), false);
  }

  return 0;
}
