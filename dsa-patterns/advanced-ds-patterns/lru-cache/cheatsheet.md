# LRU Cache — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Advanced DS pattern — design-with-data-structures technique (composing a hash map with an ordered structure). |
| **Recognition Signal** | Problem says **"design"** a structure with a fixed **capacity**, automatic eviction when full, and `get`/`put` in **O(1)**; or you need a hash map *plus* an order over its contents maintained in O(1). |
| **Problem** | A cache must answer "where is this key?" (a lookup) and "which entry is least recently used?" (an ordering) on every operation — and no single standard container answers both: hash maps have no order; lists/arrays have no keyed lookup. |
| **Solution** | Compose both: `unordered_map<Key, Node*>` as the index into a **doubly linked list** of nodes kept in recency order (MRU at front, LRU at back). A hit/overwrite unlinks the node and re-pushes it to the front; an insertion at capacity evicts `tail_->prev` first. |
| **Time / Space Complexity** | O(1) average/amortized for both `get` and `put` (worst case for the list ops; the hash map is the average case). O(capacity) space — one node + one map entry per cached key. |
| **Pros** | Both operations genuinely O(1) · eviction requires no search (victim is always `tail_->prev`) · exact LRU semantics, deterministic and easy to test · sentinels eliminate all boundary branches · the composition generalizes to LFU, random-pick, browser history. |
| **Cons** | ~16–24 bytes overhead per entry plus two allocations · pointer chasing is cache-hostile · every read mutates shared state, so reads cannot be concurrent without sharding · not scan-resistant (one cold sweep flushes the hot set) · raw-pointer version needs careful destructor/copy discipline in C++. |
| **Use When** | "Design a cache/structure with O(1) ops + eviction policy" · bounded memoization in a long-lived process · bounded dedup/idempotency window · any map that also needs O(1)-maintained order · recency is a good predictor of reuse. |
| **Avoid When** | No eviction needed (plain hash map is simpler and cheaper) · thread-safe concurrent access at scale (shard, or use Caffeine/Guava/Redis) · time-driven expiry, not capacity-driven (that is TTL: heaps/timing wheels/lazy deadline checks) · workload dominated by large sequential scans (LFU/ARC/W-TinyLFU instead) · tiny fixed entry count (flat array scan beats pointer chasing). |
| **Related Patterns** | LFU Cache (same composition + frequency buckets) · Insert/Delete/GetRandom O(1) (map + vector, swap-with-last) · Design Browser History (list + cursor) · Hash Map from scratch (the lookup half built by hand) · Fast & Slow Pointers / In-Place Reversal (shares the pointer-surgery skill). |

### Template Skeleton

```cpp
struct Node {
    int key, value;
    Node* prev = nullptr;
    Node* next = nullptr;
};

// Sentinels: head_ <-> tail_, real entries live strictly between them.
void unlink(Node* n) {              // O(1): sentinels guarantee both neighbours exist
    n->prev->next = n->next;
    n->next->prev = n->prev;
}
void pushFront(Node* n) {           // O(1): set n's pointers FIRST, then fix head_'s
    n->next = head_->next;
    n->prev = head_;
    head_->next->prev = n;
    head_->next = n;
}

int get(int key) {
    auto it = map_.find(key);
    if (it == map_.end()) return -1;    // miss: order unchanged
    unlink(it->second);                 // a read IS a use:
    pushFront(it->second);              // move to front or this is FIFO, not LRU
    return it->second->value;
}

void put(int key, int value) {
    auto it = map_.find(key);
    if (it != map_.end()) {             // overwrite path:
        it->second->value = value;      // update value,
        unlink(it->second);             // touch,
        pushFront(it->second);
        return;                         // RETURN -- do NOT evict here
    }
    if ((int)map_.size() == capacity_) {// insertion may grow the cache -> check FIRST
        Node* lru = tail_->prev;        // victim reachable directly, no search
        unlink(lru);
        map_.erase(lru->key);           // key lives IN the node -- that's why
        delete lru;
    }
    Node* fresh = new Node{key, value, nullptr, nullptr};
    pushFront(fresh);
    map_[key] = fresh;                  // invariant: map keys == list contents
}
```

### Remember In One Sentence
> **An LRU cache composes a hash map (O(1) "where is the key?") with a doubly linked list kept in recency order (O(1) "touch this entry" and O(1) "who is the victim"), storing each node's key inside the node so eviction can erase the matching map entry — because neither structure can answer both questions alone.**

### Two Facts People Get Wrong
- "`get` is a read-only lookup"? **No** — in an LRU cache every successful `get` must unlink and re-push the node to the front. Skip it and you have silently built a FIFO cache: it compiles, it evicts, and it only fails tests that read a key before it would otherwise be evicted.
- "Eviction checks happen after inserting a new key"? **No** — the capacity check belongs *before* insertion, on the insert path only; the overwrite path must `return` early because updating an existing key does not change the count, and evicting there shrinks the cache one entry per update.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the two questions every cache operation must answer fast, and why neither a hash map nor a plain linked list can answer both.
2. Why must the list be *doubly* linked? What exactly breaks with a singly linked list?
3. Why does the `Node` store its own `key` when the map already has it?
4. Walk through `put` on an existing key: what happens, and why must it never run the eviction check?
5. What are sentinel head/tail nodes for, and which two functions do they keep branch-free?
6. Why are the four assignments inside `pushFront` order-sensitive? Which line, if moved later, destroys the list?
7. What is the coupling invariant between `map_` and the list, and what does every LRU bug look like in terms of it?
8. Why is a min-heap keyed on last-access time a worse fit than the doubly linked list, even though both maintain an ordering?
9. What is the difference between LRU and FIFO, in terms of one specific code step?
10. Why did Redis deliberately *not* implement exact LRU, and what does it do instead?

*(Hint if stuck: questions 1–2 → README "Why is this problem difficult?", 3–4 → "Execution Flow", 5–7 → "Solution"/"Architecture", 8 → "Why Not Other Approaches?", 9–10 → "Common Mistakes"/"Real Interview/Production Examples".)*
