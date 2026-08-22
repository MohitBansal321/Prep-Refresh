// ============================================================================
// LeetCode 45 — Jump Game II
// ============================================================================
//
// PROBLEM
// -------
// Same setup as Jump Game (nums[i] = maximum jump length from index i), but
// now the array is GUARANTEED reachable and you must return the MINIMUM number
// of jumps to reach the last index.
//
// Example: nums = [2, 3, 1, 1, 4]  ->  2   (0 -> 1 -> 4)
//          nums = [2, 3, 0, 1, 4]  ->  2   (0 -> 1 -> 4)
//
// APPROACH — Greedy implicit BFS (layer-by-layer frontier expansion)
// ------------------------------------------------------------------
// Think of reachability in BFS layers: everything reachable in 0 jumps is
// layer 0 (just index 0); everything newly reachable with one more jump is the
// next layer. The greedy sweep walks one layer per jump using three scalars:
//   - `jumps`   : jumps taken so far (= current BFS depth).
//   - `curEnd`  : rightmost index reachable using EXACTLY `jumps` jumps --
//                 the boundary of the current layer.
//   - `farthest`: rightmost index reachable with `jumps + 1` jumps -- the
//                 boundary of the next layer, accumulated while scanning.
// Sweep i over the array, extending `farthest` from each position. The moment
// i hits `curEnd`, the current layer is exhausted; the ONLY way forward is one
// more jump, so commit: ++jumps and curEnd = farthest. Stop early once
// curEnd covers the last index.
//
// WHY THE GREEDY CHOICE IS SAFE (the proof obligation)
// Exchange argument at the layer boundary: suppose an optimal solution's next
// jump lands on some index j <= farthest within the exhausted layer's reach.
// Any index reachable from j is also reachable from whatever index achieved
// `farthest` (or from a later one scanned before the boundary), because
// farthest >= j means every target t <= j + nums[j] that matters is dominated:
// if t were beyond farthest, then some scanned i had i + nums[i] = farthest
// >= t... more directly: swapping OPT's jump-to-j for a jump reaching as far
// as possible never shortens what remains reachable, so it cannot increase the
// remaining jump count. Hence committing to "expand to the frontier" per jump
// is never worse than any specific choice of landing spot -- which is why we
// never decide WHERE to land, only HOW FAR the aggregate can go.
//
// COMPLEXITY
// ----------
// Time:  O(n) — single pass; each index extends `farthest` exactly once.
// Space: O(1) — three scalars. Contrast with BFS on explicit graph edges:
//        O(n^2) time/space for the same answer on this structure.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// Returns the minimum number of jumps to reach the last index.
// Preconditions per LeetCode: nums.size() >= 1 and the last index is always
// reachable (we still return a well-defined count without re-verifying).
int jump(const std::vector<int>& nums) {
  int n = static_cast<int>(nums.size());
  int jumps = 0;      // BFS depth committed so far.
  int curEnd = 0;     // Rightmost index reachable with exactly `jumps` jumps.
  int farthest = 0;   // Rightmost index reachable with `jumps + 1` jumps.

  // Scan up to n-2: once curEnd covers the last index no further jump is
  // needed, and extending from the last index itself would be meaningless.
  for (int i = 0; i < n - 1; ++i) {
    // This index is inside the current layer, so its jump feeds the NEXT
    // layer's boundary. Irrevocable local update -- route stays irrelevant.
    farthest = std::max(farthest, i + nums[i]);

    // Current layer exhausted at i: the only way onward is one more jump,
    // and the best that jump can do is reach the accumulated frontier.
    if (i == curEnd) {
      ++jumps;
      curEnd = farthest;

      // Early exit: the frontier already covers the last index.
      if (curEnd >= n - 1) {
        break;
      }
    }
  }

  return jumps;
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
    // Two layers: {0} -> {1, 2} -> {3, 4}. Answer 2.
    check(jump({2, 3, 1, 1, 4}) == 2, "[2,3,1,1,4] -> 2 jumps");
  }

  {
    // A mid-array zero does not hurt here: layer 1 still reaches index 4.
    check(jump({2, 3, 0, 1, 4}) == 2, "[2,3,0,1,4] -> zero crossed, 2 jumps");
  }

  {
    // Already standing on the last index: zero jumps needed.
    check(jump({0}) == 0, "[0] single element -> 0 jumps");
  }

  {
    check(jump({7}) == 0, "[7] single element -> 0 jumps");
  }

  {
    // All 1s: forced staircase, one jump per step -- the worst case.
    check(jump({1, 1, 1, 1}) == 3, "[1,1,1,1] -> staircase forces 3 jumps");
  }

  {
    // One giant first jump ends the problem immediately after one commit.
    check(jump({5, 1, 1, 1, 1}) == 1, "[5,1,1,1,1] -> single jump spans all");
  }

  {
    // The layer logic must extend from the FARTHEST position, not the first:
    // layer 1 = {1, 2}; only via index 2's value 3 does layer 2 cover the
    // last index 5. Two commits, answer 2.
    check(jump({2, 1, 3, 1, 1, 1}) == 2,
          "[2,1,3,1,1,1] -> multi-layer case needs the farthest extension");
  }

  {
    // Two jumps where the second must be chosen carefully:
    // 0 -> 1 or 2; from 2, value 4 reaches index 6 (last). Answer 2.
    check(jump({2, 1, 4, 1, 1, 1, 1}) == 2, "[2,1,4,1,1,1,1] -> 2 jumps via index 2");
  }

  {
    // Exact-boundary case: farthest lands exactly on n-1, triggering the
    // early-exit branch rather than falling off the loop end.
    check(jump({1, 2, 3}) == 2, "[1,2,3] -> exact-boundary landing, 2 jumps");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
