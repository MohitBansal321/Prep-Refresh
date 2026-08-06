# Longest Increasing Subsequence — Flow Diagram

```mermaid
flowchart TD
    Start([For each number in the array, in order]) --> Search[Binary search tails<br/>for the first entry &gt;= number]
    Search --> Found{Found such an entry?}
    Found -- Yes --> Replace["Replace that entry with number<br/>(a smaller tail for that length<br/>is never worse)"]
    Found -- No, number is larger<br/>than every current tail --> Append["Append number to tails<br/>(extends the longest length found so far)"]
    Replace --> Next{More numbers left?}
    Append --> Next
    Next -- Yes --> Search
    Next -- No --> Done(["Answer = tails.size()"])
```

**How to read it:** every number in the input triggers exactly one binary search and one of two O(1) actions — replace an existing tail with a smaller value, or extend the piles by one. The diagram's key insight is that "replace" never shrinks the answer (it only makes a future extension easier by lowering a tail value), and "append" is the only action that actually grows the answer. By the time every number has been processed, the final size of `tails` is the LIS length.
