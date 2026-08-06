// ============================================================================
// LeetCode 92 — Reverse Linked List II
// ============================================================================
//
// PROBLEM (summary):
//   Given the head of a singly linked list and two integers `left` and
//   `right` where 1 <= left <= right <= length, reverse only the nodes from
//   position `left` to position `right` (1-indexed, inclusive), then return
//   the head of the modified list. Do it in one pass, O(n) time, O(1) space.
//
// APPROACH (In-place Reversal, sub-range case):
//   Same three-pointer rewiring as whole-list reversal (LeetCode 206), but
//   now it only runs on the slice [left, right], and the two ends of that
//   slice must be correctly reconnected to whatever comes before and after
//   it:
//     1. Walk a pointer, `prevRange`, to the node just BEFORE position
//        `left`. A DUMMY head node is used so this works even when
//        `left == 1` (the range includes the true head) without a special
//        case — `dummy.next` always plays the role of "node before the
//        range."
//     2. Run the standard prev/curr/next reversal loop, but only for
//        exactly (right - left + 1) nodes.
//     3. After the loop, the OLD node at position `left` is now the TAIL of
//        the reversed slice, and the OLD node at position `right` is now
//        its HEAD. Reconnect: prevRange->next = new head; old-left-node
//        (the new tail)->next = whatever followed the old `right` node.
//   Skipping the dummy node is the single most common source of bugs here —
//   see the README's Common Mistakes for what goes wrong without it.
//
// COMPLEXITY:
//   Time:  O(n) — one pass: walk to `left`, reverse the slice, done.
//   Space: O(1) — a dummy node plus a fixed number of pointers.
//          The brute-force alternative (copy the whole list into a
//          std::vector<int>, reverse the slice with std::reverse, rebuild
//          the list) is O(n) space and touches the data twice as much.
//
// Compile:
//   g++ -std=c++17 -Wall 02-reverse-linked-list-ii.cpp -o /tmp/out_p2 && /tmp/out_p2
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
// The solution: dummy-node-anchored sub-range reversal.
// ----------------------------------------------------------------------------
ListNode* reverseBetween(ListNode* head, int left, int right) {
  ListNode dummy(0);
  dummy.next = head;

  // Step 1: find the node just before position `left`.
  ListNode* prevRange = &dummy;
  for (int i = 1; i < left; ++i) {
    prevRange = prevRange->next;
  }

  // Step 2: reverse exactly (right - left + 1) nodes starting at `left`.
  ListNode* rangeTail = prevRange->next;  // Old node at `left`; new tail.
  ListNode* prev = nullptr;
  ListNode* curr = rangeTail;
  for (int i = 0; i <= right - left; ++i) {
    ListNode* next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
  }

  // Step 3: reconnect. `prev` is the new head of the range (old node at
  // `right`); `curr` is the first node AFTER the range (or nullptr).
  prevRange->next = prev;
  rangeTail->next = curr;

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
  std::cout << "=== LeetCode 92: Reverse Linked List II ===\n\n";

  // Case 1: the canonical LeetCode example.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    head = reverseBetween(head, 2, 4);
    check("[1,2,3,4,5], left=2, right=4", to_vector(head), {1, 4, 3, 2, 5});
    free_list(head);
  }

  // Case 2: range starts at the true head (left == 1) — the dummy-node
  // case that must not need a special branch.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    head = reverseBetween(head, 1, 3);
    check("[1,2,3,4,5], left=1, right=3", to_vector(head), {3, 2, 1, 4, 5});
    free_list(head);
  }

  // Case 3: range ends at the true tail.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    head = reverseBetween(head, 3, 5);
    check("[1,2,3,4,5], left=3, right=5", to_vector(head), {1, 2, 5, 4, 3});
    free_list(head);
  }

  // Case 4: whole list is the range (left=1, right=length) — degenerates to
  // LeetCode 206.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    head = reverseBetween(head, 1, 5);
    check("[1,2,3,4,5], left=1, right=5", to_vector(head), {5, 4, 3, 2, 1});
    free_list(head);
  }

  // Case 5: left == right (single-node "range") — must be a no-op.
  {
    ListNode* head = build_list({1, 2, 3});
    head = reverseBetween(head, 2, 2);
    check("[1,2,3], left=2, right=2 (no-op)", to_vector(head), {1, 2, 3});
    free_list(head);
  }

  // Case 6: single-node list, left == right == 1.
  {
    ListNode* head = build_list({9});
    head = reverseBetween(head, 1, 1);
    check("[9], left=1, right=1", to_vector(head), {9});
    free_list(head);
  }

  return 0;
}
