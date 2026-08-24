# Functions in JavaScript

## The Big Idea

In most languages, a function is a special thing the compiler treats differently. In JavaScript, a function is just a **value** — a first-class citizen — exactly like the number `5` or the string `"hello"`. You can store it in a variable, put it in an array or object, pass it to another function, and return it from a function.

```js
const greet = function () { return "hi"; }; // stored in a variable
const ops = [greet, Math.max];              // inside arrays
const obj = { handler: greet };             // as object properties
```

> **Term: First-class function.** A language has *first-class functions* when functions can be treated as values: assigned, passed, and returned like any other value. This single property is what enables callbacks, higher-order functions, closures-based patterns, and most of modern JS style.

Everything else in this module — hoisting differences, arrow vs regular, `map`/`filter`/`reduce`, memoization — flows out of this one fact. If interviews ask "what does 'functions are first-class' mean?", this is the whole answer: **they're values**.

## Three Ways to Define a Function

### 1. Function declaration

Starts with the `function` keyword and **must have a name**. It is a complete statement.

```js
function add(a, b) {
  return a + b;
}
```

### 2. Function expression

A function appearing where a *value* is expected (right side of `=`, argument position, inside an array…). It may be named or anonymous; the name is only visible *inside* the function (useful for recursion).

```js
const subtract = function (a, b) {
  return a - b;
};

const fact = function factorial(n) {   // named expression
  return n <= 1 ? 1 : n * factorial(n - 1); // name usable here only
};
```

### 3. Arrow function

Concise syntax introduced in ES6. Not sugar for a regular function — it genuinely lacks several features regular functions have (see next section).

```js
const multiply = (a, b) => a + "*" + b === "x" ? 0 : a * b; // don't write this
const multiply = (a, b) => a * b;      // implicit return of one expression
const double = x => x * 2;             // parens optional with one param
const noop = () => {};                 // empty body needs braces
const make = () => ({ id: 1 });        // returning an object literal needs ()
```

> **Gotcha:** `(x) => ({ ok: true })` — wrapping the object literal in parentheses is required, otherwise `{ }` is parsed as the function *body*, not an object literal. Classic interview trap.

## Hoisting Differences (the interview favorite)

**Declarations are fully hoisted**: the entire function is available before the line that defines it runs.

```js
console.log(declared()); // "works!" — no error
function declared() { return "works!"; }
```

**Expressions follow variable hoisting rules**, not function hoisting. A `var`-assigned expression is hoisted but `undefined`; a `let`/`const` one sits in the temporal dead zone until its line executes.

```js
console.log(typeof exprFn); // "undefined" (var case) — not callable yet
var exprFn = function () {};

callLater(); // ReferenceError — let/const are in TDZ
const callLater = () => console.log("later");
```

So the rule to memorize:

| Form | Usable before its definition? | Why |
|------|------------------------------|-----|
| Declaration | ✅ Yes, anywhere in scope | Whole binding hoisted |
| Expression (`var`) | ⚠️ Binding hoisted but `undefined` → TypeError if called | Variable hoisting only |
| Expression (`const`/`let`) | ❌ No — TDZ until definition | Let/const semantics |
| Arrow (always an expression) | ❌ Same as expression above | Arrows can't be declared |

Practical takeaway: declarations give you flexibility (define helpers at the bottom, call at top); expressions/arrows give you predictability (nothing exists before its line). Most modern style guides prefer `const fn = ...` precisely so usage-below-definition is enforced.

## Arrow vs Regular Functions

This is asked in nearly every JS interview, usually phrased as *"when would you NOT use an arrow?"* Four concrete differences:

1. **No own `this`.** An arrow's `this` is lexically inherited from the enclosing scope and cannot be rebound with `.call`, `.apply`, or `.bind`. A regular function's `this` depends on how it's called. This deserves its own deep dive — see `js/this-and-prototypes/` for the full treatment of `this` binding rules.
2. **No `arguments` object.** Arrows must use rest parameters (`...args`) instead — which is arguably better anyway.
3. **Not constructable.** `new arrowFn()` throws `TypeError: arrowFn is not a constructor`. Regular functions can be constructors.
4. **No `prototype` property.** Regular functions automatically get a `.prototype` object (used by `new`); arrows simply don't have one.

Quick demonstration:

```js
function regular() {}
const arrow = () => {};

regular.prototype; // {} — exists
arrow.prototype;   // undefined
new regular();     // fine
new arrow();       // TypeError: arrow is not a constructor
```

**Rule of thumb:** use arrows everywhere *except* when you need (a) your own `this` — e.g., object methods that read `this.name`, class methods, DOM event handlers — or (b) to construct instances. Callbacks passed to `map`, `setTimeout`, promise chains: arrow territory.

## Parameters: Defaults, Rest, Destructuring

Modern parameter handling removed most of the old `arguments.length` gymnastics.

**Default parameters** evaluate only when the argument is `undefined` (passing `null` does NOT trigger the default):

```js
function connect(host = "localhost", port = host === "localhost" ? 5432 : 8080) {
  return `${host}:${port}`;
}
connect();               // "localhost:5432"
connect("db.internal");  // "db.internal:8080"
connect(null);           // "null:8080" — null skips nothing, becomes the value!
```

Note defaults can reference earlier parameters, which makes dependent defaults trivial.

**Rest parameters** collect remaining arguments into a real array:

```js
function sum(first, ...rest) {
  return rest.reduce((acc, n) => acc + n, first);
}
sum(1, 2, 3, 4); // 10 — rest is [2, 3, 4], an actual array
```

Rest must be last, there can be only one, and it replaces the older `arguments`-slicing pattern cleanly. Also note the related spread *call* syntax: `sum(...[1, 2, 3])` expands an array into arguments.

**Destructured parameters** let complex options objects declare their shape right in the signature:

```js
function createUser({ name, role = "member", active = true }) {
  return `${name} (${role}, ${active ? "active" : "inactive"})`;
}
createUser({ name: "Ada", role: "admin" }); // "Ada (admin, active)"
```

Combine all three: `function api(path, { method = "GET", retries = 3 } = {})` — destructuring with defaults, and the whole options object itself defaulting to `{}` so `api("/users")` still works.

## The `arguments` Object

Every **regular** (non-arrow) function gets an array-*like* `arguments` object holding all passed args. Array-like means it has `.length` and index access but none of the array methods:

```js
function legacy() {
  arguments.length;    // 2
  arguments[0];        // first arg
  // arguments.map(...)  ← TypeError: not a function
  Array.from(arguments).map(x => x * 2); // convert first
}
legacy(3, 4);
```

Arrows have no `arguments` of their own — referencing it inside an arrow resolves to the enclosing (regular) function's `arguments`, a subtle bug source. Modern code should prefer rest params: they're real arrays, they name intent, and they work everywhere.

## IIFE — Immediately Invoked Function Expression

Wrap a function expression in parens and call it on the spot. Before `let`/`const` existed, this was THE way to create private scope:

```js
const counterModule = (function () {
  let count = 0;                    // private — invisible outside
  return {
    increment() { return ++count; },
    get() { return count; },
  };
})();
counterModule.increment(); // 1
counterModule.count;       // undefined — truly private
```

The outer parens force the parser to treat `function` as an expression rather than start of a declaration. Today modules do this job, but you'll still meet IIFEs in async wrappers (`(async () => { ... })()`), one-off setup code, and legacy codebases — and the closure-created privacy trick above is exactly how memoization works too.

## Pure Functions vs Side Effects

> **Term: Side effect.** Anything a function does that affects the world outside itself: mutating external state, writing to console/disk/network, throwing, reading mutable globals, calling non-pure functions.

A **pure function** takes inputs and returns outputs — nothing else changes, and the same input always yields the same output.

```js
// PURE — output depends only on inputs, touches nothing outside
const total = (price, tax) => price + price * tax;

// IMPURE — three separate sins
let calls = 0;
function badTotal(price, taxRate) {
  calls++;                        // mutates external state
  console.log("computing");       // I/O side effect
  return price + price * taxRate * Math.random(); // nondeterministic!
}
```

Why care? Pure functions are **cacheable** (same input → same output, so results can be memoized), **testable** (no setup/teardown), **parallelizable**, and **predictable under refactoring**. Impure code isn't evil — I/O has to happen somewhere — the goal is to *concentrate* effects at the edges and keep core logic pure.

## Higher-Order Functions

A **higher-order function (HOF)** is a function that either takes a function as an argument, returns a function, or both. Because functions are values, this falls out naturally.

```js
// Takes a function:
[1, 2, 3].map(n => n * 10);            // [10, 20, 30]
[1, 2, 3, 4].filter(n => n % 2 === 0); // [2, 4]
[1, 2, 3].reduce((acc, n) => acc + n, 0); // 6

// Returns a function:
const greeter = greeting => name => `${greeting}, ${name}!`;
greeter("Hello")("World"); // "Hello, World!"
```

`map`/`filter`/`reduce` are the daily-driver HOF trio: `map` transforms each element, `filter` keeps some, `reduce` folds everything into one value. They replace manual loops with declarative pipelines and are the most common place you pass anonymous arrows.

Functions returning functions open the door to **partial application** and configuration-style APIs — and to the two patterns below.

## Function Composition

Small pure functions snap together: feed one's output into the next's input. Instead of nesting calls (`f(g(h(x)))`) we build pipelines:

```js
const compose = (f, g) => x => f(g(x)); // right-to-left
const pipe = (...fns) => x => fns.reduce((v, fn) => fn(v), x); // left-to-right

const trim = s => s.trim();
const lower = s => s.toLowerCase();
const shout = s => s + "!";

pipe(trim, lower, shout)("  HeLLo  "); // "hello!"
```

Composition only works well when the pieces are **pure** and unary-ish (one input → one output). This idea scales up to middleware chains, RxJS streams, Redux reducers, Express handlers — same principle, bigger machinery.

## Memoization (a Closures Teaser)

**Memoization** caches a function's past results keyed by its arguments, so repeated calls skip recomputation. The cache must survive between calls — and that's exactly what a **closure** gives us: an inner function that keeps a reference to variables from where it was defined.

```js
function memoize(fn) {
  const cache = new Map();          // lives on via closure
  return function (...args) {
    const key = JSON.stringify(args);
    if (!cache.has(key)) {
      cache.set(key, fn.apply(this, args)); // compute once
    }
    return cache.get(key);          // every repeat call is O(1)
  };
}
```

Two things worth noticing: the returned function *closes over* `cache` and even `fn` — neither exists outside `memoize`, yet both persist across calls; and the approach only pays off for **pure** functions (an impure function's cached result would be stale or wrong). Full runnable demo in `code.js`.

Memoized Fibonacci turns an exponential-time recursion into linear time — the classic interview illustration of trading memory for speed.

## Interview Checklist

- What does "first-class functions" mean? (They're values — assign/pass/return.)
- Declaration vs expression vs arrow — and which forms are hoisted how?
- Name the four arrow-vs-regular differences (`this`, `arguments`, `new`, `prototype`).
- When does a default parameter kick in? (Only on `undefined` — not `null`.)
- Rest params vs the `arguments` object — why prefer rest?
- What is a pure function and why do we want them?
- What makes a function "higher-order"? Give three built-in examples.
- Explain memoization and why it requires purity + closures.
