# Union Find — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "connectivity query interleaved with incremental edges" signal that means "reach for Union Find," and (2) correctly deciding what the *elements* being unioned actually are — not always the obvious "node" in the problem statement, sometimes a row/column, a String key, or an index into a completely different array.

> Rule of thumb for every exercise: before writing a single line, ask "am I being asked for connectivity/component-count/cycle-detection, or do I actually need the path/edges themselves?" and "what are the elements I'm calling `find`/`unionSets` on — are they the problem's literal nodes, or do I need to map something else onto integer indices first?" If you cannot answer both questions, you are not ready to write the union logic yet.

---

## Easy — Find if Path Exists in Graph

**LeetCode 1971 — Find if Path Exists in Graph.**

You are given `n` vertices (0-indexed) and a list of bidirectional `edges`, plus a `source` and a `destination`. Return `true` if there is a path from `source` to `destination`.

**Task:** solve it by unioning every edge into a `DisjointSet`, then answering with a single `connected(source, destination)` call — no traversal needed.

**Think about:** this problem could just as easily be solved with a single BFS/DFS from `source`. Which approach is genuinely simpler to write here, and does the "one-shot query" nature of this problem (edges are all given up front, one path check is needed) change your answer from what you'd choose for Redundant Connection?

---

## Medium — Number of Operations to Make Network Connected

**LeetCode 1319 — Number of Operations to Make Network Connected.**

You have `n` computers labeled `0` to `n-1`, connected by an array of `connections` where `connections[i] = [a, b]` is a direct cable between computer `a` and computer `b`. Any two computers can communicate if they are connected directly or indirectly. You can remove a cable between two directly connected computers and place it anywhere else. Return the minimum number of operations to make all computers connected, or `-1` if impossible.

**Task:** figure out (a) how many "extra" cables exist (cables that connect two computers already connected some other way — a redundant edge, exactly like problem 02) and (b) how many separate components remain after unioning every connection. The answer follows from combining those two numbers with a specific inequality — work out what it is and why.

**Think about:** why is it impossible in some cases, and what is the minimum number of redundant cables you need for the answer to *not* be `-1`?

---

## Hard — Bricks Falling When Hit

**LeetCode 803 — Bricks Falling When Hit.**

Given a grid of bricks (`1` = brick, `0` = empty) where a brick is "stable" if it is in the top row or touches another stable brick (4-directionally), you are given a sequence of `hits` (cells to erase, one at a time). After each hit, some bricks may become unstable and fall (removed from the grid). Return, for each hit, how many bricks fell as a result (not counting the hit brick itself).

**Task:** the natural direction — simulate hits forward and re-check stability each time — is expensive to do efficiently. Instead, work **backwards**: start from the grid state *after all hits have been applied*, and Union Find the bricks back in in **reverse hit order**, connecting each restored brick to its stable neighbors (and to a virtual "roof" node representing the top row). The number of bricks that "fall" for hit `i` (in forward order) equals the growth in the top-row-connected component's size when brick `i` is added back, in reverse order, minus one (for the hit brick itself) — with a floor at zero.

**Think about:** why is processing in reverse the key insight here, and why does a virtual node representing "the top row / stability itself" make the union logic dramatically simpler than tracking "is this brick connected to row 0" directly? This is widely considered one of the hardest standard Union Find problems — take your time with it.

---

## Real-World Challenge — Detecting Network Partitions in a Distributed Cluster

You operate a cluster of `n` service nodes (numbered `0` to `n-1`) that gossip heartbeats over a mesh of point-to-point links. Links can go up or down over time due to network blips, and you receive a stream of events, each either `{"type": "link_up", "a": i, "b": j}` or `{"type": "link_down", "a": i, "b": j}`. After every event, your monitoring dashboard needs to answer, in near-real-time: "is the cluster currently one single connected partition, or has it split?" and, if split, "does any partition currently hold a strict majority (more than n/2) of the nodes?" (a real operational concern: a cluster split into partitions where no side has a majority cannot safely elect a leader).

**Task:**
1. Implement (reading a `std::vector` of link events standing in for the live stream) an incremental connectivity tracker using `DisjointSet`, processing `link_up` events as unions.
2. Discuss: `link_down` events are the hard part — Union Find's `unionSets` has no corresponding "un-union." What would you actually have to do to correctly answer connectivity queries in the presence of edge *removal* (hint: think about whether you can get away with periodically rebuilding the DisjointSet from only the currently-up links, versus needing a fundamentally different structure like a Link-Cut Tree or an offline/reverse-processing trick similar to Bricks Falling When Hit above)?
3. Using `componentSize()`, determine after each processed event whether any single component currently holds a strict majority of the `n` nodes, and explain what "no majority partition" means operationally for a distributed system trying to elect a leader (this is the same "split-brain" problem real consensus protocols like Raft and Paxos are designed around).

---

## Bonus Challenge — Most Stones Removed with Same Row or Column

**LeetCode 947 — Most Stones Removed with Same Row or Column.**

Given `stones[i] = [x, y]` positions on a 2D plane, you may remove a stone if it shares a row **or** a column with another stone that has **not** yet been removed. Return the maximum number of stones you can remove.

**Task:** the key insight is that the *elements* being unioned are not the stones themselves in the obvious 2D sense — union each stone's **row index** with its **column index** (using a trick like encoding columns as `~y` or `y + 10001` to keep row-space and column-space from colliding in the same integer range). The answer works out to `(number of stones) - (number of connected components)`.

**Then, generalize in writing (no code required):** explain *why* "number of stones minus number of components" gives the correct maximum, referencing how many stones you can always leave standing (exactly one) per connected component while removing the rest. Then explain, using the Architecture section of the [README](README.md), why this problem's "elements" are rows-and-columns rather than the stones themselves, and how that relates to the email-keyed account-merging trick in [problems/03-accounts-merge.cpp](problems/03-accounts-merge.cpp) — is "union by shared attribute, not by direct pairwise edge" a variation on the same pattern, or a fundamentally different one? Justify your answer.

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
