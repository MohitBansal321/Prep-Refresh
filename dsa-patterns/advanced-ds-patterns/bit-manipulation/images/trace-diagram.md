# Bit Manipulation — Trace Diagram (Worked Example)

This traces the exact bit-level state of `countSetBits(11)` — the same example used in [code.cpp](../code.cpp) and [problems/02-number-of-1-bits.cpp](../problems/02-number-of-1-bits.cpp), following the control flow in [flow-diagram.md](flow-diagram.md):

```
n = 11 = 0b1011   (three set bits, so the loop must run exactly 3 times)
```

```
Iteration |      n (binary)  |    n-1 (binary)  |  n & (n-1)       | count
----------+------------------+------------------+------------------+-------
    1     |  0b1011 (11)     |  0b1010 (10)     |  0b1010 (10)     |   1
          |        ^^ lowest set bit at position 0 erased            |
----------+------------------+------------------+------------------+-------
    2     |  0b1010 (10)     |  0b1001 (9)      |  0b1000 (8)      |   2
          |       ^-- lowest set bit at position 1 erased             |
----------+------------------+------------------+------------------+-------
    3     |  0b1000 (8)      |  0b0111 (7)      |  0b0000 (0)      |   3
          |  ^--- lowest set bit at position 3 erased                 |
----------+------------------+------------------+------------------+-------
    4     |  n == 0 -> loop exits. Return count = 3                  |
```

Watch the subtraction step closely in iteration 2: `1010 - 1 = 1001` does **not** just flip the last bit — it flips the lowest set bit (position 1) to `0` and turns every bit *below* it to `1`. AND-ing against the original keeps the untouched high bits (`10..`) and zeroes the lowest set bit plus everything under it, yielding `1000`. Iteration 3 is the most dramatic case: for a lone set bit, `n - 1 = 0111` flips the set bit off and fills all three positions below it with ones, so the AND wipes `n` to zero entirely.

## How to read it

Each row is one loop iteration of the algorithm in [flow-diagram.md](flow-diagram.md): compute `n - 1`, AND it into `n`, increment the counter. The invariant that makes this correct: after iteration `k`, `n` retains exactly its original bits **except** the `k` lowest originally-set bits have been removed, and no other bit was ever disturbed — every intermediate value of `n` is a "suffix" of the original `0b1011` with progressively fewer set bits (`1011 -> 1010 -> 1000 -> 0000`). That is also why the loop runs popcount(n) times rather than 32 times: iterations are consumed one-per-set-bit, and the moment the last set bit dies (iteration 3 here), `n == 0` ends the loop.

The same identity answers power-of-two questions without looping at all: had we started with `n = 8 = 0b1000`, iteration 1 would already produce `0000`, proving exactly one set bit — which is precisely why `isPowerOfTwo(n)` can be `(n != 0 && (n & (n-1)) == 0)`, and why forgetting the `n != 0` guard misclassifies zero.
