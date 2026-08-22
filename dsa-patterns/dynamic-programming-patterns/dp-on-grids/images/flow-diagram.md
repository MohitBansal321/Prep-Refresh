# DP on Grids — Flow Diagram (Row-by-Row Fill)

This traces the control flow of the general grid-DP fill loop — the shape behind Unique Paths, Minimum Path Sum, and their obstacle variants. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual numbers.

```mermaid
flowchart TD
    Start([Start: grid with R rows, C cols]) --> Init["Allocate dp table<br/>(R x C), or a rolling 1D array of size C"]
    Init --> Outer{More rows to fill?<br/>r < R ?}

    Outer -- No --> Answer([Return: dp[R-1][C-1]<br/>— the answer lives at the<br/>bottom-right corner])

    Outer -- Yes --> Inner{More cells in this row?<br/>c < C ?}
    Inner -- No, row done --> Advance["r++<br/>(move to next row)"]
    Advance --> Outer

    Inner -- Yes --> Obs{Is (r, c)<br/>an obstacle?}
    Obs -- "Yes" --> Force["Force the special value:<br/>0 for counting / INF for cost.<br/>SKIP the recurrence entirely"]
    Obs -- No --> Base{Is (r, c) in the<br/>base case? r==0 or c==0}

    Base -- "Yes: top row or left column" --> EdgeVal["Assign the edge value:<br/>1 way (counting) or the running<br/>sum/cost along the edge"]
    Base -- "No: interior cell" --> Recur["Read already-computed neighbors:<br/>up = dp[r-1][c], left = dp[r][c-1]<br/>(treat out-of-grid as 0 / INF)"]

    Recur --> Combine["Combine:<br/>counting -> up + left<br/>cost -> min(up, left) + cell value"]
    Combine --> Store
    Force --> Store
    EdgeVal --> Store

    Store["dp[r][c] = result"] --> Step["c++"]
    Step --> Inner
```

## How to read it

The two nested loops are the entire lifetime of the algorithm — every cell triggers exactly one O(1) decision, and there are `R * C` cells, which is the whole argument for the O(R * C) time bound. The **fill order is not a stylistic choice**: row by row, left to right is precisely the order that guarantees both dependencies read by an interior cell (`up` and `left`) were stored in earlier iterations. Any fill order with that property works; this one is simply the easiest to write correctly.

The three branches inside the inner loop are where implementations go wrong, in decreasing order of frequency:

- **Obstacle check first** (`Obs`): an obstacle cell must be *forced* to its special value and the recurrence skipped — running the normal recurrence on it silently lets paths flow through a wall.
- **Base case** (`Base`): top-row and left-column cells have exactly **one** way to be reached (a straight line from the start), so counting problems store 1 there, not 0. Cost problems instead accumulate the running sum along the edge, since there is no choice of predecessor to minimize over.
- **Interior recurrence** (`Recur` → `Combine`): out-of-grid neighbors are neutralized by the identity element — 0 for counting (adds nothing), INT_MAX for cost (never chosen by min).

The rolling 1D-array optimization slots into this same flowchart unchanged: `dp[r-1][c]` becomes "the value currently sitting at index `c`" (not yet overwritten this row) and `dp[r][c-1]` becomes "the value just written at index `c-1`". The branch structure, the obstacle override, and the base-case handling are all identical — only the storage changes.
