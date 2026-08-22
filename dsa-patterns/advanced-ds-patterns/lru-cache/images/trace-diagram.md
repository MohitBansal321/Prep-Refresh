# LRU Cache — Trace Diagram (Worked Example)

This traces the exact operation sequence from `main()` in [code.cpp](../code.cpp) — the same sequence as LeetCode 146's Example 1, solved by [problems/01-lru-cache.cpp](../problems/01-lru-cache.cpp):

```
LRUCache cache(2);        // capacity 2
put(1, 10); put(2, 20);
get(1) -> 10
put(3, 30);               // full: must evict
get(2) -> -1              // evicted
get(1) -> 10; get(3) -> 30
put(1, 99);               // overwrite existing key
put(4, 40);               // full again: evict
get(3) == -1, get(1) == 99, get(4) == 40
```

```mermaid
sequenceDiagram
    autonumber
    participant C as Client
    participant M as map_
    participant L as list (head -> ... -> tail)

    Note over C,L: cache has capacity 2 -- list starts empty: head <-> tail

    C->>M: put(1, 10)
    M->>L: pushFront(node1)
    Note over L: head -> [1:10] -> tail   (size 1 of 2)

    C->>M: put(2, 20)
    M->>L: pushFront(node2)
    Note over L: head -> [2:20] -> [1:10] -> tail   (full)

    C->>M: get(1)
    M-->>C: HIT, return 10
    M->>L: unlink(node1), pushFront(node1)
    Note over L: head -> [1:10] -> [2:20] -> tail<br/>the READ moved key 1 to MRU;<br/>key 2 is now the victim

    C->>M: put(3, 30) -- miss, size == capacity
    L-->>M: victim = tail->prev = node2
    M->>L: unlink(node2)
    M->>M: erase(node2->key = 2), delete node2
    M->>L: pushFront(node3)
    Note over L: head -> [3:30] -> [1:10] -> tail

    C->>M: get(2)
    M-->>C: -1 (evicted -- the earlier read of key 1 saved it)

    C->>M: get(1)
    M-->>C: HIT, return 10 (already at front; unlink+pushFront is a no-op reorder)

    C->>M: get(3)
    M-->>C: HIT, return 30

    C->>M: put(1, 99) -- key EXISTS: overwrite path
    M->>L: node1->value = 99, then unlink + pushFront node1
    Note over L: head -> [1:99] -> [3:30] -> tail<br/>NO eviction ran -- count did not grow

    C->>M: put(4, 40) -- miss, size == capacity
    L-->>M: victim = tail->prev = node3
    M->>L: unlink(node3)
    M->>M: erase(node3->key = 3), delete node3
    M->>L: pushFront(node4)
    Note over L: head -> [4:40] -> [1:99] -> tail

    C->>M: get(3)
    M-->>C: -1 (evicted -- its recency was stale next to key 1's fresh update)
```

## How to read it

Each round trip is one `get` or `put` from `main()` in [code.cpp](../code.cpp). Watch the list, not the map: the list *is* the eviction decision being maintained continuously. Three things in this trace are the pattern's actual teaching points.

**The read that changed history.** Step 3's `get(1)` looks like a pure query, but it unlinks and re-pushes node1 — which demotes key 2 to `tail_->prev` and therefore *selects* it as step 5's victim. Delete that one touch step and the trace still runs, but step 5 would evict key 1 instead: a FIFO cache, indistinguishable until exactly this kind of test.

**Eviction reads the key out of the node.** At steps 6 and 12 the code holds only `Node*` (`tail_->prev`) and needs the corresponding map entry gone. That works because the `Node` stores its own `key` — the field most often forgotten when writing this from memory. The order is fixed: unlink, erase using `victim->key`, then `delete`; any other order either leaks the node or reads freed memory.

**The overwrite branch never evicts.** Step 11's `put(1, 99)` finds key 1 present, so it updates the value, touches the node, and returns early. The entry count did not grow, so the capacity check does not run — which is why step 12 evicts key 3 (stale since step 8) rather than something "extra." Had the overwrite fallen through to the insertion path, every update would have silently shrunk the cache by one.
