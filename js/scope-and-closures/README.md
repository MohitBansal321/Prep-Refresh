# Scope and Closures

## Intent

Understand **where JavaScript decides which variables a piece of code can see** (lexical scope), **how `var`, `let`, and `const` differ in that system**, and **what a closure actually is** — the single most-tested JavaScript concept in interviews.

If you can explain closures cold — definition, mechanism, one classic bug, one memory implication — you will pass most JS fundamentals screens.

---

## 1. Lexical Scoping: Scope Is Decided at Write-Time

JavaScript uses **lexical (static) scoping**: the set of variables a function can access is determined by *where the function is written in the source code*, not by where or how it is called.

```js
const greeting = "hello";

function outer() {
  const audience = "world";
  function inner() {
    // Written inside outer() → can see greeting AND audience,
    // no matter who later calls inner().
    console.log(`${greeting}, ${audience}`);
  }
  return inner;
}

const fn = outer();   // outer() has finished running here
fn();                 // "hello, world" — still works!
```

> **Term: Lexical environment.** The internal record of local variables + a reference to the enclosing lexical environment. Every function, block, and script has one. "Lexical" literally means "having to do with the text" — the nesting you can see by reading the file.

The key consequence: when `outer()` finishes, its local variables are **not necessarily destroyed**, because `inner` still holds a reference to that environment. That reference is the closure (Section 5).

Contrast with **dynamic scoping** (which JS does *not* have): under dynamic scoping, a function would see the variables of whoever called it. If that were true, calling `fn()` from anywhere would change what it prints. It does not — JS resolves names up the *source-code* chain, never the call stack. (The closest thing to dynamic behavior is `this`, which *is* determined by the call — but that is a separate mechanism, covered in [js/this-and-prototypes](../this-and-prototypes/README.md).)

Interview phrasing worth memorizing: *"Scope is authored, not executed."*

---

## 2. Function Scope vs Block Scope

Before ES2015 there was essentially one scope kind for variables: the **function**.

- **Function scope:** a variable declared in a function is visible everywhere in that function — including before its declaration line and inside nested blocks.
- **Block scope:** a variable declared in `{ ... }` (an `if`, `for`, bare block) is visible only inside those braces.

```js
function demo() {
  if (true) {
    var a = 1;      // function-scoped: escapes the if
    let b = 2;      // block-scoped: trapped inside the if
    const c = 3;    // block-scoped too
  }
  console.log(a);   // 1        — var ignored the block
  console.log(b);   // ReferenceError — b does not exist out here
}

demo();
```

Blocks also exist standalone:

```js
{
  const secret = "hidden";
}
// secret is gone here
```

Functions themselves create scope for *all* declarations. Loops (`for`), conditionals, `try/catch` clauses — these only create scope for `let`/`const`/`class`. This single sentence explains half of all scoping interview questions.

> Note: `let`/`const` in loop bodies create a **fresh binding per iteration** — this is exactly the machinery that fixes the classic loop bug (Section 7).

---

## 3. `var` vs `let` vs `const`: Hoisting and the TDZ

All three declarations are **hoisted** — the declaration is processed before any code in the scope runs. What differs is *what value they carry* and *whether reading them early throws*.

| Declaration | Hoisted? | Initialized to | Read before declaration | Redeclare |
|-------------|----------|----------------|-------------------------|-----------|
| `var`       | yes      | `undefined` immediately | `undefined` (no error) | allowed |
| `let`       | yes      | nothing yet     | `ReferenceError` (TDZ) | not allowed |
| `const`     | yes      | nothing yet     | `ReferenceError` (TDZ) | not allowed; must initialize |

### `var`: hoisted and pre-initialized

```js
console.log(x); // undefined — NOT an error
var x = 5;
```

The engine treats this like:

```js
var x;            // declaration moved to top of function, value = undefined
console.log(x);   // undefined
x = 5;            // assignment stays on the original line
```

Only the *declaration* moves up. Assignments stay put.

### `let`/`const`: hoisted but uninitialized — the Temporal Dead Zone

A `let`/`const` binding exists from the top of its scope, but it is **uninitialized** until the declaration line executes. The span between "scope starts" and "declaration runs" is the **Temporal Dead Zone (TDZ)**. Reading the binding there throws a `ReferenceError`.

```js
{
  // TDZ for `y` starts here
  console.log(y); // ReferenceError: Cannot access 'y' before initialization
  let y = 10;
  // TDZ ends here
}
```

> **Term: Temporal Dead Zone.** The period between entering a scope and executing the `let`/`const` declaration within it. "Temporal," not "spatial": what matters is *when* the line runs, not how many lines away it is.

Why does JS do this? As a safety net. `typeof` famously does not save you either:

```js
console.log(typeof z); // ReferenceError — even typeof hits the TDZ
let z = 1;
```

Common gotcha: TDZ applies per-scope even when an outer variable of the same name exists.

```js
let msg = "outside";
function f() {
  console.log(msg); // ReferenceError! Inner `msg` shadows outer one,
                    // so the lookup stops at the inner TDZ'd binding.
  let msg = "inside";
}
```

Practical rules:
- Default to `const`; switch to `let` when reassignment is genuinely needed.
- Never use `var` in new code — but you must read it fluently, because legacy code and interview puzzles use it.
- `const` freezes the *binding*, not the value: `const arr = []; arr.push(1)` is fine; `arr = []` is not.

---

## 4. Scope Chain and Shadowing

When code reads a name, the engine searches:

1. the current scope's bindings, then
2. the lexically enclosing scope, then
3. outward, scope by scope, until
4. the global scope. Not found → `ReferenceError`.

This linked list of environments is the **scope chain**. Assignment follows the same chain — unless the target is undeclared in non-strict mode, which silently creates a global (a bug generator; `"use strict"` turns it into an error).

**Shadowing:** an inner scope declares a name that also exists outside. Inside, the inner binding *wins* completely — there is no way to reach the shadowed one except by leaving the scope.

```js
const count = 100;

function run() {
  const count = 1;               // shadows the outer `count`
  {
    const count = 2;             // shadows again, one level deeper
    console.log(count);          // 2
  }
  console.log(count);            // 1
}

run();
console.log(count);              // 100 — outer untouched
```

Shadowing is normal and often good (local independence), but accidental shadowing — especially of parameters or loop variables — is a real bug class. A linter's `no-shadow` rule exists because humans misread shadowed code constantly.

Related trap: **variable capture happens at call time, not declaration time** — a function reads whatever value the binding holds *when it executes* (Section 7 shows this biting hard).

---

## 5. What a Closure Actually Is

> **Definition:** A closure is the combination of a **function** and the **lexical environment** it was created in. The function carries a live reference to the variables of the scopes where it was written, so those variables survive after their creating function returns.

Two mental models, pick whichever sticks:

1. **Backpack model.** When a function is born, it packs a backpack containing references to the surrounding variables it might need. Wherever the function travels — returned, passed as callback, stored in an array — the backpack goes along.
2. **Live link, not snapshot.** The closure captures *variables* (bindings), not the *values* they had at creation time. If the closed-over variable changes later, the function sees the new value.

```js
function makeGreedier() {
  let level = 0;
  return {
    bump() { level += 1; },
    get() { return level; }
  };
}

const g = makeGreedier();
g.bump();
g.bump();
console.log(g.get()); // 2 — same live `level`, mutated twice
```

Crucially, **each invocation creates a fresh environment**:

```js
function makePair() {
  let n = 0;
  return [() => ++n, () => n]; // both close over the SAME n
}

const pairA = makePair();
const pairB = makePair();
pairA[0]();          // 1 — bumps A's n
console.log(pairB[1]()); // 0 — B's n untouched
```

That last property — independent state per factory call — is the entire foundation of factories, private state, memoization, debounce, and once-style utilities.

A precise interview answer, in order: *(a)* functions remember the environment where they were defined; *(b)* that environment persists as long as any function referencing it survives; *(c)* each call of the enclosing function produces a separate environment; *(d)* closures capture references, not copied values.

---

## 6. Classic Closure Patterns

### Counter factory — private state without classes

```js
function makeCounter() {
  let count = 0;                 // invisible from outside — no key can reach it
  return {
    increment() { return ++count; },
    decrement() { return --count; },
    value() { return count; }
  };
}

const c = makeCounter();
c.increment(); // 1
c.increment(); // 2
c.decrement(); // 1
// c.count === undefined — there is NO way to touch `count` directly
```

Compare with an object literal `{ count: 0 }`: anyone can write `obj.count = 9999`. With a closure, the variable is reachable **only through the exposed methods** — genuine encapsulation, predating `class` private fields and working everywhere.

### Module pattern — a whole API over hidden state

```js
const wallet = (() => {
  let balance = 0;                          // private
  return {
    deposit(n) { balance += n; return balance; },
    withdraw(n) { return balance >= n ? (balance -= n) : null; },
    getBalance() { return balance; }
  };
})();

wallet.deposit(100);      // 100
wallet.withdraw(30);      // 70
wallet.getBalance();      // 70
// wallet.balance === undefined — state is sealed inside the IIFE
```

This is the pre-module-system way to get privacy in JS, and it is *still* how people build small self-contained pieces. See Section 8.

### Closures in the wild — you write them daily

Every callback closes over something:

- **Event handlers:** `button.addEventListener("click", () => alert(name))` — the handler remembers `name` long after the setup code finished.
- **Module pattern / namespaces:** the IIFE above.
- **Memoize:** cache lives in a closure; the wrapper checks cache before invoking the wrapped fn.
- **Debounce/throttle (teaser):** timer id lives in a closure shared by every wrapper call. Full implementation in [js/functions](../functions/README.md).
- **Partial application:** `const logInfo = log.bind(null, "INFO")`-style factories — `makeLogger("INFO")` returning `(msg) => ...` closes over the prefix.
- **Iterators/generators and lazy sequences:** position lives in closure state.

Rule of thumb: **any function returned from another function is almost certainly a closure**, and the reason it exists is usually to keep some state alive.

---

## 7. THE Loop Bug: `var` + `setTimeout`

The most famous closure question ever asked. Predict the output:

```js
for (var i = 0; i < 3; i++) {
  setTimeout(() => console.log(i), 0);
}
// Expected 0 1 2. Actual output:
// 3
// 3
// 3
```

Why:

1. `var i` is **one single function-scoped binding** for the whole loop — all three callbacks share it.
2. Callbacks run **after** the loop finishes (timers fire after synchronous code completes).
3. By then `i === 3`, and since closures capture the *live binding*, all three print `3`.

The fix is one keyword:

```js
for (let i = 0; i < 3; i++) {
  setTimeout(() => console.log(i), 0);
}
// 0
// 1
// 2
```

Why `let` changes everything: in a `for` loop, `let` creates a **new binding per iteration**, copying the current value forward. Each callback closes over *its own* `i`, frozen at that iteration's value.

Pre-ES2015 fix (worth knowing for legacy interviews): wrap in a function to force a fresh scope —

```js
for (var i = 0; i < 3; i++) {
  ((j) => setTimeout(() => console.log(j), 0))(i); // IIFE captures a copy
}
```

Variants interviewers use: swapping in `setTimeout(..., 1000)` (same result — delay length is irrelevant, ordering is), `process.nextTick`/promises instead of timers (same result), or asking you to produce `0 1 2` *without changing `var`* (the IIFE above).

---

## 8. Memory Implications

Closures keep their captured environments **alive as long as the closure itself is reachable**. That is a feature (private state) and a cost (retained memory):

- **Normal case: negligible.** Engines optimize heavily — typically only the variables *actually referenced* by the inner function are retained, not the whole enclosing frame.
- **Real leak pattern:** a long-lived closure capturing something big.

```js
function attachHandler(el) {
  const hugeData = loadHugeReport();          // MBs
  el.addEventListener("click", () => {        // tiny handler…
    console.log("clicked");
    // …but if it (or anything it reaches) references hugeData,
    // hugeData cannot be collected while the element lives.
  });
}
```

Rules of thumb:

1. **Long-lived objects holding short-lived closures** (global event listeners, caches keyed forever) are where closure leaks live. Remove listeners you no longer need (`AbortController`, `removeEventListener`).
2. **Don't capture what you don't need** — extract just the small value into a local before defining the callback.
3. Closures over DOM nodes keep whole subtrees alive; null out references in teardown paths.
4. In Node, module-level closures persist for process lifetime — fine for config/singletons, dangerous for unbounded accumulators (an ever-growing array inside a closure *is* a leak).

Interview sound bite: *"Garbage collection decides lifetime by reachability — a closure makes its captured variables reachable, so they live exactly as long as the closure does."*

---

## Summary

- **Lexical scope:** visibility decided by where code is written; nested functions can see outward, never inward.
- **Function vs block scope:** `var` ignores blocks; `let`/`const` respect them.
- **Hoisting:** all declarations move up; `var` arrives initialized to `undefined`, `let`/`const` arrive uninitialized (TDZ until their line runs).
- **Shadowing:** nearest declaration wins; the shadowed binding is unreachable inside.
- **Closure:** function + its live lexical environment; state survives the outer call and is private to the closure.
- **Loop bug:** one shared `var` binding + delayed execution → all callbacks see the final value; `let` gives a fresh binding per iteration.
- **Memory:** closures extend lifetimes; leaks come from long-lived holders of big captured data.

## Key Takeaways

1. Scope is authored at write-time ("lexical"), never determined by who calls whom.
2. Only functions (and modules) create scope for `var`; blocks create scope for `let`/`const`.
3. All declarations hoist; the difference between `var` and `let`/`const` is *initialization timing* — hence the TDZ.
4. A closure captures the live binding, not a copy of the value.
5. Each call to a factory function yields independent closed-over state — the basis of counters, memoize, debounce, once.
6. Closure variables are unreachable from outside: privacy without `class`.
7. `for (var...)` + async callback = everyone sees the final value; `let` fixes it by per-iteration binding.
8. Long-lived closures over large data are the leak pattern to watch for.

---

## Further Reading

- MDN — [Closures](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Closures)
- *You Don't Know JS: Scope & Closures* — Kyle Simpson (free online; the definitive deep dive)
- MDN — [`let` and the temporal dead zone](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/let#temporal_dead_zone_tdz)
- Cross-reference: [js/functions](../functions/README.md) (memoize/debounce implementations), [js/this-and-prototypes](../this-and-prototypes/README.md) (how `this` differs from scope), [js/async-javascript](../async-javascript/README.md) (why timers wait for sync code)
