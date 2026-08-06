# Longest Common Subsequence — Flow Diagram (2D Table Fill)

This traces the control flow of filling the `dp[i][j]` table row by row — the shape behind LCS itself, Edit Distance, and every LCS-derived problem (Shortest Common Supersequence, Delete Operation for Two Strings). See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual characters and numbers.

```mermaid
flowchart TD
    Start([Start: two sequences A length n, B length m]) --> Init["Create table dp sized (n+1) x (m+1)<br/>Initialize row 0 and column 0 to the base case<br/>(0 for LCS; i or j for Edit Distance)"]

    Init --> RowLoop{More rows i<br/>from 1 to n?}
    RowLoop -- No, all rows filled --> ReadAnswer["Read the final answer at dp[n][m]"]

    RowLoop -- Yes --> ColLoop{More columns j<br/>in this row,<br/>from 1 to m?}
    ColLoop -- No, row i done --> RowLoop

    ColLoop -- Yes --> Match{Does A[i-1]<br/>equal B[j-1]?}

    Match -- "Yes, characters match" --> ExtendDiag["MATCH BRANCH:<br/>Extend the diagonal neighbor<br/>dp[i][j] = dp[i-1][j-1] + 1  (LCS)<br/>dp[i][j] = dp[i-1][j-1]      (Edit Distance, no cost)"]

    Match -- "No, characters differ" --> BestOfSkip["NO-MATCH BRANCH:<br/>Best of skipping a character from EITHER side<br/>dp[i][j] = max(dp[i-1][j], dp[i][j-1])         (LCS)<br/>dp[i][j] = 1 + min(dp[i-1][j-1], dp[i-1][j], dp[i][j-1])  (Edit Distance)"]

    ExtendDiag --> NextCol[Advance j, stay on row i]
    BestOfSkip --> NextCol
    NextCol --> ColLoop

    ReadAnswer --> Reconstruct{Need the actual<br/>subsequence/string,<br/>not just a number?}
    Reconstruct -- No --> Done([Return dp[n][m] as the final answer])
    Reconstruct -- Yes --> WalkBack["Walk backwards from (n, m) to (0, 0):<br/>match -> record character, step diagonally<br/>no-match -> step toward whichever neighbor<br/>the forward pass actually used"]
    WalkBack --> Reverse[Reverse the collected characters<br/>since the walk moved end-to-start]
    Reverse --> Done
