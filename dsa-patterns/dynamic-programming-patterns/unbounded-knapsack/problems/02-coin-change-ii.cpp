// LeetCode 518 — Coin Change II
//
// Problem: given coin denominations `coins` (unlimited supply of each) and a
// target `amount`, return the NUMBER OF DISTINCT COMBINATIONS of coins that
// sum to exactly `amount`. Two combinations differing only in the ORDER coins
// are used are the SAME combination (e.g. one 5 + one 2 is the same
// combination whether you "use the 5 first" or "the 2 first" — order does
// not create a new answer).
//
// Why this looks like Unbounded Knapsack but the recurrence is subtly
// different: this is a COUNTING problem, not a maximize/minimize problem,
// and it counts COMBINATIONS (unordered), not PERMUTATIONS (ordered). That
// distinction changes which loop must be on the OUTSIDE:
//
//   dp[w] += dp[w - c]   for each coin c, for each capacity w
//
// looks identical to the coin-change-minimum recurrence's shape, but the
// LOOP ORDER now matters in a way it did not for 01-coin-change.cpp or for
// unboundedKnapsack()'s max-value recurrence:
//
//   - Loop coins OUTER, amount INNER (used below): for a fixed coin c, every
//     way of reaching w - c gets extended by "add one more c" before the next
//     coin denomination is ever considered. This processes each coin
//     denomination to completion before moving to the next, which is exactly
//     what makes "5 then 2" and "2 then 5" collapse into the SAME counted
//     combination — the coin's position in the loop nesting, not its position
//     in a hypothetical sequence of picks, is what's being counted.
//   - Loop amount OUTER, coins INNER (the tempting "just copy the other
//     file's loop order" mistake): for each amount w, every coin is tried
//     fresh, including coins already used to build dp[w - c]. This ends up
//     distinguishing "use a 5 then a 2" from "use a 2 then a 5" as different
//     sequences, so it counts PERMUTATIONS, not combinations — silently the
//     WRONG answer for this problem (it overcounts).
//
// This is why Coin Change II is the canonical example, in this family, of
// "the recurrence body can look identical while the loop nesting encodes a
// completely different meaning."
//
// Complexity: O(amount * coins.size()) time, O(amount) space.

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// Correct: counts combinations. Coins outer, amount inner.
long long coinChangeIICombinations(const std::vector<int>& coins, int amount) {
    std::vector<long long> dp(amount + 1, 0);
    dp[0] = 1; // base case: exactly one way to make amount 0 -- use no coins

    for (int c : coins) {
        for (int w = c; w <= amount; ++w) {
            dp[w] += dp[w - c];
        }
    }
    return dp[amount];
}

// Deliberately WRONG for this problem: counts permutations, not
// combinations, because amount is outer and every coin is retried fresh for
// every w regardless of which coins already contributed to dp[w - c]. Kept
// here, and exercised in main(), purely to make the loop-order contrast
// concrete rather than asserted in a comment.
long long countPermutationsNotCombinations(const std::vector<int>& coins, int amount) {
    std::vector<long long> dp(amount + 1, 0);
    dp[0] = 1;

    for (int w = 1; w <= amount; ++w) {
        for (int c : coins) {
            if (c <= w) {
                dp[w] += dp[w - c];
            }
        }
    }
    return dp[amount];
}

namespace {
int g_pass = 0;
int g_fail = 0;

void check(const std::string& label, long long actual, long long expected) {
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
    // amount=5, coins=[1,2,5] -> 4 combinations:
    // {5}, {1,1,1,1,1}, {1,1,1,2}, {1,2,2}
    check("combinations: coins=[1,2,5], amount=5",
          coinChangeIICombinations({1, 2, 5}, 5), 4);

    check("combinations: coins=[2], amount=3 (unreachable)",
          coinChangeIICombinations({2}, 3), 0);

    check("combinations: coins=[10], amount=10", coinChangeIICombinations({10}, 10), 1);

    check("combinations: amount=0 -> exactly 1 way (use nothing)",
          coinChangeIICombinations({1, 2, 5}, 0), 1);

    // The contrast: for coins=[1,2], amount=4, the two loop orders disagree.
    // Combinations (order doesn't matter): {1,1,1,1}, {1,1,2}, {2,2} -> 3.
    // Permutations (order matters): 1112,1121,1211,2111,1,2,2,1,2,2,2,2 ...
    // counted here purely to demonstrate the divergence, not as a "correct
    // answer" for LeetCode 518 -- the permutation count is intentionally
    // different (and larger) than the combination count.
    long long combos = coinChangeIICombinations({1, 2}, 4);
    long long perms = countPermutationsNotCombinations({1, 2}, 4);
    check("combinations: coins=[1,2], amount=4", combos, 3);
    std::cout << "[INFO] same coins/amount counted as permutations => "
              << perms << " (expected to DIFFER from combinations=" << combos
              << ", demonstrating why loop order matters)\n";
    check("permutations differ from combinations for coins=[1,2], amount=4",
          (perms != combos) ? 1 : 0, 1);

    std::cout << "\n" << g_pass << " passed, " << g_fail << " failed\n";
    return g_fail == 0 ? 0 : 1;
}
