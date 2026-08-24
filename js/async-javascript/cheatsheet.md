# Async JavaScript — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Core JS runtime concept: asynchronous control flow (callbacks → promises → async/await). |
| **Single Thread** | JS has ONE call stack — code never truly runs in parallel within a realm. Slow work (IO, timers) is delegated to the host environment; results come back later via callbacks/promises. The event loop (`js/event-loop`) schedules the "later". |
| **Callbacks** | Function handed to an API, invoked on completion (Node convention: `(err, result)`, error-first). Problems at scale: pyramid nesting, manual error checks everywhere, inversion of control (callee owns your continuation). |
| **Promise States** | `pending` → `fulfilled` (value) \| `rejected` (reason). Settled = frozen forever; double-resolve is ignored. Immutability kills the "called twice?" callback fear. |
| **Chaining / Error Propagation** | Each `.then` returns a NEW promise; returning a promise makes the chain wait for it. Errors skip success handlers down to the nearest `.catch`; a `.catch` that returns normally HEALS the chain. `.finally` always runs, gets no value. |
| **Combinators** | `all` = wait all, fail-fast · `allSettled` = wait all, never rejects ({status,value\|reason}[]) · `race` = first SETTLED wins (even a rejection) · `any` = first FULFILLED wins, else AggregateError. |
| **async/await** | Sugar over promises: `async fn` ALWAYS returns a promise; `await` suspends only that function until settlement. Desugars to `.then` chains. No new machinery, just syntax. |
| **Parallel vs Sequential** | Back-to-back `await`s serialize (waterfall bug: 3 independent fetches = 3× latency). Fix: START eagerly, await late — `const [a,b] = await Promise.all([f(), g()])`. |
| **Error Handling** | `try/catch` around `await` catches that rejection like a throw; un-awaited promises escape try/catch entirely (handle with `.catch`). Unhandled rejections crash modern Node. Scope catches tightly; recover or rethrow. |
| **Gotchas** | No cancellation — `all` rejecting doesn't stop siblings · `fetch` does NOT throw on 404/500 (check `res.ok`) · `for…of` + `await` loops sequentially by design · errors thrown in `.then` become rejections automatically · `try{}catch{}` sees nothing from un-awaited promises. |
| **Related Topics** | `js/event-loop` (microtasks vs macrotasks) · `js/functions` (closures behind promisify/retry) · fetch/HTTP basics · system-design orchestration (fan-out, timeouts, circuit breakers). |

### Skeleton

```js
const delay = ms => new Promise(r => setTimeout(r, ms)); // tiny helper

// sequential (dependency exists)          parallel (independent work)
const user = await getUser(id);            const [user, posts] = await Promise.all([
const posts = await getPosts(user);          getUser(id), getPosts(),
                                           ]);

// errors                                  // legacy callback → promise
try {                                      const p = new Promise((res, rej) =>
  const r = await risky();                   api(args, (err, d) => err ? rej(err) : res(d)));
} catch (e) { recover(e); }
```

### Remember In One Sentence

> **JS never waits — it schedules: promises turn "call me later" callbacks
> into composable values, and `await`/`Promise.all` let YOU choose whether
> steps run in sequence or in parallel.**

### Two Facts People Get Wrong

- "`Promise.all` failing cancels the other requests" — **No.** Nothing
  cancels them; they keep running, you just stop observing.
- "`fetch` throws on 404" — **No.** It rejects only on network failure;
  HTTP error statuses need an explicit `if (!res.ok) throw ...`.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write
it, *then* check against the README. If you miss one, that section is the only
thing you need to re-study.

1. Name the three promise states. Which are terminal, and what happens if you call `resolve()` twice?
2. Why does `.then` chaining stay flat while nested callbacks pyramid? What exact rule governs what one `.then` hands to the next?
3. Where does an error thrown inside a `.then` handler end up? Can the chain continue afterward — under what condition?
4. Recite the four combinators' contracts in one breath each. Which one can reject even when a success eventually arrives?
5. What does an `async` function always return, and what is `await f()` morally equivalent to?
6. Show (verbally) the slow-waterfall bug and its one-line fix. What's the rule of thumb?
7. Why does `try/catch` miss a rejection when the promise isn't awaited? Give the two ways to handle it correctly.
8. In `Promise.all([a(300), b(100)])`, what order are results in, why, and how long until resolution?
