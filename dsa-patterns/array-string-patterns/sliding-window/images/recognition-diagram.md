# Sliding Window — Recognition Diagram

Use this diagram when you read a new problem statement and are not yet sure which pattern applies. Follow the diamonds top to bottom.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Does the answer involve a<br/>CONTIGUOUS subarray or substring?}

    Q1 -- No, any subset/pair, or array is sorted --> OtherA{Sorted array,<br/>looking for a pair/triplet?}
    OtherA -- Yes --> TwoPointers[["Use Two Pointers<br/>(see ../two-pointers/)"]]
    OtherA -- No --> OtherB{Many range-sum queries,<br/>no updates to the array?}
    OtherB -- Yes --> PrefixSum[["Use Prefix Sum<br/>(see ../prefix-sum/)"]]
    OtherB -- No --> OtherC[Consider other families:<br/>DP, Greedy, Hashing]

    Q1 -- Yes, contiguous --> Q2{Is the window size K<br/>given directly in the problem?}

    Q2 -- Yes, fixed K --> FixedSignals["Signals: 'every window of size K',<br/>'K consecutive elements',<br/>'average/sum/max of size-K windows'"]
    FixedSignals --> FixedWindow[["Use FIXED-SIZE Sliding Window"]]

    Q2 -- No, size is not given --> Q3{Do you need the LONGEST or<br/>SHORTEST contiguous range<br/>satisfying some condition?}

    Q3 -- Yes --> Q4{Does growing the window monotonically<br/>help/hurt the condition<br/>e.g. more elements = only helps or only hurts?}
    Q4 -- Yes, monotonic --> VarWindow[["Use VARIABLE-SIZE Sliding Window<br/>(grow right, shrink left)"]]
    Q4 -- No, adding elements can<br/>help or hurt unpredictably --> NotMonotonic["Sliding Window's shrink logic breaks.<br/>Consider Prefix Sum + Hash Map<br/>(e.g. subarray sum equals K with negatives)"]

    Q3 -- No, just want best contiguous<br/>sum with no window-size constraint --> Kadane[["Use Kadane's Algorithm<br/>(see ../kadanes-algorithm/)"]]

    style FixedWindow fill:#2b6cb0,color:#fff
    style VarWindow fill:#2b6cb0,color:#fff
    style TwoPointers fill:#4a5568,color:#fff
    style PrefixSum fill:#4a5568,color:#fff
    style Kadane fill:#4a5568,color:#fff
```

**How to read it:** the first diamond is the gate — Sliding Window only applies to **contiguous** ranges (a subarray or substring), never to arbitrary subsets. Once you know it is contiguous, the second diamond splits on whether the window's size is an *input* (fixed-size) or an *output* of the search (variable-size, longest/shortest). The variable-size branch has one more gate that people often skip: the condition must be **monotonic** with window size — growing the window must only ever help or only ever hurt, never both, or the shrink loop's "shrink while invalid" logic is unsound. That is exactly why Sliding Window works for non-negative sum thresholds (Minimum Size Subarray Sum) but not for "subarray sum equals K" when negative numbers are allowed — that problem needs Prefix Sum + a hash map instead, because a bigger window can have either a bigger or smaller sum.
