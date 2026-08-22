# LRU Cache — Flow Diagram (get / put)

This traces the control flow of the two operations in [code.cpp](../code.cpp) — the shape behind every problem in [problems/](../problems/). See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual values and evictions.

```mermaid
flowchart TD
    Op([Operation arrives]) --> Which{get or put?}

    Which -- "get key" --> GFind{key in map_?}
    GFind -- No, miss --> Miss(["Return -1.<br/>Nothing changes: order untouched,<br/>no eviction, no allocation"])
    GFind -- Yes, hit --> GTouch["TOUCH the entry:<br/>1. unlink node -- two assignments,<br/>   sentinels guarantee both neighbours<br/>2. pushFront node -- four assignments,<br/>   n's own pointers set FIRST"]
    GTouch --> GRet(["Return node->value.<br/>One hash lookup + six pointer writes"])

    Which -- "put key value" --> PFind{key in map_?}

    PFind -- "Yes: OVERWRITE" --> Upd["node->value = value<br/>then TOUCH exactly as get does:<br/>an overwrite is a use"]
    Upd --> PReturn(["return EARLY.<br/>Do NOT run eviction here:<br/>the entry count did not grow"])

    PFind -- "No: INSERTION" --> Cap{map_.size == capacity_?}
    Cap -- No --> Ins
    Cap -- "Yes: EVICT FIRST" --> Ev["victim = tail_->prev -- the LRU,<br/>reachable directly, no search<br/>1. unlink victim<br/>2. map_.erase(victim->key)<br/>   -- needs the key stored IN the node<br/>3. delete victim"]
    Ev --> Ins["Allocate fresh Node{key, value}<br/>pushFront it -- now MRU<br/>map_[key] = fresh -- index agrees with list"]
    Ins --> PDone(["Done.<br/>Invariant restored: map keys == list contents"])
```

## How to read it

Every path through this diagram is O(1) by construction, and it is worth checking *why* per branch rather than taking it on faith: the miss path is one hash lookup; the hit and overwrite paths are one hash lookup plus `touch` — which is `unlink` (two pointer writes, no branches because the sentinels guarantee both neighbours exist) followed by `pushFront` (four pointer writes, no allocation because the node was never destroyed, only re-threaded); and the insert path adds at most one eviction whose three steps are all direct pointer or map access. Nothing loops, nothing scans, nothing depends on cache size.

The **two load-bearing details** are the ones first-time implementations get wrong. First, the overwrite branch's early return: updating an existing key does not change the entry count, so running the capacity check there evicts a live victim for no reason, shrinking the usable cache by one on every update until it holds nothing. Second, the eviction step order — unlink from the list, erase from the map *using the key read out of the node*, then delete — is not stylistic. Erasing after deleting would read freed memory; deleting before erasing loses the key needed for the erase. The key lives inside the `Node` precisely so that middle step is possible in O(1) starting from only a `Node*`.

Also notice what the diagram does *not* contain: any search. The victim is never found — it is always sitting at a known address (`tail_->prev`). That is the payoff of maintaining recency order continuously on every operation instead of reconstructing it at eviction time.
