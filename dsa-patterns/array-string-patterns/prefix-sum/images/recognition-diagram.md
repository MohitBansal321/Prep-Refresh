# Prefix Sum — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether Prefix Sum is the right tool, or whether the problem actually wants a Segment Tree/Fenwick Tree, or Sliding Window instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Does the problem ask for the<br/>sum/count over a range i..j<br/>of a FIXED array, possibly<br/>many times?}

    Q1 -- No --> Q1b{Is it instead a CONTIGUOUS<br/>window with a running-property<br/>constraint, found via a single<br/>scan (e.g. longest substring,<br/>smallest subarray >= target)?}
    Q1b -- Yes --> SlidingWindow[["Use Sliding Window<br/>(a growing/shrinking window during<br/>one pass, not after-the-fact range queries)"]]
    Q1b -- No --> Rethink[Re-check the problem shape --<br/>neither pattern may apply directly]

    Q1 -- Yes --> Q2{Does the array get<br/>UPDATED (point/range writes)<br/>between range queries?}

    Q2 -- "Yes, frequently,<br/>interleaved with queries" --> SegTree[["Use Segment Tree or<br/>Fenwick Tree (Binary Indexed Tree)<br/>O(log n) update, O(log n) query"]]

    Q2 -- "No updates, or updates<br/>are rare/batched" --> Q3{Is the aggregate you need<br/>INVERTIBLE (sum, product, XOR) --<br/>i.e. can you 'subtract off'<br/>an unwanted prefix?}

    Q3 -- "No -- e.g. minimum/maximum" --> NotInvertible[["Prefix Sum does NOT apply directly.<br/>Use a Sparse Table (static data)<br/>or a Segment Tree (if updates needed)"]]

    Q3 -- Yes --> Q4{Do you need MANY queries,<br/>or just one or two total?}

    Q4 -- "Just one or two" --> NaiveOK[["A direct O(range length) scan is fine --<br/>building a prefix array does not<br/>pay for itself for so few queries"]]

    Q4 -- Many --> PrefixSum["Use Prefix Sum<br/>Build P once: O(n) (or O(rows*cols) in 2D)<br/>Query: O(1) via P[j+1] - P[i]<br/>(four-corner formula in 2D)"]

    PrefixSum --> Done([Prefix Sum applies])
```

## How to read it

Start at the top and answer each diamond honestly. The **first fork** separates "range query over a fixed array" problems from "contiguous window found during a single scan" problems — these look similar (both involve sums over subarrays) but answer different question shapes, and confusing them leads to reaching for the wrong template. If you are not scanning once and tracking a moving window, but instead need to answer arbitrary, independently-chosen ranges after the array is already known, you are in Prefix Sum/Segment Tree territory, not Sliding Window's.

The **second fork** is the one people most often skip: does the array actually stay still? Prefix Sum's entire value proposition — O(1) queries — depends on paying the O(n) build cost exactly once. If updates and queries are interleaved frequently, that O(n)-per-update cost (a full or partial rebuild) dominates, and a Segment Tree or Fenwick Tree's O(log n) update/query pair is the better trade even though its per-query cost is technically worse than Prefix Sum's O(1).

The **third fork** catches a subtler mistake: the `P[j+1] - P[i]` subtraction trick only works because addition (and multiplication, and XOR) has an inverse operation. Minimum and maximum do not — once you have folded a value into a running minimum, there is no way to "un-fold" it back out — so Prefix Sum cannot answer range-minimum/maximum queries no matter how static the array is; that is a Sparse Table's or Segment Tree's job instead.

The **final fork** is a pure cost-benefit check: if you only expect a query or two, building the prefix array costs the same O(n) as just scanning the range directly — the precomputation only pays for itself once you amortize its one-time build cost across enough repeated queries.
