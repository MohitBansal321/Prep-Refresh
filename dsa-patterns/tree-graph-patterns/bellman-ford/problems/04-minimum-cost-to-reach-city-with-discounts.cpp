// ============================================================================
// LeetCode 2093 — Minimum Cost to Reach City With Discounts
// https://leetcode.com/problems/minimum-cost-to-reach-city-with-discounts/
// ============================================================================
//
// PROBLEM
// -------
// n cities, highways[i] = (u, v, toll) -- a bidirectional road with a toll.
// You have `discounts` discount cards; each card, used on one road, halves
// that road's toll (integer division). Each card can be used at most once,
// on at most one road. Starting at city 0, find the minimum total cost to
// reach city n-1, using at most `discounts` halved tolls along the way.
//
// APPROACH -- Bellman-Ford relaxation bounded by a resource, with a BINARY
// per-edge choice instead of a single "take it or not"
// -------------------------------------------------------------------------
// Like LeetCode 787 (bounded by stops) and 1928 (bounded by time), this
// problem bounds the relaxation by a resource: the number of discount cards
// remaining. The state is dist[node][discountsUsed] -- the minimum cost to
// reach `node` having already spent exactly `discountsUsed` cards.
//
// The relaxation rule for each edge (u, v, toll) now has TWO variants
// instead of one, because at each edge you can independently choose to pay
// full price or spend one more discount card:
//   dist[v][d]     can improve from dist[u][d] + toll            (no discount)
//   dist[v][d + 1] can improve from dist[u][d] + toll / 2         (discount)
// (and symmetrically for u from v, since the road is undirected). This is
// the same "relax every edge against every reachable state" idea as plain
// Bellman-Ford, but the per-edge relaxation itself has branched into two
// rules instead of one -- the facet this problem specifically exercises.
//
// Time:  O(discounts * E) -- one relaxation attempt per (discount-count,
//        edge) pair, run for enough "rounds" to let improvements propagate
//        (bounded above by discounts+1 rounds, since only `discounts`
//        halvings are ever available).
// Space: O(V * discounts) for the dp table.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

int minimumCost(int n, const std::vector<std::vector<int>>& highways,
                 int discounts) {
  const long long kInf = std::numeric_limits<long long>::max() / 2;

  // dist[u][d]: minimum cost to reach city u having used exactly d discounts.
  std::vector<std::vector<long long>> dist(
      n, std::vector<long long>(discounts + 1, kInf));
  dist[0][0] = 0;

  // Relax repeatedly: a shortest path uses at most n-1 edges, so n-1 full
  // sweeps over every (edge, discount-level) combination is enough for the
  // improvements to fully propagate -- the same V-1 bound Bellman-Ford
  // always relies on, just applied per discount level.
  for (int round = 0; round < n - 1; ++round) {
    bool changed = false;
    for (const auto& h : highways) {
      int u = h[0], v = h[1], toll = h[2];
      for (int d = 0; d <= discounts; ++d) {
        if (dist[u][d] == kInf && dist[v][d] == kInf) continue;

        // No discount, u -> v and v -> u (undirected).
        if (dist[u][d] != kInf && dist[u][d] + toll < dist[v][d]) {
          dist[v][d] = dist[u][d] + toll;
          changed = true;
        }
        if (dist[v][d] != kInf && dist[v][d] + toll < dist[u][d]) {
          dist[u][d] = dist[v][d] + toll;
          changed = true;
        }

        // Spend one discount card on this edge, if any remain.
        if (d + 1 <= discounts) {
          long long halved = toll / 2;
          if (dist[u][d] != kInf && dist[u][d] + halved < dist[v][d + 1]) {
            dist[v][d + 1] = dist[u][d] + halved;
            changed = true;
          }
          if (dist[v][d] != kInf && dist[v][d] + halved < dist[u][d + 1]) {
            dist[u][d + 1] = dist[v][d] + halved;
            changed = true;
          }
        }
      }
    }
    if (!changed) break;
  }

  long long best = kInf;
  for (int d = 0; d <= discounts; ++d) {
    best = std::min(best, dist[n - 1][d]);
  }
  return best >= kInf ? -1 : static_cast<int>(best);
}

// ============================================================================
// main() -- printed, verifiable output.
// ============================================================================

int g_pass = 0;
int g_fail = 0;

void check(bool condition, const std::string& label) {
  if (condition) {
    std::cout << "[PASS] " << label << "\n";
    ++g_pass;
  } else {
    std::cout << "[FAIL] " << label << "\n";
    ++g_fail;
  }
}

int main() {
  {
    // A single road, one discount card available: the full toll (10) can
    // be halved to 5.
    std::vector<std::vector<int>> highways = {{0, 1, 10}};
    check(minimumCost(2, highways, 1) == 5,
          "one road, one discount -> toll halved from 10 to 5");
  }

  {
    // Same road, zero discounts -- must pay full price.
    std::vector<std::vector<int>> highways = {{0, 1, 10}};
    check(minimumCost(2, highways, 0) == 10,
          "one road, zero discounts -> pay the full toll");
  }

  {
    // Two roads in series, one discount card: spending it on the pricier
    // road (10 -> 5) beats spending it on the cheaper one (4 -> 2), since
    // 5+4=9 beats 10+2=12.
    std::vector<std::vector<int>> highways = {{0, 1, 10}, {1, 2, 4}};
    check(minimumCost(3, highways, 1) == 9,
          "one discount spent on the pricier edge (10->5) beats spending it "
          "on the cheaper one (4->2)");
  }

  {
    // Two roads in series, two discounts -- both edges get halved.
    std::vector<std::vector<int>> highways = {{0, 1, 10}, {1, 2, 4}};
    check(minimumCost(3, highways, 2) == 7,
          "two discounts -> both edges halved: 5 + 2 = 7");
  }

  {
    // Unreachable destination -> -1.
    std::vector<std::vector<int>> highways = {{0, 1, 5}};
    check(minimumCost(3, highways, 1) == -1,
          "city 2 is unreachable -> -1");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
