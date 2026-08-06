// ============================================================================
// LeetCode 206 — Reverse Linked List
// ============================================================================
//
// PROBLEM (summary):
//   Given the head of a singly linked list, reverse the list and return the
//   new head. Do it in O(n) time and O(1) extra space — i.e. do NOT copy
//   values into a std::vector and build a new list from it.
//
// APPROACH (In-place Reversal, whole-list case):
//   This is the pattern in its purest form. Walk the list exactly once with
//   three pointers:
//     - `prev` trails behind, holding the already-reversed portion's new
//       head (starts at nullptr — the original head's `next` must end up
//       nullptr, since it becomes the new tail).
//     - `curr` walks forward through the still-unreversed portion.
//     - `next` saves `curr->next` BEFORE we overwrite it — this is the one
//       line that matters most: if you rewire `curr->next = prev` before
//       saving the original `curr->next`, you lose the rest of the list
//       forever (see the README's Common Mistakes).
//   Each iteration: save `next`, rewire `curr->next` to point at `prev`,
//   then slide both `prev` and `curr` one step forward.
//
// COMPLEXITY:
//   Time:  O(n) — every node is visited exactly once.
//   Space: O(1) — three pointer variables, regardless of list length.
//          Contrast with the brute-force approach of copying every value
//          into a std::vector<int> and building a brand-new reversed list
//          from it, which is O(n) time AND O(n) space.
//
// Compile:
//   g++ -std=c++17 -Wall 01-reverse-linked-list.cpp -o /tmp/out_p1 && /tmp/out_p1
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
// The solution: classic three-pointer in-place reversal of the whole list.
// ----------------------------------------------------------------------------
ListNode* reverseList(ListNode* head) {
  ListNode* prev = nullptr;
  ListNode* curr = head;

  while (curr != nullptr) {
    ListNode* next = curr->next;  // Save the rest of the list first.
    curr->next = prev;            // Rewire this node backwards.
    prev = curr;                  // Grow the reversed portion.
    curr = next;                  // Advance using the saved pointer.
  }

  return prev;
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
  std::cout << "=== LeetCode 206: Reverse Linked List ===\n\n";

  // Case 1: typical multi-node list.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    head = reverseList(head);
    check("[1,2,3,4,5] reversed", to_vector(head), {5, 4, 3, 2, 1});
    free_list(head);
  }

  // Case 2: two-node list.
  {
    ListNode* head = build_list({1, 2});
    head = reverseList(head);
    check("[1,2] reversed", to_vector(head), {2, 1});
    free_list(head);
  }

  // Case 3: single-node list — reversal is a no-op.
  {
    ListNode* head = build_list({7});
    head = reverseList(head);
    check("[7] reversed", to_vector(head), {7});
    free_list(head);
  }

  // Case 4: empty list.
  {
    ListNode* head = build_list({});
    head = reverseList(head);
    check("[] reversed", to_vector(head), {});
    free_list(head);
  }

  // Case 5: list with duplicate/negative values, to confirm we compare by
  // position (pointer rewiring), never by value.
  {
    ListNode* head = build_list({-1, 0, -1, 3, -1});
    head = reverseList(head);
    check("[-1,0,-1,3,-1] reversed", to_vector(head), {-1, 3, -1, 0, -1});
    free_list(head);
  }

  return 0;
}
