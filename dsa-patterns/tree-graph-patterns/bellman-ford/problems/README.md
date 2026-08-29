# Bellman-Ford — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating Bellman-Ford's core relaxation loop and its most important real-world variant: bounding the number of relaxation passes by a resource other than "every simple path," instead of always running the full `V - 1` passes. Each file is self-contained: compile and run it directly to see printed `[PASS]`/`[FAIL]` output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-network-delay-time.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Network Delay Time | [743](https://leetcode.com/problems/network-delay-time/) | Medium | Textbook `V-1`-pass relaxation with no bound and no negative weights, confirming the answer matches what Dijkstra would give. | O(V·E) time, O(V + E) space | [01-network-delay-time.cpp](01-network-delay-time.cpp) |
| Cheapest Flights Within K Stops | [787](https://leetcode.com/problems/cheapest-flights-within-k-stops/) | Medium | Bound the pass count to `K+1` (not `V-1`) so the algorithm computes the cheapest cost reachable using at most `K+1` edges. | O(K·E) time, O(V) space | [02-cheapest-flights-within-k-stops.cpp](02-cheapest-flights-within-k-stops.cpp) |
| Minimum Cost to Reach Destination in Time | [1928](https://leetcode.com/problems/minimum-cost-to-reach-destination-in-time/) | Hard | Bound the relaxation by a continuous resource (total time spent) instead of an edge count, tracking `dp[time][node]`. | O(maxTime·E) time, O(maxTime·V) space | [03-minimum-cost-to-reach-destination-in-time.cpp](03-minimum-cost-to-reach-destination-in-time.cpp) |
| Minimum Cost to Reach City With Discounts | [2093](https://leetcode.com/problems/minimum-cost-to-reach-city-with-discounts/) | Medium | Bound the relaxation by a discount-card budget, with each edge offering a binary choice (pay full toll or spend a discount). | O(discounts·E) time, O(V·discounts) space | [04-minimum-cost-to-reach-city-with-discounts.cpp](04-minimum-cost-to-reach-city-with-discounts.cpp) |

## Why these four

They cover every recognition signal called out in the [README](../README.md), with one honest caveat: LeetCode rarely poses a graph problem with genuinely negative edge weights (real-world negative-weight scenarios — arbitrage, refunds, credits — are more of a systems/finance domain than an interview-question domain), so negative-weight handling and negative-cycle detection are exercised thoroughly in [code.cpp](../code.cpp)'s own test suite instead. What LeetCode *does* test heavily is Bellman-Ford's other defining property: bounding the relaxation to a resource budget smaller than "every simple path."

- **01** is the pure, unbounded baseline — no resource limit, no negative weights — included specifically to show that Bellman-Ford's `V-1`-pass loop reaches the identical answer Dijkstra would, at a real asymptotic cost (`O(V·E)` vs `O((V+E) log V)`), establishing what you are paying for when you reach for the slower, more general algorithm.
- **02** is the canonical bounded variant: replacing `V-1` passes with `K+1` passes turns "shortest path" into "shortest path using at most `K+1` edges," and demonstrates the critical implementation detail of relaxing against a *snapshot* of the previous pass rather than mutating in place — get this wrong and a single pass silently chains more edges than the budget allows.
- **03** generalizes the bound from a discrete edge count to a continuous resource (time), showing the same relaxation idea scales to `dp[resource][node]` state without changing its underlying logic.
- **04** generalizes again to a resource with a *binary per-edge choice* (spend a discount here or not), the facet where a single edge's relaxation rule itself branches into two cases instead of one.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
