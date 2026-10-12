// ============================================================================
// Greedy — generic reusable template (C++17)
// ============================================================================
//
// maxNonOverlappingIntervals: the classic Activity Selection problem, sorted
// by END time. jumpGame: a different flavor of greedy (track the farthest
// reachable index seen so far).
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>

using namespace std;


// ----------------------------------------------------------------------------
// Activity Selection: maximum number of non-overlapping intervals.
//
// Sort by END time. Greedily keep any interval whose start is at or after
// the end of the last kept interval -- the interval ending soonest among
// all remaining candidates always leaves the most room for what follows.
// ----------------------------------------------------------------------------
int maxNonOverlappingIntervals(vector<pair<int, int>> intervals) {
  sort(intervals.begin(), intervals.end(),
            [](const pair<int, int>& a, const pair<int, int>& b) {
              return a.second < b.second;
            });

  int count = 0;
  int lastEnd = INT_MIN;

  for (const auto& interval : intervals) {
    if (interval.first >= lastEnd) {
      ++count;
      lastEnd = interval.second;
    }
  }

  return count;
}

// ----------------------------------------------------------------------------
// Jump Game: can you reach the last index, where nums[i] is the max jump
// length from index i?
//
// Greedily track the farthest index reachable so far. If the current index
// ever exceeds that farthest-reachable bound, you're stuck -- return false.
// ----------------------------------------------------------------------------
bool canJumpToEnd(const vector<int>& nums) {
  int farthest = 0;
  for (size_t i = 0; i < nums.size(); ++i) {
    if (static_cast<int>(i) > farthest) return false;  // stuck before reaching i
    farthest = max(farthest, static_cast<int>(i) + nums[i]);
  }
  return true;
}

int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](bool condition, const string& label) {
    if (condition) {
      cout << "[PASS] " << label << "\n";
      ++pass_count;
    } else {
      cout << "[FAIL] " << label << "\n";
      ++fail_count;
    }
  };

  {
    // Sorted by end time: (1,3)(2,4)(3,5)(0,6)(5,7)(3,9)(8,10).
    // Picks: (1,3) -> (3,5) [2 skipped, starts before end 3] -> (5,7)
    //        [(0,6) and (3,9) skipped, start before end 7] -> (8,10). Total: 4.
    vector<pair<int, int>> intervals = {{1, 3}, {2, 4}, {3, 5}, {0, 6}, {5, 7}, {3, 9}, {8, 10}};
    check(maxNonOverlappingIntervals(intervals) == 4,
          "activity selection: 7 intervals -> 4 non-overlapping");
  }
  {
    vector<pair<int, int>> intervals = {{1, 2}, {2, 3}, {3, 4}};
    check(maxNonOverlappingIntervals(intervals) == 3,
          "activity selection: touching-but-not-overlapping -> all 3 kept");
  }
  {
    vector<pair<int, int>> empty_intervals = {};
    check(maxNonOverlappingIntervals(empty_intervals) == 0,
          "activity selection: empty input -> 0");
  }

  check(canJumpToEnd({2, 3, 1, 1, 4}) == true, "jump game [2,3,1,1,4] -> reachable");
  check(canJumpToEnd({3, 2, 1, 0, 4}) == false, "jump game [3,2,1,0,4] -> stuck at index 3");
  check(canJumpToEnd({0}) == true, "jump game single element -> trivially reachable");

  cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
