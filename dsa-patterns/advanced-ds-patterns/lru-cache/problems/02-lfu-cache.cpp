// ============================================================================
// LeetCode 460 — LFU Cache
// ============================================================================
//
// PROBLEM
// -------
// Design and implement a data structure for a Least Frequently Used (LFU)
// cache.
//
// Implement the LFUCache class:
//   - LFUCache(int capacity): initializes the object with the capacity.
//   - int get(int key): gets the value of the key. If the key does not
//     exist, return -1. A get/put increments the key's use counter.
//   - void put(int key, int value): updates or inserts the key. If the cache
//     is at capacity and the key is new, evict the key with the LOWEST use
//     counter; on a tie in counters, evict the LEAST RECENTLY used of them.
//
// Both get and put must run in O(1) average time per operation.
//
// Example 1:
//   ["LFUCache","put","put","get","put","get","get","put","get","get","get"]
//   [[2],[1,1],[2,2],[1],[3,3],[2],[3],[4,4],[1],[3],[4]]
//   -> [null,null,null,1,null,-1,3,null,-1,3,4]
//
// APPROACH — LRU's composition plus one extra layer: frequency buckets
// ---------------------------------------------------------------------
// LFU differs from LRU only in the eviction rule: recency ("how long since
// last touch") becomes frequency ("how many touches accumulated"). The LRU
// skeleton — hash map to Node*, sentinel-bracketed doubly linked list,
// unlink + pushFront touch — survives intact; what changes is that ONE list
// can no longer represent the order, because entries now belong to frequency
// groups:
//
//   freqList_: std::unordered_map<int, Bucket*>
//       maps a use-count f to one doubly linked list holding ALL nodes whose
//       counter equals f, most-recently-used at the front.
//   minFreq_: the smallest counter present anywhere in the cache.
//       This single tracked integer is what keeps eviction O(1): the victim
//       is always bucket(minFreq_)->tail_->prev (the least frequently used,
//       ties broken by being least recently used within that bucket).
//
// Every "use" of a node moves it exactly ONE bucket up: unlink from bucket f;
// if bucket f became empty, delete it and (only if minFreq_ was f) advance
// minFreq_; pushFront into bucket f+1. Because a bump can never leave a hole
// below the bumped node's new level, advancing minFreq_ by one step is always
// sufficient — that local reasoning is why no heap and no scan are needed.
//
// Why not a min-heap keyed on (freq, timestamp)? Refreshing a key inside a
// heap costs O(log n) and needs a key->heap-index map maintained through
// every sift. Here the ordering key changes by exactly +1 per use and every
// affected node is already in hand — so O(1) pointer surgery between buckets
// beats the heap outright. Same lesson as LRU: when YOU control how the
// ordering key changes, pick the structure whose cheap primitive matches.
//
// Tie-breaking detail worth saying aloud: within one bucket the list is kept
// in recency order (pushFront on every touch), so "least recently used among
// the least frequently used" is simply the back of the min-frequency bucket.
//
// COMPLEXITY
// ----------
// Time:  O(1) average for get and put — hash lookups plus a bounded number
//               of pointer writes (bucket creation/deletion amortizes to
//               O(1) per operation because each op adds/moves one node).
// Space: O(capacity) — one node per cached key; buckets hold no data beyond
//               their two sentinels.
// ============================================================================

#include <iostream>
#include <string>
#include <unordered_map>

class LFUCache {
 public:
  explicit LFUCache(int capacity) : capacity_(capacity), minFreq_(0), size_(0) {}

  ~LFUCache() {
    // Nodes live inside buckets, so freeing every bucket frees every node.
    std::unordered_map<int, Bucket*>::iterator it = freqList_.begin();
    for (; it != freqList_.end(); ++it) {
      destroyBucket(it->second);
    }
  }

  int get(int key) {
    if (capacity_ <= 0) return -1;
    std::unordered_map<int, Node*>::iterator it = keyNode_.find(key);
    if (it == keyNode_.end()) return -1;
    bump(it->second);  // a get is a use: move up one frequency bucket
    return it->second->value;
  }

  void put(int key, int value) {
    if (capacity_ <= 0) return;

    std::unordered_map<int, Node*>::iterator it = keyNode_.find(key);

    // ---- Path 1: overwrite existing key ---------------------------------
    if (it != keyNode_.end()) {
      it->second->value = value;
      bump(it->second);  // overwrite counts as a use too
      return;            // count did not grow -> no eviction check
    }

    // ---- Path 2: insert new key -----------------------------------------
    if (size_ == capacity_) {
      // Victim: least frequent, least recent within its frequency bucket.
      Bucket* victimBucket = freqList_[minFreq_];
      Node* victim = victimBucket->tail->prev;
      unlink(victim, victimBucket);
      if (bucketEmpty(victimBucket)) {  // keep the map free of empty buckets
        freqList_.erase(minFreq_);
        destroyBucket(victimBucket);
      }
      keyNode_.erase(victim->key);  // key stored IN the node -> O(1) erase
      delete victim;
      --size_;
    }
    Bucket* b1 = getOrCreateBucket(1);
    Node* fresh = new Node();
    fresh->key = key;
    fresh->value = value;
    fresh->freq = 1;
    pushFront(fresh, b1);
    keyNode_[key] = fresh;
    minFreq_ = 1;  // a brand-new entry has the lowest possible counter
    ++size_;
  }

 private:
  struct Node {
    int key = 0;
    int value = 0;
    int freq = 0;
    Node* prev = nullptr;
    Node* next = nullptr;
  };

  // One frequency's doubly linked list, sentinel-bracketed exactly like the
  // main list in 01-lru-cache.cpp. MRU at head_->next, LRU at tail_->prev.
  struct Bucket {
    Node* head;
    Node* tail;
    Bucket() {
      head = new Node();
      tail = new Node();
      head->next = tail;
      tail->prev = head;
    }
  };

  static bool bucketEmpty(const Bucket* b) { return b->head->next == b->tail; }

  static void destroyBucket(Bucket* b) {
    Node* cur = b->head;
    while (cur != nullptr) {
      Node* next = cur->next;
      delete cur;
      cur = next;
    }
    delete b;
  }

  static void unlink(Node* n, Bucket* /*owner*/) {
    n->prev->next = n->next;
    n->next->prev = n->prev;
  }

  static void pushFront(Node* n, Bucket* b) {
    n->next = b->head->next;
    n->prev = b->head;
    b->head->next->prev = n;
    b->head->next = n;
  }

  Bucket* getOrCreateBucket(int freq) {
    std::unordered_map<int, Bucket*>::iterator it = freqList_.find(freq);
    if (it != freqList_.end()) return it->second;
    Bucket* b = new Bucket();
    freqList_[freq] = b;
    return b;
  }

  // Move node n from its current frequency bucket to the one above,
  // maintaining minFreq_ and deleting buckets that become empty.
  void bump(Node* n) {
    int f = n->freq;
    Bucket* oldBucket = freqList_[f];
    unlink(n, oldBucket);
    if (bucketEmpty(oldBucket)) {
      freqList_.erase(f);
      destroyBucket(oldBucket);
      // Only the minimum bucket going empty invalidates minFreq_, and the
      // bumped node is now at f+1, so stepping minFreq_ once is exact.
      if (minFreq_ == f) minFreq_ = f + 1;
    }
    n->freq = f + 1;
    pushFront(n, getOrCreateBucket(f + 1));
  }

  int capacity_;
  int minFreq_;                                  // lowest counter present
  int size_;                                     // number of cached keys
  std::unordered_map<int, Node*> keyNode_;       // key -> node (one copy)
  std::unordered_map<int, Bucket*> freqList_;    // freq -> bucket list
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
    // LeetCode 460, Example 1 — exercises tie-breaking by recency twice.
    LFUCache cache(2);
    cache.put(1, 1);              // cnt(1)=1
    cache.put(2, 2);              // cnt(2)=1
    check(cache.get(1) == 1, "LC example: get(1) == 1, cnt(1) -> 2");
    cache.put(3, 3);  // cnt(2)==cnt(1)? No: cnt(1)=2, so bucket 1 holds only
                      // key 2 -> evict 2 despite key 1 being older
    check(cache.get(2) == -1, "LC example: get(2) == -1, least FREQUENT evicted");
    check(cache.get(3) == 3, "LC example: get(3) == 3, cnt(3) -> 2");
    cache.put(4, 4);  // full; bucket 1 empty so minFreq_=2; keys 1 and 3 both
                      // have cnt 2 -> tie broken by recency -> evict 1
    check(cache.get(1) == -1, "LC example: tie on freq broken by recency (evict 1)");
    check(cache.get(3) == 3, "LC example: get(3) == 3");
    check(cache.get(4) == 4, "LC example: get(4) == 4");
  }

  {
    // Edge: overwriting an existing key must increment its counter AND not
    // trigger eviction — otherwise an update would shrink the cache.
    LFUCache cache(2);
    cache.put(1, 1);   // cnt(1)=1
    cache.put(2, 2);   // cnt(2)=1
    cache.put(1, 10);  // cnt(1)=2, no eviction ran
    cache.put(3, 30);  // bucket 1 holds only key 2 -> evict 2, not 1
    check(cache.get(2) == -1, "overwrite bumped freq: key 2 is the LFU victim");
    check(cache.get(1) == 10, "overwritten key survived with updated value");
    check(cache.get(3) == 30, "freshly inserted key readable");
  }

  {
    // Edge: capacity-1 LFU — every distinct insert evicts immediately, and
    // minFreq_ must reset correctly each cycle.
    LFUCache cache(1);
    cache.put(1, 1);
    cache.put(2, 2);  // evict 1
    check(cache.get(1) == -1, "capacity-1: previous key evicted");
    check(cache.get(2) == 2, "capacity-1: newest key present");
    cache.put(3, 3);  // get(2) made cnt(2)=2; new key 3 has cnt 1 but cache
                      // is full -> still evicts key 2 (capacity wins over freq)
    check(cache.get(2) == -1, "capacity-1: even a frequently-used key is evicted");
    check(cache.get(3) == 3, "capacity-1: newest key present after second cycle");
  }

  {
    // Edge: get on an absent key returns -1 and changes nothing.
    LFUCache cache(2);
    cache.put(5, 50);
    check(cache.get(99) == -1, "miss on absent key returns -1");
    check(cache.get(5) == 50, "miss left the real entry untouched");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
