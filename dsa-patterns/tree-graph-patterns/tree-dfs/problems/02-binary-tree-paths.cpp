// ============================================================================
// LeetCode 257 — Binary Tree Paths
// ============================================================================
//
// PROBLEM
// -------
// Given the root of a binary tree, return all root-to-leaf paths in any
// order, each formatted as a string like "1->2->5".
//
// Example: root = [1,2,3,null,5]  ->  ["1->2->5", "1->3"]
//
// APPROACH — Tree DFS, preorder, carry-state-down
// ------------------------------------------------
// This is the purest "collect every root-to-leaf path" problem in the
// module. Each recursive call is handed the path string built so far and
// extends it with the current node's value before deciding whether to
// recurse further or record a completed path (see ../README.md, "Solution"
// and "Execution Flow" -- preorder framing).
//
// The path string is passed BY VALUE here, so every recursive call
// automatically works with its own independent copy -- appending to it in
// one branch can never affect a sibling branch. That is a deliberate choice:
// it avoids the explicit "un-append on the way back up" discipline that a
// SHARED mutable path (a std::vector<int> reused across calls, as in
// 03-path-sum-ii.cpp) would require. The cost is that each recursive call
// allocates its own string copy -- a reasonable trade for this problem's
// small typical path lengths, and worth contrasting explicitly with 03's
// approach.
//
// LEAF CHECK
// ----------
// A leaf is a node with BOTH children null. Checking only one side, or
// checking "not null" instead of "leaf", is the most common bug in this
// family of problems -- it either stops one node too early or lets a
// non-leaf's still-partial path get recorded as if it were complete.
//
// COMPLEXITY
// ----------
// Time:  O(n * h) worst case, where n is the node count and h is tree
//        height -- every node is visited once (O(n) calls), but building and
//        copying the path STRING at each level costs up to O(h) per level in
//        the worst case (a string of length proportional to depth is copied
//        on every recursive call). This is the same n * h accounting the
//        README's Disadvantages section calls out for careless "copy the
//        whole path at every node" implementations.
// Space: O(h) for the recursion's call stack, plus O(n * h) to store the
//        output itself (n leaf paths, each up to length h) -- the O(n * h)
//        output size is inherent to the problem (you are asked to return
//        every path), not a cost of the traversal mechanism.
// ============================================================================

#include <iostream>
#include <string>
#include <vector>

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void collectPaths(TreeNode* node, std::string pathSoFar, std::vector<std::string>& results) {
  if (!node) return;  // Base case: nothing here, nothing to add.

  pathSoFar += std::to_string(node->val);

  bool isLeaf = (!node->left && !node->right);
  if (isLeaf) {
    results.push_back(pathSoFar);
    return;
  }

  if (node->left) collectPaths(node->left, pathSoFar + "->", results);
  if (node->right) collectPaths(node->right, pathSoFar + "->", results);
}

std::vector<std::string> binaryTreePaths(TreeNode* root) {
  std::vector<std::string> results;
  collectPaths(root, "", results);
  return results;
}

// ----------------------------------------------------------------------------
// Test helpers.
// ----------------------------------------------------------------------------
TreeNode* buildExampleTree() {
  // 1 -- left  --> 2 -- right --> 5 (leaf)
  //   -- right --> 3 (leaf)
  TreeNode* root = new TreeNode(1);
  root->left = new TreeNode(2);
  root->right = new TreeNode(3);
  root->left->right = new TreeNode(5);
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

  {
    TreeNode* root = buildExampleTree();
    std::vector<std::string> paths = binaryTreePaths(root);
    std::vector<std::string> expected = {"1->2->5", "1->3"};
    check(paths == expected, "example tree -> [\"1->2->5\", \"1->3\"]");
    deleteTree(root);
  }

  {
    TreeNode* single = new TreeNode(42);
    std::vector<std::string> paths = binaryTreePaths(single);
    std::vector<std::string> expected = {"42"};
    check(paths == expected, "single node -> [\"42\"] (root is its own leaf)");
    deleteTree(single);
  }

  {
    // Left-skewed chain: only one root-to-leaf path exists.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->left->left = new TreeNode(3);
    std::vector<std::string> paths = binaryTreePaths(root);
    std::vector<std::string> expected = {"1->2->3"};
    check(paths == expected, "left-skewed chain -> single path \"1->2->3\"");
    deleteTree(root);
  }

  {
    std::vector<std::string> paths = binaryTreePaths(nullptr);
    check(paths.empty(), "empty tree -> no paths");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
