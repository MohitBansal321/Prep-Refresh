# Subsets — Exercises

Work through these in order. The goal is to build three reflexes: (1) deciding whether a problem wants the subsets **themselves** (this pattern) or only a fact **about** them (Dynamic Programming); (2) writing the include/exclude or choose/recurse/undo skeleton without losing a subset or emitting one twice; and (3) recognizing the exact moment a constraint becomes *prunable*, which is the moment you have left Subsets and entered [Backtracking](../backtracking/).

> Rule of thumb for every exercise: before writing a single line, answer three questions out loud. **Does the order of elements within one answer matter?** (yes → permutation shape with a `used[]` marker; no → subset shape with include/exclude or a start index.) **Can the input contain duplicate values?** (yes → sort first and skip same-level duplicates.) **Can a *partial* answer already be judged invalid?** (yes → you need a prune, and you are one step past this module.) If you cannot answer all three, you are not ready to write the recursion yet.

---

## Easy — Sum of All Subset XOR Totals

**LeetCode 1863 — Sum of All Subset XOR Totals.**

Given an array `nums`, the XOR total of a subset is the bitwise XOR of all its elements (the XOR total of the empty subset is `0`). Return the sum of the XOR totals of **all** `2^n` subsets. Note that subsets are counted by position, so duplicate values in `nums` produce duplicate subsets that all still count.

**Task:** solve it by adapting `subsetsRecursive` in [code.cpp](code.cpp) — but do **not** build and store the subsets. Replace the shared `std::vector<int> current` buffer with a single running `int xorSoFar`, carry it down the include branch as `xorSoFar ^ nums[index]`, and at the base case (`index == nums.size()`) add `xorSoFar` to a running total instead of pushing a copy into `result`. Note what happens to the "undo" step: XOR is its own inverse, so the backtrack is either `xorSoFar ^= nums[index]` again or — cleaner — simply passing the new value by value into the recursive call and never mutating a shared variable at all.

**Think about:** this problem asks for a *number*, not a list, yet full enumeration is still the intended solution — why is that not a contradiction with the [README](README.md)'s *When NOT To Use* advice about "you only need a fact about the subsets"? (Hint: think about what makes the subset-sum DP reformulation possible, and whether "sum over all subsets of an XOR" has that structure.) **Then answer:** the output space here is `O(1)` while the time is still `O(2^n)` — restate the module's complexity claim in a form that survives that observation. Finally, work out by hand for `nums = [5,1,6]` how many times each *bit position* is set across all 8 subsets, and see whether you can spot the closed-form `O(n)` answer hiding behind the enumeration.

---

## Medium — Combinations

**LeetCode 77 — Combinations.**

Given two integers `n` and `k`, return **all** possible combinations of `k` numbers chosen from the range `[1, n]`. Order within a combination does not matter, and each number may be used at most once. For `n = 4, k = 2` the answer is `[[1,2],[1,3],[1,4],[2,3],[2,4],[3,4]]` — 6 combinations, not 16.

**Task:** start from the `start`-index loop shape shown in [problems/04-combination-sum.cpp](problems/04-combination-sum.cpp) rather than the two-branch include/exclude shape, because here you want to iterate over "which number comes next" and the `start` index is what prevents `[2,1]` from appearing alongside `[1,2]`. Record into `result` only when `current.size() == k` — not at every node, unlike [problems/02-subsets-ii.cpp](problems/02-subsets-ii.cpp), which records at every node because every node there is a valid subset. Keep the push/recurse/pop bracket exactly as it is in `04`.

**Think about:** this is the first exercise where a *partial* answer can be provably doomed. If `k = 4`, `n = 10`, and you are standing on `current = [8]`, only `9` and `10` remain — the branch cannot possibly reach size 4, so exploring it is pure waste. Add that check: skip or return early when `remaining numbers < k - current.size()`. **Then answer:** having added that check, is this still the Subsets pattern or is it Backtracking? Justify your answer against the third diamond in [images/recognition-diagram.md](images/recognition-diagram.md), and say precisely what the pruning bought you (how many nodes did it remove, roughly, for `n = 20, k = 3`?). This is the cleanest illustration in the whole module of how thin the line between the two patterns actually is.

---

## Hard — Maximum Number of Achievable Transfer Requests

**LeetCode 1601 — Maximum Number of Achievable Transfer Requests.**

There are `n` buildings and a list of `requests`, where `requests[i] = [from_i, to_i]` means an employee wants to move from building `from_i` to building `to_i`. A set of requests is **achievable** only if, after all of them happen, every building has the same net number of employees it started with (every building's incoming count equals its outgoing count). Return the maximum number of requests that can be granted simultaneously.

**Task:** this is the Subsets pattern applied to a *selection* problem, and the whole point is that `requests.size() <= 16`, so `2^16 = 65,536` subsets is trivially enumerable. Enumerate every subset of `requests` — use the bitmask form from [problems/01-subsets.cpp](problems/01-subsets.cpp), since a `mask` over at most 16 requests is far cleaner here than a recursion, and the bit tests double as "is request `i` granted." For each subset, build a `std::vector<int> degree(n, 0)`, apply `--degree[from]` and `++degree[to]` for every granted request, and accept the subset only if every entry is `0`. Track the largest accepted `popcount(mask)`.

**Then answer:** the check "every building's net change is zero" cannot reject a *partial* selection — adding more requests to an unbalanced selection can bring it back into balance, so there is no valid prune on balance alone. Explain why that fact places this problem squarely in Subsets rather than Backtracking, and contrast it with `04`'s target-sum prune, where a partial sum that has already exceeded the target can *never* come back down. This contrast — a constraint that is monotone versus one that is not — is the sharpest tool you have for telling the two patterns apart, and it is worth writing down in your own words. **Also answer:** what is the total complexity here, and which of the two factors (`2^r` subsets versus `O(r + n)` work per subset) dominates when `r = 16` and `n = 20`?

---

## Real-World Challenge — Feature-Flag Release Matrix

You own a NestJS service with a small set of independent boolean feature flags stored in Postgres and cached in Redis (say `newCheckout`, `asyncInvoices`, `bulkExport`, `strictValidation`, `v2Pricing` — five flags today, and product keeps adding more). Right now CI runs the integration suite against one hand-picked flag configuration, and last release a bug shipped that only appeared when `asyncInvoices` and `strictValidation` were **both** on. You are building a `flag-matrix` tool to stop that from happening again.

**Task:**

1. Implement `enumerateConfigurations(flagNames)` returning every on/off combination as a list of `{flagName -> bool}` maps. Adapt `subsetsIterative` from [code.cpp](code.cpp): a configuration is exactly "the subset of flags that are ON," so the power set of the flag list *is* the matrix. Confirm your output length is `2^flags` and that the all-off and all-on configurations are both present — the empty subset is the current production default and the single easiest one to accidentally drop (see the [README](README.md)'s *Common Mistakes*).
2. Add a hard guard: if `flagNames.size() > 12`, refuse and throw rather than enumerating. Write down, in a comment, the number of CI runs `2^13` would imply and how long that would take at 90 seconds per run. This guard is the operational form of the *Disadvantages* section — the failure mode is not a slow function, it is a CI pipeline that never finishes.
3. Product tells you two flags are mutually exclusive (`v2Pricing` requires `newCheckout`; `bulkExport` cannot run with `asyncInvoices`). Implement this **two ways**: (a) enumerate all `2^n` configurations and filter the invalid ones out afterward, and (b) prune during generation, abandoning a branch the moment the partial configuration already violates a rule. Measure how many configurations each approach constructs for 10 flags with those 2 rules.
4. Cache the generated matrix in Redis under a key derived from the sorted flag list. Discuss which representation you would store: the list of maps (readable, large) or the list of bitmask integers plus the flag ordering (compact, requires the ordering to be stable). What breaks if a flag is inserted in the middle of the list and the ordering shifts?
5. **Discussion:** flag combinations rarely fail independently — the real bug above needed exactly two flags on. Given a fixed CI budget of 20 runs, would you rather test 20 configurations sampled from the full power set, or use *pairwise* (all-pairs) coverage, which guarantees every **pair** of flag values appears together in at least one run while typically needing far fewer than `2^n` runs? Explain what pairwise coverage gives up relative to full enumeration, and which class of bug it would still miss.

---

## Bonus Challenge — Permutations II

**LeetCode 47 — Permutations II.**

Given a collection of numbers that **may contain duplicates**, return all possible *unique* permutations. For `nums = [1,1,2]` the answer is `[[1,1,2],[1,2,1],[2,1,1]]` — 3 permutations, not `3! = 6`.

**Task:** start from `permutations` in [code.cpp](code.cpp) (the `used[]`-marker shape) and add duplicate handling. Sort `nums` first, then inside the loop over candidate indices, skip index `i` when `i > 0 && nums[i] == nums[i-1] && !used[i-1]` — that is, skip a duplicate value whose identical predecessor has *not* been placed on the current path. Work out on paper why the condition is `!used[i-1]` and not `used[i-1]`, using `[1,1,2]` as the example: trace both versions and see exactly which output each one loses or duplicates.

**Then, generalize in writing (no code required):** [problems/02-subsets-ii.cpp](problems/02-subsets-ii.cpp) dedupes with `i > start && nums[i] == nums[i-1]`, and this problem dedupes with `i > 0 && nums[i] == nums[i-1] && !used[i-1]`. Both are "skip a same-level duplicate," yet they are written completely differently. Explain what "same recursion level" means in each shape — the subsets version iterates a *suffix* starting at `start`, so `i > start` already means "a sibling branch at this level"; the permutations version iterates the *whole* array at every level, so position alone cannot identify a sibling and the `used[]` state has to do that work instead. Then state the one unifying rule both are implementing, in a single sentence that mentions neither `start` nor `used`, and check it against the *Common Mistakes* and *Interview Discussion* sections of the [README](README.md). Finally: which of the two conditions would you rather be asked to derive live in an interview, and what is the fastest correct way to sanity-check either one on the spot?

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
