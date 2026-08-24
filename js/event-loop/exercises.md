# Event Loop — Exercises

Work through these in order. Do not look at any solution; the goal is to build one reflex: **label every line as SYNC / MICRO / MACRO, then read the queues off in priority order** (sync → nextTick → microtasks → macrotask → drain → repeat).

> Rule for every puzzle: write your predicted output BEFORE running it in node. A miss means re-walking the queue mechanics, not just fixing the answer.

---

## Easy — Predict the Output (warm-ups)

For each snippet, write the exact console output order, then verify with node:

1.
```js
console.log("a");
setTimeout(() => console.log("b"), 0);
console.log("c");
```

2.
```js
setTimeout(() => console.log("timer"), 0);
Promise.resolve().then(() => console.log("promise"));
console.log("sync");
```

3.
```js
Promise.resolve().then(() => {
  console.log("m1");
  Promise.resolve().then(() => console.log("m2"));
});
setTimeout(() => console.log("t"), 0);
```

**Acceptance:** all three match exactly. For any miss, state which queue you placed the line in and why that was wrong.

---

## Medium — Nested Chains

Predict the output:

4.
```js
setTimeout(() => {
  console.log("T1");
  Promise.resolve().then(() => console.log("P-after-T1"));
}, 0);

Promise.resolve().then(() => {
  console.log("P1");
  setTimeout(() => console.log("T2"), 0);
});

console.log("S");
```

5.
```js
async function f() {
  console.log("f-start");
  await null;                 // what does this line schedule?
  console.log("f-resumed");
}
console.log("before");
f();
console.log("after");
```

6.
```js
queueMicrotask(() => console.log("qm"));
Promise.resolve().then(() => console.log("then"));
process.nextTick(() => console.log("ntick"));
console.log("sync");
```

**Acceptance:** all three exact. **Think about:** why does `await` behave like a microtask, and where exactly does `f()` hand control back to its caller?

---

## Medium — The Starvation Report

Write a short explanation (comments or markdown) answering:

- Why does `function loop() { Promise.resolve().then(loop); } loop();` permanently block `setTimeout` callbacks?
- Why does a plain `while(true) {}` block timers differently — i.e., what is blocked in each case?
- Give ONE practical example each of: (a) a bug caused by starvation-like behavior, (b) how you would detect it in production.

**Acceptance:** your answer uses the words *call stack*, *microtask queue*, *macrotask* correctly, without looking at the README.

---

## Hard — Jump-the-Queue Scheduler

Implement `schedule(task)` so that tasks run as soon as possible WITHOUT waiting for the timer phase:

**Requirements:**
- `schedule(fn)` must NOT use `setTimeout`/`setInterval`.
- Given:
```js
setTimeout(() => console.log("timer"), 0);
schedule(() => console.log("scheduled"));
```
`"scheduled"` prints BEFORE `"timer"`.
- Add `scheduleAt(priority, fn)` where lower number = earlier execution within your scheduler.

**Acceptance:** output order is deterministic across runs; explain in comments which queue your implementation uses and why it drains before timers.

**Then answer in a comment:** what happens if every task scheduled via your API schedules another one? Which concept from this module does that recreate?

---

## Hard — Explain Like I'm Five… No, Like an Interviewer

Record or write a ≤90-second explanation of the event loop covering ALL of:

1. Call stack + single thread
2. Macrotask vs microtask sources
3. The golden rule (full drain after each macrotask)
4. Why `setTimeout(fn, 0)` isn't immediate

**Acceptance:** you can do it without notes. If you stumble on point 3, re-read README §2 and §4 only.

---

## Self-check before moving on

- [ ] I can solve any 4-line sync/micro/macro puzzle mechanically.
- [ ] I know where `process.nextTick` fits in the ordering.
- [ ] I can explain starvation and name its cause precisely.
- [ ] I understand why microtasks chained inside microtasks still beat timers.
