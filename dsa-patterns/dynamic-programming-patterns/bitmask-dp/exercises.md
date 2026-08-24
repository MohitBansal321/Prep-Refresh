# Bitmask DP — Exercises

Work through these in order. The goal is to build two reflexes: (1) checking, *before anything else*, that the problem really needs subset identity (`n <= ~20` and "which items" matters, not just "how many"), and (2) writing down the state definition — what exactly does `dp[mask]` (or `dp[mask][extra]`) mean — before touching any transition code.

> Rule of thumb for every exercise: first answer "if two different choices have used the same number of items so far, can their futures differ?" If yes, you need the mask. Then answer "do I also need to know the most recent choice?" — that decides between `dp[mask]` and `dp[mask][last]`. Only then start coding.

---

## Easy — Counting Bits

**LeetCode 338 — Counting Bits.**

Given an integer `n`, return an array `ans` of length `n + 1` such that `ans[i]` is the number of 1-bits in the binary representation of `i`, for every `i` in `[0, n]`.

**Constraints to notice:** this is a warm-up on the bit-level vocabulary the whole pattern rests on: `x & 1` extracts the lowest bit, `x >> 1` shifts it out, and `(mask >> i) & 1` reads bit `i`.

**Task:** solve it with a one-pass DP relation (no per-number bit counting loop) — find how `ans[i]` relates to an already-computed smaller index. You should be able to write the recurrence in one line.

**Think about:** your recurrence computes `__builtin_popcount` for every mask up to `2^n`. In bitmask DP proper, why can you usually *not* precompute popcounts once and forget about them cheaply — or actually, can you? What would the table size be?

---

## Medium — Shortest Path Visiting All Nodes

**LeetCode 847 — Shortest Path Visiting All Nodes.**

You have an undirected connected graph of `n` nodes (`n <= 12`) labeled `0..n-1`. Return the length of the shortest path that visits every node at least once. You may start and stop at any node, revisit nodes, and reuse edges.

**Constraints to notice:** revisiting is allowed, so this is *not* TSP — but the state still needs to know **which set of nodes has been visited so far**, because whether a node still needs visiting depends on exactly which ones you have seen, not how many.

**Task:** run BFS where each queue state is a pair `(node, mask)` instead of just `node` — the same graph, but the search space is expanded by the `2^n` masks. The first time you dequeue any state whose mask is full, its distance is the answer.

**Think about:** why must the `visited`/distance structure be indexed by both node and mask? What goes wrong if you only mark `(node)` as visited? And why is plain BFS (rather than DP-over-masks-in-order) the natural fit here — hint: think about which property of BFS guarantees optimality, and whether setting a bit always moves you "forward" along an edge.

---

## Hard — Smallest Sufficient Team

**LeetCode 1125 — Smallest Sufficient Team.**

You are given a list of required skills (up to 60 distinct strings) and a list of people, each owning some subset of those skills. Return the smallest team (list of people indices) such that together they cover every required skill. There may be multiple valid answers; return any one.

**Constraints to notice:** there are up to 60 *skills* but up to only 16 *people* — so the bitmask must go over **people**, not skills (map each skill to a bit index over `[0, 60)`, then each person becomes an integer skill-mask). The DP state is "which people are already hired"; the value being minimized is team size; the target is full skill coverage.

**Task:** memoize over `dp[mask]` = minimum additional people needed given the union of skills covered is `mask`, trying each not-yet-hired person as a transition `mask -> mask | person.skills`. Reconstruct the actual team by storing (or re-deriving during a second pass) which person achieved the optimum at each mask.

**Think about:** why is the direction reversed compared to most problems in this pattern — i.e., why do bits here represent *resources chosen* rather than *items consumed*? Does the increasing-numeric-order argument for iteration still hold when a transition ORs in a person's skill mask?

---

## Real-World Challenge — Minimum Test-Matrix Coverage for Feature Flags

Your product ships behind feature flags, and QA needs regression coverage for combinations: there are up to 20 experimental flags, and certain *pairs* of flags interact badly (a known list of conflicting pairs). Each nightly test configuration is a subset of enabled flags; a conflict pair `(A, B)` is "covered" if some single configuration enables both A and B simultaneously (that is the only way the interaction can manifest). Provisioning each extra nightly build costs real CI time, so you want the minimum number of configurations whose enabled-flag sets cover every conflicting pair.

**Task:**
1. Model each conflicting pair as requiring coverage; design a top-down search over "which configurations have been chosen so far" and explain why a naive greedy ("always pick the configuration covering the most uncovered pairs") can be off by more than a constant factor.
2. Implement it with memoization where the state includes the set of already-covered conflict pairs (as a mask over pairs — note the mask dimension may exceed `2^20`; think about which quantity should get the mask treatment: chosen builds or covered pairs).
3. Discuss: if the number of conflicting pairs grows to 200 while the number of *flags* stays at 20, does your formulation still fit in memory? What would you re-key the mask onto, and what changes in the transition?

---

## Bonus Challenge — Connect Two Groups of Points

**LeetCode 1595 — Minimum Cost to Connect Two Groups of Points.**

You have `sizeOne` points on the left and `sizeTwo` on the right (`sizeOne, sizeTwo <= 30` combined constraints allow `sizeTwo <= 12`); connecting left point `i` to right point `j` costs `cost[i][j]`. Every point must be connected to at least one point on the other side. Return the minimum total cost.

**Task:** bitmask over the *right* group's points: process left points one at a time, and decide which right points each new left point connects to — including redundantly re-connecting right points that were already covered, because sometimes paying twice is cheaper than the alternative. Memoize on `(leftIndex, maskOfCoveredRightPoints)`.

**Then answer in writing:** this problem allows transitions that set bits that are **already set**, breaking the "each item used exactly once" shape of the rest of the pattern. Why does the memoized recursion survive that anyway, and what invariant guarantees termination despite non-monotone-looking transitions?

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
