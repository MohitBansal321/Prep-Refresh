---
name: review-attempt
description: Review the user's own attempt at an exercise or problem in this study repo without handing over the solution — verify their code by actually running it, name the conceptual mismatch rather than the line to change, and escalate hints only as far as needed. Use whenever the user shares code they wrote for an exercise, asks "is this right", "review my solution", "why doesn't this work", or asks for a solution to a solution-free exercise.
allowed-tools: Read, Grep, Glob, Bash
---

# Reviewing an attempt

`AGENTS.md` makes this a hard repo rule: **exercises are deliberately solution-free — they
exist for the user to solve, not for agents to complete.** Writing the solution destroys the
only asset this repo has. Reviewing well is the whole job.

## The rule, and how to hold it

Never produce a working solution to an exercise the user is currently attempting. When asked
directly ("just show me the answer"), say plainly that the exercises are solution-free by
design, then offer the next rung of the hint ladder. Don't moralise about it — one sentence,
then get back to useful work.

**Not covered by the rule:** explaining a concept, running their code, showing them a line
that already exists in the module's `code.cpp`/`code.ts` and discussing how it differs from
theirs, or writing a *test case* that exposes their bug. Use all of these freely.

## Step 1 — establish ground truth before opining

Find the module the exercise belongs to and read its companion code — `code.cpp`/`code.ts`
and, for DSA, the relevant `problems/*.cpp`. The review is *against the module's own model of
the pattern*, not against your general taste.

Then **run their code.** This is the difference between a review and a guess:

```bash
g++ -std=c++17 -Wall /tmp/attempt.cpp -o /tmp/out && /tmp/out   # DSA (note -Wall)
npx ts-node /tmp/attempt.ts                                      # TS / sys-design
node /tmp/attempt.js                                             # JS
```

If it compiles and passes their own cases, write a case that breaks it — the edge case the
module's Common Mistakes section warns about (empty input, single node, one-sided chain,
duplicate keys, integer overflow). Show them the failing input and the actual output. Evidence
beats assertion, and it teaches them to build the case themselves next time.

## Step 2 — separate the three failure classes

Say which one you are looking at, because the response differs:

1. **Conceptual** — they misunderstood the mechanism (snapshot taken inside the loop, `prev`
   hoisted out of the level loop, mutating where the pattern requires a copy, a Facade that
   reimplements its subsystem). This is the only class worth spending real time on.
2. **Correctness** — the concept is right, an edge case is not. Hand them the failing input,
   not the fix.
3. **Style/idiom** — naming, types, `any`, missing `const`. Mention briefly at the end. Never
   lead with it; leading with style on a conceptually broken attempt is noise.

## Step 3 — the hint ladder

Climb one rung at a time. Wait for another attempt between rungs. Most attempts resolve at
rung 1 or 2, and the learning is in the gap.

1. **Locate** — "the bug is in how you bound the inner loop; everything else is right."
2. **Name the invariant** — "the loop must run over the level's membership *as it was when the
   level began*. Yours re-reads it after you've already pushed children. What is `q.size()` by
   iteration two?"
3. **Contrast with the module** — show the corresponding line from `code.cpp`, side by side
   with theirs, and ask what the difference does. Not "copy this."
4. **Trace it together** — walk the failing input through *their* code, state by state, and let
   them find the divergence.

After three genuine attempts, or if they explicitly say they want to move on: still don't
paste the solution. Offer to work through it line by line as they type, or to mark the
exercise as one to retry in the next `revise` session.

## Step 4 — the response shape

Keep it short. Long reviews get skimmed.

- **Verdict** — passes / fails, and on what input.
- **What's right** — one line, and be specific; "you took the snapshot correctly" is worth
  more than "good job."
- **What's wrong** — the conceptual reason, not the line number. Why the mental model produced
  this code.
- **The hint** — one rung, phrased as a question they can answer.
- **Next** — what to try, and which module section to check if they're stuck (a section, not
  the whole README).

## Per-area review dimensions

- **DSA** — the pattern's signature invariant first (snapshot / two-pointer movement / window
  shrink condition / visited set), then complexity vs the cheatsheet's stated bound, then edge
  cases (empty, single, all-equal, skewed, overflow), then whether `-Wall` was clean.
- **sys-design** — do the participants have the pattern's responsibilities, or has one absorbed
  another's? Is the dependency direction right? Is the facade thin or leaky? Would this pattern
  choice survive the module's *When NOT To Use* section?
- **js** — reference vs value, mutation of a shared object, `this` binding, closure capture in
  loops, await inside a loop that should be `Promise.all`, floating promises.
- **typescript-basics** — is `any` doing the work a generic should? Is the constraint as loose
  as it can be while staying safe? Does the type actually catch the mistake it was written to
  catch — test it by writing the invalid call and checking the compiler rejects it.
