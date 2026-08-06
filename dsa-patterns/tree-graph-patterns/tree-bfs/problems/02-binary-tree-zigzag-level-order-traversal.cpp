// ============================================================================
// LeetCode 103 — Binary Tree Zigzag Level Order Traversal
// ============================================================================
//
// PROBLEM
// -------
// Given the root of a binary tree, return the zigzag level order traversal
// of its nodes' values (i.e., left to right, then right to left for the next
// level, alternating for every level).
//
// Example:
//   Input:  [3, 9, 20, null, null, 15, 7]
//   Output: [[3], [20, 9], [15, 7]]
//   (level 0 left-to-right, level 1 reversed, level 2 left-to-right again)
//
// APPROACH — Tree BFS (queue + level-size snapshot), with a direction flag
// -------------------------------------------------------------------------
// The traversal mechanic is IDENTICAL to plain level order (LeetCode 102,
// see 01-binary-tree-level-order-traversal.cpp): a queue, a level-size
// snapshot before the inner loop, dequeue exactly that many nodes, push
// non-null children for the next level. Zigzag adds exactly one thing on
// top: after collecting a level's values in the normal left-to-right order
// (which is what the queue naturally gives you, regardless of direction),
// reverse the vector before storing it if this level's index is odd.
//
// This is the clearest illustration in this module that the level-size
// snapshot IS the pattern — everything else (zigzag, next-pointers,
// minimum depth) is a thin, problem-specific layer on top of the identical
// queue skeleton.
//
// COMPLEXITY
// ----------
// Time:  O(n) — same traversal as plain level order, plus an O(level_size)
//        reversal per odd level, which is still O(n) total across all
//        levels combined (each node is reversed-past at most once).
// Space: O(n) worst case for the queue, same as plain level order.
// ============================================================================

#include <algorithm>
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

std::vector<std::vector<int>> zigzagLevelOrder(TreeNode* root) {
  std::vector<std::vector<int>> result;
  if (root == nullptr) {
    return result;
  }

  std::queue<TreeNode*> q;
  q.push(root);
  bool left_to_right = true;

  while (!q.empty()) {
    size_t level_size = q.size();  // Same snapshot trick as plain level order.
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

    // The queue always yields a level in natural left-to-right order.
    // Zigzag only changes how we STORE that level, not how we traverse it.
    if (!left_to_right) {
      std::reverse(level_values.begin(), level_values.end());
    }
    result.push_back(std::move(level_values));

    left_to_right = !left_to_right;  // Alternate direction for the next level.
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
    // [3, 9, 20, null, null, 15, 7] -> [[3], [20, 9], [15, 7]]
    TreeNode* root = new TreeNode(3);
    root->left = new TreeNode(9);
    root->right = new TreeNode(20);
    root->right->left = new TreeNode(15);
    root->right->right = new TreeNode(7);

    std::vector<std::vector<int>> expected = {{3}, {20, 9}, {15, 7}};
    check(zigzagLevelOrder(root) == expected, "classic example -> [[3],[20,9],[15,7]]");

    freeTree(root);
  }

  {
    check(zigzagLevelOrder(nullptr).empty(), "empty tree -> []");
  }

  {
    TreeNode* root = new TreeNode(1);
    std::vector<std::vector<int>> expected = {{1}};
    check(zigzagLevelOrder(root) == expected, "single node -> [[1]]");
    freeTree(root);
  }

  {
    // Perfect tree, depth 4, to exercise a third alternation:
    //   Level 0: 1                    (forward)
    //   Level 1: 2  3                 (reversed  -> 3 2)
    //   Level 2: 4 5 6 7              (forward)
    //   Level 3: 8 9 10 11 12 13 14 15 (reversed)
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    root->right->left = new TreeNode(6);
    root->right->right = new TreeNode(7);
    root->left->left->left = new TreeNode(8);
    root->left->left->right = new TreeNode(9);
    root->left->right->left = new TreeNode(10);
    root->left->right->right = new TreeNode(11);
    root->right->left->left = new TreeNode(12);
    root->right->left->right = new TreeNode(13);
    root->right->right->left = new TreeNode(14);
    root->right->right->right = new TreeNode(15);

    std::vector<std::vector<int>> expected = {
        {1}, {3, 2}, {4, 5, 6, 7}, {15, 14, 13, 12, 11, 10, 9, 8}};
    check(zigzagLevelOrder(root) == expected,
          "perfect tree depth 4 -> alternating direction on every level");

    freeTree(root);
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
