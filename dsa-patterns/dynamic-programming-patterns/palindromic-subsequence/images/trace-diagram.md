# Palindromic Subsequence / Substring — Trace Diagram (Worked Example)

This traces the exact cell-by-cell fill of the interval DP table for the **Longest Palindromic Subsequence** example used in [code.cpp](../code.cpp):

```
s = "bbbab"   (0-indexed: s[0]='b', s[1]='b', s[2]='b', s[3]='a', s[4]='b')
expected answer: 4  ("bbbb" — indices 0,1,2,4)
```

The table fills **i from n−1 down to 0**, and within each row **j from i+1 up** — exactly the order in [flow-diagram.md](flow-diagram.md). Cells below the diagonal are never used (value 0).

```
        j=0    j=1    j=2    j=3    j=4
i=0      1      2      3      3     [4]   <- final answer dp[0][4]
i=0             1      2      2      3
i=2                    1      1      3
i=3                           1      1
i=4                                  1
```

Step-by-step, in computation order:

| Step | i | j | s[i] vs s[j] | Rule applied | dp[i][j] | Meaning |
|------|---|---|--------------|--------------|----------|---------|
| 1 | 4 | — | — | base case | dp[4][4] = 1 | "b" |
| 2 | 3 | 3 | — | base case | dp[3][3] = 1 | "a" |
| 3 | 3 | 4 | 'a' ≠ 'b' | max(dp[4][4], dp[3][3]) | dp[3][4] = 1 | best in "ab" is a single char |
| 4 | 2 | 2 | — | base case | dp[2][2] = 1 | "b" |
| 5 | 2 | 3 | 'b' ≠ 'a' | max(dp[3][3], dp[2][2]) | dp[2][3] = 1 | best in "ba" |
| 6 | 2 | 4 | 'b' == 'b' | dp[3][3] + 2 | dp[2][4] = 3 | "bab" inside "bbab" |
| 7 | 1 | 1 | — | base case | dp[1][1] = 1 | "b" |
| 8 | 1 | 2 | 'b' == 'b' | dp[2][1] + 2 = 0 + 2 | dp[1][2] = 2 | "bb"; note dp[2][1] is below-diagonal 0 |
| 9 | 1 | 3 | 'b' ≠ 'a' | max(dp[2][3], dp[1][2]) | dp[1][3] = 2 | drop an endpoint; "bb" beats "ba"/"aa" |
| 10 | 1 | 4 | 'b' == 'b' | dp[2][3] + 2 | dp[1][4] = 3 | "bbb" inside "bbab" |
| 11 | 0 | 0 | — | base case | dp[0][0] = 1 | "b" |
| 12 | 0 | 1 | 'b' == 'b' | dp[1][0] + 2 = 0 + 2 | dp[0][1] = 2 | "bb" |
| 13 | 0 | 2 | 'b' == 'b' | dp[1][1] + 2 | dp[0][2] = 3 | "bbb" |
| 14 | 0 | 3 | 'b' ≠ 'a' | max(dp[1][3], dp[0][2]) | dp[0][3] = 3 | keep "bbb", drop the 'a' |
| 15 | 0 | 4 | 'b' == 'b' | dp[1][3] + 2 | **dp[0][4] = 4** | wrap 'b'(0) and 'b'(4) around "bb" → "bbbb" |

## How to read it

Each row of the step table is one iteration of the inner loop in [code.cpp](../code.cpp); steps 1–3 fill row `i=3` before row `i=2` even starts, which is the fill-order guarantee in action. Watch two things. First, **step 8**: when `j == i + 1`, the formula reads `dp[i+1][j-1]` — below the diagonal, still 0 — so adjacent matching characters correctly score 2 with no special case. Second, **step 15**: `dp[0][4]` takes `dp[1][3] + 2 = 2 + 2 = 4`. The endpoints `'b'` at index 0 and index 4 join whatever the *inner* interval's best palindrome was ("bb"), producing "bbbb". The recurrence never needs to compare this against dropping an endpoint here — when the ends match, wrapping them around the inner optimum is always at least as good.

Also notice what the trace does **not** do: it never re-examines earlier rows after moving on. Every dependency (`dp[i+1][j-1]`, `dp[i+1][j]`, `dp[i][j-1]`) is already final when read — one pass, O(n²) cells, done. If you ever find yourself wanting to revisit a cell, the fill order is wrong.
