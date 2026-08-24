# Async JavaScript — Callbacks, Promises, async/await

> **Why this matters:** almost every JS interview probe here first —
> "what happens after `await`?", "`Promise.all` vs `allSettled`?",
> "why is my 'parallel' code slow?" If you can explain those cold,
> you're ahead of most candidates.

**Scope note:** this file covers *what* async primitives do and *how to use
them well*. The **event loop internals** (call stack, macrotask/microtask
queues, `setTimeout(fn, 0)` ordering tricks) live in `js/event-loop` — read
that after this one; it explains the *when* underneath everything here.

---

## 1. JS is single-threaded

JavaScript has **one call stack**. At any instant, exactly one piece of your
code is running. There is no second thread picking up your next function.

```js
function a() { console.log("a"); }
function b() { a(); }
b();
// b → a → log. One stack, unwinds top-down.
```

Consequences:

- Two functions can never literally run *at the same time* (in one JS realm).
- Long-running synchronous work blocks **everything**: UI updates, timers,
  network responses sitting in queues — all of them wait.
- So how does one thread handle IO, timers, clicks? By **delegating**: the
  host environment (browser / Node) performs the slow work on its own
  threads/processes, and hands the result back to your code later via a
  callback. That "later" is scheduled by the event loop (see
  `js/event-loop`).

The mental model to internalize: **JS never waits. It schedules.**

## 2. Blocking vs non-blocking

Blocking code makes the single thread sit idle while work completes:

```js
// BLOCKING — pretend this is a 3-second sync file read
const data = readFileSync("big.txt");   // thread frozen for 3s
console.log("done");
```

Non-blocking code asks for work and registers *what to do when it's done*:

```js
// NON-BLOCKING — thread is free immediately
readFile("big.txt", (err, data) => {
  if (err) throw err;
  console.log("done");                  // runs LATER, not NOW
});
console.log("requested");               // prints FIRST
```

Output order: `requested`, then `done`. This inversion — "the rest of the
function runs before the callback does" — is the root of every async
confusion that follows.

## 3. Callbacks and callback hell

A **callback** is a function you hand to an API so it can call you back when
async work finishes. Node's convention: `(err, result) => {}` with error
first ("error-first callbacks").

They scale badly. Three dependent operations nest three levels deep:

```js
getUser(id, (err, user) => {
  if (err) return handleError(err);
  getOrders(user.id, (err, orders) => {
    if (err) return handleError(err);
    getDetails(orders[0].id, (err, details) => {
      if (err) return handleError(err);   // ← error handling repeated everywhere
      render(details);
    });
  });
});
```

This is **callback hell** (a.k.a. "pyramid of doom"). The problems aren't
cosmetic:

1. **Inversion of control** — you handed your continuation to someone else's
   code. What if it calls back twice? Never? Synchronously?
2. **Error handling** — no propagation path; every level re-checks manually.
3. **Composition** — running steps in parallel, racing them, or timing out
   requires ad-hoc bookkeeping.

Promises fix all three.

## 4. Promises

A **Promise** is a placeholder object for a future value. It moves control
back into *your* hands: instead of passing callbacks *into* an API, you
receive a promise and attach behavior *onto* it.

### States

A promise is a tiny state machine:

| State | Meaning | Terminal? |
|---|---|---|
| `pending` | Work not finished yet | no |
| `fulfilled` | Finished successfully, has a value | yes |
| `rejected` | Failed, has a reason (usually an `Error`) | yes |

Once fulfilled/rejected it is **settled** — the state and value are frozen
forever. Calling `resolve` twice changes nothing. This immutability is the
answer to callback-hell problem #1: nobody can call you back twice.

```js
const p = new Promise((resolve, reject) => {
  setTimeout(() => resolve(42), 100);
});
p.then(v => console.log(v))     // 42   — runs on fulfillment
 .catch(e => console.error(e))  // skipped
 .finally(() => console.log("cleanup")); // always runs, gets no value
```

### Chaining

`.then()` returns a **new promise**, which is what makes chains flat instead
of nested. Each `.then` waits for the previous link to settle, and whatever
the callback returns becomes the next value:

```js
fetchUser(id)
  .then(user => getOrders(user.id))   // returning a promise → chain waits for it
  .then(orders => orders.filter(o => o.total > 100))
  .then(bigOrders => console.log(bigOrders.length));
```

Key rule: **if a `.then` callback returns a promise, the chain adopts it.**
That's how sequential async steps stay flat.

### Error propagation

Errors skip down the chain to the nearest `.catch` (or `onRejected`
handler), skipping every success handler in between — exactly like `throw`
skipping `try` blocks to the nearest `catch`:

```js
step1()
  .then(r => step2(r))
  .then(r => step3(r))        // step3 throws → both .thens below are skipped…
  .catch(err => {             // …and land HERE
    console.error(err.message);
    return fallbackValue;      // recovery: the chain continues with this
  })
  .then(v => console.log("recovered:", v));
```

Two subtleties worth interview points:

- A `.catch` that returns normally **heals** the chain — downstream `.then`s
  run again with the recovered value.
- Errors thrown inside a `.then` become rejections automatically. No
  `try/catch` needed inside the chain body.

## 5. Promise combinators

Four static methods take iterables of promises and answer four different
questions about them. All take non-promise values fine (they wrap them via
`Promise.resolve`). Shortcuts: **fail-fast**, **wait-for-all**,
**first-wins**, **first-success-wins**.

| Method | Waits for | Rejects / resolves when | Fail mode |
|---|---|---|---|
| `Promise.all` | all input promises | **rejects immediately** on the first rejection | fail-fast |
| `Promise.allSettled` | all input promises | always resolves — array of `{status, value\|reason}` | never rejects |
| `Promise.race` | the **first** settled promise (any outcome) | settles same way as that winner | winner may be a rejection |
| `Promise.any` | the **first fulfilled** promise | rejects only if **all** reject (with `AggregateError`) | ignores rejections until none succeed |

```js
const ok    = delay(100).then(() => "ok");
const boom  = delay(50).then(() => { throw new Error("boom"); });

await Promise.all([ok, boom]);       // rejects at ~50ms ("boom")
await Promise.allSettled([ok, boom]); // resolves at ~100ms:
// [{status:"fulfilled",value:"ok"},{status:"rejected",reason:Error}]
await Promise.race([ok, boom]);      // REJECTS at ~50ms — race doesn't filter errors
await Promise.any([ok, boom]);       // resolves at ~100ms with "ok"
```

Picking guide: **`all`** when losing one means losing everything ·
**`allSettled`** for batch jobs / partial-failure UIs · **`race`** for
timeouts and mirror-first-response · **`any`** when any success suffices
(retry across replicas).

Gotcha: `all`'s early rejection does **not cancel** the other promises —
they keep running; you just stop waiting. There is no built-in cancellation.

## 6. async/await

`async/await` is syntax sugar over promises — no new machinery. An `async`
function:

1. always returns a promise (returning `x` fulfills with `x`; throwing
   rejects),
2. suspends at each `await` until the awaited promise settles, then resumes
   with the value (or throws the reason into your function).

Desugared, roughly:

```js
async function main() {
  const user = await getUser();      // pause here, resume on fulfill
  const orders = await getOrders(user);
  return orders;
}
// is morally equivalent to:
function main() {
  return getUser().then(user =>
    getOrders(user)
  );
}
```

Every `await` inside a loop or sequence = one `.then` link. That insight
makes the next bug obvious.

### The classic slow-waterfall bug

`await` is **sequential by construction**. Await independent things one at a
time and you serialize work that could overlap:

```js
// BUG — ~600ms: three independent fetches run one after another
const user   = await getUser();      // 200ms
const posts  = await getPosts();     // 200ms
const stats  = await getStats();     // 200ms

// FIX — ~200ms: start all three, then await the combined promise
const [user2, posts2, stats2] = await Promise.all([
  getUser(), getPosts(), getStats(),
]);
```

Rule of thumb: **start work eagerly (no await), await late.** (`for…of` +
`await` loops through items sequentially — correct if that's what you meant,
a waterfall bug if it isn't.)

## 7. Error handling with try/catch around await

Inside an `async` function, a rejected promise behaves like a thrown
exception at the `await` point:

```js
async function loadProfile(id) {
  try {
    const res = await fetch(`/api/users/${id}`); // throws on network failure
    if (!res.ok) throw new Error(`HTTP ${res.status}`); // fetch doesn't throw on 404!
    return await res.json();                     // throws on malformed JSON
  } catch (err) {
    console.error("loadProfile failed:", err.message);
    return null;                                 // recover or rethrow — your choice
  }
}
```

Notes:

- `try/catch` around `await` catches rejections from *that* await — scope it
  tightly around what you intend to handle.
- Unhandled rejection outside any catch crashes Node (default since v15) /
  fires `unhandledrejection` in browsers. Always give chains a terminal
  `.catch`.
- `try/catch` does **nothing** around plain (non-awaited) promise creation —
  the rejection escapes asynchronously. `await` it or attach `.catch`.
- `finally` still runs for cleanup, same as sync code.

## 8. Converting callback APIs to promises

Legacy APIs take `(err, value) => {}` callbacks. Wrap once, use forever:

```js
// Manual wrapper
const getUserP = id =>
  new Promise((resolve, reject) => {
    db.getUser(id, (err, user) => (err ? reject(err) : resolve(user)));
  });

// Generic promisify (simplified — see exercises.md to build your own)
const promisify = fn =>
  (...args) =>
    new Promise((resolve, reject) =>
      fn(...args, (err, data) => (err ? reject(err) : resolve(data)))
    );

// Node already ships this for its own APIs:
const { promisify } = require("util");
const readFileP = promisify(require("fs").readFile);
```

Modern Node adds `require("fs/promises")` and `fs.promises.*` — prefer those
over hand-promisified core modules.

## 9. Fetch-style flow simulation

Putting it together — validate, fetch, transform, with proper error paths
and parallelism where safe:

```js
const delay = ms => new Promise(r => setTimeout(r, ms));

async function fakeFetch(url, ms = 100) {
  await delay(ms);                       // simulate latency
  if (url.includes("bad")) throw new Error(`HTTP 500 from ${url}`);
  return { url };
}

async function loadDashboard() {
  const config = await fakeFetch("/config", 50);   // must finish first…
  const [user, feed] = await Promise.all([         // …these don't depend on
    fakeFetch("/user"),                            // each other → parallel
    fakeFetch("/feed"),
  ]);
  return { theme: config.url, user, feed };
}

loadDashboard()
  .then(d => console.log("loaded:", d.theme))
  .catch(e => console.error("dashboard failed:", e.message));
```

Read the shape: sequential where there's dependency, `Promise.all` where
there isn't, single `.catch` as the safety net. That shape *is* idiomatic
modern async JS.

---

## Interview quick-fire

- *What does an `async` function return?* A promise — always.
- *Does `await` block the thread?* No — only that async function suspends.
- *Does `Promise.all`'s fast rejection cancel siblings?* No — they keep running.
- *Timeout an operation?* `Promise.race([op, rejectAfter(ms)])`.

## Related topics

- `js/event-loop` — macrotasks vs microtasks; why `.then` beats `setTimeout`.
- `js/functions` — closures power every helper here (`promisify`, retry).
- System design — fan-out/fan-in, timeouts, circuit breakers use all of this.
