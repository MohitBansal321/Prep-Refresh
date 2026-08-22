# Monotonic Stack/Queue — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Monotonic Stack/Queue across its two flavors (the monotonic **stack** for next-greater/smaller questions and derived quantities like spans and histogram areas, and the monotonic **deque** for fixed-size sliding-window maxima). Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-next-greater-element-i.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Next Greater Element I | [496](https://leetcode.com/problems/next-greater-element-i/) | Easy | One monotonic-stack pass over `nums2` resolves every element's next greater; a hash map then answers each `nums1` query in O(1). | O(n + m) time, O(n) space | [01-next-greater-element-i.cpp](01-next-greater-element-i.cpp) |
| Daily Temperatures | [739](https://leetcode.com/problems/daily-temperatures/) | Medium | Same stack, but the answer written on pop is the index *distance* to the resolving day rather than its value. | O(n) time, O(n) space | [02-daily-temperatures.cpp](02-daily-temperatures.cpp) |
| Largest Rectangle in Histogram | [84](https://leetcode.com/problems/largest-rectangle-in-histogram/) | Hard | Monotonic increasing stack of bars; when a bar pops, its rectangle spans between the new top (nearest smaller left) and the current bar (nearest smaller right). | O(n) time, O(n) space | [03-largest-rectangle-in-histogram.cpp](03-largest-rectangle-in-histogram.cpp) |
| Sliding Window Maximum | [239](https://leetcode.com/problems/sliding-window-maximum/) | Hard | Monotonic DEQUE of indices with decreasing values; front is the window max, back-pops remove dominated entries, front-eviction removes expired ones. | O(n) time, O(k) space | [04-sliding-window-maximum.cpp](04-sliding-window-maximum.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md):
- **01** is the textbook next-greater problem in its purest form — the base template from [code.cpp](../code.cpp), composed with a hash map for the two-array lookup wrapper.
- **02** shows that the "answer" written at pop time can be a *distance* (or any function of the two indices), not just a value — the smallest possible change to the template.
- **03** is the canonical hard application: the pop step computes a rectangle whose width comes from the *new stack top* after popping, demonstrating why the stack must hold indices and how nearest-smaller-on-both-sides falls out of one scan.
- **04** is the deque flavor — sliding-window max — including both the back-pop (domination) and front-evict (window expiry) rules, the off-by-one hotspot called out in the README's Common Mistakes.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
