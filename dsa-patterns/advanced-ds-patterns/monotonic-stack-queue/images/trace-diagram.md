# Monotonic Stack/Queue — Trace Diagram (Worked Example)

This traces the exact stack state at every step for the **Next Greater Element** example used in [code.cpp](../code.cpp):

```
nums   = [2, 1, 2, 4, 3]   (0-indexed)
index:    0  1  2  3  4
result starts as [-1, -1, -1, -1, -1]
stack holds INDICES; shown with their values
```

```mermaid
sequenceDiagram
    autonumber
    participant I as Incoming element
    participant S as Stack (indices, values increase bottom-to-top)
    participant R as result[]

    Note over S,R: Step 0 — i=0, value 2. Stack empty -> nothing to pop.
    I->>S: push 0 (value 2)
    Note over S: stack: [0:2]

    Note over S,R: Step 1 — i=1, value 1. Top is 2, not beaten by 1.
    I->>S: push 1 (value 1)
    Note over S: stack: [0:2, 1:1]

    Note over S,R: Step 2 — i=2, value 2. Top is 1 < 2 -> POP and resolve.
    S-->>R: pop index 1, result[1] = 2
    Note over R: result: [-1, 2, -1, -1, -1]
    Note over S: top now 0:2; 2 < 2 is FALSE (strict) -> stop popping
    I->>S: push 2 (value 2)
    Note over S: stack: [0:2, 2:2]

    Note over S,R: Step 3 — i=3, value 4. Top is 2 < 4 -> POP and resolve.
    S-->>R: pop index 2, result[2] = 4
    Note over R: result: [-1, 2, 4, -1, -1]
    Note over S: top now 0:2; 2 < 4 -> POP again
    S-->>R: pop index 0, result[0] = 4
    Note over R: result: [4, 2, 4, -1, -1]
    Note over S: stack empty -> stop popping
    I->>S: push 3 (value 4)
    Note over S: stack: [3:4]

    Note over S,R: Step 4 — i=4, value 3. Top is 4, not beaten by 3.
    I->>S: push 4 (value 3)
    Note over S: stack: [3:4, 4:3]

    Note over S,R: Scan ends. Indices 3 and 4 never found a next greater -> keep sentinel -1.
    Note over R: final result: [4, 2, 4, -1, -1]
```

## How to read it

Each step is one outer-loop iteration of the algorithm in [flow-diagram.md](flow-diagram.md): the incoming element first drains everything it beats (possibly nothing, possibly several), then joins the stack itself. Step 3 is the interesting one — value **4 pops two entries in a single step**, which is exactly why the pop condition must be a `while` loop rather than an `if`, and it also shows the amortized argument in action: those two pops are paid for by the two pushes that put them on the stack earlier, so even this "expensive" step costs O(1) amortized.

Notice two more things. First, the stack's values stay **non-increasing from bottom to top** at every snapshot ([2], then [2,1], then [2,2], then [4], then [4,3]): each entry survives only because nothing that arrived after it was strictly greater, so anything strictly smaller than an arriving element is popped before the new one lands — and equal values coexist (step 2 keeps both 2s) precisely because the comparison is strict `<`. Second, indices 3 and 4 end the scan still on the stack — they have no next greater element, which is why they correctly retain the `-1` sentinel they were initialized with. The stack is not "leftover garbage"; it is the set of still-unanswered questions.
