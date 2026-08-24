// ============================================================================
// EXERCISE 02 (Medium) — LeetCode 199: Binary Tree Right Side View
// ============================================================================
//
// This file is a SELF-TEST, not a solution. The assertions in main() are the
// specification; fill in the stub until they all print [PASS].
//
//   g++ -std=c++17 -Wall exercises/02-right-side-view.cpp -o /tmp/ex02 && /tmp/ex02
//
// It compiles as-is, so you get a failing baseline immediately. No solution is
// provided anywhere in this repo — see ../exercises.md for the task write-up.
//
// TASK
// ----
// Standing to the right of the tree, return the values you can see, top to
// bottom — i.e. the RIGHTMOST node of every level.
//
// ../exercises.md asks you to solve this TWICE: once with an index test inside
// the inner loop, once with a `TreeNode* last` cursor committed after it. Both
// must pass this same file. Do the second version only after the first is green.
//
// HINTS — read one at a time, only when genuinely stuck.
//   [1] After ~10 min: you do not need a per-level vector at all. You need one
//       value per level. Which iteration of the inner loop produces it?
//   [2] After ~15 min: FIFO order means the LAST node dequeued from a level's
//       snapshot batch is the rightmost one on that level.
//   [3] After ~20 min: test 4 is the tree that kills the tempting shortcut.
//       If you are walking node->right down from the root, stop and re-read
//       what "rightmost on a level" actually means.
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

// ----------------------------------------------------------------------------
// YOUR CODE HERE
// ----------------------------------------------------------------------------
std::vector<int> rightSideView(TreeNode* root) {
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
    // [1, 2, 3, null, 5, null, 4] -> [1, 3, 4]
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->right = new TreeNode(5);
    root->right->right = new TreeNode(4);
    check(rightSideView(root) == std::vector<int>({1, 3, 4}),
          "classic example -> [1, 3, 4]");
    freeTree(root);
  }

  {
    check(rightSideView(nullptr).empty(), "empty tree -> []");
  }

  {
    TreeNode* root = new TreeNode(7);
    check(rightSideView(root) == std::vector<int>({7}), "single node -> [7]");
    freeTree(root);
  }

  {
    // THE TRAP (../exercises.md's "Think about" tree). Root's right child is a
    // leaf; root's left child has a left child of its own. Walking node->right
    // from the root yields [1, 3] and never sees node 4 — but 4 IS the only
    // node on level 2, so it is trivially the rightmost node on its level.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    check(rightSideView(root) == std::vector<int>({1, 3, 4}),
          "deep-left tree -> [1, 3, 4], NOT [1, 3]");
    freeTree(root);
  }

  {
    // Left-skewed chain: every node is the only node on its level, so every
    // node is visible from the right.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->left->left = new TreeNode(3);
    root->left->left->left = new TreeNode(4);
    check(rightSideView(root) == std::vector<int>({1, 2, 3, 4}),
          "left-skewed chain -> every node visible");
    freeTree(root);
  }

  {
    // Right-skewed chain: the shortcut happens to be correct here. Passing this
    // while failing test 4 is the signature of the node->right walk.
    TreeNode* root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->right = new TreeNode(3);
    check(rightSideView(root) == std::vector<int>({1, 2, 3}),
          "right-skewed chain -> [1, 2, 3]");
    freeTree(root);
  }

  {
    // A wide bottom level: only its last node counts, and negative values must
    // survive (a sentinel-based cursor that inits to 0 or -1 breaks here).
    TreeNode* root = new TreeNode(-1);
    root->left = new TreeNode(-2);
    root->right = new TreeNode(-3);
    root->left->left = new TreeNode(-4);
    root->left->right = new TreeNode(-5);
    root->right->left = new TreeNode(-6);
    check(rightSideView(root) == std::vector<int>({-1, -3, -6}),
          "negative values, wide level -> [-1, -3, -6]");
    freeTree(root);
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
