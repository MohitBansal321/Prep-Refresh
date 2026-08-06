// ============================================================================
// LeetCode 25 — Reverse Nodes in k-Group (Hard)
// ============================================================================
//
// PROBLEM (summary):
//   Given the head of a linked list and an integer k, reverse the nodes of
//   the list k at a time and return the modified list. If the number of
//   remaining nodes is not a multiple of k, leave the final, shorter group
//   as-is (untouched, in its original order). You may not just change the
//   `val` inside each node — the actual node objects must be rearranged.
//   Do it in O(n) time and O(1) extra space.
//
// APPROACH (In-place Reversal, k-group case):
//   This is the "fiddliest" of the three shapes this pattern takes, and the
//   difficulty rating (Hard) reflects the bookkeeping, not a new algorithmic
//   idea. It is sub-range reversal (LeetCode 92) applied repeatedly, group
//   after group, with one extra check per group:
//     1. Before reversing a group, walk `k` steps ahead to confirm at least
//        k nodes actually remain. If you run out before taking k steps,
//        STOP — a partial group must be left untouched, never reversed.
//     2. Reverse exactly those k nodes with the standard three-pointer loop
//        (prev / curr / next), seeding `prev` with the node that will
//        follow the group so the new tail already points to the right
//        place once reversed.
//     3. Reconnect: the node BEFORE the group must now point at the group's
//        new head (the old k-th node); the group's new tail (the old first
//        node) already points at what follows, from step 2.
//     4. Advance the "node before the group" cursor to the new tail, and
//        repeat for the next group.
//   A dummy head node makes step 3 uniform even for the very first group
//   (which starts at the list's true head).
//
// COMPLEXITY:
//   Time:  O(n) — every node is visited a constant number of times: once
//          during the "count k nodes ahead" check and once during the
//          reversal itself.
//   Space: O(1) — a dummy node plus a fixed number of pointers, regardless
//          of list length or k. (A recursive formulation is also common and
//          reads cleanly, but each recursive call adds a stack frame —
//          O(n/k) call-stack depth — so it is not truly O(1) auxiliary
//          space; the iterative version here avoids that entirely.)
//
// Compile:
//   g++ -std=c++17 -Wall 03-reverse-nodes-in-k-group.cpp -o /tmp/out_p3 && /tmp/out_p3
// ============================================================================

#include <iostream>
#include <vector>

struct ListNode {
  int val;
  ListNode* next;
  ListNode(int x) : val(x), next(nullptr) {}
};

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

void free_list(ListNode* head) {
  while (head != nullptr) {
    ListNode* next = head->next;
    delete head;
    head = next;
  }
}

std::vector<int> to_vector(ListNode* head) {
  std::vector<int> out;
  while (head != nullptr) {
    out.push_back(head->val);
    head = head->next;
  }
  return out;
}

// ----------------------------------------------------------------------------
// The solution: iterative, dummy-node-anchored, group-by-group reversal.
// ----------------------------------------------------------------------------
ListNode* reverseKGroup(ListNode* head, int k) {
  if (head == nullptr || k <= 1) return head;

  ListNode dummy(0);
  dummy.next = head;
  ListNode* groupPrev = &dummy;  // Node just before the current group.

  while (true) {
    // Confirm a full group of k nodes exists starting after groupPrev.
    ListNode* kth = groupPrev;
    for (int i = 0; i < k && kth != nullptr; ++i) {
      kth = kth->next;
    }
    if (kth == nullptr) break;  // Fewer than k nodes remain — stop, leave as-is.

    ListNode* groupStart = groupPrev->next;
    ListNode* nextGroupStart = kth->next;

    // Reverse exactly the k nodes from groupStart through kth.
    ListNode* prev = nextGroupStart;
    ListNode* curr = groupStart;
    while (curr != nextGroupStart) {
      ListNode* next = curr->next;
      curr->next = prev;
      prev = curr;
      curr = next;
    }

    // kth is the group's new head; groupStart is the group's new tail.
    groupPrev->next = kth;
    groupPrev = groupStart;
  }

  return dummy.next;
}

// ----------------------------------------------------------------------------
// Test harness.
// ----------------------------------------------------------------------------
void check(const std::string& name, const std::vector<int>& actual,
           const std::vector<int>& expected) {
  bool pass = (actual == expected);
  std::cout << (pass ? "PASS" : "FAIL") << " — " << name << " (got [";
  for (size_t i = 0; i < actual.size(); ++i) {
    std::cout << actual[i] << (i + 1 < actual.size() ? "," : "");
  }
  std::cout << "], expected [";
  for (size_t i = 0; i < expected.size(); ++i) {
    std::cout << expected[i] << (i + 1 < expected.size() ? "," : "");
  }
  std::cout << "])\n";
}

int main() {
  std::cout << "=== LeetCode 25: Reverse Nodes in k-Group ===\n\n";

  // Case 1: the canonical LeetCode example, k=2 (length is a multiple of k).
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    head = reverseKGroup(head, 2);
    check("[1,2,3,4,5], k=2", to_vector(head), {2, 1, 4, 3, 5});
    free_list(head);
  }

  // Case 2: the canonical LeetCode example, k=3 (trailing short group of 2
  // nodes must be left untouched).
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    head = reverseKGroup(head, 3);
    check("[1,2,3,4,5], k=3", to_vector(head), {3, 2, 1, 4, 5});
    free_list(head);
  }

  // Case 3: k equals the list length exactly — the whole list reverses as
  // one group.
  {
    ListNode* head = build_list({1, 2, 3, 4});
    head = reverseKGroup(head, 4);
    check("[1,2,3,4], k=4 (whole list)", to_vector(head), {4, 3, 2, 1});
    free_list(head);
  }

  // Case 4: k larger than the list length — no group is complete, so the
  // list is returned unchanged.
  {
    ListNode* head = build_list({1, 2, 3});
    head = reverseKGroup(head, 5);
    check("[1,2,3], k=5 (too few nodes)", to_vector(head), {1, 2, 3});
    free_list(head);
  }

  // Case 5: k == 1 is defined as a no-op (reversing "groups of 1" changes
  // nothing).
  {
    ListNode* head = build_list({1, 2, 3, 4});
    head = reverseKGroup(head, 1);
    check("[1,2,3,4], k=1 (no-op)", to_vector(head), {1, 2, 3, 4});
    free_list(head);
  }

  // Case 6: single-node list.
  {
    ListNode* head = build_list({9});
    head = reverseKGroup(head, 2);
    check("[9], k=2 (too few nodes)", to_vector(head), {9});
    free_list(head);
  }

  return 0;
}
