# Repository Purpose

**Study material repository** for JavaScript, TypeScript, and system design learning. Not a production codebase.

## Layout

- `js/` — standalone `.js` files (JS fundamentals)
- `typescript-basics/` — 7 TS language feature modules (classes, generics, types, etc.), each with `README.md` + `cheatsheet.md` + `code.ts`
- `sys-design/creational-patterns/` — 5 GoF creational pattern modules (builder, singleton, factory, abstract-factory, prototype)
- `sys-design/behavioral-patterns/` — 5 GoF behavioral pattern modules (strategy, observer, command, state, iterator)
- `sys-design/structural-patterns/` — 5 GoF structural pattern modules (adapter, composite, decorator, facade, proxy)
- `.agent/skills/` — OpenCode skill definitions (design-patterns, js-fundamentals, js-ts-exercises, skill-creator)

All 15 GoF patterns use the same **full module** format: each pattern is a subdirectory containing `README.md`, `code.ts`, `exercises.md`, `cheatsheet.md`, `images/`.

`sys-design/INDEX.md` and `typescript-basics/INDEX.md` serve as spaced-repetition revision trackers.

## Running TypeScript Files

Each `code.ts` is a standalone runnable file. Use `ts-node` from the module's parent directory, or from the pattern's own directory:

```
cd sys-design/structural-patterns && npx ts-node adapter/code.ts
cd sys-design/creational-patterns && npx ts-node builder/code.ts
cd typescript-basics && npx ts-node classes-and-inheritance/code.ts
```

There are **three `tsconfig.json`** files that control compilation scope:
- `sys-design/tsconfig.json` — parent config covering all patterns
- `sys-design/creational-patterns/tsconfig.json`
- `sys-design/structural-patterns/tsconfig.json`

`structural-patterns/` has its own `package.json` with `@types/node` (the only subdirectory with npm dependencies; run `npm install` there if types are missing).

## Exercise Workflow

When the user asks for help with exercises from a pattern's `exercises.md`:
- **Review mode**: Compare their code against the pattern's `code.ts` to check correctness — point out mismatches, but explain *why*, not just "fix this line"
- **Do NOT provide code solutions** — guide the user with hints, questions, and conceptual nudges so they arrive at the answer themselves. If asked to write the solution, decline and offer to review what they've written instead.

## Commands (via opencode.json)

- `/run-pattern <path>` — run a pattern `code.ts` with ts-node
- `/list-patterns` — list all design patterns by category
- `/explain-pattern <path>` — explain a specific pattern

## Constraints

- No test framework, no CI/CD, no linting, no build pipeline
- No top-level `package.json`
- Each `code.ts` is self-contained and verified by running it directly
