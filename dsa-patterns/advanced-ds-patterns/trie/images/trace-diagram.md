# Trie — Trace Diagram (Worked Example)

This traces the tree shape produced by [code.cpp](../code.cpp)'s own `main()`, which inserts `"apple"` then `"app"`.

## After `insert("apple")`

```
root
 └─ a
     └─ p
         └─ p
             └─ l
                 └─ e*
```

Each letter of `"apple"` becomes one child pointer, one node deep — `a → p → p → l → e`. Only the final node (`e`) is marked `isWord = true` (shown as `*`); every node above it is a real, allocated node, but none of them represent a complete inserted word on their own.

## After `insert("app")`

```
root
 └─ a
     └─ p
         └─ p*
             └─ l
                 └─ e*
```

This is the line to stare at. Inserting `"app"` walks `a → p → p`, and at **each** of those three steps `insert` finds a child that already exists (built by the `"apple"` insert above) and reuses it — no new nodes are allocated for the shared prefix. The only change is that the node at depth 3 (the second `p`) flips from an ordinary node to `isWord = true`. `"app"` and `"apple"` now share every node down to and including that `p`; they diverge only after it.

## What `search` and `startsWith` see on this tree

| Call | Walks | Stops at | `isWord` there? | Result |
|---|---|---|---|---|
| `search("app")` | `a → p → p` | the second `p` | **yes** | `true` — a complete word |
| `search("appl")` | `a → p → p → l` | `l` | **no** | `false` — the node exists, but nothing was ever inserted that ends here |
| `search("appx")` | `a → p`, then no child `x` | fails at the second `p` | — | `false` — the path itself breaks |
| `startsWith("app")` | `a → p → p` | the second `p` | *(ignored)* | `true` — `startsWith` only asks "does this path exist," never checks `isWord` |

The `search("appl")` row is the one worth sitting with: `findNode` successfully walks all the way to the `l` node — the path exists, the node is real — but `search` still returns `false`, because `isWord` was never set there. That is the entire reason `search` and `startsWith` are two different functions sharing one helper (`findNode`) with one different check afterward, instead of one function with a flag: existence of the path and completeness of a word are different questions, and this tree answers both from the same structure without storing the strings themselves anywhere.

## How to read it

Read the two "after insert" trees in sequence and watch what does **not** change between them: every node built for `"apple"` survives untouched when `"app"` is inserted, because `"app"` is a *prefix* of `"apple"` — the new insert only ever needs to mark an existing node, never build new structure. Contrast that with inserting a word that diverges early (e.g. `"bat"`): it would branch off `root` on its own new child, sharing nothing with the `a`-rooted chain. That branching-vs-reusing distinction is the entire mechanism that gives a Trie `O(length)` lookups independent of how many words it holds — shared prefixes are stored exactly once, no matter how many words pass through them.
