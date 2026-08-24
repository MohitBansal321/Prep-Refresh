# JavaScript Fundamentals — Index & Revision Tracker

Master index for `js/`. Two uses:
1. **Find** a topic quickly.
2. **Revise** on a spaced-repetition schedule so topics move into long-term memory.

---

## How to revise (the flow)

For each topic, revise in tiers — do NOT re-read the full README:

1. **60 sec** — read `cheatsheet.md`.
2. **Active recall** — answer the *Recall Questions* at the bottom of `cheatsheet.md` from memory, then rebuild the Easy/Medium exercise from `exercises.md` without looking at `code.js`.
3. **Only if you miss something** — open the relevant README section. Nothing else.

## Spaced-repetition schedule

After each successful revision, set the next date using the ladder:

`Learned → +1 day → +3 days → +1 week → +2 weeks → +1 month → +3 months`

If you fail recall on a topic, drop it back one rung. Update the two date columns each time you revise.

---

## Modules

| Topic | Format | Status | Last Revised | Next Revision | Confidence (1-5) |
|-------|--------|--------|--------------|---------------|------------------|
| [Primitives & Types](primitives-and-types/README.md) | Full module | Not started | — | Study next | — |
| [Objects & References](objects-and-references/README.md) | Full module | Not started | — | Study next | — |
| [Immutability](immutability/README.md) | Full module | Not started | — | Study next | — |
| [Null & Undefined](null-and-undefined/README.md) | Full module | Not started | — | Study next | — |
| [Scope & Closures](scope-and-closures/README.md) | Full module | Not started | — | Study next | — |
| [Functions](functions/README.md) | Full module | Not started | — | Study next | — |
| [This & Prototypes](this-and-prototypes/README.md) | Full module | Not started | — | Study next | — |
| [Arrays & Array Methods](arrays-and-array-methods/README.md) | Full module | Not started | — | Study next | — |
| [Async JavaScript](async-javascript/README.md) | Full module | Not started | — | Study next | — |
| [Event Loop](event-loop/README.md) | Full module | Not started | — | Study next | — |
| [ES6+ Features](es6-features/README.md) | Full module | Not started | — | Study next | — |
| [Error Handling](error-handling/README.md) | Full module | Not started | — | Study next | — |

> **Legend — Status:** `Not started` = full module ready, not yet studied · `Learned` = studied at least once, on the revision ladder.
> Each module folder: `README.md` (concept deep-dive) + `code.js` (runnable: `node js/<topic>/code.js`) + `exercises.md` (solution-free) + `cheatsheet.md` (60-sec revision).

---

## Recommended study order (fresh start)

The modules build on each other — follow this order for a clean first pass:

1. **Primitives & Types** — what values exist, `typeof`, coercion basics.
2. **Objects & References** — the reference model everything else rests on.
3. **Immutability** — why we copy instead of mutate (uses #2 heavily).
4. **Null & Undefined** — absence, falsy vs nullish, `?.` / `??`.
5. **Scope & Closures** — lexical scope, `var` vs `let/const`, closures.
6. **Functions** — arrows vs regular, higher-order functions, composition.
7. **This & Prototypes** — binding rules, prototype chain, classes as sugar.
8. **Arrays & Array Methods** — map/filter/reduce, mutating vs not.
9. **Async JavaScript** — callbacks → promises → async/await.
10. **Event Loop** — microtasks vs macrotasks, ordering puzzles.
11. **ES6+ Features** — destructuring, spread/rest, Map/Set, modern syntax.
12. **Error Handling** — throw/catch discipline, async errors, custom errors.

Fast track if you only have one evening before an interview: cheatsheets of 1, 2, 5, 9, 10 + QUICK-RECALL-GUIDE.md.

---

## Cross-references

- Interview-question → topic mapping: [QUICK-RECALL-GUIDE.md](QUICK-RECALL-GUIDE.md)
- Design patterns: [sys-design/INDEX.md](../sys-design/INDEX.md)
- DSA patterns: [dsa-patterns/INDEX.md](../dsa-patterns/INDEX.md)
