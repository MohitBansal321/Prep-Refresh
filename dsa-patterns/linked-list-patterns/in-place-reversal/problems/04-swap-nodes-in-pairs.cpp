// ============================================================================
// LeetCode 24 — Swap Nodes in Pairs
// ============================================================================
//
// PROBLEM (summary):
//   Given the head of a linked list, swap every two adjacent nodes and
//   return the head of the modified list. You must actually rearrange the
//   nodes themselves (not just swap their `val` fields), and you may only
//   use O(1) extra space.
//
// APPROACH (In-place Reversal, k-group case with k=2):
//   This problem IS reverseKGroup(head, 2) — LeetCode presents it as a
//   separate, easier problem (LeetCode 24 predates 25 and is the "warm up"
//   version), but structurally it is the exact same operation: reverse the
//   list in fixed-size groups, where the group size happens to be 2. If a
//   single node is left over at the end (odd-length list), it is left
//   untouched — exactly the "final short group" rule from k-group reversal.
//
//   This file implements it directly (not by calling a generic
//   reverseKGroup, since every file in problems/ is standalone), using the
//   same dummy-node + three-pointer machinery, specialized for pairs:
//     1. `groupPrev` starts at a dummy node before the true head.
//     2. At each step, confirm two nodes remain: `first = groupPrev->next`,
//        `second = first->next`. If `second` is nullptr, there is no pair
//        left to swap — stop.
//     3. Rewire exactly three links to swap the pair:
//          groupPrev->next = second;   // second becomes this pair's head
//          first->next     = second->next;  // first now follows the pair
//          second->next    = first;    // second now points at first
//     4. Advance `groupPrev` to `first` (now the tail of this pair) and
//        repeat for the next pair.
//
// COMPLEXITY:
//   Time:  O(n) — each node participates in exactly one pair-swap (or is
//          the untouched final odd node).
//   Space: O(1) — a dummy node plus a fixed number of pointers. A recursive
//          formulation is common in textbooks and reads cleanly for this
//          specific k=2 case, but it adds O(n/2) call-stack depth; the
//          iterative version here has none.
//
// Compile:
//   g++ -std=c++17 -Wall 04-swap-nodes-in-pairs.cpp -o /tmp/out_p4 && /tmp/out_p4
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
// The solution: iterative, dummy-node-anchored pairwise swap
// (reverseKGroup specialized for k=2).
// ----------------------------------------------------------------------------
ListNode* swapPairs(ListNode* head) {
  ListNode dummy(0);
  dummy.next = head;
  ListNode* groupPrev = &dummy;

  while (groupPrev->next != nullptr && groupPrev->next->next != nullptr) {
    ListNode* first = groupPrev->next;
    ListNode* second = first->next;

    // Three rewires swap the pair in place:
    first->next = second->next;  // `first` now follows the swapped pair.
    second->next = first;        // `second` now leads, pointing at `first`.
    groupPrev->next = second;    // The node before the pair points at the
                                 // new head of the pair (`second`).

    groupPrev = first;  // `first` is now the tail of this pair; advance.
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
  std::cout << "=== LeetCode 24: Swap Nodes in Pairs ===\n\n";

  // Case 1: the canonical LeetCode example, even length.
  {
    ListNode* head = build_list({1, 2, 3, 4});
    head = swapPairs(head);
    check("[1,2,3,4] swapped in pairs", to_vector(head), {2, 1, 4, 3});
    free_list(head);
  }

  // Case 2: odd length — the trailing single node is left untouched.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    head = swapPairs(head);
    check("[1,2,3,4,5] swapped in pairs", to_vector(head), {2, 1, 4, 3, 5});
    free_list(head);
  }

  // Case 3: single pair only.
  {
    ListNode* head = build_list({1, 2});
    head = swapPairs(head);
    check("[1,2] swapped in pairs", to_vector(head), {2, 1});
    free_list(head);
  }

  // Case 4: single-node list — no pair to swap, unchanged.
  {
    ListNode* head = build_list({1});
    head = swapPairs(head);
    check("[1] swapped in pairs (no-op)", to_vector(head), {1});
    free_list(head);
  }

  // Case 5: empty list.
  {
    ListNode* head = build_list({});
    head = swapPairs(head);
    check("[] swapped in pairs (no-op)", to_vector(head), {});
    free_list(head);
  }

  // Case 6: this is exactly reverseKGroup(head, 2) — six nodes, three pairs.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5, 6});
    head = swapPairs(head);
    check("[1,2,3,4,5,6] swapped in pairs", to_vector(head),
          {2, 1, 4, 3, 6, 5});
    free_list(head);
  }

  return 0;
}
