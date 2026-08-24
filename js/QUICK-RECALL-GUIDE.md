# JavaScript Quick-Recall Guide

Interview question → which module to open. Use this when you get (or fear) a question and need the right section fast. Same idea as `dsa-patterns/PATTERN-RECOGNITION-GUIDE.md`.

**How to use:** find the question → go to that topic's `cheatsheet.md` first → only if shaky, its README. For hands-on verification, rebuild the matching exercise from the module's `exercises.md`.

---

## Type System & Values

| If asked… | Go to |
|---|---|
| "What are the JS data types?" / "`typeof` quirks?" | [primitives-and-types](primitives-and-types/README.md) |
| "Why is `typeof null === 'object'`?" | [primitives-and-types](primitives-and-types/README.md) · [null-and-undefined](null-and-undefined/README.md) |
| "`null` vs `undefined`?" | [null-and-undefined](null-and-undefined/README.md) |
| "`==` vs `===`?" / coercion puzzles | [primitives-and-types](primitives-and-types/README.md) |
| "`0.1 + 0.2` problem" / NaN checks | [primitives-and-types](primitives-and-types/README.md) |
| "Why does `'abc'.length` work if strings have no methods?" (auto-boxing) | [primitives-and-types](primitives-and-types/README.md) |

## References & State

| If asked… | Go to |
|---|---|
| "Pass-by-value or pass-by-reference?" | [objects-and-references](objects-and-references/README.md) |
| "Shallow vs deep copy? How to deep clone?" | [objects-and-references](objects-and-references/README.md) |
| "Why did my copied object change the original?" (predict-the-output) | [objects-and-references](objects-and-references/README.md) |
| "What does `const` actually freeze?" | [immutability](immutability/README.md) |
| "How would you update nested state immutably?" (React-style) | [immutability](immutability/README.md) |
| "`Object.freeze` — deep or shallow?" | [immutability](immutability/README.md) |

## Scope, Functions & `this`

| If asked… | Go to |
|---|---|
| "What is a closure? Real uses?" | [scope-and-closures](scope-and-closures/README.md) |
| "Classic loop bug: `var` prints 3 3 3 in setTimeout" | [scope-and-closures](scope-and-closures/README.md) |
| "`var` vs `let` vs `const`" / TDZ / hoisting | [scope-and-closures](scope-and-closures/README.md) |
| "Arrow vs regular functions?" | [functions](functions/README.md) |
| "Implement debounce / throttle / once / curry / memoize" | [functions](functions/exercises.md) + [scope-and-closures](scope-and-closures/exercises.md) |
| "How is `this` determined?" (5 binding rules) | [this-and-prototypes](this-and-prototypes/README.md) |
| "Lost `this` when passing a method / in a callback — why?" | [this-and-prototypes](this-and-prototypes/README.md) |
| "`call` vs `apply` vs `bind`" / implement bind | [this-and-prototypes](this-and-prototypes/README.md) + [exercises](this-and-prototypes/exercises.md) |
| "Prototype chain / `__proto__` vs `prototype` / how classes work" | [this-and-prototypes](this-and-prototypes/README.md) |

## Arrays & Data Manipulation

| If asked… | Go to |
|---|---|
| "map vs filter vs reduce" / implement reduce from scratch | [arrays-and-array-methods](arrays-and-array-methods/README.md) |
| "Which array methods mutate? Does sort mutate?" | [arrays-and-array-methods](arrays-and-array-methods/README.md) |
| "Group by / frequency count / flatten with reduce" | [arrays-and-array-methods](arrays-and-array-methods/README.md) |
| "Dedupe an array" | [es6-features](es6-features/README.md) (Set) |
| "`for...of` vs `for...in`" | [es6-features](es6-features/README.md) |

## Async & Event Loop

| If asked… | Go to |
|---|---|
| "Callbacks → promises → async/await evolution" | [async-javascript](async-javascript/README.md) |
| "Promise combinators: all / allSettled / race / any" | [async-javascript](async-javascript/README.md) |
| "Sequential vs parallel awaits (the slow waterfall)" | [async-javascript](async-javascript/README.md) |
| "Implement promisify / retry / Promise.all polyfill" | [async-javascript](exercises.md) |
| "Explain the event loop" | [event-loop](event-loop/README.md) |
| "Predict-the-output: setTimeout vs Promise ordering" | [event-loop](event-loop/README.md) |
| "Microtasks vs macrotasks" / starvation / `setTimeout(0)` isn't 0ms | [event-loop](event-loop/README.md) |

## Modern Syntax & Robustness

| If asked… | Go to |
|---|---|
| "Destructuring / spread / rest" (incl. swap two variables) | [es6-features](es6-features/README.md) |
| "Map/Set vs object/array — when?" | [es6-features](es6-features/README.md) |
| "`?.` optional chaining / `??` nullish coalescing (vs `\|\|`)" | [null-and-undefined](null-and-undefined/README.md) + [es6-features](es6-features/README.md) |
| "`finally` always runs?" / return-in-finally trap | [error-handling](error-handling/README.md) |
| "Error handling in async code" / unhandled rejections | [error-handling](error-handling/README.md) |
| "Custom error classes" / error wrapping (`cause`) | [error-handling](error-handling/README.md) |
| "Parse JSON without throwing" (safeParse pattern) | [error-handling](exercises.md) |

---

## The 10 most-asked, ranked by frequency

1. Closures + loop bug — [scope-and-closures](scope-and-closures/README.md)
2. Event loop ordering puzzle — [event-loop](event-loop/README.md)
3. `var` vs `let`/`const`, hoisting, TDZ — [scope-and-closures](scope-and-closures/README.md)
4. `this` binding rules + arrows — [this-and-prototypes](this-and-prototypes/README.md)
5. Debounce/throttle/promise polyfills (live coding) — [functions](exercises.md), [async-javascript](exercises.md)
6. Shallow vs deep copy — [objects-and-references](objects-and-references/README.md)
7. Promises + async/await + combinators — [async-javascript](async-javascript/README.md)
8. Immutability in React state updates — [immutability](immutability/README.md)
9. `==` vs `===` + coercion — [primitives-and-types](primitives-and-types/README.md)
10. Array methods from scratch (map/reduce/filter) — [arrays-and-array-methods](arrays-and-array-methods/README.md)

## One-evening sprint (interview tomorrow)

1. Read all 12 cheatsheets (~15 min).
2. Answer Recall Questions aloud; flag misses.
3. Re-read only missed sections' READMEs (~30 min).
4. Redo these exercises blind: closure counter, debounce, Promise.all polyfill, one event-loop puzzle, immutable nested update (~45 min).
