# Sliding Window — Exercises

Work through these in order. Do not look up a solution; the goal is to build two reflexes: (1) recognizing whether a problem needs a **fixed-size** or **variable-size** window, and (2) correctly maintaining the running aggregate (sum, frequency map, distinct-count) on both the expand step and the shrink step.

> Rule of thumb for every exercise: before writing any code, answer *"is the window width given, or is it something I'm searching for?"* If given → fixed-size. If you're optimizing for longest/shortest → variable-size, and you must also check the constraint is **monotonic** in window size (see the Recognition Diagram) before trusting the shrink-loop approach.

---

## Easy — Average of Every Contiguous Subarray of Size K

Given an array of integers `arr` and an integer `k`, return a vector of the average of every contiguous subarray of size `k`.

**Requirements:**
- Use a **fixed-size window**: compute the sum of the first `k` elements once, then slide.
- Do not recompute any window's sum from scratch — each slide should be O(1).

**Acceptance:**
- For `arr = [1, 3, 2, 6, -1, 4, 1, 8, 2]`, `k = 5`, the first three averages should be `2.2, 2.8, 2.4`.

**Then answer in a comment:** if you used `int` instead of a wider/floating type for the running sum, what class of bug could appear on large inputs?

---

## Medium — Longest Substring With At Most Two Distinct Characters

Given a string `s`, find the length of the longest substring that contains **at most two distinct characters**.

**Task:**
1. Implement it as a **variable-size window** using a frequency map keyed by character.
2. Grow `right` and add to the frequency map; shrink from `left` while the map has more than 2 distinct keys.
3. Get the "erase vs. leave a zero-count entry" detail right — explain in a comment why leaving a `0`-count entry in the map would silently give a wrong answer for the "distinct count" check.

**Acceptance:**
- `s = "eceba"` → `3` (the substring `"ece"`).
- `s = "ccaabbb"` → `5` (the substring `"aabbb"`).

---

## Hard — Longest Substring With At Most K Distinct Characters, Then Generalize to Exactly K

Start from the Medium exercise, but make the distinct-character limit a parameter `k` (at most K distinct).

**Task:**
1. Verify your solution against `s = "araaci", k = 2` → answer `4` (`"araa"`).
2. Now implement **"exactly K distinct characters"** — a different problem. Hint: `longest(at most K) - longest(at most K-1)` is a well-known trick relating "exactly K" to two "at most" computations. Explain in a comment *why* this trick is valid (what does the difference actually represent?).
3. **Prove you understand the difference**, in writing: why is "at most K" a straightforward single sliding window, while "exactly K" is not directly expressible as one variable-size window with a simple grow/shrink rule?

---

## Real-World Challenge — Rate Limiter Over a Sliding Time Window

You are building a backend rate limiter: **allow at most N requests per user in any rolling 60-second window** (not a fixed per-minute bucket — a true sliding window, so a burst straddling a minute boundary is still capped correctly).

**Task:**
1. Model incoming requests as a stream of `(userId, timestampMs)` events arriving in time order.
2. Maintain, per user, a window of the request timestamps that fall within the last 60,000 ms. Use the same expand/shrink mechanics as the algorithmic pattern: when a new request arrives, expand the window (add the new timestamp); shrink from the front by dropping timestamps older than `now - 60000`.
3. A request is allowed if, after shrinking, the window's size is `< N`; otherwise reject it.
4. **Complexity:** argue why this is O(1) *amortized* per request (each timestamp is added once and removed once, even though a burst can require popping several old timestamps in one shrink).
5. **Production reality check:** this in-memory design works for one process. If your service runs behind a load balancer across multiple pods, in-memory sliding-window state per pod under-counts or over-counts the true rate. Describe (in writing, no code needed) how you would move this to Redis (e.g. a sorted set keyed by user with timestamps as scores, `ZREMRANGEBYSCORE` to shrink, `ZCARD` to check the count) to make the limit correct across a distributed deployment.

---

## Bonus Challenge — Sliding Window Maximum

Given an array `nums` and a window size `k`, return the maximum value in every contiguous window of size `k` (this is a fixed-size window, but the aggregate is not a simple sum — it's "the maximum," which is expensive to maintain naively).

**Task:**
1. First implement the naive version: for each window, scan all `k` elements to find the max. State its complexity.
2. Now implement the efficient version using a **deque** that stores indices, maintained so that it is always decreasing in value front-to-back, and the front is always the maximum of the current window. On each step: pop from the back while the new element is `>=` the value at the back index (those are now useless — the new element will outlast them and is bigger); push the new index; pop from the front if it has fallen outside the window `[right - k + 1, right]`.
3. **Prove the complexity claim:** argue why this is O(n) total despite the inner `while` loops, using the same "each index is pushed once and popped at most once" amortized argument used elsewhere in this module.
4. Compare this pattern to the two variable-size window templates in `code.cpp` — what is genuinely different about needing an auxiliary data structure (the deque) instead of a scalar running aggregate?

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
