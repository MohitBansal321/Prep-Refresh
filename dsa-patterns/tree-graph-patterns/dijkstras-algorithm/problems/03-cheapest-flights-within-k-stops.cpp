// ============================================================================
// LeetCode 787 — Cheapest Flights Within K Stops
// ============================================================================
//
// PROBLEM
// -------
// There are `n` cities connected by directed flights; flights[i] =
// (src_i, dst_i, price_i). Find the cheapest price from `src` to `dst` with
// at most `k` stops in between (a "stop" is an intermediate city — a direct
// flight has 0 stops, one layover has 1 stop, so at most k+1 edges). If no
// such route exists, return -1.
//
// Example: n = 3, flights = [(0,1,100),(1,2,100),(0,2,500)],
//          src = 0, dst = 2, k = 1  ->  200   (route 0->1->2)
//          same but k = 0           ->  500   (direct only)
//
// APPROACH — State-augmented Dijkstra over (node, stopsUsed)
// -----------------------------------------------------------
// WHY PLAIN DIJKSTRA FAILS HERE (the whole lesson of this problem):
//
// Dijkstra's core guarantee is that a node's distance is FINAL when it pops.
// The proof relies on nothing else mattering about a path except its cost.
// But here a second property matters: how many edges the path used. Consider:
//
//        0 --10--> 2      (0 edges, cost 10)   <- plain Dijkstra finalizes
//        0 --1--> 1       node 2 at cost 10 on first pop...
//        1 --1--> 2       ...but with k >= 1, 0->1->2 costs 2!
//
// Plain Dijkstra would finalize node 2 at distance 10 the moment it pops,
// never discovering that a cheaper route exists within the hop budget. The
// cheapest-overall path can use MORE hops than allowed while a pricier
// within-budget path also exists — and both must coexist during the search.
// A single dist-per-node cannot hold two candidate values at once.
//
// THE FIX: augment the state. Run Dijkstra not over nodes but over
// (node, stopsUsed) pairs. dist[v][s] = cheapest way to reach v using exactly
// s edges. Now both candidates live separately as (2, 0) -> 10 and (2, 1) ->
// 2, each finalized independently on its own pop. The answer is the minimum
// of dist[dst][s] over all allowed s <= k+1. Weights stay non-negative, so
// the greedy pop-order argument still holds *within* this enlarged state
// graph: states pop in non-decreasing cost order, so each (node, stops)
// state is final when popped usefully. (Note we do NOT need monotone stops
// ordering — only cost ordering drives correctness.)
//
// Implementation detail: because every edge increases stopsUsed by exactly
// 1, relaxation goes from layer s to layer s+1 only. We prune any push whose
// new stops count exceeds k+1, bounding the heap to O(k*V) states.
//
// COMPLEXITY
// ----------
// Time:  O(E * K log(K*V)) — each of the E edges is relaxed once per source
//        layer s <= K, each push/pop costing log of the heap size (at most
//        K*V + 1 entries).
// Space: O(K*V + E) — the layered dist table plus the heap.
//
// Contrast: Bellman-Ford limited to k+1 rounds solves this too, in
// O(K*E) time without a heap — worth knowing, but the augmented-state view
// generalizes better to other budgets (fuel, time windows, tickets left).
// ============================================================================

#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

const int kInf = std::numeric_limits<int>::max();

typedef std::vector<std::vector<std::pair<int, int> > > AdjList;

int findCheapestPrice(int n, const std::vector<std::vector<int> >& flights,
                      int src, int dst, int k) {
  // Build adjacency: adj[u] = list of {v, price} for each flight u -> v.
  AdjList adj(n);
  for (size_t i = 0; i < flights.size(); ++i) {
    const int u = flights[i][0];
    const int v = flights[i][1];
    const int w = flights[i][2];
    adj[u].push_back(std::make_pair(v, w));
  }

  // Layered distances: dist[node][stops] = cheapest cost reaching `node`
  // using exactly `stops` edges. At most k+1 edges are allowed (k stops in
  // between), so layers run 0..k+1.
  const int maxStops = k + 1;
  std::vector<std::vector<int> > dist(n, std::vector<int>(maxStops + 1, kInf));
  dist[src][0] = 0;

  // Min-heap of {cost, node, stopsUsed}; greater<> makes it a min-heap keyed
  // on cost first — cost order is what preserves Dijkstra's correctness.
  std::priority_queue<std::tuple<int, int, int>,
                      std::vector<std::tuple<int, int, int> >,
                      std::greater<std::tuple<int, int, int> > >
      pq;
  pq.push(std::make_tuple(0, src, 0));

  while (!pq.empty()) {
    const int d = std::get<0>(pq.top());
    const int u = std::get<1>(pq.top());
    const int s = std::get<2>(pq.top());
    pq.pop();
    // Stale-entry check, same as always: dist[][] only ever improves, so
    // popping a cost above dist[u][s] means a better entry already ran.
    if (d > dist[u][s]) continue;

    // First USEFUL arrival at dst pops at minimum cost over every allowed
    // stops count — the greedy pop-order guarantee extends to the augmented
    // state graph because all edge weights are still non-negative.
    if (u == dst) return d;

    // Relax into the NEXT stop layer only: taking an edge always spends one
    // stop budget unit.
    if (s == maxStops) continue;  // no budget left to take another edge
    for (size_t i = 0; i < adj[u].size(); ++i) {
      const int v = adj[u][i].first;
      const int w = adj[u][i].second;
      const int nd = d + w;
      if (nd < dist[v][s + 1]) {
        dist[v][s + 1] = nd;
        pq.push(std::make_tuple(nd, v, s + 1));
      }
    }
  }
  return -1;  // dst never popped within the budget
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](bool condition, const std::string& label) {
    if (condition) {
      std::cout << "[PASS] " << label << "\n";
      ++pass_count;
    } else {
      std::cout << "[FAIL] " << label << "\n";
      ++fail_count;
    }
  };

  {
    // Classic example: 2-hop route (cost 200) beats direct (cost 500),
    // and k=1 permits exactly one intermediate stop.
    std::vector<std::vector<int> > flights;
    flights.push_back({0, 1, 100});
    flights.push_back({1, 2, 100});
    flights.push_back({0, 2, 500});
    check(findCheapestPrice(3, flights, 0, 2, 1) == 200,
          "k=1 allows the cheap two-hop route -> 200");
  }

  {
    // Same graph, k=0: the two-hop route is now FORBIDDEN even though it is
    // cheaper — this is precisely where plain Dijkstra gives 200 (wrong);
    // state augmentation correctly returns 500.
    std::vector<std::vector<int> > flights;
    flights.push_back({0, 1, 100});
    flights.push_back({1, 2, 100});
    flights.push_back({0, 2, 500});
    check(findCheapestPrice(3, flights, 0, 2, 0) == 500,
          "k=0 forbids the cheaper two-hop route -> 500");
  }

  {
    // The motivating counterexample from the APPROACH section: plain
    // Dijkstra would finalize node 2 at cost 10 via the 0-edge direct
    // flight; the true answer uses the budget on 0->1->2.
    std::vector<std::vector<int> > flights;
    flights.push_back({0, 2, 10});
    flights.push_back({0, 1, 1});
    flights.push_back({1, 2, 1});
    check(findCheapestPrice(3, flights, 0, 2, 5) == 2,
          "cheaper multi-stop route beats pricier direct flight -> 2");
    check(findCheapestPrice(3, flights, 0, 2, 0) == 10,
          "same graph with k=0 forces the pricier direct flight -> 10");
  }

  {
    // Destination unreachable within budget: -1.
    std::vector<std::vector<int> > flights;
    flights.push_back({0, 1, 5});
    check(findCheapestPrice(3, flights, 0, 2, 3) == -1,
          "no route at all -> -1");
  }

  {
    // Single city: src == dst with zero flights — trivially cost 0.
    std::vector<std::vector<int> > flights;
    check(findCheapestPrice(1, flights, 0, 0, 0) == 0,
          "single city, already there -> 0");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
