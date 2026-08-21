// ============================================================================
// LeetCode 117 — Populating Next Right Pointers in Each Node II
// https://leetcode.com/problems/populating-next-right-pointers-in-each-node-ii/
// ============================================================================
//
// PROBLEM
// -------
// Given a binary tree whose nodes carry an extra `next` pointer (initially
// nullptr), populate every `next` pointer so that it points to the node
// immediately to its right ON THE SAME LEVEL. The last node of each level
// must keep `next == nullptr`. Return the root.
//
// The tree is an ARBITRARY binary tree — nodes may have one child, or none.
// (LeetCode 116 is the same problem restricted to a PERFECT binary tree; the
// BFS solution below solves both without modification, which is why 116 is
// the version named in ../README.md's Further Reading. 117 is the general
// case, so this file solves 117 and 116 falls out as a special case.)
//
// Example:
//   Input:  [1, 2, 3, 4, 5, null, 7]
//
//     1
//     |-- left  --> 2
//     |             |-- left  --> 4
//     |             |-- right --> 5
//     |-- right --> 3
//                   |-- right --> 7   (node 3 has NO left child)
//
//   Output — the `next` chains this must produce, one per level:
//     level 0:  1 -> nullptr
//     level 1:  2 -> 3 -> nullptr
//     level 2:  4 -> 5 -> 7 -> nullptr
//
//   Note level 2: the link 5 -> 7 joins two nodes with DIFFERENT parents,
//   and those parents (2 and 3) are not even a matching left/right pair of
//   the same node. That is the whole difficulty of 117 over 116.
//
// APPROACH 1 — Tree BFS (queue + level-size snapshot)
// ---------------------------------------------------
// This is the reason problem 04 is in this module: it is the one problem here
// where the level-size snapshot is used not to COLLECT a level's values (as
// in 01-binary-tree-level-order-traversal.cpp and
// 02-binary-tree-zigzag-level-order-traversal.cpp) but to MUTATE the tree,
// wiring each dequeued node to the one dequeued just before it. The snapshot
// is doing strictly more work here than it does in 01/02: there it only
// grouped output, here it defines the exact boundary at which linking must
// STOP. Get the snapshot wrong and you do not merely mis-group a printed
// list — you build a `next` chain that runs off the end of a level into the
// next level, permanently corrupting the tree's structure.
//
// The whole algorithm is: for each level, walk its snapshot-bounded batch,
// keeping a `prev` pointer to the previously dequeued node of THIS level, and
// set `prev->next = node`. `prev` is reset to nullptr at the start of every
// level (see the trap below).
//
// APPROACH 2 — O(1) extra space, using the level ABOVE as its own queue
// ----------------------------------------------------------------------
// The classic follow-up, and the reason this file is the hardest in the set:
// solve it without the O(n) queue that ../README.md's Disadvantages section
// calls out as Tree BFS's main space cost. The insight is that once level k
// is fully linked, that chain of `next` pointers IS a ready-made, in-place
// iterator over level k — so you can walk level k left-to-right for free and
// stitch level k+1 together as you go, using a small dummy-head node to
// remember where level k+1 begins. The tree's own `next` pointers replace the
// queue entirely: O(1) extra space instead of O(n).
//
// Both functions are implemented and tested below against the same trees, so
// you can see that they agree on every case — including the sparse ones where
// the two nodes that must be linked are COUSINS whose parents are not
// adjacent.
//
// THE TRAP — resetting `prev` at every level boundary
// ---------------------------------------------------
// In the BFS version, the single most common bug is declaring `prev` OUTSIDE
// the outer while-loop (or failing to reset it to nullptr at the top of each
// level). If `prev` survives across the level boundary, then after the last
// node of level k is dequeued, the first node of level k+1 gets linked to it:
// `last_of_level_k->next = first_of_level_k+1`. Nothing crashes, and every
// node still has a plausible-looking `next` — but the chains now snake
// diagonally through the whole tree instead of terminating at each level's
// right edge. The `nextChains` verifier below catches exactly this, because
// walking a corrupted chain from the root yields one long row instead of one
// row per level.
//
// COMPLEXITY
// ----------
// Approach 1 (BFS):  Time O(n) — each node enqueued and dequeued once, O(1)
//                    work per node. Space O(n) worst case for the queue (the
//                    widest level of a complete tree holds ~n/2 nodes).
// Approach 2 (O(1)): Time O(n) — each node is visited exactly twice, once as
//                    a member of the level being walked and once as a child
//                    being linked. Space O(1) — one dummy node and three
//                    pointers, regardless of tree size or shape.
// ============================================================================

#include <climits>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// LeetCode's node type for problems 116/117: the familiar TreeNode plus one
// extra `next` pointer. This is why ../README.md's Code Walkthrough notes
// that problem 04 defines its own `Node` rather than the `TreeNode` used by
// code.cpp and problems 01-03 — the extra field is the whole point.
// ----------------------------------------------------------------------------
struct Node {
  int val;
  Node* left;
  Node* right;
  Node* next;
  Node(int x) : val(x), left(nullptr), right(nullptr), next(nullptr) {}
};

// ============================================================================
// APPROACH 1 — Tree BFS with the level-size snapshot
// ============================================================================
Node* connectBFS(Node* root) {
  if (root == nullptr) {
    return nullptr;  // No nodes, nothing to link.
  }

  std::queue<Node*> q;
  q.push(root);

  while (!q.empty()) {
    size_t level_size = q.size();  // Snapshot BEFORE consuming: exactly this
                                   // many nodes belong to the current level.

    // `prev` MUST be declared here, inside the outer loop, so it is reset to
    // nullptr at the start of every level. See "THE TRAP" in the header
    // comment: hoisting this declaration out of the loop is the bug that
    // links each level's last node to the next level's first node.
    Node* prev = nullptr;

    for (size_t i = 0; i < level_size; ++i) {
      Node* node = q.front();
      q.pop();

      // The linking step. On the first iteration of a level, `prev` is
      // nullptr and nothing is written — correct, because the leftmost node
      // of a level is nobody's `next`. On every later iteration, the node
      // dequeued just before this one is immediately to its left on this
      // same level (guaranteed by the queue's FIFO order), so it is exactly
      // the node whose `next` should point here.
      if (prev != nullptr) {
        prev->next = node;
      }
      prev = node;

      // Children belong to the NEXT level. They were pushed after the
      // snapshot was taken, so this level's inner loop cannot see them.
      if (node->left != nullptr) {
        q.push(node->left);
      }
      if (node->right != nullptr) {
        q.push(node->right);
      }
    }

    // Note what is NOT needed here: an explicit `prev->next = nullptr`. Every
    // node's `next` starts as nullptr and is only ever written when a node to
    // its right on the same level is found, so the last node of each level is
    // left untouched and stays nullptr by construction.
  }

  return root;
}

// ============================================================================
// APPROACH 2 — O(1) extra space: the already-linked level above IS the queue
// ============================================================================
Node* connectConstantSpace(Node* root) {
  Node* level_head = root;  // Leftmost node of the level currently being READ.

  while (level_head != nullptr) {
    // A dummy node whose `next` will end up pointing at the leftmost node of
    // the level being BUILT. Using a dummy removes the "is this the first
    // child I have seen on this level?" special case entirely: `tail` always
    // points at something writable, so the link is unconditionally
    // `tail->next = child`.
    Node dummy(0);
    Node* tail = &dummy;

    // Walk the current level left-to-right using the `next` pointers that a
    // previous iteration already installed (for the root's own level, the
    // walk is trivially one node long). No queue is involved — the tree is
    // iterating itself.
    for (Node* cur = level_head; cur != nullptr; cur = cur->next) {
      if (cur->left != nullptr) {
        tail->next = cur->left;
        tail = cur->left;
      }
      if (cur->right != nullptr) {
        tail->next = cur->right;
        tail = cur->right;
      }
    }

    // `dummy.next` is the leftmost node of the level just built, or nullptr
    // if this level had no children at all — which is exactly the loop's
    // termination condition. This is also what makes the sparse/cousin cases
    // work with no extra code: parents with no children simply contribute
    // nothing to the chain, so two nodes with distant parents end up adjacent
    // in it automatically.
    level_head = dummy.next;

    // `dummy` goes out of scope here. It was a stack local, never heap
    // allocated, so there is nothing to free — the O(1) space claim is
    // literal, not amortized.
  }

  return root;
}

// ----------------------------------------------------------------------------
// Test helper: build a tree from a LeetCode-style level-order array, where
// NUL marks an absent child. The builder is itself a small BFS: dequeue a
// parent, consume the next two array slots as its children, enqueue whichever
// of them exist.
// ----------------------------------------------------------------------------
const int NUL = INT_MIN;

Node* buildTree(const std::vector<int>& level_order) {
  if (level_order.empty() || level_order[0] == NUL) {
    return nullptr;
  }

  Node* root = new Node(level_order[0]);
  std::queue<Node*> q;
  q.push(root);
  size_t i = 1;

  while (!q.empty() && i < level_order.size()) {
    Node* parent = q.front();
    q.pop();

    if (i < level_order.size()) {
      if (level_order[i] != NUL) {
        parent->left = new Node(level_order[i]);
        q.push(parent->left);
      }
      ++i;
    }
    if (i < level_order.size()) {
      if (level_order[i] != NUL) {
        parent->right = new Node(level_order[i]);
        q.push(parent->right);
      }
      ++i;
    }
  }

  return root;
}

void freeTree(Node* root) {
  if (root == nullptr) {
    return;
  }
  freeTree(root->left);
  freeTree(root->right);
  delete root;
}

// ----------------------------------------------------------------------------
// Verifier: read the tree back using ONLY the `next` pointers, one level at a
// time. Start at the root; walk its `next` chain to read a level; the head of
// the following level is the first child found while walking that chain.
//
// This is a strict check, not a lenient one. If a `next` pointer wrongly
// crossed a level boundary, the chain walk would keep going past the level's
// right edge and this function would return one over-long row instead of two
// correct ones — so a mismatch against the expected level grouping catches
// the "forgot to reset prev" bug described in the header comment.
// ----------------------------------------------------------------------------
std::vector<std::vector<int>> nextChains(Node* root) {
  std::vector<std::vector<int>> result;
  Node* level_head = root;

  while (level_head != nullptr) {
    std::vector<int> row;
    Node* next_level_head = nullptr;

    for (Node* cur = level_head; cur != nullptr; cur = cur->next) {
      row.push_back(cur->val);
      if (next_level_head == nullptr) {
        if (cur->left != nullptr) {
          next_level_head = cur->left;
        } else if (cur->right != nullptr) {
          next_level_head = cur->right;
        }
      }
    }

    result.push_back(row);
    level_head = next_level_head;
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

  // Runs BOTH implementations against a freshly built copy of the same tree
  // and checks each one independently, so a bug in either is reported on its
  // own line rather than hidden behind the other's success.
  auto checkBoth = [&](const std::vector<int>& level_order,
                       const std::vector<std::vector<int>>& expected,
                       const std::string& label) {
    Node* a = buildTree(level_order);
    connectBFS(a);
    check(nextChains(a) == expected, "BFS          | " + label);
    freeTree(a);

    Node* b = buildTree(level_order);
    connectConstantSpace(b);
    check(nextChains(b) == expected, "O(1) space   | " + label);
    freeTree(b);
  };

  std::cout << "--- LeetCode 117 (arbitrary binary tree) ---\n";

  // The problem statement's own example: node 3 has only a right child, so
  // level 2 is [4, 5, 7] and the link 5 -> 7 spans two different parents.
  checkBoth({1, 2, 3, 4, 5, NUL, 7}, {{1}, {2, 3}, {4, 5, 7}},
            "[1,2,3,4,5,null,7] -> [[1],[2,3],[4,5,7]]");

  // Empty tree: both functions must return nullptr without dereferencing it.
  {
    check(connectBFS(nullptr) == nullptr, "BFS          | empty tree -> nullptr, no crash");
    check(connectConstantSpace(nullptr) == nullptr,
          "O(1) space   | empty tree -> nullptr, no crash");
  }

  // Single node: the root is the only node on its level, so its `next` must
  // stay nullptr. A solution that unconditionally writes a `next` fails here.
  checkBoth({1}, {{1}}, "single node -> [[1]], root->next stays nullptr");

  std::cout << "\n--- Sparse shapes: the nodes to link are COUSINS ---\n";

  // Level 2's two nodes (4 and 5) hang off different parents, and neither
  // parent has two children. A solution that only ever links a parent's own
  // left child to its own right child would leave 4->next == nullptr.
  checkBoth({1, 2, 3, 4, NUL, NUL, 5}, {{1}, {2, 3}, {4, 5}},
            "left-child-then-right-child cousins -> [[1],[2,3],[4,5]]");

  // Two levels of the same cousin situation stacked, so the O(1) version has
  // to rebuild the chain from an already-sparse level rather than from a
  // perfect one.
  checkBoth({1, 2, 3, 4, NUL, NUL, 5, 6, NUL, NUL, 7},
            {{1}, {2, 3}, {4, 5}, {6, 7}},
            "two stacked sparse levels -> [[1],[2,3],[4,5],[6,7]]");

  // A whole subtree with no children at all (node 3 is a leaf): the level
  // walk must skip it silently and contribute nothing to the next chain.
  checkBoth({1, 2, 3, 4, 5, NUL, NUL}, {{1}, {2, 3}, {4, 5}},
            "childless right sibling -> [[1],[2,3],[4,5]]");

  // Left-skewed chain: every level has exactly one node, so every `next`
  // must be nullptr. This is the shape where a "forgot to reset prev" bug is
  // most visible — it would produce a single row [1,2,3,4].
  checkBoth({1, 2, NUL, 3, NUL, 4, NUL}, {{1}, {2}, {3}, {4}},
            "left-skewed chain -> one node per level, every next == nullptr");

  // Right-skewed chain: the mirror case.
  checkBoth({1, NUL, 2, NUL, 3}, {{1}, {2}, {3}},
            "right-skewed chain -> one node per level");

  std::cout << "\n--- LeetCode 116 (perfect tree) falls out as a special case ---\n";

  checkBoth({1, 2, 3, 4, 5, 6, 7}, {{1}, {2, 3}, {4, 5, 6, 7}},
            "perfect tree depth 3 -> [[1],[2,3],[4,5,6,7]]");

  std::cout << "\n--- Value-agnostic: duplicates and negatives ---\n";

  // The algorithm never compares or inspects `val`, only structure. All-same
  // values and negatives must behave identically to distinct positive ones.
  checkBoth({5, 5, 5, 5, NUL, NUL, 5}, {{5}, {5, 5}, {5, 5}},
            "all values identical -> linking is structural, not value-based");

  checkBoth({-1, -2, -3, NUL, -4, -5, NUL}, {{-1}, {-2, -3}, {-4, -5}},
            "negative values -> [[-1],[-2,-3],[-4,-5]]");

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
