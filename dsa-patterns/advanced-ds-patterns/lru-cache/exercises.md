# LRU Cache — Exercises

Work through these in order. The goal is to build two reflexes: (1) hearing "design a structure with O(1) per operation" and immediately asking *which two questions must each operation answer, and which two structures answer them* — and (2) being able to write the sentinel-bracketed doubly linked list and its `unlink`/`pushFront` surgery from memory, without null checks and in the right assignment order.

> Rule of thumb for every exercise: before writing code, name the invariant that couples your index structure to your order structure (for LRU it is "the map's keys are exactly the list's contents, and each map entry points at the unique list node with that key"). If you cannot state the invariant, you are not ready to write the operations.

---

## Easy — Design HashMap

**LeetCode 706 — Design HashMap.**

Design a HashMap without using any built-in hash-table library: `put(key, value)`, `get(key)` (return `-1` if absent), and `remove(key)`, all in O(1) average time.

**Constraints to notice:** keys and values are bounded (`0 <= key, value <= 10^6`), which means a fixed bucket count is a legitimate design choice rather than a hack. No ordering is ever required — only keyed lookup, insertion, deletion.

**Task:** implement it with separate chaining: an array of buckets, each bucket a singly linked list of `(key, value)` pairs, plus a simple hash function over the key space. Do **not** reuse `std::unordered_map` anywhere.

**Think about:** this file *is* the lookup half of the LRU pattern in isolation. Why does a singly linked bucket list suffice here, when the LRU cache's recency list had to be doubly linked? What exactly does each structure need to do to a node mid-sequence?

---

## Medium — Insert Delete GetRandom O(1)

**LeetCode 380 — Insert Delete GetRandom O(1).**

Design a structure supporting `insert(val)` (return `true` if absent before), `remove(val)` (return `true` if present before), and `getRandom()` returning a uniformly random element — each in O(1) average time.

**Constraints to notice:** the average element count stays manageable, but every one of the three operations carries an explicit O(1) requirement, including uniform random selection. Duplicates are rejected on insert, so values are distinct.

**Task:** compose a hash map from value to *vector index* with a `std::vector` of values, deleting via **swap-with-last**: copy the last element into the removed slot, fix the map entry for the moved element, pop the back. `getRandom` is then just a random index into the vector.

**Think about:** why can't the vector simply erase from the middle like the map asks? After a swap-with-last, which *other* entry became stale, and how do you find it in O(1)? And why would the LRU cache's doubly linked list be the wrong second structure here — what does `getRandom` need that a list cannot give in O(1)?

---

## Hard — All O'one Data Structure

**LeetCode 432 — All One Data Structure.**

Design a structure supporting `inc(key)` (add 1 to a string key's counter, creating it at 1 if new), `dec(key)` (subtract 1; drop the key entirely at zero), `getMaxKey()` and `getMinKey()` — each in O(1) average time.

**Task:** this is the frequency-bucket layer from LFU thinking applied to a *global* multiset instead of a cache: a hash map from key to node, where each node lives in one of a chain of buckets — each bucket a doubly linked list holding all keys with the same count, and adjacent buckets differing by exactly one count. `inc` moves a node from its bucket to the next one up (creating it on demand); `dec` moves it down or deletes it; `getMaxKey`/`getMinKey` read from fixed end buckets. Reuse the exact sentinel + `unlink` + `pushFront` skeleton from [code.cpp](code.cpp).

**Think about:** compare with [problems/02-lfu-cache.cpp](problems/02-lfu-cache.cpp): there the buckets were indexed by frequency in a hash map because arbitrary-frequency access was needed; here they sit in a *linked chain*. What query does the linked chain buy you that a hash-map-of-buckets cannot answer in O(1)? Conversely, what operation does 460 need that makes the chain unnecessary?

---

## Real-World Challenge — TTL + Capacity Hybrid Cache

You run an in-process cache in front of a Postgres-backed permissions service. Requirements gathered from the team: entries must expire **30 seconds after they were written** (staleness bound, because another pod may update Postgres), memory must stay bounded (**at most N entries**, evicting least-recently-used when full), and `get` must stay O(1). These are two independent policies — freshness/correctness and capacity — and your cache needs both simultaneously.

**Task:**
1. Design (and implement against a simple interface) a cache whose entry stores both a value and an absolute expiry deadline, where `get` performs a *lazy* expiry check — an expired hit is treated as a miss and cleaned up on the spot.
2. Keep the LRU machinery underneath for capacity: expired-on-read entries should be unlinked from the recency list as well as erased from the map, so both structures stay in the coupled-invariant state.
3. Discuss: nothing expires a long-unaccessed-but-still-fresh entry proactively — it lingers until read or evicted. When does lazy expiry actually matter for correctness versus memory, and what would you add (a background sweeper? a timing wheel?) if stale-but-never-read entries were unacceptable?
4. Argue from the README's "When NOT To Use" section: why is "just use a bigger TTL and skip the LRU part" wrong, and why is "just use LRU and skip the TTL part" also wrong?

---

## Bonus Challenge — Design Most Recently Used Queue

**LeetCode 1756 — Design Most Recently Used Queue.**

A queue of numbers `1..n`, initially ordered naturally. `fetch(i)` removes the i-th item (1-indexed from the front) and moves it to the front, making it the most-recently used; return that item. Both construction and fetches should be efficient.

**Task:** solve it first with a plain array or `std::vector` (erase at `i-1`, insert at front) and measure its complexity honestly — then re-solve it with a doubly linked list plus a cursor/skip technique so that each fetch costs roughly O(sqrt(n)) by walking in sqrt-sized jumps, or discuss why a balanced-BST / order-statistics approach reaches O(log n) and what it trades away.

**Then answer in writing:** `fetch(i)` is structurally identical to one half of an LRU cache operation — which half, and which exact step? Why does the LRU version get away with never needing the "walk to position i" step that dominates this problem?

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
