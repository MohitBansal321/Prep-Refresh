# Palindromic Subsequence / Substring — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether this module's techniques apply, or whether the problem actually wants plain LCS, brute force, or something else entirely.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Does the question involve<br/>palindromes inside ONE string?}

    Q1 -- "No, TWO strings being compared" --> LCS[["Use Longest Common Subsequence<br/>(dp[i][j] over two independent sequences)<br/>Note: LPS(s) = LCS(s, reverse(s)) is<br/>the one legitimate bridge between them"]]
    Q1 -- "No, palindrome over a whole array of<br/>numbers / rearrangement allowed" --> Freq[["Use frequency counting:<br/>pairs contribute 2, one leftover<br/>contributes 1 (e.g. LC 409)"]]

    Q1 -- Yes --> Q2{Must the palindrome be<br/>CONTIGUOUS (substring),<br/>or are GAPS allowed<br/>(subsequence)?}

    Q2 -- Contiguous --> Q3{What is asked?}
    Q3 -- "Longest palindromic substring,<br/>n small (~10^3)" --> Expand["Expand Around Center<br/>O(n^2) time, O(1) space"]
    Q3 -- "Count all palindromic substrings" --> Expand
    Q3 -- "Need is-palindrome table for<br/>follow-up logic (partitioning/cutting)" --> BoolDP["Boolean interval DP table<br/>isPal[i][j] = s[i]==s[j] &&<br/>(len<=2 || isPal[i+1][j-1])"]
    Q3 -- "Longest palindromic substring,<br/>n large, O(n) required" --> Manacher[["Manacher's algorithm<br/>O(n) time — rarely needed<br/>outside competitive programming"]]

    Q2 -- Gaps allowed --> Q4{Already have an<br/>LCS implementation?}
    Q4 -- Yes --> ViaLCS["LPS(s) = LCS(s, reverse(s))<br/>reuse it directly"]
    Q4 -- No --> IntervalDP["Interval DP<br/>dp[i][j] from dp[i+1][j-1]+2<br/>or max(dp[i+1][j], dp[i][j-1])<br/>O(n^2) time and space"]

    Q2 -- "Min insertions/deletions to<br/>make the string a palindrome" --> Reduce["Reduce to LPS first:<br/>answer = n - LPS(s)<br/>then run IntervalDP or ViaLCS"]

    Expand --> Done([Pattern applies])
    BoolDP --> Done
    IntervalDP --> Done
    ViaLCS --> Done
    Reduce --> Done

    Start -.-> Brute[["AVOID: brute force<br/>all O(n^2) substrings x O(n)</>check each = O(n^3);<br/>subsequences: 2^n candidates"]]
```

## How to read it

Start at the top and answer each diamond honestly before moving on — the most common mistake is jumping straight to a technique because the word "palindrome" appeared, without checking **how many strings** are involved. Two strings means LCS territory ([../longest-common-subsequence/](../longest-common-subsequence/)); the only bridge is the identity `LPS(s) = LCS(s, reverse(s))`, which is why the LCS exit still points back at this module.

The **second fork** is contiguous vs gaps, and it decides everything downstream. Expansion walks *adjacent* characters outward from a center, so it physically cannot see a subsequence that skips characters — if gaps are allowed, expansion is simply the wrong tool no matter how you bend it. Conversely, interval DP handles both flavors (a boolean variant for substring questions, an integer variant for subsequence questions), so when in doubt or when you need a reusable table, interval DP is the safe choice.

The bottom branch encodes the most valuable reduction in the module: any "minimum insertions/deletions to make s a palindrome" question collapses to `n − LPS(s)` — the characters already inside some palindromic arrangement need no touching; every character outside the longest palindromic subsequence needs exactly one insertion (or its partner deletion). And the dashed exit is a reminder of what you are avoiding: brute force costs O(n³) for substrings and 2^n for subsequences, which is the entire reason these O(n²) techniques exist.
