// ============================================================================
// LeetCode 124 — Binary Tree Maximum Path Sum (Hard)
// ============================================================================
//
// PROBLEM
// -------
// A "path" here is any sequence of nodes connected by edges, where each node
// appears at most once, and the path does NOT need to pass through the root
// or end at a leaf -- it can start and end anywhere, and it is allowed to
// "bend" through a single node using both of that node's children. Given the
// root of a binary tree, return the maximum path sum of any non-empty path.
//
// Example: root = [-10,9,20,null,null,15,7]
//          -> 42, via the path 15 -> 20 -> 7 (the path bends through 20)
//
// APPROACH — Tree DFS, POSTORDER aggregation
// --------------------------------------------
// This is the hardest postorder aggregation in the module because of one
// specific wrinkle: the value a node RETURNS to its parent is NOT the same
// as the value that competes to be the overall answer. See ../README.md's
// "Solution", "Execution Flow", and "Common Mistakes" sections for the full
// argument; the short version:
//
//   - A real path cannot branch. So whatever a PARENT is allowed to extend
//     upward through this node is at most: this node's value, plus the
//     BETTER of its two children's downward contributions (not both).
//   - But AT THIS NODE, right now, we are allowed to consider the path that
//     bends through it using BOTH children at once -- that is a valid path,
//     it just cannot be extended any further upward once it has bent. So the
//     overall best answer is tracked SEPARATELY (bestPathSum, captured by
//     reference), updated at every node with "node->val + left + right",
//     while the function's RETURN VALUE only ever offers one side to the
//     caller.
//
// A second wrinkle: subtree contributions are clamped to zero before use.
// If a child's best downward path sum is negative, including it in this
// node's path could only make things worse than not including it at all --
// so a negative contribution is treated as "don't take this side."
//
// EXECUTION FLOW (mirrors ../README.md's postorder section exactly)
// -------------------------------------------------------------------
//   1. If node is null, return 0 (an absent branch contributes nothing).
//   2. Recurse: leftGain = bestDownward(node->left);
//                rightGain = bestDownward(node->right).
//      Both calls must fully complete before step 3 -- this is what makes it
//      postorder.
//   3. Clamp negative contributions to zero:
//      leftGain = max(leftGain, 0); rightGain = max(rightGain, 0).
//   4. Update the running best: bestPathSum = max(bestPathSum,
//      node->val + leftGain + rightGain) -- the one place the path is
//      allowed to bend through both children.
//   5. Return node->val + max(leftGain, rightGain) -- the value the PARENT
//      is allowed to use, extending through at most one child.
//
// COMPLEXITY
// ----------
// Time:  O(n) -- every node is visited exactly once, doing O(1) work beyond
//        its two recursive calls.
// Space: O(h) for the recursion's call stack, where h is tree height.
// ============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string>

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// Returns the best sum achievable on a path that starts at `node` and
// extends downward into AT MOST ONE child -- the value a PARENT is allowed
// to use. Along the way, updates `bestPathSum` with the best sum achievable
// on any path that bends through `node` using BOTH children -- the value
// that competes to be the PROBLEM's actual answer.
int bestDownward(TreeNode* node, long long& bestPathSum) {
  if (!node) return 0;  // Base case: an absent branch contributes nothing.

  int leftGain = bestDownward(node->left, bestPathSum);
  int rightGain = bestDownward(node->right, bestPathSum);

  // A negative contribution can only hurt -- treat it as "skip this side."
  leftGain = std::max(leftGain, 0);
  rightGain = std::max(rightGain, 0);

  // This is the ONE place the path is allowed to bend through `node` using
  // both children at once. That candidate is valid as a complete path, but
  // it cannot be extended further upward, so it is only ever compared
  // against the running best -- never returned to the caller.
  long long throughThisNode = static_cast<long long>(node->val) + leftGain + rightGain;
  bestPathSum = std::max(bestPathSum, throughThisNode);

  // What the PARENT is allowed to use: this node's value plus the BETTER of
  // its two children's downward contributions -- never both, since a real
  // path cannot branch once it continues upward through this node.
  return node->val + std::max(leftGain, rightGain);
}

int maxPathSum(TreeNode* root) {
  long long bestPathSum = LLONG_MIN;
  bestDownward(root, bestPathSum);
  return static_cast<int>(bestPathSum);
}

// ----------------------------------------------------------------------------
// Test helpers.
// ----------------------------------------------------------------------------
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

  {
    // 1 -- left --> 2, -- right --> 3.  Best path: 2 -> 1 -> 3 = 6.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    check(maxPathSum(root) == 6, "[1,2,3] -> 6 (path 2->1->3, bends through root)");
    deleteTree(root);
  }

  {
    // -10 -- left --> 9, -- right --> 20 -- left --> 15, -- right --> 7.
    // Best path: 15 -> 20 -> 7 = 42 (does not use the root at all).
    TreeNode* root = new TreeNode(-10);
    root->left = new TreeNode(9);
    root->right = new TreeNode(20);
    root->right->left = new TreeNode(15);
    root->right->right = new TreeNode(7);
    check(maxPathSum(root) == 42, "[-10,9,20,null,null,15,7] -> 42 (path 15->20->7)");
    deleteTree(root);
  }

  {
    // Single negative node: the only possible path is the node itself.
    TreeNode* single = new TreeNode(-3);
    check(maxPathSum(single) == -3, "single negative node -> -3 (a path must be non-empty)");
    deleteTree(single);
  }

  {
    // All-negative tree: best path is still the single LEAST negative node,
    // since including any additional (negative) node can only make it worse.
    TreeNode* root = new TreeNode(-2);
    root->left = new TreeNode(-1);
    check(maxPathSum(root) == -1, "[-2,-1] -> -1 (best single node beats any combination)");
    deleteTree(root);
  }

  {
    // A chain where every value is positive: the best path is the whole
    // chain, confirming the "extend through at most one child" return value
    // correctly composes across more than two levels.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->left->left = new TreeNode(3);
    check(maxPathSum(root) == 6, "[1,2,3] left-skewed chain -> 6 (whole chain 3->2->1)");
    deleteTree(root);
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
