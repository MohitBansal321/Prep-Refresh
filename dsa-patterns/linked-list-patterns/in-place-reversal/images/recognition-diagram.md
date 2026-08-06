# Recognition Diagram — In-place Reversal

Use this diagram when you are staring at a new problem and trying to decide whether it fits the In-place Reversal pattern, or whether it actually needs a different tool.

```mermaid
flowchart TD
    Start([Read the problem statement]) --> Q1{Is the input a singly<br/>linked list?}

    Q1 -- No --> OtherStructure[Probably a different family —<br/>array/string, tree/graph, etc.]

    Q1 -- Yes --> Q2{Does the problem ask you to<br/>REVERSE the list, a sub-range<br/>of it, or groups of k nodes?}

    Q2 -- No, it asks about a<br/>cycle, a repeat, or a<br/>midpoint --> FastSlow[Use Fast and Slow Pointers<br/>see ../fast-slow-pointers/]

    Q2 -- No, it just asks to<br/>read/search/count values --> PlainTraversal[A single forward traversal<br/>is enough — no rewiring needed]

    Q2 -- Yes --> Q3{Must it run in O(1)<br/>extra space, i.e. no new<br/>list, no array, no recursion<br/>stack proportional to n?}

    Q3 -- No, O(n) space<br/>is acceptable --> CopyApproach["Copying into a<br/>vector/new list also works —<br/>In-place Reversal is still<br/>better, but not strictly required"]

    Q3 -- Yes --> Q4{Which shape of reversal?}

    Q4 -- Whole list --> WholeList[reverseList: one prev/curr/next<br/>pass, no dummy node needed]
    Q4 -- A sub-range left..right --> SubRange[reverseBetween: prev/curr/next<br/>PLUS a dummy head to handle<br/>left == 1 uniformly]
    Q4 -- Groups of k nodes --> KGroup[reverseKGroup: repeat the<br/>sub-range reversal group by<br/>group, checking k nodes remain<br/>before reversing each group]

    style WholeList fill:#2f6f4f,color:#fff
    style SubRange fill:#2f6f4f,color:#fff
    style KGroup fill:#2f6f4f,color:#fff
```

## How to read it

Start at the top and answer each diamond honestly before moving on — this is a decision tree, not a checklist to skim. The first gate (`Q1`) filters out anything that is not fundamentally a chain of `next` pointers; In-place Reversal has no purchase on a random-access array, because the whole technique is *rewiring pointers*, and an array has no pointers to rewire — you would use in-place index-swapping there instead (a different, simpler technique).

The second gate (`Q2`) is the actual recognition signal: the phrase to listen for is "reverse," "reversed order," or "in groups of k." If the ask is instead "detect a cycle," "find the middle," or "does it repeat," that is the sibling pattern's job — **Fast & Slow Pointers** — not this one. It is easy to conflate the two early on because both patterns live in the same family and both use a small, fixed number of pointer variables; the test is *what question the problem is asking*, not *how many pointers you'll use*.

The third gate (`Q3`) is where In-place Reversal earns its keep over the naive alternative: copying values into a `std::vector`, reversing that, and rebuilding a new list (or a new sub-list) also works, but costs O(n) extra space. If the problem explicitly says "O(1) space" — a very common constraint on linked-list reversal problems — that phrasing is itself the strongest signal to reach for this pattern.

The final branch (`Q4`) is not about "which pattern" anymore — it is about *which variant of the same pattern*. All three variants (whole list, sub-range, k-group) use the identical three-pointer rewiring loop at their core; they differ only in **where the loop starts, where it stops, and how the two ends get reconnected to the rest of the list**. Recognizing which of the three shapes a problem needs, and reaching for a dummy head node whenever the reversal might include the true head (sub-range and k-group both need this; whole-list reversal does not), is the actual skill this pattern teaches beyond the basic loop.
