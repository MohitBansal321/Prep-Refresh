# Kadane's Algorithm — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether Kadane's Algorithm is the right tool, or whether the problem actually wants Sliding Window or Dynamic Programming instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Does it ask for the<br/>best/max/min SUM (or a value<br/>reducible to a sum) of a<br/>CONTIGUOUS run of elements?}

    Q1 -- "No -- elements can be<br/>skipped (a subsequence),<br/>order preserved but not adjacency" --> DP[["Use Dynamic Programming<br/>(e.g. Longest Increasing Subsequence,<br/>Longest Common Subsequence)<br/>-- non-contiguous is out of Kadane's scope"]]

    Q1 -- Yes, contiguous --> Q2{Is a window SIZE given<br/>or bounded, e.g. 'exactly k'<br/>or 'at most k distinct'?}

    Q2 -- "Yes, fixed/bounded size" --> SlidingWindow[["Use Sliding Window<br/>(grow/shrink a window enforcing<br/>the size/property constraint)"]]

    Q2 -- "No window-size given --<br/>just 'best contiguous sum'" --> Q3{Is the array/data<br/>structure 1D or 2D?}

    Q3 -- 2D, e.g. "max sum sub-matrix" --> DPGrid[["Use Kadane's AS A SUBROUTINE<br/>inside DP on Grids:<br/>fix row boundaries, collapse<br/>columns, run 1D Kadane's"]]

    Q3 -- 1D array --> Q4{Is it SUM-based, or does<br/>it need a twist -- PRODUCT,<br/>CIRCULAR wraparound, or a<br/>disguised delta reformulation?}

    Q4 -- "Plain sum, no twist" --> Vanilla["Use Kadane's Algorithm<br/>(VANILLA)<br/>current_sum = max(nums[i], current_sum + nums[i])<br/>best_sum updated every step"]

    Q4 -- "Product instead of sum" --> Product["Use Kadane's Algorithm<br/>(PRODUCT VARIANT)<br/>track running MAX and MIN,<br/>swap them when nums[i] is negative"]

    Q4 -- "Circular array,<br/>subarray may wrap around" --> Circular["Use Kadane's Algorithm<br/>(CIRCULAR VARIANT)<br/>max(vanilla max,<br/>total_sum - vanilla min),<br/>with an all-negative guard"]

    Q4 -- "Disguised as 'best single<br/>buy/sell day' or similar --<br/>reduces to a delta array" --> Delta["Reformulate as day-over-day<br/>deltas, then apply VANILLA<br/>Kadane's to the deltas"]

    Vanilla --> Done([Kadane's applies])
    Product --> Done
    Circular --> Done
    Delta --> Done
```

## How to read it

Start at the top and answer each diamond honestly — the first and most important fork is **contiguous vs. non-contiguous**. Kadane's Algorithm has no mechanism at all for "skip some elements but keep relative order" problems; that entire class (Longest Increasing Subsequence and its relatives) needs a genuinely different DP recurrence, not a Kadane's adaptation. Getting this fork wrong is the single most common way engineers misapply this pattern — the word "subarray" (contiguous) versus "subsequence" (order-preserving, but gaps allowed) in a problem statement is the tell.

The **second fork** is whether a window size is given or bounded. If the problem says "exactly k consecutive elements" or "smallest window with sum at least target," that is Sliding Window's territory — it enforces or searches over an explicit size/property constraint on the window, something Kadane's has no concept of at all. Kadane's only applies when the question is purely "best contiguous sum, no size given."

The **final fork** separates vanilla Kadane's from its three common disguises, all covered by this module's [problems/](../problems/): a straightforward sum-based question needs no adaptation at all; a **product** question needs a running minimum tracked alongside the running maximum (because a negative number can flip the sign of a large-magnitude negative running product into the new best positive one); a **circular** array needs the "total sum minus the minimum non-wrapping subarray" algebraic trick, with a guard for the all-negative degenerate case; and a question that does not even mention "subarray sum" explicitly (like "best day to buy and sell a stock") often reduces to vanilla Kadane's once you reformulate the input as day-over-day deltas. Recognizing which of these four shapes you are looking at — not just recognizing "this is a Kadane's problem" in general — is what separates a candidate who has memorized one template from one who understands the underlying argument well enough to adapt it.
