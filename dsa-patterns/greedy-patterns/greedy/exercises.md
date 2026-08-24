# Greedy — Exercises

Work through these in order. The goal is to build two reflexes: (1) before writing any code, stating the **sort key and the exchange argument** for your candidate greedy rule out loud — and treating an inability to state the argument as a stop signal, not a formality; (2) actively hunting for the **counter-example that would break a plausible-but-wrong key**, because in this pattern a wrong rule passes every sample you feed it.

> Rule of thumb for every exercise: before writing a single line, ask "what is my sort key, and what are the two keys I am rejecting?" and "can I swap my choice into some optimal solution without making it worse?" If you cannot answer both questions, you are not ready to write the loop yet.

---

## Easy — Assign Cookies

**LeetCode 455 — Assign Cookies.**

You have children each with a greed factor `g[i]` (minimum cookie size that satisfies them) and cookies each with a size `s[j]`. A child is content if they receive a cookie of size at least their greed factor. Maximize the number of content children. Each cookie goes to at most one child.

**Constraints to notice:** both arrays are unsorted; sizes and greed factors can be equal; a cookie bigger than needed is "wasted" satisfaction capacity but costs you nothing if no smaller child remains.

**Task:** solve with two sorted arrays walked by two pointers, committing irrevocably at each step. Then answer: which of the two plausible assignment directions did you pick — smallest sufficient cookie to the least greedy child, or largest cookie to the greediest child? State the exchange argument for yours, and try to construct a three-element input where the *other* direction also gives the optimum (does it always? what does that tell you about ties?).

**Think about:** why does giving the *smallest* sufficient cookie matter rather than any sufficient cookie? What resource does it conserve for future decisions?

---

## Medium — Minimum Number of Arrows to Burst Balloons

**LeetCode 452 — Minimum Number of Arrows to Burst Balloons.**

Balloons are horizontal intervals `[x_start, x_end]` at various heights. An arrow shot vertically at position `x` bursts every balloon whose interval contains `x`. Find the minimum number of arrows to burst all balloons.

**Constraints to notice:** this is interval scheduling wearing a costume — but inverted. Activity Selection maximizes intervals kept; here you minimize arrows, i.e. maximize the number of balloons per arrow. Touching endpoints (`[1,2]` and `[2,8]`) count as overlapping per LeetCode's convention — check the statement's exact wording, because `>=` vs `>` changes the answer.

**Task:** sort by one endpoint, sweep once, fire an arrow only when forced. Then answer: does sorting by **start** work here, unlike in Activity Selection? Trace both keys on `{[1,10],[2,3],[4,5],[6,7]}` and explain why the answer comes out the same or different — the key follows the *goal*, not the data type.

**Think about:** the running state is one scalar. What exactly is it, and what single fact about the past does it compress?

---

## Hard — Candy

**LeetCode 135 — Candy.**

`n` children stand in a line, each with a rating. Every child gets at least 1 candy; every child with a rating higher than an adjacent neighbor must get more candies than that neighbor. Minimize total candies distributed.

**Constraints to notice:** the constraint is on **adjacent pairs only**, but a change can cascade down a whole run (ratings `[1,2,3,4,5]` forces 1+2+3+4+5 candies); equal adjacent ratings impose **no** constraint; `n` can be large enough that re-scanning naively per element is too slow.

**Task:** solve in two linear passes (`O(n)` time, `O(n)` space): one left-to-right pass handling only left-neighbor constraints, then one right-to-left pass handling only right-neighbor constraints, taking the max where they interact. Then answer: why is a single pass insufficient? Construct the input where a one-directional-only fix produces a violation on the other side.

**Think about:** this greedy has no sort at all. Where is the "irrevocable local commitment" here, and why do two opposite-direction sweeps together cover all constraints without ever needing to revisit a decision within a pass?

---

## Real-World Challenge — Build-Farm Job Scheduling Under a Machine Limit

Your CI farm has `k` identical build machines and a backlog of jobs, each with a known duration. Jobs cannot be split or migrated once started. You want to minimize makespan (the time when the last job finishes).

**Task:**
1. Implement **Longest Processing Time first** (LPT): sort jobs descending by duration, then assign each job — irrevocably — to whichever machine currently has the least total load (a min-heap of loads). This is greedy: one commitment per job, `O(1)` state per machine.
2. Write a brute-force oracle (try every assignment for small inputs) and differential-test LPT against it on a few hundred random cases with `n <= 12`, `k <= 3`. Record whether LPT ever loses.
3. Now answer honestly, in writing: LPT is provably within `4/3` of optimal but **not always optimal** — find (or let your differential test find) the smallest input where it loses, and identify which greedy-choice-property step fails there.
4. Discuss: given the ratio bound, when would you still ship LPT in production (hint: bin packing / multiprocessor scheduling is NP-hard, so the alternative is exponential), and how would you phrase the guarantee in a design doc so nobody mistakes it for an optimum?

---

## Bonus Challenge — Partition Labels

**LeetCode 763 — Partition Labels.**

Given a string, partition it into as many parts as possible so that each letter appears in at most one part. Return the list of part sizes.

**Task:** precompute each character's last occurrence, then sweep once maintaining a running frontier — the farthest last-occurrence among characters seen in the current part. When the cursor reaches the frontier, cut a part. This is structurally the same frontier greedy as Jump Game: compare the two side by side and write down what plays the role of `nums[i] + i`.

**Then, generalize in writing (no code required):** the module's [README](README.md) says greedy has four participants — sort key, running state, decision rule, proof obligation. For Partition Labels, name all four explicitly. Which participant replaces the sort key here, and why is the precomputation step doing the same conceptual work that sorting does elsewhere?

---

*Solutions are intentionally omitted, on purpose — that is not a gap to route around. If stuck, ask for a hint or a review of your attempt, not the answer.*
