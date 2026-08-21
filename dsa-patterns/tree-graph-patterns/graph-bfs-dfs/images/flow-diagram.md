# Graph BFS/DFS — Flow Diagram

This traces the control flow of both guarded traversals side by side — BFS's queue loop on the left, DFS's recursion on the right — so the one structural difference between them is visible in a single picture. See [trace-diagram.md](trace-diagram.md) for a concrete worked example with actual node numbers.

```mermaid
flowchart TD
    Start([Graph-wide entry point]) --> OuterLoop{"For each node s in 0..V-1:<br/>is s still unvisited?"}

    OuterLoop -- "No -- already reached by<br/>an earlier traversal" --> NextNode[Skip to the next s]
    NextNode --> OuterLoop

    OuterLoop -- "Yes -- a node nothing has<br/>reached yet: a NEW component" --> Which{Which traversal does<br/>the question need?}

    OuterLoop -- "Loop exhausted" --> Finish([Every node visited exactly once<br/>-- component count / cycle verdict<br/>/ distance array is complete])

    %% ---------------- BFS branch ----------------
    Which -- "BFS -- the question says<br/>shortest / minimum / fewest hops" --> BInit["Mark s visited AND set<br/>dist[s] = 0.<br/>Push s onto a FIFO queue"]

    BInit --> BLoop{Queue empty?}
    BLoop -- Yes --> OuterLoop

    BLoop -- No --> BPop["Pop the FRONT node u<br/>-- FIFO order is the whole<br/>mechanism: every node at<br/>distance d is popped before<br/>any node at distance d+1"]

    BPop --> BNeighbors{"For each neighbour v of u:<br/>is v already visited?"}

    BNeighbors -- "Yes -- reached earlier,<br/>so via an equal-or-shorter path" --> BSkip[Skip v entirely]
    BSkip --> BNeighbors

    BNeighbors -- No --> BDiscover["MARK v VISITED and set<br/>dist[v] = dist[u] + 1<br/>*** in the same step as ***<br/>pushing v onto the queue"]

    BDiscover --> BNeighbors
    BNeighbors -- "Neighbours exhausted" --> BLoop

    %% ---------------- DFS branch ----------------
    Which -- "DFS -- reachability, component<br/>count, cycle detection" --> DEnter["Enter node u:<br/>MARK u VISITED immediately,<br/>before looking at any neighbour"]

    DEnter --> DState{"Does this problem need to know<br/>whether u is on the CURRENT path,<br/>not merely visited at some point?<br/>-- directed cycle detection"}

    DState -- Yes --> DInProgress["Use a 3-state marker:<br/>set state[u] = IN-PROGRESS<br/>(not just 'visited')"]
    DState -- No --> DPlain["A plain boolean visited[u] = true<br/>is sufficient"]

    DInProgress --> DNeighbors
    DPlain --> DNeighbors{"For each neighbour v of u:"}

    DNeighbors -- "v is IN-PROGRESS<br/>(directed graphs only)" --> DCycle[["BACK EDGE -> a cycle exists.<br/>Report and unwind"]]

    DNeighbors -- "v unvisited" --> DRecurse["Recurse into v<br/>-- one more frame on the<br/>call stack; depth can reach V<br/>on a chain-shaped graph"]
    DRecurse --> DNeighbors

    DNeighbors -- "v already visited / DONE" --> DSkipV[Skip v -- already accounted for]
    DSkipV --> DNeighbors

    DNeighbors -- "Neighbours exhausted" --> DDone["If using the 3-state marker:<br/>set state[u] = DONE<br/>-- u is no longer on the<br/>current path"]

    DDone --> DReturn([Return to the caller<br/>-- backtrack one frame])
    DReturn --> OuterLoop
```

## How to read it

**The outer loop is the frame around everything, and it is the piece most often left out.** Both traversals sit *inside* a loop over every node as a potential start. A single BFS or DFS from one node reaches exactly one connected component and no more, so any question with a graph-wide answer — how many components, does a cycle exist anywhere, is every node reachable — is wrong without it. The failure is silent: no crash, no exception, just an under-count. This is the loop `dfsConnectedComponents` and `hasCycleDirected` in [../code.cpp](../code.cpp) both wrap around their traversals, and the one `bfsShortestPath` deliberately does *not* have, because "distances from one specific source" is a single-source question by definition.

**On the BFS side, the single load-bearing box is "mark visited and set the distance in the same step as pushing."** Split that box in two — push now, mark when popped — and two different in-flight nodes that both point at the same undiscovered neighbour will both push it before either marks it. The node is then processed twice, the queue holds duplicates, and in a distance-counting context the second push can write a distance that came from a longer route. Mark on **discovery**, never on processing. The FIFO pop is the other load-bearing detail, and it is the entire proof of the shortest-path guarantee: because the queue is first-in-first-out and every push adds exactly one to the distance, nodes come out in non-decreasing distance order, so the first time a node is reached is via a shortest route. Swap the queue for a stack and you have DFS — that one substitution is the whole difference, and it is exactly what destroys the guarantee.

**On the DFS side, the interesting fork is the three-state question.** Most DFS problems need nothing more than a boolean: reachability, component counting, flood fill ([problems/01-number-of-islands.cpp](../problems/01-number-of-islands.cpp), [problems/02-number-of-provinces.cpp](../problems/02-number-of-provinces.cpp)) all just need "have I been here." Directed cycle detection is the exception, and the reason is specific: in a directed graph, arriving at an already-visited node is completely normal — a diamond `0->1, 0->2, 1->3, 2->3` reaches node 3 twice with no cycle anywhere. What proves a cycle is arriving at a node that is still **on the current path**, which a boolean cannot express. Hence `unvisited` / `in-progress` / `done`, with `in-progress` set on entry and cleared to `done` on exit — the marker is a mirror of the recursion stack itself.

**Both branches converge on "every node visited exactly once," and that is where the O(V + E) bound comes from.** The visited guard is not an optimisation layered on top of a working algorithm; it is what makes the algorithm terminate at all on a graph with a cycle, and it is simultaneously what bounds the total work: V node entries, plus each edge examined at most once per endpoint. Remove it and you do not get a slower correct traversal — you get a BFS whose queue never empties or a DFS that overflows the stack.

**Note also what the diagram does *not* show: any notion of distance on the DFS side.** There is no `dist[]` box in the right-hand branch, and that absence is the answer to "why not just use DFS for shortest path." DFS knows how deep it currently is; it does not and cannot know that the depth it arrived at a node with is the minimum. See [problems/04-word-ladder.cpp](../problems/04-word-ladder.cpp) for the concrete case where that difference turns a correct answer into a wrong one.
