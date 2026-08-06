// ============================================================================
// LeetCode 502 — IPO
// ============================================================================
//
// PROBLEM
// -------
// You are a startup preparing to launch an IPO. To boost your valuation
// before the IPO, you want to finish some projects to increase your
// capital before the IPO. Since resources are limited, you can only
// finish at most `k` distinct projects before the IPO. Design an
// algorithm to pick the `k` projects to maximize your final capital.
//
// You are given `n` projects: `profits[i]` is the profit from project i,
// and `capital[i]` is the minimum capital needed to START project i. You
// start with capital `w`. You may only start a project if your current
// capital is >= that project's required capital. Upon completion, the
// project's profit is added to your capital (which may unlock other,
// previously-too-expensive projects for later rounds). Pick at most `k`
// projects (one per round, greedily) to maximize final capital.
//
// Example: k=2, w=0, profits=[1,2,3], capital=[0,1,1] -> 4
//   (start with project 0 (capital 0, profit 1) -> capital becomes 1;
//    now afford project 1 or 2 (capital 1), pick profit 3 -> capital 4)
//
// APPROACH — Two Heaps (structural cousin of the median pattern)
// -----------------------------------------------------------------
// This is not literally computing a median, but the README's Similar
// Patterns section calls it a structural cousin of Two Heaps for a
// specific reason: the projects are split across a MOVING BOUNDARY
// (your current capital), exactly the way the median pattern splits
// numbers across a moving boundary (the halfway point of everything
// inserted so far) -- and the same "two heaps, one on each side of the
// boundary" shape solves both problems.
//
//   - A MIN-HEAP of "not yet affordable" projects, ordered by required
//     capital. Its top() is always the cheapest not-yet-affordable
//     project -- the next one that could become affordable as capital
//     grows.
//   - A MAX-HEAP of "currently affordable" projects, ordered by profit.
//     Its top() is always the single best project you could pick right
//     now.
//
// Each of the k rounds:
//   1. Move every project from the min-heap into the max-heap whose
//      required capital is now <= current capital (they just became
//      affordable, possibly because of profit gained in a previous
//      round).
//   2. If the max-heap (affordable projects) is empty, no further
//      project can ever be completed -- stop early.
//   3. Otherwise, greedily take the max-heap's top (the most profitable
//      currently-affordable project), add its profit to capital, and
//      remove it (pop) so it cannot be picked again.
//
// Why greedy is correct here: among all currently-affordable projects,
// picking anything other than the most profitable one can never help --
// every affordable project remains equally affordable in future rounds
// regardless of which one you pick this round (your capital only ever
// goes up), so there is no future benefit to "saving" a more profitable
// affordable project for later. Taking the best available option now is
// always at least as good as any other order.
//
// COMPLEXITY
// ----------
// Time:  O(n log n) -- building the initial min-heap of all n projects is
//        O(n log n) (n pushes); across k rounds, each project is moved
//        from the min-heap to the max-heap at most once (O(log n) each),
//        and each round pops at most one project from the max-heap
//        (O(log n)). Total: O(n log n + k log n) = O(n log n) since k <= n.
// Space: O(n) -- both heaps together hold at most n projects total.
// ============================================================================

#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

// Returns the maximum final capital achievable by completing at most k
// projects, starting with capital w.
long long findMaximizedCapital(int k, long long w, const std::vector<int>& profits,
                                const std::vector<int>& capital) {
  int n = static_cast<int>(profits.size());

  // Min-heap of {required_capital, profit} for projects not yet affordable,
  // ordered by required_capital ascending (std::greater on the pair, which
  // compares required_capital first by default pair ordering).
  using Project = std::pair<long long, long long>;  // {capital_needed, profit}
  std::priority_queue<Project, std::vector<Project>, std::greater<Project>> not_affordable;

  // Max-heap of profits for currently-affordable projects.
  std::priority_queue<long long> affordable;

  for (int i = 0; i < n; ++i) {
    not_affordable.push({capital[i], profits[i]});
  }

  long long current_capital = w;

  for (int round = 0; round < k; ++round) {
    // Unlock every project that is now affordable given current_capital.
    while (!not_affordable.empty() && not_affordable.top().first <= current_capital) {
      affordable.push(not_affordable.top().second);
      not_affordable.pop();
    }

    if (affordable.empty()) {
      // No project can be completed this round (or ever, since capital
      // only grows) -- stop early.
      break;
    }

    // Greedily take the most profitable currently-affordable project.
    current_capital += affordable.top();
    affordable.pop();
  }

  return current_capital;
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
    // Classic LeetCode example.
    std::vector<int> profits = {1, 2, 3};
    std::vector<int> capital = {0, 1, 1};
    check(findMaximizedCapital(2, 0, profits, capital) == 4,
          "k=2, w=0, profits=[1,2,3], capital=[0,1,1] -> 4");
  }

  {
    // Second classic LeetCode example.
    std::vector<int> profits = {1, 2, 3};
    std::vector<int> capital = {0, 1, 2};
    check(findMaximizedCapital(3, 0, profits, capital) == 6,
          "k=3, w=0, profits=[1,2,3], capital=[0,1,2] -> 6");
  }

  {
    // Starting capital already covers every project immediately -- should
    // just pick the k most profitable projects overall.
    std::vector<int> profits = {5, 1, 9, 3};
    std::vector<int> capital = {0, 0, 0, 0};
    check(findMaximizedCapital(2, 0, profits, capital) == 14,
          "all affordable immediately, k=2 -> pick top 2 profits (9+5) = 14");
  }

  {
    // Not enough capital to ever afford anything.
    std::vector<int> profits = {5, 10};
    std::vector<int> capital = {100, 200};
    check(findMaximizedCapital(2, 0, profits, capital) == 0,
          "nothing affordable ever -> capital stays 0");
  }

  {
    // k = 0: no rounds should ever execute.
    std::vector<int> profits = {5, 10};
    std::vector<int> capital = {0, 0};
    check(findMaximizedCapital(0, 3, profits, capital) == 3,
          "k=0 -> capital unchanged at starting w=3");
  }

  {
    // A single project exactly at the affordability boundary (capital ==
    // requirement should count as affordable, per the problem's <= rule).
    std::vector<int> profits = {7};
    std::vector<int> capital = {5};
    check(findMaximizedCapital(1, 5, profits, capital) == 12,
          "capital exactly equals requirement -> project is affordable");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
