# Palindromic Subsequence / Substring — Worked Problems

Four fully worked, heavily commented, standalone C++17 solutions demonstrating this module's techniques across its main flavors: interval DP for subsequences, expand-around-center for contiguous substrings (longest and counted), and the `n − LPS(s)` reduction for minimum insertions. Each file is self-contained: compile and run it directly to see printed PASS/FAIL output proving correctness against known expected answers.

```bash
g++ -std=c++17 -Wall problems/01-longest-palindromic-subsequence.cpp -o /tmp/out && /tmp/out
```

| Name | LeetCode # | Difficulty | One-line approach | Time / Space | File |
|------|-----------|------------|--------------------|---------------|------|
| Longest Palindromic Subsequence | [516](https://leetcode.com/problems/longest-palindromic-subsequence/) | Medium | Interval DP: match endpoints and add 2 to the inner interval, else drop one endpoint; fill by increasing interval length. | O(n²) time, O(n²) space | [01-longest-palindromic-subsequence.cpp](01-longest-palindromic-subsequence.cpp) |
| Longest Palindromic Substring | [5](https://leetcode.com/problems/longest-palindromic-substring/) | Medium | Expand around all 2n−1 centers (chars + gaps); track the longest window found. | O(n²) time, O(1) space | [02-longest-palindromic-substring.cpp](02-longest-palindromic-substring.cpp) |
| Palindromic Substrings | [647](https://leetcode.com/problems/palindromic-substrings/) | Medium | Same center expansion as LC 5, but count every successful expansion step instead of tracking the longest. | O(n²) time, O(1) space | [03-palindromic-substrings.cpp](03-palindromic-substrings.cpp) |
| Minimum Insertion Steps to Make a String Palindrome | [1312](https://leetcode.com/problems/minimum-insertion-steps-to-make-a-string-palindrome/) | Hard | Reduce to LPS first: answer = n − LPS(s), then run the interval DP from problem 01. | O(n²) time, O(n²) space | [04-minimum-insertion-steps-to-make-string-palindrome.cpp](04-minimum-insertion-steps-to-make-string-palindrome.cpp) |

## Why these four

They cover every technique called out in the [README](../README.md):
- **01** is the canonical interval DP — the pattern in its purest form, including the below-diagonal subtlety when adjacent characters match.
- **02** is the canonical expand-around-center — the pattern's other main flavor, with no table at all.
- **03** shows how a one-line change to 02's expansion (count every step vs track the longest) answers a differently-phrased question — same centers, different bookkeeping.
- **04** shows the most valuable reduction in the module: a "min operations" phrasing that collapses to `n − LPS(s)`, reusing problem 01's DP unchanged.

For unguided practice on problems that are *not* worked out step by step, see [exercises.md](../exercises.md).
