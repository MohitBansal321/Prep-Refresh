// ============================================================================
// LeetCode 621 — Task Scheduler
// ============================================================================
//
// PROBLEM
// -------
// A tasks[i] is an uppercase letter naming a task; a CPU runs one task per
// unit of time (tasks may run in any order). Between two executions of the
// SAME task there must be at least n units of cooldown — used either by other
// tasks or by idle time. Return the minimum number of time units to finish all
// tasks.
//
// Example: tasks = [A,A,A,B,B,B], n = 2  ->  8   (A B idle A B idle A B)
//
// APPROACH — Greedy formula: the most frequent task dictates the skeleton
// ----------------------------------------------------------------------
// Let maxFreq = highest task frequency and maxCount = how many distinct tasks
// share that frequency. The answer is:
//
//     max(tasks.size(), (maxFreq - 1) * (n + 1) + maxCount)
//
// WHY THE GREEDY CHOICE IS SAFE (the proof obligation)
// Exchange argument, skeleton-first: take the most frequent task M. Any valid
// schedule contains maxFreq copies of M, so it decomposes into maxFreq "slots"
// separated by at least n units each — a skeleton of
// (maxFreq - 1) * (n + 1) units with the last slot hanging off the end.
// Now suppose some optimal schedule OPT does not place the OTHER max-frequency
// tasks in this maximal-spread arrangement: swap them into the skeleton's open
// columns (each column holds at most one of each letter, so the n-gap holds),
// which cannot lengthen the schedule and keeps validity — the swap-in is never
// worse, so a skeleton-shaped optimum exists.
//   - If the skeleton's gaps exceed the number of remaining tasks, idles are
//     FORCED (nothing else can fill an M-column), giving exactly
//     (maxFreq - 1) * (n + 1) + maxCount.
//   - If instead there are enough leftover tasks to fill every gap, no idle is
//     ever needed: pack the leftovers into gap cells (or extend past the last
//     M-slot where no cooldown applies), so the answer is just tasks.size().
// The max() of the two cases is therefore exact. Note the greedy commitment:
// we fix M's positions first and never reconsider — the proof says we need not.
//
// COMPLEXITY
// ----------
// Time:  O(n + 26) = O(n) — one counting pass plus a scan of 26 buckets.
//        (A heap-based simulation also works but costs O(n log 26) for nothing.)
// Space: O(26) = O(1) — one frequency array.
// ============================================================================

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>

// Returns the minimum number of time units to finish all tasks.
int leastTime(const std::vector<char>& tasks, int n) {
  // Count frequencies across the fixed 26-letter alphabet.
  std::array<int, 26> freq{};
  for (char t : tasks) {
    ++freq[static_cast<size_t>(t - 'A')];
  }

  // Most frequent task and how many tasks tie with it.
  int maxFreq = *std::max_element(freq.begin(), freq.end());
  int maxCount = static_cast<int>(std::count(freq.begin(), freq.end(), maxFreq));

  // Forced-idle case vs. enough-work-to-fill-the-gaps case -- see header.
  int skeleton = (maxFreq - 1) * (n + 1) + maxCount;
  int totalTasks = static_cast<int>(tasks.size());
  return std::max(totalTasks, skeleton);
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
    // Skeleton wins: (3-1)*(2+1)+1 = 7... but TWO tasks tie at freq 3, so
    // maxCount = 2 -> (3-1)*3+2 = 8 > 6 tasks. Idles forced in the middle gaps.
    std::vector<char> tasks = {'A', 'A', 'A', 'B', 'B', 'B'};
    check(leastTime(tasks, 2) == 8,
          "[A,A,A,B,B,B], n=2 -> 8 (skeleton with forced idles)");
  }

  {
    // No cooldown: every task runs back-to-back, pure count wins.
    check(leastTime({'A', 'A', 'A', 'B', 'B', 'B'}, 0) == 6,
          "n=0 -> no cooldown constraint -> 6");
  }

  {
    // Six distinct singletons around six A's: gaps absorb B..G exactly,
    // skeleton (6-1)*3+1 = 16 beats the 12 actual tasks.
    check(leastTime({'A', 'A', 'A', 'A', 'A', 'A',
                     'B', 'C', 'D', 'E', 'F', 'G'}, 2) == 16,
          "6xA + 6 singletons, n=2 -> 16 (classic LeetCode example)");
  }

  {
    // Two tasks only, cooldown longer than supply: A _ _ A forces idles;
    // skeleton (2-1)*3+1 = 4 > 2 tasks.
    check(leastTime({'A', 'A'}, 2) == 4,
          "[A,A], n=2 -> 4 (long cooldown forces idles)");
  }

  {
    // Enough filler to cover every gap: answer collapses to raw task count.
    // A x3, B..E fill both gaps twice over -> 9 total, no idle needed.
    check(leastTime({'A', 'A', 'A', 'B', 'C', 'D', 'E', 'F', 'G'}, 2) == 9,
          "3xA + 6 fillers, n=2 -> 9 (gaps fully covered, no idles)");
  }

  {
    // Single task instance: no repetition, no constraint ever fires.
    check(leastTime({'Z'}, 5) == 1, "[Z], n=5 -> 1");
  }

  {
    // All identical tasks: worst case, every gap is dead idle time.
    // (4-1)*(3+1)+1 = 13.
    check(leastTime({'K', 'K', 'K', 'K'}, 3) == 13,
          "[K,K,K,K], n=3 -> 13 (all-identical worst case)");
  }

  {
    // Boundary: n=1 with two tied max-freq tasks -> (3-1)*2+2 = 6 == task count.
    check(leastTime({'A', 'A', 'A', 'B', 'B', 'B'}, 1) == 6,
          "[A,A,A,B,B,B], n=1 -> 6 (interleave exactly)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
