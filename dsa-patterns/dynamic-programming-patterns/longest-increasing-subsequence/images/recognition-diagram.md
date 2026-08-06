# Longest Increasing Subsequence — Recognition Diagram

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{How many sequences<br/>are being compared?}
    Q1 -- Two --> LCS[Not LIS --<br/>Longest Common Subsequence]
    Q1 -- One --> Q2{Must the result be<br/>CONTIGUOUS?}
    Q2 -- Yes --> Other[Not LIS --<br/>simple O(n) scan,<br/>or Kadane's if sum-based]
    Q2 -- No, gaps allowed --> Q3{Is the relationship<br/>an ORDER property<br/>-- increasing/decreasing?}
    Q3 -- No, it's equality/matching --> LCS2[Not LIS --<br/>likely Longest Common<br/>Subsequence against itself,<br/>or a different pattern entirely]
    Q3 -- Yes --> UseLIS[["Use Longest Increasing Subsequence:<br/>O(n^2) DP for simplicity,<br/>O(n log n) patience sorting for scale"]]
```

**How to read it:** the diagram narrows down through three questions, in order of how often they're conflated. First: one sequence or two — LIS is strictly a one-sequence pattern. Second: contiguous or not — LIS explicitly allows gaps, which is what makes it a DP problem instead of a simple linear scan. Third: is the relationship genuinely about *order* (each next pick strictly bigger/smaller) rather than equality — that's the property LIS's recurrence actually tracks.
