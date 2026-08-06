# Greedy

## Intent

Make the locally-optimal choice at each step — usually after sorting by some key — and prove that choice never needs to be reconsidered, rather than exploring both "take it" and "skip it" the way Dynamic Programming does.

## Recognition Signal

The problem can be solved by sorting on the right key and then making one irrevocable choice per step (interval scheduling, jump games, assignment problems). The defining feature: you only ever explore ONE branch, never both.

## Core Idea

Sort the input by a carefully-chosen key, then walk through it once, committing to a decision at each step that is never revisited. The hard part is never the code — it's proving the local choice is always safe, typically via an **exchange argument**: assume some optimal solution disagrees with the greedy choice, show you can swap in the greedy choice without making the solution worse, and conclude the greedy choice is at least as good as any alternative.

**Classic example — Activity Selection (maximize non-overlapping intervals):** sort by END time (not start time — a common wrong instinct). Greedily pick the first activity, then repeatedly pick the next activity whose start is at or after the previously-picked activity's end. This is provably optimal: among all activities that could be picked first, the one ending soonest leaves the most room for everything after it.

**Contrast with Dynamic Programming:** if you find yourself needing to consider *both* "take it" and "skip it" and compare results, that's DP, not Greedy. The defining feature of Greedy is that a proof lets you skip that comparison entirely — you know in advance which branch is safe.

## Template

```cpp
// Activity Selection: maximum number of non-overlapping intervals.
int maxNonOverlappingIntervals(std::vector<std::pair<int,int>> intervals) {
  std::sort(intervals.begin(), intervals.end(),
            [](auto& a, auto& b) { return a.second < b.second; });  // sort by END time
  int count = 0;
  int lastEnd = INT_MIN;
  for (auto& [start, end] : intervals) {
    if (start >= lastEnd) {   // no overlap with the last picked activity
      ++count;
      lastEnd = end;
    }
  }
  return count;
}
```

## Complexity

**Time:** `O(n log n)` — dominated by the sort; the greedy pass itself is `O(n)`.
**Space:** `O(1)` extra (beyond the sort's own space).

## Common Mistakes

- **Sorting by the wrong key.** Activity Selection sorted by START time (instead of END time) does not work — a classic, easy-to-fall-into trap.
- **Assuming a greedy approach exists without proving it.** Not every "sort and pick" problem has a valid greedy solution — if there's no exchange argument, the problem may genuinely need DP instead.
- **Confusing "greedy" with "any simple sort-based approach."** A greedy algorithm specifically commits to one choice per step without reconsidering it — an approach that revisits or compares alternatives after sorting is doing something closer to DP.

## When To Use

- The input can be sorted by a key such that processing it in that order and committing to each choice never needs to be undone (interval scheduling, jump games, gas station, task assignment).

## When NOT To Use

- **You need to consider both "take it" and "skip it" and compare outcomes** — that's Dynamic Programming, not Greedy.
- **No exchange argument exists** for the candidate greedy rule — if you can't prove the local choice is always safe, don't trust it just because it "seems to work" on a few examples.

## Similar Patterns

- **Merge Intervals** ([../../array-string-patterns/merge-intervals/](../../array-string-patterns/merge-intervals/)): itself a specific, provably-correct greedy strategy (sort by start, extend-or-flush) — one concrete application of the general Greedy pattern.
- **Dynamic Programming** (various, [../../dynamic-programming-patterns/](../../dynamic-programming-patterns/)): the fallback when no safe greedy rule exists — DP explores both branches and lets the recurrence decide, rather than committing upfront.

## Further Reading

- LeetCode — Jump Game (55), Gas Station (134), Task Scheduler (621), Non-overlapping Intervals (435).
- *Introduction to Algorithms* (CLRS) — exchange-argument correctness proofs for classical greedy algorithms (activity selection, Huffman coding).
