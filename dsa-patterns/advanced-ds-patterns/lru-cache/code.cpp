// ============================================================================
// LRU Cache — generic reusable template (C++17)
// ============================================================================
//
// O(1) get/put backed by a hash map (key -> node pointer) plus a doubly
// linked list ordered by recency (most-recently-used at the front).
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <unordered_map>

class LRUCache {
 public:
  explicit LRUCache(int capacity) : capacity_(capacity) {
    head_ = new Node();
    tail_ = new Node();
    head_->next = tail_;
    tail_->prev = head_;
  }

  ~LRUCache() {
    Node* cur = head_;
    while (cur) {
      Node* next = cur->next;
      delete cur;
      cur = next;
    }
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
    Node* fresh = new Node{key, value, nullptr, nullptr};
    pushFront(fresh);
    map_[key] = fresh;
  }

  int size() const { return static_cast<int>(map_.size()); }

 private:
  struct Node {
    int key = 0, value = 0;
    Node* prev = nullptr;
    Node* next = nullptr;
  };

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

  int capacity_;
  Node* head_;
  Node* tail_;
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

  LRUCache cache(2);
  cache.put(1, 10);
  cache.put(2, 20);
  check(cache.get(1) == 10, "get(1) == 10 right after inserting");

  cache.put(3, 30);  // capacity 2, key 2 is LRU (1 was just touched) -> evicted
  check(cache.get(2) == -1, "get(2) == -1, key 2 evicted as least-recently-used");
  check(cache.get(1) == 10, "get(1) == 10, key 1 survived (was touched before eviction)");
  check(cache.get(3) == 30, "get(3) == 30, freshly inserted");

  cache.put(1, 99);  // update existing key's value, also refreshes recency
  check(cache.get(1) == 99, "put on existing key updates its value");
  check(cache.size() == 2, "size stays at capacity after an update-in-place put");

  cache.put(4, 40);  // capacity 2, key 3 is now LRU -> evicted
  check(cache.get(3) == -1, "get(3) == -1, key 3 evicted after key 1 was refreshed");
  check(cache.get(1) == 99, "get(1) == 99, still present");
  check(cache.get(4) == 40, "get(4) == 40, freshly inserted");

  LRUCache single(1);
  single.put(1, 1);
  single.put(2, 2);  // capacity 1 -> key 1 evicted immediately
  check(single.get(1) == -1, "capacity-1 cache evicts previous key on next put");
  check(single.get(2) == 2, "capacity-1 cache keeps the newest key");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
