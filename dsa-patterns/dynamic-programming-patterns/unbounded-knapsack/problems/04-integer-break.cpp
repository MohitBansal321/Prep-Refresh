// LeetCode 343 — Integer Break
//
// Problem: given an integer `n >= 2`, break it into the sum of AT LEAST TWO
// positive integers, and maximize the PRODUCT of those integers. Return the
// maximum product.
//
// Why this is Unbounded-Knapsack-flavored: instead of items with an explicit
// weight/value pair, the "item" here is a part size `j` (1 <= j < i), and
// "reuse" shows up not as an explicit unlimited-supply list but as the
// recurrence deliberately allowing the SAME part size to be used again when
// further breaking the remainder. dp[i] = best product obtainable by
// breaking i into two or more parts. Recurrence:
//     dp[i] = max over every split point j (1 <= j < i) of
//             max( j * (i - j),      // split into exactly two parts: j and (i-j)
//                  j * dp[i - j] )   // split into j and "however dp[i-j] best
//                                    //  breaks the remainder" -- which may
//                                    //  itself choose j again as one of its
//                                    //  parts, exactly like reusing a coin.
// dp[i - j] having already been computed (i - j < i) and being free to reuse
// any part size, including j itself, is the same "may reuse the same item
// again" idea as the coin-change/rod-cutting recurrences elsewhere in this
// module -- just applied to "parts of an integer" instead of "coins" or
// "weighted items". Filled with i increasing from 2 to n (forward fill),
// exactly the direction this whole pattern depends on.
//
// Complexity: O(n^2) time (n outer values of i, up to n split points j per
// value), O(n) space.

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int integerBreak(int n) {
    std::vector<int> dp(n + 1, 0);
    // dp[1] is deliberately left at 0: 1 cannot be broken into two positive
    // parts, and 1 is never a useful "remainder" to reuse further, so it is
    // simply never read for anything meaningful in the loop below.
    for (int i = 2; i <= n; ++i) {
        for (int j = 1; j < i; ++j) {
            dp[i] = std::max({dp[i], j * (i - j), j * dp[i - j]});
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
    check("n=2 (1*1)", integerBreak(2), 1);
    check("n=3 (1*2)", integerBreak(3), 2);
    check("n=8 (2*3*3)", integerBreak(8), 18);
    check("n=10 (3*3*4)", integerBreak(10), 36);
    check("n=4 (2*2, reusing part size 2)", integerBreak(4), 4);

    std::cout << "\n" << g_pass << " passed, " << g_fail << " failed\n";
    return g_fail == 0 ? 0 : 1;
}
