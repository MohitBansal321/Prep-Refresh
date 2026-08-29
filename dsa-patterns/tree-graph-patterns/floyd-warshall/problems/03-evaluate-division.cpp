// ============================================================================
// LeetCode 399 — Evaluate Division
// https://leetcode.com/problems/evaluate-division/
// ============================================================================
//
// PROBLEM
// -------
// equations[i] = [A, B] with values[i] = k means A / B = k. Given a list of
// queries [C, D], for each return C / D if it can be derived from the given
// equations, or -1.0 if it cannot (either variable is unknown, or no chain
// of equations connects them).
//
// APPROACH -- Floyd-Warshall's waypoint relaxation, specialized to products
// -----------------------------------------------------------------------
// Build a graph where each equation A / B = k becomes TWO directed edges:
// A -> B with weight k, and B -> A with weight 1/k (since B / A = 1/k
// follows immediately). The waypoint relaxation rule changes from "does
// routing through k give a smaller SUM" to "does routing through k give a
// value at all, computed as a PRODUCT of edge weights along the path" --
// dist[i][j] becomes dist[i][k] * dist[k][j] rather than
// dist[i][k] + dist[k][j]. This is the same triple loop and the same
// "consider every waypoint" logic Bellman-Ford's exercises.md also explores
// for the currency-arbitrage transformation, applied here directly instead
// of via a log-space trick, since there is no cycle-detection question in
// this problem -- just "does a value exist for this pair."
//
// Time:  O(V^3) to build the full quotient matrix (V = distinct variables),
//        O(1) per query afterward.
// Space: O(V^2) for the quotient matrix, O(V) for the variable-to-index map.
// ============================================================================

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<double> calcEquation(
    const std::vector<std::vector<std::string>>& equations,
    const std::vector<double>& values,
    const std::vector<std::vector<std::string>>& queries) {
  std::unordered_map<std::string, int> index;
  for (const auto& eq : equations) {
    for (const std::string& var : eq) {
      if (index.find(var) == index.end()) {
        int next = static_cast<int>(index.size());
        index[var] = next;
      }
    }
  }
  int n = static_cast<int>(index.size());

  // quotient[i][j] = value of (variable i) / (variable j), or -1 if unknown.
  std::vector<std::vector<double>> quotient(n, std::vector<double>(n, -1.0));
  for (int i = 0; i < n; ++i) quotient[i][i] = 1.0;  // any variable over itself is 1

  for (size_t i = 0; i < equations.size(); ++i) {
    int a = index[equations[i][0]];
    int b = index[equations[i][1]];
    quotient[a][b] = values[i];
    quotient[b][a] = 1.0 / values[i];
  }

  for (int k = 0; k < n; ++k) {
    for (int i = 0; i < n; ++i) {
      if (quotient[i][k] < 0) continue;  // no known path to the waypoint
      for (int j = 0; j < n; ++j) {
        if (quotient[k][j] < 0) continue;
        if (quotient[i][j] < 0) {
          // Only fill in a value that is not already known directly --
          // unlike the numeric MIN case, a "better" product does not make
          // sense here (there is exactly one consistent value per pair,
          // guaranteed by the problem's constraints).
          quotient[i][j] = quotient[i][k] * quotient[k][j];
        }
      }
    }
  }

  std::vector<double> answers;
  answers.reserve(queries.size());
  for (const auto& q : queries) {
    auto itA = index.find(q[0]);
    auto itB = index.find(q[1]);
    if (itA == index.end() || itB == index.end()) {
      answers.push_back(-1.0);  // unknown variable
      continue;
    }
    double result = quotient[itA->second][itB->second];
    answers.push_back(result < 0 ? -1.0 : result);
  }
  return answers;
}

// ============================================================================
// main() -- printed, verifiable output.
// ============================================================================

int g_pass = 0;
int g_fail = 0;

bool approxEqual(double a, double b) {
  return a < 0 && b < 0 ? true : (a - b < 1e-5 && b - a < 1e-5);
}

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
    // equations = [a/b=2, b/c=3], queries = [a/c, c/a, a/e (unknown var),
    // a/a, x/x (both unknown)].
    std::vector<std::vector<std::string>> equations = {{"a", "b"}, {"b", "c"}};
    std::vector<double> values = {2.0, 3.0};
    std::vector<std::vector<std::string>> queries = {
        {"a", "c"}, {"c", "a"}, {"a", "e"}, {"a", "a"}, {"x", "x"}};
    std::vector<double> result = calcEquation(equations, values, queries);
    check(approxEqual(result[0], 6.0), "a/c == 6.0 (a/b * b/c = 2 * 3, via waypoint b)");
    check(approxEqual(result[1], 1.0 / 6.0), "c/a == 1/6 (the reverse direction)");
    check(approxEqual(result[2], -1.0), "a/e -> -1.0 (e is an unknown variable)");
    check(approxEqual(result[3], 1.0), "a/a == 1.0 (any known variable over itself)");
    check(approxEqual(result[4], -1.0), "x/x -> -1.0 (x never appears in any equation)");
  }

  {
    // Two disconnected equation chains: a/b and x/y are never linked by
    // any equation, so querying across them must return -1, not a
    // nonsensical computed value.
    std::vector<std::vector<std::string>> equations = {{"a", "b"}, {"x", "y"}};
    std::vector<double> values = {2.0, 5.0};
    std::vector<std::vector<std::string>> queries = {{"a", "x"}, {"a", "b"}};
    std::vector<double> result = calcEquation(equations, values, queries);
    check(approxEqual(result[0], -1.0), "a/x -> -1.0, no equation chain connects the two groups");
    check(approxEqual(result[1], 2.0), "a/b == 2.0, the direct equation value");
  }

  std::cout << "\n" << g_pass << " passed, " << g_fail << " failed.\n";
  return g_fail == 0 ? 0 : 1;
}
