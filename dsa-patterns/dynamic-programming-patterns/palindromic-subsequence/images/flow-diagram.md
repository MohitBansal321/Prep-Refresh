# Palindromic Subsequence / Substring — Flow Diagram (Both Variants)

This traces the control flow of the two algorithms in [code.cpp](../code.cpp): expand-around-center for contiguous substrings, and interval DP for subsequences. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual numbers.

```mermaid
flowchart TD
    Start([Start: string s of length n]) --> Empty{s == empty?}
    Empty -- Yes --> Ret0([Return: '' / 0])

    Empty -- No --> Variant{Which question?}

    %% ---- Expand around center branch ----
    Variant -- "contiguous substring" --> Init["start = 0, maxLen = 1<br/>(any single char is a palindrome)"]
    Init --> CenterLoop{center < n?}
    CenterLoop -- No --> Answer(["Return s.substr(start, maxLen)"])

    CenterLoop -- Yes --> Odd["expand(c, c)<br/>odd-length: center is character c"]
    Odd --> Even["expand(c, c + 1)<br/>even-length: center is gap after c"]
    Even --> NextCenter[center++]
    NextCenter --> CenterLoop

    Odd --> Walk{left >= 0 && right < n<br/>&& s[left] == s[right]?}
    Even --> Walk
    Walk -- Yes --> Grow["--left; ++right"]
    Grow --> Walk
    Walk -- No --> Len["len = right - left - 1<br/>(window overshot by one on each side)"]
    Len --> Better{len > maxLen?}
    Better -- Yes --> Save["maxLen = len; start = left + 1"]
    Better -- No --> DoneExpand([this center exhausted])
    Save --> DoneExpand

    %% ---- Interval DP branch ----
    Variant -- "subsequence / gaps allowed" --> Table["allocate dp[n][n] = 0"]
    Table --> OuterLoop{i from n-1 down to 0?}
    OuterLoop -- "No, i went below 0" --> Final(["Return dp[0][n-1]"])

    OuterLoop -- Yes --> Base["dp[i][i] = 1<br/>(single char base case)"]
    Base --> InnerLoop{j from i+1 up to n-1?}
    InnerLoop -- "No, j exhausted" --> DecI[i--]
    DecI --> OuterLoop

    InnerLoop -- Yes --> Match{s[i] == s[j]?}
    Match -- Yes --> Take2["dp[i][j] = dp[i+1][j-1] + 2<br/>endpoints join the inner palindrome;<br/>note j-1 < i+1 when j == i+1,<br/>and that cell is 0 (below diagonal) —<br/>which correctly yields 'bb' style pairs"]
    Match -- No --> Drop["dp[i][j] = max(dp[i+1][j], dp[i][j-1])<br/>drop one endpoint, keep the better one"]
    Take2 --> NextJ[j++]
    Drop --> NextJ
    NextJ --> InnerLoop
```

## How to read it

The **left branch** (expansion) has no table at all: its entire state is `start`/`maxLen` plus the two walking indices. Each of the `2n − 1` centers triggers at most O(n) expansion steps, which is the whole O(n²) bound. The subtle line is `len = right - left - 1`: the while loop exits only *after* overshooting (either a mismatch or the boundary), so the last valid window is one smaller on each side than where the pointers stopped.

The **right branch** (interval DP) is where fill order lives or dies. The outer loop runs `i` **downward** and the inner loop `j` upward from `i + 1`; this guarantees that whenever `dp[i][j]` is computed, both dependencies — `dp[i+1][j-1]`, `dp[i+1][j]` (row below, already finished), and `dp[i][j-1]` (earlier in this row) — are final. Filling row-by-row top-down instead reads row `i+1` before it exists. One more subtlety hidden in the diagram: when `j == i + 1`, the cell `dp[i+1][j-1]` sits below the diagonal and holds the initialization value 0, so the `+ 2` formula lands on exactly 2 for an adjacent matching pair with no special-case code.
