// ============================================================================
// Tree BFS — generic reusable template (C++17)
// ============================================================================
//
// This file is NOT tied to one specific LeetCode problem. It demonstrates the
// core mechanic every Tree BFS problem reuses: a queue that holds exactly one
// level's worth of nodes at a time, processed with the "level size snapshot"
// trick — record `queue.size()` BEFORE the inner loop starts, so children
// pushed onto the queue during this level are never mistaken for members of
// the current level.
//
// Two small, generic functions are provided:
//
//   1. levelOrder(root)  — collects every level into its own vector<int>,
//      returning vector<vector<int>>. This is the direct ancestor of
//      problems/01-binary-tree-level-order-traversal.cpp.
//
//   2. minDepth(root)    — returns as soon as the FIRST leaf is reached,
//      which is exactly why BFS (not DFS) is the natural fit for "minimum"
//      depth: BFS visits nodes in strictly non-decreasing depth order, so
//      the first leaf found is guaranteed to be at the shallowest depth.
//      This is the direct ancestor of
//      problems/03-minimum-depth-of-binary-tree.cpp.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out_tree_bfs_code && /tmp/out_tree_bfs_code
// ============================================================================

#include <iostream>
#include <queue>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// The tree node every function in this file (and every file in problems/)
// operates on. Deliberately the same shape LeetCode uses, so the functions
// here transfer directly onto real problem statements without adaptation.
// ----------------------------------------------------------------------------
struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// ----------------------------------------------------------------------------
// Helper: build a small, fixed example tree for the demos in main().
//
//   Level 0:      3
//   Level 1:    9    20
//   Level 2:        15  7   (15 and 7 are children of 20; 9 has no children)
//
// This is the same tree used throughout the README's diagrams and trace, so
// readers can follow one concrete example from prose to diagram to code.
// ----------------------------------------------------------------------------
TreeNode* buildSampleTree() {
  TreeNode* root = new TreeNode(3);
  root->left = new TreeNode(9);
  root->right = new TreeNode(20);
  root->right->left = new TreeNode(15);
  root->right->right = new TreeNode(7);
  return root;
}

// Recursively frees every node in a tree. Every tree built with `new` in this
// file (and in problems/*.cpp) must be paired with a call to this, or the
// equivalent, to avoid leaking memory — there is no garbage collector in C++.
void freeTree(TreeNode* root) {
  if (root == nullptr) {
    return;
  }
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

// ----------------------------------------------------------------------------
// levelOrder — the canonical Tree BFS routine.
//
// Returns one vector<int> per level, top (root) to bottom, left to right
// within each level.
//
// THE SIZE-SNAPSHOT TRICK: `level_size = q.size()` is read once, before the
// inner for-loop begins. Without this snapshot, `q.size()` would grow as
// children are pushed during the loop, and the inner loop would keep
// consuming newly-added next-level nodes as if they belonged to the current
// level — silently merging two levels into one.
// ----------------------------------------------------------------------------
std::vector<std::vector<int>> levelOrder(TreeNode* root) {
  std::vector<std::vector<int>> result;
  if (root == nullptr) {
    return result;  // An empty tree has zero levels.
  }

  std::queue<TreeNode*> q;
  q.push(root);

  while (!q.empty()) {
    size_t level_size = q.size();  // Snapshot: exactly this many nodes belong
                                    // to the level we are about to process.
    std::vector<int> level_values;
    level_values.reserve(level_size);

    for (size_t i = 0; i < level_size; ++i) {
      TreeNode* node = q.front();
      q.pop();
      level_values.push_back(node->val);

      // Enqueue children AFTER recording the snapshot above, so they are
      // processed on the NEXT iteration of the outer while-loop, not this one.
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

// ----------------------------------------------------------------------------
// minDepth — BFS's signature "stop early" advantage over DFS.
//
// Returns the number of nodes on the shortest root-to-leaf path (a single
// node counts as depth 1). BFS explores depth 1, then depth 2, then depth 3,
// in strict order, so the FIRST leaf it encounters is provably at the
// minimum depth — no need to explore the rest of the tree once that leaf is
// found. A DFS solution has to explore every root-to-leaf path and take a
// running minimum, because it cannot know in advance which path is shortest.
//
// Edge case worth calling out: a node with exactly ONE child is NOT a leaf.
// LeetCode 111 is explicit about this — depth is measured to the nearest
// LEAF (zero children), not the nearest node with a missing child.
// ----------------------------------------------------------------------------
int minDepth(TreeNode* root) {
  if (root == nullptr) {
    return 0;  // An empty tree has depth 0 by convention.
  }

  std::queue<TreeNode*> q;
  q.push(root);
  int depth = 1;

  while (!q.empty()) {
    size_t level_size = q.size();  // Same snapshot trick as levelOrder.

    for (size_t i = 0; i < level_size; ++i) {
      TreeNode* node = q.front();
      q.pop();

      // A leaf is a node with NO children — not a node missing just one.
      if (node->left == nullptr && node->right == nullptr) {
        return depth;  // First leaf found at this depth: guaranteed minimum.
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

  return depth;  // Unreachable when root != nullptr (every node eventually
                 // reaches a leaf), kept only so the function has a
                 // well-defined return on every path.
}

// ============================================================================
// main() — demonstrates both functions with printed, verifiable output.
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

  std::cout << "--- levelOrder ---\n";
  {
    TreeNode* root = buildSampleTree();
    std::vector<std::vector<int>> result = levelOrder(root);
    std::vector<std::vector<int>> expected = {{3}, {9, 20}, {15, 7}};
    check(result == expected, "sample tree -> [[3],[9,20],[15,7]]");
    freeTree(root);
  }
  {
    std::vector<std::vector<int>> result = levelOrder(nullptr);
    check(result.empty(), "empty tree -> []");
  }
  {
    TreeNode* root = new TreeNode(42);
    std::vector<std::vector<int>> result = levelOrder(root);
    std::vector<std::vector<int>> expected = {{42}};
    check(result == expected, "single node -> [[42]]");
    freeTree(root);
  }

  std::cout << "\n--- minDepth ---\n";
  {
    TreeNode* root = buildSampleTree();
    check(minDepth(root) == 2, "sample tree -> min depth 2 (root -> 9)");
    freeTree(root);
  }
  {
    check(minDepth(nullptr) == 0, "empty tree -> min depth 0");
  }
  {
    // One-sided chain: 1 -> right(2) -> right(3). The only leaf is 3, at
    // depth 3. This exercises the "single child is not a leaf" rule: a naive
    // solution that stops at the first node missing ANY child would
    // wrongly report depth 1 (node 1 has no left child) or depth 2.
    TreeNode* root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->right = new TreeNode(3);
    check(minDepth(root) == 3,
          "one-sided chain -> min depth 3 (single-child nodes are not leaves)");
    freeTree(root);
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
