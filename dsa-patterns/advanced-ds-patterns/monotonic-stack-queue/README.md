# Monotonic Stack/Queue

## Intent

Keep a stack (or deque) whose elements stay in strictly increasing or decreasing order, popping elements that can never be part of the answer again, to answer "next greater/smaller element" and sliding-window-max/min questions in `O(n)` total instead of `O(n^2)`.

## Recognition Signal

The problem asks for the "next greater/smaller element" for every position, or the max/min over every sliding window of a fixed size.

## Core Idea

**Monotonic stack (next greater/smaller element):** scan the array left to right, maintaining a stack that stays in strictly increasing (or decreasing) order of value from bottom to top. For each new element, pop everything on the stack that it "beats" (e.g. for "next greater," pop everything smaller than the current element) — each popped element has just found its answer (the current element IS its next-greater), and it can never usefully be compared against anything else, so it's popped for good. Push the current element, and move on. The total number of pushes and pops across the whole scan is bounded by `2n`, giving `O(n)` overall despite the inner "while" loop.

**Monotonic deque (sliding window maximum):** maintain a deque of indices whose corresponding values are in strictly decreasing order from front to back. For each new element, pop from the BACK any indices whose value is smaller than the current element (they can never be the max of any future window that also contains the current element). Pop from the FRONT any index that has fallen out of the current window. The front of the deque is always the current window's maximum.

## Template

```cpp
// Next Greater Element for every position (circular or linear).
std::vector<int> nextGreaterElement(const std::vector<int>& nums) {
  int n = nums.size();
  std::vector<int> result(n, -1);
  std::vector<int> stack;  // holds INDICES; values increase bottom-to-top
  for (int i = 0; i < n; ++i) {
    while (!stack.empty() && nums[stack.back()] < nums[i]) {
      result[stack.back()] = nums[i];
      stack.pop_back();
    }
    stack.push_back(i);
  }
  return result;
}

// Sliding Window Maximum.
std::vector<int> maxSlidingWindow(const std::vector<int>& nums, int k) {
  std::deque<int> dq;  // holds indices, values strictly decreasing front-to-back
  std::vector<int> result;
  for (int i = 0; i < (int)nums.size(); ++i) {
    while (!dq.empty() && nums[dq.back()] < nums[i]) dq.pop_back();
    dq.push_back(i);
    if (dq.front() <= i - k) dq.pop_front();  // fell out of the window
    if (i >= k - 1) result.push_back(nums[dq.front()]);
  }
  return result;
}
```

## Complexity

**Time:** `O(n)` total for either variant — each element is pushed once and popped at most once, despite the inner while loop.
**Space:** `O(n)` worst case for the stack/deque.

## Common Mistakes

- **Forgetting the stack/deque holds INDICES, not values** — needed to know both the value and its position (for window-boundary checks or to write results back to the correct index).
- **Popping from the wrong end of the deque**, or forgetting the front-eviction check (`dq.front() <= i - k`) in the sliding-window-max variant — the window boundary check is easy to get off-by-one.
- **Assuming this pattern gives you the max/min over an ARBITRARY range** — it only efficiently answers the *next* greater/smaller (one lookahead) or a *fixed-size sliding* window, not arbitrary range-max queries (that's a Segment Tree's job).

## When To Use

- "Next greater/smaller element" for every array position (in either direction, or circularly).
- Sliding-window maximum/minimum over a fixed window size.

## When NOT To Use

- **Need arbitrary range-max/min queries (not just a sliding window)** — that's Segment Tree / Fenwick Tree territory ([../segment-tree-fenwick-tree/](../segment-tree-fenwick-tree/)).
- **Need the running median or a general order statistic** — that's Two Heaps ([../../searching-sorting-patterns/two-heaps/](../../searching-sorting-patterns/two-heaps/)).

## Similar Patterns

- **Sliding Window** ([../../array-string-patterns/sliding-window/](../../array-string-patterns/sliding-window/)): also processes a window left to right, but tracks a running aggregate (sum, frequency count) rather than needing the max/min specifically — the monotonic deque is what makes window-max/min efficient, since a naive running aggregate doesn't handle "max" cheaply on removal.
- **Two Heaps** ([../../searching-sorting-patterns/two-heaps/](../../searching-sorting-patterns/two-heaps/)): the right tool for a running median instead of a running max/min.

## Further Reading

- LeetCode — Next Greater Element I (496), Daily Temperatures (739), Sliding Window Maximum (239, hard).
- GeeksforGeeks — "Monotonic Stack" explainer.
