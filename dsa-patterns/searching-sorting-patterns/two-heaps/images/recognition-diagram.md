# Two Heaps — Recognition Diagram

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Does it need the<br/>MEDIAN, or another<br/>"middle" order statistic,<br/>of numbers seen so far?}
    Q1 -- No --> Other1[Not Two Heaps --<br/>consider Top K Elements<br/>or K-way Merge]
    Q1 -- Yes --> Q2{Do numbers arrive<br/>one at a time<br/>(a stream)?}
    Q2 -- No, it's a fixed<br/>array queried once --> Other2[Sort once and index --<br/>Two Heaps' insert cost<br/>isn't needed]
    Q2 -- Yes --> Q3{Do you need the<br/>median after EVERY<br/>insertion, not just<br/>at the end?}
    Q3 -- No --> Other3[Insert everything,<br/>sort once at the end]
    Q3 -- Yes --> UseTwoHeaps[["Use Two Heaps:<br/>max-heap for the lower half +<br/>min-heap for the upper half,<br/>rebalanced after every insert"]]
```

**How to read it:** the diagram is a funnel of increasingly specific questions. The first branch rules out anything that isn't fundamentally about a *middle* value (median, or a similarly-defined order statistic) — if you need the largest/smallest K, that's Top K Elements, a different heap shape entirely. The second and third branches rule out cases where you could just sort once instead of paying an insertion cost on every single new number: Two Heaps only earns its keep when the data arrives incrementally *and* you need an up-to-date answer after each arrival, which is exactly the "running median" shape.
