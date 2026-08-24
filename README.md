# js-refresh

A **study material repository** for JavaScript, TypeScript, system design patterns, and DSA
patterns. Not a production codebase — everything here exists to be read, run, and practiced
against, not deployed.

## Layout

| Folder | What it's for |
|--------|----------------|
| [js/](js/) | 12 core-JavaScript modules (concepts → runnable code → exercises → cheatsheets), with revision tracker and interview quick-recall guide |
| [typescript-basics/](typescript-basics/) | TypeScript language features (generics, classes, access modifiers, utility types, etc.) that don't come up often enough to become muscle memory |
| [sys-design/](sys-design/) | All 15 GoF design patterns (creational, structural, behavioral), each as a full module: `README.md` + `code.ts` + `exercises.md` + `cheatsheet.md` |
| [dsa-patterns/](dsa-patterns/) | Interview-style DSA patterns in C++, organized by the 8 families (array/string, linked list, trees/graphs, DP, greedy, etc.) rather than by individual problem |

Each of the four top-level folders has its own `README.md` — start there for that area's
philosophy and how its modules are structured.

## Shared format

Most topics (TypeScript basics, design patterns, DSA patterns) follow the same **full module**
shape: a subdirectory per topic containing —

- `README.md` — the deep-dive explanation
- `code.ts` / `code.cpp` — a runnable, self-contained example
- `exercises.md` — Easy → Hard → Real-world practice problems, solutions intentionally omitted
- `cheatsheet.md` — a one-minute revision sheet with recall questions

An `INDEX.md` inside each top-level folder acts as a spaced-repetition revision tracker —
use it to decide what to review next rather than working front-to-back.

## Running code

TypeScript files are run directly with `ts-node` from the relevant folder, e.g.:

```
cd sys-design/structural-patterns && npx ts-node facade/exercises.ts
cd typescript-basics && npx ts-node generics/code.ts
```

`sys-design/structural-patterns/` has its own `package.json` (just `@types/node`, for
type-checking Node globals under `strict` mode) — run `npm install` there if types are
missing. No other folder has npm dependencies, and there's no top-level `package.json`,
test framework, CI, or build pipeline — each file is self-contained and verified by running
it directly.

See [AGENTS.md](AGENTS.md) for more detail on repo conventions and the exercise-review
workflow.
