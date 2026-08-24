// ============================================================
// EVENT LOOP — runnable examples
// Run: node js/event-loop/code.js
// Golden rule: after each macrotask, the microtask queue is
// drained COMPLETELY before the next macrotask runs.
//
// Each demo runs inside its own macrotask "bubble" (a setTimeout
// whose completion is signalled from the LAST timer callback),
// so every section's output matches its comments exactly.
// ============================================================

function nextBubble(fn) {
    return new Promise(resolve => {
        setTimeout(() => {
            fn(() => setTimeout(resolve, 0)); // done(): end-of-section macrotask break
        }, 0);
    });
}

// ------------------------------------------------------------
// SECTION 1 — Synchronous code: one job, straight down the stack
// ------------------------------------------------------------

await nextBubble(done => {
    console.log("1 sync start");            // 1 sync start
    console.log("2 sync end");              // 2 sync end
    done();
});

// ------------------------------------------------------------
// SECTION 2 — THE classic ordering puzzle:
//   synchronous > microtasks (promises) > macrotasks (timers)
// ------------------------------------------------------------

await nextBubble(done => {
    console.log("--- classic puzzle ---");
    console.log("A: script start");                          // 1st

    setTimeout(() => console.log("E: setTimeout 0"), 0);     // 4th

    Promise.resolve().then(() => console.log("C: promise then")); // 3rd

    console.log("B: script end");                            // 2nd
    done();
});
// Order: A → B → C → E.  "promise then" fires BEFORE "setTimeout"
// even though it appears later in source — microtasks always beat
// already-queued macrotasks.

// ------------------------------------------------------------
// SECTION 3 — Full drain rule: ALL microtasks run between two
// macrotasks, even ones scheduled DURING a microtask.
// ------------------------------------------------------------

await nextBubble(done => {
    console.log("--- drain rule ---");
    setTimeout(() => console.log("T: timeout"), 0);          // last

    Promise.resolve()
        .then(() => {
            console.log("M1: first microtask");
            // a microtask spawned FROM a microtask joins the SAME
            // drain — it still beats the timer below:
            Promise.resolve().then(() => console.log("M2: chained microtask"));
        })
        .then(() => console.log("M3: second .then"));

    console.log("S: sync done");
    done();
});
// Order: S → M1 → M2 → M3 → T

// ------------------------------------------------------------
// SECTION 4 — Multiple interleaved timers and promises
// ------------------------------------------------------------

await nextBubble(done => {
    console.log("--- interleave ---");
    setTimeout(() => {
        console.log("timer-1");
        // drain happens after EVERY macrotask, not just after sync code:
        Promise.resolve().then(() => console.log("  micro-after-timer-1"));
    }, 0);

    setTimeout(() => {
        console.log("timer-2");
        done();
    }, 0);

    Promise.resolve().then(() => {
        console.log("micro-1");
        Promise.resolve().then(() => console.log("  micro-2"));
    });

    console.log("interleave: sync");
});
// Order:
//   interleave: sync      (sync)
//   micro-1               (drain starts)
//     micro-2             (same drain)
//   timer-1               (first macrotask)
//     micro-after-timer-1 (drain after EVERY macrotask!)
//   timer-2               (next macrotask)

// ------------------------------------------------------------
// SECTION 5 — queueMicrotask vs setTimeout vs process.nextTick
// ------------------------------------------------------------

await nextBubble(done => {
    console.log("--- queueMicrotask / nextTick ---");
    Promise.resolve().then(() => console.log("qm: promise.then"));   // 2nd
    queueMicrotask(() => console.log("qm: queueMicrotask"));         // 3rd
    setTimeout(() => { console.log("qm: setTimeout"); done(); }, 0); // 5th
    console.log("qm: sync");                                         // 1st
    // Node bonus: nextTick outranks even promise microtasks:
    process.nextTick(() => console.log("ntick: nextTick"));          // actually 2nd!
});
// Real order in Node: qm: sync → ntick: nextTick → qm: promise.then
//                  → qm: queueMicrotask → qm: setTimeout
// (nextTick queue drains BEFORE the promise microtask queue.)

// ------------------------------------------------------------
// SECTION 6 — Starvation: endless microtasks block everything
// (bounded here so node exits cleanly!)
// ------------------------------------------------------------

await nextBubble(done => {
    console.log("--- starvation ---");
    let spins = 0;
    function starve() {
        if (spins++ < 3) {
            Promise.resolve().then(starve);  // keeps re-entering the microtask queue
        }
    }
    starve();
    setTimeout(() => {
        console.log(`starve: timer ran only after ${spins} microtask spins`);
        done();
    }, 0);
});
// The timer CANNOT jump ahead of the microtask chain — that's starvation.
// With spins < Infinity, the page/server would freeze forever.

// ------------------------------------------------------------
// SECTION 7 — setTimeout(fn, 0) is a MINIMUM delay, not zero
// ------------------------------------------------------------

await nextBubble(done => {
    console.log("--- min delay ---");
    const t0 = Date.now();
    setTimeout(() => {
        const elapsed = Date.now() - t0;
        console.log(`min-delay: setTimeout(0) waited ~${elapsed}ms (>= 0, often 1+)`);
        done();
    }, 0);
});
// Timers never fire early; the delay is a lower bound. Nested timeouts
// are also clamped to >= ~4ms by HTML spec after depth 5 (browsers).

console.log("done");                      // done — final line
