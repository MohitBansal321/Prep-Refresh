// ============================================================================
// EXERCISE 05 (Bonus) — LeetCode 863: All Nodes Distance K in Binary Tree
// ============================================================================
//
// This file is a SELF-TEST, not a solution. The assertions in main() are the
// specification; fill in the stub until they all print [PASS].
//
//   g++ -std=c++17 -Wall exercises/05-all-nodes-distance-k.cpp -o /tmp/ex05 && /tmp/ex05
//
// It compiles as-is, so you get a failing baseline immediately. No solution is
// provided anywhere in this repo — see ../exercises.md for the task write-up.
//
// TASK
// ----
// Return the values of every node exactly `k` edges from `target`. Distance is
// counted in ANY direction — up toward the root as well as down. Return order
// does not matter; the harness sorts before comparing.
//
// This exercise exists to show you where Tree BFS ENDS. ../exercises.md then
// asks you to argue, in writing, whether this problem belongs in this module or
// in ../../graph-bfs-dfs/. Answer that after the tests are green — it is the
// actual point of the exercise, and no harness can grade it.
//
// HINTS — read one at a time, only when genuinely stuck.
//   [1] After ~10 min: left/right only go DOWN. Hops may go up. Do a
//       preparation pass first and build something that lets you go up.
//   [2] After ~20 min: once every node has three neighbours (left, right,
//       parent), the standard queue-and-snapshot loop works unchanged — with
//       `k` as a level counter. Return the whole level when the counter hits k.
//   [3] After ~30 min: test 6 hangs or returns garbage without one specific
//       addition. You walked from a node to its parent and straight back down
//       to the same node. The README says Tree BFS never needs `visited`; the
//       README is right, and you are no longer traversing a tree.
// ============================================================================

#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// ----------------------------------------------------------------------------
// YOUR CODE HERE
// ----------------------------------------------------------------------------
std::vector<int> distanceK(TreeNode* root, TreeNode* target, int k) {
  (void)root;    // remove these lines once you use the parameters
  (void)target;
  (void)k;
  return {};
}

// ============================================================================
// Test harness below this line — you should not need to modify it.
// ============================================================================

static void freeTree(TreeNode* node) {
  if (node == nullptr) return;
  freeTree(node->left);
  freeTree(node->right);
  delete node;
}

// Finds a node by value so the tests can name a target readably. Assumes the
// fixture trees below have distinct values, which they do.
static TreeNode* findNode(TreeNode* root, int val) {
  if (root == nullptr) return nullptr;
  if (root->val == val) return root;
  TreeNode* left = findNode(root->left, val);
  return left != nullptr ? left : findNode(root->right, val);
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

  auto sorted = [](std::vector<int> v) {
    std::sort(v.begin(), v.end());
    return v;
  };

  // The LeetCode 863 example tree:
  //             3
  //        +----+----+
  //        5         1
  //      +-+-+     +-+-+
  //      6   2     0   8
  //         +-+
  //         7 4
  auto buildExample = []() {
    TreeNode* root = new TreeNode(3);
    root->left = new TreeNode(5);
    root->right = new TreeNode(1);
    root->left->left = new TreeNode(6);
    root->left->right = new TreeNode(2);
    root->left->right->left = new TreeNode(7);
    root->left->right->right = new TreeNode(4);
    root->right->left = new TreeNode(0);
    root->right->right = new TreeNode(8);
    return root;
  };

  {
    // THE HEADLINE CASE. From node 5 at distance 2: node 7 and 4 are two hops
    // DOWN, node 1 is two hops UP-then-down (5 -> 3 -> 1). A solution that only
    // walks downward returns [7, 4] and misses 1 entirely.
    TreeNode* root = buildExample();
    check(sorted(distanceK(root, findNode(root, 5), 2)) ==
              std::vector<int>({1, 4, 7}),
          "target 5, k=2 -> [1, 4, 7] (node 1 is reached by going UP)");
    freeTree(root);
  }

  {
    // k = 0 is the target itself, not its neighbours.
    TreeNode* root = buildExample();
    check(sorted(distanceK(root, findNode(root, 5), 0)) ==
              std::vector<int>({5}),
          "k=0 -> the target itself");
    freeTree(root);
  }

  {
    TreeNode* root = buildExample();
    check(sorted(distanceK(root, findNode(root, 5), 1)) ==
              std::vector<int>({2, 3, 6}),
          "target 5, k=1 -> both children and the parent");
    freeTree(root);
  }

  {
    // From a leaf, every hop must travel upward first: 7 -> 2 -> 5 gets you to
    // distance 2, and distance 3 is then 5's other child (6) and 5's parent (3).
    TreeNode* root = buildExample();
    check(sorted(distanceK(root, findNode(root, 7), 3)) ==
              std::vector<int>({3, 6}),
          "target 7 (a leaf), k=3 -> [3, 6], reached only by going up first");
    freeTree(root);
  }

  {
    // Distance larger than the tree's reach -> nothing, not a crash.
    TreeNode* root = buildExample();
    check(distanceK(root, findNode(root, 7), 9).empty(),
          "k beyond the tree's diameter -> empty");
    freeTree(root);
  }

  {
    // THE OSCILLATION TRAP. Target is the root of a long left chain, so every
    // path out of a node leads back into a node you have already seen. Without
    // a visited set this loops forever (or inflates every distance).
    //   1 - 2 - 3 - 4 - 5   (each the LEFT child of the previous)
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->left->left = new TreeNode(3);
    root->left->left->left = new TreeNode(4);
    root->left->left->left->left = new TreeNode(5);
    check(sorted(distanceK(root, findNode(root, 3), 2)) ==
              std::vector<int>({1, 5}),
          "left chain, target in the middle, k=2 -> [1, 5] (no oscillation)");
    freeTree(root);
  }

  {
    // The target is the root: only downward hops exist. A solution that
    // unconditionally dereferences a parent pointer segfaults here.
    TreeNode* root = buildExample();
    check(sorted(distanceK(root, findNode(root, 3), 1)) ==
              std::vector<int>({1, 5}),
          "target is the root, k=1 -> the root has no parent to visit");
    freeTree(root);
  }

  {
    // Single-node tree: k=0 finds it, k=1 finds nothing.
    TreeNode* root = new TreeNode(42);
    check(sorted(distanceK(root, root, 0)) == std::vector<int>({42}),
          "single node, k=0 -> itself");
    check(distanceK(root, root, 1).empty(), "single node, k=1 -> empty");
    freeTree(root);
  }

  {
    // A wide result set: several nodes at the same distance in different
    // directions. Catches an early `return` that stops at the first match.
    TreeNode* root = buildExample();
    check(sorted(distanceK(root, findNode(root, 3), 2)) ==
              std::vector<int>({0, 2, 6, 8}),
          "target 3, k=2 -> all four grandchildren, both subtrees");
    freeTree(root);
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
