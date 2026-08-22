# LRU Cache — Recognition Diagram

Use this flowchart when a problem says "design a structure" with a capacity and an eviction rule, and you are deciding whether hash-map-plus-doubly-linked-list is the right composition — or whether the eviction key, the concurrency story, or the language context points somewhere else.

```mermaid
flowchart TD
    Start([Problem: design a structure with<br/>get / put and automatic removal<br/>when capacity is exceeded]) --> Q1{What decides the victim<br/>when the structure is full?}

    Q1 -- "Least recently USED:<br/>every op incl. reads refreshes recency" --> Q2{Can every read afford to<br/>mutate shared structure?<br/>single-threaded? interview setting?}
    Q2 -- Yes --> Combo["Hash map (key -> Node*)<br/>+ DOUBLY linked list in recency order,<br/>MRU at front / LRU at tail_->prev"]
    Q2 -- "No: concurrent at scale,<br/>millions of keys (production)" --> Approx[["Approximate LRU: timestamp per object,<br/>sample k random keys, evict oldest sampled<br/>(Redis) -- or clock-sweep usage counters<br/>(Postgres). No list, no read mutation"]]

    Q1 -- "Oldest INSERTED;<br/>reads do not matter" --> FIFO[["Plain FIFO queue -- no move-to-front.<br/>If you skip unlink+pushFront inside get<br/>this is what you built by accident"]]
    Q1 -- "Least FREQUENTLY used:<br/>a counter that accumulates" --> LFU[["Add frequency buckets:<br/>map from freq -> doubly linked list,<br/>+ tracked minFreq (LeetCode 460)"]]
    Q1 -- "External deadline or score<br/>I do not control: TTL, priority" --> Heap[["Min-heap keyed on that score<br/>+ map key -> heap slot.<br/>O(log n) -- correct ONLY because<br/>the ordering key is external"]]
    Q1 -- "Need ordered iteration /<br/>floor-ceiling queries too" --> OrderedMap[["std::map / balanced BST keyed on access count:<br/>O(log n) per op; re-keying on every get<br/>is erase + insert = two rebalances per READ"]]

    Q1 -- "C++ production shortcut,<br/>not an interview answer" --> Splice[["unordered_map<K, std::list<T>::iterator><br/>+ list::splice(begin, list, it): same O(1)s,<br/>a third of the code, iterators stay valid"]]

    Combo --> Done([Compose two structures so each<br/>answers what the other cannot])
    Splice --> Done
```

## How to read it

The **first fork** is the victim-selection rule, not the data structures — naming the rule precisely is where the whole solution falls out. "Least recently *used*" contains a trap worth saying out loud: *used* includes reads, which means `get` must reorder the structure, which means the ordering structure must support **O(1) repositioning of an arbitrary node you already hold** — and that single requirement eliminates the timestamp scan (repositioning is free but finding the victim is O(n)), the min-heap (finding the victim is cheap but refreshing a key is O(log n) plus an index map), and the ordered map (two tree rebalances per read). The doubly linked list is not chosen for being a linked list; it is chosen because it is the only standard shape whose unlink-a-node-you-hold and insert-at-front are both genuinely constant time.

The **second fork**, on the LRU branch, separates the interview answer from the production one. In an interview you hand-roll the sentinel-bracketed list exactly as in [code.cpp](../code.cpp) because the point being tested is that you know *why* it must be doubly linked — knowledge that `std::list::splice` hides. In real C++ production code the splice variant in [code.cpp](../code.cpp)'s Tradeoffs discussion implements the identical cache in a third of the code with no raw pointers and no destructor. And if the setting is Redis-or-Postgres scale — millions of keys, reads from many cores — exit left to approximate LRU before writing any list at all: at that scale the ~16 bytes of pointers per key and the per-read structural mutation cost more than exactness earns back. The sibling design problems (LFU via buckets, TTL expiry via heaps) are deliberately shown as *different branches*, not variations: changing the victim rule changes the required second structure.
