// ============================================================================
// ASYNC JAVASCRIPT — runnable companion to README.md
// Run:  node js/async-javascript/code.js
// Predict each output BEFORE reading the trailing comment.
// ============================================================================

// ----------------------------------------------------------------------------
// The one helper this whole file leans on: a promise that settles after `ms`.
// ----------------------------------------------------------------------------
const delay = ms => new Promise(r => setTimeout(r, ms));

// Timers drift (Windows can be coarse by ~15ms), so elapsed time is logged
// as a floored 100s bucket — a lower bound that is ALWAYS true, since
// setTimeout never fires early.
const bucket = ms => `${Math.floor(ms / 100) * 100}+ms`;

// ----------------------------------------------------------------------------
// SECTION 1 — Promise basics, chaining, catch
// ----------------------------------------------------------------------------

async function section1() {
  console.log("=== 1. PROMISE BASICS, CHAINING, CATCH ===");

  // The executor runs synchronously; resolve just marks fulfillment.
  const p = new Promise(resolve => resolve(42));
  console.log(await p);                        // 42

  // Every .then returns a NEW promise, which keeps chains flat.
  // Returning a promise from a handler makes the chain WAIT for it.
  const chain = await Promise.resolve("user:42")
    .then(id => delay(10).then(() => `orders(${id})`))
    .then(label => label.toUpperCase());
  console.log(chain);                          // ORDERS(USER:42)

  // Rejection skips every success handler down to the nearest .catch,
  // and a .catch that returns normally HEALS the chain below it.
  const healed = await Promise.reject(new Error("step 2 failed"))
    .then(v => `never reached (${v})`)
    .catch(err => err.message)
    .then(msg => `recovered: ${msg}`);
  console.log(healed);                         // recovered: step 2 failed

  // finally always runs and receives NO value — cleanup only.
  let cleanedUp = false;
  await delay(10).finally(() => { cleanedUp = true; });
  console.log(cleanedUp);                      // true
}

// ----------------------------------------------------------------------------
// SECTION 2 — Combinators: all / allSettled / race / any (with timing)
// ----------------------------------------------------------------------------

async function section2() {
  console.log("\n=== 2. COMBINATORS ===");

  const job = (name, ms, fail) =>
    delay(ms).then(() => {
      if (fail) throw new Error(`${name} failed`);
      return name;
    });

  // --- all: waits for ALL; result order follows INPUT order, not finish order
  let t0 = Date.now();
  const all = await Promise.all([job("fast", 100), job("slow", 300)]);
  console.log(all.join(","), "@", bucket(Date.now() - t0));  // fast,slow @ 300+ms

  // --- all: FAILS FAST on the first rejection (the sibling keeps running!)
  t0 = Date.now();
  try {
    await Promise.all([job("bad", 100, true), job("good", 300)]);
  } catch (err) {
    console.log(err.message, "@", bucket(Date.now() - t0));  // bad failed @ 100+ms
  }

  // --- allSettled: NEVER rejects; waits for everyone, reports each outcome
  t0 = Date.now();
  const settled = await Promise.allSettled([job("ok", 100), job("nope", 300, true)]);
  console.log(settled.map(s => s.status).join(","), "@", bucket(Date.now() - t0));
  // fulfilled,rejected @ 300+ms

  // --- race: settles with WHOEVER SETTLES FIRST — even if that's a rejection
  t0 = Date.now();
  const winner = await Promise.race([
    delay(300).then(() => "sluggish"),
    delay(100).then(() => "quick"),
  ]);
  console.log(winner, "@", bucket(Date.now() - t0));         // quick @ 100+ms

  // --- any: FIRST FULFILLMENT wins; earlier rejections are simply ignored
  t0 = Date.now();
  const firstOk = await Promise.any([job("mirror-a", 100, true), job("mirror-b", 300)]);
  console.log(firstOk, "@", bucket(Date.now() - t0));        // mirror-b @ 300+ms

  // --- any rejects with an AggregateError ONLY when every input failed
  try {
    await Promise.any([job("a", 10, true), job("b", 20, true)]);
  } catch (err) {
    console.log(err.constructor.name, err.errors.length);    // AggregateError 2
  }
}

// ----------------------------------------------------------------------------
// SECTION 3 — Sequential vs parallel awaits (the classic slow-waterfall bug)
// ----------------------------------------------------------------------------

async function section3() {
  console.log("\n=== 3. SEQUENTIAL vs PARALLEL AWAITS ===");

  const work = name => delay(200).then(() => `${name}-done`);

  // THE BUG: independent awaits written back-to-back form a waterfall.
  // Each await suspends until the previous finishes → 3 × 200ms.
  let t0 = Date.now();
  const a = await work("a");                   // starts at t ≈ 0
  const b = await work("b");                   // starts at t ≈ 200
  const c = await work("c");                   // starts at t ≈ 400
  console.log([a, b, c].join(",") + " sequential=" + bucket(Date.now() - t0));
  // a-done,b-done,c-done sequential=600+ms

  // THE FIX: start all work eagerly (no await), then await once, together.
  t0 = Date.now();
  const [x, y, z] = await Promise.all([work("x"), work("y"), work("z")]);
  console.log([x, y, z].join(",") + " parallel=" + bucket(Date.now() - t0));
  // x-done,y-done,z-done parallel=200+ms
}

// ----------------------------------------------------------------------------
// SECTION 4 — try/catch around awaited promises
// ----------------------------------------------------------------------------

async function section4() {
  console.log("\n=== 4. TRY/CATCH WITH REJECTED PROMISES ===");

  async function risky() {
    await delay(10);
    throw new Error("disk on fire");
  }

  // await turns a rejection into a throw AT THIS LINE — catchable here.
  try {
    await risky();
    console.log("never printed");
  } catch (err) {
    console.log("try/catch caught:", err.message);   // try/catch caught: disk on fire
  }

  // ...but an UN-awaited rejection sails straight past the try block:
  let stray;
  try {
    stray = risky();               // rejected promise created, never awaited here
  } catch (err) {
    console.log("sync catch ran"); // never happens — nothing threw synchronously
  }
  console.log("try/catch saw nothing");            // try/catch saw nothing
  const outcome = await stray.then(() => "ok", e => e.message);
  console.log(outcome);                            // disk on fire

  // Rejections climb through awaited call chains exactly like sync throws:
  const lvl3 = async () => { throw new Error("deep failure"); };
  const lvl2 = async () => { await lvl3(); };
  const lvl1 = async () => { await lvl2(); };
  try {
    await lvl1();
  } catch (err) {
    console.log("propagated:", err.message);       // propagated: deep failure
  }
}

// ----------------------------------------------------------------------------
// SECTION 5 — Mini task queue: N workers pulling from a shared queue
// ----------------------------------------------------------------------------

async function section5() {
  console.log("\n=== 5. TASK QUEUE (concurrency limit = 2) ===");

  const jobs = [
    () => delay(450).then(() => "R0"),           // long-running
    () => delay(100).then(() => "R1"),
    () => delay(250).then(() => "R2"),
    () => delay(150).then(() => "R3"),
  ];

  async function runQueue(tasks, limit) {
    const results = new Array(tasks.length);
    let next = 0;                                // shared cursor into the queue

    async function worker(name) {
      while (next < tasks.length) {
        const i = next++;                        // safe: single thread, sync claim
        console.log(`${name} picked task ${i}`);
        results[i] = await tasks[i]();
        console.log(`task ${i} done -> ${results[i]}`);
      }
    }

    await Promise.all(
      Array.from({ length: limit }, (_, k) =>
        worker(k === 0 ? "workerA" : "workerB")
      )
    );
    return results;
  }

  const t0 = Date.now();
  const results = await runQueue(jobs, 2);
  console.log(results.join(",") + " in " + bucket(Date.now() - t0));
  // R0,R1,R2,R3 in 500+ms
}

// ----------------------------------------------------------------------------
// Run everything, top to bottom.
// ----------------------------------------------------------------------------

(async () => {
  await section1();
  await section2();
  await section3();
  await section4();
  await section5();
  console.log("\nAll sections completed.");
})();
