# Trie — Flow Diagram (Insert / Search Control Flow)

This traces the control flow shared by all three core operations — `insert`, `search`, and `startsWith` — which differ only in what they do when the walk completes. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual words.

```mermaid
flowchart TD
    Start(["Start: operation(word), cursor = root"]) --> Next{More characters<br/>left in the string?}

    Next -- No, walked the whole string --> Complete{"Which operation?"}
    Complete -- "insert" --> MarkFlag["Set node->isWord = true<br/>(the path now ends a real word)"]
    Complete -- "search" --> CheckFlag{node->isWord?}
    CheckFlag -- Yes --> Found([Return true: word exists])
    CheckFlag -- No --> PrefixOnly([Return false: path exists but<br/>only as a PREFIX of another word])
    Complete -- "startsWith" --> Exists([Return true: the prefix path exists])

    Next -- Yes --> Idx["idx = c - 'a'"]
    Idx --> Child{children[idx] exists?}

    Child -- No --> Op{"Which operation?"}
    Op -- "insert" --> Create["Create new Node,<br/>link it as children[idx],<br/>advance cursor to it"]
    Create --> Next
    Op -- "search / startsWith" --> Fail([Return false: path breaks —<br/>string not present])

    Child -- Yes --> Advance["cursor = children[idx]"]
    Advance --> Next
```

## How to read it

All three operations are the same walk with different endings — that is why the private helper `findNode` in [code.cpp](../code.cpp) exists: it performs the loop (`Next -> Idx -> Child -> Advance`) once, and each public method interprets the result. `insert` never fails: a missing child is created on the spot, so after L characters exactly L nodes exist along the path (some freshly allocated, some shared with earlier words). `search` and `startsWith` treat a missing child as immediate failure — the string cannot be present if its character-path was never built.

The two return points worth internalizing are **CheckFlag** and **Fail**. `Fail` answers "is even the prefix absent?" — it is what `startsWith` returns on. `CheckFlag` answers the subtler question: the path may fully exist because some *longer* word passes through it (`"app"` inside `"apple"`), so `search` must additionally demand the end-of-word flag. Confusing these two exit points is the single most common Trie bug — see "Two Facts People Get Wrong" in the [cheatsheet](../cheatsheet.md).

The complexity argument lives entirely in the loop: every iteration consumes exactly one character and does O(1) work (one array index, one pointer dereference), and the loop runs at most L times where L is the string's own length. Nothing in the loop ever touches the rest of the dictionary — that independence from dictionary size is the property hashing cannot offer.
