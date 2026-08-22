// ============================================================================
// LeetCode 134 — Gas Station
// ============================================================================
//
// PROBLEM
// -------
// There are n gas stations arranged in a circle. Station i holds gas[i] liters
// and driving from station i to the next costs cost[i] liters. You start with
// an EMPTY tank at one station. Return the index of the station from which you
// can travel the entire circuit once clockwise, or -1 if none exists.
// Guaranteed: at most one such station exists, and tanks are unbounded.
//
// Example: gas = [1,2,3,4,5], cost = [3,4,5,1,2]  ->  3
//
// APPROACH — Greedy running deficit (single pass over a circle)
// ------------------------------------------------------------
// Two facts drive everything:
//   (1) A circuit is completable from SOME station iff total gas >= total
//       cost. Necessity is obvious; sufficiency follows because summing gains
//       around a full loop nets >= 0, so no prefix deficit can persist forever.
//   (2) If starting at station `start` the tank first goes NEGATIVE upon
//       arriving at station j (i.e. after consuming cost[j-1]), then NO station
//       strictly between start and j can work either -- discard them ALL at
//       once and restart from the next candidate after j.
//
// WHY THE GREEDY CHOICE IS SAFE (the proof obligation — the exchange argument)
// Let the run from `start` go negative for the first time upon arriving at j,
// and write the net gain of leg k as g[k] = gas[k] - cost[k]. Then
// sum(g[start..j-1]) < 0. Take any intermediate candidate s with
// start < s <= j. During the failed run the tank was still >= 0 when the car
// LEFT s (otherwise s would have been the failure point), so the balance
// banked on legs start..s-1 was >= 0:
//     sum(g[s..j-1]) = sum(g[start..j-1]) - sum(g[start..s-1])
//                    < 0            - 0                  = 0.
// A fresh start at s therefore arrives at j with a negative tank too — s fails
// as well. Every skipped station is provably hopeless, which is why we never
// revisit any of them, why each reset discards a whole range at once, and why
// every element is examined exactly once despite the "restarting".
//
// COMPLEXITY
// ----------
// Time:  O(n) — although `start` resets look like re-scanning, i never moves
//        backward; every element is examined exactly once.
// Space: O(1) — two scalars (`total`, `tank`) plus the answer index.
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

// Returns an index from which the full circuit is completable, or -1.
int completeCircuit(const std::vector<int>& gas, const std::vector<int>& cost) {
  int total = 0;   // Net gain around the WHOLE circuit. Decides existence.
  int tank = 0;    // Tank of the current candidate run since `start`.
  int start = 0;   // Current candidate starting station.

  for (size_t i = 0; i < gas.size(); ++i) {
    int gain = gas[i] - cost[i];
    total += gain;
    tank += gain;

    if (tank < 0) {
      // The candidate run just died arriving at station i. By the exchange
      // argument above, EVERY station between `start` and i is also doomed --
      // skip them all irrevocably and try from the next one.
      start = static_cast<int>(i) + 1;
      tank = 0;  // Fresh empty-tank run from the new candidate.
    }
  }

  // total >= 0 proves some station works; uniqueness (guaranteed) plus the
  // fact that all stations before `start` were provably eliminated means the
  // survivor must be `start` itself. If total < 0, no station works at all.
  return total >= 0 ? start : -1;
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
    // Classic example: candidates die at 0 (tank -2), then 1 (-2), then 2 (-2);
    // survivor start=3 completes with tank 4-1 + 5-2 = 6.
    check(completeCircuit({1, 2, 3, 4, 5}, {3, 4, 5, 1, 2}) == 3,
          "gas [1,2,3,4,5], cost [3,4,5,1,2] -> start 3");
  }

  {
    // Total deficit: no start can ever complete the circuit.
    check(completeCircuit({2, 3, 4}, {3, 4, 3}) == -1,
          "gas [2,3,4], cost [3,4,3] -> total deficit -> -1");
  }

  {
    // Single station, positive margin: trivially index 0.
    check(completeCircuit({5}, {4}) == 0, "gas [5], cost [4] -> single station 0");
  }

  {
    // Single station, exact break-even: completing requires arriving back with
    // tank >= 0, and 5 - 5 = 0 satisfies that.
    check(completeCircuit({5}, {5}) == 0, "gas [5], cost [5] -> exact break-even works");
  }

  {
    // First station works directly -- no resets ever fire.
    check(completeCircuit({3, 1, 1}, {1, 2, 2}) == 0,
          "gas [3,1,1], cost [1,2,2] -> start 0 works immediately");
  }

  {
    // Reset fires late but the survivor is NOT the last index: run from 0 dies
    // at index 2 (tank 3-1 +1-2 +1-3 = -1); restart at 3 succeeds.
    check(completeCircuit({3, 1, 1, 2}, {1, 2, 3, 1}) == 3,
          "gas [3,1,1,2], cost [1,2,3,1] -> reset mid-array, start 3");
  }

  {
    // Multiple resets before finding the winner.
    check(completeCircuit({1, 1, 10}, {2, 2, 1}) == 2,
          "gas [1,1,10], cost [2,2,1] -> two resets then start 2");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
