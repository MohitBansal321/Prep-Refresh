// ============================================================================
// LeetCode 102 — Binary Tree Level Order Traversal
// ============================================================================
//
// PROBLEM
// -------
// Given the root of a binary tree, return the level order traversal of its
// nodes' values (i.e., from left to right, level by level).
//
// Example:
//   Input:  [3, 9, 20, null, null, 15, 7]
//   Output: [[3], [9, 20], [15, 7]]
//
// APPROACH — Tree BFS (queue + level-size snapshot)
// --------------------------------------------------
// This is the textbook Tree BFS problem: the wording "level order" is the
// direct recognition signal for this pattern (see ../README.md).
//
// Push the root onto a queue. Repeatedly: snapshot `level_size = q.size()`
// BEFORE the inner loop, then dequeue exactly `level_size` nodes, recording
// each value and pushing its non-null children. Because children are pushed
// only after the snapshot was taken, they are invisible to the current
// level's inner loop and only get processed once the outer loop re-reads
// `q.size()` on its next pass. That single discipline is what keeps every
// level's values in their own bucket instead of bleeding into each other.
//
// COMPLEXITY
// ----------
// Time:  O(n) — every node is enqueued once and dequeued once, O(1) work each.
// Space: O(n) worst case — a wide, shallow tree can have up to ~n/2 nodes in
//        its widest level, all sitting in the queue at the same time.
// ============================================================================

#include <iostream>
#include <queue>
#include <string>
#include <vector>

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

std::vector<std::vector<int>> levelOrder(TreeNode* root) {
  std::vector<std::vector<int>> result;
  if (root == nullptr) {
    return result;
  }

  std::queue<TreeNode*> q;
  q.push(root);

  while (!q.empty()) {
    size_t level_size = q.size();  // Snapshot: the level-size trick.
    std::vector<int> level_values;
    level_values.reserve(level_size);

    for (size_t i = 0; i < level_size; ++i) {
      TreeNode* node = q.front();
      q.pop();
      level_values.push_back(node->val);

      if (node->left != nullptr) {
        q.push(node->left);
      }
      if (node->right != nullptr) {
        q.push(node->right);
      }
    }

    result.push_back(std::move(level_values));
  }

  return result;
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
    // [3, 9, 20, null, null, 15, 7] -> [[3], [9, 20], [15, 7]]
    TreeNode* root = new TreeNode(3);
    root->left = new TreeNode(9);
    root->right = new TreeNode(20);
    root->right->left = new TreeNode(15);
    root->right->right = new TreeNode(7);

    std::vector<std::vector<int>> expected = {{3}, {9, 20}, {15, 7}};
    check(levelOrder(root) == expected, "classic example -> [[3],[9,20],[15,7]]");

    freeTree(root);
  }

  {
    check(levelOrder(nullptr).empty(), "empty tree -> []");
  }

  {
    TreeNode* root = new TreeNode(1);
    std::vector<std::vector<int>> expected = {{1}};
    check(levelOrder(root) == expected, "single node -> [[1]]");
    freeTree(root);
  }

  {
    // A perfectly balanced tree of depth 3: 7 nodes across 3 levels.
    //   Level 0: 1
    //   Level 1: 2  3
    //   Level 2: 4 5 6 7
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    root->right->left = new TreeNode(6);
    root->right->right = new TreeNode(7);

    std::vector<std::vector<int>> expected = {{1}, {2, 3}, {4, 5, 6, 7}};
    check(levelOrder(root) == expected, "perfect tree depth 3 -> [[1],[2,3],[4,5,6,7]]");

    freeTree(root);
  }

  {
    // Left-skewed chain: 1 -> 2 -> 3, each only a left child.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->left->left = new TreeNode(3);

    std::vector<std::vector<int>> expected = {{1}, {2}, {3}};
    check(levelOrder(root) == expected, "left-skewed chain -> [[1],[2],[3]]");

    freeTree(root);
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
