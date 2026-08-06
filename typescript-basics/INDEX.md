# TypeScript Basics — Index & Revision Tracker

This is the master index for the TypeScript fundamentals repo. Use it two ways:
1. **Find** a topic quickly.
2. **Revise** on a spaced-repetition schedule so topics move into long-term memory.

---

## How to revise (the flow)

For each topic, revise in tiers — do NOT re-read the full README:

1. **60 sec** — read `cheatsheet.md`.
2. **Active recall** — answer the *Recall Questions* at the bottom of `cheatsheet.md` from
   memory, then try to rebuild the code skeleton without looking at `code.ts`.
3. **Only if you miss something** — open the relevant `README.md` section. Nothing else.

## Spaced-repetition schedule

After each successful revision, set the next date using the ladder:

`Learned → +1 day → +3 days → +1 week → +2 weeks → +1 month → +3 months`

If you fail recall on a topic, drop it back one rung. Update the two date columns each
time you revise.

---

## Topics

| Topic | Format | Status | Last Revised | Next Revision | Confidence (1-5) |
|-------|--------|--------|--------------|---------------|------------------|
| [Classes and Inheritance](classes-and-inheritance/README.md) | Full module | Not started | — | Study next | — |
| [Interfaces and Abstract Classes](interfaces-and-abstract-classes/README.md) | Full module | Not started | — | Study next | — |
| [Access Modifiers](access-modifiers/README.md) | Full module | Not started | — | Study next | — |
| [Generics](generics/README.md) | Full module | Not started | — | Study next | — |
| [Types and Inference](types-and-inference/README.md) | Full module | Not started | — | Study next | — |
| [Utility Types](utility-types/README.md) | Full module | Not started | — | Study next | — |
| [Advanced Types](advanced-types/README.md) | Full module | Not started | — | Study next | — |

> **Legend — Status:** `Not started` = full module ready, not yet studied · `Learned` =
> studied at least once, on the revision ladder. All seven topics use the full template
> (README + cheatsheet + code).

---

## Recommended study order

1. **Classes and Inheritance** — the OOP foundation (`extends`/`super`).
2. **Interfaces and Abstract Classes** — contracts vs. shared implementation (`implements`).
3. **Access Modifiers** — controlling what's visible on the classes you just learned.
4. **Types and Inference** — zoom out from classes to TS's general type vocabulary.
5. **Generics** — reusable, type-safe code across many types.
6. **Utility Types** — deriving new types from existing ones (needs Types + Generics).
7. **Advanced Types** — narrowing and discriminated unions (ties everything together).

When you finish studying one, change its Status to `Learned`, set Last Revised = today,
and Next Revision = today + 1 day.

---

*Convention: each topic lives in its own folder `topic-name/` with `README.md` (deep
explanation), `cheatsheet.md` (fast revision), and `code.ts` (runnable example).*
