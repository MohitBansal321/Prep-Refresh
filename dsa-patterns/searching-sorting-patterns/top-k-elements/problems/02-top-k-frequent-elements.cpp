// LeetCode 347 -- Top K Frequent Elements
//
// Problem: given an integer array `nums` and an integer `k`, return the k values that
// occur most frequently. Any order is acceptable in the returned answer.
//
// Approach (Top K Elements, frequency-keyed variant):
// 1. One O(n) pass builds a hash map from value -> how many times it occurs.
// 2. A size-K MIN-heap, ordered by frequency, scans the distinct (value, frequency)
//    pairs. Whenever the heap exceeds size K, pop the pair with the SMALLEST
//    frequency -- it is the weakest candidate for "top K most frequent" once K
//    stronger candidates already occupy the heap. This is the exact same
//    push-then-evict-if-oversized rule as "K largest," just applied to frequency
//    counts instead of raw values.
//
// Complexity: O(n) for the counting pass + O(d log k) for the heap pass, where d is
// the number of DISTINCT values (d <= n). Worst case (all distinct) this is
// O(n log k) -- still strictly better than sorting all distinct values by frequency,
// which would cost O(d log d).

#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<int> topKFrequent(const std::vector<int>& nums, int k) {
    std::unordered_map<int, int> frequency;
    for (int value : nums) {
        ++frequency[value];
    }

    // Min-heap of (frequency, value) pairs, ordered by frequency ascending so the
    // least-frequent value currently held is always the cheap-to-evict top element.
    using FreqValue = std::pair<int, int>;
    std::priority_queue<FreqValue, std::vector<FreqValue>, std::greater<FreqValue>> minHeap;

    for (const auto& entry : frequency) {
        minHeap.push({entry.second, entry.first});  // (frequency, value)
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();
        }
    }

    std::vector<int> result;
    result.reserve(minHeap.size());
    while (!minHeap.empty()) {
        result.push_back(minHeap.top().second);
        minHeap.pop();
    }
    return result;  // order is unspecified by the problem statement
}

namespace {

bool sameElementsIgnoringOrder(std::vector<int> a, std::vector<int> b) {
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

void check(bool condition, const std::string& label) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << label << '\n';
}

}  // namespace

int main() {
    check(sameElementsIgnoringOrder(topKFrequent({1, 1, 1, 2, 2, 3}, 2), {1, 2}),
          "topKFrequent({1,1,1,2,2,3}, 2) == {1,2} (order-independent)");

    check(sameElementsIgnoringOrder(topKFrequent({1}, 1), {1}),
          "topKFrequent({1}, 1) == {1}");

    check(sameElementsIgnoringOrder(
              topKFrequent({4, 4, 4, 6, 6, 6, 1, 1, 1, 1, 2}, 3), {1, 4, 6}),
          "topKFrequent picks the 3 most frequent values, ties broken arbitrarily");

    check(sameElementsIgnoringOrder(topKFrequent({5, 3, 5, 3, 5, 3}, 2), {3, 5}),
          "topKFrequent with exactly k distinct values returns all of them");

    check(topKFrequent({}, 0).empty(), "topKFrequent on empty input with k == 0");

    std::cout << "\nAll topKFrequent checks executed.\n";
    return 0;
}
