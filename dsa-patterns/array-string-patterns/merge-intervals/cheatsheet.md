# Merge Intervals — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Array/String pattern — sort-and-sweep technique over ranges. |
| **Recognition Signal** | Input is a list of `[start, end]` ranges, and the question involves **overlap, merging, insertion, or scheduling conflicts** ("merge these meetings," "insert this booking," "minimum rooms/removals/arrows"). |
| **Problem** | Brute-force pairwise overlap checking costs O(n²) — comparing every interval against every other interval to decide what merges with what. |
| **Solution** | **Sort by start** (or, for counting-only variants, **sort by end**), then sweep once keeping a running "current" interval: extend it if the next interval overlaps, flush it and start a new one if not. |
| **Time / Space Complexity** | O(n log n) time (dominated by the sort), O(n) space for the output (O(1) extra beyond that) — versus O(n²) for pairwise comparison. |
| **Pros** | O(n log n) total vs. O(n²) brute force · single linear sweep after sorting · easy to reason about (one comparison per step) · generalizes cleanly to insertion, counting, and greedy-removal variants. |
| **Cons** | Requires sorting first — no better than O(n log n) even if a hash-based shortcut exists elsewhere · does not handle truly unsorted **streaming** intervals without re-sorting · off-by-one on closed/open boundary treatment is an easy silent bug. |
| **Use When** | Merging overlapping ranges into disjoint blocks · inserting one new range into an already-sorted, disjoint list · counting minimum removals/groups to eliminate all overlap (non-overlapping intervals, minimum arrows). |
| **Avoid When** | You need a running **median**/order-statistic of a stream (use Two Heaps) · you need the count of intervals **simultaneously active** at any instant, not just pairwise overlap (consider a start/end event sweep or Two Heaps) · intervals arrive one at a time online and re-sorting every arrival is too costly for the throughput required. |
| **Real Examples** | Calendar/meeting-block merging · hotel/resource booking systems · CPU/IO job scheduling · balloon/interval counting puzzles that model resource-allocation problems. |
| **Related Patterns** | Two Heaps (median/"middle" order statistic of a stream, not sort-and-sweep) · Greedy (Merge Intervals' sort-by-end counting variant is one specific instance of the general greedy "sort by a key, commit per step" idea). |

### Template Skeleton

```cpp
// Merge (sort by START, build the merged output)
std::sort(intervals.begin(), intervals.end(),
          [](auto& a, auto& b) { return a.first < b.first; });

std::vector<std::pair<int,int>> merged;
auto current = intervals[0];
for (size_t i = 1; i < intervals.size(); ++i) {
    if (intervals[i].first <= current.second) {
        current.second = std::max(current.second, intervals[i].second);  // extend
    } else {
        merged.push_back(current);   // flush
        current = intervals[i];
    }
}
merged.push_back(current);  // flush the final running interval — easy to forget!

// Count groups / minimum removals (sort by END)
std::sort(intervals.begin(), intervals.end(),
          [](auto& a, auto& b) { return a.second < b.second; });

int groups = 1;
int lastEnd = intervals[0].second;
for (size_t i = 1; i < intervals.size(); ++i) {
    if (intervals[i].first > lastEnd) {
        ++groups;                    // starts a new, non-overlapping group
        lastEnd = intervals[i].second;
    }
    // else: overlaps the current group — no new group, lastEnd unchanged
}
```

### Remember In One Sentence
> **Merge Intervals replaces an O(n²) pairwise overlap check with a single O(n) sweep by sorting once (by start to merge, by end to count groups) so that the only interval that can ever overlap the one you are building is the very next one in order.**

### Two Facts People Get Wrong
- Merge Intervals always sorts by **start**? **No** — problems that only need a *count* (minimum removals, minimum arrows) sort by **end**, because the earliest-ending interval gives the tightest possible boundary for closing out the current group as early as possible.
- Touching endpoints like `[1,3]` and `[3,5]` are **not** overlapping? **Depends on the problem's stated boundary convention** — most Merge Intervals problems treat intervals as closed (inclusive) on both ends, so `next.start <= current.end` (not `<`) counts a touch as an overlap; get this comparison operator wrong and you silently under- or over-merge.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the [README](README.md). If you miss one, that section is the only thing you need to re-study.

1. Why must the intervals be sorted before the sweep, and what specifically breaks if you skip the sort?
2. State the exact comparison used to decide "overlap vs. flush" in the merge sweep, and explain what it means for closed intervals.
3. Why do Non-overlapping Intervals and Minimum Arrows sort by **end** time instead of **start** time, when plain Merge Intervals sorts by start?
4. What is the single most common off-by-one bug in a first implementation of the merge sweep?
5. Why does Insert Interval not need to re-sort the whole list, when Merge Intervals does?
6. Name the brute-force complexity that Merge Intervals replaces, and what the replacement complexity is.
7. When would you reach for Two Heaps instead of Merge Intervals, even though both can appear in "scheduling"-flavored problems?
8. Why is `current.end = max(current.end, next.end)` written with `max()` instead of a plain assignment `current.end = next.end`?
9. Give one real production/system context (not a LeetCode problem) where the sort-by-start-and-sweep idea shows up directly.
10. What breaks about the "which pointer/interval moves next" argument if the input arrives as a true unsorted online stream rather than a fixed array you can sort up front?
