# Scope & Closures — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Core JavaScript language mechanics (scope model + function behavior). |
| **Lexical Scope** | Scope is decided at **write-time** by where code sits in the source. Inner functions can see outward, never inward. "Scope is authored, not executed." (`this` is the exception — call-time.) |
| **var vs let/const** | `var`: function-scoped, hoisted and initialized to `undefined`, redeclarable. `let`/`const`: block-scoped, hoisted but uninitialized, no redeclare, TDZ until their line runs. Default to `const`, then `let`; never new `var`. |
| **TDZ** | Temporal Dead Zone: from scope entry to the `let`/`const` declaration line, reading the binding throws `ReferenceError: Cannot access ... before initialization`. Even `typeof` throws. A same-named inner declaration shadows the outer one *including during its own TDZ*. |
| **Shadowing** | An inner binding with the same name hides the outer one entirely; lookup stops at the nearest match. The shadowed variable is unreachable from inside. Assignment walks the chain too; undeclared assignment creates a global (non-strict). |
| **Closure Definition** | Function **+ its live lexical environment**, kept alive as long as any reference to the function exists. Captures *bindings*, not copied values — sees later mutations. Each factory call creates a fresh, independent environment. |
| **Classic Uses** | Counter factories · private state / module pattern (IIFE API over hidden vars) · event handlers capturing setup data · memoize (cache in closure) · debounce/throttle (timer id in closure) · once() · partial application. See [js/functions](../functions/README.md). |
| **Loop Bug Fix** | `for (var i...)` = ONE shared binding → all delayed callbacks print final value (`3 3 3`). Fix: `let` (fresh binding per iteration) or IIFE copy `((j) => ...)(i)`. Delay length is irrelevant to the values. |
| **Memory Cost** | Closure keeps captured variables reachable → they live as long as the closure does. Engines retain only referenced variables. Leak risk = long-lived closures holding big data (global listeners, unbounded caches, DOM nodes); remove listeners / don't capture what you don't need. |
| **Gotchas** | `typeof` doesn't save you from TDZ · inner `let x` poisons whole inner scope (TDZ shadowing) · closures capture references, so mutation is visible · loop callbacks see final value · accidental global via non-strict undeclared assignment. |
| **Related Topics** | [js/functions](../functions/README.md) (memoize/debounce impls) · [js/this-and-prototypes](../this-and-prototypes/README.md) (`this` vs lexical scope) · [js/async-javascript](../async-javascript/README.md) (why timers wait for sync code) |

### Skeleton
```js
function makeCounter() {
  let count = 0;                    // private state — unreachable outside
  return {
    increment() { return ++count; }, // each method closes over SAME count
    value()      { return count; },
  };
}
const a = makeCounter(), b = makeCounter(); // independent states

// THE loop bug:
for (var i = 0; i < 3; i++) setTimeout(() => console.log(i), 0); // 3 3 3
for (let j = 0; j < 3; j++) setTimeout(() => console.log(j), 0); // 0 1 2
```

### Remember In One Sentence
> **A closure is a function bundled with its live lexical environment, so its private variables survive the outer call — and since scope is written, not called, `var`'s one shared binding makes async loops print the final value while `let`'s per-iteration binding fixes it.**

### Two Facts People Get Wrong
- Closures capture a **snapshot of the value? No** — they capture the live *binding*; later mutations are visible through the closure.
- Hoisting means `let`/`const` are not hoisted? **They are hoisted too** — just left uninitialized, which is exactly why reading them early throws (the TDZ).

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. What does "lexical scoping" mean, and what determines which variables a function can access?
2. Contrast function scope vs block scope — which declarations respect blocks, and which ignore them?
3. All three of `var`/`let`/`const` are hoisted. What actually differs among them?
4. Define the Temporal Dead Zone precisely, and explain why even `typeof` throws inside it.
5. How can an inner `let msg` cause a `ReferenceError` when an outer `msg` already exists?
6. Give the precise definition of a closure, and state whether it captures values or bindings.
7. Why does `for (var i...)` + `setTimeout` print `3 3 3`, and name two different fixes?
8. Describe the memory implication of closures and one realistic leak scenario involving event listeners.
