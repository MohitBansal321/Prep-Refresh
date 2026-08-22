# Bit Manipulation — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether bit tricks are the right tool, or whether the problem actually wants a hash set, plain arithmetic, or dynamic programming instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Does the answer hinge on the<br/>BINARY REPRESENTATION of integers,<br/>or on elements cancelling in pairs?}

    Q1 -- "No — it is general key/value<br/>lookup or membership" --> HashSet[["Use a Hash Set / Map<br/>O(n) time, O(n) extra space"]]
    Q1 -- "No — plain arithmetic works,<br/>e.g. sum formulas or division" --> Arithmetic[["Use arithmetic<br/>(Gauss sum for missing number,<br/>mod/div for digit problems)"]]

    Q1 -- Yes --> Q2{Which structure<br/>does the problem have?}

    Q2 -- "'Exactly one element appears once;<br/>every other element appears an EVEN<br/>number of times'" --> Xor[["Use XOR accumulation<br/>result ^= x over all elements<br/>O(n) time, O(1) space"]]

    Q2 -- "'Every element appears k times except one'<br/>where k is ODD and > 1" --> PerBit[["Per-bit-position counting<br/>sum bits per position, mod k<br/>NOT plain XOR — it breaks here"]]

    Q2 -- "Power-of-two check, lowest set bit,<br/>count set bits of ONE value" --> ClearLow[["Use n & (n-1) tricks<br/>clear lowest set bit / test == 0<br/>loop runs popcount times"]]

    Q2 -- "Count set bits for EVERY i<br/>in 0..n (a whole table)" --> BitsDP[["Use DP recurrence<br/>bits[i] = bits[i >> 1] + (i & 1)<br/>reuses smaller subproblems,<br/>never re-popcounts from scratch"]]

    Q2 -- "Enumerate ALL subsets of an n-element set<br/>and n <= ~20" --> MaskEnum[["Iterate mask 0 .. 2^n - 1<br/>bit i set => element i included<br/>non-recursive subset generation"]]

    Q2 -- "Enumerate ALL subsets but<br/>n > ~20-25" --> ExponentialWall[["STOP: 2^n masks is infeasible<br/>same exponential wall as Subsets —<br/>reformulate as DP / greedy / backtracking"]]

    Q2 -- "Find a duplicate/missing value where<br/>values map to a bounded index range" --> Cyclic[["Use Cyclic Sort or index-marking<br/>(values-as-indices), not XOR"]]

    Xor --> Done([Bit Manipulation applies])
    PerBit --> Done
    ClearLow --> Done
    BitsDP --> Done
    MaskEnum --> Done
```

## How to read it

Start at the top and answer each diamond honestly before moving on — the most common mistake is reaching for XOR because the problem says "single unique element," without checking what the *distractors'* multiplicities are. The **first real fork** is whether binary representation matters at all: if you are doing ordinary lookup ("have I seen this value?"), a hash set is simpler and more general, and if arithmetic already solves it (sum formula, division), do not obscure the code with bit tricks. Bit Manipulation earns its complexity only when the problem's structure mirrors the algebra of bits.

The **second fork** separates the four working flavors by their exact preconditions: XOR cancellation needs every distractor to appear an **even** number of times; per-bit-position counting handles odd multiplicities like "everything thrice, answer once"; `n & (n-1)` covers single-value properties (power-of-two tests, popcount); and the Counting Bits DP exists precisely because re-popcounting each of `0..n` independently wastes work that the `bits[i >> 1]` subproblem already did. Mask enumeration is the only exponential flavor — before using it, confirm `n <= ~20` and remember that beyond that boundary it is the same wall recursive Subsets hits, so the exit at the bottom right applies. When two flavors look plausible, the Similar Patterns section of this module's [README](../README.md) and the worked traces in [trace-diagram.md](trace-diagram.md) are worth checking before committing.
