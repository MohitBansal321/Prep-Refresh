# Async JavaScript — Exercises

> **No solutions here — by design.** These are for you to build from memory.
> When you're done, share your attempt and it will be *reviewed* (against the
> ideas in `README.md` / `code.js`), not graded against a hidden answer.
> Rule of thumb: if you can't start without peeking, re-read the README
> section listed in the hint line first.

---

## Easy

### E1. `delayWith(value, ms)`

**Requirements**
- Return a promise that fulfills with `value` after `ms` milliseconds.
- Then write `timeout(promise, ms)` that **rejects** with an `Error("timed out")`
  if the given promise hasn't settled within `ms`.

**Acceptance**
- `await delayWith("tea", 50)` → `"tea"`.
- `await timeout(delayWith("x", 100), 10)` → rejects with "timed out".
- `await timeout(delayWith("x", 10), 100)` → resolves `"x"`.

*Hint: README §5 (`race`) is the intended tool.*

**Reflection:** when the loser of your race settles late, does anyone still
observe its value or rejection? What problem can that cause in a real app?

---

## Medium

### M1. `promisify(fn)`

**Requirements**
- Convert an error-first callback API `(arg1, ..., callback)` into a
  promise-returning function.
- Reject on truthy `err`, resolve with the data otherwise.
- Preserve all leading arguments via rest/spread.

**Acceptance**
```js
const readP = promisify((path, cb) => cb(null, "contents:" + path));
await readP("a.txt");            // "contents:a.txt"
const failP = promisify((cb) => cb(new Error("ENOENT")));
await failP();                   // rejects Error: ENOENT
```

*Hint: README §8. Compare your version with Node's `util.promisify` docs afterwards.*

**Reflection:** what happens if the wrapped API calls its callback twice?
Whose fault is that — yours or the caller's — and why do promises make this
safe where raw callbacks weren't?

### M2. `retry(fn, attempts, backoffMs)`

**Requirements**
- `fn` returns a promise. Call it; on rejection wait `backoffMs`, then try
  again, up to `attempts` total tries.
- Double the delay between each attempt (100 → 200 → 400...).
- If every attempt fails, reject with the LAST error.
- Resolve immediately on the first success.

**Acceptance**
- A `fn` that fails twice then succeeds resolves on attempt 3, having waited
  roughly 100 + 200 = 300ms of backoff.
- An always-failing `fn` rejects after exactly `attempts` invocations.

*Hint: this fits naturally as either recursion or a loop inside an async
function — try one, then rewrite it the other way.*

**Reflection:** which inputs would you NOT want to retry (think HTTP status
codes)? How would you let callers decide?

### M3. `allPolyfill(promises)` — hand-rolled `Promise.all`

**Requirements**
- Accept an array (mix of promises and plain values).
- Resolve with results **in input order**, regardless of completion order.
- Reject immediately on the first rejection.
- Do not use `Promise.all`, `Promise.allSettled`, or `Promise.any`.

**Acceptance**
- `[d(300,"a"), d(100,"b"), "plain"]` → `["a","b","plain"]` at ~300ms — note
  `"b"` finished first but lands second.
- One rejection anywhere rejects the whole polyfill.

*Hint: you need ONE result array plus a counter of how many have settled.
Why is a counter necessary when the array already has slots?*

**Reflection:** your polyfill probably attaches handlers to every input up
front. Explain in one sentence why that's fine even though `Promise.race`-
style "first settle wins" logic isn't used.

---

## Hard

### H1. Parallel runner with concurrency limit

**Requirements**
- `runPool(tasks, limit)` runs an array of promise-returning task functions
  with AT MOST `limit` executing simultaneously.
- Results resolve in input order.
- A new task starts the moment any running one finishes — no idle workers,
  never more than `limit` in flight.

**Acceptance**
- 6 tasks of ~100ms each with `limit = 2` finish in ~300ms (not 600).
- Instrumented logging must show at most 2 tasks active at any instant
  (log start/end per task and eyeball the interleaving).
- Empty array resolves to `[]` immediately; `limit >= tasks.length`
  behaves like `Promise.all`.

*Hint: two classic shapes — fixed pool of worker loops pulling from a shared
index (see `code.js` §5), or a "start next when one finishes" chain. Build one,
then sketch the other on paper.*

**Reflection:** your shared cursor `next++` was safe only because JS is
single-threaded between awaits. Point at the exact line where a multi-threaded
version would need a lock, and explain why the await boundary is what makes it
racy there.

### H2. `settleInOrder(tasks)` — allSettled semantics, hand-rolled

**Requirements**
- Like `Promise.allSettled`: always resolves, never rejects.
- Result items are `{ status: "fulfilled", value }` or
  `{ status: "rejected", reason }`, in input order.
- Waits for ALL inputs, even early failures.

**Acceptance**
- Mixed success/failure input yields statuses in input order with correct
  `value`/`reason` fields.
- Reusing your M3 structure? Notice what changes: rejection handling moves
  from "reject the outer promise" to "record into the slot".

**Reflection:** after building M3 + H2, state the ONE structural difference
between `all` and `allSettled`. Why did the counter/array design survive
unchanged?

---

## Self-check before moving on

1. Can you explain every line of your own M3 out loud?
2. Did any exercise tempt you toward `setTimeout(...,0)` ordering assumptions?
   That's event-loop territory (`js/event-loop`) — park it for that module.
