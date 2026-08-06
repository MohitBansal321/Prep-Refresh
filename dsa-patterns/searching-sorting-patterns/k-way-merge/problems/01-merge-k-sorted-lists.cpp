// ============================================================================
// LeetCode 23 — Merge k Sorted Lists (Hard)
// ============================================================================
//
// PROBLEM
// -------
// You are given an array of k linked-lists `lists`, each linked-list is
// sorted in ascending order. Merge all the linked-lists into one sorted
// linked list and return its head.
//
// Example: lists = [[1,4,5],[1,3,4],[2,6]] -> [1,1,2,3,4,4,5,6]
//
// APPROACH — K-way Merge (min-heap over one node per list)
// ----------------------------------------------------------
// This is the canonical K-way Merge problem, just over linked lists instead
// of vectors: each of the k lists is already sorted, and we need one fully
// merged, sorted list. Rather than tagging heap entries with
// (list_index, element_index) as we do for vector inputs (see ../code.cpp),
// a linked list makes this even simpler: the NODE POINTER ITSELF already
// tells us both "the current value" (node->val) and "where the next
// candidate from this source comes from" (node->next). So the heap only
// needs to hold node pointers, compared by their `val`.
//
// Steps:
//   1. Push the head node of every non-empty input list onto a min-heap,
//      ordered by node->val.
//   2. Repeatedly pop the node with the smallest val, append it to the
//      output list (by relinking, not copying — we reuse the existing
//      nodes).
//   3. If the popped node has a `next`, push that `next` onto the heap —
//      this is the "advance the source pointer" step from the README's
//      Architecture section, expressed as "follow this node's own next
//      pointer" instead of incrementing a separate index variable.
//   4. Continue until the heap is empty.
//
// COMPLEXITY
// ----------
// Let n = total number of nodes across all k lists.
// Time:  O(n log k) — n pops/pushes total, each an O(log k) heap operation
//        because the heap never holds more than k nodes at once (one per
//        still-active list).
// Space: O(k) for the heap, plus O(1) extra for the output (nodes are
//        relinked, not copied) — contrast with concatenating all node
//        values into one array and sorting, which would cost O(n log n)
//        time and O(n) extra space for that array.
// ============================================================================

#include <iostream>
#include <queue>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Standard singly linked list node, matching the convention used throughout
// this repo's linked-list-pattern modules (see
// ../../../linked-list-patterns/fast-slow-pointers/code.cpp).
// ----------------------------------------------------------------------------
struct ListNode {
  int val;
  ListNode* next;
  ListNode(int x) : val(x), next(nullptr) {}
};

// ----------------------------------------------------------------------------
// Helper: build a singly linked list from a std::vector<int>. Returns the
// head pointer, or nullptr for an empty vector. Caller owns the memory.
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
// Helper: free every node of a (non-cyclic) list.
// ----------------------------------------------------------------------------
void free_list(ListNode* head) {
  while (head != nullptr) {
    ListNode* next = head->next;
    delete head;
    head = next;
  }
}

// ----------------------------------------------------------------------------
// Helper: collect a list's values into a std::vector<int>, for comparison
// against expected output in tests.
// ----------------------------------------------------------------------------
std::vector<int> list_to_vector(ListNode* head) {
  std::vector<int> result;
  while (head != nullptr) {
    result.push_back(head->val);
    head = head->next;
  }
  return result;
}

// ----------------------------------------------------------------------------
// Comparator that makes std::priority_queue<ListNode*> behave as a MIN-heap
// ordered by node->val. std::priority_queue is a max-heap by default; this
// struct's operator() is used as the "is a lower priority than" test, and
// returning (a->val > b->val) inverts the default max-heap into a min-heap —
// the same effect as std::greater<> in ../code.cpp, spelled out explicitly
// here because we are comparing pointers by a field, not comparing plain
// values directly.
// ----------------------------------------------------------------------------
struct CompareNodeVal {
  bool operator()(const ListNode* a, const ListNode* b) const {
    return a->val > b->val;
  }
};

// ----------------------------------------------------------------------------
// Merge k sorted linked lists into one sorted linked list.
// ----------------------------------------------------------------------------
ListNode* mergeKLists(std::vector<ListNode*>& lists) {
  std::priority_queue<ListNode*, std::vector<ListNode*>, CompareNodeVal> heap;

  // Seed: push the head of every non-empty list.
  for (ListNode* head : lists) {
    if (head != nullptr) heap.push(head);
  }

  ListNode dummy(0);  // Dummy head simplifies "attach to the tail" logic —
                      // no special case needed for the very first node.
  ListNode* tail = &dummy;

  while (!heap.empty()) {
    ListNode* smallest = heap.top();
    heap.pop();

    tail->next = smallest;  // Relink the existing node into the output —
    tail = tail->next;      // no new node is ever allocated here.

    // Advance the source pointer: if this node has a next node in its
    // original list, that next node is the new candidate from this source.
    if (smallest->next != nullptr) {
      heap.push(smallest->next);
    }
  }

  tail->next = nullptr;  // Terminate the merged list explicitly.
  return dummy.next;
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](bool condition, const std::string& label) {
    if (condition) {
      std::cout << "[PASS] " << label << "\n";
      ++pass_count;
    } else {
      std::cout << "[FAIL] " << label << "\n";
      ++fail_count;
    }
  };

  // ---- Test 1: three lists, the classic example ------------------------------
  {
    std::vector<ListNode*> lists = {
        build_list({1, 4, 5}),
        build_list({1, 3, 4}),
        build_list({2, 6}),
    };
    ListNode* merged = mergeKLists(lists);
    std::vector<int> expected = {1, 1, 2, 3, 4, 4, 5, 6};
    check(list_to_vector(merged) == expected, "[1,4,5],[1,3,4],[2,6] -> [1,1,2,3,4,4,5,6]");
    free_list(merged);
  }

  // ---- Test 2: empty input array ----------------------------------------------
  {
    std::vector<ListNode*> lists = {};
    ListNode* merged = mergeKLists(lists);
    check(merged == nullptr, "empty lists array -> nullptr");
    // Nothing allocated, nothing to free.
  }

  // ---- Test 3: array containing only empty lists (nullptr entries) -----------
  {
    std::vector<ListNode*> lists = {nullptr, nullptr};
    ListNode* merged = mergeKLists(lists);
    check(merged == nullptr, "array of only nullptr lists -> nullptr");
  }

  // ---- Test 4: a mix of empty and non-empty lists -----------------------------
  {
    std::vector<ListNode*> lists = {
        nullptr,
        build_list({5, 10}),
        build_list({}),  // nullptr, same as an empty list
        build_list({1, 2, 3}),
    };
    ListNode* merged = mergeKLists(lists);
    std::vector<int> expected = {1, 2, 3, 5, 10};
    check(list_to_vector(merged) == expected, "mix of empty/non-empty lists merges correctly");
    free_list(merged);
  }

  // ---- Test 5: a single list (k = 1) -------------------------------------------
  {
    std::vector<ListNode*> lists = {build_list({7, 8, 9})};
    ListNode* merged = mergeKLists(lists);
    std::vector<int> expected = {7, 8, 9};
    check(list_to_vector(merged) == expected, "k=1 returns the single list unchanged");
    free_list(merged);
  }

  // ---- Test 6: duplicate values across lists -----------------------------------
  {
    std::vector<ListNode*> lists = {
        build_list({1, 1, 1}),
        build_list({1, 1}),
    };
    ListNode* merged = mergeKLists(lists);
    std::vector<int> expected = {1, 1, 1, 1, 1};
    check(list_to_vector(merged) == expected, "all-duplicate values merge to the right count/order");
    free_list(merged);
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
