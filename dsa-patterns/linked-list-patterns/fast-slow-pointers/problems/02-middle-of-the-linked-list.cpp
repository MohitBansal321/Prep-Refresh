// ============================================================================
// LeetCode 876 — Middle of the Linked List
// ============================================================================
//
// PROBLEM (summary):
//   Given the head of a singly linked list, return the middle node. If
//   there are two middle nodes (even length), return the SECOND middle
//   node.
//
//   Example: [1,2,3,4,5] -> middle is 3.
//            [1,2,3,4,5,6] -> middle is 4 (the second of the two middles).
//
// APPROACH (Fast & Slow Pointers):
//   Move `slow` one step and `fast` two steps per iteration, starting
//   both at head. By the time `fast` cannot advance two more steps
//   (either fast == nullptr or fast->next == nullptr), `fast` has
//   traveled exactly twice as far as `slow`. Since `fast` covers the
//   whole list length, `slow` has covered half of it — landing exactly
//   on the middle. This avoids the two-pass brute force of (1) counting
//   the length, then (2) walking length/2 steps.
//
//   The loop condition `fast != nullptr && fast->next != nullptr` is
//   exactly what makes this return the SECOND middle on even-length
//   lists — see the README's Common Mistakes section for the off-by-one
//   that happens if you get this condition wrong.
//
// COMPLEXITY:
//   Time:  O(n) — one pass, fast pointer visits each node at most once
//          (conceptually) as it moves through the list.
//   Space: O(1) — two pointers only. Contrast with the brute-force
//          "count nodes, then walk again" approach, which is also O(n)
//          time but requires TWO full passes over the list; fast/slow
//          does it in a single pass.
//
// Compile:
//   g++ -std=c++17 -Wall 02-middle-of-the-linked-list.cpp -o /tmp/out_p2 && /tmp/out_p2
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

// ----------------------------------------------------------------------------
// The solution itself.
// ----------------------------------------------------------------------------
ListNode* middleNode(ListNode* head) {
  ListNode* slow = head;
  ListNode* fast = head;

  while (fast != nullptr && fast->next != nullptr) {
    slow = slow->next;
    fast = fast->next->next;
  }

  return slow;
}

// ----------------------------------------------------------------------------
// Test harness.
// ----------------------------------------------------------------------------
void check(const std::string& name, int actual, int expected) {
  std::cout << (actual == expected ? "PASS" : "FAIL") << " — " << name
             << " (got " << actual << ", expected " << expected << ")\n";
}

int main() {
  std::cout << "=== LeetCode 876: Middle of the Linked List ===\n\n";

  // Case 1: odd length -> single exact middle.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    check("odd length [1..5] -> 3", middleNode(head)->val, 3);
    free_list(head);
  }

  // Case 2: even length -> second of the two middles.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5, 6});
    check("even length [1..6] -> 4", middleNode(head)->val, 4);
    free_list(head);
  }

  // Case 3: single node.
  {
    ListNode* head = build_list({7});
    check("single node [7] -> 7", middleNode(head)->val, 7);
    free_list(head);
  }

  // Case 4: two nodes -> second node.
  {
    ListNode* head = build_list({1, 2});
    check("two nodes [1,2] -> 2", middleNode(head)->val, 2);
    free_list(head);
  }

  // Case 5: longer even list.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5, 6, 7, 8});
    check("eight nodes [1..8] -> 5", middleNode(head)->val, 5);
    free_list(head);
  }

  return 0;
}
