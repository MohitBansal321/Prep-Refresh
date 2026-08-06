// ============================================================================
// DP on Grids — generic reusable template (C++17)
// ============================================================================
//
// uniquePaths counts paths through an empty grid; minPathSum finds the
// minimum-cost path; uniquePathsWithObstacles shows how obstacle cells break
// the normal recurrence.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>

int uniquePaths(int rows, int cols) {
  std::vector<std::vector<long long>> dp(rows, std::vector<long long>(cols, 0));
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (r == 0 || c == 0) {
        dp[r][c] = 1;  // exactly one way along the top row or left column
        continue;
      }
      dp[r][c] = dp[r - 1][c] + dp[r][c - 1];
    }
  }
  return static_cast<int>(dp[rows - 1][cols - 1]);
}

int minPathSum(const std::vector<std::vector<int>>& grid) {
  int rows = static_cast<int>(grid.size());
  int cols = static_cast<int>(grid[0].size());
  std::vector<std::vector<int>> dp(rows, std::vector<int>(cols));

  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (r == 0 && c == 0) {
        dp[r][c] = grid[r][c];
        continue;
      }
      int fromTop = (r > 0) ? dp[r - 1][c] : INT_MAX;
      int fromLeft = (c > 0) ? dp[r][c - 1] : INT_MAX;
      dp[r][c] = std::min(fromTop, fromLeft) + grid[r][c];
    }
  }

  return dp[rows - 1][cols - 1];
}

int uniquePathsWithObstacles(const std::vector<std::vector<int>>& grid) {
  int rows = static_cast<int>(grid.size());
  int cols = static_cast<int>(grid[0].size());
  std::vector<std::vector<long long>> dp(rows, std::vector<long long>(cols, 0));

  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (grid[r][c] == 1) {
        dp[r][c] = 0;  // obstacle: this cell is unreachable, no recurrence applies
        continue;
      }
      if (r == 0 && c == 0) {
        dp[r][c] = 1;
        continue;
      }
      long long fromTop = (r > 0) ? dp[r - 1][c] : 0;
      long long fromLeft = (c > 0) ? dp[r][c - 1] : 0;
      dp[r][c] = fromTop + fromLeft;
    }
  }

  return static_cast<int>(dp[rows - 1][cols - 1]);
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

  check(uniquePaths(3, 7) == 28, "uniquePaths(3,7) -> 28");
  check(uniquePaths(1, 1) == 1, "uniquePaths(1,1) -> 1 (already at destination)");

  {
    std::vector<std::vector<int>> grid = {{1, 3, 1}, {1, 5, 1}, {4, 2, 1}};
    check(minPathSum(grid) == 7, "minPathSum classic 3x3 grid -> 7");
  }

  {
    std::vector<std::vector<int>> grid = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
    check(uniquePathsWithObstacles(grid) == 2,
          "uniquePathsWithObstacles with one central obstacle -> 2");
  }

  {
    std::vector<std::vector<int>> grid = {{0, 1}, {0, 0}};
    check(uniquePathsWithObstacles(grid) == 1,
          "uniquePathsWithObstacles, obstacle blocking the top route -> 1");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
