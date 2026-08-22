// ============================================================================
// LeetCode 421 — Maximum XOR of Two Numbers in an Array
// ============================================================================
//
// PROBLEM
// -------
// Given an integer array `nums`, return the maximum result of nums[i] XOR
// nums[j], where 0 <= i <= j < n. Values fit in 32 bits (non-negative).
//
// Example: nums = [3, 10, 5, 25, 2, 8] -> 28   (5 XOR 25 = 28)
//
// APPROACH — bitwise Trie + greedy descent (the alphabet is {0,1})
// -------------------------------------------------------------
// Brute force checks all pairs: O(n^2). The trie view reframes the problem:
// a number's binary representation IS a string over the alphabet {0,1}, and
// "which numbers share/contrast this bit prefix" is exactly a prefix query.
//
// 1. Insert every number into a trie as its 31-bit binary string, one node
//    per bit, two child slots per node. All numbers share depth 31, so the
//    tree's height is fixed and every path is a real stored number.
//
// 2. For each number x, walk the trie AGAIN asking: at every bit position,
//    which child maximizes the final XOR? XOR is 1 exactly when bits
//    differ, so the greedy rule is: prefer the child holding the OPPOSITE
//    of x's current bit; if that branch doesn't exist, settle for the same
//    bit (XOR contributes 0 there) and continue.
//
// Why greedy is safe: bits are positional powers of two, and a higher bit
// outweighs ALL lower bits combined (2^b > 2^(b-1) + ... + 2^0). So making
// the optimal choice at bit b is always at least as good as any sacrifice
// at bit b could ever be repaid by lower bits — the classic exchange
// argument for lexicographic-style optimization over fixed-length strings.
//
// Doing this once per element yields max(x XOR y) over all pairs, because
// for the true optimal pair (a, b), when we process `a` the walk finds `b`
// and collects the maximal value.
//
// COMPLEXITY
// ----------
// Time:  O(n · B) where B = 31 bits — one insert pass plus one query pass,
//        each O(B) per number. Compare with brute force O(n^2 · 1).
// Space: O(n · B) trie nodes worst case; shared bit prefixes compress this
//        in practice, exactly as with letter tries.
// ============================================================================

#include <iostream>
#include <vector>

class BitTrie {
 public:
  BitTrie() : root_(new Node()) {}

  ~BitTrie() { destroy(root_); }

  // Insert the 31-bit representation of x, most significant bit first.
  void insert(unsigned x) {
    Node* cur = root_;
    for (int b = 30; b >= 0; --b) {
      int bit = static_cast<int>((x >> b) & 1u);
      if (!cur->child[bit]) cur->child[bit] = new Node();
      cur = cur->child[bit];
    }
  }

  // Best possible value of x XOR y over all stored y: descend greedily
  // toward the opposite bit whenever that branch exists.
  unsigned maxXorWith(unsigned x) const {
    const Node* cur = root_;
    unsigned best = 0;
    for (int b = 30; b >= 0; --b) {
      int bit = static_cast<int>((x >> b) & 1u);
      int want = 1 - bit;  // opposite bit makes this XOR position a 1

      if (cur->child[want]) {
        best |= (1u << b);       // this bit of the result is set
        cur = cur->child[want];
      } else {
        cur = cur->child[bit];   // forced to match: contributes 0 here
      }
    }
    return best;
  }

 private:
  struct Node {
    Node* child[2] = {nullptr, nullptr};  // slot 0 = bit 0, slot 1 = bit 1
  };

  Node* root_;

  void destroy(Node* node) {
    if (!node) return;
    destroy(node->child[0]);
    destroy(node->child[1]);
    delete node;
  }
};

unsigned findMaximumXOR(const std::vector<int>& nums) {
  BitTrie trie;

  // Pass 1: insert every number's bit-string.
  for (int n : nums) trie.insert(static_cast<unsigned>(n));

  // Pass 2: for each number, find the stored number maximizing XOR with it.
  unsigned best = 0;
  for (int n : nums) {
    unsigned candidate = trie.maxXorWith(static_cast<unsigned>(n));
    if (candidate > best) best = candidate;
  }
  return best;
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
    // Canonical LeetCode example: 5 ^ 25 = 28.
    std::vector<int> nums = {3, 10, 5, 25, 2, 8};
    check(findMaximumXOR(nums) == 28u, "[3,10,5,25,2,8] -> 28");
  }

  {
    std::vector<int> nums = {14, 70, 53, 83, 49, 91, 36, 80, 92, 51, 66, 70};
    check(findMaximumXOR(nums) == 127u, "larger mixed array -> 127");
  }

  {
    // Edge: single element — i == j means x XOR x = 0.
    std::vector<int> nums = {0};
    check(findMaximumXOR(nums) == 0u, "[0] -> 0");
  }

  {
    // Edge: single nonzero element still XORs with itself -> 0.
    std::vector<int> nums = {7};
    check(findMaximumXOR(nums) == 0u, "[7] -> 0 (self-XOR)");
  }

  {
    // Edge: pair of two elements, direct check: 2 ^ 4 = 6.
    std::vector<int> nums = {2, 4};
    check(findMaximumXOR(nums) == 6u, "[2,4] -> 6");
  }

  {
    // Edge: all zeros — every XOR is 0.
    std::vector<int> nums = {0, 0, 0};
    check(findMaximumXOR(nums) == 0u, "[0,0,0] -> 0");
  }

  {
    // Edge: duplicates are fine; the answer may come from equal values (0).
    std::vector<int> nums = {5, 5};
    check(findMaximumXOR(nums) == 0u, "[5,5] -> 0");
  }

  {
    // Edge: high-bit dominance — 2^30 beats any combination of small values.
    std::vector<int> nums = {1, 2, 3, 1073741824};  // 2^30
    unsigned expected = 1073741824u ^ 3u;           // 2^30 paired with 3
    check(findMaximumXOR(nums) == expected, "high bit dominates -> 2^30 ^ 3");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
