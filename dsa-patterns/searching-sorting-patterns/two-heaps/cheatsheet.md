# Two Heaps — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Searching/Sorting pattern — heap-based order-statistic tracking. |
| **Recognition Signal** | Problem needs the **running median** (or a similarly fixed "middle" order-statistic) of a **growing or streaming** set of numbers, with queries interleaved with inserts. Bonus signal: a "moving boundary between an affordable/eligible set and an ineligible set" shape (IPO-style problems). |
| **Problem** | Re-sorting the whole dataset on every query costs O(n log n) per query; a single sorted array costs O(n) per insert (shifting elements) to keep O(1) reads. |
| **Solution** | A max-heap holds the **lower half**, a min-heap holds the **upper half**, kept balanced in size (differ by at most 1) after every insert. The median is read from the two heaps' top elements — no sort, no scan. |
| **Participants** | `low` (max-heap, lower half) · `high` (min-heap, upper half) · the rebalancing rule (keeps sizes within 1 of each other) · the insertion-routing decision (which heap a new value belongs in). |
| **Flow** | 1. Insert: route the new number into `low` or `high` based on comparison against `low.top()`. 2. Rebalance: if sizes differ by more than 1, move the larger heap's top to the smaller heap. 3. Query: equal sizes -> average both tops; unequal -> the larger heap's top is the median. |
| **Time / Space Complexity** | O(log n) per insert (one push + at most one rebalance pop/push), O(1) per median query, O(n) total space. |
| **Pros** | O(1) median reads after O(log n) inserts · no full re-sort ever · naturally incremental/streaming-friendly · built from off-the-shelf `std::priority_queue`, no custom tree. |
| **Cons** | Only answers the fixed middle split, not arbitrary percentiles · two structures whose two invariants (value split + size balance) must both be kept correct · `std::priority_queue` cannot cheaply remove an arbitrary element, so a sliding-window variant needs added lazy deletion. |
| **Use When** | Running median of a stream · a fixed middle order-statistic queried repeatedly as data grows · IPO-style "affordable vs. not-yet-affordable" scheduling (structural cousin, same two-heap-balance idea). |
| **Avoid When** | You need arbitrary percentiles (P90, P99), not just the median — use a quantile sketch (t-digest) or an order-statistics tree instead · you only need a running max/min/sum/mean (use a single running variable) · the dataset is static and you only need the median once (use `std::nth_element`, O(n), no heap bookkeeping). |
| **Real Examples** | Median/percentile latency tracking in monitoring systems · real-time analytics dashboards (median order value, median page-load time) · financial tick-data running-median computation. |
| **Related Patterns** | Top K Elements (one fixed-size heap tracking extreme K values, not the middle) · Merge Intervals (interval sweep; "meeting rooms" min-rooms variant uses a single heap of end-times, structurally closer to Top K than Two Heaps) · Order-statistics tree (answers k-th smallest for any k, strictly more powerful and more complex than what's needed for just the median). |

### Template Skeleton

```cpp
#include <queue>

class MedianFinder {
 public:
  void addNum(int num) {
    // 1. Route into the correct half.
    if (low_.empty() || num <= low_.top()) {
      low_.push(num);
    } else {
      high_.push(num);
    }

    // 2. Rebalance sizes unconditionally, every time.
    if (low_.size() > high_.size() + 1) {
      high_.push(low_.top());
      low_.pop();
    } else if (high_.size() > low_.size()) {
      low_.push(high_.top());
      high_.pop();
    }
  }

  double findMedian() const {
    if (low_.size() == high_.size()) {
      return (low_.top() + high_.top()) / 2.0;
    }
    return low_.top();  // low_ holds the extra element on odd counts.
  }

 private:
  std::priority_queue<int> low_;                                              // max-heap
  std::priority_queue<int, std::vector<int>, std::greater<int>> high_;        // min-heap
};
```

### Remember In One Sentence
> **Two Heaps turns an O(n log n)-per-query re-sort into O(log n)-per-insert and O(1)-per-query, by keeping a max-heap of the lower half and a min-heap of the upper half always balanced in size, so the median is never more than two heap-top reads away.**

### Two Facts People Get Wrong
- Two Heaps gives you any percentile cheaply, not just the median? **No** — the 50/50 split is fixed; a different percentile needs a different structure entirely (a quantile sketch or an order-statistics tree), not a re-tuned two-heap ratio.
- A `std::priority_queue` can remove an arbitrary aging-out element in O(log n), so sliding-window median is a trivial extension? **No** — `std::priority_queue` only supports removing its root cheaply; a sliding window needs an added lazy-deletion layer (tracking expired values and cleaning them off the top when encountered) to work at all.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. State the two invariants that must both hold after every insert in a two-heap median finder.
2. Why is `std::priority_queue<int>` a max-heap by default, and what one change makes it a min-heap?
3. When the two heaps are the same size, how do you compute the median? When they differ by one, how?
4. Why does rebalancing need to happen after *every* insert rather than only right before a `findMedian` call?
5. What is the time complexity of `addNum`, and what is the time complexity of `findMedian`? Why are they different?
6. Name the two brute-force alternatives Two Heaps replaces, and the complexity each one costs per insert or per query.
7. Why can't a plain `std::priority_queue` support a sliding-window median without an extra mechanism, and what is that mechanism called?
8. What real-world production use case (not a LeetCode problem) motivates tracking a running median instead of a running mean?
9. Why is IPO (LeetCode 502) considered a structural cousin of Two Heaps even though it is not computing a median?
10. What is the key structural difference between Two Heaps and Top K Elements, given that both use one or more heaps?
