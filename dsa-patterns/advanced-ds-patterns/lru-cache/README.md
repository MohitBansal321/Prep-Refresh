# LRU Cache (Design With Data Structures)

## Intent

Support `get(key)` and `put(key, value)` in `O(1)` each, while automatically evicting the **least recently used** entry once the cache is full — the canonical "design a data structure" interview question, and the template for a whole family of similar design problems (LFU Cache, design a browser history, design a rate limiter).

## Recognition Signal

The problem says "design a \<cache/structure\>" and names an eviction or ordering policy tied to *recency of access* (or, in variants, frequency of access) — and expects both the lookup and the reordering-on-access to be `O(1)`, not `O(n)`.

## Core Idea

Neither a hash map nor a linked list alone is enough: a hash map gives `O(1)` lookup but has no sense of order, and a linked list gives `O(1)` reordering (once you're at a node) but `O(n)` lookup by key. Combining them gets both for free — a **hash map from key to a node pointer**, where each node lives in a **doubly linked list ordered by recency** (most-recently-used at the front, least-recently-used at the back). On `get`, look the node up in `O(1)` via the map, then unlink and re-insert it at the front in `O(1)` via the list (the map already handed you the node, so no scan is needed). On `put` when full, evict the node at the back — again `O(1)`, because a doubly linked list's tail removal needs no scan — and erase its key from the map.

## Template

```cpp
class LRUCache {
  struct Node {
    int key, value;
    Node* prev = nullptr;
    Node* next = nullptr;
  };

  int capacity_;
  Node* head_;  // sentinel; head_->next is most-recently-used
  Node* tail_;  // sentinel; tail_->prev is least-recently-used
  std::unordered_map<int, Node*> map_;

  void unlink(Node* n) {
    n->prev->next = n->next;
    n->next->prev = n->prev;
  }

  void pushFront(Node* n) {
    n->next = head_->next;
    n->prev = head_;
    head_->next->prev = n;
    head_->next = n;
  }

 public:
  LRUCache(int capacity) : capacity_(capacity) {
    head_ = new Node();
    tail_ = new Node();
    head_->next = tail_;
    tail_->prev = head_;
  }

  int get(int key) {
    auto it = map_.find(key);
    if (it == map_.end()) return -1;
    unlink(it->second);
    pushFront(it->second);
    return it->second->value;
  }

  void put(int key, int value) {
    auto it = map_.find(key);
    if (it != map_.end()) {
      it->second->value = value;
      unlink(it->second);
      pushFront(it->second);
      return;
    }
    if (static_cast<int>(map_.size()) == capacity_) {
      Node* lru = tail_->prev;
      unlink(lru);
      map_.erase(lru->key);
      delete lru;
    }
    Node* fresh = new Node{key, value};
    pushFront(fresh);
    map_[key] = fresh;
  }
};
```

## Complexity

**Time:** `O(1)` for both `get` and `put` — the map gives `O(1)` node lookup, and unlinking/inserting at a known node in a doubly linked list needs no traversal.
**Space:** `O(capacity)` — one map entry and one list node per cached key.

## Common Mistakes

- **Using a singly linked list, or storing raw indices instead of node pointers.** Removing an arbitrary node from a singly linked list needs its predecessor, which means a scan — that reintroduces the `O(n)` cost this whole structure exists to avoid. The map must store pointers directly to list nodes, not keys or positions.
- **Forgetting to move a node to the front on `get`, not just `put`.** A cache is "least recently *used*," and a read counts as a use — skipping the move-to-front on `get` silently turns this into an insertion-order (not access-order) eviction policy.
- **Skipping sentinel head/tail nodes and hand-checking `nullptr` at both ends.** Real (non-sentinel) head/tail pointers force every unlink/insert to special-case "is this the first/last real node," which is exactly the kind of off-by-one surface a fixed pair of dummy sentinels eliminates for free.
- **Not freeing evicted nodes in C++** — a `delete` is needed on eviction, or the cache leaks one `Node` per eviction over its lifetime.

## When To Use

- Any "design a cache/structure with O(1) access and an eviction policy" question — LRU is the base case; LFU (evict least-*frequently*-used) is the same map+list idea with one extra layer: a map from frequency to a list of keys at that frequency.

## When NOT To Use

- **Only ever need pure key-value storage with no eviction policy** — a plain hash map is simpler and sufficient.
- **Need thread-safe concurrent access** — this template has no locking; a production cache needs a mutex (or a lock-free structure) around every mutation, which is a separate concern from the eviction logic itself.

## Similar Patterns

- **Trie** ([../trie/](../trie/)): another "build a custom structure to make a specific operation cheap" pattern, though the operation and structure shape are unrelated.
- **Monotonic Stack/Queue** ([../monotonic-stack-queue/](../monotonic-stack-queue/)): also maintains an order-sensitive sequence with O(1) amortized updates, but drops elements based on a value comparison rather than a recency/frequency policy.

## Further Reading

- LeetCode — LRU Cache (146), LFU Cache (460), Design Twitter (355), Design Browser History (1472), Insert Delete GetRandom O(1) (380).
- *Introduction to Algorithms* (CLRS) — doubly linked list operations, as the mechanical building block this pattern composes with a hash map.
