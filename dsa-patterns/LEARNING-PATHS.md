# Learning Paths — How To Use This Repo

The repo has 33 patterns and three other docs ([`README.md`](./README.md), [`INDEX.md`](./INDEX.md), [`PATTERN-RECOGNITION-GUIDE.md`](./PATTERN-RECOGNITION-GUIDE.md)). What order you touch them in should depend on where you're starting from — a first-timer and someone refreshing a 3-year-old memory should *not* use this repo the same way. Pick the path below that matches you.

---

## Path 1 — New to DSA entirely

You haven't built strong intuition for any of these patterns yet. Go in order, don't skip ahead, and go deep on each one before moving to the next.

1. **Read [`README.md`](./README.md)'s 8-families table once**, just for orientation — you don't need to understand each row yet, just see the map.
2. **Go family by family in this order:** Array & String → Linked List → Searching & Sorting → Tree & Graph → Recursion & Backtracking → Dynamic Programming → Greedy → Advanced Data Structures. This isn't arbitrary — later families lean on earlier intuition (Bitmask DP assumes you're comfortable with Subsets' recursion tree; Segment Tree is motivated by having already felt Prefix Sum's limits; Topological Sort and Dijkstra both build directly on Graph BFS/DFS). Skipping ahead means hitting "why does this work" moments the repo already answered two families ago.
3. **Within a family, follow its own "Recommended study order"** — every family `README.md` has one, and it's ordered the same way (simplest mental model first, hardest/most-composed pattern last).
4. **For each individual pattern:**
   - Read the `README.md` top to bottom � every pattern is a Full-tier module, so budget a real read (15-20 min).
   - Compile and run `code.cpp` yourself — don't just read it. `g++ -std=c++17 -Wall code.cpp -o /tmp/out && /tmp/out`.
   - If `exercises.md` exists, attempt at least the Easy and Medium problems **before** looking at `problems/`. `exercises.md` has no solutions on purpose.
   - If `problems/` exists, read the worked solutions afterward — compare your approach to theirs, not the other way around.
   - If `cheatsheet.md` exists, do the recall questions at the bottom once, right after finishing the pattern, to check what actually stuck.
   - Mark it `Learned` in [`INDEX.md`](./INDEX.md) and set a `+1 day` revision date on the spaced-repetition ladder.
5. **Once a family is done, go solve 3-5 real problems tagged to it on NeetCode/LeetCode** before moving to the next family — reading a pattern and being able to execute it cold under time pressure are different skills, and this repo only builds the first one.

Rough pace: 2-4 sessions per pattern. There's no need to rush the order — a shaky Array & String foundation makes everything after it harder, not faster.

---

## Path 2 — Want full, systematic mastery (know some DSA, want to cover everything properly)

You've solved problems before but never organized them by pattern, or you want to be certain nothing is missing. Don't restart from zero — run a placement pass first, then fill gaps.

1. **Placement pass:** for every pattern, read just its **Recognition Signal** (or skim the Compact README, since it's already short) and glance at the template code. If it's instantly familiar and you could sketch the code from memory, mark it `Learned` with confidence 4-5 in [`INDEX.md`](./INDEX.md) and move on — no need to re-read the full README. If it's shaky or half-remembered, treat that one pattern exactly like Path 1: full README, exercises, problems, cheatsheet.
2. **Still go in the family order** ([`INDEX.md`](./INDEX.md) lists it top to bottom) even during the placement pass — the *composition* between patterns (e.g., knowing Dijkstra individually but never having contrasted it against Graph BFS/DFS side by side) is often the part that's actually rusty, even when each pattern alone feels familiar.
3. **After the first full pass**, you'll have an [`INDEX.md`](./INDEX.md) fully marked with confidence scores. Do a second pass focused only on anything scoring ≤3 — that's your real gap list, not the 33-pattern list you started with.
4. **From here on, switch to Path 3's revision rhythm** — you've now "started" the spaced-repetition ladder for every pattern, so treat this repo as a revision tool going forward, not a first-read tool.

---

## Path 3 — Learned this before, refreshing before interviews

You've done this material before (a bootcamp, a CS course, a previous job-search cycle) but it's faded. Treat every pattern as a 5-10 minute check, not a re-read.

1. **Don't open the full `README.md` first.** Read `cheatsheet.md` only.
2. **Answer the recall questions (or re-derive the template) from memory, no peeking.** Then try to re-solve one `problems/` file from scratch and compare against the worked solution.
3. **Score yourself honestly in [`INDEX.md`](./INDEX.md)'s Confidence column right after**, not before — the point is to measure what you actually retained, not what you expect to remember.
4. **Confidence ≤3 → that's where real time goes.** Read the full `README.md`, redo `exercises.md`, then re-test yourself again after a day. Confidence 4-5 → don't re-study it, just schedule its next revision date on the ladder (`+1 day → +3 days → +1 week → +2 weeks → +1 month → +3 months`) and move on. Re-studying something you already hold wastes the time this approach is meant to save.
5. **Forgotten which family a pattern even lives in?** [`PATTERN-RECOGNITION-GUIDE.md`](./PATTERN-RECOGNITION-GUIDE.md) is built exactly for that "oh right, that's the ___ one" moment — scan it by input shape/problem signal before diving into a specific pattern's README.
6. **This whole pass should take a few days, not weeks** — you're reactivating memory, not building it from scratch. If a pattern feels like it needs the full Path-1 treatment, that's a sign you didn't actually learn it solidly the first time, and it's fine to give it that treatment — just don't assume the rest of the repo also needs it.

---

## Path 4 — Only need one specific family right now (e.g. "I have a DP-heavy interview next week")

1. Go straight to that family's `README.md` and follow its own "Recommended study order."
2. Check whether your target family depends on an earlier one — the family `README.md`s call this out explicitly (e.g. Dynamic Programming's Bitmask DP needs Subsets from Recursion & Backtracking; Advanced Data Structures' Segment Tree assumes Prefix Sum from Array & String). Read just those 1-2 prerequisite patterns, not the whole earlier family.
3. Use [`PATTERN-RECOGNITION-GUIDE.md`](./PATTERN-RECOGNITION-GUIDE.md) to check whether your target family is commonly tested *alongside* a neighbor (Greedy and Dynamic Programming are frequently tested together specifically because interviewers want to see you tell them apart — see that guide's tie-breaker table).

---

*Whichever path you're on, [`INDEX.md`](./INDEX.md) is where progress lives — it's the only file that needs to stay up to date as you go.*
