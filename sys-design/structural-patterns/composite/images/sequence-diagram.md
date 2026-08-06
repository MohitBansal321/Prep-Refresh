# Composite Pattern — Sequence Diagram

Runtime message exchange for one `getSizeInBytes()` call on a small tree:

```
project/            (Composite)
├── src/            (Composite)
│   └── index.ts    (Leaf, 2048 B)
└── README.md       (Leaf, 4096 B)
```

The Client calls **one** method on the root; the recursion happens entirely inside
the tree via delegation.

```mermaid
sequenceDiagram
    autonumber
    participant C as StorageService (Client)
    participant Root as project/ (Composite)
    participant Src as src/ (Composite)
    participant F1 as index.ts (Leaf)
    participant F2 as README.md (Leaf)

    C->>Root: getSizeInBytes()
    activate Root
    Note over Root: cache miss → total = 0, iterate children

    Root->>Src: getSizeInBytes()
    activate Src
    Note over Src: cache miss → total = 0, iterate children
    Src->>F1: getSizeInBytes()
    F1-->>Src: 2048  (base case: own size)
    Note over Src: sum = 2048 → cache it
    Src-->>Root: 2048
    deactivate Src

    Root->>F2: getSizeInBytes()
    F2-->>Root: 4096  (base case: own size)

    Note over Root: sum = 2048 + 4096 = 6144 → cache it
    Root-->>C: 6144
    deactivate Root
```

**How to read it**
- The Client sends exactly **one** message (`getSizeInBytes()` to the root) and receives one number. It writes no loop and no `instanceof` check.
- Each **Composite** (`project/`, `src/`) responds by delegating the *same* message to each child, then summing — the recursive step. Note it caches its subtree total before returning.
- Each **Leaf** (`index.ts`, `README.md`) responds immediately with its own size — the base case that stops the recursion.
- Activation bars show the nested recursion: `project/` is still "active" (waiting) while `src/` computes its subtree, which in turn waits on `index.ts`. Results bubble back up the tree to the Client.
