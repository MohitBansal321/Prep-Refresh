// ============================================================================
// LeetCode 1472 — Design Browser History
// ============================================================================
//
// PROBLEM
// -------
// Implement the BrowserHistory class:
//   - BrowserHistory(string homepage): starts with the homepage.
//   - void visit(string url): visits url from the current page. It clears up
//     all the forward history, then appends url.
//   - string back(int steps): move back in history at most `steps`; return
//     the current page after moving.
//   - string forward(int steps): move forward at most `steps`; return the
//     current page after moving.
//
// Example:
//   BrowserHistory b("leetcode.com");
//   b.visit("google.com"); b.visit("facebook.com"); b.visit("youtube.com");
//   b.back(1)      -> "facebook.com"
//   b.back(1)      -> "google.com"
//   b.forward(1)   -> "facebook.com"
//   b.visit("linkedin.com")          // clears forward history (youtube)
//   b.forward(2)   -> "linkedin.com" // clamped: nothing ahead of it
//   b.back(2)      -> "google.com"
//   b.back(7)      -> "leetcode.com" // clamped at homepage
//
// APPROACH — history array plus a cursor index
// ---------------------------------------------
// This is the LRU module's doubly-linked-list primitive wearing different
// clothes: a linear sequence of "pages", a moving cursor, and O(1)-style
// relinking around that cursor. But notice what the problem does NOT need —
// keyed lookup. There is no "jump to the page named X". Without a hash map,
// the reason to hand-roll prev/next pointers disappears, and index
// arithmetic on one contiguous array beats pointer chasing outright:
//
//   history_: vector<string> of visited pages, in visit order.
//   cur_    : index of the current page.
//
//   visit(url): truncate everything AFTER cur_ (this IS "clearing forward
//               history" — the same unlink-away-the-tail operation as in an
//               LRU list, done by resize instead of pointer surgery), then
//               push url and advance the cursor.
//   back(s)   : cur_ = max(0, cur_ - s)        — clamped jump.
//   forward(s): cur_ = min(size-1, cur_ + s)   — clamped jump.
//
// Why is truncation O(n) acceptable here? It destroys n-cur_ entries, so it
// costs no more than the work those entries represent; amortized over the
// visits that created them it is O(1) per visit. And unlike a linked list,
// the vector keeps every page contiguous and cache-friendly.
//
// The design lesson this file exists for: "map for lookup + second structure
// for order" is the general shape, and when the LOOKUP half is absent, the
// second structure should be the simplest one that moves the cursor in O(1).
// A doubly linked list with manual pointers would work but buys nothing.
//
// COMPLEXITY
// ----------
// Time:  visit O(k) where k = pages discarded (amortized O(1) per visit);
//        back/forward O(1) each — pure integer arithmetic.
// Space: O(n) for the pages currently reachable from the cursor; truncated
//        pages are freed immediately rather than lingering off-cursor.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class BrowserHistory {
 public:
  explicit BrowserHistory(const std::string& homepage) {
    history_.push_back(homepage);
    cur_ = 0;
  }

  void visit(const std::string& url) {
    // Clear forward history: everything after the cursor is unreachable the
    // moment we navigate elsewhere. resize() both logically removes and
    // actually frees them — no stale tail lingers off to the right.
    history_.resize(static_cast<size_t>(cur_) + 1);
    history_.push_back(url);
    ++cur_;
  }

  std::string back(int steps) {
    cur_ = std::max(0, cur_ - steps);  // clamp at the homepage
    return history_[static_cast<size_t>(cur_)];
  }

  std::string forward(int steps) {
    int last = static_cast<int>(history_.size()) - 1;
    cur_ = std::min(last, cur_ + steps);  // clamp at the newest page
    return history_[static_cast<size_t>(cur_)];
  }

 private:
  std::vector<std::string> history_;
  int cur_;  // index of the current page in history_
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
    // LeetCode 1472 official example sequence.
    BrowserHistory bh("leetcode.com");
    bh.visit("google.com");     // leetcode -> google
    bh.visit("facebook.com");   // google -> facebook
    bh.visit("youtube.com");    // facebook -> youtube

    check(bh.back(1) == "facebook.com", "LC example: back(1) -> facebook.com");
    check(bh.back(1) == "google.com", "LC example: back(1) -> google.com");
    check(bh.forward(1) == "facebook.com", "LC example: forward(1) -> facebook.com");

    bh.visit("linkedin.com");   // clears youtube from forward history
    check(bh.forward(2) == "linkedin.com",
          "LC example: visit cleared forward history; forward(2) clamps at linkedin");
    check(bh.back(2) == "google.com", "LC example: back(2) -> google.com");
    check(bh.back(7) == "leetcode.com",
          "LC example: back(7) clamps at the homepage");
  }

  {
    // Edge: oversized jumps clamp in both directions; forward history stays
    // usable again after being rebuilt by new visits.
    BrowserHistory bh("home");
    bh.visit("a");
    bh.visit("b");
    check(bh.forward(5) == "b", "forward past the end clamps at newest page");
    check(bh.back(100) == "home", "back past the start clamps at homepage");
    bh.visit("c");  // forward history (nothing left) then gains "c"
    check(bh.forward(3) == "c", "forward stays clamped after fresh visit");
    check(bh.back(1) == "home", "back(1) from 'c' lands on homepage");
  }

  {
    // Edge: revisiting after going back must discard the abandoned branch,
    // not append behind it (the classic browser-history correctness bug).
    BrowserHistory bh("x.com");
    bh.visit("y.com");
    bh.visit("z.com");
    bh.back(2);            // at x.com; y,z are forward history
    bh.visit("w.com");     // y.com and z.com must now be gone
    check(bh.back(1) == "x.com",
          "visit after back discards the abandoned forward branch");
    check(bh.forward(10) == "w.com",
          "old forward pages are unreachable after branching visit");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
