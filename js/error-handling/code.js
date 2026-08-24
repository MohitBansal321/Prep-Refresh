// ============================================================================
// ERROR HANDLING — runnable companion to README.md
// Run:  node js/error-handling/code.js
// Predict each output BEFORE reading the trailing comment.
// ============================================================================

// ----------------------------------------------------------------------------
// SECTION 1 — Throw an Error object (never a string), catch it, read its fields
// ----------------------------------------------------------------------------

function section1() {
  console.log("=== 1. THROWING AND CATCHING AN Error ===");

  // `throw "oops"` is legal but terrible: no .message, no .name, no .stack,
  // and `instanceof Error` downstream is impossible.
  try {
    throw new Error("payment failed");
  } catch (err) {
    console.log(err.message);        // payment failed
    console.log(err.name);           // Error
    console.log(typeof err.stack);   // string — the stack trace exists
    console.log(err instanceof Error); // true — primitives can never do this
  }

  // The stack records where the error was CREATED (at construction time),
  // which is why `new Error(...)` should happen at the failure site.
  try {
    throw new Error("created here");
  } catch (err) {
    console.log(err.stack.startsWith("Error: created here")); // true
  }
}

// ----------------------------------------------------------------------------
// SECTION 2 — Built-in error types: TypeError, ReferenceError, RangeError
// ----------------------------------------------------------------------------

function section2() {
  console.log("\n=== 2. BUILT-IN ERROR TYPES ===");

  // TypeError — wrong TYPE used: prop of null/undefined, calling non-functions.
  try {
    null.foo;
  } catch (err) {
    console.log(err.name);                                  // TypeError
    console.log(err instanceof TypeError);                  // true
    console.log(err.message.startsWith("Cannot read"));     // true
  }

  // ReferenceError — a variable that was NEVER DECLARED was accessed.
  try {
    console.log(notDeclaredAnywhere);
  } catch (err) {
    console.log(err.name);           // ReferenceError
    console.log(err.message);        // notDeclaredAnywhere is not defined
  }

  // RangeError — value outside the valid range. Two classics:
  try {
    new Array(-5);                   // array length must be a non-negative int
  } catch (err) {
    console.log(err.name, "-", err.message); // RangeError - Invalid array length
  }
  try {
    "ab".repeat(-1);                 // repeat count can't be negative
  } catch (err) {
    console.log(err.name, "-", err.message); // RangeError - Invalid count value: -1
  }

  // Rule of thumb from the README: TypeError/RangeError = programmer mistakes;
  // plain Error / custom subclasses = domain failures.
}

// ----------------------------------------------------------------------------
// SECTION 3 — finally always runs ... including the return-in-finally TRAP
// ----------------------------------------------------------------------------

function section3() {
  console.log("\n=== 3. TRY/CATCH/FINALLY ===");

  // finally runs on every exit path: normal completion, caught error,
  // even return/break/continue from try or catch.
  function withFinally(mode) {
    try {
      if (mode === "throw") throw new Error("boom");
      if (mode === "return") return "returned from try";
      return "completed normally";
    } catch (err) {
      return "caught: " + err.message;
    } finally {
      console.log(`  finally ran (${mode})`); // cleanup happens EVERY time
    }
  }
  console.log(withFinally("normal")); // completed normally
  console.log(withFinally("throw"));  // caught: boom
  console.log(withFinally("return")); // returned from try

  // If catch itself throws, finally still runs before the new error escapes.
  try {
    try {
      throw new Error("first");
    } catch (err) {
      throw new Error("second");
    } finally {
      console.log("finally ran around catch's own throw"); // runs first
    }
  } catch (err) {
    console.log("outer caught:", err.message); // outer caught: second
  }

  // THE GOTCHA: return in finally overrides EVERY other exit path —
  // returns from try/catch AND even exceptions — silently swallowing them.
  function trap() {
    try {
      throw new Error("important!");
    } catch (err) {
      return "from catch";
    } finally {
      return "from finally"; // overrides BOTH the return above AND the throw
    }
  }
  console.log(trap()); // from finally  ← the error vanished without a trace

  // Never put return/throw in finally. Use it ONLY for cleanup:
  const log = [];
  function guarded() {
    try {
      return "work";
    } finally {
      log.push("lock released");   // cleanup that cannot change the outcome
    }
  }
  console.log(guarded(), "|", log.join(",")); // work | lock released
}

// ----------------------------------------------------------------------------
// SECTION 4 — Custom AppError subclass: instanceof checks + cause wrapping
// ----------------------------------------------------------------------------

class AppError extends Error {
  constructor(message, options = {}) {
    super(message);                       // must come first — initializes `this`
    this.name = "AppError";               // otherwise logs show generic "Error"
    this.code = options.code ?? "APP_ERROR";
    this.retryable = options.retryable ?? false;
    if (options.cause) this.cause = options.cause;
    Error.captureStackTrace?.(this, AppError); // Node: hide ctor from the stack
  }
}

class NotFoundError extends AppError {
  constructor(resource, id) {
    super(`${resource} ${id} not found`, { code: "NOT_FOUND" });
    this.name = "NotFoundError";
  }
}

function section4() {
  console.log("\n=== 4. CUSTOM ERRORS + cause WRAPPING ===");

  // Callers react by TYPE via the prototype chain, not by string-matching.
  try {
    throw new NotFoundError("user", 42);
  } catch (err) {
    console.log(err instanceof NotFoundError); // true
    console.log(err instanceof AppError);      // true — subclass chain works
    console.log(err instanceof Error);         // true — all the way up
    console.log(err.name, "/", err.code);      // NotFoundError / NOT_FOUND
    console.log(err.retryable);                // false
  }

  // The ES2022 `cause` option wraps low-level errors WITHOUT destroying them:
  // high-level context at the boundary, full original error for debugging.
  try {
    JSON.parse("{broken");            // throws SyntaxError
  } catch (err) {
    const wrapped = new NotFoundError("config", "app.json");
    const contextual = new AppError("Failed to load user config", {
      code: "CONFIG_LOAD",
      cause: err,                     // preserve root cause + its stack
    });
    console.log(contextual.cause instanceof SyntaxError); // true
    console.log(contextual.message);                      // Failed to load user config
    console.log(contextual.cause.message);
    // Expected property name or '}' in JSON at position 1 (line 1 column 2)
  }
}

// ----------------------------------------------------------------------------
// SECTION 5 — Async errors: try/catch around await catches rejections
// ----------------------------------------------------------------------------

async function section5() {
  console.log("\n=== 5. ASYNC ERRORS VIA try/catch AROUND await ===");

  async function load() {
    try {
      const data = await Promise.reject(new Error("network down"));
      return data;
    } catch (err) {
      console.log("caught:", err.message); // caught: network down
      return null;                         // recover with a fallback value
    }
  }
  console.log(await load()); // null

  // await turns a rejection into a throw AT THAT LINE — so the surrounding
  // try/catch sees it exactly like a synchronous throw.
  async function risky() {
    throw new RangeError("index out of bounds"); // thrown after a tick
  }
  try {
    await risky();
    console.log("never printed");
  } catch (err) {
    console.log("try/catch caught:", err.name, "-", err.message);
    // try/catch caught: RangeError - index out of bounds
  }
}

// ----------------------------------------------------------------------------
// SECTION 6 — Raw promises: .catch() is the promise-world equivalent of catch
// ----------------------------------------------------------------------------

async function section6() {
  console.log("\n=== 6. REJECTED PROMISES HANDLED WITH .catch() ===");

  // Rejection skips every success handler down to the nearest .catch...
  Promise.reject(new Error("timeout"))
    .then(() => console.log("never reached"))
    .catch((err) => console.log("caught:", err.message)); // caught: timeout

  // ...and a .catch that returns normally HEALS the chain below it.
  const healed = await Promise.reject(new Error("step failed"))
    .catch((err) => `recovered from: ${err.message}`)
    .then((msg) => msg.toUpperCase());
  console.log(healed); // RECOVERED FROM: STEP FAILED

  // Pick ONE style per call site — mixing await-with-catch and .catch means
  // whichever handles it first wins and the other never fires.
}

// ----------------------------------------------------------------------------
// Run everything, top to bottom. Exit 0 when all sections complete.
// ----------------------------------------------------------------------------

(async () => {
  section1();
  section2();
  section3();
  section4();
  await section5();
  await section6();
  console.log("\nAll sections completed.");
})();
