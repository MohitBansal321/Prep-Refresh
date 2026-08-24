# Monotonic Stack/Queue — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "next greater/smaller" or "fixed-window max/min" signal that means "reach for a monotonic stack/deque," and (2) correctly deciding, on every step, *which* end to pop from, what the comparison should be, and why each popped element can be discarded for good.

> Rule of thumb for every exercise: before writing a single line, ask "what question does each stack entry wait to have answered?" and "when the incoming element pops it, what exactly becomes known?" If you cannot state both in one sentence, you are not ready to write the pop condition yet.

---

## Easy — Final Prices With a Special Discount

**LeetCode 1475 — Final Prices With a Special Discount in a Shop.**

Given array `prices` where `prices[i]` is the price of item `i`, the discount for item `i` is `prices[j]` where `j` is the **smallest index > i** with `prices[j] <= prices[i]` (or no discount if none exists). Return the final prices.

**Constraints to notice:** `1 <= prices.length <= 500`, so an O(n²) double loop passes on LeetCode — that is precisely why this is an *exercise* and not a proof of mastery. The discount rule is literally "next smaller-or-equal element," just wearing a retail costume.

**Task:** solve it with a monotonic stack in one pass, then solve it again with the naive double loop and confirm both agree on a few hand-made cases including ties (equal prices).

**Think about:** your pop condition compares `prices[stack.back()]` against the incoming value. Which comparison operator gives you "next smaller **or equal**" rather than strict "next smaller"? What breaks with equal prices if you pick the wrong one?

---

## Medium — Next Greater Element II (Circular Array)

**LeetCode 503 — Next Greater Element II.**

Given a circular integer array `nums` (the next element after the last is the first), return the next greater number for every element; -1 if none exists.

**Task:** adapt the linear next-greater template from [code.cpp](code.cpp) to handle wraparound. The standard trick is to iterate the loop `2n` times using index `i % n`, letting elements from the front of the array get a second chance to resolve stack entries pushed near the end.

**Think about:** in the doubled loop, do you push every index twice? Why is pushing only during the first `n` iterations (or equivalently, never re-pushing in the second pass) both sufficient and necessary for correctness? What would go wrong with the result array if you blindly wrote results during the second pass?

---

## Medium — Online Stock Span

**LeetCode 901 — Online Stock Span.**

Design a class that receives a stream of daily stock prices via `next(price)` and returns the **span**: the maximum number of consecutive days (including today) going backward for which the price was less than or equal to today's price.

**Constraints to notice:** up to 10⁴ calls, but the real constraint is that it is **online** — you must answer after each price arrives, with no lookahead. A per-call rescan of all past days is O(n) per call → O(n²) total; the intended solution answers each call in amortized O(1).

**Task:** implement it as a monotonic stack of `(price, span)` pairs. When a new price arrives, pop and accumulate the spans of everything it dominates before pushing.

**Think about:** this is "previous greater-or-equal element" rather than next greater — how does scanning direction flip relative to the template? Why does storing the accumulated span alongside each price make the amortized argument still work?

---

## Hard — Trapping Rain Water

**LeetCode 42 — Trapping Rain Water.**

Given `n` non-negative integers representing an elevation map of width-1 bars, compute how much water it traps after raining.

**Task:** solve it with a **monotonic stack** in O(n) time (the two-pointer O(1)-space solution also exists, but here the point is the stack formulation): maintain a stack of bar indices with decreasing heights; when a taller bar arrives, pop the top, treat it as the *floor* of a trapped-water layer, and compute water bounded by the new bar on one side and the new stack top on the other, layer by horizontal layer.

**Think about:** when you pop a middle bar, the water above it is bounded by `min(height[left wall], height[right wall]) - height[popped]`, times the horizontal distance between the two walls. Why does the stack invariant guarantee the popped bar's left wall is exactly the new `stack.back()`? Contrast this layer-by-layer accounting with the per-column accounting of the two-pointer approach — which one did you find easier to prove correct, and why?

---

## Real-World Challenge — Streaming Anomaly Detector with Lookahead Windows

You monitor a service's request-latency time series arriving as a stream of `(timestamp_ms, latency_ms)` samples, roughly one sample every 100 ms but with jitter and occasional gaps. Two alerts must fire in real time:

1. **"Recovery event":** for each sample, report how many milliseconds elapsed until the *first later sample whose latency strictly exceeds* it (ops uses this to measure how long each latency plateau lasted). If no later sample ever exceeds it, report "still open."
2. **"Spike alert":** whenever the max latency over the trailing 5-second window changes, emit the new max.

**Task:**
1. Design (and implement, reading from a `std::vector<std::pair<long long,long long>>` standing in for the stream) the recovery-event computation with a monotonic stack over the timestamp axis.
2. Implement the spike alert with a monotonic deque keyed by the fixed window size derived from timestamps.
3. Discuss: real streams arrive out of order by a few hundred ms due to network jitter. Both of your structures assume strictly increasing arrival order. Where exactly does each structure's correctness break if a late sample arrives *smaller* than recent ones after you already popped entries because of it? Propose a production mitigation (hint: bounded reorder buffer / watermark delay, same idea as stream-processing systems' allowed-lateness windows).

---

## Bonus Challenge — Maximal Rectangle

**LeetCode 85 — Maximal Rectangle.**

Given a binary matrix filled with 0s and 1s, find the largest rectangle containing only 1s and return its area.

**Task:** reduce the problem to Largest Rectangle in Histogram ([problems/03-largest-rectangle-in-histogram.cpp](problems/03-largest-rectangle-in-histogram.cpp)): process rows top to bottom, maintaining a running "histogram heights" array where `heights[j]` is the count of consecutive 1s ending at the current row in column `j` (reset to 0 on a 0 cell), and run the histogram algorithm once per row.

**Then, generalize in writing (no code required):** explain why this reduction is valid — i.e., why the largest all-1s rectangle in the matrix must appear as the largest rectangle under the histogram of *some* row. Justify it using the "each popped bar's rectangle extends between its two nearest smaller bars" argument from the histogram problem's [README](README.md).

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
