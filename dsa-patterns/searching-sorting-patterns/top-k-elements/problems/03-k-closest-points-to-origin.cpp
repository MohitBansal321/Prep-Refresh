// LeetCode 973 -- K Closest Points to Origin
//
// Problem: given an array of points on the 2D plane and an integer k, return the k
// points closest to the origin (0, 0). Any order in the returned answer is accepted.
//
// Approach (Top K Elements, "smallest K" variant -- the mirror of "largest K"):
// This problem wants the K SMALLEST distances, which is the inverted case from
// findKthLargest: here we use a MAX-heap of size K. Whenever the heap grows past
// size K, pop the point with the LARGEST distance held so far -- it is the weakest
// candidate for "K closest" once K closer candidates already occupy the heap. This
// is the exact mirror of the min-heap-for-largest-K rule, and mixing the two up
// (using a min-heap here) is one of the most common mistakes with this pattern --
// see the README's Common Mistakes section.
//
// We compare SQUARED Euclidean distance (dx*dx + dy*dy), never the square root:
// sqrt is monotonic, so it never changes which point is closer, and skipping it
// avoids floating-point comparisons and cost for no benefit.
//
// Complexity: O(n log k) time (n pushes/pops, each O(log k)), O(k) extra space.
// Contrast with sorting all n points by distance first (O(n log n)) -- strictly
// worse whenever k < n, which is the whole point of using a bounded heap here.

#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

using Point = std::pair<int, int>;  // (x, y)

long long squaredDistance(const Point& p) {
    long long dx = p.first;
    long long dy = p.second;
    return dx * dx + dy * dy;
}

std::vector<Point> kClosest(const std::vector<Point>& points, int k) {
    // Max-heap of (squaredDistance, point), ordered by distance descending so the
    // farthest point currently held is always the cheap-to-evict top element.
    using DistPoint = std::pair<long long, Point>;
    std::priority_queue<DistPoint> maxHeap;  // default comparator: largest distance on top

    for (const Point& p : points) {
        maxHeap.push({squaredDistance(p), p});
        if (static_cast<int>(maxHeap.size()) > k) {
            maxHeap.pop();  // discard the farthest point among the current top-k-closest
        }
    }

    std::vector<Point> result;
    result.reserve(maxHeap.size());
    while (!maxHeap.empty()) {
        result.push_back(maxHeap.top().second);
        maxHeap.pop();
    }
    return result;  // order unspecified by the problem statement
}

namespace {

bool samePointSet(std::vector<Point> a, std::vector<Point> b) {
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

void check(bool condition, const std::string& label) {
    std::cout << (condition ? "[PASS] " : "[FAIL] ") << label << '\n';
}

}  // namespace

int main() {
    check(samePointSet(kClosest({{1, 3}, {-2, 2}}, 1), {{-2, 2}}),
          "kClosest({(1,3),(-2,2)}, 1) == {(-2,2)}");

    check(samePointSet(kClosest({{3, 3}, {5, -1}, {-2, 4}}, 2), {{3, 3}, {-2, 4}}),
          "kClosest({(3,3),(5,-1),(-2,4)}, 2) == {(3,3),(-2,4)}");

    check(samePointSet(kClosest({{0, 0}}, 1), {{0, 0}}),
          "single point at the origin itself");

    check(samePointSet(kClosest({{1, 1}, {1, 1}, {2, 2}}, 2), {{1, 1}, {1, 1}}),
          "duplicate points closest to origin");

    check(kClosest({{1, 1}, {2, 2}, {3, 3}}, 3).size() == 3,
          "k == n returns every point");

    std::cout << "\nAll kClosest checks executed.\n";
    return 0;
}
