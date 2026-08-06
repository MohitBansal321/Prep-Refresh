# Union Find — Flow Diagram (find with path compression, union by rank)

This traces the control flow of the two core operations — `find(x)` and `unionSets(x, y)` — and how they compose into "process one edge." See [trace-diagram.md](trace-diagram.md) for a concrete worked example with an actual forest of nodes.

```mermaid
flowchart TD
    subgraph FIND["find(x) -- with path compression"]
        FStart([Start: find x]) --> FCheck{parent x == x?<br/>i.e. is x its own root?}
        FCheck -- Yes, x IS the root --> FReturn([Return x])
        FCheck -- No --> FRecurse["root = find( parent x )<br/>(recurse upward toward the root)"]
        FRecurse --> FCompress["parent x = root<br/>PATH COMPRESSION:<br/>point x directly at the root,<br/>flattening the chain for next time"]
        FCompress --> FReturn2([Return root])
    end

    subgraph UNION["unionSets(x, y) -- with union by rank"]
        UStart([Start: unionSets x, y]) --> UFindX["rootX = find(x)"]
        UFindX --> UFindY["rootY = find(y)"]
        UFindY --> USame{rootX == rootY?}
        USame -- "Yes, already connected" --> UFalse(["Return FALSE<br/>(no merge -- this edge<br/>would close a cycle)"])
        USame -- No --> URank{"compare rank[rootX]<br/>vs rank[rootY]"}
        URank -- "rank rootX is smaller" --> USwap["swap rootX and rootY<br/>(always attach the SHORTER<br/>tree under the TALLER one)"]
        URank -- "rank rootX is larger<br/>or equal" --> UAttach
        USwap --> UAttach["parent rootY = rootX<br/>(attach the smaller/shallower<br/>tree under the bigger one)"]
        UAttach --> UBump{"ranks were EQUAL?"}
        UBump -- Yes --> UIncRank["++rank rootX<br/>(tree height grew by one level)"]
        UBump -- No --> UTrue
        UIncRank --> UTrue(["Return TRUE<br/>(merge happened --<br/>one edge, two groups<br/>became one)"])
    end

    FIND -.-> |used twice per call| UNION
```

## How to read it

The `find` subgraph is a single recursive climb: if the current node is already its own root, stop; otherwise recurse upward, and — critically — on the way back down, re-point the current node **directly** at the root that was found (`FCompress`). This is path compression: every node visited during this one `find` call gets flattened, so any future `find` on any of those nodes is O(1) instead of re-walking the same chain. The cost is paid once per chain, not once per future query.

The `unionSets` subgraph always calls `find` exactly twice first — to locate both elements' current roots — before deciding anything. The `USame` diamond is the cycle-detection signal used throughout this module's worked problems: if `x` and `y` already share a root, no merge is possible, and `unionSets` returning `false` is exactly how callers (see Redundant Connection and Number of Islands II) learn "this edge is redundant" without any separate cycle-detection logic. If the roots differ, the `URank` diamond is union by rank: the tree with the smaller rank (an upper bound on height) is always attached under the taller one, and the taller tree's rank only increases when both trees were exactly the same height — attaching a shorter tree under a taller one never increases the taller tree's own height, so its rank correctly stays unchanged. This is what keeps every tree's height growing logarithmically instead of linearly, which is the other half (alongside path compression) of the near-O(1) amortized bound.
