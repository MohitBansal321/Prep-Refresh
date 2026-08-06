// ============================================================================
// 0/1 Knapsack — generic reusable template (C++17)
// ============================================================================
//
// Demonstrates the classic 2D table formulation and its 1D space-optimized
// counterpart for the same recurrence: for each item, either skip it or
// take it (if it fits), each item usable at most once.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <iostream>
#include <vector>

// ----------------------------------------------------------------------------
// knapsack01 — 2D table version.
//
// dp[i][w] = best value achievable using the first i items with capacity w.
// dp[i][w] = dp[i-1][w] (skip item i-1)
//            or dp[i-1][w - weight[i-1]] + value[i-1] (take it, if it fits)
// ----------------------------------------------------------------------------
int knapsack01(const std::vector<int>& weights, const std::vector<int>& values,
               int capacity) {
  int n = static_cast<int>(weights.size());
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));

  for (int i = 1; i <= n; ++i) {
    for (int w = 0; w <= capacity; ++w) {
      dp[i][w] = dp[i - 1][w];  // skip item i-1
      if (weights[i - 1] <= w) {
        dp[i][w] = std::max(dp[i][w], dp[i - 1][w - weights[i - 1]] + values[i - 1]);
      }
    }
  }

  return dp[n][capacity];
}

// ----------------------------------------------------------------------------
// knapsack01Optimized — 1D space-optimized version.
//
// Since dp[i][w] only ever depends on row i-1, a single 1D array suffices,
// PROVIDED capacity is iterated backward (high to low) within each item.
// Iterating forward would let an item's own update at a smaller capacity
// bleed into its update at a larger capacity within the SAME item pass --
// silently turning 0/1 semantics into unbounded (reuse) semantics.
// ----------------------------------------------------------------------------
int knapsack01Optimized(const std::vector<int>& weights, const std::vector<int>& values,
                         int capacity) {
  std::vector<int> dp(capacity + 1, 0);

  for (size_t i = 0; i < weights.size(); ++i) {
    for (int w = capacity; w >= weights[i]; --w) {
      dp[w] = std::max(dp[w], dp[w - weights[i]] + values[i]);
    }
  }

  return dp[capacity];
}

// ============================================================================
// main() — demonstrates both versions agree on the same test cases.
// ============================================================================
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
    // Classic textbook example: weights [1,3,4,5], values [1,4,5,7], capacity 7.
    // Best: items with weight 3+4=7, value 4+5=9.
    std::vector<int> weights = {1, 3, 4, 5};
    std::vector<int> values = {1, 4, 5, 7};
    int capacity = 7;
    int expected = 9;
    check(knapsack01(weights, values, capacity) == expected,
          "2D table: classic example -> 9");
    check(knapsack01Optimized(weights, values, capacity) == expected,
          "1D optimized: classic example -> 9");
  }

  {
    // Capacity 0 -> nothing fits, value 0.
    std::vector<int> weights = {2, 3};
    std::vector<int> values = {10, 20};
    check(knapsack01(weights, values, 0) == 0, "2D table: zero capacity -> 0");
    check(knapsack01Optimized(weights, values, 0) == 0, "1D optimized: zero capacity -> 0");
  }

  {
    // Single item that exactly fits.
    std::vector<int> weights = {5};
    std::vector<int> values = {100};
    check(knapsack01(weights, values, 5) == 100, "2D table: single item exact fit -> 100");
    check(knapsack01Optimized(weights, values, 5) == 100,
          "1D optimized: single item exact fit -> 100");
  }

  {
    // Every item too heavy -> 0.
    std::vector<int> weights = {10, 20, 30};
    std::vector<int> values = {1, 2, 3};
    check(knapsack01(weights, values, 5) == 0, "2D table: all items too heavy -> 0");
    check(knapsack01Optimized(weights, values, 5) == 0,
          "1D optimized: all items too heavy -> 0");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
