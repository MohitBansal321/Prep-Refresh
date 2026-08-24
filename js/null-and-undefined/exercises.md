# null & undefined — Exercises

Work through these in order. There are deliberately **no solutions here** — the goal is
to build three reflexes: (1) predicting exactly what JavaScript produces for "nothing"
values, (2) choosing `??` / `?.` / defaults correctly instead of reaching for `||`, and
(3) explaining the *why* out loud, interview-style.

Run your attempts with `node js/null-and-undefined/<your-file>.js` and compare against
your own predictions **before** reading any output.

---

## Easy — Safe Deep Property Reader

Write a function `getIn(obj, path)` that reads a nested property safely.

**Requirements:**
- `path` is an array of keys, e.g. `["user", "address", "city"]`.
- Return the value at that path, or a caller-supplied fallback (default `"unknown"`).
- Must never throw, even if an intermediate level is `null` or missing.
- Implement it twice: once using optional chaining, once WITHOUT any `?.` (plain checks).

**Acceptance:**
- `getIn(order, ["customer", "address", "city"])` returns `"Pune"` for the order object
  from `code.js`.
- `getIn({}, ["user", "name"])` returns `"unknown"` — not a TypeError.
- `getIn({ tag: null }, ["tag"])` returns `null`, and you can explain why that is *not*
  the same as returning `undefined`.

**Reflection:** when does your no-`?.` version become unreadable compared to the `?.`
version? At what nesting depth would you personally refuse to write it by hand?

---

## Easy — Predict-the-Output: Falsy Warm-up

Before running anything, write down the output of each line on paper.

```js
console.log(Boolean([]), Boolean("0"), Boolean("false"), Boolean(NaN));
console.log(0 == null, 0 ?? null, null ?? 0);
console.log(typeof null, typeof undefined, typeof NaN);
```

**Requirements:**
- Commit to written predictions first; no "running to check."
- For every line you got wrong, name the exact rule from the README that you violated.

**Acceptance:**
- All nine outputs predicted correctly.
- You can state the six falsy values from memory in under 10 seconds.

**Reflection:** which truthy-but-"looks empty" value (`[]`, `{}`, `"0"`) do you think
bites real codebases most often, and in what kind of bug?

---

## Medium — Rewrite `||` as `??` and Justify Every Change

You inherit this settings resolver full of `||`:

```js
function resolveSettings(input) {
  return {
    retries: input.retries || 3,
    timeoutMs: input.timeoutMs || 5000,
    label: input.label || "default",
    verbose: input.verbose || false,
    tags: input.tags || [],
  };
}
```

**Requirements:**
1. List, for each field, every input value where `||` and `??` give DIFFERENT results.
2. Rewrite using `??` only where it changes behavior — keep `||` where collapsing all
   falsy values is genuinely intended, and say why.
3. Add one field where `||` is actually the CORRECT choice, and defend it.

**Acceptance:**
- `resolveSettings({ retries: 0 })` keeps `retries: 0`.
- `resolveSettings({ label: "" })` — decide and justify whether `""` should survive.
- You can explain in one sentence per changed field why `??` is more faithful to intent.

**Reflection:** if a teammate says "`??` is just modern style, they're equivalent,"
what is the shortest counterexample you could type into a REPL?

---

## Medium — The Default-Parameter Trap

Implement `createConnection({ host, port, timeout } = {})` with defaults
`"localhost"`, `8080`, `1000`.

**Requirements:**
- Callers may pass nothing, a partial object, or explicit `undefined` fields.
- Explicit `null` must NOT silently pass through to the returned config object.
- Normalize inside the body using `??` so both `null` and `undefined` get defaults.
- Then implement a second version using ONLY default parameters (no `??`) and document
  exactly which caller inputs produce different results between the two versions.

**Acceptance:**
- `createConnection()` → all defaults.
- `createConnection({ host: null })` → host becomes `"localhost"` in version 2, but
  stays `null` in the default-parameter-only version — verify and explain why.
- `createConnection({ port: 0 })` → port stays `0` in BOTH versions.

**Reflection:** default parameters trigger on `undefined` only. Why is that design
choice (rather than "on any falsy") the only sane option? What would break otherwise?

---

## Hard — Predict-the-Output: Nullish Puzzle Gauntlet

Write a module of 12+ expressions mixing `?.`, `??`, `||`, `==`, `typeof`, and default
parameters. Solve it as a quiz: predict ALL outputs before running.

**Requirements:**
- Include at least: one chain where `?.` short-circuits mid-chain; one expression where
  `?.` yields `undefined` because a property was `null` (not missing); one mixed
  `??`/`||` requiring parentheses; one `typeof` result that surprises you; one case
  where `x == null` differs from `x === null`; one function call whose argument list
  makes a default fire vs not fire.
- Number every line and put your prediction in a comment BEFORE the code runs.
- After running, mark each prediction right/wrong inline.

**Acceptance:**
- Zero TypeErrors thrown.
- Every line annotated with a correct trailing-output comment after verification.
- Score yourself; anything below 9/12 means re-read README Parts 4–8.

**Reflection:** which single mental model ("system shrugs vs person answers none",
"falsy vs nullish") resolved most of your misses? Could you teach it in 60 seconds?

---

## Hard — Audit a Realistic API Response Handler

Given this typical messy API payload handler, harden it:

```js
function getShippingInfo(payload) {
  return {
    city: payload.order.customer.address.city,
    courier: payload.order.shipping.courier.name.toUpperCase(),
    cost: payload.order.shipping.cost || "free",
    discountApplied: payload.order.discount || false,
  };
}
```

**Requirements:**
- Make it throw-proof against: missing `order`, `customer`, `address`, `shipping`,
  `courier`, or `discount`; `cost === 0`; `courier.name === null`.
- Preserve legitimate `0` costs as `0` (decide how to represent that distinctly from
  "free" and justify).
- Return sensible fallbacks for every field without masking genuine data bugs — add a
  comment per field saying what its fallback means diagnostically.
- Bonus: refactor into small helpers so each field's safety strategy is visible at a
  glance instead of one giant chained expression.

**Acceptance:**
- `getShippingInfo({})` returns a complete object, throws nothing.
- `getShippingInfo({ order: { shipping: { cost: 0 } } })` keeps cost as `0`.
- No field silently converts valid falsy data (`""`, `0`, `false`) into a fallback.

**Reflection:** `?.` removes crashes, but blanket use of it can HIDE bugs (data that
*should* exist). Where did you choose to let a crash happen (or log loudly) rather than
paper over it with `?.`, and why?
