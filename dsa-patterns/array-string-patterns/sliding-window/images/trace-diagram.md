# Sliding Window — Trace Diagram

A step-by-step trace of the **Longest Substring Without Repeating Characters** problem on `s = "abcabcbb"` (indices 0-7: `a b c a b c b b`), using the classic "window is a set of characters" version of the algorithm: expand `right`; if `s[right]` is already in the window, shrink from `left` one character at a time until the duplicate is gone; then add `s[right]` and update `best`.

```mermaid
sequenceDiagram
    autonumber
    participant R as right pointer
    participant W as Window state {chars}
    participant L as left pointer
    participant B as best (longest length)

    Note over R,B: Initial: left=0, right=0, window={}, best=0

    R->>W: right=0, s[right]='a'
    Note over W: 'a' not in window -> add it. window={a}
    W->>B: len = right-left+1 = 1
    Note over B: best = 1

    R->>W: right=1, s[right]='b'
    Note over W: 'b' not in window -> add it. window={a,b}
    W->>B: len = 2
    Note over B: best = 2

    R->>W: right=2, s[right]='c'
    Note over W: 'c' not in window -> add it. window={a,b,c}
    W->>B: len = 3
    Note over B: best = 3

    R->>W: right=3, s[right]='a'
    Note over W: 'a' IS in window -> shrink from left
    W->>L: remove s[left=0]='a', left becomes 1. window={b,c}
    Note over W: duplicate gone -> add 'a'. window={b,c,a}
    W->>B: len = right(3)-left(1)+1 = 3
    Note over B: best stays 3

    R->>W: right=4, s[right]='b'
    Note over W: 'b' IS in window -> shrink from left
    W->>L: remove s[left=1]='b', left becomes 2. window={c,a}
    Note over W: duplicate gone -> add 'b'. window={c,a,b}
    W->>B: len = 4-2+1 = 3
    Note over B: best stays 3

    R->>W: right=5, s[right]='c'
    Note over W: 'c' IS in window -> shrink from left
    W->>L: remove s[left=2]='c', left becomes 3. window={a,b}
    Note over W: duplicate gone -> add 'c'. window={a,b,c}
    W->>B: len = 5-3+1 = 3
    Note over B: best stays 3

    R->>W: right=6, s[right]='b'
    Note over W: 'b' IS in window -> shrink from left (may take 2 steps)
    W->>L: remove s[left=3]='a', left becomes 4. window={b,c} - 'b' still present!
    W->>L: remove s[left=4]='b', left becomes 5. window={c}
    Note over W: duplicate gone -> add 'b'. window={c,b}
    W->>B: len = 6-5+1 = 2
    Note over B: best stays 3

    R->>W: right=7, s[right]='b'
    Note over W: 'b' IS in window -> shrink from left (may take 2 steps)
    W->>L: remove s[left=5]='c', left becomes 6. window={b} - 'b' still present!
    W->>L: remove s[left=6]='b', left becomes 7. window={}
    Note over W: duplicate gone -> add 'b'. window={b}
    W->>B: len = 7-7+1 = 1
    Note over B: best stays 3

    Note over R,B: right reached end of string. Final answer: best = 3 (the substring "abc")
```

**How to read it:** each numbered step is one iteration of `right`. Most steps are cheap — `right` grows the window by exactly one character with no shrinking (steps 1-3). The interesting steps are 4-8, where a duplicate character forces the shrink loop to run: notice at right=6 and right=7 the shrink loop runs **twice** in a single outer iteration, because one `left++` was not enough to evict the duplicate. This is exactly why the shrink step must be a `while`, not an `if` — a single-step shrink would leave a stale duplicate in the window and silently produce a wrong (too-large) answer. Also notice `best` never decreases — it only updates when a strictly longer valid window is found, which is why the final answer (3) was actually locked in as early as step 3 and every later step just fails to beat it.
