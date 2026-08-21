# 0/1 Knapsack — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether 0/1 Knapsack is the right tool, or whether the problem actually wants Unbounded Knapsack, plain Subsets, or a greedy pass instead. The hard part of this pattern is almost never the recurrence — it is noticing that a problem about stones, signs, or binary strings *is* a knapsack at all.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Is there a set of discrete<br/>items, and a NUMERIC BUDGET<br/>that choosing an item<br/>CONSUMES?<br/>-- weight, cost, sum, count,<br/>time, capacity, digits --}

    Q1 -- "No budget being consumed --<br/>I just need every combination" --> Subsets[["Use Subsets<br/>(../../recursion-backtracking-patterns/subsets/)<br/>full 2^n enumeration;<br/>nothing to optimize, so no<br/>overlapping-subproblem reuse<br/>is available"]]

    Q1 -- "Yes -- a budget is consumed" --> Q2{Can a single item be<br/>selected MORE THAN ONCE?}

    Q2 -- "Unlimited reuse<br/>-- coin change, rod cutting,<br/>'as many as you like'" --> Unbounded[["Use Unbounded Knapsack<br/>(../unbounded-knapsack/)<br/>recurrence reads dp[i][...],<br/>the SAME item's row;<br/>1D sweep goes FORWARD"]]

    Q2 -- "Finite limit > 1<br/>-- 'you have 3 of item A'" --> Bounded[["Bounded Knapsack<br/>duplicate each item 'limit'<br/>times and run 0/1, or use<br/>the binary-splitting trick"]]

    Q2 -- "At most ONCE<br/>each item is whole-or-absent" --> Q3{Are the items DIVISIBLE?<br/>-- can I take 40% of one<br/>and get 40% of its value?}

    Q3 -- "Yes, divisible<br/>-- rice, oil, gold dust" --> Greedy[["Use greedy by value/weight<br/>ratio -- PROVABLY optimal for<br/>the fractional problem via the<br/>exchange argument. Do NOT<br/>use it for the 0/1 problem."]]

    Q3 -- "No -- indivisible" --> Q4{Is the budget's numeric VALUE<br/>small enough to be an array<br/>index?<br/>-- roughly up to a few million}

    Q4 -- "No -- astronomically large<br/>capacity, or real-valued weights" --> Other[["Pseudo-polynomial wall.<br/>Meet-in-the-middle for small n,<br/>scaling/rounding for floats,<br/>or an approximation scheme.<br/>NOT greedy -- it has no bound here."]]

    Q4 -- "Yes -- it fits in a table" --> Q5{What is the question<br/>actually asking for?}

    Q5 -- "The BEST total value" --> Max["0/1 Knapsack, MAX flavor<br/>dp[w] = max(dp[w], dp[w-wt] + val)<br/>-> code.cpp knapsack01"]

    Q5 -- "Is an exact total REACHABLE?<br/>-- equal split, exact target" --> Feasible["0/1 Knapsack, FEASIBILITY flavor<br/>value == weight; max becomes OR<br/>dp[0] = true<br/>-> problems/01-partition-equal-subset-sum.cpp"]

    Q5 -- "The SMALLEST possible gap<br/>between two groups" --> MinDiff["0/1 Knapsack, MINIMIZE-DIFF flavor<br/>maximize the pile <= total/2,<br/>answer = total - 2*pile<br/>-> problems/02-last-stone-weight-ii.cpp"]

    Q5 -- "HOW MANY ways / subsets" --> Count["0/1 Knapsack, COUNTING flavor<br/>max becomes +=; dp[0] = 1<br/>-> problems/03-target-sum.cpp"]

    Q5 -- "The LARGEST SUBSET SIZE<br/>under two or more budgets" --> MultiAxis["0/1 Knapsack, MULTI-AXIS flavor<br/>value == 1; capacity is a tuple;<br/>sweep EVERY axis backward<br/>-> problems/04-ones-and-zeroes.cpp"]

    Max --> Done([0/1 Knapsack applies -- sweep capacity BACKWARD in 1D])
    Feasible --> Done
    MinDiff --> Done
    Count --> Done
    MultiAxis --> Done
```

## How to read it

Start at the top and answer each diamond honestly, because the **first fork is the one people skip**. Before asking anything about items or values, ask whether choosing an item *consumes a finite numeric budget*. If nothing is being consumed — the problem just wants every combination listed — you are in the Subsets pattern, not here. That distinction matters more than it looks: 0/1 Knapsack's entire `2^n → n·W` win exists *because* the question narrowed from "show me all the answers" to "give me one number." Enumeration cannot exploit overlapping subproblems, since two different subsets are genuinely different outputs even when they share a `(items considered, budget left)` state. A single number can be reused; a list of subsets cannot.

The **second fork — reuse rules — is the one that silently produces wrong answers** rather than obviously wrong ones. "At most once" versus "unlimited" is a one-symbol difference in the recurrence (`dp[i-1][w-wt]` versus `dp[i][w-wt]`) and a one-word difference in the 1D loop (`--w` versus `++w`), so a mistake here compiles, runs, and returns a plausible number that answers *the other problem*. Read the statement for the phrase that settles it — "each item may be used at most once," "you may reuse coins," "you have exactly 3 of these" — and write the sweep direction down before writing the loop.

The **third fork is the greedy trap**, and it is worth pausing on because it is the most commonly misunderstood fact in this family. Greedy by value-to-weight ratio is not a heuristic that "usually works" for knapsack — it is *provably optimal* for the fractional version and *structurally broken* for the 0/1 version. The exchange argument that proves it correct requires being able to trade an infinitesimal amount of a worse-ratio item for a better-ratio one; when items are whole-or-absent, that trade is unavailable, and greedy's early commitment can permanently block a better combination (the `W = 50` counterexample in the [README](../README.md) loses 27% of the optimal value on a three-item input). Divisibility is the *only* thing separating those two worlds, so it deserves its own question rather than being assumed.

The **fourth fork is the pseudo-polynomial wall**, and it is a real engineering decision, not a footnote. `O(n · capacity)` is polynomial in the capacity's *magnitude*, not in the number of bits needed to write it down — so a capacity of one billion (a perfectly ordinary 32-bit integer, about 30 bits) demands a billion columns per row. When you hit that wall, greedy is *not* the fallback: it carries no approximation bound for 0/1 Knapsack. Meet-in-the-middle (viable when `n` is small even though the capacity is huge) or an explicit approximation scheme is.

The **fifth fork picks the flavor**, and all five flavors share one table, one fill order, and one backward sweep — they differ only in what a cell holds and how two cells combine. `max` for best-value, OR for reachability, `+=` for counting, and a constant value of 1 when the "value" is really just "how many items did I fit." Two tricks make the whole family collapse into one implementation: setting **`value == weight`** (which turns a maximization recurrence into a subset-sum question, and is what makes flavors 2 and 3 possible at all), and noticing that **minimizing a gap between two groups is maximizing one group under a `total/2` cap** (flavor 3). Once you see those, the five boxes at the bottom of this diagram are the same twelve lines of code with one operator swapped.

Whichever flavor you land in, the exit condition is the same and is worth repeating out loud every time: **in the 1D form, capacity descends.** That single rule is what enforces "each item at most once," and it is the one detail every file in [problems/](../problems/) has a dedicated test for.
