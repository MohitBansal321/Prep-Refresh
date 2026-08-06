# Top K Elements — Flow Diagram

This traces the control flow of the general "K largest" size-K min-heap algorithm — the shape behind Kth Largest Element, Top K Frequent Elements (with a frequency key substituted for the raw value), and every other variant in this module. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual numbers.

```mermaid
flowchart TD
    Start([Start: input collection, K]) --> Init["Initialize an empty heap<br/>(MIN-heap for 'K largest',<br/>MAX-heap for 'K smallest')"]
    Init --> Loop{Any elements left<br/>to process?}

    Loop -- No --> Drain["Drain the heap:<br/>pop everything into a list"]
    Drain --> NeedSort{Does the problem need<br/>the final K elements<br/>in sorted order?}
    NeedSort -- Yes --> Sort["Sort the small list<br/>(extra O(k log k), cheap<br/>because k is small)"]
    NeedSort -- No --> Done([Return the K elements])
    Sort --> Done

    Loop -- Yes --> Push["Push the next element<br/>onto the heap"]
    Push --> SizeCheck{"heap.size() > K?"}

    SizeCheck -- No --> Loop
    SizeCheck -- "Yes (heap overgrew by 1)" --> Evict["Pop once --<br/>evicts the current WEAKEST<br/>member of the top-K-so-far<br/>(smallest, for a min-heap;<br/>largest, for a max-heap)"]
    Evict --> Loop
```

## How to read it

The loop body is exactly two steps, repeated once per input element: **push**, then **check the size and pop if oversized**. That is the entire mechanism — there is no separate "is this candidate even worth considering" comparison before the push. Every element gets pushed unconditionally; the heap's own size-triggered eviction is what decides, in O(log k), whether it was worth keeping. This is deliberately simpler than it might first seem: a candidate that turns out to be weaker than everything already held gets pushed and then immediately popped back off, at the same O(log k) cost as if you had checked first — so there is no correctness or performance reason to add a pre-check, only extra code to get wrong.

The **size check** (`heap.size() > K`) is the only place K appears in the whole loop, and it is why the heap never grows past K elements no matter how large the input is — that bound is what gives the pattern its O(k) space guarantee and its O(log k) per-element cost, both independent of the total input size n. Notice also the **drain-and-optionally-sort** step after the loop ends: the heap guarantees the *identity* of the top-K elements, not their relative order, so if the problem's output must be sorted, that is always a separate, explicit, cheap (O(k log k)) step tacked on afterward — never something the heap does for you automatically.
