// ============================================================================
// LeetCode 113 — Path Sum II
// ============================================================================
//
// PROBLEM
// -------
// Given the root of a binary tree and an integer targetSum, return ALL
// root-to-leaf paths where the sum of the node values along the path equals
// targetSum. Each path should be returned as a list of the node values along
// it, in order from root to leaf.
//
// Example: root = [5,4,8,11,null,13,4,7,2,null,null,5,1], targetSum = 22
//          -> [[5,4,11,2], [5,8,4,5]]
//
// APPROACH — Tree DFS, preorder, with EXPLICIT BACKTRACKING on a shared,
//            mutable path vector
// -----------------------------------------------------------------------
// This problem is deliberately solved with a SINGLE shared `currentPath`
// vector reused across every recursive call, instead of copying the path at
// every level (as 02-binary-tree-paths.cpp does with strings). That means
// every call must undo exactly what it did before returning to its caller --
// the "backtracking" discipline described in ../README.md's Common Mistakes
// section:
//
//   1. push_back the current node's value onto currentPath, and subtract it
//      from the running remainder.
//   2. if this is a leaf AND the remainder is now exactly 0, currentPath IS
//      a valid answer right now -- copy it into results (a COPY, not a
//      reference -- currentPath keeps being mutated after this point, so
//      storing a reference to it would later change the recorded answer).
//   3. otherwise, recurse into whichever children exist.
//   4. CRITICAL: after both recursive calls return (or immediately after step
//      2/3 for a leaf), pop_back the value that was pushed in step 1. This
//      restores currentPath to exactly the state the CALLER expects, so that
//      when control returns to a sibling branch, that branch does not
//      inherit a value left over from a branch that has already been fully
//      explored and abandoned.
//
// This push-then-pop bracketing around the two recursive calls is the exact
// same discipline used in Backtracking (see
// ../../recursion-backtracking-patterns/backtracking/) -- Path Sum II is a
// natural bridge example between plain Tree DFS and full Backtracking.
//
// COMPLEXITY
// ----------
// Time:  O(n^2) worst case -- O(n) calls, but copying currentPath into
//        results at every valid leaf costs O(h) per copy, and in the worst
//        (degenerate) tree shape h can be O(n), with up to O(n) valid paths.
//        A tighter, more typical bound is O(n * h).
// Space: O(h) for the recursion's call stack and for currentPath itself
//        (never larger than the current depth, thanks to the pop_back
//        discipline), plus O(n * h) for the output (inherent to the
//        problem, not the traversal).
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

void findPaths(TreeNode* node, int remaining, std::vector<int>& currentPath,
               std::vector<std::vector<int>>& results) {
  if (!node) return;  // Base case: no node, nothing to add or recurse into.

  // --- "Go down": commit this node to the shared path. ---
  currentPath.push_back(node->val);
  remaining -= node->val;

  bool isLeaf = (!node->left && !node->right);
  if (isLeaf) {
    if (remaining == 0) {
      // Copy currentPath NOW -- it will keep changing after we return from
      // this call, so a stored reference/pointer to it would be wrong later.
      results.push_back(currentPath);
    }
  } else {
    findPaths(node->left, remaining, currentPath, results);
    findPaths(node->right, remaining, currentPath, results);
  }

  // --- "Backtrack": undo exactly what was committed above, so the CALLER
  // (and any sibling branch explored after we return) sees the path exactly
  // as it was before this call started. ---
  currentPath.pop_back();
}

std::vector<std::vector<int>> pathSum(TreeNode* root, int targetSum) {
  std::vector<std::vector<int>> results;
  std::vector<int> currentPath;
  findPaths(root, targetSum, currentPath, results);
  return results;
}

// ----------------------------------------------------------------------------
// Test helpers.
//
//   5 (root)
//     -- left  --> 4 -- left  --> 11 -- left  --> 7 (leaf)
//                                    -- right --> 2 (leaf)
//     -- right --> 8 -- left  --> 13 (leaf)
//                   -- right --> 4 -- left  --> 5 (leaf)
//                                  -- right --> 1 (leaf)
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
  root->right->right->left = new TreeNode(5);
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

  {
    TreeNode* root = buildExampleTree();
    std::vector<std::vector<int>> paths = pathSum(root, 22);
    std::vector<std::vector<int>> expected = {{5, 4, 11, 2}, {5, 8, 4, 5}};
    check(paths == expected, "example tree, target 22 -> [[5,4,11,2],[5,8,4,5]]");
    deleteTree(root);
  }

  {
    TreeNode* root = buildExampleTree();
    std::vector<std::vector<int>> paths = pathSum(root, 1000);
    check(paths.empty(), "example tree, unreachable target -> no paths");
    deleteTree(root);
  }

  {
    // Every recursive call must leave currentPath exactly as it found it;
    // running the search twice on the same tree with the same target is a
    // simple way to catch a missing pop_back (a leak would corrupt the
    // SECOND run's results, since currentPath is fresh here but any bug in
    // findPaths's balance of push/pop would still show up as a wrong-length
    // or wrong-content path within a single run).
    TreeNode* root = buildExampleTree();
    std::vector<std::vector<int>> first = pathSum(root, 22);
    std::vector<std::vector<int>> second = pathSum(root, 22);
    check(first == second, "repeated calls on the same tree are deterministic (no state leak)");
    deleteTree(root);
  }

  {
    TreeNode* single = new TreeNode(5);
    std::vector<std::vector<int>> paths = pathSum(single, 5);
    std::vector<std::vector<int>> expected = {{5}};
    check(paths == expected, "single node equal to target -> [[5]]");
    deleteTree(single);
  }

  {
    std::vector<std::vector<int>> paths = pathSum(nullptr, 0);
    check(paths.empty(), "empty tree -> no paths");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
