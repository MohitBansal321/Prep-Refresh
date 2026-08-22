// ============================================================================
// LeetCode 464 — Can I Win
// ============================================================================
//
// PROBLEM
// -------
// In the "100 game" two players take turns adding, to a running total, any
// integer from 1 to `maxChoosableInteger`. Each number may be used by
// EITHER player AT MOST ONCE (the pool is shared and shrinks). The player
// whose addition makes the running total reach or exceed `desiredTotal`
// wins. Assuming both players play optimally, return true if the FIRST
// player can force a win, otherwise false.
//
// Example: maxChoosableInteger = 10, desiredTotal = 11 -> false.
//   Whatever the first player picks (x), the second can pick a number
//   >= 11 - x from the remaining pool and win immediately.
//
// APPROACH — Game-theoretic bitmask DP with memoization
// -----------------------------------------------------
// Why bitmask DP? The shared, finite pool of numbers IS the game state:
// which numbers remain changes what moves are legal, and two positions
// with the same total-but-different pools are entirely different games.
// maxChoosableInteger <= 20, so the pool fits in an int mask — bit i set
// means number i+1 has been consumed. This lands squarely in the pattern's
// recognition signal (see ../README.md): small n, subset-identity state,
// plus the adversarial twist of alternating turns.
//
// State: solve(mask, remaining) = can the player ABOUT TO MOVE force a win,
// given the used numbers are exactly `mask` and they need to add at least
// `remaining` to reach desiredTotal?
//
// Win condition for the mover: there exists an unused number x such that
// EITHER x >= remaining (instant win) OR the OPPONENT cannot win from the
// resulting position: !solve(mask | bit(x), remaining - x). This is
// minimax on a win/loss lattice — "I can win iff some move leaves you
// unable to force a win."
//
// WHY MEMOIZING ON THE MASK ALONE IS SOUND: remaining is fully determined
// by mask (remaining = desiredTotal - sum of used numbers), so no second
// memo dimension is needed. Equally important, WHOSE TURN it is does not
// need storing either: solve() is defined relative to "the player about to
// move", and each recursive call flips perspective automatically. One int
// per mask suffices: -1 unknown, 0 losing, 1 winning.
//
// Edge cases handled up front:
//   - desiredTotal <= 0: trivially won before any move -> true.
//   - Sum of the entire pool < desiredTotal: nobody can EVER win -> false
//     (without this check the recursion would explore every mask finding
//     no winner; correct but wasteful, and LeetCode expects false).
//
// COMPLEXITY
// ----------
// Time:  O(2^n * n) — 2^n masks, each scanning n candidate numbers once.
// Space: O(2^n) for the memo table (plus O(n) recursion stack).
// ============================================================================
#include <iostream>
#include <string>
#include <vector>

class Solution {
 public:
  bool canIWin(int maxChoosableInteger, int desiredTotal) {
    // Trivial win: the target is already met before anyone moves.
    if (desiredTotal <= 0) return true;

    // Nobody can ever reach the total if even exhausting the whole pool
    // falls short: n*(n+1)/2 numbers sum formula over 1..maxChoosableInteger.
    long long poolSum = static_cast<long long>(maxChoosableInteger) *
                        (maxChoosableInteger + 1) / 2;
    if (poolSum < static_cast<long long>(desiredTotal)) return false;

    n_ = maxChoosableInteger;
    // -1 = not computed, 0 = current mover loses, 1 = current mover wins.
    memo_.assign(1 << n_, -1);
    return solve(0, desiredTotal) == 1;
  }

 private:
  int solve(int mask, int remaining) {
    int& cached = memo_[mask];
    if (cached != -1) return cached;

    // Try every unused number as this player's move.
    for (int x = 1; x <= n_; ++x) {
      int bit = 1 << (x - 1);
      if (mask & bit) continue;  // already taken by someone

      // Instant win, or the move strands the opponent in a losing state.
      if (x >= remaining || solve(mask | bit, remaining - x) == 0) {
        return cached = 1;
      }
    }
    return cached = 0;  // every legal move lets the opponent force a win
  }

  std::vector<int> memo_;
  int n_ = 0;
};

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

  Solution sol;

  {
    // Classic example: second player always has a counter-pick >= 11 - x.
    check(sol.canIWin(10, 11) == false, "max=10, total=11 -> first loses");
  }

  {
    // Trivial edge cases around zero/one.
    check(sol.canIWin(10, 0) == true, "total=0 -> already met, first wins");
    check(sol.canIWin(10, 1) == true, "total=1 -> pick 1 and win");
  }

  {
    // Pool cannot possibly reach the total: guaranteed loss for both sides,
    // so the first player certainly cannot force a win.
    check(sol.canIWin(5, 50) == false,
          "max=5, total=50 -> false (pool sum 15 < 50)");
    // Pool sum for max=20 is exactly 210; asking for one more than that
    // exercises the boundary of the same early-exit check.
    check(sol.canIWin(20, 211) == false,
          "max=20, total=211 -> false (pool sum 210 < 211)");
  }

  {
    // Small hand-verifiable position: with {1..4} and total 6, the first
    // player must open with 1 (any other opener is immediately countered),
    // after which every reply still loses.
    check(sol.canIWin(4, 6) == true, "max=4, total=6 -> first wins via 1");
    // With {1..3} and total 6, opening with 3 forces the issue: the second
    // player can never reach 6 on their own turn, while the first player's
    // second move always closes the game.
    check(sol.canIWin(3, 6) == true, "max=3, total=6 -> first wins via 3");
  }

  {
    // Known LeetCode case reachable only through deep optimal play.
    check(sol.canIWin(10, 40) == false, "max=10, total=40 -> first loses");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
