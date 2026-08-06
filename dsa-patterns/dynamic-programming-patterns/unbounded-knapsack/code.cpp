// Unbounded Knapsack — generic, problem-agnostic template.
//
// This file is NOT a solution to one specific LeetCode problem. It exists to
// show the *shape* of the pattern in isolation: a 1D dp table indexed by
// remaining capacity, filled with capacity iterating FORWARD (increasing),
// where the recurrence is allowed to reuse the same item again after taking
// it. See problems/ for four fully worked, named LeetCode solutions built on
// this same shape.
//
// Contrast with 0/1 Knapsack (../0-1-knapsack/code.cpp): the ONLY structural
// difference between "each item usable once" and "each item usable unlimited
// times" is the direction the capacity axis is filled in. Everything else —
// the dp[] array, the recurrence "take vs. don't take", the base case — is
// identical. That single fill-direction flip is the entire subject of this
// module.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out

#include <algorithm>
#include <climits>
#include <iostream>
#include <string>
#include <vector>

// unboundedKnapsack — maximize total value packed into `capacity`, where each
// item i (weight[i], value[i]) may be used any number of times (including
// zero).
//
// dp[w] = the best total value achievable with exactly-at-most capacity w,
// using any number of copies of any item.
//
// Recurrence: dp[w] = max over every item i of ( dp[w - weight[i]] + value[i] ),
// whenever weight[i] <= w, and dp[w] = 0 (no items fit) as the floor.
//
// The critical detail: dp[w - weight[i]] is read AFTER dp[] has already been
// updated for this same item at smaller capacities earlier in the SAME forward
// pass over w. That is precisely what allows item i to be "reused" — the
// value at w - weight[i] may itself already include one or more copies of
// item i. Filling w backward (as 0/1 Knapsack does) would instead read a
// value computed BEFORE item i was considered at all, guaranteeing at most
// one use per row — which is exactly the 0/1 (bounded) recurrence, not this
// one.
long long unboundedKnapsack(const std::vector<int>& weights,
                            const std::vector<long long>& values,
                            int capacity) {
    const int n = static_cast<int>(weights.size());
    std::vector<long long> dp(capacity + 1, 0);

    // Base case: dp[0] = 0 — with zero capacity, no item fits, so the best
    // achievable value is 0 (a "0 items chosen" state, not an error state).
    for (int w = 1; w <= capacity; ++w) {
        for (int i = 0; i < n; ++i) {
            if (weights[i] <= w) {
                dp[w] = std::max(dp[w], dp[w - weights[i]] + values[i]);
            }
        }
    }
    return dp[capacity];
}

// coinChangeMinCoins — the "count/minimize" sibling of the maximize-value
// recurrence above. Given coin denominations (each available in unlimited
// supply) and a target amount, return the fewest coins that sum exactly to
// amount, or -1 if amount cannot be formed at all.
//
// dp[w] = fewest coins to make EXACTLY amount w (not "at most w" — exact-sum
// problems use a sentinel for "unreachable" instead of a 0 floor).
//
// Recurrence: dp[w] = min over every coin c of ( dp[w - c] + 1 ), whenever
// c <= w. Same forward-fill-over-capacity shape as unboundedKnapsack above;
// only the aggregation (min + 1 instead of max + value) and the base/sentinel
// handling differ, because this variant asks "can we hit this sum exactly"
// rather than "what's the best value up to this capacity."
int coinChangeMinCoins(const std::vector<int>& coins, int amount) {
    const int UNREACHABLE = INT_MAX / 2; // avoid overflow when we add 1 to it
    std::vector<int> dp(amount + 1, UNREACHABLE);
    dp[0] = 0; // base case: 0 coins needed to make amount 0

    for (int w = 1; w <= amount; ++w) {
        for (int c : coins) {
            if (c <= w && dp[w - c] != UNREACHABLE) {
                dp[w] = std::min(dp[w], dp[w - c] + 1);
            }
        }
    }
    return dp[amount] == UNREACHABLE ? -1 : dp[amount];
}

// --- Tiny test harness -------------------------------------------------

namespace {

int g_pass = 0;
int g_fail = 0;

void expectEq(const std::string& label, long long actual, long long expected) {
    if (actual == expected) {
        std::cout << "[PASS] " << label << " => " << actual << "\n";
        ++g_pass;
    } else {
        std::cout << "[FAIL] " << label << " => got " << actual
                   << ", expected " << expected << "\n";
        ++g_fail;
    }
}

} // namespace

int main() {
    // unboundedKnapsack: classic rod-cutting-style example.
    // weights (piece lengths): 1, 3, 4  values (price per piece): 2, 5, 7
    // capacity (rod length): 8
    // Best per-unit value is the length-1 piece (value 2 per unit length), so
    // the optimum is eight length-1 pieces -> value 8 * 2 = 16 (beating, e.g.,
    // two length-4 pieces at 7 + 7 = 14).
    {
        std::vector<int> weights{1, 3, 4};
        std::vector<long long> values{2, 5, 7};
        expectEq("unboundedKnapsack(rod length 8)",
                 unboundedKnapsack(weights, values, 8), 16);
    }

    // unboundedKnapsack: capacity 0 -> value 0 (base case).
    {
        std::vector<int> weights{2, 3};
        std::vector<long long> values{3, 4};
        expectEq("unboundedKnapsack(capacity 0)",
                 unboundedKnapsack(weights, values, 0), 0);
    }

    // unboundedKnapsack: a single reusable item that fits many times.
    // weight 2, value 3, capacity 10 -> 5 copies -> value 15.
    {
        std::vector<int> weights{2};
        std::vector<long long> values{3};
        expectEq("unboundedKnapsack(single reusable item)",
                 unboundedKnapsack(weights, values, 10), 15);
    }

    // coinChangeMinCoins: coins [1, 2, 5], amount 11 -> 5 + 5 + 1 = 3 coins.
    {
        std::vector<int> coins{1, 2, 5};
        expectEq("coinChangeMinCoins([1,2,5], 11)",
                 coinChangeMinCoins(coins, 11), 3);
    }

    // coinChangeMinCoins: amount 0 -> 0 coins.
    {
        std::vector<int> coins{1, 2, 5};
        expectEq("coinChangeMinCoins([1,2,5], 0)",
                 coinChangeMinCoins(coins, 0), 0);
    }

    // coinChangeMinCoins: unreachable amount -> -1.
    {
        std::vector<int> coins{2};
        expectEq("coinChangeMinCoins([2], 3) unreachable",
                 coinChangeMinCoins(coins, 3), -1);
    }

    std::cout << "\n" << g_pass << " passed, " << g_fail << " failed\n";
    return g_fail == 0 ? 0 : 1;
}
