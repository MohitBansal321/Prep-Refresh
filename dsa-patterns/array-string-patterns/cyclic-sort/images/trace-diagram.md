# Cyclic Sort — Trace Diagram

Tracing `cyclic_sort` on `[3, 1, 5, 4, 2]` (a complete permutation of `[1..5]`), one swap at a time.

```mermaid
sequenceDiagram
    autonumber
    participant C as Cursor i
    participant A as Array

    Note over A: [3, 1, 5, 4, 2]  (i=0)
    C->>A: nums[0]=3, home index = 2, nums[2]=5 (not home) -> swap(0,2)
    Note over A: [5, 1, 3, 4, 2]  (i stays 0)

    C->>A: nums[0]=5, home index = 4, nums[4]=2 (not home) -> swap(0,4)
    Note over A: [2, 1, 3, 4, 5]  (i stays 0)

    C->>A: nums[0]=2, home index = 1, nums[1]=1 (not home) -> swap(0,1)
    Note over A: [1, 2, 3, 4, 5]  (i stays 0)

    C->>A: nums[0]=1, home index = 0 -- already home -> advance
    Note over A: i=1

    C->>A: nums[1]=2, home index = 1 -- already home -> advance
    Note over A: i=2, 3, 4 all already home -> loop ends

    Note over A: Final: [1, 2, 3, 4, 5] -- fully sorted, 3 swaps total
```

**How to read it:** position 0 needed three swaps before it settled, because each swap brought in a brand-new value that itself wasn't home yet — this is exactly why the cursor doesn't advance after a swap. Once a value lands in its correct home (like the final `1` at index 0), it is never touched again for the rest of the run. Total swaps across the whole array never exceed `n`, because each swap permanently places one more value into its final position.
