// ============================================================================
// LeetCode 112 — Path Sum
// ============================================================================
//
// PROBLEM
// -------
// Given the root of a binary tree and an integer targetSum, return true if
// the tree has a ROOT-TO-LEAF path such that adding up all the values along
// the path equals targetSum. A leaf is a node with no children.
//
// Example: root = [5,4,8,11,null,13,4,7,2,null,null,null,1], targetSum = 22
//          -> true, via the path 5 -> 4 -> 11 -> 2 (5+4+11+2 = 22)
//
// APPROACH — Tree DFS, preorder, carry-state-down
// ------------------------------------------------
// This is the canonical preorder Tree DFS problem: we need to know something
// about the FULL path from root to a leaf, and the natural way to accumulate
// "the sum along this path" is to carry a running remainder DOWN the
// recursion as a parameter (see ../README.md, "Solution" and "Execution
// Flow" -- preorder framing).
//
// At each call:
//   - subtract the current node's value from the target we still need.
//   - if this node is a LEAF (both children null), the path ends here: it is
//     a valid answer if and only if the remaining target is now exactly 0.
//   - otherwise, recurse into whichever children exist, passing the reduced
//     target down. If EITHER subtree finds a valid path, the answer is true
//     (short-circuit via ||, so the right subtree is skipped entirely once
//     the left subtree already found a match).
//
// Why carrying a running remainder (rather than a running sum compared
// against targetSum at the end) is the same idea either way: both require
// exactly one pass down the tree, carrying one integer of state. Subtracting
// as we go just means the leaf check is "remaining == 0" instead of
// "sum == targetSum" -- purely a matter of taste.
//
// COMMON MISTAKE THIS FILE GUARDS AGAINST
// ----------------------------------------
// A node with exactly ONE child is NOT a leaf. Checking only "node->left ==
// nullptr" (or only right) as the stopping condition would incorrectly treat
// an internal node as a leaf and check a partial, incomplete path against
// targetSum. The leaf check below explicitly requires BOTH children null.
//
// COMPLEXITY
// ----------
// Time:  O(n) -- every node is visited at most once.
// Space: O(h) -- the recursion's call stack, where h is the tree's height
//                (O(log n) balanced, O(n) worst case on a degenerate tree).
// ============================================================================

#include <iostream>
#include <string>

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

bool hasPathSum(TreeNode* node, int targetSum) {
  if (!node) return false;  // Base case: no node, no path through here.

  int remaining = targetSum - node->val;

  bool isLeaf = (!node->left && !node->right);
  if (isLeaf) {
    return remaining == 0;
  }

  // Not a leaf: at least one child exists. Recurse into whichever children
  // are present; a missing child simply contributes "false" (there is no
  // path through it), which the recursive call itself already handles via
  // its own base case, so we can call it unconditionally.
  return hasPathSum(node->left, remaining) || hasPathSum(node->right, remaining);
}

// ----------------------------------------------------------------------------
// Test helpers: build and free the example tree from LeetCode's own prompt.
//
//   5 (root)
//     -- left  --> 4 -- left  --> 11 -- left  --> 7 (leaf)
//                                    -- right --> 2 (leaf)
//     -- right --> 8 -- left  --> 13 (leaf)
//                   -- right --> 4 -- right --> 1 (leaf)
// ----------------------------------------------------------------------------
TreeNode* buildExampleTree() {
  TreeNode* root = new TreeNode(5);
  root->left = new TreeNode(4);
  root->right = new TreeNode(8);

  root->left->left = new TreeNode(11);
  root->left->left->left = new TreeNode(7);
  root->left->left->right = new TreeNode(2);

  root->right->left = new TreeNode(13);
  root->right->right = new TreeNode(4);
  root->right->right->right = new TreeNode(1);

  return root;
}

void deleteTree(TreeNode* node) {
  if (!node) return;
  deleteTree(node->left);
  deleteTree(node->right);
  delete node;
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

  TreeNode* root = buildExampleTree();

  check(hasPathSum(root, 22) == true, "example tree, target 22 -> true (5+4+11+2)");
  check(hasPathSum(root, 26) == true, "example tree, target 26 -> true (5+8+13)");
  check(hasPathSum(root, 27) == true, "example tree, target 27 -> true (5+4+11+7)");
  check(hasPathSum(root, 100) == false, "example tree, target 100 -> false, no matching path");

  deleteTree(root);

  {
    TreeNode* single = new TreeNode(1);
    check(hasPathSum(single, 1) == true, "single node equal to target -> true");
    check(hasPathSum(single, 2) == false, "single node not equal to target -> false");
    deleteTree(single);
  }

  {
    // Internal node with only one child: the leaf check must not treat the
    // internal node itself as a leaf.
    TreeNode* chain = new TreeNode(1);
    chain->left = new TreeNode(2);
    check(hasPathSum(chain, 1) == false, "root alone sums to target, but root is not a leaf -> false");
    check(hasPathSum(chain, 3) == true, "1 -> 2 leaf sums to target -> true");
    deleteTree(chain);
  }

  check(hasPathSum(nullptr, 0) == false, "empty tree -> false regardless of target");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
