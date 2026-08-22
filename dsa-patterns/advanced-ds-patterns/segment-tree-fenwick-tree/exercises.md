# Segment Tree / Fenwick Tree — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "repeated range queries + updates" signal — and its disguised cousin, the "count elements/pairs with a value condition" signal — and (2) correctly handling the two classic traps: 0-indexed input vs 1-indexed internal tree positions, and coordinate compression before indexing.

> Rule of thumb for every exercise: before writing a single line, ask "what operation does my tree aggregate (sum? count? min?) and is it invertible?" and "do my values fit directly as indices, or must I compress them first?" If you cannot answer both, you are not ready to write the update/query loops yet.

---

## Easy — Queries on a Permutation With a Key

**LeetCode 1409 — Queries on a Permutation With a Key.**

Given a permutation `m` of the integers `1..n` (1-indexed) and a sequence of queries `queries[i]`, process each query by finding the **current position** of value `queries[i]` in `m`, reporting it, then moving that value to the **front** of the list (shifting everything before it right by one).

**Constraints to notice:** `n` and the query count can each reach ~10^4, so simulating with an array and shifting costs O(n·q) — up to 10^8 element moves. Positions change after every query, so nothing can be precomputed statically.

**Task:** maintain a Fenwick Tree over positions storing 1 = occupied, 0 = empty. Answering "current position of value v" becomes a prefix-count question over occupancy; moving to front becomes two point updates (clear old slot, fill slot 0) plus bookkeeping of where each value currently lives. Coordinate-compress nothing here — positions are already small indices — but notice *why* that is true.

**Think about:** your tree stores counts, not sums — what exactly goes into each slot during `update`, and what does `query(p)` return semantically? Why does "find position" reduce to a prefix sum rather than a search?

---

## Medium — Count Number of Teams

**LeetCode 1395 — Count Number of Teams.**

Soldiers stand in a line with distinct ratings. Count teams of three indices `i < j < k` whose ratings are either strictly increasing (`rating[i] < rating[j] < rating[k]`) or strictly decreasing.

**Constraints to notice:** n up to ~1000 makes brute force O(n^3) feasible but O(n^2) is expected at scale; ratings can be arbitrary values, so compress before indexing.

**Task:** for each middle soldier `j`, count `less[j]` = soldiers before `j` with smaller rating and `greater[j]` = soldiers after `j` with larger rating. The answer is `sum(less[j] * greater[j]) + sum(greaterBefore[j] * lessAfter[j])`. Compute these four quantities with Fenwick Trees over compressed ratings — one left-to-right pass and one right-to-left pass.

**Think about:** why does anchoring on the *middle* index collapse a triple-counting problem into independent pair-counts on each side? What would go wrong if you anchored on the first index instead?

---

## Hard — Create Sorted Array Through Instructions

**LeetCode 1649 — Create Sorted Array Through Instructions.**

Process instructions one at a time, inserting each value into a growing multiset that must stay sorted. The cost of inserting `x` is `min(number of existing elements strictly less than x, number strictly greater than x)`. Return the total cost modulo 10^9 + 7.

**Constraints to notice:** up to 10^5 instructions with values up to 10^5 — small enough to skip compression if you want, but large enough that per-insert rescanning (O(n) each) is too slow. Equal values count as neither less nor greater, so strictness matters at both ends of the query.

**Task:** insert into a Fenwick Tree of value-buckets; before each insertion ask two prefix-count questions to get the strictly-less and strictly-greater populations, take the cheaper, then increment the bucket. Then redo it with coordinate compression and confirm you get identical answers — this is the cheapest way to internalize when compression is optional versus mandatory.

**Think about:** how do you derive "strictly greater" from prefix counts alone (no suffix-query loop)? And why is taking the min of the two sides per insertion locally optimal even though it ignores future insertions entirely?

---

## Real-World Challenge — Leaderboard With Live Score Changes

You run ranked matchmaking for a game. Players' scores change constantly (wins, losses, decay). At any moment, operations wants two answers instantly: (1) a given player's current rank ("how many players have a strictly higher score?"), and (2) how many players fall inside a score band `[lo, hi]` (for matchmaking pools).

**Task:**
1. Design the data structure: scores can be huge and arbitrary (not small non-negative integers), so decide what your Fenwick is indexed by and how score changes translate into tree operations.
2. Rank queries and band queries are both range-count questions — write down, in terms of two prefix-count calls each, exactly how you answer them.
3. Discuss: a player's score changes by +150. Why is this a *two-step* update if you store presence-by-score (decrement old bucket, increment new)? What breaks if you forget the decrement?

**Bonus:** tie-break equal scores by registration time so ranks are unique — explain how you compress the (score, timestamp) pair into a single totally ordered key without ever materializing all possible pairs.

---

## Bonus Challenge — Range Minimum With Segment Tree

**LeetCode-style — the segment tree half of the module.**

Maintain an array under point assignments and range-minimum queries over arbitrary ranges.

**Task:** implement the full segment tree from [code.cpp](code.cpp) from memory: array-backed implicit tree sized `4n`, recursive build/update/query with the three-way overlap check (no overlap / full cover / partial). Verify each case with a hand-drawn tree over `[5, 2, 8, 1, 9, 3]`.

**Then answer in writing:** why can't this problem be solved with a Fenwick Tree plus the same `query(r)` vs `query(l-1)` trick used for sums? Your answer should name the algebraic property min lacks.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
