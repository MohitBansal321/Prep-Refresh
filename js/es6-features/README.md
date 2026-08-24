# ES6+ Features — The Everyday Syntax

ES6 (2015) is the dividing line in JavaScript interviews. Before it, JS code looks like a
different language; after it, everything you write daily — `let`/`const`, template literals,
destructuring, spread/rest, arrow functions, `Map`/`Set`, modules — falls into place. This
module is the **syntax toolbox**: each feature is short, but interviewers love them because
they hide real semantics (shallow copies, iteration traps, default-value timing).

This file is a tour of what you must know cold. Deeper topics get pointers to their own
modules rather than being re-taught here.

---

## 1. `let` and `const` — Quick Recap

You already met these properly in [js/scope-and-closures](../scope-and-closures/README.md) —
block scoping, the TDZ, why `var` hoists and initializes to `undefined`. The one-line recap:

- **`const`** = *binding* is constant, not the value. You cannot reassign the variable, but
  you **can** mutate an object or array it points to:

```js
const user = { name: "Ada" };
user.name = "Lovelace";   // fine — mutating the object
// user = {};             // TypeError: Assignment to constant variable.
```

- **`let`** = block-scoped and reassignable; no re-declaration in the same block.

House rule for this repo: **default to `const`; reach for `let` only when reassignment is
genuinely needed; never write new `var`.** In interviews, saying "I use `const` by default so
readers know a binding won't be reassigned" signals discipline.

---

## 2. Template Literals

Backtick strings with `${...}` interpolation and true multi-line support:

```js
const name = "Ada";
const score = 91.567;

console.log(`${name} scored ${score.toFixed(1)}%`);
// Ada scored 91.6%

console.log(`line one
line two`);               // real newline, no "\n" or concatenation
```

Why they matter beyond cosmetics:

- **No more `"Hello " + name + ", you have " + n + " items"`** — that style is where
  off-by-one quote bugs live.
- The interpolation runs **any expression**, including function calls and ternaries:
  `` `Status: ${errors.length ? "fail" : "ok"}` ``.
- Tagged templates (`` tag`...` ``) are the power feature behind styled-components and
  GraphQL query tags — recognize the syntax even if you never write one.

Interview angle: template literals do string coercion via `toString()`, so
`` `${null}` `` is `"null"` and `` `${undefined}` `` is `"undefined"` — know those two cold.

---

## 3. Destructuring

Pull fields out of objects/arrays in one pattern. This is the single most-used ES6 feature
in modern codebases — you will read it in every function signature you ever encounter.

### Object destructuring

```js
const user = { id: 7, name: "Ada", role: "admin" };

const { id, name } = user;          // id=7, name="Ada"
const { role: userRole } = user;    // renaming: variable is `userRole`
const { email = "n/a" } = user;     // default when property is undefined
```

Three moves bundled above: plain extraction, **renaming** (`role: userRole`), and
**defaults** (`email = "n/a"`, applied only when the value is `undefined` — not `null`,
not `0`). Rename + default combine: `{ role: r = "guest" } = {}`.

### Array destructuring

```js
const rgb = [255, 128, 0];
const [red, green] = rgb;           // positional — order matters
const [first, , third] = rgb;       // skip middle elements
const [head, ...tail] = rgb;        // head=255, tail=[128, 0]
```

### Nested patterns

Patterns nest arbitrarily deep — this is where messy API responses get tamed:

```js
const response = {
  data: {
    user: { name: "Ada", address: { city: "London" } },
    tags: ["admin", "beta"],
  },
};

const {
  data: {
    user: { name, address: { city } },
    tags: [primaryTag],
  },
} = response;

console.log(name, city, primaryTag);   // Ada London admin
```

Read it inside-out: the *shape* on the left mirrors the shape of the data. Note the trap —
intermediate levels (`data`, `user`) are **not** bound as variables unless you also capture
them (`{ data: { user, user: { name } } }` binds both `user` and `name`).

### Function parameters

Destructuring shines most in parameter lists — functions declare exactly what they need:

```js
function createUser({ name, role = "member", isActive = true }) {
  return { name, role, isActive };
}
createUser({ name: "Ada", role: "admin" });   // call site reads like named args
```

This is effectively "options objects done right," and it self-documents the contract.

---

## 4. Spread and Rest

Same three dots, opposite directions: **spread** expands at a call site / literal;
**rest** collects into a variable.

### Spread

```js
const base = [1, 2];
const extended = [...base, 3];              // [1, 2, 3]

const defaults = { theme: "dark", retries: 3 };
const config = { ...defaults, retries: 5 }; // { theme: "dark", retries: 5 }
```

Later properties win in object spread — that ordering rule is how people express
"defaults first, overrides after."

### The shallow-copy trap (interview favorite)

Spread creates a **shallow** copy: one new level, then references copied as-is.

```js
const original = { profile: { city: "London" }, tags: ["a"] };
const copy = { ...original };

copy.profile.city = "Paris";     // mutates through the shared reference!
console.log(original.profile.city);   // Paris — surprise
copy.tags.push("b");
console.log(original.tags);            // ["a", "b"] — same array
```

The top-level object is genuinely new (`copy !== original`), but `copy.profile` and
`copy.tags` point at **the same inner objects**. For a true deep copy use
`structuredClone(original)` (see below) or a dedicated library. Know how to say this in an
interview: *"spread copies the shape, not the nested values."*

### Rest parameters

Rest collects remaining arguments into a real array (note: it is a genuine array —
unlike old-school `arguments`, you can `.map()`/`.filter()` it directly):

```js
function sum(label, ...numbers) {
  return `${label}: ${numbers.reduce((a, b) => a + b, 0)}`;
}
sum("total", 1, 2, 3);   // "total: 6"
```

Rules worth stating aloud: rest must be the **last** parameter; there can be only one;
in object/array destructuring, rest grabs the leftover keys/elements.

---

## 5. Enhanced Object Literals

Small sugar, everywhere in real code:

```js
const name = "Ada";
const role = "admin";

const user = {
  name,                       // shorthand property ({ name: name })
  role,
  describe() {                // shorthand method (no ": function")
    return `${this.name} (${this.role})`;
  },
  [`key_${1 + 1}`]: "computed key",   // computed property names
};
```

Shorthand properties make `return { name, role }` idiomatic — you will see it in every
modern reducer and factory. Computed keys let you build maps-in-an-object dynamically.

---

## 6. Default Parameters

Evaluated **at call time, left to right**, and can reference earlier parameters:

```js
function greet(name, salutation = `Hello, ${name}!`) {
  console.log(salutation);
}
greet("Ada");                 // Hello, Ada!
greet("Grace", "Ahoy");      // Ahoy
```

Key subtlety: defaults apply only for `undefined`. Passing `null` explicitly does **not**
trigger the default — `greet(null)` prints `Hello, null!`. If you want `null` treated as
missing, coalesce inside the body (`name ?? "stranger"`). Also note defaults replaced the
old `function f(x) { x = x || 10 }` idiom — which was buggy anyway because `0`, `""`, and
`false` all fell through to the fallback.

Arrow functions were covered in depth alongside closures in
[js/scope-and-closures](../scope-and-closures/README.md) — remember the two headline facts:
lexical `this` (no own binding), and no `arguments` object of their own.

---

## 7. Map & Set

Two built-ins people under-use because plain objects/arrays feel familiar.

### Set — unique values

```js
const ids = [3, 1, 3, 2, 1];
const unique = [...new Set(ids)];     // [3, 1, 2]
```

One-liner dedupe is the classic usage, but `Set` also gives you O(1)-average
`has()` membership checks — dramatically better than `arr.includes()` inside a loop
(O(n) per check → O(n²) overall). Use `Set` when the question is *"have I seen this?"*.

### Map — keyed data, any key type

Prefer `Map` over a plain object when:

- Keys are **not strings/symbols** (objects, functions, DOM nodes work as Map keys).
- You need frequent **add/delete** and care about size — `map.size` is O(1);
  counting object keys requires `Object.keys(obj).length`.
- Iteration order and direct iteration matter — a `Map` is iterable in insertion order;
  objects need `Object.entries()` ceremony.
- The key set is dynamic (a cache/registry), not a fixed record shape.

```js
const cache = new Map();
cache.set({ id: 1 }, "result-for-object-key");
cache.set(42, "number key is fine too");

for (const [key, value] of cache) { /* insertion order guaranteed */ }
```

Rule of thumb: **fixed record → object; dynamic registry → Map.**

### WeakMap & WeakRef — the idea

A `WeakMap` holds keys **weakly**: if the key object has no other references, it can be
garbage-collected and its entry vanishes with it. That makes WeakMap the tool for attaching
metadata to objects without preventing their cleanup (private per-instance state, caching
derived data per DOM node). `WeakRef` lets *you* hold a weak reference to a target. Neither
is iterable — you cannot enumerate what's alive — which is the price of not keeping things
alive. Interviews usually stop at "why does WeakMap exist?" — answer: *memory-safe metadata.*

---

## 8. Iterators: `for...of` vs `for...in` — The Classic Trap

These look interchangeable and are not:

- **`for...of`** iterates **values** of anything *iterable* (arrays, strings, Maps, Sets,
  generators).
- **`for...in`** iterates **enumerable string keys** of an object — designed for records,
  not lists.

```js
const langs = ["js", "go", "ts"];

for (const v of langs) console.log(v);   // js, go, ts  (values)
for (const k in langs) console.log(k);   // "0", "1", "2" (string index keys!)
```

The trap bites twice:

1. Using `for...in` on arrays gives you **string indices** plus any extra enumerable
   properties someone attached (`langs.extra = "?"` shows up in `for...in`, never in
   `for...of`).
2. Plain objects have **no iterator**, so `for...of` on `{}` throws — loop their entries
   instead: `for (const [k, v] of Object.entries(obj))`.

Interview phrasing: *"`for...in` walks enumerable property names; `for...of` walks the
iteration protocol's values. Arrays get `for...of`; dictionaries get `Object.keys`/
`entries`."*

Optional chaining (`?.`) and nullish coalescing (`??`) are deliberately **not** covered here
— they belong to [js/null-and-undefined](../null-and-undefined/README.md). Just know they
exist and pair beautifully with defaults.

---

## 9. Modules — `import` / `export` in Brief

ES modules give each file its own scope and an explicit surface area:

```js
// math-utils.js
export const add = (a, b) => a + b;      // named export
export default class Calculator {}       // one default export per module

// app.js
import Calculator, { add } from "./math-utils.js";
import * as utils from "./math-utils.js"; // namespace import
```

Facts worth knowing: exports are **live bindings** (importers see later updates to the
exported variable); imports are hoisted and evaluated once per module (which is why a
module-level singleton works — see the Singleton module's Node-cache discussion);
CommonJS `require` is the older Node system you will still meet in the wild.

---

## 10. Newer Syntax Worth Knowing (Post-ES6 Quick Tour)

These arrived after ES6 but show up constantly in modern code:

```js
// Optional chaining + nullish coalescing (full story: js/null-and-undefined)
const city = user?.address?.city ?? "unknown";

// Logical assignment operators
config.retries ??= 3;     // assign only if null/undefined (NOT if 0)
counter ||= 1;            // assign only if falsy
flags.debug &&= true;     // assign only if truthy

// Numeric separators — readability for big literals
const maxConnections = 1_000_000;

// Structured clone — real deep copy of plain data
const deepCopy = structuredClone(original);
deepCopy.profile.city = "Paris";  // original untouched this time
```

Distinguish `??=` from `||=` carefully: `??=` guards against `null`/`undefined` only, so
legitimate falsy values like `0`, `""`, `false` survive. That distinction is a frequent
follow-up question.

---

## Summary Checklist

Before moving to exercises, you should be able to say these without looking:

- `const` freezes the binding, not the value.
- Destructuring supports nesting, renaming, defaults (only for `undefined`).
- Spread makes **shallow** copies; nested objects stay shared.
- Rest params collect trailing arguments into a real array; must come last.
- `Set` for uniqueness/membership, `Map` for dynamic non-string keys.
- `for...of` = values (iterables); `for...in` = enumerable string keys (records).
- `?. ?? ??= ||=` — modern safety operators, with `??` vs `||` as the sharp edge.

## Related Modules

- [js/scope-and-closures](../scope-and-closures/README.md) — `let`/`const`, TDZ, arrows and `this`.
- [js/null-and-undefined](../null-and-undefined/README.md) — `?.`, `??`, and the full null story.
- [js/immutability](../immutability/README.md) — why shallow vs deep copying matters, worked by hand.
