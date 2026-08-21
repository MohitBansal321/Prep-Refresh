# 0/1 Knapsack — Flow Diagram

This traces the control flow of the table fill — the outer items loop, the inner capacity loop, and the skip-or-take decision made at every single cell — for both the 2D formulation and its 1D space-optimized counterpart, exactly as implemented by `knapsack01` and `knapsack01Optimized` in [code.cpp](../code.cpp). See [trace-diagram.md](trace-diagram.md) for the same algorithm with concrete numbers filled in.

```mermaid
flowchart TD
    Start([knapsack01: weights, values, capacity]) --> Alloc["Allocate dp with n+1 rows<br/>and capacity+1 columns,<br/>zero-initialized"]

    Alloc --> Base["Base row i=0 and base column w=0<br/>are ALREADY correct at zero:<br/>no items -> no value,<br/>no capacity -> nothing fits.<br/>The +1 row/column are the<br/>base case, not padding."]

    Base --> OuterInit["i = 1<br/>-- OUTER loop is ITEMS --"]

    OuterInit --> OuterTest{i <= n?}

    OuterTest -- No --> Answer(["Return dp[n][capacity]<br/>-- bottom-right corner --<br/>best value using ANY subset<br/>of all n items within capacity"])

    OuterTest -- Yes --> InnerInit["w = 0<br/>-- INNER loop is CAPACITY --<br/>item under consideration is<br/>index i-1 in weights/values<br/>(the row-to-item index shift)"]

    InnerInit --> InnerTest{w <= capacity?}

    InnerTest -- No --> NextItem["Row i is now COMPLETE and<br/>FROZEN. ++i"]
    NextItem --> OuterTest

    InnerTest -- Yes --> Skip["CHOICE 1 -- skip item i-1:<br/>dp[i][w] = dp[i-1][w]<br/>always legal, always available"]

    Skip --> Fits{weights[i-1] <= w ?<br/>-- does the item even fit<br/>in a budget of w? --}

    Fits -- "No -- too heavy" --> Forced["FORCED SKIP.<br/>dp[i][w] keeps dp[i-1][w].<br/>This is the whole reason the<br/>recurrence has two cases."]

    Fits -- "Yes -- it fits" --> Take["CHOICE 2 -- take item i-1:<br/>dp[i-1][w - weights[i-1]] + values[i-1]<br/>pay the weight, gain the value,<br/>ask the SMALLER subproblem<br/>for the rest"]

    Take --> Better["dp[i][w] = max(skip, take)<br/>Both reads came from row i-1.<br/>NEVER from row i -- that one<br/>fact is what forbids reusing<br/>the item within its own row."]

    Better --> Advance["++w"]
    Forced --> Advance
    Advance --> InnerTest

    Answer --> Optimize{Do you need the CHOSEN<br/>ITEMS, or only the value?}

    Optimize -- "The chosen items" --> Keep2D[["Keep the full 2D table.<br/>Walk backward from dp[n][capacity]:<br/>if dp[i][w] != dp[i-1][w] then<br/>item i-1 was taken -- subtract its<br/>weight and continue at row i-1.<br/>The 1D version CANNOT do this."]]

    Optimize -- "Only the value" --> To1D["Collapse to ONE row of<br/>length capacity+1.<br/>Row i only ever reads row i-1,<br/>so rows i-2 and older are dead."]

    To1D --> Rev["For each item, sweep<br/>w from capacity DOWN TO weights[i]:<br/>dp[w] = max(dp[w], dp[w-weights[i]] + values[i])"]

    Rev --> Why{Why must w DESCEND?}

    Why -- "Descending" --> Correct["dp[w - weights[i]] has NOT yet been<br/>touched by this item's pass, so it<br/>still holds the row-(i-1) value.<br/>The item contributes AT MOST ONCE.<br/>-> correct 0/1 semantics"]

    Why -- "Ascending" --> Wrong["dp[w - weights[i]] was ALREADY<br/>overwritten by this item's pass,<br/>so it may already include this<br/>item -- adding it a second time.<br/>-> silently solves UNBOUNDED knapsack"]

    Correct --> Done([Return dp[capacity]])
```

## How to read it

The diagram splits into three regions, and each one answers a different question.

**The top region is the fill order, and its single load-bearing claim is the box labelled "Row `i` is now COMPLETE and FROZEN."** Items are the outer loop and capacity is the inner loop, which means that by the time any cell in row `i` is computed, the *entire* row `i - 1` already holds its final values — it was finished in the previous outer iteration and is never written again. That is what makes every read in the recurrence safe. Notice that the direction you sweep `w` within a row is genuinely *irrelevant* in the 2D version, because every read targets the frozen row above; the whole forward-versus-backward controversy only exists once you collapse the table. If you ever find yourself unsure which direction the 1D loop needs, the fastest way to re-derive it is to come back to this region and ask "which cells still hold row `i - 1` values?"

**The middle region is the skip-or-take decision, and the `weights[i-1] <= w` diamond is why the recurrence has two cases rather than one.** "Skip" is unconditional — leaving an item out is always legal, so `dp[i][w] = dp[i-1][w]` runs first as a floor. "Take" is conditional, and when the item does not fit, the skip branch is not merely *preferred*, it is the only branch that exists; attempting the take branch anyway would index `dp[i-1][w - weights[i-1]]` with a negative column. The `max` at the bottom of this region is the only place the two candidates ever meet, and it is the operator that the other flavors of this pattern swap out: OR for reachability ([problems/01](../problems/01-partition-equal-subset-sum.cpp)), `+=` for counting ([problems/03](../problems/03-target-sum.cpp)). The loop structure above it does not change at all.

The comment on the `max` box — **both reads come from row `i - 1`, never from row `i`** — is the mechanical enforcement of the 0/1 constraint, and it is worth stating as a positive rule rather than a prohibition. Row `i - 1` describes a world in which item `i - 1` does not exist yet. So when the take branch adds `values[i-1]` to a row-`i-1` cell, it is adding the item to a solution that provably does not already contain it. Change that one read to `dp[i][w - weights[i-1]]` and you are adding the item to a solution that *may already contain it* — which is precisely the definition of Unbounded Knapsack, and precisely the recurrence the [unbounded-knapsack](../../unbounded-knapsack/) module uses on purpose.

**The bottom region is the space optimization, and it opens with a fork that is easy to get wrong in the other direction.** Before collapsing to 1D, ask what the caller actually needs. The 1D version is strictly cheaper in memory and identical in time, but it *destroys the information required to reconstruct which items were chosen* — recovering the subset means walking backward through the full table comparing `dp[i][w]` against `dp[i-1][w]`, and after the collapse those intermediate rows no longer exist. In an interview, "can you reduce the space?" and "can you tell me *which* items?" are follow-ups that pull in opposite directions, and saying so explicitly is a better answer than picking one silently.

The final `Why must w DESCEND?` diamond is the same fact stated from the 1D side, and the two outcome boxes are worth memorizing as a matched pair because the failure is *silent*: the ascending version compiles, runs, and returns a number that is a perfectly correct answer to a different problem. With one item of weight 1 and value 10 against capacity 3, ascending computes `dp[1] = 10`, then reads that fresh `dp[1]` to get `dp[2] = 20`, then `dp[3] = 30` — the single item counted three times. Descending reads `dp[w - weight]` before this item's pass can reach it, capping the contribution at one. Every worked problem in [problems/](../problems/) carries a test whose only purpose is to fail if that direction is flipped, and [04-ones-and-zeroes.cpp](../problems/04-ones-and-zeroes.cpp) generalizes the rule to its correct form: it is not "the inner loop descends," it is **every dimension that indexes a capacity descends.**
