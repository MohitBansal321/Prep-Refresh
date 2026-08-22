# Greedy — Recognition Diagram

Use this flowchart when you are staring at a new optimization problem and trying to decide whether Greedy is the right tool, whether the problem actually wants Dynamic Programming, or whether it is merely a sorting problem with a greedy-shaped solution. It is the expanded version of the diagram in the [README](../README.md).

```mermaid
flowchart TD
    Start([Optimization problem:<br/>maximize / minimize over items]) --> Q1{Must I compare BOTH<br/>'take it' and 'skip it'<br/>to decide each step?}

    Q1 -- "Yes / not sure" --> SubQ{Do subproblems<br/>overlap enough to memoize?}
    SubQ -- Yes --> DP[["Use Dynamic Programming<br/>O(n * W) time, O(W) space.<br/>The recurrence is its own proof --<br/>correct for ALL inputs."]]
    SubQ -- No, need all solutions / ordering --> Backtrack[["Use Backtracking<br/>(prune invalid branches;<br/>exponential worst case)"]]

    Q1 -- "No -- one branch is<br/>provably always safe" --> Q2{Can I name a SORT KEY<br/>(or an O(1) running frontier)<br/>making each local choice<br/>decidable from one scalar?}

    Q2 -- No --> DP

    Q2 -- Yes --> Q3{Can I state an EXCHANGE ARGUMENT<br/>for that key in one sentence?<br/>'Swapping my choice into any optimal<br/>solution leaves it valid and no worse.'}

    Q3 -- "No / can't quite say it" --> Danger[["STOP. Write the DP instead.<br/>An unproven greedy rule fails SILENTLY --<br/>it terminates and returns a plausible,<br/>wrong answer that passes your samples."]]

    Q3 -- Yes --> Q4{Does the input actually need<br/>REORDERING before local choices<br/>become safe?}
    Q4 -- "Yes -- order matters to the decision" --> SortGreedy[["SORT-BASED GREEDY<br/>sort by the proven key,<br/>one pass, O(1) state<br/>e.g. interval scheduling,<br/>assign cookies, boats<br/>O(n log n) time"]]
    Q4 -- "No -- a running max/deficit<br/>over original order suffices" --> FrontierGreedy[["FRONTIER GREEDY (no sort)<br/>one pass extending a frontier<br/>or deficit: farthest-reachable,<br/>tank level, last occurrence<br/>e.g. Jump Game, Gas Station<br/>O(n) time"]]

    Q1b{"Is there really any CHOICE at all --<br/>or is one fixed order forced?"} --> Forced[["Just sort by the required key<br/>and process -- plain SORTING,<br/>not greedy; no proof needed<br/>because no commitment is made"]]
    Start -.->|"sanity fork"| Q1b
```

## How to read it

Start at the top and answer each diamond honestly — but in this module the **first fork is the load-bearing one**, so dwell on it. "Must I compare both branches?" is not a question about your coding preference; it is a question about the problem's structure. If you catch yourself sketching `dp[i] = max(take, skip)` or wanting two recursive calls whose results you compare, you have already answered "yes," and the honest exit is Dynamic Programming on the right. The exchange argument is precisely the artifact that lets you answer "no": it is a *proof* that one branch is never worse, which is what makes skipping the comparison legal. No sentence-length argument, no greedy — that gate exists because this pattern is unique in failing silently.

The **second fork** separates "no safe local choice exists" (exit to DP) from "a key exists." Note that the key does not have to be a sort: the diagram deliberately shows two green exits. Sort-based greedy (interval scheduling, Assign Cookies) reorders items so each commitment becomes locally decidable; frontier greedy (Jump Game, Gas Station, Partition Labels) keeps the original order and tracks a single scalar summary — a farthest index, a fuel tank, a deficit — proving along the way that nothing else about the past matters. If you believe you need neither a key nor a scalar state, then either the problem has no real choice (the dashed sanity fork at the bottom: plain sorting, no commitment, no proof needed) or your state is hiding more than one scalar, which means the choice is not actually local and DP is calling.

Finally, treat the red STOP box as a routing instruction, not a warning label. Failing to produce the exchange argument on the spot is *information* — most wrong greedy rules die exactly at that step, and the {1, 3, 4} coin-change counter-example worked through in the [README](../README.md) and [trace-diagram.md](trace-diagram.md) is what proceeding anyway looks like. When the argument will not come, write the DP first (correct beats fast), then differential-test any candidate greedy rule against it on small random inputs.
