// ============================================================================
// Fast & Slow Pointers — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// two things you will re-derive on almost every problem that fits this
// pattern:
//
//   1. CYCLE DETECTION (Floyd's Tortoise and Hare) — a slow pointer moves
//      one step at a time, a fast pointer moves two steps at a time. If
//      there is a cycle, the fast pointer eventually laps the slow one and
//      they occupy the same node. If there is no cycle, the fast pointer
//      reaches the end (nullptr) first.
//
//   2. FINDING THE MIDDLE — the same two-speed idea, except now we do not
//      care about meeting; we care that when the fast pointer (moving 2x)
//      falls off the end, the slow pointer (moving 1x) is standing exactly
//      halfway through the list. No need to count the length first.
//
// Both variants are expressed as small, generic, reusable function templates
// so you can see the *shape* of the pattern independent of any one problem.
// The worked, problem-specific solutions live in problems/*.cpp.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_fsp_code && /tmp/out_fsp_code
// ============================================================================

#include <iostream>
#include <vector>

// ----------------------------------------------------------------------------
// The node type shared by every example in this file. Deliberately minimal:
// one payload (`val`) and one link (`next`) — the only two things a singly
// linked list needs.
// ----------------------------------------------------------------------------
struct ListNode {
  int val;
  ListNode* next;
  ListNode(int x) : val(x), next(nullptr) {}
};

// ----------------------------------------------------------------------------
// Helper: build a singly linked list from a std::vector<int>.
//
// If `cycle_pos` is provided (>= 0), the last node's `next` is rewired to
// point back at the node at index `cycle_pos` (0-indexed), turning the list
// into a cyclic structure for testing cycle detection. Pass -1 (default) for
// a plain, nullptr-terminated list.
//
// Returns the head pointer. Caller owns the memory; for a non-cyclic list
// that means calling free_list() when done. A cyclic list can never be
// walked to a "last node" safely with a simple loop, so we do not attempt
// to free those in this demo (test data is small and process exits after
// main() anyway — see the note on cyclic lists in the README's Common
// Mistakes section).
// ----------------------------------------------------------------------------
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
    tail->next = cycle_target;  // Wire the tail back to create a cycle.
  }

  return head;
}

// ----------------------------------------------------------------------------
// Helper: free every node of a NON-CYCLIC list. Walking `next` until nullptr
// is only safe when there is no cycle — calling this on a cyclic list would
// loop forever, which is itself a good illustration of why cycle detection
// matters before you do anything else with an untrusted list.
// ----------------------------------------------------------------------------
void free_list(ListNode* head) {
  while (head != nullptr) {
    ListNode* next = head->next;
    delete head;
    head = next;
  }
}

// ----------------------------------------------------------------------------
// Generic template #1: Floyd's cycle detection.
//
// Returns true if the list starting at `head` contains a cycle, false
// otherwise. This is the "does the fast pointer ever meet the slow pointer"
// check — the foundation every other fast/slow-pointer routine builds on.
//
// Invariant: `fast` is always at or ahead of `slow` by an even number of
// steps, so if a cycle exists, `fast` re-enters it and closes the gap by
// exactly one node per iteration (relative speed = 2 - 1 = 1), guaranteeing
// a meeting rather than an infinite skip-over.
// ----------------------------------------------------------------------------
template <typename Node>
bool has_cycle(Node* head) {
  Node* slow = head;
  Node* fast = head;

  // Guard BOTH `fast` and `fast->next` before dereferencing twice per step.
  // Skipping either half of this check is the single most common bug in
  // fast/slow-pointer code (null dereference on odd-length non-cyclic
  // lists) — see the README's Common Mistakes section.
  while (fast != nullptr && fast->next != nullptr) {
    slow = slow->next;
    fast = fast->next->next;
    if (slow == fast) return true;  // Fast lapped slow inside a cycle.
  }

  return false;  // fast (or fast->next) hit nullptr: the list terminates.
}

// ----------------------------------------------------------------------------
// Generic template #2: Floyd's cycle detection, phase 2 — find the START
// node of the cycle (not just whether one exists).
//
// Classic result: once slow and fast meet inside the cycle, resetting one
// pointer to `head` and advancing BOTH pointers one step at a time makes
// them meet again exactly at the first node of the cycle. The proof rests
// on the arithmetic of how far into the cycle the first meeting point is
// relative to the distance from head to the cycle's start — spelled out in
// the README's Execution Flow section.
//
// Returns the cycle's entry node, or nullptr if there is no cycle.
// ----------------------------------------------------------------------------
template <typename Node>
Node* find_cycle_start(Node* head) {
  Node* slow = head;
  Node* fast = head;
  bool cycle_found = false;

  while (fast != nullptr && fast->next != nullptr) {
    slow = slow->next;
    fast = fast->next->next;
    if (slow == fast) {
      cycle_found = true;
      break;
    }
  }

  if (!cycle_found) return nullptr;

  // Phase 2: reset one pointer to head, advance both at speed 1.
  slow = head;
  while (slow != fast) {
    slow = slow->next;
    fast = fast->next;
  }

  return slow;  // slow == fast == cycle entry node.
}

// ----------------------------------------------------------------------------
// Generic template #3: find the middle node of a (non-cyclic) list.
//
// When `fast` reaches the end (nullptr, or fast->next == nullptr for an
// even-length list), `slow` — having moved half as far — is standing at
// the middle. For an EVEN-length list this returns the SECOND of the two
// middle nodes (the common convention, matching LeetCode 876); adjust the
// loop condition if your problem wants the first middle instead (see the
// Common Mistakes section on this exact off-by-one).
// ----------------------------------------------------------------------------
template <typename Node>
Node* find_middle(Node* head) {
  Node* slow = head;
  Node* fast = head;

  while (fast != nullptr && fast->next != nullptr) {
    slow = slow->next;
    fast = fast->next->next;
  }

  return slow;
}

// ----------------------------------------------------------------------------
// Small printing helper for demo output.
// ----------------------------------------------------------------------------
void print_list(ListNode* head, int max_nodes = 20) {
  int count = 0;
  while (head != nullptr && count < max_nodes) {
    std::cout << head->val;
    head = head->next;
    if (head != nullptr && count + 1 < max_nodes) std::cout << " -> ";
    ++count;
  }
  std::cout << (count == max_nodes ? " -> ... (truncated, likely cyclic)" : "");
  std::cout << "\n";
}

int main() {
  std::cout << "=== Fast & Slow Pointers: generic template demo ===\n\n";

  // ---- Test 1: cycle detection on a list WITH a cycle ----------------------
  // 1 -> 2 -> 3 -> 4 -> 5 -> back to node holding 3 (index 2)
  {
    ListNode* head = build_list({1, 2, 3, 4, 5}, /*cycle_pos=*/2);
    std::cout << "Test 1 (cyclic list, tail points back to value 3):\n";
    std::cout << "  has_cycle(head) = " << std::boolalpha << has_cycle(head)
               << " (expected: true)\n";
    ListNode* start = find_cycle_start(head);
    std::cout << "  find_cycle_start(head)->val = "
               << (start != nullptr ? std::to_string(start->val) : "null")
               << " (expected: 3)\n\n";
    // Not freeing: this list is cyclic, see free_list()'s doc comment.
  }

  // ---- Test 2: cycle detection on a list WITHOUT a cycle --------------------
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    std::cout << "Test 2 (plain list, no cycle):\n";
    std::cout << "  List: ";
    print_list(head);
    std::cout << "  has_cycle(head) = " << std::boolalpha << has_cycle(head)
               << " (expected: false)\n";
    ListNode* start = find_cycle_start(head);
    std::cout << "  find_cycle_start(head) = "
               << (start != nullptr ? std::to_string(start->val) : "null")
               << " (expected: null)\n\n";
    free_list(head);
  }

  // ---- Test 3: middle of an ODD-length list ---------------------------------
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    std::cout << "Test 3 (odd-length list [1,2,3,4,5]):\n";
    std::cout << "  List: ";
    print_list(head);
    ListNode* mid = find_middle(head);
    std::cout << "  find_middle(head)->val = " << mid->val
               << " (expected: 3)\n\n";
    free_list(head);
  }

  // ---- Test 4: middle of an EVEN-length list --------------------------------
  {
    ListNode* head = build_list({1, 2, 3, 4, 5, 6});
    std::cout << "Test 4 (even-length list [1,2,3,4,5,6]):\n";
    std::cout << "  List: ";
    print_list(head);
    ListNode* mid = find_middle(head);
    std::cout << "  find_middle(head)->val = " << mid->val
               << " (expected: 4, the SECOND of the two middle nodes)\n\n";
    free_list(head);
  }

  // ---- Test 5: single-node and empty-list edge cases -------------------------
  {
    ListNode* single = build_list({42});
    std::cout << "Test 5a (single node [42]):\n";
    std::cout << "  has_cycle = " << std::boolalpha << has_cycle(single)
               << " (expected: false)\n";
    std::cout << "  find_middle->val = " << find_middle(single)->val
               << " (expected: 42)\n";
    free_list(single);

    ListNode* empty = build_list({});
    std::cout << "Test 5b (empty list):\n";
    std::cout << "  has_cycle = " << std::boolalpha << has_cycle(empty)
               << " (expected: false)\n";
    std::cout << "  find_middle = "
               << (find_middle(empty) == nullptr ? "null" : "not null")
               << " (expected: null)\n";
    // Nothing allocated for an empty list — nothing to free.
  }

  std::cout << "\nAll demo cases printed above. Compare each result against\n"
               "its \"expected\" comment to confirm the template behaves.\n";

  return 0;
}
