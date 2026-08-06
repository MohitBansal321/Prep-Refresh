// ============================================================================
// LeetCode 111 — Minimum Depth of Binary Tree
// ============================================================================
//
// PROBLEM
// -------
// Given a binary tree, find its minimum depth: the number of nodes along the
// shortest path from the root node down to the nearest LEAF node. A leaf is
// a node with no children. A single-node tree has minimum depth 1.
//
// Example:
//   Input:  [3, 9, 20, null, null, 15, 7]
//   Output: 2   (root -> 9 is the shortest root-to-leaf path)
//
// APPROACH — Tree BFS with early exit
// -------------------------------------
// This is the single best illustration in this module of WHY Tree BFS beats
// Tree DFS on a "minimum" question. BFS visits nodes in strictly
// non-decreasing depth order (see ../README.md), so the FIRST leaf it
// dequeues is PROVABLY at the minimum depth — there is no need to look at
// the rest of the tree once that leaf is found. A DFS solution would have
// to explore every root-to-leaf path and keep a running minimum, because
// nothing about DFS's visiting order favors shallow leaves over deep ones.
//
// The traversal skeleton is the familiar queue + level-size snapshot. The
// only per-node logic is: check whether this node is a leaf (BOTH children
// null, not just one — see the edge case below), and if so return the
// current depth immediately.
//
// EDGE CASE — a node with exactly one child is NOT a leaf
// ----------------------------------------------------------
// LeetCode is explicit about this, and it is the most common bug in a naive
// solution: `node->left == nullptr || node->right == nullptr` looks like a
// reasonable leaf check but is WRONG — it fires on a node that has only one
// child, undercounting the true minimum depth. The correct check requires
// BOTH children to be absent (`&&`, not `||`). See the one-sided-chain test
// in main() below, which specifically exercises this.
//
// COMPLEXITY
// ----------
// Time:  O(n) worst case (a tree with no early-exit opportunity, e.g. the
//        only leaf is the very last node BFS would visit); O(1)-relative-to-n
//        best case (a leaf directly under the root).
// Space: O(n) worst case for the queue (a wide tree's widest level).
// ============================================================================

#include <iostream>
#include <queue>
#include <string>

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void freeTree(TreeNode* root) {
  if (root == nullptr) {
    return;
  }
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

int minDepth(TreeNode* root) {
  if (root == nullptr) {
    return 0;  // An empty tree has depth 0 by convention.
  }

  std::queue<TreeNode*> q;
  q.push(root);
  int depth = 1;  // A single node counts as depth 1, per the problem statement.

  while (!q.empty()) {
    size_t level_size = q.size();  // Level-size snapshot.

    for (size_t i = 0; i < level_size; ++i) {
      TreeNode* node = q.front();
      q.pop();

      // A leaf has ZERO children -- not "is missing at least one child."
      if (node->left == nullptr && node->right == nullptr) {
        return depth;  // First leaf found: guaranteed to be the minimum.
      }

      if (node->left != nullptr) {
        q.push(node->left);
      }
      if (node->right != nullptr) {
        q.push(node->right);
      }
    }

    ++depth;
  }

  return depth;  // Unreachable when root != nullptr; kept for a well-defined
                 // return on every code path.
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
    // [3, 9, 20, null, null, 15, 7] -> min depth 2 (root -> 9)
    TreeNode* root = new TreeNode(3);
    root->left = new TreeNode(9);
    root->right = new TreeNode(20);
    root->right->left = new TreeNode(15);
    root->right->right = new TreeNode(7);

    check(minDepth(root) == 2, "classic example -> min depth 2");

    freeTree(root);
  }

  {
    check(minDepth(nullptr) == 0, "empty tree -> min depth 0");
  }

  {
    TreeNode* root = new TreeNode(1);
    check(minDepth(root) == 1, "single node -> min depth 1");
    freeTree(root);
  }

  {
    // The classic trap: 2 -> right(3) -> right(4) -> right(5) -> right(6).
    // Node 2 has ONLY a right child, so it is NOT a leaf, even though its
    // left child is null. The only true leaf is node 6, at depth 5.
    // A buggy `||`-based leaf check would wrongly return 1 here (mistaking
    // node 2, which is missing its left child, for a leaf).
    TreeNode* root = new TreeNode(2);
    root->right = new TreeNode(3);
    root->right->right = new TreeNode(4);
    root->right->right->right = new TreeNode(5);
    root->right->right->right->right = new TreeNode(6);

    check(minDepth(root) == 5,
          "one-sided chain (LeetCode's own trap case) -> min depth 5, not 1");

    freeTree(root);
  }

  {
    // Perfect tree, depth 3: every leaf is at the same depth, so the
    // early-exit still correctly reports the true (and only) minimum.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    root->right->left = new TreeNode(6);
    root->right->right = new TreeNode(7);

    check(minDepth(root) == 3, "perfect tree depth 3 -> min depth 3 (all leaves equal)");

    freeTree(root);
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
