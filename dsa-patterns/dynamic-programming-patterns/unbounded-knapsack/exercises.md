# Unbounded Knapsack — Exercises

Work through these in order. The goal is to build two reflexes: (1) recognizing the "unlimited reuse of each item/coin/piece under a capacity or exact target" signal that means "reach for Unbounded Knapsack," and (2) correctly choosing the fill direction (forward) and, for counting problems, the loop nesting order — the two details this pattern is most commonly gotten subtly wrong on.

> Rule of thumb for every exercise: before writing a single line, ask "can each item/coin/piece be used more than once?" and "am I maximizing a value, minimizing a count to an exact target, or counting the number of distinct ways?" If you cannot answer both questions, you are not ready to write the dp recurrence yet — and if your answer to the first question is "no," you want [../0-1-knapsack/](../0-1-knapsack/) instead, not this pattern.

---

## Easy — Climbing Stairs (with variable step sizes)

**LeetCode 70 — Climbing Stairs**, generalized: instead of only 1 or 2 steps at a time, you may climb `1`, `2`, or `3` steps at a time. How many distinct ways are there to reach the top of an `n`-step staircase?

**Constraints to notice:** each step size (1, 2, or 3) can be used any number of times, in any order, and different **orders** of the same step sizes count as different ways (climbing "1 then 2" is a different way from climbing "2 then 1" — this is a *permutations* count, not a *combinations* count).

**Task:** solve it with a 1D dp table, `dp[i]` = number of distinct ways to reach step `i`, filled with `i` increasing from `1` to `n`.

**Think about:** in [problems/02-coin-change-ii.cpp](problems/02-coin-change-ii.cpp), counting *combinations* requires the coin loop on the outside. Here, order matters (this is a permutations count). Which loop should be on the outside for this problem, and why does that answer differ from Coin Change II's?

---

## Medium — Rod Cutting

**A classic (not a single fixed LeetCode number, but ubiquitous in DP textbooks and interview prep) — Rod Cutting.**

Given a rod of length `n` and a price table `price[1..n]` (the sale price of a piece of each possible length, where `price[i]` may not scale linearly with `i`), determine the maximum total sale value obtainable by cutting the rod into pieces of any lengths (including "not cutting it at all," i.e. selling it as one piece of length `n`) and selling each piece.

**Task:** implement it using the maximize-value flavor of the pattern (`dp[w] = max(dp[w], dp[w - length] + price[length])` for every possible cut length), forward-filling `dp` from `1` to `n`.

**Then answer:** why is "not cutting the rod at all" automatically included in this recurrence's search space, without needing a special case for it?

---

## Hard — Combination Sum IV

**LeetCode 377 — Combination Sum IV.**

Given a list of distinct positive integers `nums` and a target `target`, return the number of possible combinations that add up to `target`, where **the order of numbers matters** (i.e. this counts arrangements/permutations, not combinations, despite the problem's name).

**Task:** solve it in O(target * nums.size()) time using a 1D dp table where `dp[i]` = number of ways (with order mattering) to reach sum `i`. Be deliberate about which axis is the outer loop, using your answer from the Easy exercise above as a starting hypothesis, then verify it against a hand-traced small example (e.g. `nums = [1, 2, 3]`, `target = 4`).

**Then answer:** LeetCode 377's name says "Combination Sum" but the actual counting rule is order-sensitive (a permutations count). Contrast this explicitly against [problems/02-coin-change-ii.cpp](problems/02-coin-change-ii.cpp) — same shape of input (a list of positive integers and a target), same `dp[i] += dp[i - num]` recurrence body, but a different loop nesting order. Write out, in your own words, exactly which single implementation detail flips a counting recurrence from "combinations" to "permutations," and why the recurrence body itself cannot tell you which one you're computing.

---

## Real-World Challenge — Minimum-Denomination Ledger Settlement

You operate an internal ledger system where account adjustments are only ever applied in fixed denominations (e.g. adjustment "packets" of size 1, 5, 10, 25, 100 currency units — analogous to coin denominations, but here representing pre-approved adjustment sizes a finance-ops tool is allowed to apply automatically). A support engineer needs to zero out a customer's exact outstanding balance using the **fewest possible adjustment packets** (fewer line items means a cleaner audit trail and less manual review).

**Task:**
1. Implement a function that, given the list of allowed denominations (each usable any number of times) and an exact target balance, returns the minimum number of packets needed, or reports the balance cannot be settled exactly with the available denominations.
2. Extend it to also **reconstruct** one optimal combination of packets (not just the count) — you will need to track, alongside `dp[w]`, which denomination achieved that minimum at each `w`, then walk that trail backward from `dp[amount]` to `dp[0]`.
3. Discuss: your finance-ops team wants to add a new constraint — "no more than 3 packets of any single denomination in one settlement" (an auditing/fraud-prevention rule). Does this still fit the Unbounded Knapsack shape as described in this module, or does it become a **bounded** knapsack problem instead? Explain exactly what changes in the recurrence and why (see the README's Similar Patterns section on Bounded Knapsack for a starting point).

---

## Bonus Challenge — Word Break

**LeetCode 139 — Word Break.**

Given a string `s` and a dictionary of strings `wordDict` (each dictionary word may be reused any number of times), return whether `s` can be segmented into a space-separated sequence of one or more dictionary words.

**Task:** implement it as a boolean 1D dp table, `dp[i]` = "can the prefix `s[0..i)` be fully segmented using words from `wordDict`, each reusable any number of times." Forward-fill `i` from `1` to `s.length()`, and for each `i`, check every dictionary word (or, as an optimization, every valid ending position `j < i` where `s[j..i)` is itself in the dictionary and `dp[j]` is true).

**Then, generalize in writing (no code required):** Word Break has no numeric "weight" or "capacity" in the traditional sense — the "capacity axis" is a string length/prefix position, and the "items" are dictionary words of varying lengths, reusable without limit. Explain, in your own words, why this still fits the Unbounded Knapsack shape as defined in this module's Architecture section, despite looking, on the surface, nothing like coins or rod-cutting. Is the forward-fill-over-a-length-axis detail still the mechanism that permits reuse here? Justify your answer by describing what `dp[i - len(word)]` being already-finalized means concretely for this specific problem.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
