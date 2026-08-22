// ============================================================================
// LeetCode 55 — Jump Game
// ============================================================================
//
// PROBLEM
// -------
// You are given an integer array `nums`. You are initially positioned at the
// first index, and each element nums[i] represents your MAXIMUM jump length
// from that position (you may jump fewer). Return true if you can reach the
// last index, false otherwise.
//
// Example: nums = [2, 3, 1, 1, 4]  ->  true   (0 -> 1 -> 4)
//          nums = [3, 2, 1, 0, 4]  ->  false  (every route gets stuck at index 3)
//
// APPROACH — Greedy running frontier (no sort)
// --------------------------------------------
// The greedy insight: WHICH jumps you take is irrelevant; only HOW FAR you can
// eventually get matters. So maintain a single scalar `farthest` — the highest
// index reachable using any combination of jumps from indices already visited —
// and sweep left to right once:
//   - if the cursor i ever overtakes farthest (i > farthest), index i is
//     unreachable and so is everything after it: return false immediately.
//   - otherwise extend the frontier: farthest = max(farthest, i + nums[i]).
//
// WHY THE GREEDY CHOICE IS SAFE (the proof obligation)
// The "choice" here is not picking a jump — it is the claim that a single
// frontier scalar suffices. Exchange argument: suppose some optimal route R
// reaches index k via a path whose intermediate hops we never track. Every hop
// of R lands on an index <= max over its prefix of (i + nums[i]) — i.e. every
// index R touches lies within our frontier at the moment the cursor passes it.
// So replacing R's specific hops with "anything that keeps the frontier
// maximal" reaches every index R reaches. The set of reachable indices depends
// only on the frontier's extent, not on the route that produced it — which is
// why one int of state captures EVERYTHING the past decisions imply.
//
// COMPLEXITY
// ----------
// Time:  O(n) — one pass, O(1) work per element. No sort: the original order
//               already makes each local update safe.
// Space: O(1) — one scalar. Contrast with DP over reachability: O(n^2) time,
//        O(n) space — this problem never needed it.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// Returns true if the last index of `nums` is reachable from index 0.
bool canJump(const std::vector<int>& nums) {
  // `farthest` = highest index reachable so far. Starts at 0 because we begin
  // standing on index 0 without needing any jump.
  int farthest = 0;

  for (size_t i = 0; i < nums.size(); ++i) {
    // Cursor overtook the frontier: index i (and everything after it) is
    // unreachable no matter what. Bail out early instead of scanning on.
    if (static_cast<int>(i) > farthest) {
      return false;
    }
    // Standing on i is legal, so its jump extends (or fails to extend) the
    // frontier. Irrevocable local commitment -- we never reconsider it.
    farthest = std::max(farthest, static_cast<int>(i) + nums[i]);
  }

  // The loop finished without the cursor ever overtaking the frontier, so the
  // last index was reachable when we walked past it.
  return true;
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
    // Classic reachable case: 0 -> 1 (jump 2 or 3), then 1 + 3 = 4 = last index.
    check(canJump({2, 3, 1, 1, 4}) == true, "[2,3,1,1,4] -> reachable");
  }

  {
    // Classic stuck case: index 3 holds a 0; farthest stalls at exactly 3,
    // cursor reaches 4 > 3 -> unreachable.
    check(canJump({3, 2, 1, 0, 4}) == false, "[3,2,1,0,4] -> stuck at index 3");
  }

  {
    // Single element: already standing on the last index, zero jumps needed.
    check(canJump({0}) == true, "[0] single element -> trivially reachable");
  }

  {
    check(canJump({5}) == true, "[5] single element with jump room -> reachable");
  }

  {
    // A zero early in the array is harmless if a later index re-extends the
    // frontier past it: farthest goes 2 -> 2 -> 2 -> ... wait, actually
    // farthest = max(2, 0)=2, then max(2, 2)=2 -- but the last index IS 2,
    // so the cursor never overtakes the frontier. Reachable by stalling.
    check(canJump({2, 0, 0}) == true, "[2,0,0] -> zeros crossed, still reachable");
  }

  {
    // Frontier lands EXACTLY on a zero and cannot pass it: farthest = 1 after
    // index 0; at i=1, nums[1]=0 keeps farthest at 1; i=2 > 1 -> false.
    check(canJump({1, 0, 0}) == false, "[1,0,0] -> frontier dies exactly on a zero");
  }

  {
    // Landing EXACTLY on the last index is success even if that slot is a
    // zero: the jump from 0 reaches index 1 == last index, done.
    check(canJump({1, 0}) == true, "[1,0] -> exact landing on last index");
  }

  {
    // One big first jump spans several zeros: farthest = 4 covers all of them.
    check(canJump({4, 0, 0, 0, 0}) == true, "[4,0,0,0,0] -> big jump spans zeros");
  }

  {
    // Big jump ALMOST spans them: farthest = 3, last index 4 -> false.
    check(canJump({3, 0, 0, 0, 0}) == false, "[3,0,0,0,0] -> falls one short");
  }

  {
    // Long staircase of 1s: frontier advances exactly one step per element --
    // worst case for the early-exit, still linear.
    check(canJump({1, 1, 1, 1, 1, 1}) == true, "[1,1,1,1,1,1] -> staircase reachable");
  }

  {
    // Staircase that breaks mid-way: farthest tracks i exactly until the 0.
    check(canJump({1, 1, 1, 0, 1}) == false, "[1,1,1,0,1] -> staircase blocked");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
