# Modified Binary Search — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether Modified Binary Search is the right tool, or whether the problem actually wants a plain linear scan or Two Pointers instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Is the input sorted,<br/>OR piecewise-sorted<br/>e.g. rotated at an<br/>unknown pivot,<br/>OR does it have some<br/>other monotonic property?}

    Q1 -- "No monotonic property at all" --> LinearScan[["Use a linear scan<br/>O(n) time — there is no<br/>exploitable structure to halve"]]

    Q1 -- Yes --> Q2{Do you need O(log n),<br/>or is O(n) already<br/>acceptable for this input size?}

    Q2 -- "O(n) is fine / input is tiny" --> LinearScan

    Q2 -- "Need O(log n)" --> Q3{What are you looking for?}

    Q3 -- "Does an exact value exist,<br/>and at what single index?" --> Classic["Use CLASSIC binary search<br/>lo=0, hi=n-1, three-way<br/>compare against nums[mid]"]

    Q3 -- "The first-or-last index<br/>of a value that may repeat,<br/>OR any 'boundary' between<br/>a false-region and a<br/>true-region" --> Boundary["Use BOUNDARY search<br/>on a match, keep narrowing<br/>into the same side you'd<br/>narrow into if it weren't<br/>a match yet"]

    Q3 -- "Array was sorted, then<br/>rotated at an unknown pivot" --> Rotated["Use ROTATED-ARRAY search<br/>at each mid, figure out which<br/>half is normally ordered,<br/>then test if target's value<br/>falls inside that half's range"]

    Q3 -- "Need a pair of elements<br/>from opposite ends of the<br/>array, not a single index" --> TwoPointers[["Use Two Pointers instead<br/>see ../../array-string-patterns/two-pointers/"]]

    Classic --> Done([Modified Binary Search applies])
    Boundary --> Done
    Rotated --> Done
```

## How to read it

Start at the top and answer each diamond honestly. The **first real fork** is whether the data has any exploitable monotonic structure at all — sortedness, piecewise-sortedness (rotated), or some other property where "everything on one side of a boundary is false and everything on the other side is true." Without that structure, there is nothing to halve, and a linear scan is not just simpler but *correct where binary search would not be* — halving requires being able to prove, from a single comparison at `mid`, which entire half can be discarded, and that proof only exists when the data has order to exploit.

The **second fork** is a pragmatic one that beginners skip: if the array is small (a few dozen elements) or this code runs once, not in a hot path, O(n) vs O(log n) does not matter in practice, and a simpler linear scan is easier to get right and to review. Reach for Modified Binary Search when the input is large enough, or called frequently enough, that the O(n) → O(log n) win is real.

The **third fork** separates the three shapes Modified Binary Search actually takes: an exact single-index lookup is the classic form; finding the first/last occurrence of a repeated value (or, more generally, the boundary between a region where some predicate is false and a region where it is true) is the *boundary-finding* form; and searching a rotated sorted array needs the halving *test itself* to change, because a plain `nums[mid]` vs `target` comparison is not enough when the array is not globally sorted. If instead you are looking for a *pair* of elements from two ends of the array (not a single index), that is Two Pointers' territory, not this pattern's — see the [family README](../../README.md) and this module's [Similar Patterns](../README.md#similar-patterns) section if you are unsure which one a problem wants.
