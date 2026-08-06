// LeetCode 322 — Coin Change
//
// Problem: given coin denominations `coins` (each denomination available in
// unlimited supply) and a target `amount`, return the FEWEST number of coins
// needed to make up exactly `amount`. Return -1 if `amount` cannot be made
// with any combination of the given coins.
//
// Why this is Unbounded Knapsack: each coin is an "item" that can be reused
// any number of times, and `amount` is the capacity we must fill EXACTLY
// (not "at most", like a maximize-value knapsack — an exact-sum problem
// needs a sentinel for "unreachable" instead of a 0 floor). dp[w] = minimum
// coins to make exact sum w. The recurrence
//     dp[w] = min over every coin c <= w of ( dp[w - c] + 1 )
// reads dp[w - c] AFTER it has already potentially been updated using this
// same coin c earlier in the same forward pass over w — which is exactly
// what allows a coin to be reused. Fill order is capacity w increasing from
// 1 to amount, coins innermost — the same "reuse via forward fill" shape as
// unboundedKnapsack() in ../code.cpp.
//
// Complexity: O(amount * coins.size()) time, O(amount) space.

#include <climits>
#include <iostream>
#include <string>
#include <vector>

int coinChange(const std::vector<int>& coins, int amount) {
    const int UNREACHABLE = INT_MAX / 2; // avoid overflow on dp[w - c] + 1
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

namespace {
int g_pass = 0;
int g_fail = 0;

void check(const std::string& label, int actual, int expected) {
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
    check("coins=[1,2,5], amount=11 (5+5+1)", coinChange({1, 2, 5}, 11), 3);
    check("coins=[2], amount=3 (unreachable)", coinChange({2}, 3), -1);
    check("coins=[1], amount=0", coinChange({1}, 0), 0);
    check("coins=[1,3,4], amount=6 (3+3)", coinChange({1, 3, 4}, 6), 2);
    check("coins=[186,419,83,408], amount=6249", coinChange({186, 419, 83, 408}, 6249), 20);

    std::cout << "\n" << g_pass << " passed, " << g_fail << " failed\n";
    return g_fail == 0 ? 0 : 1;
}
