# ES6+ Features — Exercises

Work through these in order. No solutions are provided — the point is to write the code
yourself and be able to *defend* every line. When you finish an attempt, compare it against
`code.js` in this folder and against the README sections.

> Ground rule for all exercises: use `const` by default, no `var`, and prefer destructuring
> in parameter lists over digging into a big object inside the function body.

---

## Easy — Dedupe and Membership with Set

Given this (deliberately ugly) data:

```js
const events = [
  { user: "ada", action: "login" },
  { user: "grace", action: "view" },
  { user: "ada", action: "logout" },
  { user: "linus", action: "view" },
  { user: "grace", action: "view" },
];
```

**Requirements:**
- Produce the list of **unique users**, preserving first-appearance order.
- Write a function `hasUser(events, name)` that answers "did this user appear?" using a
  `Set` built **once** outside the check loop (think about why that matters for complexity).
- Count how many distinct actions exist.

**Acceptance:**
- Unique users print as `['ada', 'grace', 'linus']` in that exact order.
- `hasUser(events, "grace")` is `true`; `hasUser(events, "ken")` is `false`.

**Reflection:** if you rebuilt the `Set` inside `hasUser` on every call, what would the
complexity of checking 10,000 names look like versus building it once?

---

## Medium — Destructure a Messy API Response

Real APIs return shapes like this:

```js
const apiResponse = {
  data: {
    results: [
      { id: 1, profile: { displayName: null, location: undefined }, roles: [] },
      { id: 2, profile: { displayName: "Grace", location: "NYC" }, roles: ["admin"] },
    ],
    pagination: { page: 1 },
    // notice: NO 'totalPages' key at all
  },
  error: null,
};
```

**Requirements:**
- With a **single destructuring statement**, extract:
  - the first result's `id` (as `firstId`),
  - the second result's `displayName` renamed to `author`, defaulting to `"anonymous"`
    when it is missing or `undefined`,
  - `pagination.page` with a default of `1`,
  - `totalPages` with a default of `10` even though the key doesn't exist.
- Then extract the second result's roles array so that mutating your extracted variable
  does **not** mutate `apiResponse`.
- Wrap everything so the whole statement survives being handed `{}` instead of
  `apiResponse` (nested defaults — think about where they must go).

**Acceptance:**
- All five bindings exist after one destructure; none throw.
- With input `{}`, every binding takes its default instead of crashing.
- Mutating your extracted roles array leaves `apiResponse.data.results[1].roles` untouched.

**Reflection:** why did `location: undefined` behave differently from `displayName: null`
with respect to defaults? Which one triggers a default, and why is that asymmetry useful?

---

## Medium — Merge Configs with Specific Precedence

You are writing a small settings loader. Three layers, lowest to highest precedence:

```js
const fileDefaults = { theme: "dark", retries: 3, verbose: false, cache: {} };
const cliFlags    = { verbose: true };
const envOverrides = { retries: 0 };
```

**Requirements:**
- Produce one merged config object where later layers win, using object spread.
- The original three objects must remain **completely unmutated** afterwards.
- `cache: {}` in the result must be safe to mutate without corrupting `fileDefaults.cache`
  — state explicitly which technique you used and its cost/tradeoff vs plain spread.

**Acceptance:**
- Result is `{ theme: "dark", retries: 0, verbose: true, cache: {...} }`.
- After merging, mutating the result's `retries` and `cache` leaves all three inputs
  untouched.
- You can explain, in one sentence each, why spread ordering controls precedence and why
  spread alone was not enough for `cache`.

**Reflection:** `retries: 0` is falsy. If someone rewrote the merge using `||` per-property,
what would go wrong, and which modern operator fixes exactly that?

---

## Hard — Rewrite Legacy ES5 into Modern Equivalents

Rewrite each snippet using modern syntax (template literals, destructuring, spread/rest,
default params, arrow functions, Map/Set). Keep behavior identical.

```js
// A. String building
function describe(user) {
  return user.name + " (" + user.role + ") - " + user.tags.join(", ");
}

// B. Defaults with the buggy idiom
function setTimeout_(ms) {
  ms = ms || 1000;
  return "waiting " + ms;
}

// C. Arguments juggling
function sumAll() {
  var args = Array.prototype.slice.call(arguments);
  var total = 0;
  for (var i = 0; i < args.length; i++) {
    total += args[i];
  }
  return total;
}

// D. Manual clone-and-append
function addItem(cart, item) {
  var copy = cart.slice(0);
  copy.push(item);
  return copy;
}

// E. Key collection and lookup
function findFirstAdmin(users) {
  var found = null;
  for (var i = 0; i < users.length; i++) {
    if (users[i].roles.indexOf("admin") !== -1) {
      found = users[i].name;
      break;
    }
  }
  return found;
}
```

**Acceptance:**
- A becomes a template literal with destructured parameters in the signature.
- B fixes a real latent bug: `setTimeout_(0)` must return `"waiting 0"` (the legacy version
  returns `"waiting 1000"` — say why before fixing it).
- C uses rest parameters; no `arguments`, no `var`.
- D is a one-expression spread-based version returning a new array.
- E's lookup uses a `Set` per user's roles (or another O(1)-membership structure), and the
  whole function fits in one `.find()` call with an arrow.

**Reflection:** which rewrite changed actual *behavior* rather than just style? Why is that
rewrite a bug fix and not just modernization?

---

## Hard — Symmetric Difference and Set Algebra

Implement `symmetricDifference(arrA, arrB)` — elements present in exactly one of the two
arrays (not both). Example: `symDiff([1, 2, 3], [2, 3, 4])` → `[1, 4]` (any order).

**Requirements:**
- Build it from Set operations (`has`, `add`) plus one final spread back to an array.
- Must handle duplicates in the inputs gracefully: `symDiff([1, 1, 2], [2, 3])` → `[1, 3]`.
- Then generalize: write `union`, `intersection`, and `difference(A, B)` alongside it.
- Target complexity: O(n + m), not O(n × m).

**Acceptance:**
- All four functions return arrays, deduped, and do not mutate their arguments.
- `intersection([1, 2], [2, 3])` → `[2]`; `difference([1, 2, 3], [2])` → `[1, 3]`.
- You can state out loud why each function is linear time and where `arr.includes()`
  would have made it quadratic.

**Reflection:** Sets compare elements by same-value-zero equality. Which of your four
functions breaks silently if given arrays of objects instead of primitives, and why?

---

## Bonus — Spread Semantics Under the Microscope

Predict the output of each line **before running it**, then verify:

```js
console.log([...[...new Set([1, 1, 2, 3, 3])]].length);

const base = { nested: { x: 1 } };
const copy = { ...base, ...base };
copy.nested.x = 9;
console.log(base.nested.x);

function f({ a = 1 } = {}, b = a * 2) {
  return [a, b];
}
console.log(f());
console.log(f({ a: 5 }));
console.log(f(undefined, 3));

const [x = 10, y = x + 1] = [5];
console.log(x, y);
```

**Acceptance:**
- Your written predictions match node's output for all six lines.
- For the `f(undefined, 3)` case you can explain *why* `undefined` (and only `undefined`)
  activates a parameter default mid-list.

**Reflection:** the last destructuring line shows defaults evaluated left-to-right and able
to reference earlier bindings. Where would that bite someone converting an old function
that read its options right-to-left?

---

*Solutions are intentionally omitted. If stuck, re-run the matching numbered section of*
`code.js`, change one thing, and observe. Ask for a walkthrough of a specific exercise only
after you've attempted it.*
