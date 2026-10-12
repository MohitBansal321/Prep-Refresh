// ============================================================================
// Subsets — generic reusable template (C++17)
// ============================================================================
//
// Demonstrates the two equivalent ways to enumerate every subset of a set:
//   1. subsetsIterative — BFS-style doubling: for each new element, copy
//      every subset built so far and append the element to each copy.
//   2. subsetsRecursive — DFS-style include/exclude recursion.
//   3. permutations — a related enumeration (every ordering, not every
//      subset), included for completeness since it uses the same
//      choose/recurse/undo skeleton this whole family shares.
//
// Compile:
//   g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out
// ============================================================================

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;


// ----------------------------------------------------------------------------
// subsetsIterative
//
// Start with just the empty subset. For each new element, duplicate every
// subset built so far and append the element to each duplicate, then add
// those duplicates to the result set. After processing all n elements, the
// result holds all 2^n subsets.
// ----------------------------------------------------------------------------
vector<vector<int>> subsetsIterative(const vector<int>& nums) {
  vector<vector<int>> result = {{}};

  for (int num : nums) {
    size_t existing_count = result.size();
    for (size_t i = 0; i < existing_count; ++i) {
      vector<int> extended = result[i];
      extended.push_back(num);
      result.push_back(move(extended));
    }
  }

  return result;
}

// ----------------------------------------------------------------------------
// subsetsRecursive
//
// At each index, branch into two recursive calls: one that includes
// nums[index] in the current subset, one that excludes it. When index
// reaches nums.size(), the current subset is complete and recorded.
// ----------------------------------------------------------------------------
void subsetsRecursiveHelper(const vector<int>& nums, size_t index,
                             vector<int>& current,
                             vector<vector<int>>& result) {
  if (index == nums.size()) {
    result.push_back(current);
    return;
  }

  // Exclude nums[index].
  subsetsRecursiveHelper(nums, index + 1, current, result);

  // Include nums[index], recurse, then undo before returning (backtrack).
  current.push_back(nums[index]);
  subsetsRecursiveHelper(nums, index + 1, current, result);
  current.pop_back();
}

vector<vector<int>> subsetsRecursive(const vector<int>& nums) {
  vector<vector<int>> result;
  vector<int> current;
  subsetsRecursiveHelper(nums, 0, current, result);
  return result;
}

// ----------------------------------------------------------------------------
// permutations
//
// A related enumeration: every ORDERING of all n elements (n! total),
// rather than every SUBSET (2^n total). Uses a "used" marker per element
// instead of an include/exclude branch, since every element must appear
// exactly once in every permutation.
// ----------------------------------------------------------------------------
void permutationsHelper(const vector<int>& nums, vector<bool>& used,
                         vector<int>& current,
                         vector<vector<int>>& result) {
  if (current.size() == nums.size()) {
    result.push_back(current);
    return;
  }

  for (size_t i = 0; i < nums.size(); ++i) {
    if (used[i]) continue;
    used[i] = true;
    current.push_back(nums[i]);
    permutationsHelper(nums, used, current, result);
    current.pop_back();
    used[i] = false;
  }
}

vector<vector<int>> permutations(const vector<int>& nums) {
  vector<vector<int>> result;
  vector<int> current;
  vector<bool> used(nums.size(), false);
  permutationsHelper(nums, used, current, result);
  return result;
}

// ============================================================================
// main() — demonstrates all three functions with printed, verifiable output.
// ============================================================================
int main() {
  int pass_count = 0;
  int fail_count = 0;

  auto check = [&](bool condition, const string& label) {
    if (condition) {
      cout << "[PASS] " << label << "\n";
      ++pass_count;
    } else {
      cout << "[FAIL] " << label << "\n";
      ++fail_count;
    }
  };

  auto sortedCopy = [](vector<vector<int>> v) {
    for (auto& inner : v) sort(inner.begin(), inner.end());
    sort(v.begin(), v.end());
    return v;
  };

  vector<int> nums = {1, 2, 3};

  auto iterative = subsetsIterative(nums);
  auto recursive = subsetsRecursive(nums);

  check(iterative.size() == 8, "subsetsIterative({1,2,3}) produces 2^3 = 8 subsets");
  check(recursive.size() == 8, "subsetsRecursive({1,2,3}) produces 2^3 = 8 subsets");
  check(sortedCopy(iterative) == sortedCopy(recursive),
        "iterative and recursive approaches agree on the same set of subsets");

  auto empty_result = subsetsIterative({});
  check(empty_result.size() == 1 && empty_result[0].empty(),
        "empty input -> exactly one subset, the empty set itself");

  auto perms = permutations({1, 2, 3});
  check(perms.size() == 6, "permutations({1,2,3}) produces 3! = 6 orderings");

  bool all_have_three_elements = true;
  for (const auto& p : perms) {
    if (p.size() != 3) all_have_three_elements = false;
  }
  check(all_have_three_elements, "every permutation uses all 3 elements exactly once");

  cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
