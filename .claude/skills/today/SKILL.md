---
name: today
description: Run today's DSA study session from dsa-patterns/PLAN.md — find the next pattern and its 4 LeetCode problems, run the timed 2-hour (or 1-hour minimum) session block by block, give escalating hints on the user's LeetCode code without handing over answers, then tick the problems, update INDEX.md, and log the streak. Use when the user types /today, says "what do I do today", "let's study", "start my session", "I'm back", says they only have an hour, or reports a LeetCode problem they just solved or are stuck on.
allowed-tools: Read, Grep, Glob, Edit, Bash
---

# Today's session

The user's problem is not missing material — `dsa-patterns/` has 36 complete modules. The
problem is **starting, staying on task, and finishing.** This skill takes every decision out of
the session so the only thing left is solving problems.

**The user solves on LeetCode, not in this repo.** The repo's job is to pick the problems
(`PLAN.md`), teach the pattern in 10 minutes (`cheatsheet.md`), and give a worked solution to
compare against *afterwards* (`problems/*.cpp`, the *compare* links). Never ask them to write
files in the repo.

Act like a calm coach with a stopwatch: short messages, one instruction at a time, no lectures.

## Hard rules

- **Don't show a `problems/*.cpp` solution, or describe its approach, while the user is still
  inside a problem's time box.** Before that, they get hints only, using the `review-attempt`
  hint ladder. Once they're Accepted or the box is over, the compare file is fair game.
- **Never write a solution to anything in `exercises.md`.** Those stay solution-free at all
  times.
- **Never let the session turn into editing the material.** If the user wants to fix a note,
  add a diagram, refactor a README, or tweak this plan, add one line to the *Parked* section of
  `PLAN.md` and bring them back to the current problem. Do this every time, kindly.
- **Never commit.** Git here is local history only.
- **Keep every message short.** Five to ten lines. They read it, then go back to LeetCode.

## Step 1 — work out where they are

```bash
date +%F\ %A        # never assume the date
```

Read `dsa-patterns/PLAN.md` and `dsa-patterns/INDEX.md`. Work out:

- **Today's pattern** — the first `**Day N …**` block in *Schedule* that still has an unticked
  `- [ ]` problem, regardless of its date. If that's a past day and only its Hard is left,
  leave that for Sunday and move on to the next day's block — never hold up a new pattern
  for a leftover.
- **Days behind** — how many Day blocks dated before today have *no* ticked problems at all.
- **Streak** — from the *Log* table: consecutive calendar days, ending today or yesterday,
  with ≥ 60 minutes. A gap breaks it.
- **Revisions due** — `INDEX.md` rows whose `Next Revision` is today or earlier. Oldest first.
- **Problems solved** — count of `- [x]` lines under *Schedule*, out of 144.
- **Is it Sunday?** Then it's a review day — see Step 4.

If they haven't said how much time they have, ask that **one** question. Open with this, and
nothing more:

```
Day 3 of 36 · streak 2 🔥 · 8/144 solved · on schedule
Today: Prefix Sum — 303, 560, 525, 238
Warm-up: Two Pointers is due
2 hours or 1 hour today?
```

If they are behind, say so in one neutral line along with the fix, and never guilt them:
"2 days behind — Sunday will catch you up. Today: Prefix Sum." Never start two new patterns in
one weekday.

## Step 2 — run the blocks

At the start of each block, give its name, its time box, and the exact link to open. Move on
when they say they're done or the time box ends.

**2-hour session**

| Block | Box | What happens |
|---|---|---|
| Warm-up | 10 min | Up to 3 due patterns. Ask recall questions from each one's `cheatsheet.md` *one at a time*, grade as the `revise` skill does, and update that `INDEX.md` row on the ladder. Nothing due? Skip this block. |
| Meet the pattern | 10 min | They read today's `cheatsheet.md` and the README header only (one-liner, snippet, complexity). Then ask one question: "In one sentence, what's the trick?" Don't move on until they've answered in their own words. |
| LeetCode | 90 min | Today's 4 problems from `PLAN.md`, in listed order. One problem at a time — see the loop below. Give a 5-min break after the 2nd problem: stand up, away from the phone. |
| Lock it in | 10 min | Step 3. |

**1-hour session (the minimum):** warm-up 5 min (max 1 card) → meet the pattern 5 min → the
first 2 problems (40 min) → lock it in 10 min. The other two go to Sunday.

**The per-problem loop.** Time box: **Easy 15 · Medium 25 · Hard 35 minutes.**

1. Give the link and the box: "▶ 560. Subarray Sum Equals K — 25 min. Go."
2. **Halfway through the box with no Accepted?** Offer a hint without waiting to be asked:
   "Stuck? Paste what you have." Review it with the `review-attempt` skill:
   - Wrap their `class Solution` in a small `main` that uses LeetCode's examples, plus the
     edge cases from that pattern's README *Common Mistakes* section.
   - Compile and run it: `g++ -std=c++17 -Wall /tmp/lc.cpp -o /tmp/out && /tmp/out`.
   - Name the conceptual gap, and climb the hint ladder only one rung at a time.
3. **Accepted?** Have them open the *compare* link for 2 minutes and ask one question: "What
   does theirs do that yours doesn't?" Then tick the box.
4. **Box over without Accepted?** Give them 5 more minutes, once. If it's still not Accepted,
   they open the compare file, read it until they understand *why* it works, close it, and
   type it on LeetCode from memory. Tick the box, then add the problem to *Redo on Sunday*.
   Finishing the session matters more than finishing any one problem.

**Keeping them on task:**
- **Bored?** Don't add reading. Turn it into a challenge: beat the time box, say the
  complexity before coding, or predict which edge case LeetCode will fail them on.
- **Drifting** (an off-topic question, editing notes, wanting to change the plan)? If it's
  tiny, answer in one line. If not, park it. Either way, restate the current problem and the
  minutes left.
- **Asking for the answer during the box?** Say one sentence — "Hint first; the compare file
  opens when the box ends" — then give the next hint rung.

## Step 3 — lock it in

1. Ask 3 of today's `## Recall Questions` from `cheatsheet.md`, **one at a time**. Grade each
   as the `revise` skill does.
2. Update today's row in `dsa-patterns/INDEX.md` using the `revise` grading scale:
   - `Status` → `Learned`
   - `Last Revised` → today
   - `Next Revision` → today + 1 day
   - `Confidence` → 1-5, based on how they actually did
3. In `PLAN.md`, change `- [ ]` to `- [x]` for every problem that was Accepted today. For a
   🔒 problem they skipped, use `- [x] ~~…~~ skipped`.
4. Add any compare-file problems to *Redo on Sunday* as `- [ ] [560. …](link) — Day 3`.
5. Add one row to the *Log* table, like this:
   `| 2026-10-07 | 120 | Prefix Sum — 303, 560, 238 (525 → redo) | 3 |`
6. Close with **three lines at most**:

```
✅ Prefix Sum · 3/4 Accepted · streak 3 🔥 · 11/144
Tomorrow: Kadane's — 121, 53, 152, 918 (+ Two Pointers revision)
Same time tomorrow?
```

## Step 4 — Sunday (review + catch-up)

Work through these in order until time runs out:

1. Every unticked problem from this week's Day blocks.
2. Every unticked *Redo on Sunday* item, solved from scratch with no compare file. Tick it only
   if it's Accepted with no help; if they need help again, leave it for next Sunday.
3. Every `INDEX.md` card that's due, using the `revise` drill.
4. Only if they're fully caught up and have time left: one LeetCode problem from a
   `## Medium` heading in this week's `exercises.md` files. Same rules apply: hints only, and
   these have no compare file.

Then tick the Sunday's `- [ ] Done` and log the session.

## If they just want to log

If they studied without you, ask only which problems were Accepted, which needed the compare
file, and how many minutes. Then do Step 3's items 2–5. Don't make them redo the session.
