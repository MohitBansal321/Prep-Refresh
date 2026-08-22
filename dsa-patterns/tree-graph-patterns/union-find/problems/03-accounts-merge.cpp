// ============================================================================
// LeetCode 721 — Accounts Merge  (Hard)
// ============================================================================
//
// PROBLEM
// -------
// Each account is a list [name, email1, email2, ...]. Two accounts belong to
// the SAME person if they share at least one email address (names can be
// duplicated across different people; only shared emails prove identity).
// Merge all accounts that belong to the same person into one account: the
// person's name, followed by all their emails in sorted order. Return the
// merged accounts in any order.
//
// Example:
//   [["John","johnsmith@mail.com","john_newyork@mail.com"],
//    ["John","johnsmith@mail.com","john00@mail.com"],
//    ["Mary","mary@mail.com"],
//    ["John","johnnybravo@mail.com"]]
//   ->
//   [["John","john00@mail.com","john_newyork@mail.com","johnsmith@mail.com"],
//    ["Mary","mary@mail.com"],
//    ["John","johnnybravo@mail.com"]]
//   (the first two "John" accounts share johnsmith@mail.com, so they merge;
//    the fourth "John" shares no email with anyone, so it stays separate —
//    proving that name alone is NEVER a valid signal for merging.)
//
// APPROACH — Union Find over ACCOUNT INDICES, keyed by shared emails
// ---------------------------------------------------------------------
// The elements being unioned are not emails directly — they are ACCOUNT
// INDICES (0, 1, 2, ... for each row in the input). Two accounts (i, j)
// belong to the same person if and only if they are connected through a
// CHAIN of shared emails, exactly the "transitive connectivity" question
// Union Find answers: account 0 and account 1 share "johnsmith@mail.com"
// directly; if account 2 shared a DIFFERENT email with account 1, then
// accounts 0, 1, and 2 would all end up in the same group even without 0
// and 2 sharing anything directly — that transitive merging is precisely
// why this cannot be solved with a single pass of "group by email."
//
// Algorithm:
//   1. Build a DisjointSet with one slot per ACCOUNT (n accounts).
//   2. Build a map from email -> the index of the FIRST account seen that
//      owns that email (`email_to_account`).
//   3. For every account i, for every email it owns: if that email was
//      already seen belonging to some earlier account j, union(i, j) —
//      this account and that earlier one are the same person. Otherwise,
//      record email_to_account[email] = i.
//   4. After all unions, group every email by its account's ROOT
//      (find(i)), using an ordered structure (std::map<std::string,...> or
//      a set) so the final emails per person come out sorted for free.
//   5. For each root group, the output row is
//      [accounts[root][0] (the name), sorted emails...].
//
// Why Union Find specifically (vs. building an explicit email-adjacency
// graph and running DFS/BFS): both work and are the same asymptotic
// complexity here since this is a single batch input, not a stream of
// incremental queries. Union Find is used because it sidesteps building an
// explicit email-to-email adjacency list entirely — every email only ever
// needs to remember the ONE account index it first appeared under, and the
// DisjointSet handles all the transitive merging bookkeeping. This is the
// same "elements identified by a shared key, merge by that key" shape you
// will meet again in problems like "merge similar items" or "sentence
// similarity" style questions.
//
// COMPLEXITY
// ----------
// Let n = number of accounts, k = average emails per account.
// Time:  O(n * k * alpha(n))  for the union passes,
//        + O(n * k * log(n * k)) for grouping into a sorted map by email
//        (dominates in practice) -> overall O(n * k * log(n * k)).
// Space: O(n * k) — the DisjointSet arrays plus the email-to-account map
//        and the final grouped-emails structure.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class DisjointSet {
 public:
  explicit DisjointSet(int n) : parent_(n), rank_(n, 0) {
    std::iota(parent_.begin(), parent_.end(), 0);
  }

  int find(int x) {
    if (parent_[x] != x) {
      parent_[x] = find(parent_[x]);  // path compression
    }
    return parent_[x];
  }

  void unionSets(int x, int y) {
    int root_x = find(x);
    int root_y = find(y);
    if (root_x == root_y) return;

    if (rank_[root_x] < rank_[root_y]) std::swap(root_x, root_y);
    parent_[root_y] = root_x;
    if (rank_[root_x] == rank_[root_y]) ++rank_[root_x];
  }

 private:
  std::vector<int> parent_;
  std::vector<int> rank_;
};

std::vector<std::vector<std::string>> accountsMerge(
    const std::vector<std::vector<std::string>>& accounts) {
  int n = static_cast<int>(accounts.size());
  DisjointSet dsu(n);

  // email -> index of the first account seen owning it.
  std::unordered_map<std::string, int> email_to_account;

  for (int i = 0; i < n; ++i) {
    for (size_t e = 1; e < accounts[i].size(); ++e) {  // index 0 is the name
      const std::string& email = accounts[i][e];
      auto it = email_to_account.find(email);
      if (it == email_to_account.end()) {
        email_to_account[email] = i;
      } else {
        dsu.unionSets(i, it->second);  // same email -> same person
      }
    }
  }

  // Group emails by root account. std::map keeps emails sorted per group
  // automatically (it is a sorted associative container), which is exactly
  // the output format the problem requires.
  std::unordered_map<int, std::set<std::string>> root_to_emails;
  for (int i = 0; i < n; ++i) {
    int root = dsu.find(i);
    for (size_t e = 1; e < accounts[i].size(); ++e) {
      root_to_emails[root].insert(accounts[i][e]);
    }
  }

  std::vector<std::vector<std::string>> result;
  result.reserve(root_to_emails.size());
  for (const auto& group : root_to_emails) {
    const int root = group.first;
    const std::set<std::string>& emails = group.second;
    std::vector<std::string> row;
    row.push_back(accounts[root][0]);  // the person's name
    for (const auto& email : emails) {
      row.push_back(email);
    }
    result.push_back(std::move(row));
  }

  return result;
}

// Test helper: compare merged results ignoring row order (since the problem
// allows any order), but each row's own emails must already be sorted (the
// grouping step guarantees that) and the row's own contents must match
// exactly one expected row.
bool sameMergedAccounts(std::vector<std::vector<std::string>> actual,
                         std::vector<std::vector<std::string>> expected) {
  if (actual.size() != expected.size()) return false;
  std::sort(actual.begin(), actual.end());
  std::sort(expected.begin(), expected.end());
  return actual == expected;
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
    std::vector<std::vector<std::string>> accounts = {
        {"John", "johnsmith@mail.com", "john_newyork@mail.com"},
        {"John", "johnsmith@mail.com", "john00@mail.com"},
        {"Mary", "mary@mail.com"},
        {"John", "johnnybravo@mail.com"},
    };
    std::vector<std::vector<std::string>> expected = {
        {"John", "john00@mail.com", "john_newyork@mail.com", "johnsmith@mail.com"},
        {"Mary", "mary@mail.com"},
        {"John", "johnnybravo@mail.com"},
    };
    check(sameMergedAccounts(accountsMerge(accounts), expected),
          "classic example: two Johns merge via shared email, third John stays separate");
  }

  {
    // Chain merge: A shares with B, B shares with C, but A and C share
    // nothing directly -- Union Find must merge all three transitively.
    std::vector<std::vector<std::string>> accounts = {
        {"A", "a@mail.com", "shared1@mail.com"},
        {"A", "shared1@mail.com", "shared2@mail.com"},
        {"A", "shared2@mail.com", "z@mail.com"},
    };
    auto result = accountsMerge(accounts);
    check(result.size() == 1, "transitive chain of 3 accounts merges into exactly 1");
    if (result.size() == 1) {
      std::vector<std::string> expected_emails = {"a@mail.com", "shared1@mail.com",
                                                    "shared2@mail.com", "z@mail.com"};
      std::vector<std::string> actual_emails(result[0].begin() + 1, result[0].end());
      check(actual_emails == expected_emails,
            "merged account's emails are the union of all three, sorted");
    } else {
      check(false, "merged account's emails are the union of all three, sorted");
    }
  }

  {
    // No shared emails at all: every account stays its own person.
    std::vector<std::vector<std::string>> accounts = {
        {"Alice", "alice@mail.com"},
        {"Bob", "bob@mail.com"},
        {"Carol", "carol@mail.com"},
    };
    auto result = accountsMerge(accounts);
    check(result.size() == 3, "no shared emails -> 3 accounts stay separate");
  }

  {
    std::vector<std::vector<std::string>> accounts = {
        {"Gabe", "Gabe0@m.co", "Gabe3@m.co", "Gabe1@m.co"},
        {"Kevin", "Kevin3@m.co", "Kevin5@m.co", "Kevin0@m.co"},
        {"Ethan", "Ethan5@m.co", "Ethan4@m.co", "Ethan0@m.co"},
        {"Hanzo", "Hanzo3@m.co", "Hanzo1@m.co", "Hanzo0@m.co"},
        {"Fern", "Fern5@m.co", "Fern1@m.co", "Fern0@m.co"},
    };
    auto result = accountsMerge(accounts);
    check(result.size() == 5, "five accounts with no overlapping emails -> 5 groups");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
