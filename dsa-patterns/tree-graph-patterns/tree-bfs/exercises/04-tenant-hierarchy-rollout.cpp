// ============================================================================
// EXERCISE 04 (Real-World Challenge) — Tenant Hierarchy Config Rollout
// ============================================================================
//
// This file is a SELF-TEST, not a solution. The assertions in main() are the
// specification; fill in the two stubs until they all print [PASS].
//
//   g++ -std=c++17 -Wall exercises/04-tenant-hierarchy-rollout.cpp -o /tmp/ex04 && /tmp/ex04
//
// It compiles as-is, so you get a failing baseline immediately. No solution is
// provided anywhere in this repo — see ../exercises.md for the full write-up.
//
// WHAT THIS FILE COVERS, AND WHAT IT DOES NOT
// -------------------------------------------
// ../exercises.md poses five parts. Parts 1 and 4 are algorithms, so they are
// tested here. Parts 2 (per-level metrics), 3 (Redis checkpointing) and 5 (the
// 200k-sub-account space argument) are DESIGN answers — write them in prose;
// no harness can grade them. Do not skip them: part 5 is the one an interviewer
// actually asks.
//
// The real service queries Postgres. Here, TenantStore stands in for the DB and
// counts your queries, because the whole point of part 1 is the query PATTERN:
// one batched query per LEVEL, not one per node. Tests 5 and 6 fail a solution
// that is functionally correct but queries per node — which is exactly the bug
// that survives code review and then melts the database.
//
// TASK
// ----
//   getLevel(store, rootId, depth)  -> the tenant IDs exactly `depth` levels
//                                      below rootId (depth 0 == {rootId}).
//   findNearestOverride(parentOf, hasOverride, tenantId)
//                                   -> hops from tenantId up to its nearest
//                                      ancestor-or-self holding an override,
//                                      or -1 if no such ancestor exists.
//
// HINTS — read one at a time, only when genuinely stuck.
//   [1] After ~10 min: the level-size snapshot from ../code.cpp becomes the
//       whole frontier here. You hand the entire frontier to one query.
//   [2] After ~15 min: getLevel does not need to collect anything except the
//       current frontier. There is no result accumulator across levels.
//   [3] After ~20 min: findNearestOverride is ../problems/03's early exit
//       pointed the other way — the first match found is provably nearest,
//       so return the moment you see it.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ----------------------------------------------------------------------------
// Stands in for Postgres. fetchChildren models the real batched query
//   SELECT id FROM tenants WHERE parent_id = ANY(...)
// and counts how many times you issued it.
// ----------------------------------------------------------------------------
class TenantStore {
 public:
  explicit TenantStore(std::unordered_map<int, std::vector<int>> children)
      : children_(std::move(children)) {}

  std::vector<int> fetchChildren(const std::vector<int>& parent_ids) {
    ++query_count_;
    std::vector<int> out;
    for (int parent : parent_ids) {
      auto it = children_.find(parent);
      if (it != children_.end()) {
        out.insert(out.end(), it->second.begin(), it->second.end());
      }
    }
    std::sort(out.begin(), out.end());
    return out;
  }

  int queryCount() const { return query_count_; }

 private:
  std::unordered_map<int, std::vector<int>> children_;
  int query_count_ = 0;
};

// ----------------------------------------------------------------------------
// YOUR CODE HERE (part 1)
// ----------------------------------------------------------------------------
std::vector<int> getLevel(TenantStore& store, int root_id, int depth) {
  (void)store;    // remove these lines once you use the parameters
  (void)root_id;
  (void)depth;
  return {};
}

// ----------------------------------------------------------------------------
// YOUR CODE HERE (part 4)
// ----------------------------------------------------------------------------
int findNearestOverride(const std::unordered_map<int, int>& parent_of,
                        const std::unordered_set<int>& has_override,
                        int tenant_id) {
  (void)parent_of;  // remove these lines once you use the parameters
  (void)has_override;
  (void)tenant_id;
  return -1;
}

// ============================================================================
// Test harness below this line — you should not need to modify it.
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

  auto sorted = [](std::vector<int> v) {
    std::sort(v.begin(), v.end());
    return v;
  };

  // Fixture hierarchy:
  //           1
  //      +----+----+
  //      2    3    4
  //     / |   |    |
  //    5  6   7    8
  //       |
  //       9
  const std::unordered_map<int, std::vector<int>> kChildren = {
      {1, {2, 3, 4}}, {2, {5, 6}}, {3, {7}}, {4, {8}}, {6, {9}}};

  {
    TenantStore store(kChildren);
    check(sorted(getLevel(store, 1, 0)) == std::vector<int>({1}),
          "depth 0 -> just the root");
    check(store.queryCount() == 0, "depth 0 costs ZERO queries");
  }

  {
    TenantStore store(kChildren);
    check(sorted(getLevel(store, 1, 1)) == std::vector<int>({2, 3, 4}),
          "depth 1 -> the root's direct children");
  }

  {
    TenantStore store(kChildren);
    check(sorted(getLevel(store, 1, 2)) == std::vector<int>({5, 6, 7, 8}),
          "depth 2 -> all four grandchildren");
  }

  {
    TenantStore store(kChildren);
    check(sorted(getLevel(store, 1, 3)) == std::vector<int>({9}),
          "depth 3 -> the single deepest tenant");
  }

  {
    // THE QUERY BUDGET. Reaching depth 2 means exactly two batched queries:
    // one for the root's children, one for theirs. A per-node loop needs four
    // here (1 for node 1, then 1 each for nodes 2, 3, 4) and fails this.
    TenantStore store(kChildren);
    getLevel(store, 1, 2);
    check(store.queryCount() == 2,
          "depth 2 costs EXACTLY 2 queries (one per level, not one per node)");
  }

  {
    // Same budget rule one level deeper. A per-node solution needs 8 here.
    TenantStore store(kChildren);
    getLevel(store, 1, 3);
    check(store.queryCount() == 3, "depth 3 costs EXACTLY 3 queries");
  }

  {
    // Past the bottom of the hierarchy. Must return empty, and must not keep
    // querying once the frontier is empty — there is nothing left to ask about.
    TenantStore store(kChildren);
    check(getLevel(store, 1, 9).empty(), "depth beyond the leaves -> empty");
    check(store.queryCount() <= 4,
          "an exhausted frontier stops querying instead of looping to depth");
  }

  {
    // A subtree root, not the global root — the function takes a rootTenantId
    // for a reason.
    TenantStore store(kChildren);
    check(sorted(getLevel(store, 2, 1)) == std::vector<int>({5, 6}),
          "subtree root -> its own children, not the global root's");
  }

  {
    // A leaf as the root.
    TenantStore store(kChildren);
    check(getLevel(store, 9, 1).empty(), "leaf as root -> no children");
  }

  // --- part 4: nearest ancestor-or-self holding a config override -----------
  const std::unordered_map<int, int> kParentOf = {
      {2, 1}, {3, 1}, {4, 1}, {5, 2}, {6, 2}, {7, 3}, {8, 4}, {9, 6}};

  {
    const std::unordered_set<int> overrides = {1};
    check(findNearestOverride(kParentOf, overrides, 9) == 3,
          "override at the root -> 3 hops up from tenant 9");
  }

  {
    const std::unordered_set<int> overrides = {2};
    check(findNearestOverride(kParentOf, overrides, 9) == 2,
          "nearer override wins -> 2 hops, not 3");
  }

  {
    // Self counts as distance 0. Easy to miss by starting the walk at parent.
    const std::unordered_set<int> overrides = {9, 1};
    check(findNearestOverride(kParentOf, overrides, 9) == 0,
          "the tenant itself holds an override -> 0 hops");
  }

  {
    const std::unordered_set<int> overrides = {};
    check(findNearestOverride(kParentOf, overrides, 9) == -1,
          "no override anywhere on the chain -> -1");
  }

  {
    // Two overrides, one nearer. Confirms you stop at the FIRST match rather
    // than walking to the root and keeping the last one you saw.
    const std::unordered_set<int> overrides = {1, 6};
    check(findNearestOverride(kParentOf, overrides, 9) == 1,
          "stops at the first match found (1 hop), not the last");
  }

  {
    const std::unordered_set<int> overrides = {1};
    check(findNearestOverride(kParentOf, overrides, 1) == 0,
          "the root itself -> 0 hops");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
