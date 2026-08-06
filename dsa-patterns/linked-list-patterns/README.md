# Linked List Patterns

Linked lists forbid random access (`arr[i]`) and forbid extra memory in most interview questions ("solve it in O(1) space"). Both patterns here exist to work around exactly those two constraints using only pointer arithmetic on `next`.

| Pattern | Core idea | Typical complexity win |
|---------|-----------|-------------------------|
| [Fast & Slow Pointers](fast-slow-pointers/README.md) | Two pointers advance at different speeds (1x / 2x) through the list | O(n) space (hash set of visited nodes) → O(1) space |
| [In-place Reversal](in-place-reversal/README.md) | Rewire `next` pointers one node at a time instead of copying into a new list | O(n) space (new list) → O(1) space |

## How to tell them apart

- **Detecting a cycle, finding the middle, or finding where two things "meet"?** → Fast & Slow Pointers.
- **Reversing the whole list, or just a `[left, right]` sub-range, or reversing in groups of k?** → In-place Reversal.

They combine often: e.g. "check if a linked list is a palindrome" uses Fast & Slow to find the middle, *then* In-place Reversal on the second half.

## Recommended study order

1. **Fast & Slow Pointers** — teaches the core "two speeds meet" insight (Floyd's cycle detection), which shows up far beyond linked lists (e.g. "Happy Number" on plain integers).
2. **In-place Reversal** — a more mechanical pointer-rewiring skill; easiest to learn once you're comfortable manipulating `next` pointers from pattern #1.

Both patterns are built as Full-tier modules: **Fast & Slow Pointers** and **In-place Reversal** (see [`../INDEX.md`](../INDEX.md) for details).
