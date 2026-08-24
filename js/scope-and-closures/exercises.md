# Exercises — Scope and Closures

Work through these in order. Write each solution as a small standalone `.js` file (e.g. `make-counter.js`) and run it with `node <file>.js`. Compare your behavior against the Acceptance criteria — they are the spec. After attempting each one, check the concept in `README.md` / `code.js` only if you get stuck.

> **Rule of this repo:** these exercises are deliberately solution-free. The learning happens when you build it yourself and hit the walls.

---

## Easy

### E1. `makeCounter()` — the classic, from scratch

**Requirements**
- `makeCounter()` returns an object with four methods: `increment()`, `decrement()`, `reset()`, `value()`.
- `increment()`/`decrement()` adjust the count by 1 and return the new value.
- `reset()` sets the count back to 0 and returns 0.
- The count variable must be **completely inaccessible** from outside — no property, no global.

**Acceptance**
```js
const c = makeCounter();
c.increment(); // 1
c.increment(); // 2
c.decrement(); // 1
c.value();     // 1
c.reset();     // 0
c.value();     // 0
console.log(c.count); // undefined
```

**Reflection question:** Why does `c.count` log `undefined` instead of throwing a `ReferenceError`? What is the difference between those two outcomes?

### E2. Predict the output — shadowing

**Requirements**
- Write this snippet **by hand first**, predict every line, then run it to verify.
```js
let x = "global";

function outer() {
  let x = "outer";
  function inner() {
    console.log(x);
  }
  inner();
  return inner;
}

const fn = outer(); // what prints here?
fn();               // and here?
console.log(x);     // and here?
```

**Acceptance**
- Your predicted output matches the actual output for all three logs.
- You can say *why* in one sentence per line before checking the README.

**Reflection question:** When `fn()` runs long after `outer()` finished, where does the engine look for `x` — the call stack of `fn()`, or somewhere else?

---

## Medium

### M1. `once(fn)` — run at most one time

**Requirements**
- `once(fn)` returns a new function with the same behavior as `fn`.
- The first call invokes `fn` with all arguments and caches its return value.
- Every subsequent call returns the cached result without invoking `fn` again.
- All state must live inside the closure — no module-level variables.
- Bonus: preserve `this` so `once(obj.method)()` still works.

**Acceptance**
```js
let calls = 0;
const init = once((a, b) => { calls++; return a + b; });
init(1, 2); // 3   (calls === 1)
init(5, 5); // 3   (cached — calls still === 1)
init(9, 9); // 3   (calls still === 1)
```

**Reflection question:** If you had written `once` using a module-level flag instead of a closure, what would break the moment two different functions were wrapped?

### M2. Fix the loop bug three ways

**Requirements**
- Start from this broken code:
```js
for (var i = 1; i <= 5; i++) {
  setTimeout(() => console.log("tick", i), i * 100);
}
```
- Produce three working versions that each print `tick 1` through `tick 5` (in order):
  1. change the declaration keyword,
  2. keep `var` but wrap the body in an IIFE,
  3. keep `var` but pass the value via a named helper function's parameter.
- Each version must print in the correct order despite staggered delays.

**Acceptance**
- All three versions print exactly: `tick 1` … `tick 5`, each at its own delay.
- In version 3, explain out loud why the helper function creates a new binding while the raw arrow callback did not.

**Reflection question:** Does increasing or decreasing the timeout delay ever change the `var` version's output values? Why not?

### M3. Secret holder with a guess limit

**Requirements**
- `createSecret(secretValue)` returns `{ guess(n), getHintsLeft() }`.
- `guess(n)` returns `"correct"` on the right value; otherwise `"too low"` / `"too high"`.
- Only **3 wrong guesses** are allowed. After that, `guess()` always returns `"locked out"` — even if the next guess is correct.
- `getHintsLeft()` returns how many wrong guesses remain.
- `secretValue` must be unreachable except through `guess` — no way to read it directly.

**Acceptance**
```js
const s = createSecret(42);
s.guess(10);          // "too low"
s.guess(50);          // "too high"
s.getHintsLeft();     // 1
s.guess(60);          // "too high"
s.getHintsLeft();     // 0
s.guess(42);          // "locked out" — even though 42 is correct!
```

**Reflection question:** Which single closure variable carries the "locked" state, and could you derive it from the other variables instead of storing it separately?

---

## Hard

### H1. `memoize(fn)` with cache introspection

**Requirements**
- `memoize(fn)` returns a memoized wrapper for any single-argument function.
- Repeated calls with the same argument return the cached result without re-invoking `fn`; different arguments call through.
- Forward the argument correctly (`this` preservation is a bonus).
- Add methods on the returned function: `cacheSize()` and `clearCache()`.
- The cache itself must not be reachable from outside.

**Acceptance**
```js
let calls = 0;
const slowSquare = (n) => { calls++; return n * n; };
const fast = memoize(slowSquare);
fast(4);            // 16  (calls === 1)
fast(4);            // 16  (calls === 1)
fast(5);            // 25  (calls === 2)
fast.cacheSize();   // 2
fast.clearCache();
fast.cacheSize();   // 0
fast(4);            // 16  (calls === 3)
```

**Reflection question:** What happens if `fn` is nondeterministic (e.g. reads the current time)? Is that a flaw in `memoize` or a contract violation by the caller?

### H2. Predict the output — closure puzzle gauntlet

**Requirements**
- For each snippet below: write down your prediction **before running anything**, then run and compare. For every miss, write one sentence naming the exact concept you got wrong.

```js
// Puzzle A
const fns = [];
for (var i = 0; i < 3; i++) fns.push(() => i);
console.log(fns.map((f) => f()));

// Puzzle B
const gns = [];
for (let j = 0; j < 3; j++) gns.push(() => j);
console.log(gns.map((f) => f()));

// Puzzle C
function outer() {
  let n = 0;
  const bump = () => ++n;
  return [bump, () => n];
}
const [bumpA, readA] = outer();
const [bumpB] = outer();
bumpA(); bumpA(); bumpB();
console.log(readA(), bumpB());

// Puzzle D
var x = "v";
(function () {
  console.log(x);
  var x = "local";
  console.log(x);
})();
```

**Acceptance**
- All four predictions written down before execution; actual outputs match or misses are annotated with the precise concept (hoisting/TDZ, shared vs fresh binding, capture-by-reference).

**Reflection question:** Puzzles A and B differ by one keyword yet behave oppositely — can you state the engine-level mechanism (per-iteration binding creation) without looking?

### H3. Rate limiter — closures as a state machine

**Requirements**
- `createRateLimiter(maxCalls, windowMs)` returns a wrapped function `limited(fn)`.
- Calling `limited(...args)` invokes `fn(...args)` only if fewer than `maxCalls` calls happened in the last `windowMs`; otherwise it returns `"RATE LIMITED"` without invoking `fn`.
- Track timestamps in a closure array; prune entries older than the window on every call.
- Expose `remaining()` returning how many calls are left in the current window.
- Test with `maxCalls = 2, windowMs = 300`: two immediate calls succeed, a third fails, after 350 ms another succeeds.

**Acceptance**
```js
const limited = createRateLimiter(2, 300)(() => "ran");
limited(); // "ran"
limited(); // "ran"
limited(); // "RATE LIMITED"
// ...after 350 ms:
limited(); // "ran"
```

**Reflection question:** Two separate rate limiters created from the same factory never interfere with each other. Where exactly does that isolation come from, and which earlier exercise used the identical mechanism?
