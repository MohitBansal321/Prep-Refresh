# 0/1 Knapsack — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating 0/1 Knapsack across the four framings the pattern shows up in: subset-sum feasibility, minimize-the-partition-difference, count-the-ways, and multi-axis capacity. Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers. Every file also cross-checks its DP against either a second implementation (2D table vs. 1D reverse sweep) or an independent `O(2^n)` brute-force oracle, so a subtle sweep-direction bug shows up as a failure instead of as a plausible-looking wrong number.

```bash
g++ -std=c++17 -Wall problems/01-partition-equal-subset-sum.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Partition Equal Subset Sum | [416](https://leetcode.com/problems/partition-equal-subset-sum/) | Medium | Reject odd totals, then boolean subset-sum for exactly `total/2`; `max` becomes OR. | O(n · total/2) time, O(total/2) space (1D) | [01-partition-equal-subset-sum.cpp](01-partition-equal-subset-sum.cpp) |
| Last Stone Weight II | [1049](https://leetcode.com/problems/last-stone-weight-ii/) | Medium | Signs → two piles; maximize the pile sum not exceeding `total/2`, answer `total - 2·pile`. | O(n · total/2) time, O(total/2) space (1D) | [02-last-stone-weight-ii.cpp](02-last-stone-weight-ii.cpp) |
| Target Sum | [494](https://leetcode.com/problems/target-sum/) | Medium | `sum(P) = (total+target)/2`; count subsets hitting it — `max` becomes `+=`, `dp[0] = 1`. | O(n · (total+target)/2) time, O((total+target)/2) space | [03-target-sum.cpp](03-target-sum.cpp) |
| Ones and Zeroes | [474](https://leetcode.com/problems/ones-and-zeroes/) | Medium | Capacity is the pair `(m zeros, n ones)`, value is a constant 1; sweep **both** axes backward. | O(L + k·m·n) time, O(m·n) space | [04-ones-and-zeroes.cpp](04-ones-and-zeroes.cpp) |

All four carry LeetCode's "Medium" tag, but they are ordered by how much reformulation work each one demands before the recurrence from the [README](../README.md) becomes visible: `01` needs one substitution, `04` needs the whole capacity concept re-imagined as a pair. Read them in order.

## Why these four

They cover every recognition signal called out in the [README](../README.md) — and, critically, **not one of them contains the words "weight," "value," or "capacity."** Recognizing the shape through that disguise is the actual transferable skill this pattern teaches, which is why every file here opens with an explicit "map this onto weight / value / capacity" reformulation section before any code.

- **01 — Partition Equal Subset Sum** is the **subset-sum framing** in its purest form, and the gateway to the whole family: set `value == weight`, replace `max` with "reachable or not," and the maximization problem becomes a feasibility question. It also does the two-step reformulation ("equal halves" → "one exact target" → "subset sum") that every other file in this directory reuses. Implemented twice (2D table and 1D reverse sweep) plus a `2^n` brute force, so all three must agree.
- **02 — Last Stone Weight II** is the **partition framing**: the problem statement describes a physical simulation, and the greedy heap simulation that solves its sibling (LeetCode 1046) gives the wrong answer here for the same structural reason greedy fails 0/1 Knapsack. Reformulating "smash stones" into "split into two piles, minimize the difference" turns it into `01` with the boolean table promoted back to integers — asking "how close to `total/2` can I get?" instead of "can I hit `total/2` exactly?". `01` is precisely the special case where this file's answer is 0, and a test asserts that.
- **03 — Target Sum** is the **count-the-ways framing**, the one place in the module where the recurrence's *operator* changes: `max(...)` becomes `+=`, because two disjoint sets of ways combine by addition, not by comparison. It is also the file with the most feasibility guards — odd `total + target`, `|target| > total`, and the negative-capacity crash they prevent — plus the zero-elements subtlety that each `0` in the input *doubles* the answer, which the reverse sweep handles with no special case and which a well-meaning `if (num == 0) continue;` silently breaks.
- **04 — Ones and Zeroes** is the case where **the "capacity" is the least obvious thing in the problem**: the input is an array of strings with no numbers in it at all, and the budget turns out to be the *pair* `(m zeros, n ones)` consumed simultaneously by every single choice, with each item's value pinned at a constant 1 so "maximize value" degenerates into "maximize subset size." It is the direct answer to the follow-up the README's Interview Discussion section flags, and it generalizes the reverse-sweep rule from "the inner loop goes backward" to the correct statement: **every dimension indexing a capacity must descend**, because getting one axis right and one wrong is the classic half-fix.

Two threads run through all four files deliberately. First, the **reverse capacity sweep**: each file contains a test whose only job is to fail loudly if the sweep direction were flipped (`01`'s `[2] -> false`, `02`'s `[2,2,2] -> 2`, `03`'s zero-doubling, `04`'s `["1"], m=0 n=3 -> 1`). Second, the **`value == weight` trick**, which is what lets a maximization recurrence answer feasibility (`01`), minimization (`02`), and counting (`03`) questions without changing its structure.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
