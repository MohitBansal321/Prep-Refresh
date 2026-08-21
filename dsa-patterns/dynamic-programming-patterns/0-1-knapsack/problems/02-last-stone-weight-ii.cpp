// ============================================================================
// LeetCode 1049 — Last Stone Weight II
// https://leetcode.com/problems/last-stone-weight-ii/
// ============================================================================
//
// PROBLEM
// -------
// You are given an array of stone weights. On each turn you pick any two
// stones x and y and smash them together: if x == y both are destroyed, and
// if x != y the stone of weight x is destroyed while the other becomes
// y - x. Repeat until at most one stone remains. Return the smallest
// possible weight of that last remaining stone (0 if none remains).
//
// Example: stones = [2,7,4,1,8,1] -> 1
//          stones = [31,26,33,21,40] -> 5
//
// APPROACH — 0/1 Knapsack, the PARTITION (minimize-the-difference) framing
// -------------------------------------------------------------------------
// The problem statement describes a simulation, and simulating it greedily
// (always smash the two heaviest, which is what LeetCode 1046 asks for) gives
// the WRONG answer here -- on [31,26,33,21,40] the greedy heap simulation
// yields 5 only by luck; on other inputs it overshoots, for the same
// structural reason greedy fails 0/1 Knapsack (see ../README.md, "Why Not
// Other Approaches?"). Do not simulate. Reformulate.
//
// STEP 1 — every sequence of smashes is a choice of SIGNS.
// Smashing x against y and keeping y - x is the same as writing (-x) + (+y).
// Chain the smashes and the final stone's weight is
//     | s1*w1 + s2*w2 + ... + sn*wn |   where every si is +1 or -1.
// So the reachable final weights are exactly the absolute values of signed
// sums of all the stones. (Every sign assignment is reachable by some smash
// order; this is the one step of the reduction worth convincing yourself of
// on paper rather than taking on faith.)
//
// STEP 2 — a sign assignment is a PARTITION into two piles.
// Call P the pile of stones that got a +, and N the pile that got a -. Then
//     answer = | sum(P) - sum(N) |  with  sum(P) + sum(N) = total.
// Substituting sum(N) = total - sum(P):
//     answer = | 2 * sum(P) - total |
// which is minimized by making sum(P) as close to total / 2 as possible.
//
// STEP 3 — "the largest reachable subset sum not exceeding total / 2" IS
// 0/1 Knapsack, using the value == weight trick from
// 01-partition-equal-subset-sum.cpp:
//     item i's WEIGHT = stones[i]
//     item i's VALUE  = stones[i]
//     CAPACITY        = total / 2   (integer division, deliberately)
// Maximize value, then answer = total - 2 * bestPileSum.
//
// So this file is 01-partition-equal-subset-sum.cpp with the boolean table
// promoted back to an integer table: instead of asking "is total/2 exactly
// reachable?" it asks "how close to total/2 can I get?". Problem 416 is the
// special case where the answer to THIS problem is 0.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// Using total/2 with integer division is not a rounding sloppiness -- it is
// load-bearing. When total is odd, total/2 floors, so the DP searches only
// the SMALLER of the two piles. That is exactly what you want: the two piles
// are symmetric (swapping their labels does not change |difference|), so
// restricting the search to the pile at or below the midpoint loses nothing
// and guarantees total - 2 * bestPileSum is non-negative. Search up to
// (total + 1) / 2 instead and you can produce a negative "weight".
//
// COMPLEXITY
// ----------
// Time:  O(n * total/2).
// Space: O(total/2) with the 1D sweep, O(n * total/2) for the 2D table.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// lastStoneWeightII2D — the literal 2D table, kept for readability.
//
// dp[i][w] = the largest pile sum <= w achievable using only the first i
//            stones. Identical in shape to knapsack01 in ../code.cpp, with
//            values[i] replaced by weights[i].
// ----------------------------------------------------------------------------
int lastStoneWeightII2D(const std::vector<int>& stones) {
  int total = 0;
  for (size_t k = 0; k < stones.size(); ++k) total += stones[k];

  int half = total / 2;  // Deliberate floor -- see header comment.
  int n = static_cast<int>(stones.size());

  std::vector<std::vector<int> > dp(n + 1, std::vector<int>(half + 1, 0));

  for (int i = 1; i <= n; ++i) {
    int w = stones[i - 1];  // Row i means "first i stones" -> index i - 1.
    for (int cap = 0; cap <= half; ++cap) {
      dp[i][cap] = dp[i - 1][cap];  // Put stone i in the OTHER pile.
      if (w <= cap) {
        // Put stone i in THIS pile. Both reads come from row i - 1, so the
        // stone cannot be placed twice.
        dp[i][cap] = std::max(dp[i][cap], dp[i - 1][cap - w] + w);
      }
    }
  }

  int bestPile = dp[n][half];
  // bestPile <= half <= total - bestPile, so this is never negative.
  return total - 2 * bestPile;
}

// ----------------------------------------------------------------------------
// lastStoneWeightII1D — the version to actually write in an interview.
// One row of length half + 1, capacity swept BACKWARD.
// ----------------------------------------------------------------------------
int lastStoneWeightII1D(const std::vector<int>& stones) {
  int total = 0;
  for (size_t k = 0; k < stones.size(); ++k) total += stones[k];

  int half = total / 2;
  std::vector<int> dp(half + 1, 0);

  for (size_t k = 0; k < stones.size(); ++k) {
    int w = stones[k];
    // BACKWARD. Forward would let one stone land in the same pile twice,
    // e.g. stones = [1], total = 1, half = 0 is safe, but stones = [1,1,1,1]
    // with half = 2 would let a single 1 be counted twice and report a pile
    // sum of 2 built from one stone. Backward reads dp[cap - w] before this
    // stone's pass can touch it.
    for (int cap = half; cap >= w; --cap) {
      dp[cap] = std::max(dp[cap], dp[cap - w] + w);
    }
  }

  return total - 2 * dp[half];
}

// ============================================================================
// Tests
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

  auto checkBoth = [&](const std::vector<int>& stones, int expected,
                       const std::string& label) {
    check(lastStoneWeightII2D(stones) == expected, "2D: " + label);
    check(lastStoneWeightII1D(stones) == expected, "1D: " + label);
  };

  {
    // LeetCode's own first example. total = 23, half = 11, and 11 is exactly
    // reachable as 7 + 4, so the split is [7,4] (=11) vs [2,1,8,1] (=12) and
    // the answer is 23 - 2*11 = 1.
    std::vector<int> stones;
    stones.push_back(2); stones.push_back(7); stones.push_back(4);
    stones.push_back(1); stones.push_back(8); stones.push_back(1);
    checkBoth(stones, 1, "[2,7,4,1,8,1] -> 1 (total 23, best pile 11)");
  }

  {
    // LeetCode's second example. total = 151, half = 75, best pile 73.
    std::vector<int> stones;
    stones.push_back(31); stones.push_back(26); stones.push_back(33);
    stones.push_back(21); stones.push_back(40);
    checkBoth(stones, 5, "[31,26,33,21,40] -> 5 (total 151, best pile 73)");
  }

  {
    // Single stone: nothing to smash it against, so it survives intact.
    // half = 0 here, so the capacity loop never runs at all -- the base case
    // of an empty table must still produce the right answer.
    std::vector<int> stones;
    stones.push_back(1);
    checkBoth(stones, 1, "[1] -> 1 (one stone, nothing to smash it with)");
  }

  {
    std::vector<int> stones;
    stones.push_back(100);
    checkBoth(stones, 100, "[100] -> 100 (single large stone survives whole)");
  }

  {
    // Two equal stones annihilate each other exactly.
    std::vector<int> stones;
    stones.push_back(1); stones.push_back(1);
    checkBoth(stones, 0, "[1,1] -> 0 (equal pair destroys itself)");
  }

  {
    std::vector<int> stones;
    stones.push_back(1); stones.push_back(2);
    checkBoth(stones, 1, "[1,2] -> 1 (total 3, best pile 1)");
  }

  {
    // All-same, even count -> perfect split, answer 0.
    std::vector<int> stones;
    for (int k = 0; k < 4; ++k) stones.push_back(10);
    checkBoth(stones, 0, "[10,10,10,10] -> 0 (all-same, even count)");
  }

  {
    // All-same, ODD count -> one stone is always left over.
    // total 15, half 7, best reachable pile <= 7 is 5, so 15 - 10 = 5.
    std::vector<int> stones;
    for (int k = 0; k < 3; ++k) stones.push_back(5);
    checkBoth(stones, 5, "[5,5,5] -> 5 (all-same, odd count leaves one behind)");
  }

  {
    // total 6, half 3, but the only reachable pile sums are {0,2,4,6}:
    // 3 itself is unreachable, so the best pile <= 3 is 2 and the answer is
    // 6 - 4 = 2. This is the case that proves the DP is doing real work
    // rather than just returning total % 2.
    std::vector<int> stones;
    for (int k = 0; k < 3; ++k) stones.push_back(2);
    checkBoth(stones, 2, "[2,2,2] -> 2 (half = 3 is UNREACHABLE, best pile 2)");
  }

  {
    // total 15, half 7, and 7 = 3 + 4 is exactly reachable -> answer 1.
    std::vector<int> stones;
    for (int k = 1; k <= 5; ++k) stones.push_back(k);
    checkBoth(stones, 1, "[1,2,3,4,5] -> 1 (3+4 = 7 hits the floor of 15/2)");
  }

  {
    // Cross-check against problem 416: a perfectly partitionable array must
    // report 0 here, since an equal split has difference 0.
    std::vector<int> stones;
    stones.push_back(1); stones.push_back(5);
    stones.push_back(11); stones.push_back(5);
    checkBoth(stones, 0, "[1,5,11,5] -> 0 (the LC 416 'true' case, seen here as 0)");
  }

  {
    // Empty input: no stones, no last stone. half = 0, loops do not run.
    std::vector<int> stones;
    checkBoth(stones, 0, "[] -> 0 (no stones at all)");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
