// LeetCode 215 -- Kth Largest Element in an Array
//
// Problem: given an integer array `nums` and an integer `k`, return the k-th LARGEST
// element in the array -- the element that would sit at index (n - k) if the array
// were sorted ascending, NOT the k-th distinct value.
//
// Approach (Top K Elements, converging case: K == 1 slot we actually report):
// Maintain a MIN-heap of size K while scanning the array once. Whenever the heap
// grows past size K, pop the smallest element held so far -- it cannot be among the
// K largest once K stronger candidates already occupy the heap. After the scan, the
// top of the min-heap (its smallest element) IS the k-th largest overall, because
// the heap holds exactly the K largest values and the smallest of those K values is,
// by definition, the k-th largest in the whole array.
//
// This is the purest possible use of the "min-heap of size K for K largest" idea from
// the README's Solution section -- there is no extra bookkeeping beyond push/evict.
//
// Complexity: O(n log k) time (n pushes/pops, each O(log k) since the heap never
// exceeds size k), O(k) extra space. Contrast with sorting the whole array first,
// which is O(n log n) time -- strictly worse whenever k < n.

#include <iostream>
#include <queue>
#include <string>
#include <vector>

int findKthLargest(const std::vector<int>& nums, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

    for (int value : nums) {
        minHeap.push(value);
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();  // discard the weakest candidate for "top k largest so far"
        }
    }

    // After scanning everything, the heap holds exactly the k largest values.
    // Its top (the smallest of those k) is, by definition, the k-th largest overall.
    return minHeap.top();
}

namespace {
void check(int actual, int expected, const std::string& label) {
    std::cout << (actual == expected ? "[PASS] " : "[FAIL] ") << label
              << " (got " << actual << ", expected " << expected << ")\n";
}
}  // namespace

int main() {
    check(findKthLargest({3, 2, 1, 5, 6, 4}, 2), 5,
          "findKthLargest({3,2,1,5,6,4}, 2) == 5");

    check(findKthLargest({3, 2, 3, 1, 2, 4, 5, 5, 6}, 4), 4,
          "findKthLargest({3,2,3,1,2,4,5,5,6}, 4) == 4 (duplicates present)");

    check(findKthLargest({1}, 1), 1, "single-element array, k == 1");

    check(findKthLargest({7, 7, 7, 7}, 3), 7, "all-duplicate array, k == 3");

    check(findKthLargest({-1, -5, -3, -2, -4}, 1), -1,
          "all-negative array, k == 1 returns the largest (least negative)");

    std::cout << "\nAll findKthLargest checks executed.\n";
    return 0;
}
