# TypeScript Basics

This folder is for the TypeScript language features that don't come up often enough to
become muscle memory — `extends`, `super`, `implements`, generics, and the rest of the
type-system vocabulary. Unlike `sys-design/` (which is about *design patterns* — reusable
solutions to recurring architecture problems), this folder is about the *language itself*:
the keywords and type-system tools TypeScript gives you to express those solutions.

Every topic uses the same **full module** format as the design-patterns repo:

- **`README.md`** — the deep explanation: what the feature is, what problem it solves,
  the common beginner confusions, worked through with one consistent example.
- **`cheatsheet.md`** — a one-minute table + code skeleton + recall questions. This is
  the file to open when you just need a fast refresher, not the full story.
- **`code.ts`** — a single runnable file with the same example as the README, so you can
  `ts-node code.ts` and see it work.

## How to read this repo

For **fast revision** (checking/refreshing a concept you've seen before), use the
[revision tracker (INDEX.md)](./INDEX.md) — start with `cheatsheet.md` for each topic and
only drop into `README.md` if a recall question stumps you.

For **learning a concept for the first time**, read `README.md` top to bottom, then run
`code.ts`, then do the recall questions in `cheatsheet.md` from memory.

## Topics

| Topic | What it covers |
|-------|-----------------|
| [Classes and Inheritance](./classes-and-inheritance/) | `class`, `extends`, `super`, method overriding, `static` members |
| [Interfaces and Abstract Classes](./interfaces-and-abstract-classes/) | `interface`, `implements`, `abstract class`, contract vs shared implementation |
| [Access Modifiers](./access-modifiers/) | `public`/`private`/`protected`/`readonly`, parameter properties, getters/setters |
| [Generics](./generics/) | `<T>`, generic constraints, generic classes, default type parameters |
| [Types and Inference](./types-and-inference/) | `type` vs `interface`, unions, intersections, literal types, `enum` |
| [Utility Types](./utility-types/) | `Partial`, `Pick`, `Omit`, `Record`, `ReturnType`, and friends |
| [Advanced Types](./advanced-types/) | type guards, narrowing, discriminated unions, exhaustiveness checks |

## Recommended study order

These build on each other, so going in order makes each next topic easier:

1. **Classes and Inheritance** — the OOP foundation (`extends`/`super`).
2. **Interfaces and Abstract Classes** — contracts vs. shared implementation (`implements`).
3. **Access Modifiers** — controlling what's visible on the classes you just learned.
4. **Types and Inference** — zoom out from classes to TS's general type vocabulary.
5. **Generics** — reusable, type-safe code across many types.
6. **Utility Types** — deriving new types from existing ones (needs Types + Generics).
7. **Advanced Types** — narrowing and discriminated unions (ties everything together).
