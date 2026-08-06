# Composite Pattern — Flow Diagram

Control flow of a single recursive aggregate call (`getSizeInBytes()`) through the
tree, including the leaf base case, the composite recursive step, and the size
cache fast-path.

```mermaid
flowchart TD
    Start([Client calls node.getSizeInBytes]) --> Type{Leaf or Composite?}

    Type -- "Leaf (File)" --> LeafReturn["Return own byte count<br/>BASE CASE — no recursion"]

    Type -- "Composite (Directory)" --> Cache{Valid cached size?}
    Cache -- Yes --> ReturnCached["Return cached value — O(1)"]
    Cache -- No --> Init["total = 0"]
    Init --> Loop{More children?}
    Loop -- Yes --> Delegate["child.getSizeInBytes()<br/>RECURSIVE STEP<br/>(child may be leaf or composite)"]
    Delegate --> Add["total += child result"]
    Add --> Loop
    Loop -- No --> Store["Store total in cache"]
    Store --> ReturnSum["Return total to caller"]

    LeafReturn --> End([Caller receives a single number])
    ReturnCached --> End
    ReturnSum --> End
```

**Key idea:** the composite does not care whether a child is a leaf or another
composite — it calls the **same** `getSizeInBytes()` on each and lets polymorphism
route the call. Leaves terminate the recursion; composites drive it. The cache
fast-path makes repeated reads O(1); mutations (`add`/`remove`) clear the cache up
the parent chain so the next read recomputes correctly.
