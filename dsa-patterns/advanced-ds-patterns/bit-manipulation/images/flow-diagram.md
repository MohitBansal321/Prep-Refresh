# Bit Manipulation — Flow Diagram (n & (n-1) Clear-Lowest-Bit Loop)

This traces the control flow of the `n & (n-1)` loop — the shape behind set-bit counting, power-of-two testing, and any "process one set bit at a time" technique. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual binary values.

```mermaid
flowchart TD
    Start([Start: an unsigned value n]) --> Test{n != 0?<br/>any set bits remain?}

    Test -- No --> Done([Return: count of set bits<br/>or 'loop ran popcount(n) times'])

    Test -- Yes --> Sub["Compute n - 1<br/>flips the lowest set bit to 0 and turns<br/>every bit BELOW it to 1"]

    Sub --> And["n = n & (n - 1)<br/>keeps bits above the lowest set bit,<br/>zeroes it and everything below"]

    And --> Count[Increment count<br/>or: this is where a one-shot variant stops —<br/>if n was a power of two, n & (n - 1)<br/>is now 0, proving exactly one set bit]

    Count --> Test
```

## How to read it

The single test `n != 0` is the entire lifetime of the algorithm — every iteration does a fixed O(1) amount of work (one subtraction, one AND, one increment), and the number of iterations equals **popcount(n)**, the number of set bits in `n`, *not* the 32/64-bit word width. That is why `countSetBits(0xFFFFFFFF00000000)` takes 32 iterations even on a 64-bit value, and why `countSetBits(1)` takes one. Each iteration surgically removes exactly one set bit — the lowest one — so total work is proportional to how many bits are actually set.

The **subtraction step** is where all the cleverness lives: subtracting 1 from `n` flips its lowest set bit from 1 to 0 and sets every bit below it (e.g. `1011 -> 1010`, `1000 -> 0111`). AND-ing that against the original therefore erases the lowest set bit and everything beneath it while preserving everything above. The same identity doubles as the **power-of-two test** in one shot without looping: a power of two has exactly one set bit (`1000...`), so `n & (n-1)` collapses it to zero immediately — with the mandatory `n != 0` guard, since zero also satisfies `(n & (n-1)) == 0` but is not a power of two. For the other canonical control flow — XOR accumulation over an array, which has no loop condition at all beyond visiting each element once — see [singleNumber](../problems/01-single-number.cpp).
