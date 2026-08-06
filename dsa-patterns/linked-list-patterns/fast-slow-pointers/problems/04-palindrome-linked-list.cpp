// ============================================================================
// LeetCode 234 — Palindrome Linked List
// ============================================================================
//
// PROBLEM (summary):
//   Given the head of a singly linked list, return true if it is a
//   palindrome (reads the same forwards and backwards), false otherwise.
//   The follow-up constraint that makes this interesting: do it in O(n)
//   time and O(1) extra space (i.e. do NOT copy the values into a
//   std::vector and check it there, which would be O(n) space).
//
// APPROACH (Fast & Slow Pointers COMBINED with In-place Reversal):
//   This problem is the textbook example of two Linked List patterns
//   composing:
//     1. FAST & SLOW POINTERS (this module) finds the middle of the list
//        in a single pass, in O(1) space — exactly the `find_middle`
//        template from code.cpp.
//     2. IN-PLACE REVERSAL (sibling pattern, see ../in-place-reversal/)
//        then reverses the SECOND half of the list in place, again in
//        O(1) space, so we get a pointer walking the second half
//        backwards without ever allocating a new list or array.
//     3. Walk two pointers — one from the original head, one from the
//        reversed second half's head — comparing values step by step.
//        If every pair matches, the list is a palindrome.
//
//   Why this composition matters: neither pattern alone solves the O(1)
//   space constraint as cleanly. Fast & Slow gets you the midpoint
//   cheaply; In-place Reversal gets you "backwards traversal" without
//   memory. Together they replace what would otherwise require an
//   O(n)-space array/stack to check the second half in reverse order.
//
// COMPLEXITY:
//   Time:  O(n) — one pass to find the middle, one pass to reverse the
//          second half, one pass to compare. Three passes, still linear
//          overall (3 * O(n) = O(n)).
//   Space: O(1) — only a constant number of pointers. Contrast with the
//          brute-force approach of copying all values into a
//          std::vector<int> and checking it with two converging indices,
//          which is O(n) time AND O(n) space.
//
// Compile:
//   g++ -std=c++17 -Wall 04-palindrome-linked-list.cpp -o /tmp/out_p4 && /tmp/out_p4
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
// Step 1 (Fast & Slow Pointers): find the middle. Same template as
// code.cpp's find_middle — for an even-length list this returns the
// SECOND of the two middle nodes, which is exactly where we want the
// second half to begin for this problem.
// ----------------------------------------------------------------------------
ListNode* find_middle(ListNode* head) {
  ListNode* slow = head;
  ListNode* fast = head;
  while (fast != nullptr && fast->next != nullptr) {
    slow = slow->next;
    fast = fast->next->next;
  }
  return slow;
}

// ----------------------------------------------------------------------------
// Step 2 (In-place Reversal — the sibling pattern): reverse the list
// starting at `head` in place, returning the new head. This is the
// standard three-pointer (prev / curr / next) in-place reversal; see
// ../in-place-reversal/ once that module is written for the deep dive.
// ----------------------------------------------------------------------------
ListNode* reverse_list(ListNode* head) {
  ListNode* prev = nullptr;
  ListNode* curr = head;
  while (curr != nullptr) {
    ListNode* next_node = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next_node;
  }
  return prev;
}

// ----------------------------------------------------------------------------
// The solution itself: combine both steps, then compare.
//
// Note: this version MUTATES the input list's second half by reversing
// it (matching the common LeetCode-accepted solution). A "purist" O(1)
// space solution would reverse it back before returning to restore the
// caller's original list; we do that here too, since leaving a caller's
// data structure mutated as a side effect is a real-world code smell.
// ----------------------------------------------------------------------------
bool isPalindrome(ListNode* head) {
  if (head == nullptr || head->next == nullptr) return true;  // 0 or 1 node.

  // Step 1: find the middle.
  ListNode* middle = find_middle(head);

  // Step 2: reverse the second half (from `middle` to the end).
  ListNode* second_half_head = reverse_list(middle);
  ListNode* second_half_head_saved = second_half_head;  // for restoring later

  // Step 3: compare first half (from head) against reversed second half.
  ListNode* p1 = head;
  ListNode* p2 = second_half_head;
  bool is_palindrome = true;
  while (p2 != nullptr) {  // second half is always <= first half in length
    if (p1->val != p2->val) {
      is_palindrome = false;
      break;
    }
    p1 = p1->next;
    p2 = p2->next;
  }

  // Restore the list to its original order (good citizenship — do not
  // leave the caller's structure mutated as a hidden side effect).
  reverse_list(second_half_head_saved);

  return is_palindrome;
}

// ----------------------------------------------------------------------------
// Test harness.
// ----------------------------------------------------------------------------
void check(const std::string& name, bool actual, bool expected) {
  std::cout << (actual == expected ? "PASS" : "FAIL") << " — " << name
             << " (got " << std::boolalpha << actual << ", expected "
             << expected << ")\n";
}

int main() {
  std::cout << "=== LeetCode 234: Palindrome Linked List ===\n\n";

  // Case 1: even-length palindrome.
  {
    ListNode* head = build_list({1, 2, 2, 1});
    check("[1,2,2,1] is palindrome", isPalindrome(head), true);
    free_list(head);
  }

  // Case 2: even-length, not a palindrome.
  {
    ListNode* head = build_list({1, 2});
    check("[1,2] is not palindrome", isPalindrome(head), false);
    free_list(head);
  }

  // Case 3: odd-length palindrome.
  {
    ListNode* head = build_list({1, 2, 3, 2, 1});
    check("[1,2,3,2,1] is palindrome", isPalindrome(head), true);
    free_list(head);
  }

  // Case 4: odd-length, not a palindrome.
  {
    ListNode* head = build_list({1, 2, 3, 4, 5});
    check("[1,2,3,4,5] is not palindrome", isPalindrome(head), false);
    free_list(head);
  }

  // Case 5: single node — trivially a palindrome.
  {
    ListNode* head = build_list({9});
    check("[9] is palindrome", isPalindrome(head), true);
    free_list(head);
  }

  // Case 6: verify the list is restored (not left reversed) after checking.
  {
    ListNode* head = build_list({1, 2, 3, 4});
    isPalindrome(head);  // mutate-and-restore internally
    bool restored_correctly = true;
    std::vector<int> expected = {1, 2, 3, 4};
    ListNode* curr = head;
    for (int v : expected) {
      if (curr == nullptr || curr->val != v) {
        restored_correctly = false;
        break;
      }
      curr = curr->next;
    }
    check("list order restored after isPalindrome([1,2,3,4])",
          restored_correctly, true);
    free_list(head);
  }

  return 0;
}
