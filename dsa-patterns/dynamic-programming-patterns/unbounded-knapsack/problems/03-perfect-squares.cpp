// LeetCode 279 — Perfect Squares
//
// Problem: given an integer `n`, return the LEAST number of perfect square
// numbers (1, 4, 9, 16, ...) that sum to exactly `n`.
//
// Why this is Unbounded Knapsack: the "items" are the perfect squares
// <= n (1, 4, 9, ...), each reusable an unlimited number of times, and `n` is
// the capacity we must hit EXACTLY while minimizing the count of items used
// -- structurally identical to 01-coin-change.cpp with the coin list replaced
// by { 1, 4, 9, 16, ... } up to n. dp[w] = fewest perfect squares summing to
// exactly w. Recurrence:
//     dp[w] = min over every square s <= w of ( dp[w - s] + 1 )
// filled with w increasing from 1 to n, reading dp[w - s] after it has
// already potentially been updated using that same square earlier in the
// pass -- the same forward-fill reuse mechanism as every other file in this
// module.
//
// Complexity: O(n * sqrt(n)) time (n capacities, up to sqrt(n) square
// "coins" tried per capacity), O(n) space.

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

int numSquares(int n) {
    std::vector<int> dp(n + 1, n + 1); // n+1 is a safe "unreachable" sentinel:
                                        // worst case is n copies of 1*1, so
                                        // the true answer never exceeds n.
    dp[0] = 0; // base case: 0 squares needed to make 0

    for (int w = 1; w <= n; ++w) {
        for (int root = 1; root * root <= w; ++root) {
            int square = root * root;
            dp[w] = std::min(dp[w], dp[w - square] + 1);
        }
    }
    return dp[n];
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
    check("n=12 (4+4+4)", numSquares(12), 3);
    check("n=13 (4+9)", numSquares(13), 2);
    check("n=1 (1)", numSquares(1), 1);
    check("n=0", numSquares(0), 0);
    check("n=4 (a perfect square itself)", numSquares(4), 1);
    check("n=7292", numSquares(7292), 4);

    std::cout << "\n" << g_pass << " passed, " << g_fail << " failed\n";
    return g_fail == 0 ? 0 : 1;
}
