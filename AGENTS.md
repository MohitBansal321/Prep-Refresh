# Agent Guide — js-refresh

**Study material repo** (JS, TypeScript, GoF design patterns, C++ DSA patterns).
Not a production codebase — read, run, and practice against, never deploy.

## Repo layout

- `js/` — **12 JS topics** as full modules (`README.md` + runnable `code.js` +
  solution-free `exercises.md` + `cheatsheet.md`). Run a module with
  `node js/<topic>/code.js`. Trackers: `INDEX.md`, quick lookup:
  `QUICK-RECALL-GUIDE.md` (interview question → topic).
- `typescript-basics/` — 7 TS topics, full module each
  (`README.md` + `cheatsheet.md` + `code.ts`).
- `sys-design/` — all **15 GoF patterns** (5 creational, 5 behavioral, 5 structural),
  each a full module (`README.md` + `code.ts` + `exercises.md` + `cheatsheet.md` +
  `images/`). Revision tracker: `INDEX.md`.
- `dsa-patterns/` — 33 C++17 DSA patterns across 8 families (array/string,
  linked-list, searching/sorting, tree/graph, recursion/backtracking, DP, greedy,
  advanced DS). Every pattern has README+`code.cpp`+`exercises.md`+`cheatsheet.md`
  +`images/`+`problems/`; each README opens with a one-line summary, a runnable
  snippet, and a "pick your depth" table pointing at the cheatsheet, the code, or
  the full prose, so a reader is never forced through the whole file to find the
  loop. Revision tracker: `INDEX.md`; quick lookup: `PATTERN-RECOGNITION-GUIDE.md`;
  walkthrough: `LEARNING-PATHS.md` for newcomers.
- `.agent/skills/` — OpenCode skills. Load with `/load <name>`:
  `design-patterns`, `js-fundamentals`, `js-ts-exercises` (the review-mode workflow
  below), `skill-creator`.

## Running code

- TypeScript: `npx ts-node <file>`. **No top-level `package.json`** exists, and
  `npx` fetches `ts-node` on demand so `npm install` is not required. From a pattern
  dir: `cd sys-design/structural-patterns && npx ts-node adapter/code.ts`.
  `structural-patterns/` is the only dir with a `package.json` (`@types/node` for
  strict Node-global typechecking); `npm install` there only if `@types/node` is
  missing.
- C++: `g++ -std=c++17 -Wall path/to/file.cpp -o /tmp/out && /tmp/out`
  (every `.cpp` is standalone and self-checks via `assert`).
- The `opencode.json` formatter runs `npx ts-node --transpile-only $FILE`.

## Revision discipline (spaced repetition)

Every `INDEX.md` tracks status/next-due/confidence. When revising a topic/pattern, do
**NOT** re-read the full README — instead:

1. **60 sec** — read `cheatsheet.md`.
2. **Active recall** — answer the *Recall Questions* in the cheatsheet from memory, then
   rebuild `code.ts`/`code.cpp` (or one `problems/` solution) without looking.
3. **Only if you miss something** — open the relevant README section.

After a successful recall, advance the next-due date along the ladder:
`Learned → +1d → +3d → +1w → +2w → +1mo → +3mo`. A miss drops the card back one rung.

## Exercise workflow (review mode only)

Exercises in `exercises.md` are **deliberately solution-free** — they exist for the user
to solve, not for agents to complete. When the user shares their attempt:

- **Review only**: compare against the companion `code.ts`/`code.cpp` and point out
  mismatches with *why* (conceptual reason), not "change this line".
- **Never hand over a solution.** If asked directly, decline and offer to review what
  they've written instead. If asked to "write the solution", point at the exercises'
  no-solutions rule and offer guided hints.

For generating/grading exercises use the `js-ts-exercises` skill (`/load js-ts-exercises`).

## Commands (via opencode.json)

- `/run-pattern <path>` — run a pattern `code.ts` with ts-node.
- `/list-patterns` — list all design patterns by category.
- `/explain-pattern <path>` — explain a specific pattern.

## Hard constraints

- No test framework, no CI/CD, no linter, no build pipeline, no top-level
  `package.json`. There is **no npm test / tsc --noEmit gate** to satisfy — each file is
  verified by running it directly.
- `opencode.json` watcher ignores `*.js` and `*.md`; only `.ts` changes trigger reloads.
- Do not commit (this is local study material); git is used only for local history.
