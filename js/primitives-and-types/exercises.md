# Primitives and Types — Exercises

Work through these in order. There are no solutions here on purpose — the point is to build the reflexes yourself: (1) writing type checks that survive the `typeof` quirks, and (2) reasoning about value-copy vs reference-copy without guessing.

> Rule of thumb for every exercise: primitives are immutable and compared by value; objects are compared by reference. Every edge case you hit is a quirk from the README — name it in a comment when you encounter it.

---

## Easy — Implement a Safe `typeof`

Write a function `safeTypeOf(value)` that returns a **string** describing the value's type, fixing `typeof`'s famous blind spots.

**Requirements:**
- Returns `"null"` for `null` (not `"object"`).
- Returns `"array"` for arrays.
- Returns `"number"` for numbers, including correctly reporting `NaN` — decide what string NaN should produce, document your choice in a comment, and be consistent.
- Falls back to the built-in `typeof` result for everything else.

**Acceptance:**
- `safeTypeOf(null)` is not `"object"`.
- `safeTypeOf([1, 2, 3])` is not `"object"`.
- `safeTypeOf(() => 1)` still reports a sensible function-ish result.
- `safeTypeOf(new String("hi"))` returns something that distinguishes it from `safeTypeOf("hi")` — explain in a comment which of the two results you'd want callers to see, and why.
- Calling it on an undeclared variable reference (pass it via a helper that takes `typeof x` as an argument, or reason it in comments) does not crash — note what `typeof` guarantees here that your function inherits.

**Then answer in a comment:** why was `typeof null === "object"` never fixed? What category of breakage would a fix cause?

---

## Medium — Deep Equality for Primitives vs Objects

Write `deepEqual(a, b)` that compares two values: strictly for primitives, recursively for objects and arrays.

**Requirements:**
- Primitives compare with strict equality semantics (`===`) — with one deliberate exception: two values that are both `NaN` should count as equal (decide how you detect that, given that `NaN !== NaN`).
- Plain objects are equal if they have identical key sets and every corresponding pair of values is deep-equal.
- Arrays are equal only if same length and each element pair is deep-equal, in order.
- Decide and document (in comments): should `{a: 1} instanceof Array`-style mismatches (array vs object with same contents) be equal or not? Pick one and enforce it.
- Must not blow the stack on reasonably nested structures — but you may state a depth limit rather than solve iteration explicitly. Note the tradeoff in a comment.

**Acceptance:**
- `deepEqual(1, "1")` is `false`.
- `deepEqual(NaN, NaN)` is `true` while `NaN === NaN` is `false`.
- `deepEqual({x: {y: [1, 2]}}, {x: {y: [1, 2]}})` is `true`.
- `deepEqual([1, 2], [2, 1])` is `false` (order matters).
- `deepEqual(null, {})` is `false` and doesn't throw.
- Mutating one input after a `true` result makes a re-run return `false`.

**Then answer in a comment:** if `a` and `b` are objects and `deepEqual(a, b)` returns `true`, are `a` and `b` the *same* object? What does that tell you about the difference between equality and identity?

---

## Hard — Coerce-and-Compare Exploration

Build a small exploration harness `coerceCompare(x, y)` plus a printed table of results, mapping out exactly where loose `==` and strict `===` disagree — then justify each row from the coercion rules.

**Requirements:**
- `coerceCompare(x, y)` returns an object like `{ loose: true, strict: false }` for a given pair.
- Your harness must cover at least these pairs: `1/"1"`, `0/""`, `""/"0"`, `false/""`, `null/undefined`, `null/0`, `undefined/NaN`, `NaN/NaN`, `[1]/1`, `[""]/""`, `"0"/false`, `[]/false`, `new String("5")/"5"` (boxed vs primitive), `Object.create(null)/{}`.
- For each pair where `loose` differs from `strict`, print a one-line explanation naming the actual coercion step (ToNumber? ToPrimitive? the special null/undefined rule?).

**Acceptance:**
- Every row's booleans match what Node actually prints when you run the raw comparisons directly next to your harness output (verify side by side).
- The boxed-wrapper row shows `strict: false` — and your explanation says why auto-boxing is involved.
- At least one row demonstrates that `==` is NOT transitive across three values; show the three pairwise comparisons that prove it.

**Think about:** after building the table, write a short comment answering — if you were forced to use `==` somewhere (e.g., legacy API comparing against `null`), which single comparison idiom is actually safe to keep, and why is it safe when everything else in your table is surprising?

---

## Bonus Challenge — Mutation Audit

Take any function you've written above (or write a new `formatUser(user)` that uppercases `user.name` and appends a computed field).

1. Write a version that mutates its argument, and a version that doesn't.
2. Demonstrate the difference with a caller whose object changes unexpectedly between two calls.
3. In comments, trace exactly where the reference copy happens (the call site? inside the function?) and why the primitive-returning version has no equivalent hazard.

**Think about:** strings can't be mutated even though you pass them everywhere cheaply — what does that imply about why large mutable objects get passed by reference instead?
