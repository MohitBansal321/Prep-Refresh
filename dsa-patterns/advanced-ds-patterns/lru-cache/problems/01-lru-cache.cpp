// ============================================================================
// LeetCode 146 — LRU Cache
// ============================================================================
//
// PROBLEM
// -------
// Design a data structure that follows the constraints of a Least Recently
// Used (LRU) cache.
//
// Implement the LRUCache class:
//   - LRUCache(int capacity): initialize with positive size capacity.
//   - int get(int key): return the value of the key if it exists, otherwise
//     return -1.
//   - void put(int key, int value): update the value of the key if it exists;
//     otherwise add the key-value pair. If the number of keys exceeds
//     capacity, evict the least recently used key.
//
// Both get and put must run in O(1) average time — and both operations count
// as a "use" of the key for recency purposes.
//
// Example 1:
//   ["LRUCache","put","put","get","put","get","put","get","get","get"]
//   [[2],[1,1],[2,2],[1],[3,3],[2],[4,4],[1],[3],[4]]
//   -> [null,null,null,1,null,-1,null,-1,3,4]
//
// APPROACH — hash map indexing into a sentinel doubly linked list
// ---------------------------------------------------------------
// The cache must answer two different questions on every operation:
//   1. "Where is this key?"        -> a lookup over an arbitrary key space.
//   2. "Which entry is oldest?"    -> an order question, changing on EVERY
//                                     access, including reads.
// No single standard container answers both: a hash map has no order; a list
// cannot look up a key in O(1). So compose them:
//
//   - std::unordered_map<int, Node*> map_ : key -> address of its list node.
//     The map stores POINTERS, never copies — there is one copy of each
//     entry, living in the node, so there is nothing to keep in sync.
//   - a doubly linked list of nodes kept in recency order: most recently
//     used at the front (just after head_), least recently used at the back
//     (just before tail_).
//
// Why DOUBLY linked? Unlinking a node you already hold requires fixing up its
// predecessor AND successor. A doubly linked node knows both neighbours, so
// unlink is two pointer writes. A singly linked node does not know its
// predecessor, so unlinking would mean scanning from the head: O(n) — the
// exact cost the design exists to remove.
//
// Why SENTINELS? Two permanent dummy nodes (head_, tail_) bracket every real
// entry, so every unlinked node is guaranteed non-null neighbours on both
// sides. unlink becomes branchless: no "is this the first/last element?"
// special case, no empty-list check, ever.
//
// Why store the KEY inside the node? Eviction starts from tail_->prev, which
// is only a Node*. To erase the victim from map_ you need its key. Without
// the field, going from Node* back to key is a linear search through the map.
//
// The coupling invariant — worth stating out loud in an interview:
//   map_ and the list contain exactly the same set of keys at all times,
//   and map_[k] always points at the unique list node whose key == k.
// Every operation below restores that invariant before returning.
//
// COMPLEXITY
// ----------
// Time:  O(1) average per operation. The list surgery is O(1) worst case
//               (a fixed number of pointer writes); the hash map is O(1)
//               average/amortized (rehashing occasionally costs O(n) for one
//               insertion, spread over all insertions).
// Space: O(capacity) — one node + one map entry per cached key, never more
//               by construction.
// ============================================================================

#include <iostream>
#include <string>
#include <unordered_map>

class LRUCache {
 public:
  explicit LRUCache(int capacity) : capacity_(capacity) {
    // Sentinels: dataless boundary markers. From here on the list is never
    // "empty" from the pointer-surgery code's point of view.
    head_ = new Node();
    tail_ = new Node();
    head_->next = tail_;
    tail_->prev = head_;
  }

  ~LRUCache() {
    // Walk the whole chain (sentinels included) and free everything.
    Node* cur = head_;
    while (cur != nullptr) {
      Node* next = cur->next;
      delete cur;
      cur = next;
    }
  }

  int get(int key) {
    std::unordered_map<int, Node*>::iterator it = map_.find(key);
    if (it == map_.end()) {
      return -1;  // Miss: order unchanged — nothing was "used".
    }
    // HIT. A read IS a use: move the node to the front. Skipping these two
    // lines still compiles and still evicts — but silently turns the cache
    // into FIFO instead of LRU.
    unlink(it->second);
    pushFront(it->second);
    return it->second->value;
  }

  void put(int key, int value) {
    std::unordered_map<int, Node*>::iterator it = map_.find(key);

    // ---- Path 1: overwrite an existing key ------------------------------
    if (it != map_.end()) {
      it->second->value = value;  // Update the single copy of the value...
      unlink(it->second);         // ...refresh its recency exactly like get...
      pushFront(it->second);
      return;                     // ...and RETURN EARLY. The count did not
                                  // grow, so eviction must NOT run here —
                                  // evicting would shrink the cache by one
                                  // on every update until it holds nothing.
    }

    // ---- Path 2: insert a new key ---------------------------------------
    // Check capacity BEFORE inserting: the count is about to grow.
    if (static_cast<int>(map_.size()) == capacity_) {
      // Evict first. tail_->prev IS the victim — no search needed.
      Node* lru = tail_->prev;
      unlink(lru);              // 1. out of the ordering,
      map_.erase(lru->key);     // 2. out of the index (reads key FROM node),
      delete lru;               // 3. free it. Order matters: erase before
                                //    delete, or we read freed memory.
    }
    Node* fresh = new Node();
    fresh->key = key;
    fresh->value = value;
    pushFront(fresh);           // New entry is the most-recently-used.
    map_[key] = fresh;          // Invariant restored: index agrees with list.
  }

 private:
  struct Node {
    int key = 0;
    int value = 0;
    Node* prev = nullptr;
    Node* next = nullptr;
  };

  // Splice n out of its position. Two assignments, no branches: the sentinels
  // guarantee n->prev and n->next are never null. Does not clear n's own
  // pointers — every caller either re-inserts n (overwriting them anyway)
  // or deletes it.
  void unlink(Node* n) {
    n->prev->next = n->next;
    n->next->prev = n->prev;
  }

  // Insert n just after head_ (the MRU slot). Assignment ORDER is load-
  // bearing: set n's own pointers FIRST, then fix head_'s side — the last
  // line overwrites head_->next, which the first line needed to read.
  void pushFront(Node* n) {
    n->next = head_->next;
    n->prev = head_;
    head_->next->prev = n;
    head_->next = n;
  }

  int capacity_;
  Node* head_;  // sentinel: head_->next is the MRU real entry
  Node* tail_;  // sentinel: tail_->prev is the LRU real entry (the victim)
  std::unordered_map<int, Node*> map_;
};

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

  {
    // LeetCode 146, Example 1 — the canonical sequence.
    LRUCache cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    check(cache.get(1) == 1, "LC example: get(1) == 1 after put(1,1), put(2,2)");
    cache.put(3, 3);  // full; key 2 is LRU because get(1) touched key 1
    check(cache.get(2) == -1, "LC example: get(2) == -1, evicted as LRU");
    cache.put(4, 4);  // full again; key 1 was refreshed, so key 3 is now LRU
    check(cache.get(1) == -1, "LC example: get(1) == -1 after put(4,4)");
    check(cache.get(3) == 3, "LC example: get(3) == 3");
    check(cache.get(4) == 4, "LC example: get(4) == 4");
  }

  {
    // Edge: overwrite path must refresh recency AND not trigger eviction.
    LRUCache cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(1, 10);  // existing key: value updates, key 1 becomes MRU
    check(cache.get(1) == 10, "overwrite updates the value in place");
    cache.put(3, 30);  // key 2 is LRU (key 1 was just touched) -> evict 2
    check(cache.get(2) == -1, "overwrite refreshed recency: key 2 evicted");
    check(cache.get(1) == 10 && cache.get(3) == 30,
          "both survivors intact after eviction triggered by an insert");
  }

  {
    // Edge: capacity-1 cache — every distinct put evicts immediately.
    LRUCache cache(1);
    cache.put(1, 1);
    cache.put(2, 2);
    check(cache.get(1) == -1, "capacity-1: previous key evicted on next put");
    check(cache.get(2) == 2, "capacity-1: newest key survives");
    cache.put(2, 9);  // overwrite the ONLY entry: must not self-evict
    check(cache.get(2) == 9, "capacity-1 overwrite keeps its own entry");
    check(cache.get(5) == -1, "miss on absent key returns -1");
  }

  {
    // Edge: repeated gets keep re-promoting without any eviction pressure.
    LRUCache cache(2);
    cache.put(1, 100);
    cache.put(2, 200);
    cache.get(1);
    cache.get(1);
    cache.get(1);  // hammering key 1 must not disturb key 2
    check(cache.get(2) == 200, "repeat hits on one key do not evict another");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
