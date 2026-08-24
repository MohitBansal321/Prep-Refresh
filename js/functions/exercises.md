# Exercises — Functions

Work through these in order. Write each solution as a small standalone `.js` file (e.g. `once.js`) and run it with `node <file>.js`. Compare your behavior against the Acceptance criteria — they are the spec. After attempting each one, check the concept in `README.md` / `code.js` only if you get stuck.

> **Rule of this repo:** these exercises are deliberately solution-free. The learning happens when you build it yourself and hit the walls.

---

## Easy

### E1. `once(fn)` — run at most one time

**Requirements**
- `once(fn)` returns a new function.
- The first call invokes `fn` with all arguments and returns its result.
- Every subsequent call returns the *same* first result without invoking `fn` again.
- Track the call state with a closure variable.

**Acceptance**
```js
let calls = 0;
const init = once(() => { calls++; return "ready"; });
init(); // "ready"
init(); // "ready"
init(); // "ready"
// calls === 1
```

**Reflection question:** Where does the "already ran" state live, and why is it invisible from outside `once`?

### E2. `makeAdder(n)` — function factory

**Requirements**
- `makeAdder(n)` returns a function that adds `n` to its argument.
- No global/module-level state allowed; everything must live in parameters/closure.

**Acceptance**
```js
const add5 = makeAdder(5);
const add10 = makeAdder(10);
add5(3);  // 8
add10(3); // 13
add5(0);  // 5
```

**Reflection question:** Why do `add5` and `add10` remember different values even though they come from the same factory?

---

## Medium

### M1. `debounce(fn, waitMs)` — trailing-edge debounce

**Requirements**
- `debounce(fn, waitMs)` returns a wrapped function.
- Each call resets a timer; `fn` runs only after `waitMs` milliseconds pass with **no further calls**.
- The wrapper forwards the latest call's arguments to `fn`.
- Use `setTimeout`/`clearTimeout`; no external libraries.

**Acceptance**
```js
let saved = null;
const save = debounce(name => { saved = name; }, 100);
save("a");
save("b");
save("c");          // after ~150ms: saved === "c", fn ran exactly once
```

**Reflection question:** Which closure variables must survive between calls of the wrapper, and what would break if any of them were declared inside it instead?

### M2. `throttle(fn, waitMs)`

**Requirements**
- Like debounce but opposite: the wrapped function runs **at most once per `waitMs` window**, on leading edge (first call fires immediately).
- Calls during the cooldown are dropped (or queued — your choice, document which you chose).

**Acceptance**
```js
let count = 0;
const ping = throttle(() => count++, 100);
ping(); ping(); ping(); // immediately: count === 1
// after 150ms: count still === 1
// after another full window passes and a new ping(): count === 2
```

**Reflection question:** In one sentence each: when would a UI want debounce (search box?) vs throttle (scroll handler?).

---

## Hard

### H1. `curry(fn)` — full curry for known arity

**Requirements**
- `curry(fn)` returns a curried version of a regular function: calling it with fewer arguments than `fn.length` returns a new function that remembers them.
- Once enough total arguments have accumulated, invoke `fn` with all of them.
- Must support partial calls in multiple steps (`curried(1)(2)(3)` and `curried(1, 2)(3)` both work).
- Hint: rest parameters + recursion; check `fn.length` or count collected args.

**Acceptance**
```js
function volume(w, h, d) { return w * h * d; }
const cVolume = curry(volume);
cVolume(2)(3)(4);    // 24
cVolume(2, 3)(4);    // 24
cVolume(2)(3, 4);    // 24
cVolume(2, 3, 4);    // 24
```

**Reflection question:** What does this exercise assume about `fn` being pure or at least effect-free, and why?

### H2. `pipe(...fns)` and `compose(...fns)` — variadic pipelines

**Requirements**
- Both take N functions and return a single function of one value.
- `pipe(f, g, h)(x)` applies left-to-right: `h(g(f(x)))`.
- `compose(f, g, h)(x)` applies right-to-left: `f(g(h(x)))`.
- Build `compose` using `pipe`, or vice versa — don't duplicate logic.
- Bonus: support an initial-value argument in a `reduceWith(init, ...fns)` variant.

**Acceptance**
```js
const trim  = s => s.trim();
const lower = s => s.toLowerCase();
const shout = s => s + "!";

pipe(trim, lower, shout)("  HeLLo  ");   // "hello!"
compose(shout, lower, trim)("  HeLLo  "); // "hello!"
```

**Reflection question:** Your `reduce` accumulator here is a value flowing through functions — how is that structurally identical to the memoize cache lookup loop, and how is it different?

---

## Self-check before moving on

Can you explain, without looking:

1. Why every one of these solutions relies on closures?
2. Which of `once`, `debounce`, `curry`, `pipe` are higher-order in which direction (take a fn, return a fn, or both)?
3. Which ones require their input function to be pure to behave correctly?
