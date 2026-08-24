# Event Loop

**Prerequisite:** [js/async-javascript](../async-javascript/README.md) — promises and `async/await` fundamentals. This module explains *when* that async code actually runs.

## 1. The pieces

JavaScript runs on a **single thread** with a **call stack**. Long-running synchronous code blocks everything (rendering, input handling). Async APIs (`setTimeout`, I/O, timers) let you schedule work without blocking — but the scheduled callbacks need someone to pick them up when it's their turn. That's the **event loop**.

- **Call stack** — where synchronous functions execute, one frame at a time.
- **Macrotask queue** (a.k.a. task queue) — `setTimeout` / `setInterval` callbacks, I/O callbacks, UI events.
- **Microtask queue** — promise reactions (`.then/.catch/.finally`), `queueMicrotask`, `await` continuations. In Node there's also the **nextTick queue**, which drains even before microtasks.
- **Event loop** — an endless loop: pop a macrotask → run it to completion → drain ALL microtasks → repeat.

## 2. The algorithm, step by step

1. Execute all synchronous script code (itself one big macrotask).
2. When the stack empties, drain the **entire** microtask queue — including new microtasks created during draining.
3. (Browser: render if due.)
4. Take the **oldest** macrotask from the task queue; run it fully.
5. Drain microtasks again.
6. Repeat forever.

> The golden rule: **after every macrotask, ALL pending microtasks run before the next macrotask.**

## 3. The classic ordering puzzle

```js
console.log("A: script start");

setTimeout(() => console.log("E: setTimeout 0"), 0);

Promise.resolve().then(() => console.log("C: promise then"));

console.log("B: script end");
```

Walkthrough:
- Sync first: logs `A`, registers timer E in the macrotask queue, registers C in the microtask queue, logs `B`.
- Stack empties → microtask drain: `C`.
- Only now the loop picks a macrotask: `E`.

Order: **A B C E** — even though `setTimeout(..., 0)` appears earlier in source. Microtasks always beat already-queued macrotasks.

## 4. Full drain rule

Microtasks scheduled *during* a microtask still run in the **same drain**:

```js
setTimeout(() => console.log("T: timeout"), 0);

Promise.resolve()
  .then(() => {
    console.log("M1");
    Promise.resolve().then(() => console.log("M2")); // joins this same drain
  })
  .then(() => console.log("M3"));
```

Order: `M1 M2 M3 T`. The loop won't touch the timer until the microtask queue is truly empty.

And after **every** macrotask — not just the initial script:

```js
setTimeout(() => {
  console.log("timer-1");
  Promise.resolve().then(() => console.log("micro-after-timer-1")); // runs before timer-2!
}, 0);
setTimeout(() => console.log("timer-2"), 0);
```

Order: `timer-1 → micro-after-timer-1 → timer-2`.

## 5. Priority summary (Node)

| Queue | Sources | When drained |
|---|---|---|
| **nextTick** | `process.nextTick` | before anything else once stack empties |
| **Microtasks** | `.then/.catch/.finally`, `queueMicrotask`, `await` resumption | after nextTick, before any macrotask |
| **Macrotasks** | `setTimeout/setInterval`, I/O events, UI events | oldest-first, one at a time |

So: sync code → nextTick → microtasks → (macrotask → nextTick → microtasks) → …

## 6. `setTimeout(fn, 0)` myths

- It does **not** run "immediately" or after exactly 0ms. The delay is a **minimum**: the callback can only run when the stack is empty, all microtasks are done, AND the timer has aged ≥ its delay.
- In practice you'll see ~1ms+ (and browsers clamp nested timeouts to ≥4ms after depth 5).
- If sync code or a microtask chain keeps running, your timer waits indefinitely — that's **starvation**:

```js
function starve() { Promise.resolve().then(starve); } // endless self-rescheduling
starve();
// setTimeout never fires again — microtasks block everything
```

(Bounded version demonstrated in `code.js` Section 6 so node exits cleanly.)

## 7. Why interviews love this

Almost every JS interview includes a "predict the output" puzzle built from mixed `console.log`, `setTimeout`, and `Promise.then`. Solving it reliably = mechanically applying the golden rule line by line: mark each statement as SYNC / MICRO / MACRO, then read off: all SYNC in order → all MICRO in order → each MACRO followed by any MICROs it spawned.

## Key Takeaways

- One thread, one call stack; async = queues + the event loop picking work.
- Microtasks (promises) always beat macrotasks (timers); nextTick beats both in Node.
- Microtask queue drains **completely** — including newly chained microtasks — before any macrotask.
- The drain happens after **every** macrotask, not just after the script.
- `setTimeout(fn, 0)` is a minimum delay; starvation is real when microtasks never stop.

## Common Interview Questions

1. Explain the event loop in under a minute. What are the queues involved?
2. Difference between microtasks and macrotasks? Give two examples of each.
3. Predict-the-output puzzle mixing sync logs, `setTimeout(0)` and `Promise.then`.
4. Does `setTimeout(fn, 0)` guarantee execution after 0ms? Why not?
5. What happens between two macrotasks in a browser besides running tasks?
6. What is microtask starvation? How would you accidentally cause it?
7. Where do `process.nextTick` callbacks run relative to promise callbacks?
8. Why doesn't a long `while` loop allow timers to fire mid-loop?
