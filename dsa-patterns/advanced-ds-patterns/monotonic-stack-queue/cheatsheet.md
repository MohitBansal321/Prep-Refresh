# Monotonic Stack/Queue — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Advanced DS pattern — order-maintaining auxiliary structure over an array scan. |
| **Recognition Signal** | The problem asks for the **"next greater/smaller element"** for every position (left, right, or circularly), or the **max/min of every fixed-size sliding window**, or a per-position answer that depends on "how far until something bigger/smaller appears" (spans, areas, temperatures). |
| **Problem** | For each position, naively scanning rightward for its next greater element costs O(n²); computing each window's max by scanning the window costs O(n·k). |
| **Solution** | Keep a **stack (or deque) of indices whose values are monotonic**. When a new element arrives, pop everything it "beats" — each popped element has just found its answer (the new element IS its next-greater / can never be a future window's max). Push the new index and continue. |
| **Time / Space Complexity** | O(n) time total — each index is pushed exactly once and popped at most once, so the inner while loop is **amortized** O(1). O(n) worst-case extra space for the stack/deque (e.g. strictly decreasing input never pops). |
| **Pros** | Turns an O(n²)/O(n·k) scan into a single O(n) pass · answers fall out as a side effect of maintenance, no separate query phase · same skeleton solves next-greater, spans, histogram areas, window max/min · simple to prove correct via the amortized push/pop count. |
| **Cons** | Only answers *one-step lookahead* ("next") or *fixed sliding window* questions — not arbitrary range max/min · easy to get the comparison direction wrong (`<` vs `<=` decides strict vs non-strict monotonicity and duplicate handling) · stack must hold indices, not values, whenever positions matter · off-by-one risk on the deque's front-eviction check. |
| **Use When** | "Next greater/smaller element" for every position · stock-span / daily-temperature style "how far to the previous/next X" problems · largest rectangle under a histogram · sliding-window maximum/minimum with fixed k. |
| **Avoid When** | You need max/min over **arbitrary ranges** (use Segment Tree / Fenwick Tree) · you need a running **median** or order statistic (use Two Heaps) · you need a running aggregate like sum/count (a plain running variable works — no structure needed) · the window size varies arbitrarily during the scan. |
| **Related Patterns** | Sliding Window (same left-to-right scan, but tracks running aggregates rather than max/min — the monotonic deque is what makes window-max cheap on removal) · Two Heaps (running median instead of running max/min) · Stack (the degenerate case where you only need matching/nesting, not ordering). |

### Template Skeleton

```cpp
// Next Greater Element for every position.
std::vector<int> nextGreaterElement(const std::vector<int>& nums) {
  int n = static_cast<int>(nums.size());
  std::vector<int> result(n, -1);
  std::vector<int> st;  // holds INDICES; values non-increasing bottom-to-top
  for (int i = 0; i < n; ++i) {
    while (!st.empty() && nums[st.back()] < nums[i]) {
      result[st.back()] = nums[i];  // nums[i] is st.back()'s next greater
      st.pop_back();                // popped for good — resolved forever
    }
    st.push_back(i);
  }
  return result;
}

// Sliding Window Maximum (monotonic DEQUE variant).
std::vector<int> maxSlidingWindow(const std::vector<int>& nums, int k) {
  std::deque<int> dq;   // indices, values strictly decreasing front-to-back
  std::vector<int> result;
  for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
    while (!dq.empty() && nums[dq.back()] < nums[i]) dq.pop_back();
    dq.push_back(i);
    if (dq.front() <= i - k) dq.pop_front();  // fell out of the window
    if (i >= k - 1) result.push_back(nums[dq.front()]);
  }
  return result;
}
```

### Remember In One Sentence
> **A Monotonic Stack keeps indices whose values stay ordered so that every new element instantly resolves everything it dominates — turning "next greater/smaller" and sliding-window-max questions into a single O(n) pass because each index is pushed once and popped at most once.**

### Two Facts People Get Wrong
- The inner `while` loop makes it O(n²)? **No** — every index enters the stack exactly once and leaves at most once, so total pushes + pops across the entire run is at most 2n; the inner loop's cost is charged to the pops, giving amortized O(1) per element.
- The stack should store values? **No** — store **indices**: you need the position both to write results back to the right slot and to check whether a deque front has slid out of the current window (`dq.front() <= i - k`). Values are always recoverable via `nums[index]`.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the two recognition signals that mean "reach for a Monotonic Stack/Queue."
2. Why does the inner `while` loop not make the algorithm O(n²)? Give the exact accounting argument.
3. Why must the stack hold indices rather than values? Name two places where the index is required.
4. In the next-greater template, when index `j` is popped by incoming element `i`, what do we know about `nums[i]` relative to `nums[j]`, and what does that let us write into `result[j]`?
5. In the sliding-window-max deque, why is it safe to pop smaller elements from the BACK when a new larger element arrives?
6. What is the exact condition for evicting from the FRONT of the deque, and which window boundary does it encode?
7. How would you change the comparison in the next-greater template to compute "next greater OR EQUAL" instead of strict next greater?
8. How does Largest Rectangle in Histogram reuse the next-smaller-element idea, and what role does the width computation play when popping?
9. Name two problems where a Monotonic Stack is the WRONG tool, and the pattern that should be used instead.
10. What happens to the stack's contents after processing a strictly decreasing array, and what does that tell you about worst-case space?

