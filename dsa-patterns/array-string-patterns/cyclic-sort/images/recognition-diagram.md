# Cyclic Sort — Recognition Diagram

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Are the array's values<br/>drawn from a bounded range<br/>tied to its size --<br/>[1..n] or [0..n-1]?}
    Q1 -- No, values span a huge<br/>or unrelated range --> Other1[Not Cyclic Sort --<br/>use a hash set/map instead]
    Q1 -- Yes --> Q2{Is the question about a<br/>missing, duplicate, or<br/>misplaced value?}
    Q2 -- No --> Other2[Not Cyclic Sort --<br/>consider a different array pattern]
    Q2 -- Yes --> Q3{Is O(1) extra space<br/>required, and is mutating<br/>the input acceptable?}
    Q3 -- Input must NOT<br/>be mutated --> Other3[Consider Fast &amp; Slow Pointers<br/>-- finds a single duplicate<br/>without mutating the array]
    Q3 -- Yes, mutation is fine --> UseCyclicSort[["Use Cyclic Sort:<br/>swap each value to its home index<br/>in a single O(n) pass"]]
```

**How to read it:** the diagram funnels through three checks. First, the single most important signal — do the values themselves encode valid array indices (the `[1..n]`/`[0..n-1]` bounded-range property)? Without that, there's no "home index" to swap toward, and the pattern doesn't apply at all. Second, is the question actually about missing/duplicate/misplaced values, as opposed to some other property of the array? Third, a genuine tradeoff: if the input can't be mutated, Fast & Slow Pointers solves the single-duplicate case in the same O(1) space without touching the original array.
