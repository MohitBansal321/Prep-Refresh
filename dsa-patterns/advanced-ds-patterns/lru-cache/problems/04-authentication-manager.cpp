// ============================================================================
// LeetCode 1797 — Design Authentication Manager
// ============================================================================
//
// PROBLEM
// -------
// An authentication manager keeps track of login tokens. Each token lives
// for `timeToLive` seconds, measured from the moment it was (last) valid.
//
// Implement AuthenticationManager:
//   - AuthenticationManager(int timeToLive): constructs the manager with
//     the given time-to-live.
//   - void generate(string tokenId, int currentTime): create a token that
//     expires at currentTime + timeToLive.
//   - void renew(string tokenId, int currentTime): if the token exists AND
//     is still unexpired at currentTime, extend its life by timeToLive FROM
//     ITS CURRENT EXPIRY (not from currentTime); otherwise do nothing.
//   - int countUnexpiredTokens(int currentTime): how many tokens are still
//     unexpired at currentTime.
//
// Example 1:
//   AuthenticationManager auth(5);
//   auth.renew("aaa", 1);            // unknown -> ignored
//   auth.generate("aaa", 2);         // expires at 7
//   auth.countUnexpiredTokens(6);    // -> 1
//   auth.generate("bbb", 7);         // expires at 12
//   auth.renew("aaa", 8);            // expired (7 <= 8) -> ignored
//   auth.renew("bbb", 10);           // extends to 12 + 5 = 17
//   auth.countUnexpiredTokens(15);   // -> 1  ("bbb")
//
// APPROACH — hash map for state + lazy-pruning min-heap for expiry
// -----------------------------------------------------------------
// This is the LRU module's contrast case: eviction driven by an EXTERNAL
// deadline rather than by capacity or recency. Two consequences follow,
// and each one picks a structure:
//
//   1. The ordering key is a deadline WE DO NOT CONTROL. That is the one
//      situation where a min-heap IS the right tool (see the README's
//      "Why Not Other Approaches"): we never re-key an existing entry by
//      choice — keys only ever get NEW entries appended when a token is
//      generated or renewed.
//
//   2. Nothing forces us to clean up on a schedule. Expiry only matters
//      when someone asks (a renew that must validate, a count that must
//      be exact). So pruning is LAZY: whenever countUnexpiredTokens runs,
//      pop every heap entry whose expiry <= now. A token whose stored
//      expiry differs from the popped entry's value is stale — it was
//      superseded by a renew — so it is skipped, not erased.
//
// Why lazy pruning beats eager cleanup: generate/renew stay O(log n) with
// no scanning of "will expire soon" entries, and the heap only ever does
// work proportional to entries that genuinely died. Every heap entry is
// popped at most once, so total prune cost across any operation sequence
// is bounded by total entries ever pushed — amortized O(log n) per op.
//
// Renewal subtlety worth stating aloud: renewal adds timeToLive to the
// OLD expiry, not to currentTime (that is why "bbb" renewed at 10 expires
// at 17, not 15), and a token counts as expired the moment now >= its
// expiry — boundary included.
//
// COMPLEXITY
// ----------
// Time:  generate O(log n); renew O(log n); count O(k log n) where k =
//               tokens actually pruned (amortized O(log n)).
// Space: O(n) — one map entry plus at most one heap entry per push
//               (stale heap entries are bounded by the number of pushes).
// ============================================================================

#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class AuthenticationManager {
 public:
  explicit AuthenticationManager(int timeToLive) : ttl_(timeToLive) {}

  void generate(const std::string& tokenId, int currentTime) {
    int expiry = currentTime + ttl_;
    expiry_[tokenId] = expiry;
    // Min-heap ordered by expiry time: soonest death on top.
    heap_.push(std::make_pair(expiry, tokenId));
  }

  void renew(const std::string& tokenId, int currentTime) {
    std::unordered_map<std::string, int>::iterator it = expiry_.find(tokenId);
    // Must exist AND strictly outlive currentTime (expiry == now means it
    // just died — boundary included, matching the problem's examples).
    if (it == expiry_.end() || it->second <= currentTime) return;
    it->second += ttl_;  // extends from the OLD expiry, not from now
    heap_.push(std::make_pair(it->second, tokenId));
  }

  int countUnexpiredTokens(int currentTime) {
    prune(currentTime);
    return static_cast<int>(expiry_.size());
  }

 private:
  // Pop expired entries off the heap and drop them from the map. A popped
  // entry whose map value no longer matches is a STALE duplicate left behind
  // by a renew — skip it; its newer entry carries the truth.
  void prune(int currentTime) {
    while (!heap_.empty() && heap_.top().first <= currentTime) {
      std::pair<int, std::string> top = heap_.top();
      heap_.pop();
      std::unordered_map<std::string, int>::iterator it = expiry_.find(top.second);
      if (it != expiry_.end() && it->second == top.first) {
        expiry_.erase(it);  // genuinely dead token: remove its state
      }
    }
  }

  int ttl_;
  std::unordered_map<std::string, int> expiry_;  // token -> absolute expiry
  // (min-heap by expiry, then token string — the tiebreak is irrelevant to
  // correctness, only needed to make pair comparison well-formed)
  std::priority_queue<std::pair<int, std::string>,
                      std::vector<std::pair<int, std::string> >,
                      std::greater<std::pair<int, std::string> > >
      heap_;
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

  {
    // LeetCode 1797, Example 1 — full official sequence.
    AuthenticationManager auth(5);
    auth.renew("aaa", 1);  // unknown token: must be a silent no-op
    auth.generate("aaa", 2);
    check(auth.countUnexpiredTokens(6) == 1, "LC example: count(6) == 1");
    auth.generate("bbb", 7);
    auth.renew("aaa", 8);  // aaa expired at 7 (<= 8): no-op
    auth.renew("bbb", 10); // bbb extends from expiry 12 -> 17 (NOT from 10)
    check(auth.countUnexpiredTokens(15) == 1,
          "LC example: count(15) == 1 -- aaa stayed dead, bbb alive until 17");
  }

  {
    // Edge: expiry boundary is inclusive — a token dies exactly at its
    // expiry second.
    AuthenticationManager auth(5);
    auth.generate("x", 1);  // expires at 6
    check(auth.countUnexpiredTokens(5) == 1, "one second before expiry: alive");
    check(auth.countUnexpiredTokens(6) == 0, "at the expiry second: dead");
  }

  {
    // Edge: renewal extends from the current expiry, not from renewal time.
    AuthenticationManager auth(10);
    auth.generate("t", 0);   // expires at 10
    auth.renew("t", 3);      // extends to 20 (not 13)
    check(auth.countUnexpiredTokens(15) == 1,
          "renewal extends from old expiry: alive past original death");
    check(auth.countUnexpiredTokens(20) == 0,
          "extended token still dies at expiry + ttl");
  }

  {
    // Edge: renewing an unknown token, and renewing twice — the first renew
    // leaves a stale heap entry that prune must skip without erasing the
    // live state.
    AuthenticationManager auth(4);
    auth.renew("ghost", 100);
    check(auth.countUnexpiredTokens(101) == 0,
          "renewing an unknown token changes nothing");
    auth.generate("dup", 0);  // expires 4
    auth.renew("dup", 1);     // -> 8, stale heap entry (4,"dup") remains
    auth.renew("dup", 2);     // -> 12, another stale entry
    check(auth.countUnexpiredTokens(5) == 1,
          "stale heap entries skipped: renewed token survives early prunes");
    check(auth.countUnexpiredTokens(11) == 1,
          "stale heap entries skipped again at the next prune");
    check(auth.countUnexpiredTokens(12) == 0,
          "final expiry honoured despite multiple stale heap entries");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
