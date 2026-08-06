// ============================================================================
// Union Find / Disjoint Set — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// two core operations you will re-derive on almost every problem that fits
// this pattern:
//
//   1. find(x)        — walk up the `parent` chain from x to its group's
//                        representative ("root"), flattening the chain along
//                        the way (PATH COMPRESSION) so future calls are fast.
//   2. unionSets(x, y) — merge the two groups containing x and y by attaching
//                        the smaller/shallower tree under the larger/deeper
//                        one (UNION BY RANK, with size tracked too), rather
//                        than attaching arbitrarily and letting chains grow
//                        long.
//
// Together, path compression and union by rank give amortized O(alpha(n))
// time per operation — alpha being the inverse Ackermann function, which is
// less than 5 for any n you could ever construct in practice. That is why
// this is described as "effectively O(1)" rather than "exactly O(1)": it is
// not a fixed constant in the formal sense, but it never meaningfully grows.
//
// The worked, problem-specific solutions live in problems/*.cpp. This file
// exists to show the *shape* of the pattern, independent of any one problem.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// DisjointSet — a generic Union-Find structure over the integers [0, n).
//
// If your elements are not already small dense integers (e.g. strings,
// emails, arbitrary IDs), map each distinct element to an index first with a
// hash map, then use that index with this class — see problems/03 and 04 for
// worked examples of exactly that mapping step.
// ----------------------------------------------------------------------------
class DisjointSet {
 public:
  // Creates n singleton groups: {0}, {1}, ..., {n-1}. Every element starts
  // as its own root, which is why the initializing loop sets parent[i] = i
  // (an element that IS the root of its own group, by definition, points to
  // itself) rather than to some sentinel like -1.
  explicit DisjointSet(int n)
      : parent_(n), rank_(n, 0), size_(n, 1), component_count_(n) {
    if (n < 0) {
      throw std::invalid_argument("DisjointSet size must be non-negative.");
    }
    std::iota(parent_.begin(), parent_.end(), 0);  // parent_[i] = i
  }

  // find(x): returns the representative ("root") of the group containing x.
  // Two elements are in the same group if and only if find(a) == find(b) —
  // never compare parent_[a] == parent_[b] directly, since neither a nor b
  // may currently point straight at the root (see Common Mistakes in the
  // README for why that shortcut is wrong).
  //
  // PATH COMPRESSION: while unwinding the recursion, every node visited on
  // the way to the root is re-pointed DIRECTLY at the root. The next find()
  // on any of those nodes is then O(1) instead of re-walking the same chain.
  // This is what turns a naive O(n) worst-case chain into a near-flat tree
  // over repeated calls.
  int find(int x) {
    check_bounds(x);
    if (parent_[x] != x) {
      parent_[x] = find(parent_[x]);  // recurse to the root, then flatten
    }
    return parent_[x];
  }

  // unionSets(x, y): merges the groups containing x and y.
  // Returns false if x and y were ALREADY in the same group (i.e. this
  // "edge" would create a cycle in a graph built one edge at a time) —
  // this return value is exactly how cycle detection falls out for free.
  // Returns true if a merge actually happened.
  //
  // UNION BY RANK (+ SIZE): `rank_` is an upper bound on a tree's height.
  // We always attach the root with the SMALLER rank underneath the root
  // with the LARGER rank. This keeps the resulting tree's height growing
  // only logarithmically instead of linearly — attaching arbitrarily (e.g.
  // always making x's root the new parent) can degrade to an O(n)-tall
  // chain if unions happen to arrive in the wrong order. `size_` is tracked
  // alongside rank purely so callers can cheaply ask "how big is this
  // component now?" via componentSize().
  bool unionSets(int x, int y) {
    int root_x = find(x);
    int root_y = find(y);

    if (root_x == root_y) {
      return false;  // Already connected: union would be a no-op / a cycle.
    }

    // Attach the shallower tree under the deeper one.
    if (rank_[root_x] < rank_[root_y]) {
      std::swap(root_x, root_y);
    }
    parent_[root_y] = root_x;
    size_[root_x] += size_[root_y];
    if (rank_[root_x] == rank_[root_y]) {
      // Equal ranks: the resulting tree's height grew by exactly one level,
      // so the surviving root's rank must increase to keep reflecting a
      // valid upper bound on height. When ranks differ, attaching the
      // shorter tree under the taller one never increases the taller
      // tree's height, so its rank does not need to change.
      ++rank_[root_x];
    }
    --component_count_;
    return true;
  }

  // connected(x, y): true iff x and y are currently in the same group.
  bool connected(int x, int y) { return find(x) == find(y); }

  // componentSize(x): how many elements currently share x's group.
  int componentSize(int x) { return size_[find(x)]; }

  // componentCount(): how many disjoint groups exist right now. Starts at n
  // and decreases by exactly one on every successful (non-cycle) union —
  // this is the standard trick behind "count connected components" and
  // "count provinces" style problems.
  int componentCount() const { return component_count_; }

 private:
  void check_bounds(int x) const {
    if (x < 0 || x >= static_cast<int>(parent_.size())) {
      throw std::out_of_range("DisjointSet: index out of range.");
    }
  }

  std::vector<int> parent_;  // parent_[i] = parent of i (i itself, if root)
  std::vector<int> rank_;    // rank_[i] = upper bound on height of tree rooted at i
  std::vector<int> size_;    // size_[i] = number of elements in i's tree (i must be a root for this to be current)
  int component_count_;      // number of disjoint groups remaining
};

// ============================================================================
// main() — demonstrates DisjointSet with printed, verifiable output.
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

  std::cout << "--- Basic union/find/connected ---\n";
  {
    DisjointSet dsu(10);  // 10 singleton groups: {0}, {1}, ..., {9}
    check(dsu.componentCount() == 10, "starts with 10 components");
    check(!dsu.connected(0, 1), "0 and 1 not connected before any union");

    dsu.unionSets(0, 1);
    check(dsu.connected(0, 1), "0 and 1 connected after union");
    check(dsu.componentCount() == 9, "component count drops to 9 after one union");

    dsu.unionSets(1, 2);
    check(dsu.connected(0, 2), "0 and 2 connected transitively via 1");
    check(dsu.componentCount() == 8, "component count drops to 8");
  }

  std::cout << "\n--- Union returns false on redundant edge (cycle signal) ---\n";
  {
    DisjointSet dsu(5);
    check(dsu.unionSets(0, 1) == true, "first union of 0-1 succeeds");
    check(dsu.unionSets(1, 2) == true, "first union of 1-2 succeeds");
    // 0 and 2 are already connected via 1, so this edge closes a cycle.
    check(dsu.unionSets(0, 2) == false,
          "unioning already-connected 0 and 2 returns false (cycle detected)");
    check(dsu.componentCount() == 3,
          "component count unaffected by the redundant (cycle) edge");
  }

  std::cout << "\n--- componentSize tracks merged group sizes ---\n";
  {
    DisjointSet dsu(6);
    dsu.unionSets(0, 1);
    dsu.unionSets(1, 2);
    dsu.unionSets(3, 4);
    check(dsu.componentSize(0) == 3, "{0,1,2} has size 3");
    check(dsu.componentSize(3) == 2, "{3,4} has size 2");
    check(dsu.componentSize(5) == 1, "{5} alone has size 1");

    dsu.unionSets(2, 3);  // merges {0,1,2} with {3,4}
    check(dsu.componentSize(0) == 5, "merging groups combines sizes -> 5");
    check(dsu.componentCount() == 2, "two components remain: {0,1,2,3,4} and {5}");
  }

  std::cout << "\n--- Path compression keeps find() flat after repeated calls ---\n";
  {
    // Deliberately chain unions in a way that WOULD build a tall chain
    // without union by rank (0 under 1 under 2 under ... under 9).
    DisjointSet dsu(10);
    for (int i = 0; i < 9; ++i) {
      dsu.unionSets(i, i + 1);
    }
    check(dsu.componentCount() == 1, "chain of unions merges everything into one component");
    int root = dsu.find(0);
    bool all_same_root = true;
    for (int i = 1; i < 10; ++i) {
      if (dsu.find(i) != root) all_same_root = false;
    }
    check(all_same_root, "every element resolves to the same root after the chain");
  }

  std::cout << "\n--- Out-of-range access throws instead of corrupting state ---\n";
  {
    DisjointSet dsu(3);
    bool threw = false;
    try {
      dsu.find(5);
    } catch (const std::out_of_range&) {
      threw = true;
    }
    check(threw, "find() on an out-of-range index throws std::out_of_range");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
