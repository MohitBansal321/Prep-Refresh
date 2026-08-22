# Monotonic Stack/Queue — Flow Diagram (Pop-While-Beaten Control Flow)

This traces the control flow of the general monotonic-stack scan — the shape behind next-greater-element, daily temperatures, stock spans, and largest-rectangle-in-histogram. The deque variant is the same skeleton with one extra eviction step. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual numbers.

```mermaid
flowchart TD
    Start([Start: array, a direction and a comparison]) --> Init["Initialize result array filled with the<br/>'no answer' sentinel (e.g. -1)<br/>empty stack holding INDICES"]
    Init --> Loop{More elements<br/>to scan?}

    Loop -- No --> Drain["Stack still holds indices whose answer<br/>was never found — they keep their sentinel.<br/>Return result."]
    Drain --> Done([Done])

    Loop -- "Yes, current index i" --> Pop{Stack non-empty AND<br/>nums[stack.back()] is BEATEN<br/>by nums[i]?}

    Pop -- Yes --> Resolve["POP stack.back():<br/>the incoming element IS its answer.<br/>Write result[popped] = answer<br/>(value, distance, or area contribution)."]
    Resolve --> Pop

    Pop -- "No — stack top survives<br/>(or stack empty)" --> Push["Push i onto the stack.<br/>Monotonicity of the stack is restored."]
    Push --> Loop
```

## How to read it

The entire algorithm lives in the **inner pop loop**: while the incoming element beats the stack's top, pop and resolve. Each popped element has just found its final answer — the incoming element is its next greater (or smaller) element — and it can *never* be useful again, because anything that would have resolved it later is farther away than the element that just resolved it. That is why pops are permanent and why the pop condition must be checked in a `while`, not an `if`: one incoming element can resolve many waiting entries at once.

The amortized O(n) argument reads directly off the diagram: every iteration of the outer loop performs exactly **one push** (the `Push` node), and each index can be popped at most once because popping removes it permanently. So across the whole run there are at most n pushes and n pops — total work ≤ 2n — even though any single inner loop could pop many elements. The cost of the inner loop is charged to the pops, not to the outer iterations.

The **deque variant** inserts two extra steps between `Push` and `Loop`: after pushing index `i`, evict from the FRONT if `dq.front() <= i - k` (it has slid out of the fixed-size window), and only then, if the window is full (`i >= k - 1`), record `nums[dq.front()]` as the current window's max. Everything else — including the amortized argument — is identical.
