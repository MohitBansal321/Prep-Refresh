# Two Pointers — Recognition Diagram

Use this flowchart when you are staring at a new problem and trying to decide whether Two Pointers is the right tool, or whether the problem actually wants Sliding Window, Hashing, or Binary Search instead.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Is the input an array/string<br/>that is sorted, or can be<br/>sorted without losing<br/>needed information?}

    Q1 -- No, order/index matters and is unsorted --> Q1b{Do you need a<br/>pair/element by original index<br/>e.g. classic Two Sum?}
    Q1b -- Yes --> Hashing[["Use Hashing<br/>(store seen values in a<br/>hash map, O(n) time, O(n) space)"]]
    Q1b -- No, could sort first --> ConsiderSort[Consider sorting,<br/>then re-enter this diagram]

    Q1 -- Yes, sorted or sortable --> Q2{What are you looking for?}

    Q2 -- "A pair/triplet that sums to<br/>(or compares to) a target" --> Q3{Need an EXACT<br/>single value, or a single<br/>index via search?}
    Q3 -- "Single index, e.g. 'find target X'" --> BinarySearch[["Use Binary Search<br/>O(log n) time"]]
    Q3 -- "A pair/triplet across the array" --> Converging["Use Two Pointers<br/>(CONVERGING variant)<br/>left = 0, right = n-1,<br/>move inward based on comparison"]

    Q2 -- "Best/maximum value derived from<br/>two endpoints, e.g. area, capacity" --> Converging

    Q2 -- "In-place compaction: remove<br/>duplicates, remove a value,<br/>move zeroes, partition" --> SameDir["Use Two Pointers<br/>(SAME-DIRECTION variant)<br/>slow write pointer + fast read pointer"]

    Q2 -- "Contiguous subarray/substring<br/>with a size or running-property<br/>constraint (sum, distinct count...)" --> SlidingWindow[["Use Sliding Window<br/>(a growing/shrinking window,<br/>not two independent pointers)"]]

    Converging --> Done([Two Pointers applies])
    SameDir --> Done
```

## How to read it

Start at the top and answer each diamond honestly before moving on — the most common mistake is skipping the sortedness check because "the array looks small enough." The **first real fork** is sortedness: Two Pointers' converging variant leans entirely on the fact that moving a pointer inward has a *predictable, monotonic* effect on the sum/comparison being tracked, and that guarantee only holds on sorted data. If the input is unsorted and you cannot sort it without losing information you need (like original indices), you almost always want a hash map instead — that is the "Hashing" exit on the left.

The **second fork** separates the three sorted-input outcomes: an exact pair/triplet search or an area-maximization problem is the *converging* flavor (pointers start at opposite ends and move toward each other); an in-place removal/compaction problem is the *same-direction* flavor (both pointers start together and move forward at different rates). If instead the problem wants a *contiguous run* of elements satisfying a running property (not two anchored endpoints), that is Sliding Window's territory, not Two Pointers' — the two patterns are siblings and it is easy to confuse them, so the "How to tell them apart" section in the [family README](../../README.md) and the Similar Patterns section of this module's [README](../README.md) are worth re-reading whenever you are unsure.
