# Topological Sort — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions covering the four distinct questions this pattern gets asked: *can it be ordered at all*, *give me an order*, *is the order unique*, and *find the graph before you sort it*. Each file is self-contained — compile and run it directly to see printed PASS/FAIL output against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-course-schedule.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Course Schedule | [207](https://leetcode.com/problems/course-schedule/) | Medium | Kahn's, counting only: the answer is `placedCount == numCourses` — the order itself is never stored. | O(V + E) time, O(V + E) space | [01-course-schedule.cpp](01-course-schedule.cpp) |
| Course Schedule II | [210](https://leetcode.com/problems/course-schedule-ii/) | Medium | Kahn's, keeping the popped nodes; return `{}` if the final order is shorter than `numCourses`. | O(V + E) time, O(V + E) space | [02-course-schedule-ii.cpp](02-course-schedule-ii.cpp) |
| Sequence Reconstruction | [444](https://leetcode.com/problems/sequence-reconstruction/) | Medium | Kahn's with a per-step uniqueness check: the ready queue must hold exactly one node every iteration, and each pop must match `nums`. | O(n + L log L) time, O(n + L) space | [03-sequence-reconstruction.cpp](03-sequence-reconstruction.cpp) |
| Alien Dictionary | [269](https://leetcode.com/problems/alien-dictionary/) | Hard | Derive edges from the first mismatch of each adjacent word pair, then DFS post-order + reverse with white/gray/black cycle detection. | O(C) time, O(1) graph space (≤ 26 nodes) | [04-alien-dictionary.cpp](04-alien-dictionary.cpp) |

## Kahn's vs DFS — why each file picks what it picks

The [README](../README.md)'s Solution and Tradeoffs sections describe two standard ways to produce a topological order. These four files are arranged so the choice between them is made explicitly, with a reason, every time — the header comment of each file states which one it uses and why.

| File | Chosen | Deciding reason |
|---|---|---|
| 01 | Kahn's (BFS) | Only the cycle verdict is needed, and in Kahn's that is one integer comparison. DFS would need a three-state colour array for an answer a counter already gives. |
| 02 | Kahn's (BFS) | Kahn's emits in forward order (no final `std::reverse`), and "return `{}` on failure" falls straight out of the length check instead of unwinding a deep recursion. |
| 03 | Kahn's (BFS) | **Only Kahn's can answer this question.** Uniqueness ⟺ the ready queue holds exactly one node at every step; DFS never materialises "the set of currently-legal next nodes," so there is nothing to measure. |
| 04 | DFS post-order + reverse | The node set is discovered from the input, so DFS's per-node colour needs no separate in-degree pass; recursion depth is capped at 26 letters, removing the usual stack-depth objection; and this is where the module makes the alternative concrete rather than describing it. |

## Why these four

They cover every recognition signal in the [README](../README.md) and every branch of [images/recognition-diagram.md](../images/recognition-diagram.md):

- **01 — Course Schedule** is the recognition diagram's `CycleOnly` branch: the problem statement asks a yes/no question, so you run the whole algorithm and throw the order away. It is also the cleanest place to nail the edge-direction trap, because LeetCode's `{a, b}` pairs mean "b before a" — the edge runs *backwards* relative to how the pair is written.
- **02 — Course Schedule II** is the `Kahn` branch, and deliberately the same input as 01 so the diff between "is it possible" and "give me the plan" is a single `order.push_back(current)` line plus the final length check. Its tests include an orderable prefix feeding into a downstream cycle — the case where a missing length check returns a plausible-looking partial answer instead of failing.
- **03 — Sequence Reconstruction** attacks the README's first listed misconception ("topological sort always produces a unique answer") head-on. It is the only file where the ready queue's *size* is inspected rather than just its contents, and it is where the trace diagram's observation — two simultaneously-ready nodes mean two valid orders — becomes an executable test.
- **04 — Alien Dictionary** is the hardest facet: the graph does not exist until you build it. Roughly all of the difficulty lives above the topological sort, in three separate rules (only the first mismatch of an adjacent pair is an edge; non-adjacent pairs add nothing; a word followed by its own prefix is invalid input, detectable only by a length comparison). It also carries the module's DFS implementation, so the gray-node cycle check and the closing reversal are visible in code and not just in prose.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
