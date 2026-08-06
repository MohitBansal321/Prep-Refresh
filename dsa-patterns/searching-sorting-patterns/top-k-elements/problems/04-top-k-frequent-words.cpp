// LeetCode 692 -- Top K Frequent Words
//
// Problem: given an array of strings `words` and an integer k, return the k most
// frequent strings, sorted by frequency (highest first). If two words have the SAME
// frequency, the lexicographically SMALLER word must come first -- a tie-break rule
// that classic "top K frequent elements" (LeetCode 347) does not require, which is
// exactly why this problem needs a custom heap comparator instead of the plain
// (frequency, value) pair ordering used in problems/02-top-k-frequent-elements.cpp.
//
// Approach (Top K Elements, frequency-keyed variant with custom tie-breaking):
// 1. One O(n) pass builds a hash map from word -> occurrence count.
// 2. A size-K MIN-heap scans the distinct words. The heap's ordering defines
//    "better" as: higher frequency, or -- on a frequency tie -- lexicographically
//    smaller. The comparator is written so the WORST candidate (lowest frequency,
//    or the lexicographically LARGER word on a tie) is always the one sitting on
//    top and evicted first when the heap exceeds size K. This is the same
//    push-then-evict-if-oversized rule as every other problem in this module --
//    only the definition of "worse" changes.
// 3. After the scan, popping the heap repeatedly yields words from worst to best;
//    reversing that gives the required best-to-worst (highest frequency first,
//    ties broken lexicographically ascending) output order.
//
// Complexity: O(n) for the counting pass + O(d log k) for the heap pass, where d is
// the number of distinct words (d <= n). Worst case (all distinct) this is
// O(n log k), still better than sorting all d distinct words with a custom
// comparator, which costs O(d log d).

#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<std::string> topKFrequentWords(const std::vector<std::string>& words, int k) {
    std::unordered_map<std::string, int> frequency;
    for (const std::string& word : words) {
        ++frequency[word];
    }

    using WordCount = std::pair<std::string, int>;  // (word, frequency)

    // Comparator returns true when `a` is BETTER than `b` (higher frequency, or a
    // lexicographically smaller word on a tie). Because std::priority_queue keeps
    // the element the comparator treats as "greatest" on top, and this comparator
    // treats "better" as "less than", the WORST word ends up on top -- exactly the
    // one we want cheap to evict.
    auto better = [](const WordCount& a, const WordCount& b) {
        if (a.second != b.second) {
            return a.second > b.second;  // a is better if strictly more frequent
        }
        return a.first < b.first;  // tie on frequency: lexicographically smaller wins
    };

    std::priority_queue<WordCount, std::vector<WordCount>, decltype(better)> minHeap(better);

    for (const auto& entry : frequency) {
        minHeap.push(entry);
        if (static_cast<int>(minHeap.size()) > k) {
            minHeap.pop();  // evict the current worst (least frequent / lexicographically largest tie)
        }
    }

    // Popping yields worst-to-best order; reverse to get the required
    // best-first (highest frequency, then lexicographically smallest) order.
    std::vector<std::string> result;
    result.reserve(minHeap.size());
    while (!minHeap.empty()) {
        result.push_back(minHeap.top().first);
        minHeap.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}

namespace {
void check(const std::vector<std::string>& actual, const std::vector<std::string>& expected,
           const std::string& label) {
    bool pass = (actual == expected);
    std::cout << (pass ? "[PASS] " : "[FAIL] ") << label << '\n';
    if (!pass) {
        std::cout << "        got: ";
        for (const auto& w : actual) std::cout << w << ' ';
        std::cout << "\n        expected: ";
        for (const auto& w : expected) std::cout << w << ' ';
        std::cout << '\n';
    }
}
}  // namespace

int main() {
    check(topKFrequentWords({"i", "love", "leetcode", "i", "love", "coding"}, 2),
          {"i", "love"}, "example 1: {\"i\",\"love\",...}, k=2 -> {\"i\",\"love\"}");

    check(topKFrequentWords(
              {"the", "day", "is", "sunny", "the", "the", "sunny", "is", "is", "the"}, 4),
          {"the", "is", "sunny", "day"},
          "example 2: the=4, is=3, sunny=2, day=1 -> {\"the\",\"is\",\"sunny\",\"day\"} (no tie at the top)");

    check(topKFrequentWords({"a", "aa", "aaa"}, 1), {"a"},
          "all-distinct words, k=1 picks the lexicographically smallest (all freq 1)");

    check(topKFrequentWords({"apple"}, 1), {"apple"}, "single word, k=1");

    check(topKFrequentWords({"b", "a", "b", "a", "c"}, 3), {"a", "b", "c"},
          "three-way tie in frequency, lexicographic order enforced");

    std::cout << "\nAll topKFrequentWords checks executed.\n";
    return 0;
}
