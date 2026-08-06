// Top "K" Elements — generic, problem-agnostic template.
//
// This file is NOT a solution to one specific LeetCode question. It exists to show
// the *shape* of the pattern clearly, separated from any one problem's details, before
// looking at the worked, problem-specific solutions in problems/.
//
// Core idea: maintain a heap of EXACTLY size K while scanning the input once.
//   - For "K largest" -> use a MIN-heap of size K. The smallest element currently in
//     the heap sits on top, so it is O(log k) to find and evict when a bigger candidate
//     shows up. This inversion (min-heap for "largest") is the single most important
//     idea in this whole pattern -- see the README's Solution section for why.
//   - For "K smallest" -> use a MAX-heap of size K, for the symmetric reason: the
//     largest element currently kept sits on top and is the one to evict.
//   - For "K most frequent" -> same min-heap-of-size-K shape, but the ordering key is
//     frequency (computed once via a hash map) instead of the raw value.
//
// std::priority_queue<T> is a MAX-heap by default (biggest on top). Passing the
// comparator std::greater<T> flips it into a MIN-heap (smallest on top). That single
// template argument is the entire mechanism behind "K largest uses a min-heap."

#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

// ---------------------------------------------------------------------------
// topKLargest — the canonical size-K min-heap template.
//
// Push every element; whenever the heap grows past size K, pop the smallest
// (the top of a min-heap), because it is the weakest candidate for "top K largest"
// once K stronger candidates are already held. After one pass over all n elements,
// the heap holds exactly the K largest, in no particular order.
//
// Time:  O(n log k)  -- n pushes/pops, each O(log k) because the heap never exceeds size K.
// Space: O(k)         -- the heap itself; input is read, not copied.
// ---------------------------------------------------------------------------
template <typename T>
std::vector<T> topKLargest(const std::vector<T>& nums, int k) {
    if (k <= 0) return {};

    // Min-heap: std::greater<T> makes the SMALLEST element rise to the top,
    // which is exactly the element we want easy access to for eviction.
    std::priority_queue<T, std::vector<T>, std::greater<T>> minHeap;

    for (const T& value : nums) {
        minHeap.push(value);
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();  // evict the current smallest of the top-K-so-far
        }
    }

    // The heap now holds the K largest elements, but NOT in sorted order --
    // a priority_queue only guarantees the top element, not full ordering.
    // Extracting into a sorted vector costs an extra O(k log k), only pay it
    // if the caller actually needs sorted output.
    std::vector<T> result;
    result.reserve(minHeap.size());
    while (!minHeap.empty()) {
        result.push_back(minHeap.top());
        minHeap.pop();
    }
    std::sort(result.begin(), result.end(), std::greater<T>());  // largest first
    return result;
}

// ---------------------------------------------------------------------------
// topKSmallest — the mirror image: a MAX-heap of size K.
//
// Same eviction rule, opposite heap type: whenever the heap grows past size K,
// pop the LARGEST element held so far (top of a max-heap), because it is the
// weakest candidate for "top K smallest." This is included specifically to make
// the inversion concrete: "largest K" pairs with a min-heap, "smallest K" pairs
// with a max-heap -- always the OPPOSITE of what intuition first suggests.
// ---------------------------------------------------------------------------
template <typename T>
std::vector<T> topKSmallest(const std::vector<T>& nums, int k) {
    if (k <= 0) return {};

    // Default std::priority_queue<T> is already a max-heap (largest on top) --
    // no comparator argument needed here, unlike topKLargest above.
    std::priority_queue<T> maxHeap;

    for (const T& value : nums) {
        maxHeap.push(value);
        if (static_cast<int>(maxHeap.size()) > k) {
            maxHeap.pop();  // evict the current largest of the bottom-K-so-far
        }
    }

    std::vector<T> result;
    result.reserve(maxHeap.size());
    while (!maxHeap.empty()) {
        result.push_back(maxHeap.top());
        maxHeap.pop();
    }
    std::sort(result.begin(), result.end());  // smallest first
    return result;
}

// ---------------------------------------------------------------------------
// topKFrequent — same size-K min-heap shape, keyed by frequency instead of value.
//
// Two passes:
//   1. O(n) hash-map pass to count how many times each distinct value occurs.
//   2. O(d log k) heap pass over the d distinct values (d <= n), keeping only the
//      K most frequent, using a min-heap of (frequency, value) pairs ordered by
//      frequency so the least-frequent of the current top-K is always evictable.
//
// Time:  O(n + d log k), which is O(n log k) in the worst case where d is close to n.
// Space: O(d) for the frequency map plus O(k) for the heap.
// ---------------------------------------------------------------------------
std::vector<int> topKFrequent(const std::vector<int>& nums, int k) {
    if (k <= 0) return {};

    std::unordered_map<int, int> frequency;  // value -> count
    for (int value : nums) {
        ++frequency[value];
    }

    // Min-heap of (frequency, value) pairs. std::pair's default operator<
    // compares .first before .second, so ordering by frequency falls out
    // naturally; std::greater<> flips it to a min-heap on that same ordering.
    using FreqValue = std::pair<int, int>;  // (frequency, value)
    std::priority_queue<FreqValue, std::vector<FreqValue>, std::greater<FreqValue>> minHeap;

    for (const auto& [value, count] : frequency) {
        minHeap.push({count, value});
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();  // evict the least-frequent value seen so far
        }
    }

    std::vector<int> result;
    result.reserve(minHeap.size());
    while (!minHeap.empty()) {
        result.push_back(minHeap.top().second);
        minHeap.pop();
    }
    // Most frequent first -- purely a presentation choice, the heap logic above
    // does not depend on this final ordering.
    std::sort(result.begin(), result.end(), [&frequency](int a, int b) {
        return frequency.at(a) > frequency.at(b);
    });
    return result;
}

// ---------------------------------------------------------------------------
// main() -- exercises all three functions against small, hand-checkable inputs
// and prints [PASS]/[FAIL] for each assertion, proving the template compiles
// and runs correctly end to end.
// ---------------------------------------------------------------------------
namespace {

void check(bool condition, const std::string& label) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << label << '\n';
}

template <typename T>
bool sameElementsIgnoringOrder(std::vector<T> a, std::vector<T> b) {
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

}  // namespace

int main() {
    // --- topKLargest ---
    {
        std::vector<int> nums = {3, 1, 5, 12, 2, 11, 9, 7};
        auto result = topKLargest(nums, 3);
        check(result == std::vector<int>({12, 11, 9}),
              "topKLargest({3,1,5,12,2,11,9,7}, 3) == {12,11,9}");
    }
    {
        std::vector<int> nums = {4, 4, 4, 4};
        auto result = topKLargest(nums, 2);
        check(result == std::vector<int>({4, 4}), "topKLargest with duplicate values");
    }
    {
        std::vector<int> nums = {5};
        auto result = topKLargest(nums, 3);  // k larger than n -- should just return all
        check(result == std::vector<int>({5}), "topKLargest with k > n returns all elements");
    }

    // --- topKSmallest ---
    {
        std::vector<int> nums = {3, 1, 5, 12, 2, 11, 9, 7};
        auto result = topKSmallest(nums, 3);
        check(result == std::vector<int>({1, 2, 3}),
              "topKSmallest({3,1,5,12,2,11,9,7}, 3) == {1,2,3}");
    }
    {
        std::vector<double> nums = {2.5, 0.1, 9.9, -3.2, 4.4};
        auto result = topKSmallest(nums, 2);
        check(result == std::vector<double>({-3.2, 0.1}), "topKSmallest works over double (template check)");
    }

    // --- topKFrequent ---
    {
        std::vector<int> nums = {1, 1, 1, 2, 2, 3};
        auto result = topKFrequent(nums, 2);
        check(sameElementsIgnoringOrder(result, {1, 2}),
              "topKFrequent({1,1,1,2,2,3}, 2) contains {1,2}");
    }
    {
        std::vector<int> nums = {1};
        auto result = topKFrequent(nums, 1);
        check(result == std::vector<int>({1}), "topKFrequent with a single distinct value");
    }

    std::cout << "\nAll topKLargest / topKSmallest / topKFrequent checks executed.\n";
    return 0;
}
