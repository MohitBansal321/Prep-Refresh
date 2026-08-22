# Greedy — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Greedy across its two structurally distinct flavors (no-sort running frontier/deficit, and the formula-driven schedule where the most frequent item dictates the skeleton). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers — with the header comment in each stating **the greedy choice** and **the exchange argument for why it is safe**, because in this pattern that argument is part of the solution, not decoration around it.

```bash
g++ -std=c++17 -Wall problems/01-jump-game.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Jump Game | [55](https://leetcode.com/problems/jump-game/) | Medium | No sort: sweep once extending a `farthest` frontier; fail fast if the cursor overtakes it. | O(n) time, O(1) space | [01-jump-game.cpp](01-jump-game.cpp) |
| Jump Game II | [45](https://leetcode.com/problems/jump-game-ii/) | Medium | Same frontier run as implicit BFS layers: one jump committed each time the cursor exhausts the current layer's boundary. | O(n) time, O(1) space | [02-jump-game-ii.cpp](02-jump-game-ii.cpp) |
| Gas Station | [134](https://leetcode.com/problems/gas-station/) | Medium | Running tank over a circle; when the tank dies at station j, every start between is provably doomed — reset once past it. | O(n) time, O(1) space | [03-gas-station.cpp](03-gas-station.cpp) |
| Task Scheduler | [621](https://leetcode.com/problems/task-scheduler/) | Medium | The most frequent task dictates a forced-idle skeleton; answer is `max(task count, (maxFreq-1)*(n+1) + ties)` — no simulation needed. | O(n) time, O(1) space | [04-task-scheduler.cpp](04-task-scheduler.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the purest statement of the *frontier* flavor — greedy with no sort at all, proving the pattern does not require ordering the input, and the canonical example of state compressing to a single scalar.
- **02** shows the same frontier doing double duty as implicit BFS: the layer boundary makes "minimum jumps" fall out of an `O(1)`-state sweep, and demonstrates greedy answering an optimization question (how few), not just reachability.
- **03** is the running-deficit flavor over a circular structure, with the sharpest discard argument of the four: one failed run eliminates every intermediate start simultaneously.
- **04** is the hardest proof in the set — the schedule-length formula — showing that some greedy arguments are about the *shape of any valid solution* (a skeleton forced by the most frequent task) rather than about sorting items and sweeping.

Together they also illustrate the module's central warning: none of these solutions would reveal a broken rule by crashing or timing out, which is why every file pairs its assertions with a written justification.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
