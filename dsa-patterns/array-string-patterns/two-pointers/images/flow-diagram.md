# Two Pointers — Flow Diagram (Converging Variant)

This traces the control flow of the general converging two-pointers algorithm — the shape behind pair-sum search, 3Sum's inner loop, and container-with-most-water. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual numbers.

```mermaid
flowchart TD
    Start([Start: sorted array, a target/goal]) --> Init["Initialize left = 0<br/>right = n - 1"]
    Init --> Loop{left < right?}

    Loop -- No, pointers met/crossed --> NotFound([Return: no more pairs to check<br/>report best answer found, or 'not found'])

    Loop -- Yes --> Compute["Compute current = combine(a[left], a[right])<br/>e.g. sum, or min(height[left], height[right]) * width"]

    Compute --> Track[Optionally update the running<br/>best answer if this is an<br/>optimization problem, not exact-match]

    Track --> Compare{Compare current<br/>to target/goal}

    Compare -- "current == target<br/>(exact-match problems)" --> Found([Return: found the pair<br/>e.g. indices left, right])

    Compare -- "current too small" --> MoveLeft["left++<br/>(only increasing left can<br/>increase the combined value)"]
    Compare -- "current too large" --> MoveRight["right--<br/>(only decreasing right can<br/>decrease the combined value)"]
    Compare -- "optimization: no exact match,<br/>shorter/limiting side decided" --> MoveLimiting["Move the pointer at the<br/>limiting/shorter side inward"]

    MoveLeft --> Loop
    MoveRight --> Loop
    MoveLimiting --> Loop
```

## How to read it

The single loop condition `left < right` is the entire lifetime of the algorithm — every iteration does a fixed amount of O(1) work (one comparison, one pointer move), and the loop runs at most `n` times total because `left` and `right` are converging toward each other and never move apart. That is the whole argument for the O(n) time bound: **total work equals total pointer movement**, and total pointer movement is capped by the initial gap `n - 1`.

The branch after "Compare" is where the two sub-flavors diverge. In an **exact-match** search (Two Sum II, the inner loop of 3Sum), hitting `current == target` ends the algorithm immediately — there is nothing left to optimize, you found the unique answer. In an **optimization** search (Container With Most Water), there usually is no single "correct" stopping point — every configuration is evaluated, the best one is tracked in a running variable, and the "compare" step decides which pointer is *provably* safe to discard rather than which one to keep, because you are searching for a maximum, not a match.
