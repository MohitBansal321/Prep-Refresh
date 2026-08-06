// ============================================================================
// Tree DFS — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// two families of recursive traversal you will re-derive on almost every
// problem that fits this pattern:
//
//   1. PREORDER, carry-state-down — process the current node first (fold it
//      into a running path/sum), THEN recurse into left and right, passing
//      the updated state down as a parameter. Used for: root-to-leaf path
//      collection, path-sum checks.
//
//   2. POSTORDER, combine-on-the-way-up — recurse into left and right FIRST,
//      and only after both calls return, combine their results with the
//      current node's value. Used for: max depth/height, balance checks,
//      diameter, max path sum.
//
// Both variants are expressed as small, generic, reusable functions so you
// can see the *shape* of the pattern independent of any one problem. The
// worked, problem-specific solutions live in problems/*.cpp.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_tdfs_code && /tmp/out_tdfs_code
// ============================================================================

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// The standard binary tree node used by every file in this module.
// ----------------------------------------------------------------------------
struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// ----------------------------------------------------------------------------
// Builds a small, fixed, hand-checkable tree so main() has something concrete
// to run every function against without repeating construction boilerplate.
//
//   5 (root)
//     -- left  --> 4
//                     -- left --> 11 -- left --> 7 (leaf)
//                                    -- right --> 2 (leaf)
//     -- right --> 8
//                     -- left  --> 13 (leaf)
//                     -- right --> 4 -- right --> 1 (leaf)
//
// (This is the same shape used in images/trace-diagram.md.)
// ----------------------------------------------------------------------------
TreeNode* buildSampleTree() {
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

// Postorder deletion: children must be freed before the node itself, since
// once `node` is deleted, `node->left`/`node->right` can no longer be read.
void deleteTree(TreeNode* node) {
  if (!node) return;
  deleteTree(node->left);
  deleteTree(node->right);
  delete node;
}

// ----------------------------------------------------------------------------
// PREORDER, carry-state-down: collect every root-to-leaf path.
//
// Each recursive call receives the path built so far AS A STRING PASSED BY
// VALUE, so every call automatically gets its own independent copy. That is
// why there is no explicit "un-append" step here (contrast with
// problems/03-path-sum-ii.cpp, which deliberately uses a SHARED mutable
// vector and must un-append on the way back up).
// ----------------------------------------------------------------------------
void collectPaths(TreeNode* node, std::string pathSoFar, std::vector<std::string>& results) {
  if (!node) return;  // Base case: nothing here, nothing to add.

  pathSoFar += std::to_string(node->val);

  bool isLeaf = (!node->left && !node->right);
  if (isLeaf) {
    // A leaf is a node with BOTH children null -- not just "a null child".
    results.push_back(pathSoFar);
    return;
  }

  // Not a leaf: extend the path with the connector and recurse into
  // whichever children actually exist.
  if (node->left) collectPaths(node->left, pathSoFar + "->", results);
  if (node->right) collectPaths(node->right, pathSoFar + "->", results);
}

std::vector<std::string> binaryTreePaths(TreeNode* root) {
  std::vector<std::string> results;
  collectPaths(root, "", results);
  return results;
}

// ----------------------------------------------------------------------------
// PREORDER, carry-state-down: does any root-to-leaf path sum to targetSum?
//
// The running sum is carried down as a parameter, exactly like the path
// string above -- the only difference is the payload being accumulated.
// ----------------------------------------------------------------------------
bool hasPathSum(TreeNode* node, int targetSum) {
  if (!node) return false;  // Base case: no node, no path through here.

  int remaining = targetSum - node->val;

  bool isLeaf = (!node->left && !node->right);
  if (isLeaf) {
    return remaining == 0;
  }

  // Short-circuit: if the left subtree already finds a match, the right
  // subtree is never even explored.
  return hasPathSum(node->left, remaining) || hasPathSum(node->right, remaining);
}

// ----------------------------------------------------------------------------
// POSTORDER, combine-on-the-way-up: the simplest possible aggregation.
//
// A node cannot know its own height until BOTH children have fully reported
// theirs -- that is what makes this postorder rather than preorder.
// ----------------------------------------------------------------------------
int maxDepth(TreeNode* node) {
  if (!node) return 0;  // Base case: an empty subtree has height 0.

  int leftDepth = maxDepth(node->left);
  int rightDepth = maxDepth(node->right);

  // Only after BOTH recursive calls have returned can this node compute its
  // own answer.
  return 1 + std::max(leftDepth, rightDepth);
}

// ============================================================================
// main() -- demonstrates all three functions with printed, verifiable output.
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

  TreeNode* root = buildSampleTree();

  std::cout << "--- binaryTreePaths (preorder, path-building) ---\n";
  {
    std::vector<std::string> paths = binaryTreePaths(root);
    std::vector<std::string> expected = {
        "5->4->11->7", "5->4->11->2", "5->8->13", "5->8->4->1"};
    check(paths == expected, "sample tree -> 4 root-to-leaf paths, in left-to-right order");
    for (const auto& p : paths) {
      std::cout << "    " << p << "\n";
    }
  }

  std::cout << "\n--- hasPathSum (preorder, running sum) ---\n";
  {
    check(hasPathSum(root, 22) == true, "5->4->11->2 sums to 22 -> true");
    check(hasPathSum(root, 26) == true, "5->8->13 sums to 26 -> true");
    check(hasPathSum(root, 100) == false, "no root-to-leaf path sums to 100 -> false");

    TreeNode* empty = nullptr;
    check(hasPathSum(empty, 0) == false, "empty tree -> false, regardless of target");
  }

  std::cout << "\n--- maxDepth (postorder, aggregation) ---\n";
  {
    check(maxDepth(root) == 4, "sample tree -> max depth 4 (5-4-11-7 or 5-4-11-2)");

    TreeNode* single = new TreeNode(1);
    check(maxDepth(single) == 1, "single node -> depth 1");
    deleteTree(single);

    check(maxDepth(nullptr) == 0, "empty tree -> depth 0");
  }

  deleteTree(root);

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
