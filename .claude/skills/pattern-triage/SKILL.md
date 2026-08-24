---
name: pattern-triage
description: Given an unseen problem, identify which of this repo's 33 DSA patterns (or which design pattern / JS concept) it calls for, and why — using the recognition guides and the input-shape → question-shape → constraint tie-break process, without solving it. Use when the user pastes a LeetCode or interview problem and asks which pattern applies, how to approach it, or which pattern two similar problems differ on.
allowed-tools: Read, Grep, Glob
---

# Pattern triage

The hard part of pattern-based prep is never the code once you know the pattern — it's the
30 seconds before you start typing. That decision is the skill being trained here, so the
output is a *diagnosis and a link*, never an implementation.

## The rule

**Do not solve the problem.** No algorithm walkthrough, no pseudocode, no "and then you just
maintain two pointers while…". You name the pattern, justify it from the problem's own words,
name the runner-up and the signal that separates them, and hand off to the module. If the user
insists on the solution, point them at the module's `cheatsheet.md` skeleton and
`problems/` — that is what those files are for.

## Step 1 — classify, in this order

From `dsa-patterns/PATTERN-RECOGNITION-GUIDE.md`:

1. **Input shape** — array/string · linked list · tree · graph · 2D grid · small fixed set
   (`n ≤ ~20`) · a stream · or "design a structure" rather than "compute a value". This alone
   eliminates most of the eight families.
2. **What's being asked** — a count · a boundary or index · an optimum · all arrangements ·
   a yes/no reachability · a running order-statistic. Narrows to 2-4 candidates.
3. **Constraints as tie-breaker** — size of `n` · sorted or not · weighted edges · a fixed
   capacity/budget · updates interleaved with queries. This is almost always what decides
   between two look-alike patterns; the tie-break table at the bottom of the recognition guide
   exists for exactly this.

Quote the **actual phrase** from the problem statement that triggered each step. "It says
'contiguous subarray' and gives no window size" is a diagnosis; "this looks like sliding
window" is a guess.

## Step 2 — confirm against the module

Open the candidate's `images/recognition-diagram.md` and walk its decision nodes against the
problem. That flowchart was written to separate this pattern from the one it is most often
confused with — if the problem doesn't cleanly reach a terminal node, your candidate is wrong.
Check the candidate's cheatsheet **Avoid When** row too: a problem that trips *Avoid When* is
not this pattern regardless of how well the signals matched.

## Step 3 — the answer

Five parts, short:

1. **Pattern** — name and link to `<family>/<slug>/README.md`.
2. **Why** — the specific phrases in the statement that triggered it, mapped to the pattern's
   Recognition Signal.
3. **Runner-up** — the pattern a reasonable person would also consider, and the *one* signal
   that rules it out here. If nothing was close, say so — a problem with no near-miss is worth
   flagging as an easy one.
4. **Before you type** — the three things to state out loud, in the pattern's own terms:
   - what the **invariant** is (what must stay true at every step),
   - what the **carried state** is (per-level vector, window counts, dp row, monotonic stack),
   - what the **per-step work** is (the one line inside the loop that is problem-specific).
   If they can answer these three, the code is already written. If not, they aren't ready to
   type — and that's the useful finding.
5. **Where to look** — the cheatsheet's Skeleton for the shape, and the closest
   `problems/*.cpp` for a worked variation. Say which of the four is closest and why.

## Non-DSA triage

- **"Which design pattern is this?"** — same shape, driven by `sys-design/INDEX.md`. Classify
  by the *force* being resolved: one interface needs changing (Adapter) · a subsystem needs a
  task-level entry point (Facade) · behaviour varies at runtime (Strategy) · behaviour varies
  by internal mode (State) · access needs controlling (Proxy) · behaviour needs stacking
  (Decorator). Always name the closest neighbour and the distinguishing force — those pairs are
  what interviews probe.
- **"Which JS concept explains this bug?"** — use `js/QUICK-RECALL-GUIDE.md`, which maps
  interview questions to modules directly.

## When the answer is "this isn't one pattern"

Say so. Hard problems compose (binary search over the answer *with* a greedy feasibility
check; DP *on* a topological order). Name both patterns, which one is the outer frame and
which is the inner step, and link both modules. Forcing a composite problem into a single
pattern is the most expensive triage mistake there is.
