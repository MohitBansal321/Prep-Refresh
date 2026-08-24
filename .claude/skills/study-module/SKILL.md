---
name: study-module
description: Author or extend a study module in this repo to its exact per-area spec — README, runnable code, solution-free exercises, cheatsheet with recall questions, mermaid diagrams, worked problems, and index registration. Use when adding a new topic or DSA pattern, upgrading a Compact/Partial module to Full, adding worked problems or diagrams to an existing module, or filling a gap found by module-audit.
---

# Authoring a study module

The repo's value is that every module is built the same way, so revision is muscle memory
instead of re-orientation. A module that is 90% to spec is worse than no module — it breaks
the drill and quietly rots the INDEX. Build to spec or don't ship it.

## Non-negotiables

1. **`exercises.md` contains no solutions.** Not in a fenced block, not "for reference", not
   collapsed behind a `<details>`. Exercises exist for the user to solve. Worked solutions
   live only in `problems/` (DSA) and must be *different problems* from the exercises.
2. **The code file is the source of truth.** Write and run it *first*. Every claim in the
   README and cheatsheet must be checkable against a real line in it.
3. **The cheatsheet ends with `## Recall Questions (answer from memory — no peeking)`.**
   Without it the module cannot enter the spaced-repetition ladder — it is not optional
   decoration, it is the module's interface to `INDEX.md`.
4. **Register the module.** A module not linked from its area `INDEX.md` (and the area's
   lookup guide) is invisible and will never be revised.
5. **Never commit.** `AGENTS.md` is explicit: git here is local history only.

## Step 1 — place the module

Area determines language, file set, README shape, and diagram names. They are **not** the
same. Read [references/module-spec.md](references/module-spec.md) before creating any file.

| Area | Path | Code | Diagrams | `problems/` |
|---|---|---|---|---|
| `js/` | `js/<topic>/` | `code.js` | none | no |
| `typescript-basics/` | `typescript-basics/<topic>/` | `code.ts` | none | no |
| `sys-design/` | `sys-design/<category>-patterns/<pattern>/` | `code.ts` | class · sequence · flow | no |
| `dsa-patterns/` | `dsa-patterns/<family>/<pattern>/` | `code.cpp` | recognition · flow · trace | **yes — 4 worked LeetCode solutions + `problems/README.md`** |

If the topic already exists as Compact or Partial, you are *upgrading*: keep the existing
README's correct content, add the missing files, and deepen the README to the full heading
order rather than rewriting from scratch.

## Step 2 — build in this order

The order matters: each artifact is derived from the one before it, so building out of order
produces a cheatsheet that describes code you have not written.

1. **`code.{js,ts,cpp}`** — self-contained, self-verifying. C++ asserts *and* prints
   `[PASS] <case> -> <expected>` per case (see `dsa-patterns/tree-graph-patterns/tree-bfs/code.cpp`).
   Run it before writing a word of prose:
   - `node js/<topic>/code.js`
   - `npx ts-node <path>/code.ts`
   - `g++ -std=c++17 -Wall <path>/code.cpp -o /tmp/out && /tmp/out` (Bash tool, not PowerShell)
2. **`README.md`** — the deep-dive, in the area's exact heading order. Why before how; never
   open with code.
3. **`problems/` (DSA only)** — 4 standalone LeetCode solutions, each compiling and printing
   PASS/FAIL, chosen so each one exercises a *different facet* of the pattern. Then
   `problems/README.md` with the summary table and a "Why these four" section that says what
   facet each covers.
4. **`exercises.md`** — Easy → Medium → Hard → Real-World Challenge → Bonus. Every problem
   different from `problems/`. Each ends with **Think about** (a trap to reason through) and
   **Then answer** (a question that forces cross-referencing another file in the module).
5. **`cheatsheet.md`** — summary table → `### Skeleton` → `### Remember In One Sentence` →
   `### Two Facts People Get Wrong` → `## Recall Questions`. Written last because every row
   must be traceable to the code and README you just wrote.
6. **`images/*.md`** — mermaid only, one fenced ```mermaid block per file, plus a
   "How to read it" section underneath.
7. **Registration** — see [references/registration.md](references/registration.md).

## Step 3 — verify before declaring done

Run every one of these. Do not report the module complete until they all pass:

- [ ] Code file compiles/runs clean and prints its own PASS output.
- [ ] DSA: all 4 `problems/*.cpp` compile with `-Wall` and print PASS.
- [ ] `grep -c 'Recall Questions' cheatsheet.md` → 1, with ≥ 8 numbered questions.
- [ ] No fenced solution code in `exercises.md` beyond signatures/skeletons.
- [ ] Every relative link resolves (`python .claude/skills/module-audit/scripts/audit.py <path>`).
- [ ] Module row added to the area `INDEX.md` with Status `Not started`.
- [ ] Lookup guide updated (`PATTERN-RECOGNITION-GUIDE.md` or `QUICK-RECALL-GUIDE.md`).

## Writing quality

The repo's prose has a specific voice — mechanism-first, anchored to real lines, allergic to
filler. Read [references/writing-standards.md](references/writing-standards.md) before drafting
the README. If the draft could have been written without opening the code file, it is wrong.
