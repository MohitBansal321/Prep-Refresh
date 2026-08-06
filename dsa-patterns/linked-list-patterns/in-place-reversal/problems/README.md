# In-place Reversal — Worked Problems

Four fully worked, standalone, heavily commented C++17 solutions. Each file defines its own `ListNode` struct and helper functions, so any file can be compiled and run in isolation — you do not need `code.cpp` or any other file in this folder.

Compile and run any of them with:

```bash
g++ -std=c++17 -Wall path/to/file.cpp -o /tmp/out && /tmp/out
```

| # | Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|---|------|-----------|------------|--------------------|---------------|------|
| 1 | Reverse Linked List | [206](https://leetcode.com/problems/reverse-linked-list/) | Easy | Classic three-pointer (`prev`/`curr`/`next`) rewiring over the whole list. | O(n) time, O(1) space | [01-reverse-linked-list.cpp](01-reverse-linked-list.cpp) |
| 2 | Reverse Linked List II | [92](https://leetcode.com/problems/reverse-linked-list-ii/) | Medium | Same rewiring, restricted to the `[left, right]` sub-range, anchored by a dummy head so `left == 1` needs no special case. | O(n) time, O(1) space | [02-reverse-linked-list-ii.cpp](02-reverse-linked-list-ii.cpp) |
| 3 | Reverse Nodes in k-Group | [25](https://leetcode.com/problems/reverse-nodes-in-k-group/) | Hard | Repeat the sub-range reversal group by group, checking `k` nodes remain before reversing each group; the final short group is left untouched. | O(n) time, O(1) space | [03-reverse-nodes-in-k-group.cpp](03-reverse-nodes-in-k-group.cpp) |
| 4 | Swap Nodes in Pairs | [24](https://leetcode.com/problems/swap-nodes-in-pairs/) | Medium | The k-group reversal with `k` fixed at 2 — the "warm up" version of problem 25. | O(n) time, O(1) space | [04-swap-nodes-in-pairs.cpp](04-swap-nodes-in-pairs.cpp) |

## Why these four

- **206** is the pattern in its purest form — no sub-range, no groups, just the three-pointer loop over the entire list. Every other variant builds on this one.
- **92** introduces the dummy-head-node bookkeeping needed the moment a reversal does not start at the true head, or *might* start at the true head depending on the input (`left == 1` must not require a special-case branch).
- **25** composes the sub-range idea into a loop over multiple groups, adding the "confirm a full group exists first" check — the step most solutions get wrong under time pressure, and the reason this problem is rated Hard despite reusing an Easy problem's core loop.
- **24** shows that a problem can look like a distinct, separately-numbered LeetCode question (and even predate its "harder" sibling on the site) while actually being a special case of a more general pattern already covered — recognizing "this is k-group reversal with k=2" is itself the skill being tested.

`exercises.md` (in the parent folder) gives you more problems to solve **without** a worked solution, for deliberate practice once you've studied these four.
