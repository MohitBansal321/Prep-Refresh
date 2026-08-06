// ============================================================================
// LeetCode 121 — Best Time to Buy and Sell Stock
// ============================================================================
//
// PROBLEM
// -------
// You are given an array `prices` where prices[i] is the price of a given
// stock on day i. You want to maximize profit by choosing a single day to
// buy one share and a single (later) day to sell it. Return the maximum
// profit achievable; return 0 if no profit is possible (never buy/sell).
//
// Example: prices = [7,1,5,3,6,4]  ->  5  (buy at 1, sell at 6)
// Example: prices = [7,6,4,3,1]    ->  0  (prices only fall -- no profit)
//
// APPROACH — Kadane's Algorithm applied to day-over-day price DELTAS
// -------------------------------------------------------------------
// This problem is not phrased as "find the max sum contiguous subarray," but
// it reduces to exactly that once you look at day-over-day price CHANGES
// instead of raw prices. Define delta[i] = prices[i] - prices[i-1] for
// i >= 1. A single buy-low-sell-high transaction spanning days [b, s] earns
// prices[s] - prices[b], which telescopes into the SUM of every day-over-day
// delta strictly between b and s: delta[b+1] + delta[b+2] + ... + delta[s].
// So "best single buy/sell pair" is exactly "best-sum contiguous subarray of
// the delta array" -- vanilla Kadane's, applied to a transformed array.
//
// This module solves it with an equivalent, more direct formulation that
// avoids building a separate delta array: track the minimum price SEEN SO
// FAR while scanning left to right (the best possible buy day up to now),
// and at each day compute the profit if selling today against that running
// minimum, keeping the best such profit. This is the same extend-or-restart
// idea in disguise: "current_sum" here is "price today minus the best buy
// price so far," and "restart" happens implicitly whenever today's price
// itself becomes the new running minimum (a worse floor to have bought at
// disappears the same way a negative running sum does in vanilla Kadane's).
//
// COMPLEXITY
// ----------
// Time:  O(n) -- one pass, O(1) work per day.
// Space: O(1) -- two running scalars (min price so far, best profit so far).
//
// Contrast with brute force (try every buy/sell day pair): O(n^2) pairs,
// each O(1) to evaluate, so O(n^2) overall. Kadane's-style scanning collapses
// this to O(n) by never re-examining a day once a better buy floor has been
// established.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int maxProfit(const std::vector<int>& prices) {
  if (prices.empty()) return 0;

  int min_price_so_far = prices[0];
  int best_profit = 0;

  for (size_t i = 1; i < prices.size(); ++i) {
    // Profit if we sold today, having bought at the lowest price seen so far.
    int profit_if_sold_today = prices[i] - min_price_so_far;
    best_profit = std::max(best_profit, profit_if_sold_today);

    // Update the running floor -- a lower buy price found later always
    // strictly improves every future day's potential profit, the same way
    // vanilla Kadane's abandons a losing running sum in favor of a fresh
    // start.
    min_price_so_far = std::min(min_price_so_far, prices[i]);
  }

  return best_profit;
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
    std::vector<int> prices = {7, 1, 5, 3, 6, 4};
    check(maxProfit(prices) == 5, "[7,1,5,3,6,4] -> 5 (buy at 1, sell at 6)");
  }
  {
    std::vector<int> prices = {7, 6, 4, 3, 1};
    check(maxProfit(prices) == 0, "[7,6,4,3,1] -> 0 (only falls, never buy)");
  }
  {
    std::vector<int> prices = {2, 4, 1};
    check(maxProfit(prices) == 2, "[2,4,1] -> 2 (buy at 2, sell at 4)");
  }
  {
    std::vector<int> prices = {1};
    check(maxProfit(prices) == 0, "[1] -> 0 (can't buy and sell on the same/only day)");
  }
  {
    std::vector<int> prices = {3, 2, 6, 5, 0, 3};
    check(maxProfit(prices) == 4, "[3,2,6,5,0,3] -> 4 (buy at 2, sell at 6)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
