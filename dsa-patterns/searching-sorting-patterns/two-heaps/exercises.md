# Two Heaps — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing when a problem's "middle" or "boundary between two halves" shape calls for two balanced heaps rather than a single sorted structure, and (2) correctly stating, for every insert, which heap the new element belongs in and whether a rebalance is needed — before writing a single line of code.

> Rule of thumb for every exercise: before touching code, ask "do I need the exact middle (or a similarly fixed split point) of a growing/streaming set, queried repeatedly?" If the answer is yes, ask next: "does anything ever need to leave the dataset (a sliding window), or does it only ever grow?" That second question decides whether you need plain two heaps or two heaps plus lazy deletion.

---

## Easy — Find Median from Data Stream

**LeetCode 295 — Find Median from Data Stream.**

Design a data structure that supports adding integers from a data stream and finding the median of all elements seen so far, at any point.

**Task:** implement `addNum(int num)` and `findMedian()` using the two-heap balancing technique from [code.cpp](code.cpp) — a max-heap for the lower half, a min-heap for the upper half, rebalanced after every insert.

**Think about:** why must the rebalancing step run unconditionally after every single `addNum` call, rather than only when a `findMedian` call is about to happen? What would go wrong if you deferred rebalancing until query time?

---

## Medium — IPO

**LeetCode 502 — IPO.**

You are given `k` (the number of projects you may select, in order, one at a time), `w` (your current available capital), and two arrays `profits` and `capital`, where `capital[i]` is the minimum capital needed to start project `i` and `profits[i]` is the profit from completing it. Pick up to `k` distinct projects to maximize your final capital, where after finishing a project its profit is added to your capital (which may unlock previously-too-expensive projects).

**Task:** solve it with two heaps — a min-heap of not-yet-affordable projects ordered by required capital, and a max-heap of currently-affordable projects ordered by profit. On each of the `k` rounds, move every newly-affordable project (capital requirement `<=` current capital) from the min-heap into the max-heap, then greedily take the max-heap's most profitable project.

**Think about:** this is not literally computing a median, so why does the Similar Patterns section of the [README](README.md) still call it a structural cousin of Two Heaps? What is the "moving boundary" in this problem, and which heap tracks which side of it?

---

## Hard — Sliding Window Median

**LeetCode 480 — Sliding Window Median.**

Given an array `nums` and a window size `k`, return the median of each contiguous window of size `k` as the window slides from left to right across the array.

**Task:** extend the plain two-heap median finder with **lazy deletion**: since `std::priority_queue` cannot remove an arbitrary element cheaply, track which values have "expired" out of the window (in a hash map counting pending removals), and only actually pop a heap's top when it turns out to be one of those expired values — doing this cleanup immediately before every rebalance and every median read.

**Think about:** why is it not enough to just track heap *sizes* correctly when an element expires — what invariant beyond size would silently break if you only decremented a counter without also eventually removing the stale value from the heap it sits in?

---

## Real-World Challenge — Real-Time P50 Latency Dashboard

You operate a service that logs request latencies continuously (potentially thousands per second), and you need a dashboard endpoint that reports the **running median (P50) latency** since the process started, refreshed on every request without ever re-scanning the full history of latencies.

**Task:**
1. Design (and implement, using `MedianFinder` from [code.cpp](code.cpp) or your own two-heap class) a service-side component that ingests each new latency measurement via `addNum` and exposes the current P50 via `findMedian`, so a `/metrics` endpoint can read it in O(1) at any time.
2. Extend the design in writing (no code required) to also report the **P90** latency at the same time, updated incrementally. Can the exact two-heap 50/50 split be reused for this, or does answering a second, different percentile require a structurally different approach? Justify your answer using the Disadvantages and When NOT To Use sections of the [README](README.md).
3. Discuss: your service restarts periodically (deploys, crashes, autoscaling). What happens to the "running since process started" median across a restart, and would you want it to reset, or would you architect persistence/aggregation differently in production? What tradeoff are you making either way?

---

## Bonus Challenge — Kth Largest Element in a Stream (single-heap warm-up, generalized)

**LeetCode 703 — Kth Largest Element in a Stream.**

Design a class that, given an integer `k` and an initial array of integers, supports adding new integers to the stream one at a time and returns the k-th largest element in the stream after each addition.

**Task:** implement this with a single min-heap of size at most `k` (see [problems/04-kth-largest-element-in-a-stream.cpp](problems/04-kth-largest-element-in-a-stream.cpp) for the worked reference if you get stuck) — do **not** use two heaps here.

**Then, generalize in writing:** this problem uses one fixed-size heap, not two balanced heaps. Explain, in your own words, why "kth largest, fixed k" only needs one heap while "the median" needs two. What changes structurally if the "k" in this problem were allowed to grow proportionally with the stream (so it always represented "the middle" rather than a fixed count)? Would a single heap still suffice, and if not, why does the Two Heaps pattern become necessary at that point? Use the comparison table in the README's Similar Patterns section to support your answer.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
