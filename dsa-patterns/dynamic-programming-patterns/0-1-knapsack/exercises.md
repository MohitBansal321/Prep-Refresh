# 0/1 Knapsack — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing a capacity-constrained "each item at most once" subset problem *even when the problem statement never says weight, value, or capacity*, and (2) correctly deciding which framing applies — maximize, feasibility, or counting — before writing any recurrence.

> Rule of thumb for every exercise: before writing a single line, ask "what exactly is the item, what exactly is the capacity, and what is the value being optimized (or the property being counted)?" If you cannot name all three in one sentence, you are not ready to write the recurrence yet — and if there is no genuine resource-under-budget axis at all, this pattern does not apply no matter how "include or exclude" the decision feels.

---

## Easy — House Robber (Recognition Exercise)

**LeetCode 198 — House Robber.**

Given an array `nums` representing money stashed in houses along a street, return the maximum you can rob tonight without alerting the police — you cannot rob two adjacent houses.

**Constraints to notice:** each house is chosen **at most once**, and the decision for each house is binary (rob it or skip it). That looks like 0/1 Knapsack's shape — but check the rule-of-thumb question above before committing.

**Task:** decide whether this problem is actually a 0/1 Knapsack instance. Write down your answer as three explicit claims ("the item is ___, the capacity is ___, the value is ___") and test each one against the problem's actual constraint structure.

**Think about:** is there a numeric budget that gets *consumed* by your choices, such that feasibility depends on remaining budget? Or is the only constraint positional (adjacency), which depends on *where* you are in the array rather than on how much of a resource you have left? What does that make the state variable — and is the resulting DP simpler or more complex than the knapsack table?

---

## Medium — Number of Dice Rolls With Target Sum

**LeetCode 1155 — Number of Dice Rolls With Target Sum.**

You have `n` dice, each with `k` faces numbered `1` through `k`. Return the number of distinct ways to roll the dice so the face-up numbers sum to `target`, modulo `10^9 + 7`.

**Constraints to notice:** each die contributes exactly one face value from a fixed set (`1..k`) and is then gone — a bounded choice per item, used once. The answer is a **count**, not a maximum. LeetCode's bounds keep `n`, `k`, and `target` small enough for a table.

**Task:** reformulate this as a knapsack-family counting problem: identify what plays the role of the items, what plays the role of the capacity, and what replaces `max` in the recurrence. Then implement it with a 2D table first, and only afterwards attempt the 1D space optimization.

**Think about:** when two disjoint sets of ways combine, why must the combining operator be addition rather than comparison? Which worked problem in [problems/](problems/README.md) makes the same operator change, and what did its base case become? For the 1D version: given that each die's contribution is at least 1, which direction must the inner sweep go to avoid letting one die contribute twice?

---

## Hard — Profitable Schemes

**LeetCode 879 — Profitable Schemes.**

There are `n` members available and `minProfit` needed. Each crime requires a fixed number of `group[i]` members and yields `profit[i]`. Count how many subsets of crimes can be chosen so total members used stays `<= n` and total profit reaches at least `minProfit`, modulo `10^9 + 7`.

**Constraints to notice:** each crime is usable **at most once**; the budget is a **pair** `(members, profit)` consumed simultaneously by every choice — directly analogous to the multi-axis capacity idea in the module's worked problems. The counting target is also subtly non-standard: profit is capped by an "*at least* minProfit" condition, so states above the threshold collapse together.

**Task:** build the DP over both capacity axes at once, with the counting operator. Decide explicitly, before coding, how you will represent "profit >= minProfit" inside a finite table — there is a one-line clamping trick, but you must derive where and why it is applied (at read time or write time).

**Think about:** which worked problem in [problems/](problems/README.md) already established that *every* dimension indexing a capacity must be swept backward in the compressed form? Does that rule apply to both axes here? Also consider the edge cases LeetCode's constraints allow: `minProfit = 0` (does the empty subset count?) and a crime with zero required members.

---

## Real-World Challenge — CI Test-Suite Selector Under a Time Budget

Your CI system has a hard wall-clock budget of `B` minutes per commit. You maintain `n` candidate test suites; suite `i` takes `t_i` minutes and historically catches bugs worth a score `v_i` (derived from flakiness-adjusted coverage data). Suites cannot be split or run partially, and each runs at most once per commit.

**Task:**
1. Model this as a 0/1 Knapsack instance and implement it generically (weights, values, capacity) using the 1D reverse-sweep form from [code.cpp](code.cpp).
2. Extend it: engineering wants to know *which* suites were selected, not just the total score. Decide whether you keep the full 2D table or add parallel bookkeeping, and justify the memory tradeoff.
3. Discuss: next quarter the budget becomes `B'` measured in dollars of compute cost instead of minutes, with suites priced in fractional cents. What breaks about the table-based approach as the capacity magnitude grows, and what would you do about it in production (hint: revisit the pseudo-polynomial discussion in the [README](../README.md))?

**Think about:** which axis is genuinely the capacity here, and could a second capacity axis (e.g., a parallel-worker limit) appear if suites also consumed concurrency slots? How would your dp dimensions change?

---

## Bonus Challenge — Split Array With Same Average

**LeetCode 805 — Split Array With Same Average.**

Given `nums`, determine whether you can split it into two non-empty subsequences A and B such that the average of A equals the average of B.

**Constraints to notice:** LeetCode caps `n` at 30 but lets values reach `10^5` — so the naive transformed target can be far too large for a plain table, while `n` is small enough for smarter enumeration. This is the boundary case the [README](../README.md)'s "When NOT To Use" section warns about.

**Task:** first do the algebra on paper: if A has average equal to the overall average, what exact sum must A have? Derive the transformed per-element values that turn the condition into a subset-sum question, then decide — given the constraint profile — whether a straight DP table, a size-constrained DP (tracking subset cardinality as an extra dimension), or meet-in-the-middle enumeration is the right tool, and defend the choice against the pseudo-polynomial pitfall.

**Think about:** why does the transformed subset-sum need the extra "how many elements chosen" dimension to be correct (what false positive appears without it)? And why can you prune any candidate subset size whose implied target sum is impossible given the value range?

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
