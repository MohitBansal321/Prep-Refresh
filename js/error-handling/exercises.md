# Error Handling — Exercises

> **No solutions here — by design.** These are for you to build from memory.
> When you're done, share your attempt and it will be *reviewed* (against the
> ideas in `README.md` / `code.js`), not graded against a hidden answer.
> Rule of thumb: if you can't start without peeking, re-read the README
> section listed in the hint line first.

---

## Easy

### E1. `safeParseJSON(str)`

**Requirements**
- Return `{ ok: true, value }` when `JSON.parse` succeeds.
- Return `{ ok: false, error }` (the caught error) instead of throwing.
- Never let an exception escape the function.

**Acceptance**
- `safeParseJSON('{"a":1}')` → `{ ok: true, value: { a: 1 } }`.
- `safeParseJSON("{broken")` → `{ ok: false, error: <SyntaxError> }`, and
  `error instanceof SyntaxError` is `true`.

*Hint: README §4. Notice this is the "Result pattern" — errors as values,
not exceptions. When is that better than throwing?*

**Then answer in a comment:** what does your version lose compared to letting
the SyntaxError propagate with its stack — and why is that acceptable here?

### E2. Predict the output — finally puzzles

**Requirements**
- Write these three functions, run them, and write the output as a comment
  BEFORE running. Then run and correct yourself.

```js
function a() {
  try { return "try"; }
  finally { console.log("A"); }
}
function b() {
  try { throw new Error("x"); }
  catch { return "catch"; }
  finally { console.log("B"); }
}
function c() {
  try { return "try"; }
  finally { return "finally"; }
}
```

**Acceptance**
- You can state, before running: what each call returns/prints, and why `c`
  is dangerous.

*Hint: README §3, especially rule 2 and the return-in-finally trap.*

**Think about:** if `finally` always runs, what exactly does `return` inside
it override — and why do linters treat it as an error rather than a warning?

---

## Medium

### M1. `retry(fn, n)`

**Requirements**
- `fn` returns a promise. Call it; on rejection try again, up to `n` total
  attempts.
- Collect **all** errors from failed attempts into an array.
- Resolve with the value on first success.
- If all `n` attempts fail, reject with an `Error("all attempts failed")`
  whose `cause` carries the LAST error — and attach the full array somewhere
  the caller can inspect (you choose how).

**Acceptance**
- A `fn` that fails twice then succeeds resolves on attempt 3; your collected
  errors array has length 2 at that point.
- An always-failing `fn` rejects after exactly `n` invocations, and the
  rejection's `cause instanceof Error` is `true`.

*Hint: README §6 + §4 (`cause`). Loop inside an async function is the
simplest shape.*

**Then answer in a comment:** which real-world failures should NOT be retried
(think HTTP status codes), and how would you use the `retryable` flag idea
from README §5 to let callers opt out?

### M2. `validateUser(user)` — aggregate ALL validation errors

**Requirements**
- Validate: `name` non-empty string, `email` contains `"@"`, `age` is a
  number ≥ 0.
- Do NOT fail fast — collect every problem.
- If any problems exist, throw ONE `AggregateError(errors, "invalid user")`.
- Otherwise return the trimmed/coerced user object.

**Acceptance**
- `validateUser({ name: "", email: "no-at-sign", age: -3 })` throws once, and
  `err.errors.length === 3`.
- All-valid input returns cleanly.

*Hint: README §2 mentions `AggregateError`; §8 shows the opposite style
(fail-fast). Build both mentally and note when each is right.*

**Then answer in a comment:** fail-fast (§8) vs aggregate-all (this
exercise) — name one scenario where each is the correct choice and why.

### M3. Predict the output — async boundary puzzle

**Requirements**
- Write this, predict the output as a comment, then run:

```js
function oldStyle(cb) {
  setTimeout(() => {
    throw new Error("async boom");
  }, 0);
}
try {
  oldStyle(() => {});
  console.log("after call");
} catch (err) {
  console.log("caught:", err.message);
}
```

**Acceptance**
- You predicted correctly whether "caught:" ever prints — and you can explain
  the fix using Node's error-first callback convention.

*Hint: README §7. The try block exits long before the callback fires.*

**Think about:** each turn of the event loop gets a fresh stack — so where
does the thrown error actually go, and what channel should `oldStyle` have
provided instead?

---

## Hard

### H1. `withRetry` policy engine

**Requirements**
- `withRetry(fn, { attempts, retryable })` wraps an async `fn`.
- Before retrying, consult `retryable(err)` — a predicate the caller supplies;
  if it returns `false`, rethrow the original error immediately.
- On final failure, throw a NEW `AppError("operation failed after N attempts")`
  with `cause` set to the last error and the attempts count attached.
- Wrap the predicate call itself defensively: a buggy `retryable` must not
  crash the retry loop.

**Acceptance**
- A non-retryable failure on attempt 1 propagates immediately — total
  invocations: 1.
- A retryable failure exhausts all attempts and the thrown object is
  `instanceof AppError` with a working `.cause`.

*Hint: compose M1 with the custom-error machinery in README §5 / code.js §4.
Defensive wrapping means try/catch around the CALLER'S function — justify to
yourself why that isn't an empty-catch anti-pattern (README §9).*

**Reflection:** you now have error *classification* driving control flow —
where does this show up in system design (circuit breakers, backoff)? What
does your `retryable` predicate replace, and why is string-matching messages
(README §9 #5) the brittle alternative?

### H2. `runTasks(tasks)` — allSettled semantics with typed results

**Requirements**
- Takes an array of async task functions, runs them ALL (in parallel).
- Always resolves — never rejects — even if every task fails.
- Resolves with items shaped `{ status: "fulfilled", value }` or
  `{ status: "rejected", reason }`, in input order.
- Additionally logs a summary line: `X ok, Y failed` using each reason's
  `.name` for the failed ones.

**Acceptance**
- `[okTask, failTask, okTask]` resolves (not rejects) with statuses
  `fulfilled,rejected,fulfilled` in input order regardless of finish order.
- An empty input resolves immediately to `[]`.

*Hint: build it WITHOUT `Promise.allSettled` first (see the async module's
exercises M3/H2 for the counter trick), then compare with the one-liner.*
 
**Reflection:** after building both versions, state the ONE structural
difference between `Promise.all`'s contract and yours — and give a concrete
batch-job example where "partial success matters."

---

## Self-check before moving on

1. Can you explain, out loud, why E2's `c()` makes evidence disappear?
2. Did any exercise tempt you toward an empty `catch {}`? That's README §9 —
   park it and review anti-patterns before moving to the next module.
