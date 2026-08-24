// ============================================================================
// EXERCISE 03 (Hard) — LeetCode 297: Serialize and Deserialize Binary Tree
// ============================================================================
//
// This file is a SELF-TEST, not a solution. The assertions in main() are the
// specification; fill in the two stubs until they all print [PASS].
//
//   g++ -std=c++17 -Wall exercises/03-serialize-deserialize.cpp -o /tmp/ex03 && /tmp/ex03
//
// It compiles as-is, so you get a failing baseline immediately. No solution is
// provided anywhere in this repo — see ../exercises.md for the task write-up.
//
// TASK
// ----
// Encode an arbitrary binary tree to a string, and decode that string back to
// an identical tree. Values may be negative. Nodes may have one child or none.
// ../exercises.md requires you to do BOTH halves with BFS, not recursion.
//
// NOTE ON WHAT IS TESTED
// ----------------------
// These tests never inspect your string format — that is your design choice
// ("#" placeholders, a length prefix, whatever you like). They test the only
// thing that is actually required: that deserialize(serialize(t)) reproduces t
// exactly. One test does check that the format survives a round-trip TWICE, so
// a format that is merely self-consistent still counts as correct.
//
// HINTS — read one at a time, only when genuinely stuck.
//   [1] After ~10 min: on the serialize side, push children UNCONDITIONALLY and
//       emit a placeholder token for each null. Skipping nulls makes the string
//       ambiguous — that is why test 5 and test 6 are different trees.
//   [2] After ~20 min: deserialize needs a second queue, holding the parents
//       that are still waiting for their two children.
//   [3] After ~30 min: ../problems/04-populating-next-right-pointers-ii.cpp has
//       a buildTree helper at the bottom. It is already this exact algorithm,
//       written for a vector<int> with an INT_MIN sentinel. Read it, then port.
// ============================================================================

#include <iostream>
#include <queue>
#include <sstream>
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
std::string serialize(TreeNode* root) {
  (void)root;  // remove this line once you use the parameter
  return "";
}

TreeNode* deserialize(const std::string& data) {
  (void)data;  // remove this line once you use the parameter
  return nullptr;
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

// Structural equality: same shape AND same values, everywhere.
static bool sameTree(TreeNode* a, TreeNode* b) {
  if (a == nullptr && b == nullptr) return true;
  if (a == nullptr || b == nullptr) return false;
  if (a->val != b->val) return false;
  return sameTree(a->left, b->left) && sameTree(a->right, b->right);
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

  // Runs one tree through serialize -> deserialize and compares structurally.
  auto roundTrip = [&](TreeNode* original, const std::string& label) {
    TreeNode* rebuilt = deserialize(serialize(original));
    check(sameTree(original, rebuilt), label);
    freeTree(rebuilt);
  };

  {
    // [1, 2, 3, null, null, 4, 5]
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->right->left = new TreeNode(4);
    root->right->right = new TreeNode(5);
    roundTrip(root, "classic example round-trips");
    freeTree(root);
  }

  {
    TreeNode* rebuilt = deserialize(serialize(nullptr));
    check(rebuilt == nullptr, "empty tree round-trips to nullptr");
    freeTree(rebuilt);
  }

  {
    TreeNode* root = new TreeNode(42);
    roundTrip(root, "single node round-trips");
    freeTree(root);
  }

  {
    // Negative values, including one that a "#" placeholder must not collide
    // with, and a value containing a '-' that must survive tokenizing.
    TreeNode* root = new TreeNode(-1);
    root->left = new TreeNode(-2147483648);
    root->right = new TreeNode(0);
    roundTrip(root, "negative values (incl. INT_MIN) round-trip");
    freeTree(root);
  }

  {
    // THE AMBIGUITY PAIR, part 1: a left-only chain of three.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->left->left = new TreeNode(3);
    roundTrip(root, "left-skewed chain round-trips");
    freeTree(root);
  }

  {
    // THE AMBIGUITY PAIR, part 2: a right-only chain of the SAME three values.
    // If your format drops nulls, both trees serialize to "1 2 3" and one of
    // these two tests must fail. Placeholders are what tell them apart.
    TreeNode* root = new TreeNode(1);
    root->right = new TreeNode(2);
    root->right->right = new TreeNode(3);
    roundTrip(root, "right-skewed chain round-trips (distinct from left)");
    freeTree(root);
  }

  {
    // Single-child nodes on both sides at the same level — the shape that
    // breaks "children are always pushed in pairs" assumptions on decode.
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->right = new TreeNode(4);
    root->right->left = new TreeNode(5);
    roundTrip(root, "mixed single-child nodes round-trip");
    freeTree(root);
  }

  {
    // Idempotence: serializing a rebuilt tree must produce the same string as
    // serializing the original. Catches a decoder that silently reorders or
    // loses trailing placeholders.
    TreeNode* root = new TreeNode(5);
    root->left = new TreeNode(3);
    root->right = new TreeNode(8);
    root->left->left = new TreeNode(1);
    root->right->right = new TreeNode(9);

    const std::string once = serialize(root);
    TreeNode* rebuilt = deserialize(once);
    const std::string twice = serialize(rebuilt);
    check(once == twice && !once.empty(),
          "serialize is stable across a round-trip");
    freeTree(rebuilt);
    freeTree(root);
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
