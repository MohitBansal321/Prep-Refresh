// ============================================================================
// EXERCISE 01 (Easy) — LeetCode 637: Average of Levels in Binary Tree
// ============================================================================
//
// This file is a SELF-TEST, not a solution. The assertions in main() are the
// specification; fill in the stub until they all print [PASS].
//
//   g++ -std=c++17 -Wall exercises/01-average-of-levels.cpp -o /tmp/ex01 && /tmp/ex01
//
// It compiles as-is, so you get a failing baseline immediately. No solution is
// provided anywhere in this repo — see ../exercises.md for the task write-up.
//
// TASK
// ----
// Return a vector<double>: the average of the node values on each level,
// ordered from the root's level downward.
//
// HINTS — read one at a time, only when genuinely stuck.
//   [1] After ~10 min: the traversal is byte-for-byte ../code.cpp's levelOrder.
//       Only the per-level state and the commit line change.
//   [2] After ~15 min: you do NOT need a separate counter for the denominator.
//       Look at what you already snapshotted.
//   [3] After ~20 min: test 5 is failing for a reason that has nothing to do
//       with BFS. Look at the TYPE of your accumulator, not your logic.
// ============================================================================

#include <climits>
#include <cmath>
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

// ----------------------------------------------------------------------------
// YOUR CODE HERE
// ----------------------------------------------------------------------------
std::vector<double> averageOfLevels(TreeNode* root) {
  (void)root;  // remove this line once you use the parameter
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

// Compares two double vectors within a tolerance, so 14.5 == 14.499999999 .
static bool nearly(const std::vector<double>& got,
                   const std::vector<double>& want) {
  if (got.size() != want.size()) return false;
  for (size_t i = 0; i < got.size(); ++i) {
    if (std::fabs(got[i] - want[i]) > 1e-9) return false;
  }
  return true;
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
    // [3, 9, 20, null, null, 15, 7] -> [3.0, 14.5, 11.0]
    TreeNode* root = new TreeNode(3);
    root->left = new TreeNode(9);
    root->right = new TreeNode(20);
    root->right->left = new TreeNode(15);
    root->right->right = new TreeNode(7);
    check(nearly(averageOfLevels(root), {3.0, 14.5, 11.0}),
          "classic example -> [3.0, 14.5, 11.0]");
    freeTree(root);
  }

  {
    check(averageOfLevels(nullptr).empty(), "empty tree -> []");
  }

  {
    TreeNode* root = new TreeNode(7);
    check(nearly(averageOfLevels(root), {7.0}), "single node -> [7.0]");
    freeTree(root);
  }

  {
    // Left-skewed chain: every level holds exactly one node, so each average
    // is that node's own value. Catches a wrong loop bound that merges levels.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->left->left = new TreeNode(3);
    check(nearly(averageOfLevels(root), {1.0, 2.0, 3.0}),
          "left-skewed chain -> one node per level");
    freeTree(root);
  }

  {
    // THE TRAP. Two INT_MAX values on one level. Their sum does not fit in a
    // 32-bit int, so an `int` accumulator overflows before you ever divide.
    // The BFS is not what is wrong here.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(INT_MAX);
    root->right = new TreeNode(INT_MAX);
    check(nearly(averageOfLevels(root), {1.0, 2147483647.0}),
          "level of two INT_MAX values -> accumulator must not overflow");
    freeTree(root);
  }

  {
    // Negative values, and a level whose average is not a whole number.
    TreeNode* root = new TreeNode(-10);
    root->left = new TreeNode(-3);
    root->right = new TreeNode(-4);
    check(nearly(averageOfLevels(root), {-10.0, -3.5}),
          "negative values -> [-10.0, -3.5]");
    freeTree(root);
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
