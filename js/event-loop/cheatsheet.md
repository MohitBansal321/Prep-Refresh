# Event Loop — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Core JS runtime model (concurrency). |
| **Call Stack** | Single thread; one synchronous frame chain at a time. Long sync code blocks everything. |
| **Macrotasks (sources)** | `setTimeout` / `setInterval`, I/O callbacks, UI events — oldest-first, ONE per turn. |
| **Microtasks (sources)** | Promise reactions `.then/.catch/.finally`, `queueMicrotask`, `await` resumption. |
| **The Golden Rule** | After EACH macrotask, drain the ENTIRE microtask queue (incl. newly chained ones) before the next macrotask. |
| **Ordering Priority** | sync code → `process.nextTick` (Node) → microtasks → macrotask → nextTick → microtasks → … |
| **setTimeout(0) Truth** | Minimum delay, not zero: needs empty stack, drained microtasks, AND elapsed timer time (~1ms+; browsers clamp nested ≥4ms after depth 5). |
| **Starvation** | Endlessly re-scheduled microtasks (`p.then(loop)`) block all macrotasks forever; a `while(true)` blocks even microtasks (stack never empties). |
| **nextTick (Node)** | Own queue above promise microtasks; overuse starves the I/O phase. |
| **Gotchas** | Microtask spawned during drain joins SAME drain · drain runs after EVERY macrotask, not just script end · puzzle method: label lines SYNC/MICRO/MACRO then read off queues. |
| **Related Topics** | [async-javascript](../async-javascript/README.md) · scope-and-closures (callback state). |

### Skeleton
```js
console.log("1 sync");                              // SYNC
setTimeout(() => console.log("4 macro"), 0);        // MACRO
Promise.resolve().then(() => console.log("3 micro")); // MICRO
console.log("2 sync");                              // SYNC
// Output: 1 2 3 4  — always: sync → microtasks → macrotasks
```

### Remember In One Sentence
> **The event loop takes one macrotask at a time, and between any two macrotasks it completely empties the microtask queue — so promises always cut ahead of timers.**

### Two Facts People Get Wrong
- `setTimeout(fn, 0)` does NOT run "right away" or "after 0ms" — it's a minimum delay queued behind ALL pending microtasks.
- The microtask queue is drained after EVERY macrotask, not once at the end of the script.

---

## Recall Questions (answer from memory — no peeking)

1. Name the three queues/areas involved in running JS code and one source for each.
2. State the golden rule about when microtasks run.
3. In the classic puzzle (sync log → setTimeout(0) → promise.then → sync log), give the exact output order.
4. If a microtask schedules another microtask, when does the new one run?
5. Where does `process.nextTick` run relative to `.then` callbacks?
6. Why can't a timer fire in the middle of a long `while` loop? What's different from starvation via microtasks?
7. What two conditions must hold before a `setTimeout(fn, 0)` callback actually executes?
8. Walk through your mechanical method for solving any predict-the-output event-loop puzzle.
