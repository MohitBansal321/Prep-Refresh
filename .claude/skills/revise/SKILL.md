---
name: revise
description: Run a spaced-repetition revision session against this repo's INDEX trackers — pick what is due today, drill it with cheatsheet-first active recall, then advance or demote the card on the ladder and write the dates back into INDEX.md. Use when the user asks what to revise, wants to be quizzed, says they are due for review, or wants to log a revision they just finished.
allowed-tools: Read, Grep, Glob, Edit, Bash
---

# Revision drill

The repo is a spaced-repetition system. `INDEX.md` files hold the card state
(`Status · Last Revised · Next Revision · Confidence`) and the ladder rule. Your job is to
select, drill, grade, and write state back — in that order, without skipping the grading.

## Rule zero: never leak the answer

This whole skill is worthless if you show answers before the user attempts them. Concretely:

- Ask **one** recall question at a time. Stop. Wait for their answer. Do not batch questions,
  and never append "(the answer is…)".
- When they ask you to "just tell me", give the *next* question's worth of scaffolding
  instead — a pointer to the section, not the sentence.
- Do not read the README aloud before the drill. The README is the *remediation* step, opened
  only for the specific things they missed.
- If they get it right in different words than the cheatsheet, that's a pass. Recall is about
  the mechanism, not the phrasing.

## Step 1 — build today's queue

```bash
date +%F                        # never assume the date
```

Read the relevant `INDEX.md` (`js/`, `typescript-basics/`, `sys-design/`, `dsa-patterns/` —
all four if the user didn't say). Sort into:

1. **Overdue** — `Next Revision` earlier than today. Oldest first. These decay fastest.
2. **Due today** — `Next Revision` equals today.
3. **New** — `Status: Not started`. Take these in the file's *Recommended study order*, not
   table order, and never more than one or two per session.

Present the queue as a short list with the due date and last confidence, then ask which to
start with (or start at the top if they said "just quiz me"). If nothing is due, say so
plainly and offer the lowest-confidence card as optional practice — do not invent work.

## Step 2 — run the drill

Follow the tier ladder from `AGENTS.md`, and the per-area variant in
[references/drill-protocols.md](references/drill-protocols.md):

1. **60 seconds** — they read `cheatsheet.md`. (DSA/sys-design: plus a skim of `images/`.)
2. **Active recall** — you ask the `## Recall Questions` from that cheatsheet, one at a time.
3. **Rebuild from memory** — they reconstruct the code without looking. Then *actually run it*
   (`node` / `npx ts-node` / `g++ -std=c++17 -Wall … && /tmp/out`) so the verdict comes from
   the compiler, not from your reading of it.
4. **Remediate misses only** — open the exact README section for what they missed. Nothing
   else. Re-reading the whole README is the failure mode this system exists to prevent.

## Step 3 — grade

Per question: **got it** / **partial** (right shape, missing the mechanism) / **missed**.

The card **passes** if they got the mechanism right — the thing in *Remember In One Sentence* —
and their rebuild runs. Fumbling a syntax detail is a pass; reproducing the code but not being
able to say *why* the key line is there is a **fail**. This repo drills understanding, not
typing.

Confidence 1-5, from their performance, not their mood:
- **5** — mechanism stated cleanly, rebuild ran first try, handled follow-ups.
- **4** — mechanism right, rebuild needed one small fix.
- **3** — mechanism recovered with a nudge, or rebuild needed real debugging.
- **2** — needed the README for the core idea.
- **1** — could not reconstruct the approach.

## Step 4 — write the card back

Update that module's row in `INDEX.md` — and only that row. Ladder math, rung inference, and
the exact edit are in [references/drill-protocols.md](references/drill-protocols.md#ladder).

```
Learned → +1 day → +3 days → +1 week → +2 weeks → +1 month → +3 months
```

Pass = advance one rung. Fail = drop one rung (never below `+1 day`). First-ever pass on a
`Not started` card sets `Status` to `Learned` and `Next Revision` to today + 1 day.

Then tell them, in one line: what moved, to when, and the single thing worth re-reading before
that date.

## If they just want to log a revision

They may have revised offline. Ask only for the module and pass/fail, then do Step 4. Do not
insist on running the drill.
