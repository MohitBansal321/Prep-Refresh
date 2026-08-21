// ============================================================================
// LeetCode 127 — Word Ladder
// https://leetcode.com/problems/word-ladder/
// ============================================================================
//
// PROBLEM
// -------
// Given two words `beginWord` and `endWord`, and a dictionary `wordList`,
// return the number of WORDS in the shortest transformation sequence from
// beginWord to endWord, or 0 if no such sequence exists. Rules:
//   - every adjacent pair of words in the sequence differs by exactly one
//     letter,
//   - every word in the sequence except beginWord must be in wordList,
//   - all words have the same length.
//
// Example: beginWord = "hit", endWord = "cog",
//          wordList  = {"hot","dot","dog","lot","log","cog"}
//          -> 5, via hit -> hot -> dot -> dog -> cog (5 words, 4 hops)
//
// RECOGNITION SIGNAL — "shortest," over a graph nobody handed you
// ---------------------------------------------------------------
// Two signals stack up here.
//
// First: **"shortest transformation sequence."** The words "shortest,"
// "minimum," "fewest," and "nearest" are the README's stated trigger for BFS.
// This is not a preference — see the BFS vs DFS note below.
//
// Second, and the reason this is the hardest of the four: **the graph is
// implicit and never materialised.** There is no adjacency list in the input.
// The nodes are words, and there is an edge between two words iff they differ in
// exactly one position. 01-number-of-islands.cpp had an implicit graph too, but
// a grid's neighbours are geometric and obvious. Here you have to *notice* that
// "differs by one letter" is an edge relation at all, then decide how to
// enumerate a word's neighbours without building the whole edge set.
//
// BFS vs DFS — WHICH ONE AND WHY
// -------------------------------
// **BFS is mandatory. DFS is not a slower alternative here — it is wrong.**
//
// BFS explores in strict rings: it fully processes every word reachable in k
// hops before touching anything reachable in k+1. So the FIRST time endWord is
// dequeued, the hop count that reached it is provably minimal — no shorter route
// can exist, because every shorter ring was already exhausted. That guarantee
// comes free from the queue's FIFO order; nothing extra is tracked to earn it.
//
// DFS has no such ordering. It commits to one chain of transformations and
// follows it as far as it goes, so it can reach "cog" after a 9-hop detour and,
// having marked every word on that detour as visited, never discover the 4-hop
// route at all. The failure mode is the nastiest kind: DFS returns a real,
// valid transformation sequence that is simply not the shortest one — no crash,
// no exception, just a number that is too big, and only when the test data
// happens to contain a shorter alternative route.
//
// "Fine, so DFS while tracking the minimum over ALL paths." That fixes
// correctness and destroys the complexity: you can no longer mark a word visited
// permanently (a word on a bad path may be essential to a good one), so you must
// un-mark on backtrack, which turns the traversal into an enumeration of every
// simple path — exponential in the worst case, versus BFS's O(V + E). This is
// the sharpest BFS-vs-DFS distinction in the whole module, and the reason this
// problem is the one to reach for when explaining it.
//
// APPROACH
// --------
//   1. Put wordList into an unordered_set for O(1) membership tests. If endWord
//      is not in it, return 0 immediately — no sequence can end there.
//   2. Erase beginWord from the set if present. The set doubles as the `visited`
//      marker: a word is removed the instant it is discovered, so it can never
//      be enqueued twice.
//   3. BFS from beginWord with distance 1 (the problem counts WORDS, not hops,
//      and beginWord is the first word).
//   4. To enumerate a word's neighbours, do NOT compare it against every word in
//      the dictionary. Instead, for each of the L positions, try all 26 letters
//      and test each candidate for membership. That is L*26 hash lookups per
//      word, independent of dictionary size N — versus N*L character
//      comparisons for the pairwise approach, which is dramatically worse for a
//      large dictionary of short words.
//   5. The moment a generated candidate equals endWord, return distance + 1. It
//      is safe to return here rather than waiting for it to be dequeued,
//      because BFS discovers nodes in non-decreasing distance order.
//
// THE DETAIL PEOPLE GET WRONG
// ----------------------------
// Two things, and both are in this file's comments where they happen.
//   (a) **Off by one on what is being counted.** The answer is the number of
//       words in the sequence, which is hops + 1. Starting the BFS distance at 0
//       (the natural instinct, and what `bfsShortestPath` in ../code.cpp does)
//       yields an answer one too small on every single non-trivial input.
//   (b) **Erasing the candidate from the dictionary at discovery, not at
//       dequeue.** In a word graph a word typically has many one-letter
//       neighbours, so several in-flight words will generate the same candidate.
//       Remove it when it is first generated and it is enqueued once; remove it
//       only when popped and the same word gets enqueued many times, blowing up
//       the queue for no benefit. This is exactly the README's headline Common
//       Mistake in its natural habitat.
//
// COMPLEXITY
// ----------
// Let N = wordList size, L = word length.
// Time:  O(N * L * 26 * L) = O(N * L^2) -- each of the up-to-N words is
//        dequeued once; for each we build L*26 candidate strings, and building
//        and hashing an L-character string is O(L).
// Space: O(N * L) -- the dictionary set and the queue both hold up to N words
//        of length L.
// ============================================================================

#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

int ladderLength(const std::string& beginWord,
                 const std::string& endWord,
                 const std::vector<std::string>& wordList) {
  // The dictionary. It serves two purposes at once: "is this candidate a real
  // word?" and "has this word already been visited?" — because a word is
  // ERASED the moment it is discovered.
  std::unordered_set<std::string> unused(wordList.begin(), wordList.end());

  // Early exit: the rules require endWord to be in wordList, so if it is not,
  // no sequence can possibly end there. Skipping this check does not produce a
  // wrong answer (BFS would simply never generate it and return 0 anyway) but
  // it saves a full traversal of the reachable component.
  if (unused.find(endWord) == unused.end()) return 0;

  // beginWord is allowed to be absent from wordList, but if it IS present it
  // must be removed, or the BFS could rediscover and re-enqueue its own start.
  unused.erase(beginWord);

  std::queue<std::string> frontier;
  frontier.push(beginWord);

  // DISTANCE STARTS AT 1, NOT 0. The problem counts the number of WORDS in the
  // sequence, and beginWord is the first of them. `bfsShortestPath` in
  // ../code.cpp counts HOPS (edges) and correctly starts at 0; the conversion
  // is words = hops + 1, and forgetting it is the single most common wrong
  // answer on this problem.
  int wordsInSequence = 1;

  while (!frontier.empty()) {
    // Process the CURRENT RING in full before moving to the next one. The
    // level-size snapshot is what lets a single counter stand in for a whole
    // distance map: every word popped inside this inner loop is at exactly
    // `wordsInSequence` words from the start.
    int ringSize = static_cast<int>(frontier.size());

    for (int i = 0; i < ringSize; ++i) {
      std::string word = frontier.front();
      frontier.pop();

      // Enumerate neighbours by mutation, not by comparison. For each
      // position, swap in each of the 26 lowercase letters and ask the set
      // whether the result is a still-unused dictionary word.
      for (size_t pos = 0; pos < word.size(); ++pos) {
        char original = word[pos];

        for (char letter = 'a'; letter <= 'z'; ++letter) {
          if (letter == original) continue;  // Zero-change is not an edge.

          word[pos] = letter;

          std::unordered_set<std::string>::iterator found = unused.find(word);
          if (found == unused.end()) continue;  // Not a word, or already visited.

          if (word == endWord) {
            // BFS discovers nodes in non-decreasing distance order, so the
            // first time endWord is generated it is via a shortest route. No
            // need to enqueue it and wait for the next ring — just add the
            // one final word and return.
            return wordsInSequence + 1;
          }

          // MARK ON DISCOVERY: erase before enqueueing. Many words in this
          // same ring will generate this identical candidate; erasing now
          // means only the first one enqueues it.
          unused.erase(found);
          frontier.push(word);
        }

        word[pos] = original;  // Restore before moving to the next position.
      }
    }

    ++wordsInSequence;  // The whole ring is processed; everything discovered
                        // during it sits one word further out.
  }

  return 0;  // Queue drained without ever reaching endWord: unreachable.
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
    std::vector<std::string> words;
    words.push_back("hot");
    words.push_back("dot");
    words.push_back("dog");
    words.push_back("lot");
    words.push_back("log");
    words.push_back("cog");
    check(ladderLength("hit", "cog", words) == 5,
          "LeetCode example 1: hit->hot->dot->dog->cog -> 5 words");
  }

  {
    // Same dictionary minus "cog": endWord is not in the list at all.
    std::vector<std::string> words;
    words.push_back("hot");
    words.push_back("dot");
    words.push_back("dog");
    words.push_back("lot");
    words.push_back("log");
    check(ladderLength("hit", "cog", words) == 0,
          "LeetCode example 2: endWord absent from wordList -> 0");
  }

  {
    // Empty dictionary: the early endWord check returns before any traversal.
    std::vector<std::string> words;
    check(ladderLength("hit", "cog", words) == 0, "empty wordList -> 0");
  }

  {
    // One hop: begin and end differ by a single letter, and endWord is present.
    // 2 words, not 1 -- the off-by-one guard.
    std::vector<std::string> words;
    words.push_back("cog");
    check(ladderLength("cot", "cog", words) == 2, "single hop -> 2 words (not 1)");
  }

  {
    // Single-character words: L == 1, so every dictionary word is adjacent to
    // every other. Exercises the smallest possible mutation loop.
    std::vector<std::string> words;
    words.push_back("a");
    words.push_back("b");
    words.push_back("c");
    check(ladderLength("a", "c", words) == 2, "single-letter words a->c -> 2");
  }

  {
    // A dead end plus the real route. "hot"->"hut" is a valid transformation
    // that leads nowhere; BFS must not be derailed by it.
    std::vector<std::string> words;
    words.push_back("hot");
    words.push_back("hut");
    words.push_back("hog");
    words.push_back("cog");
    check(ladderLength("hot", "cog", words) == 3, "dead-end branch ignored: hot->hog->cog -> 3");
  }

  {
    // THE CASE THAT BREAKS DFS. Two genuine routes from "aaa" to "abb":
    //   short: aaa -> aab -> abb                      (3 words)
    //   long:  aaa -> baa -> bab -> bbb -> abb        (5 words)
    // Both are legal transformation sequences. A DFS that happened to expand
    // "baa" before "aab" would walk the long route to the end, mark every word
    // on it visited, and report 5 -- a correct-looking answer that is simply not
    // the minimum. BFS exhausts the entire 2-word ring (baa, aab) before any
    // 3-word candidate, so it discovers "abb" via "aab" and cannot miss it.
    std::vector<std::string> words;
    words.push_back("aab");  // short route, step 1
    words.push_back("abb");  // the target
    words.push_back("baa");  // long route, step 1
    words.push_back("bab");
    words.push_back("bbb");
    check(ladderLength("aaa", "abb", words) == 3,
          "shortest route wins over a longer valid one -> 3 (the DFS trap)");
  }

  {
    // A bridging word makes an otherwise-unreachable target reachable: "cot"
    // is the only word joining the "h_t" group to the "co_" group.
    std::vector<std::string> words;
    words.push_back("hot");
    words.push_back("cot");
    words.push_back("cog");
    check(ladderLength("hit", "cog", words) == 4, "hit->hot->cot->cog -> 4 words");
  }

  {
    // Genuinely unreachable: endWord is present but in a disconnected part of
    // the word graph (nothing is one letter away from anything in the other
    // group).
    std::vector<std::string> words;
    words.push_back("hot");
    words.push_back("dog");
    check(ladderLength("hit", "dog", words) == 0,
          "endWord present but unreachable (no bridging word) -> 0");
  }

  {
    // beginWord itself appears in wordList. It must be erased up front, or it
    // can be rediscovered and re-enqueued.
    std::vector<std::string> words;
    words.push_back("hit");
    words.push_back("hot");
    words.push_back("hog");
    check(ladderLength("hit", "hog", words) == 3, "beginWord present in wordList -> still 3");
  }

  {
    // Duplicate entries in wordList. The unordered_set collapses them, so the
    // answer is unaffected -- worth asserting, since a vector-based visited
    // scheme could double-count.
    std::vector<std::string> words;
    words.push_back("hot");
    words.push_back("hot");
    words.push_back("hog");
    words.push_back("hog");
    check(ladderLength("hit", "hog", words) == 3, "duplicate words in wordList -> still 3");
  }

  {
    // A straight chain of 5 dictionary words: the longest ladder here, and a
    // check that the ring counter increments exactly once per level.
    std::vector<std::string> words;
    words.push_back("aab");
    words.push_back("abb");
    words.push_back("bbb");
    words.push_back("bbc");
    words.push_back("bcc");
    check(ladderLength("aaa", "bcc", words) == 6, "6-word chain aaa->aab->abb->bbb->bbc->bcc -> 6");
  }

  {
    // endWord == beginWord and it is in the list. LeetCode guarantees they
    // differ, but the code should not hang or misbehave: the early check finds
    // endWord present, beginWord is erased (so endWord is gone too), and the
    // BFS finds nothing -> 0. Documented, not a hidden crash.
    std::vector<std::string> words;
    words.push_back("hit");
    check(ladderLength("hit", "hit", words) == 0,
          "beginWord == endWord (out of spec) -> 0, no hang");
  }

  std::cout << "\n" << pass_count << " passed, " << fail_count << " failed.\n";
  return fail_count == 0 ? 0 : 1;
}
