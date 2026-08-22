// ============================================================================
// LeetCode 739 — Daily Temperatures
// ============================================================================
//
// PROBLEM
// -------
// Given an array of integers `temperatures` representing daily temperatures,
// return an array `answer` such that answer[i] is the number of days you have
// to wait after day i to get a WARMER temperature. If there is no future day
// for which this is possible, answer[i] == 0.
//
// Example: temperatures = [73,74,75,71,69,72,76,73]
//   answer          = [1, 1, 4, 2, 1, 1, 0, 0]
//   (e.g. day 2 (75) waits until day 6 (76): 6 - 2 = 4 days)
//
// APPROACH — Monotonic stack; the pop writes a DISTANCE, not a value
// ------------------------------------------------------------------
// Restate the problem: for each i, find the nearest index j > i with
// temperatures[j] > temperatures[i], and output j - i. That is exactly "next
// greater element," wearing a weather costume — so the monotonic-stack
// template applies directly.
//
// Scan left to right with a stack of indices whose temperatures are still
// waiting for a warmer day. Invariant: temperatures are non-increasing from
// bottom to top of the stack. When day i arrives with temperature t:
//   - Every waiting day colder than t has just found its warmer day — it is
//     day i, and no closer one exists because i is the first index since that
//     day where anything warmer appeared. Pop it and write answer[popped] =
//     i - popped (the DISTANCE, which is the only difference from the plain
//     next-greater template).
//   - Push i; it now waits too.
//
// WHY THIS IS O(n) DESPITE THE INNER WHILE LOOP (amortized argument):
// each index is pushed exactly once and popped at most once across the whole
// run, so total work <= 2n. The naive alternative — for each day, scan
// forward until a warmer day appears — is O(n^2) in the worst case (a
// non-increasing temperature sequence forces a full scan from every day).
//
// COMPLEXITY
// ----------
// Time:  O(n) amortized — at most n pushes + n pops total.
// Space: O(n) worst case for the stack (strictly non-increasing input never
//        pops anything, e.g. [80,79,78,...] leaves all indices stacked).
// ============================================================================
#include <iostream>
#include <string>
#include <vector>

std::vector<int> dailyTemperatures(const std::vector<int>& temperatures) {
  int n = static_cast<int>(temperatures.size());
  std::vector<int> answer(n, 0);   // 0 = "no warmer day ever" (the sentinel)
  std::vector<int> waiting;        // holds INDICES; temps non-increasing
  waiting.reserve(n);

  for (int i = 0; i < n; ++i) {
    // Strict '>' : a day equal to a waiting day does NOT resolve it — we
    // need strictly WARMER. Equal days therefore coexist on the stack.
    while (!waiting.empty() && temperatures[waiting.back()] < temperatures[i]) {
      int popped = waiting.back();
      waiting.pop_back();
      answer[popped] = i - popped;  // distance in days, not the temperature
    }
    waiting.push_back(i);
  }
  // Indices still on the stack never see a warmer day; their answer stays 0.

  return answer;
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
    std::vector<int> temps = {73, 74, 75, 71, 69, 72, 76, 73};
    std::vector<int> expected = {1, 1, 4, 2, 1, 1, 0, 0};
    check(dailyTemperatures(temps) == expected,
          "classic example -> [1,1,4,2,1,1,0,0]");
  }

  {
    // All same temperature: strict '>' means nothing ever pops -> all zeros.
    std::vector<int> temps = {30, 30, 30, 30};
    std::vector<int> expected = {0, 0, 0, 0};
    check(dailyTemperatures(temps) == expected,
          "all equal -> all zeros (equal never counts as warmer)");
  }

  {
    // Non-increasing sequence: worst case, no pops, everything stays on the
    // stack, all answers zero.
    std::vector<int> temps = {80, 79, 78, 60};
    std::vector<int> expected = {0, 0, 0, 0};
    check(dailyTemperatures(temps) == expected,
          "non-increasing -> all zeros");
  }

  {
    // Single element: no future day exists.
    std::vector<int> temps = {42};
    std::vector<int> expected = {0};
    check(dailyTemperatures(temps) == expected, "single day -> [0]");
  }

  {
    // One cold dip in the middle must wait for the global max far ahead;
    // exercises a multi-pop step resolving several days at once.
    std::vector<int> temps = {70, 71, 65, 66, 90};
    std::vector<int> expected = {1, 3, 1, 1, 0};
    check(dailyTemperatures(temps) == expected,
          "dip waits for spike; multi-pop resolves batch -> [1,3,1,1,0]");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
